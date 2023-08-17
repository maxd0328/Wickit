import java.util.Set;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;
import java.util.Map;
import java.util.HashMap;

public class Grammar {
	
	private final String startSymbol;
	private final HashSet<Production> productions;
	
	// Contains the cached FIRST sets for each production
	private final Map<Production, Set<String>> productionCache;
	
	public Grammar(String startSymbol, Set<Production> productions) {
		assert startSymbol != null && productions != null && productions.size() > 0;
		assert productions.stream().filter(prod -> prod.getNonTerminal().equals(startSymbol)).findAny().isPresent();
		this.startSymbol = startSymbol;
		this.productions = new HashSet<>();
		this.productions.addAll(productions);
		
		this.productionCache = new HashMap<>();
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
	
	public Set<String> getFirstSet(String nonTerminal) {
		// New FIRST set discovered in this pass
		Set<String> firstSet = new HashSet<>();
		
		// For each production belonging to the non-terminal
		Set<Production> prods = getProductionsOf(nonTerminal);
		for(Production prod : prods) {
			// If we've already visited this production, we add its known FIRST step and skip over it
			if(productionCache.containsKey(prod)) {
				firstSet.addAll(productionCache.get(prod));
				continue;
			}
			
			// Otherwise, add an entry for this production in the cache
			Set<String> productionFirstSet = new HashSet<>();
			productionCache.put(prod, productionFirstSet);
			
			// If the production is empty, add the empty string as a derivation
			if(prod.isEmpty())
				productionFirstSet.add("");
			else {
				// Otherwise, we iterate through the symbols of the production finding the FIRST set for each until
				// either we reach the end or it no longer contains the empty string
				Set<String> partialFirstSet = new HashSet<>();
				int index = 0;
				do {
					// Remove the empty string derivation if it exists
					partialFirstSet.remove("");
					
					// If the symbol is a terminal, we simply add that terminal value to the new FIRST set and
					// the loop will be broken. Otherwise, we recursively calculate the FIRST set of the non-terminal
					// and if that set includes the empty string the loop can continue
					if(prod.getSymbol(index).isTerminal())
						partialFirstSet.add(prod.getSymbol(index).getValue());
					else partialFirstSet.addAll(getFirstSet(prod.getSymbol(index).getValue()));
					index++;
				}
				while(partialFirstSet.contains("") && index < prod.getSymbolCount());
				
				// After the loop, we add all these new FIRST terminals to the main set. Note that 'subTerminals' will
				// only contain the empty string if the loop was broken by reaching the end, which means the parent call
				// will have to calculate the FIRST set of the subsequent symbol, as done above
				productionFirstSet.addAll(partialFirstSet);
			}
			
			// Add the production FIRST set to the aggregate FIRST set for this non-terminal
			firstSet.addAll(productionFirstSet);
		}
		
		// Return newly discovered elements in the FIRST set
		return firstSet;
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
