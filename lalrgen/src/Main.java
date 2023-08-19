import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;
import java.util.Set;

import io.GrammarReader;
import io.SourceWriter;
import io.GrammarReader.GrammarFormatException;
import lalr.Grammar;
import lalr.LALRParseTable;
import lalr.State;
import lalr.StateGenerator;
import lalr.TableGenerator;

public class Main {
	
	private static Scanner SCANNER = new Scanner(System.in);
	
	private static void info(String str) {
		System.out.println(str);
	}
	
	private static void error(String str) {
		System.err.println("Error! " + str);
		System.exit(1);
	}
	
	private static String prompt(String str) {
		System.out.print(str);
		return SCANNER.nextLine().trim();
	}
	
	private static LALRParseTable.Action resolveConflicts(TableGenerator.ConflictInformation info) {
		info(info.getConflictType() + " conflict detected!");
		info(info.getConfictingActions().size() + " action(s) for lookahead \'" + info.getLookAhead() + "\'");
		info("Currently in state " + info.getStateNumber());
		info("See conflict details below:");
		info("");
		
		info("[Current State] " + info.getState().toString());
		info("");
		
		LALRParseTable.Action[] actions = new LALRParseTable.Action[info.getConfictingActions().size()];
		
		int index = 0;
		for(LALRParseTable.Action action : info.getConfictingActions()) {
			info("Action " + index + ": " + action.getDetailString() + " (" + action.toString() + ")");
			if(action.getType() == LALRParseTable.Action.SHIFT)
				info("[Transition State] " + info.getState(action.getNumber()));
			else if(action.getType() == LALRParseTable.Action.REDUCE)
				info("Production " + action.getNumber() + ": " + info.getProduction(action.getNumber()).toString());
			info("");
			actions[index++] = action;
		}
		
		info("Action Summary:");
		for(int i = 0 ; i < actions.length ; ++i) {
			String num = String.valueOf(i);
			info(num + ":" + new String(new char[5 - num.length()]).replace("\0", " ") + actions[i].getDetailString() + " (" + actions[i].toString() + ")");
		}
		
		String input;
		for(;;) {
			input = prompt("Please select an action or enter 'a' to abort (default=0): ");
			if(input.isEmpty() || input.equals("a") || (input.matches("[0-9]+") && Integer.parseInt(input) < actions.length))
				break;
			info("Invalid input.");
		}
		if(input.isEmpty())
			input = "0";
		else if(input.equals("a")) {
			info("Abort.");
			System.exit(-1);
		}
		
		return actions[Integer.parseInt(input)];
	}
	
	public static void main(String[] args) {
		String inFilename = "grammar.txt";
		String outFilename = "src/lalr.cpp";
		
		GrammarReader reader = null;
		try { reader = new GrammarReader(new FileInputStream(new File(inFilename))); }
		catch (FileNotFoundException e) { error("File not found: " + inFilename); }
		
		Grammar grammar = null;
		try { grammar = reader.read(); }
		catch(GrammarFormatException ex) { error("While reading grammar file:\n" + ex.getMessage()); }
		catch(IOException ex) { error("Failed to read file \'" + inFilename + "\', please verify file permissions"); }
		
		StateGenerator stateGenerator = new StateGenerator();
		Set<State> states = stateGenerator.computeStates(grammar);
		
		TableGenerator tableGenerator = new TableGenerator(Main::resolveConflicts);
		LALRParseTable table = tableGenerator.generateParseTable(grammar, states);
		
		SourceWriter writer = new SourceWriter(new File(outFilename));
		try { writer.write(table); }
		catch(IOException ex) { error("Failed to write source file \'" + outFilename + "\', please verify file permissions"); }
		
		info(table.toString());
	}
	
}
