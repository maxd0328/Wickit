import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.Objects;
import java.util.stream.Collectors;

public class Production {
	
	private final String nonTerminal;
	private final List<Symbol> symbols;
	
	public Production(String nonTerminal, Symbol... symbols) {
		this(nonTerminal, Arrays.asList(symbols));
	}
	
	public Production(String nonTerminal, List<Symbol> symbols) {
		assert nonTerminal != null && symbols != null;
		assert nonTerminal.length() > 0;
		this.nonTerminal = nonTerminal;
		this.symbols = new ArrayList<>();
		this.symbols.addAll(symbols);
	}
	
	public String getNonTerminal() {
		return nonTerminal;
	}
	
	public List<Symbol> getSymbols() {
		return Collections.unmodifiableList(symbols);
	}
	
	public int getSymbolCount() {
		return symbols.size();
	}
	
	public Symbol getSymbol(int index) {
		assert index >= 0 && index < symbols.size();
		return symbols.get(index);
	}
	
	public String toItemString(int position) {
		StringBuilder sb = new StringBuilder();
		sb.append(nonTerminal).append(" -> ");
		for(int i = 0 ; i < symbols.size() ; ++i)
			sb.append(i > 0 ? " " : "").append(i == position ? "." : "").append(symbols.get(i).toString());
		if(position >= symbols.size())
			sb.append(".");
		return sb.toString();
	}
	
	@Override
	public String toString() {
		return nonTerminal + " -> " + symbols.stream().map(Symbol::toString).collect(Collectors.joining(" "));
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? ((Production) o).nonTerminal.equals(nonTerminal) && ((Production) o).symbols.equals(symbols) : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(nonTerminal, symbols);
	}
	
}