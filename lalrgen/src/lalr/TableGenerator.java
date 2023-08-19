package lalr;

import java.util.Set;
import java.util.TreeSet;
import java.util.stream.IntStream;
import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class TableGenerator {
	
	private static java.util.function.IntFunction<String[]>		S_ARR_GEN = e -> new String[e];
	private static java.util.function.IntFunction<Production[]>	P_ARR_GEN = e -> new Production[e];
	
	private final ConflictResolver conflictResolver;
	
	public TableGenerator(ConflictResolver conflictResolver) {
		assert conflictResolver != null;
		this.conflictResolver = conflictResolver;
	}
	
	public ConflictResolver getConflictResolver() {
		return conflictResolver;
	}
	
	public LALRParseTable generateParseTable(Grammar grammar, Set<State> states) {
		// Get all reachable symbols (terminals and non-terminals) in the grammar considering the start symbol
		Set<Symbol> allSymbols = grammar.getReachableSymbols();
		
		// Create a list of all the non-terminals, terminals, and productions given the reachable symbols
		List<String> allNonTerminals = allSymbols.stream().filter(Symbol::isNonTerminal).map(Symbol::getValue).toList();
		List<String> allTerminals = allSymbols.stream().filter(Symbol::isTerminal).map(Symbol::getValue).toList();
		List<Production> allProductions = new ArrayList<>(grammar.getProductions());
		int nonTerminalQty = allNonTerminals.size(),
			terminalQty = allTerminals.size(),
			productionQty = allProductions.size();
		
		// Create some useful mappings to speed up the process, like mapping terminals and non-terminals to column number,
		// productions to their index in the production list, and state numbers to their corresponding state objects
		Map<String, Integer> nonTerminalMap = new HashMap<>();
		Map<String, Integer> terminalMap = new HashMap<>();
		Map<Production, Integer> productionMap = new HashMap<>();
		Map<Integer, State> stateMap = new HashMap<>();
		IntStream.range(0, nonTerminalQty).forEach(i -> nonTerminalMap.put(allNonTerminals.get(i), i));
		IntStream.range(0, terminalQty).forEach(i -> terminalMap.put(allTerminals.get(i), i));
		IntStream.range(0, productionQty).forEach(i -> productionMap.put(allProductions.get(i), i));
		states.forEach(state -> stateMap.put(state.getStateNumber(), state));
		
		// Create the parse table by defining the action columns, goto columns, and production list, respectively
		LALRParseTable table = new LALRParseTable(allTerminals.toArray(S_ARR_GEN),
			allNonTerminals.toArray(S_ARR_GEN), allProductions.toArray(P_ARR_GEN));
		
		// For every state we discovered earlier, we insert a row into the parse table
		for(State state : states) {
			// Store a map of lookaheads to actions, we use this to get all actions that may contribute to a conflict
			Map<String, Set<LALRParseTable.Action>> actionMap = new HashMap<>();
			int[] gotos = new int[nonTerminalQty];
			
			// We first iterate through every transition, and use this to create the SHIFT, ACCEPT, and GOTO entries
			for(Map.Entry<Symbol, State.Transition> entry : state.getTransitions().entrySet()) {
				Symbol symbol = entry.getKey();
				State.Transition transition = entry.getValue();
				
				// If the transition symbol is non-terminal, it's a GOTO entry, getting the column number from the mapping above
				if(symbol.isNonTerminal())
					gotos[nonTerminalMap.get(symbol.getValue())] = transition.getTarget().getStateNumber();
				else {
					// Otherwise, its either a SHIFT or ACCEPT. A null transition state in our case is used to represent successfully
					// matching and end-of-stream, and hence we put ACCEPT in the action map, otherwise we put a SHIFT
					Set<LALRParseTable.Action> curActionSet = getOrCreateSet(actionMap, symbol.getValue());
					if(transition.getTarget() == null)
						curActionSet.add(new LALRParseTable.Action(LALRParseTable.Action.ACCEPT));
					else
						curActionSet.add(new LALRParseTable.Action(LALRParseTable.Action.SHIFT, transition.getTarget().getStateNumber()));
				}
			}
			
			// Then we iterate through all reduction items ('.' at the end) and add REDUCE actions to their lookaheads in the action map
			for(Item reductionItem : state.getReductionItems()) {
				for(String lookAhead : reductionItem.getLookAheads()) {
					Set<LALRParseTable.Action> curActionSet = getOrCreateSet(actionMap, lookAhead);
					// Use the mapping above to get the index of the production we are reducing to
					curActionSet.add(new LALRParseTable.Action(LALRParseTable.Action.REDUCE, productionMap.get(reductionItem.getProduction())));
				}
			}
			
			// Now we create the actual array of actions for the row where the final actions will be placed
			LALRParseTable.Action[] actions = new LALRParseTable.Action[terminalQty];
			
			// For every lookahead terminal in the action map, we insert it into the action array and resolve conflicts if necessary
			for(Map.Entry<String, Set<LALRParseTable.Action>> entry : actionMap.entrySet()) {
				String terminal = entry.getKey();
				Set<LALRParseTable.Action> actionSet = entry.getValue();
				LALRParseTable.Action chosenAction = null;
				
				// If it has no actions for some reason we leave it blank, if it has one action we insert it into the row
				// If it has more than one action we use the conflict resolution agent to choose one that we insert
				if(actionSet.isEmpty())
					continue;
				if(actionSet.size() > 1) {
					chosenAction = conflictResolver.resolve(new ConflictInformation
						(state.getStateNumber(), terminal, actionSet, stateMap, allProductions));
					assert chosenAction != null && actionSet.contains(chosenAction);
				}
				else chosenAction = actionSet.stream().findFirst().orElse(null);
				
				actions[terminalMap.get(terminal)] = chosenAction;
			}
			
			// Finally, we insert both the action and goto part of the row into the table
			table.insertRow(state.getStateNumber(), actions, gotos);
		}
		
		return table;
	}
	
	private Set<LALRParseTable.Action> getOrCreateSet(Map<String, Set<LALRParseTable.Action>> map, String key) {
		Set<LALRParseTable.Action> set = map.get(key);
		if(set == null) {
			set = new TreeSet<>();
			map.put(key, set);
		}
		return set;
	}
	
	public static interface ConflictResolver {
		
		public LALRParseTable.Action resolve(ConflictInformation info);
		
	}
	
	public static class ConflictInformation {
		
		private final int stateNumber;
		private final String lookAhead;
		private final Set<LALRParseTable.Action> conflictingActions;
		private final Map<Integer, State> stateMap;
		private final List<Production> productionList;
		
		public ConflictInformation(int stateNumber, String lookAhead, Set<LALRParseTable.Action> conflictingActions,
				Map<Integer, State> stateMap, List<Production> productionList) {
			assert lookAhead != null && conflictingActions != null && stateMap != null && productionList != null;
			assert conflictingActions.size() > 1;
			this.stateNumber = stateNumber;
			this.lookAhead = lookAhead;
			this.conflictingActions = conflictingActions;
			this.stateMap = stateMap;
			this.productionList = productionList;
		}
		
		public Set<LALRParseTable.Action> getConfictingActions() {
			return Collections.unmodifiableSet(conflictingActions);
		}
		
		public int getStateNumber() {
			return stateNumber;
		}
		
		public State getState() {
			return stateMap.get(stateNumber);
		}
		
		public State getState(int stateNumber) {
			return stateMap.get(stateNumber);
		}
		
		public String getLookAhead() {
			return lookAhead;
		}
		
		public Production getProduction(int productionNumber) {
			return productionList.get(productionNumber);
		}
		
		public String getConflictType() {
			int acceptQty = 0, shiftQty = 0, reduceQty = 0;
			for(LALRParseTable.Action action : conflictingActions) {
				switch(action.getType()) {
					case LALRParseTable.Action.ACCEPT:	acceptQty++; break;
					case LALRParseTable.Action.SHIFT:	shiftQty++; break;
					case LALRParseTable.Action.REDUCE:	reduceQty++; break;
				}
			}
			if(reduceQty > 1) {
				if(acceptQty > 0)
					return "Accept-Reduce-Reduce";
				if(shiftQty > 0)
					return "Shift-Reduce-Reduce";
				else return "Reduce-Reduce";
			}
			else if(acceptQty > 0)
				return "Accept-Reduce";
			else return "Shift-Reduce";
		}
		
	}
	
}
