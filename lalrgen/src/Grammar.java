import java.util.Set;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;

public class Grammar {
	
	private final String startSymbol;
	private final HashSet<Production> productions;
	
	public Grammar(String startSymbol, Set<Production> productions) {
		assert startSymbol != null && productions != null && productions.size() > 0;
		assert productions.stream().filter(prod -> prod.getNonTerminal().equals(startSymbol)).findAny().isPresent();
		this.startSymbol = startSymbol;
		this.productions = new HashSet<>();
		this.productions.addAll(productions);
	}
	
	public String getStartSymbol() {
		return startSymbol;
	}
	
	public Set<Production> getProductions() {
		return Collections.unmodifiableSet(productions);
	}
	
	public int getProductionCount() {
		return productions.size();
	}
	
	public Set<Production> getProductionsOf(String nonTerminal) {
		Set<Production> prods = new HashSet<>();
		for(Production prod : productions)
			if(prod.getNonTerminal().equals(nonTerminal))
				prods.add(prod);
		return prods;
	}
	
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder("Grammar [" + startSymbol + "]:");
		for(Production prod : productions)
			sb.append("\n\t").append(prod.toString());
		return sb.toString();
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? ((Grammar) o).startSymbol
			.equals(startSymbol) && ((Grammar) o).productions.equals(productions) : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(startSymbol, productions);
	}
	
}
