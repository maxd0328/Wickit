package lalr;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;

public class StateGenerator {
	
	public Set<State> computeStates(Grammar grammar) {
		assert grammar != null;
		
		// Create a dummy start production which matches the start symbol and end-of-stream, i.e. S' -> S $
		Production startProduction = new Production("Start!",
			new Symbol(Symbol.NON_TERMINAL, grammar.getStartSymbol()),
			new Symbol(Symbol.TERMINAL, "END_OF_STREAM"));
		
		// Create the initial item and state based on this start production
		Item initialItem = new Item(startProduction, 0);
		State initialState = new State(0, new HashSet<>(Arrays.asList(initialItem)));
		initialState.computeClosure(grammar);
		
		// Discover all transition states that arise from the inital state
		Set<State> states = discoverStates(initialState, grammar);
		// Propagate all lookaheads across all states
		propagateLookAheads(states, grammar);
		
		return states;
	}
	
	private Set<State> discoverStates(State initialState, Grammar grammar) {
		// Create a queue to hold all unprocessed states and a set (self mapping) to hold all discovered states
		Queue<State> unprocessedStates = new LinkedList<>();
		Map<State, State> states = new HashMap<>();
		
		// Insert the inital state into both
		unprocessedStates.add(initialState);
		states.put(initialState, initialState);
		
		// Define the state number as initial-state-number + 1 (usually turns out to be 1)
		int stateNumber = initialState.getStateNumber() + 1;
		
		// While we still have unprocessed states we compute the transition states for each
		while(!unprocessedStates.isEmpty()) {
			State state = unprocessedStates.poll();
			
			// Get the symbol mapping, i.e. group items in the state by the symbol they point to and iterate over the groups
			Map<Symbol, Set<Item>> symbolMap = state.getSymbolMapping();
			for(Map.Entry<Symbol, Set<Item>> entry : symbolMap.entrySet()) {
				Symbol symbol = entry.getKey();
				Set<Item> itemSet = entry.getValue();
				
				// Take every item in the group and and its transition to a new set (item with pointer moved right by one)
				Set<Item> transitionItemSet = new HashSet<>();
				for(Item item : itemSet)
					transitionItemSet.add(item.getTransitionItem());
				
				// If the transition symbol is END_OF_STREAM, we don't create a new state and leave it as null instead
				// This will eventually translate to an 'Accept' action
				if(symbol.isTerminal() && symbol.getValue().equals("END_OF_STREAM")) {
					state.addTransitionState(symbol, null);
					continue;
				}
				
				// Create a new state with this item set and compute its closure
				State newState = new State(stateNumber, transitionItemSet);
				newState.computeClosure(grammar);
				
				// If this state hasn't been discovered yet we add it and consider it unprocessed, otherwise just fetch the old one
				if(!states.containsKey(newState)) {
					unprocessedStates.add(newState);
					states.put(newState, newState);
					stateNumber++;
				}
				else newState = states.get(newState);
				
				// Make sure each state can track all of its transitions
				state.addTransitionState(symbol, newState);
			}
		}
		
		return new HashSet<>(states.keySet());
	}
	
	private void propagateLookAheads(Set<State> states, Grammar grammar) {
		// As long as any lookaheads change, we continue to propagate lookaheads, only stopping when lookaheads are stabilized
		boolean changed;
		do {
			changed = false;
			
			// Transition the lookaheads from every state to every one of its transitions
			for(State state : states) {
				for(Map.Entry<Symbol, State> entry : state.getTransitions().entrySet()) {
					Symbol symbol = entry.getKey();
					State transitionState = entry.getValue();
					
					if(transitionState == null)
						continue;
					
					// Get the items which map to the transition items
					Map<Item, Item> sourceItems = new HashMap<>();
					state.getSymbolMapping().get(symbol).forEach(sourceItem -> sourceItems.put(sourceItem.getTransitionItem(), sourceItem));
					
					// Do transition lookahead propagation from source items to their corresponding transition items
					for(Item item : transitionState.getItems())
						if(sourceItems.containsKey(item))
							changed |= item.unionLookAheads(sourceItems.get(item).getLookAheads());
				}
			}
			
			// Do closure lookahead propagation for all states after transition lookahead propagation has occurred
			for(State state : states)
				changed |= state.propagateLookAheads(grammar);
		}
		while(changed);
	}
	
	public List<State> toOrderedList(Set<State> states) {
		List<State> stateList = new ArrayList<>();
		stateList.addAll(states);
		Collections.sort(stateList, (s0, s1) -> Integer.compare(s0.getStateNumber(), s1.getStateNumber()));
		return stateList;
	}
	
}
