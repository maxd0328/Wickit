import java.util.Objects;
import java.util.Set;
import java.util.Collections;
import java.util.HashSet;

public class Item {
	
	private final Production production;
	private final int position;
	
	private final HashSet<String> lookAheads;
	
	public Item(Production production, int position) {
		this(production, position, new HashSet<>());
	}
	
	public Item(Production production, int position, Set<String> lookAheads) {
		assert production != null && lookAheads != null;
		assert position >= 0 && position <= production.getSymbolCount();
		assert !lookAheads.contains("");
		this.production = production;
		this.position = position;
		this.lookAheads = new HashSet<>();
		this.lookAheads.addAll(lookAheads);
	}
	
	public Production getProduction() {
		return production;
	}
	
	public int getPosition() {
		return position;
	}
	
	public Set<String> getLookAheads() {
		return Collections.unmodifiableSet(lookAheads);
	}
	
	public boolean unionLookAheads(Set<String> lookAheads) {
		return this.lookAheads.addAll(lookAheads);
	}
	
	public Symbol nextSymbol() {
		if(position >= production.getSymbolCount())
			return null;
		return production.getSymbol(position);
	}
	
	public Item getTransitionItem() {
		assert position < production.getSymbolCount();
		return new Item(production, position + 1, lookAheads);
	}
	
	public Set<String> calculateNextLookAheads(Grammar grammar) {
		Set<String> lookAheads = new HashSet<>();
		// For every symbol after the one being considered
		for(int i = position + 1 ; i < production.getSymbolCount() ; ++i) {
			// Remove empty string because we're considering a new symbol
			lookAheads.remove("");
			
			Symbol symbol = production.getSymbol(i);
			if(symbol.isTerminal()) // If it's a terminal just add it as a look ahead
				lookAheads.add(symbol.getValue());
			else lookAheads.addAll(grammar.getFirstSet(symbol.getValue())); // Otherwise compute firsts for that non-terminal
			
			// If there's no empty string, we're done and have the lookaheads
			if(!lookAheads.contains(""))
				break;
		}
		
		// If even at the end of the rule, there's still lookaheads, add all original lookaheads
		if(lookAheads.contains("")) {
			lookAheads.remove("");
			lookAheads.addAll(this.lookAheads);
		}
		
		return lookAheads;
	}
	
	@Override
	public String toString() {
		return "Item: " + production.toItemString(position);
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? ((Item) o).production.equals(production) && ((Item) o).position == position : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(production, position);
	}
	
}
