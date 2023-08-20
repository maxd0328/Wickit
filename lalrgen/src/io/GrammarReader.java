package io;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Objects;
import java.util.Queue;
import java.util.Set;

import lalr.Grammar;
import lalr.Production;
import lalr.Symbol;

public class GrammarReader {
	
	private final BufferedReader reader;
	
	private Set<String> inclusions;
	private Set<String> usings;
	private String startSymbol;
	private Set<Production> productions;
	
	private boolean used;
	private Queue<String> symbols;
	private int line;
	private boolean done;
	
	public GrammarReader(InputStream istream) {
		assert istream != null;
		this.reader = new BufferedReader(new InputStreamReader(istream));
		
		this.inclusions = new HashSet<>();
		this.usings = new HashSet<>();
		this.startSymbol = null;
		this.productions = new HashSet<>();
		
		this.used = false;
		this.symbols = new LinkedList<>();
		this.line = 0;
		this.done = false;
	}
	
	public Grammar read() throws IOException, GrammarFormatException {
		assert !used : "Grammar reader already used";
		this.used = true;
		
		while(!isDone()) {
			String start = nextSymbolNotNull();
			
			if(start.equals("START")) {
				String nonTerminal = nextSymbolNotNull();
				assertThat(isNonTerminal(nonTerminal), "expected non-terminal after \'START\'");
				assertThat(startSymbol == null, "start symbol already declared");
				assertNextSymbol(";");
				
				this.startSymbol = nonTerminal.substring(1);
			}
			else if(start.equals("INCLUDE")) {
				String inclusion = nextSymbolNotNull();
				assertThat(inclusion.startsWith("\""), "expected string after \'INCLUDE\'");
				assertNextSymbol(";");
				
				this.inclusions.add(inclusion.substring(1, inclusion.length() - 1));
			}
			else if(start.equals("USING")) {
				String using = nextSymbolNotNull();
				assertThat(using.startsWith("\""), "expected string after \'USING\'");
				assertNextSymbol(";");
				
				this.usings.add(using.substring(1, using.length() - 1));
			}
			else {
				assertThat(isNonTerminal(start), "expected non-terminal on left-hand side of production");
				assertNextSymbol("->");
				List<Symbol> prodSymbols = new ArrayList<>();
				String semanticAction = null;
				
				for(String prodSymbol = nextSymbolNotNull() ; !prodSymbol.equals(";") ; prodSymbol = nextSymbolNotNull()) {
					assertThat(isNonTerminal(prodSymbol) || isTerminal(prodSymbol), "expected terminal or non-terminal in production");
					if(isTerminal(prodSymbol))
						assertThat(!prodSymbol.equals("END_OF_STREAM"), "\'END_OF_STREAM\' is a disallowed terminal");
					prodSymbols.add(new Symbol(isNonTerminal(prodSymbol) ? Symbol.NON_TERMINAL : Symbol.TERMINAL, prodSymbol.substring(1)));
				}
				
				if(peekSymbol() != null && peekSymbol().startsWith("{")) {
					semanticAction = nextSymbolNotNull();
					semanticAction = semanticAction.substring(1, semanticAction.length() - 1);
				}
				
				Production production = new Production(start.substring(1), prodSymbols, semanticAction);
				assertThat(!productions.contains(production), "duplicate production for " + start);
				productions.add(production);
			}
		}
		
		assertThat(startSymbol != null, "no start symbol declared");
		return new Grammar(inclusions, usings, startSymbol, productions);
	}
	
	private boolean whitespace(char ch) {
		return "\n\r\t ".indexOf(ch) != -1;
	}
	
	private boolean alphanumeric(char ch) {
		return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
			|| (ch >= '0' && ch <= '9') || ch == '$' || ch == '_' || ch == '#';
	}
	
	private boolean symbolic(char ch) {
		return ";->%".indexOf(ch) != -1;
	}
	
	private boolean isNonTerminal(String token) {
		return token != null && token.length() >= 2 && token.charAt(0) == '#';
	}
	
	private boolean isTerminal(String token) {
		return token != null && token.length() >= 2 && token.charAt(0) == '$';
	}
	
	private void readNextLine() throws IOException, GrammarFormatException {
		String line = reader.readLine();
		this.line++;
		if(line == null) {
			done = true;
			return;
		}
		
		String curSymbol = null;
		int numBraces = 0;
		for(int i = 0 ; i < line.length() ; ++i) {
			char ch = line.charAt(i);
			if(curSymbol == null) {
				if(whitespace(ch)) continue;
				else if(alphanumeric(ch) || symbolic(ch) || ch == '{' || ch == '\"') curSymbol = String.valueOf(ch);
				else assertThat(false, "Illegal character \'" + ch + "\'");
			}
			else if(curSymbol.charAt(0) == '{') {
				curSymbol += ch;
				if(ch == '}') {
					if(numBraces > 0)
						numBraces--;
					else {
						symbols.add(curSymbol);
						curSymbol = null;
					}
					continue;
				}
				else if(ch == '{') numBraces++;
				
				if(i == line.length() - 1) {
					String newLine = reader.readLine();
					assertThat(newLine != null, "unclosed brace");
					line += newLine;
					this.line++;
				}
			}
			else if(curSymbol.charAt(0) == '\"') {
				curSymbol += ch;
				if(ch == '\"') {
					symbols.add(curSymbol);
					curSymbol = null;
					continue;
				}
			}
			else if((alphanumeric(curSymbol.charAt(0)) && alphanumeric(ch))
				|| (symbolic(curSymbol.charAt(0)) && symbolic(ch)))
				curSymbol += ch;
			else {
				if(curSymbol.equals("%"))
					return;
				symbols.add(curSymbol);
				curSymbol = null;
				i--;
			}
		}
		if(curSymbol != null && !curSymbol.equals("%"))
			symbols.add(curSymbol);
		
		if(curSymbol != null && curSymbol.charAt(0) == '\"')
			assertThat(curSymbol.charAt(curSymbol.length() - 1) == '\"', "unterminated string");
	}
	
	private boolean isDone() throws IOException, GrammarFormatException {
		while(symbols.isEmpty() && !done)
			readNextLine();
		
		return symbols.isEmpty();
	}
	
	private String nextSymbol() throws IOException, GrammarFormatException {
		if(isDone())
			return null;
		
		return symbols.poll();
	}
	
	private String peekSymbol() throws IOException, GrammarFormatException {
		if(isDone())
			return null;
		
		return symbols.peek();
	}
	
	private String nextSymbolNotNull() throws GrammarFormatException, IOException {
		String symbol = nextSymbol();
		assertThat(symbol != null, "unexpected end-of-stream");
		return symbol;
	}
	
	private void assertNextSymbol(String value) throws GrammarFormatException, IOException {
		assertThat(Objects.equals(nextSymbol(), value), "expected \'" + value + "\'");
	}
	
	private void assertThat(boolean condition, String message) throws GrammarFormatException {
		if(!condition)
			throw new GrammarFormatException("Near line " + line + ": " + message);
	}
	
	public static class GrammarFormatException extends Exception {
		
		public GrammarFormatException(String message) {
			super(message);
		}
		
	}
	
}
