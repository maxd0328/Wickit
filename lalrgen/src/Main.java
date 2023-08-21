import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;
import java.util.Set;

import io.CommandArguments;
import io.GrammarReader;
import io.SourceWriter;
import io.GrammarReader.GrammarFormatException;
import lalr.Grammar;
import lalr.LALRParseTable;
import lalr.State;
import lalr.StateGenerator;
import lalr.TableGenerator;

public class Main {
	
	private static final Scanner SCANNER = new Scanner(System.in);
	private static boolean verbose = false;
	
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
	
	private static String getReductionString(LALRParseTable.Action action, TableGenerator.ConflictInformation info) {
		return action.getType() == LALRParseTable.Action.REDUCE ? " [" + info.getProduction(action.getNumber()).toString() + "]" : "";
	}
	
	private static LALRParseTable.Action[] printVerboseDetails(TableGenerator.ConflictInformation info) {
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
		return actions;
	}
	
	private static LALRParseTable.Action resolveConflicts(TableGenerator.ConflictInformation info) {
		info(info.getConflictType() + " conflict detected!");
		info(info.getConfictingActions().size() + " ambiguous actions for lookahead \'" + info.getLookAhead() + "\'");
		if(info.isPrecedenceDisabled())
			info("NOTE: Auto-conflict resolution is disabled, so precedence is not considered");
		else info("All actions have precedence \'" + info.getPrecedence() + "\', consider writing explicit precedences");
		info("Currently in state " + info.getStateNumber());
		info("See conflict details below:");
		info("");
		
		LALRParseTable.Action[] actions = printVerboseDetails(info);
		
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
	
	private static void informConflicts(TableGenerator.ConflictInformation info) {
		info("INFO: Auto-conflict resolution has chosen the following action for state " + info.getStateNumber() + " and look-ahead " + info.getLookAhead()
			+ ": " + info.getChosenAction().getDetailString() + " (" + info.getChosenAction().toString() + ")" + getReductionString(info.getChosenAction(), info));
		if(!verbose) {
			boolean first = true;
			for(LALRParseTable.Action action : info.getConfictingActions()) {
				if(action.equals(info.getChosenAction()))
					continue;
				info("   " + (first ? "Instead of " : "Or ") + action.getDetailString() + " (" + action.toString() + ")" + getReductionString(action, info));
				first = false;
			}
		}
		else {
			info("[VERBOSE] See conflict details below:");
			info("");
			printVerboseDetails(info);
			info("---");
		}
	}
	
	public static void main(String[] args) {
		CommandArguments arguments = null;
		try { arguments = new CommandArguments(args); }
		catch(IllegalArgumentException ex) { error(ex.getMessage()); }

		arguments.doModeOperation(Main::info);
		verbose = arguments.isVerboseEnabled();

		GrammarReader reader = null;
		try { reader = new GrammarReader(new FileInputStream(new File(arguments.getInputFile()))); }
		catch (FileNotFoundException ex) { error("File not found: " + arguments.getInputFile()); }
		
		Grammar grammar = null;
		try { grammar = reader.read(); }
		catch(GrammarFormatException ex) { error("While reading grammar file:\n" + ex.getMessage()); }
		catch(IOException ex) { error("Failed to read file \'" + arguments.getInputFile() + "\', please verify file permissions"); }
		
		StateGenerator stateGenerator = new StateGenerator();
		Set<State> states = stateGenerator.computeStates(grammar);
		
		TableGenerator tableGenerator = new TableGenerator(Main::resolveConflicts, Main::informConflicts, arguments.isAutoResolutionEnabled());
		LALRParseTable table = tableGenerator.generateParseTable(grammar, states);
		
		if(arguments.isShowTableEnabled())
			info(table.toString());

		if(arguments.getOutputFile() != null) {
			SourceWriter writer = new SourceWriter(new File(arguments.getOutputFile()));
			try { writer.write(grammar, table); }
			catch(IOException ex) { error("Failed to write source file \'" + arguments.getOutputFile() + "\', please verify file permissions"); }
		}
	}
	
}
