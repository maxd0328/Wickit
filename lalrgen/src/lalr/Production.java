package lalr;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.Objects;
import java.util.stream.Collectors;

public class Production {
	
	private final String nonTerminal;
	private final List<Symbol> symbols;
	
	private final String semanticAction;
	private final Precedence precedence;
	
	public Production(String nonTerminal, Symbol... symbols) {
		this(nonTerminal, Arrays.asList(symbols), null, Precedence.SHIFT_PRECEDENCE);
	}
	
	public Production(String nonTerminal, List<Symbol> symbols, String semanticAction, Precedence precedence) {
		assert nonTerminal != null && symbols != null;
		assert nonTerminal.length() > 0;
		this.nonTerminal = nonTerminal;
		this.symbols = new ArrayList<>();
		this.symbols.addAll(symbols);
		this.semanticAction = semanticAction;
		this.precedence = precedence;
	}
	
	public String getNonTerminal() {
		return nonTerminal;
	}
	
	public List<Symbol> getSymbols() {
		return Collections.unmodifiableList(symbols);
	}
	
	public String getSemanticAction() {
		return semanticAction;
	}
	
	public Precedence getPrecedence() {
		return precedence;
	}
	
	public int getSymbolCount() {
		return symbols.size();
	}
	
	public boolean isEmpty() {
		return symbols.isEmpty();
	}
	
	public Symbol getSymbol(int index) {
		assert index >= 0 && index < symbols.size();
		return symbols.get(index);
	}
	
	public String toItemString(int position) {
		StringBuilder sb = new StringBuilder();
		sb.append("#" + nonTerminal).append(" -> ");
		for(int i = 0 ; i < symbols.size() ; ++i)
			sb.append(i > 0 ? " " : "").append(i == position ? "." : "").append(symbols.get(i).toString());
		if(position >= symbols.size())
			sb.append(".");
		return sb.toString();
	}
	
	@Override
	public String toString() {
		return "#" + nonTerminal + " -> " + symbols.stream().map(Symbol::toString).collect(Collectors.joining(" "));
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? ((Production) o).nonTerminal.equals(nonTerminal) && ((Production) o).symbols.equals(symbols) : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(nonTerminal, symbols);
	}
	
	public static class Precedence implements Comparable<Precedence> {
		
		public static final int LOW			= 0;
		public static final int DEFAULT		= 1;
		public static final int HIGH		= 2;
		
		public static final Precedence SHIFT_PRECEDENCE = new Precedence(DEFAULT, 0);
		
		private final int order;
		private final int level;
		
		public Precedence(int order, int level) {
			assert order == DEFAULT || order == LOW || order == HIGH;
			assert order == DEFAULT ? level == 0 : level >= 0;
			this.order = order;
			this.level = level;
		}
		
		public int getOrder() {
			return order;
		}
		
		public int getLevel() {
			return level;
		}
		
		@Override
		public String toString() {
			return switch(order) {
				case LOW -> "LOW";
				case HIGH -> "HIGH";
				default -> "DEFAULT";
			} + (order == DEFAULT ? "" : " " + level);
		}
		
		@Override
		public boolean equals(Object o) {
			return o == this ? true : o.getClass() == getClass() ? ((Precedence) o).order == order && ((Precedence) o).level == level : false;
		}
		
		@Override
		public int hashCode() {
			return Objects.hash(order, level);
		}
		
		@Override
		public int compareTo(Precedence other) {
			if(order != other.order)
				return Integer.compare(order, other.order);
			else return order == DEFAULT ? 0 : -Integer.compare(level, other.level);
		}
		
	}
	
}
