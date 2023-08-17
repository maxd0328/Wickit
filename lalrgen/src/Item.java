import java.util.Objects;
import java.util.Set;
import java.util.Collections;
import java.util.HashSet;

public class Item {
	
	private final Production production;
	private final int position;
	
	private final HashSet<String> lookAheads;
	
	public Item(Production production, int position, Set<String> lookAheads) {
		assert production != null && lookAheads != null;
		assert position >= 0 && position <= production.getSymbolCount();
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
	
	public Symbol nextSymbol() {
		if(position >= production.getSymbolCount())
			return null;
		return production.getSymbol(position);
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
	
	public static Item merge(Item i0, Item i1) {
		assert i0 != null && i1 != null;
		assert i0.equals(i1);
		
		Set<String> lookAheads = new HashSet<>();
		lookAheads.addAll(i0.lookAheads);
		lookAheads.addAll(i1.lookAheads);
		return new Item(i0.production, i0.position, lookAheads);
	}
	
}
