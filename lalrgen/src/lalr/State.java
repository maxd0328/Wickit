package lalr;

import java.util.Set;
import java.util.stream.Collectors;
import java.util.Collections;
import java.util.HashSet;
import java.util.Queue;
import java.util.LinkedList;
import java.util.Map;
import java.util.HashMap;
import java.util.Objects;

public class State {
	
	private final int stateNumber;
	private final Set<Item> items;
	
	private Map<Symbol, Transition> transitions;
	private Map<Symbol, Set<Item>> symbolMapCache;
	private Set<Item> reductionCache;
	
	public State(int stateNumber, Set<Item> items) {
		assert items != null && items.size() > 0;
		this.stateNumber = stateNumber;
		this.items = new HashSet<>();
		this.items.addAll(items);
		
		this.transitions = new HashMap<>();
		this.symbolMapCache = null;
		this.reductionCache = null;
	}
	
	public int getStateNumber() {
		return stateNumber;
	}
	
	public Set<Item> getItems() {
		return Collections.unmodifiableSet(items);
	}
	
	public Map<Symbol, Transition> getTransitions() {
		return Collections.unmodifiableMap(transitions);
	}
	
	public Transition getTransitionState(Symbol symbol) {
		assert transitions.containsKey(symbol);
		return transitions.get(symbol);
	}
	
	public void addTransitionState(Symbol symbol, State state, Set<Item> transitionedItems) {
		assert !transitions.containsKey(symbol);
		this.transitions.put(symbol, new Transition(state, transitionedItems));
	}
	
	// Merges look aheads from target state into this state
	public void mergeWith(State state) {
		assert state != null && this.equals(state);
		
		Map<Item, Item> itemMap = new HashMap<>();
		
		for(Item item : items) // Put items in a map so they're accessible by key
			itemMap.put(item, item);
		for(Item item : state.items)
			itemMap.get(item).unionLookAheads(item.getLookAheads());
	}
	
	public void computeClosure(Grammar grammar) {
		// Will need to be recomputed after this, make it null to indicate as such
		symbolMapCache = null;
		reductionCache = null;
		
		// The queue of all items which haven't been processed by the discovery phase yet
		Queue<Item> unprocessedItems = new LinkedList<>();
		unprocessedItems.addAll(items);
		
		// Discovery phase, find which productions belong in the closure
		while(!unprocessedItems.isEmpty()) {
			Item item = unprocessedItems.poll();
			Symbol nextSymbol = item.nextSymbol();
			
			// If an item points to a terminal or is at the end, it's already closed
			if(nextSymbol == null || nextSymbol.isTerminal())
				continue;
			
			// Otherwise, find all productions belonging to the non-terminal being pointed to by the item
			Set<Production> productions = grammar.getProductionsOf(nextSymbol.getValue());
			for(Production prod : productions) {
				// Make an item for this production with the pointer at the first symbol
				Item newItem = new Item(prod, 0);
				// If we haven't discovered this item before, add it both to the closure and queue of unprocessed items
				if(!items.contains(newItem)) {
					unprocessedItems.add(newItem);
					items.add(newItem);
				}
			}
		}
	}
	
	public boolean propagateLookAheads(Grammar grammar) {
		// Create a mapping of non-terminals to belonging productions for the closure and add initial items
		// This is used for efficient lookup during the propagation
		Map<String, Set<Item>> itemsByNonTerminal = new HashMap<>();
		for(Item item : items) {
			if(item.getPosition() > 0) // We only propagate to new items
				continue;
			
			if(itemsByNonTerminal.containsKey(item.getProduction().getNonTerminal()))
				itemsByNonTerminal.get(item.getProduction().getNonTerminal()).add(item);
			else {
				Set<Item> nonTerminalSet = new HashSet<>();
				nonTerminalSet.add(item);
				itemsByNonTerminal.put(item.getProduction().getNonTerminal(), nonTerminalSet);
			}
		}
		
		// Propagation stage, as long as something changes - keep propagating the lookaheads
		boolean changed, anyChanged = false;
		do {
			changed = false;
			
			// Iterate through every item in the now closed state
			for(Item item : items) {
				// If it points to a terminal or is at the end, it doesn't propagate anywhere
				if(item.nextSymbol() == null || item.nextSymbol().isTerminal())
					continue;
				
				// Otherwise, determine the non-terminal and calculate look aheads that can come after
				String nonTerminal = item.nextSymbol().getValue();
				Set<String> newLookAheads = item.calculateNextLookAheads(grammar);
				
				// For every item with a production belonging to the non-terminal (using the mapping), propagate lookaheads
				for(Item derivedItem : itemsByNonTerminal.get(nonTerminal)) {
					// If the number of lookaheads was changed at all by this endeavor, raise 'changed' flag so we continue
					if(derivedItem.unionLookAheads(newLookAheads)) {
						changed = true;
						anyChanged = true;
					}
				}
			}
		}
		while(changed);
		
		// Return true if any change occurred during the propagation
		return anyChanged;
	}
	
	public Map<Symbol, Set<Item>> getSymbolMapping() {
		if(symbolMapCache == null) {
			symbolMapCache = new HashMap<>();
			for(Item item : items) {
				Symbol symbol = item.nextSymbol();
				if(symbol == null)
					continue;
				
				if(symbolMapCache.containsKey(symbol))
					symbolMapCache.get(symbol).add(item);
				else {
					Set<Item> itemSet = new HashSet<>();
					itemSet.add(item);
					symbolMapCache.put(symbol, itemSet);
				}
			}
		}
		return Collections.unmodifiableMap(symbolMapCache);
	}
	
	public Set<Item> getReductionItems() {
		if(reductionCache == null) {
			reductionCache = new HashSet<>();
			for(Item item : items)
				if(item.nextSymbol() == null)
					reductionCache.add(item);
		}
		return Collections.unmodifiableSet(reductionCache);
	}
	
	public String toString(boolean withLookaheads) {
		return "State " + stateNumber + ":\n\t" + items.stream().map(item -> item.toString(withLookaheads)).collect(Collectors.joining("\n\t"));
	}
	
	@Override
	public String toString() {
		return toString(false);
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? ((State) o).items.equals(items) : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(items);
	}
	
	public static class Transition {
		
		private final State target;
		private Set<Item> transitionedItems;
		
		public Transition(State target, Set<Item> transitionedItems) {
			assert target != null && transitionedItems != null;
			assert transitionedItems.size() > 0;
			this.target = target;
			this.transitionedItems = new HashSet<>();
			this.transitionedItems.addAll(transitionedItems);
		}
		
		public State getTarget() {
			return target;
		}
		
		public Set<Item> getTransitionedItems() {
			return Collections.unmodifiableSet(transitionedItems);
		}
		
	}
	
}
