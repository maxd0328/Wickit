import java.util.Set;
import java.util.stream.Collectors;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.HashMap;
import java.util.Objects;
import java.util.Queue;

public class State {
	
	private final int stateNumber;
	private HashSet<Item> items;
	
	private Map<Symbol, State> transitions;
	
	public State(int stateNumber, Set<Item> items) {
		assert items != null && items.size() > 0;
		this.stateNumber = stateNumber;
		this.items = new HashSet<>();
		this.items.addAll(items);
		
		this.transitions = new HashMap<>();
	}
	
	public int getStateNumber() {
		return stateNumber;
	}
	
	public Set<Item> getItems() {
		return Collections.unmodifiableSet(items);
	}
	
	public Map<Symbol, State> getTransitions() {
		return Collections.unmodifiableMap(transitions);
	}
	
	public State getTransitionState(Symbol symbol) {
		assert transitions.containsKey(symbol);
		return transitions.get(symbol);
	}
	
	public void addTransitionState(Symbol symbol, State state) {
		assert !transitions.containsKey(symbol);
		this.transitions.put(symbol, state);
	}
	
	// Merges look aheads from target state into this state
	public void mergeWith(State state) {
		assert state != null && this.equals(state);
		
		Map<Item, Item> itemMap = new HashMap<>();
		Set<Item> newSet = new HashSet<>();
		
		for(Item item : items) // Put items in a map so they're accessible by key
			itemMap.put(item, item);
		for(Item item : state.items)
			newSet.add(Item.merge(item, itemMap.get(item)));
		
		this.items.clear();
		this.items.addAll(newSet);
	}
	
	public void computeClosure(Grammar grammar) {
		Queue<Item> unprocessedItems = new LinkedList<>();
		Set<Item> processedItems = new HashSet<>();
		
		unprocessedItems.addAll(items);
		
		while(!unprocessedItems.isEmpty()) {
			Item item = unprocessedItems.poll();
			Symbol nextSymbol = item.nextSymbol();
			
			if(nextSymbol.isTerminal())
				continue;
			
			Set<Production> productions = grammar.getProductionsOf(nextSymbol.getValue());
			for(Production prod : productions) {
				Item newItem = new Item(prod, 0, null);
			}
		}
	}
	
	@Override
	public String toString() {
		return "State " + stateNumber + ":\n\t" + items.stream().map(Item::toString).collect(Collectors.joining("\n\t"));
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? ((State) o).items.equals(items) : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(items);
	}
	
}
