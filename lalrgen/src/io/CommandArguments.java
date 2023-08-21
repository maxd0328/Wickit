package io;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.Queue;

public class CommandArguments {

	private static final String[] HELP_OUTPUT = {
		"Usage:  lalrgen [options] <input-file>",
		"   or   lalrgen <input-file> [options]",
		"",
		"Options:",
		"   -h      --help        Display this information",
		"   -o      --output      Specify the output filepath",
		"   -v      --verbose     Enable verbose terminal output",
		"   -s      --show-table  Display parse table in terminal",
		"   --show-states         Display information about each state",
		"   --no-auto             Disable auto-conflict resolution",
		"   --version             Display version information"
	};

	private static final String[] VERSION_OUTPUT = {
		"Wickit LALR Parse Table Generator 1.0.0",
		"Max Derbenwick 2023"
	};

	private final Mode mode;
	private final String inputFile;
	private final String outputFile;
	private final boolean verbose;
	private final boolean noAutoResolution;
	private final boolean showTable;
	private final boolean showStates;

	public CommandArguments(String[] args) {
		Queue<String> queue = new LinkedList<>(Arrays.asList(args));

		Mode mode = Mode.EXECUTE;
		String inputFile = null;
		String outputFile = null;
		boolean verbose = false,
			noAutoResolution = false,
			showTable = false,
			showStates = false;
		
		while(!queue.isEmpty()) {
			String arg = queue.poll();
			switch(arg) {
				case "-h":
				case "--help":
					assertThat(args.length == 1, "\'" + arg + "\' is not allowed here");
					mode = Mode.HELP;
					break;
				case "--version":
					assertThat(args.length == 1, "\'" + arg + "\' is not allowed here");
					mode = Mode.VERSION;
					break;
				case "-o":
				case "--output":
					assertThat(outputFile == null, "Multiple output files specified");
					assertThat(!queue.isEmpty(), "Missing filename after \'" + arg + "\'");
					outputFile = queue.poll().trim();
					break;
				case "-v":
				case "--verbose":
					verbose = true;
					break;
				case "--no-auto":
					noAutoResolution = true;
					break;
				case "-s":
				case "--show-table":
					showTable = true;
					break;
				case "--show-states":
					showStates = true;
					break;
				default:
					assertThat(inputFile == null, "Multiple input files specified");
					inputFile = arg;
					break;
			}
		};

		if(mode == Mode.EXECUTE) {
			assertThat(inputFile != null, "No input file specified");
			assertThat(outputFile != null || showTable || showStates, "No output file specified");
		}

		this.mode = mode;
		this.inputFile = inputFile;
		this.outputFile = outputFile;
		this.verbose = verbose;
		this.noAutoResolution = noAutoResolution;
		this.showTable = showTable;
		this.showStates = showStates;
	}

	public Mode getMode() {
		return mode;
	}

	public String getInputFile() {
		return inputFile;
	}

	public String getOutputFile() {
		return outputFile;
	}

	public boolean isVerboseEnabled() {
		return verbose;
	}

	public boolean isAutoResolutionEnabled() {
		return !noAutoResolution;
	}

	public boolean isShowTableEnabled() {
		return showTable;
	}

	public boolean isShowStatesEnabled() {
		return showStates;
	}

	public void doModeOperation(Informer informer) {
		switch(mode) {
			case HELP:
				Arrays.stream(HELP_OUTPUT).forEach(informer::inform);
				System.exit(0);
			case VERSION:
				Arrays.stream(VERSION_OUTPUT).forEach(informer::inform);
				System.exit(0);
			default:
				return;
		}
	}

	private void assertThat(boolean condition, String message) {
		if(!condition)
			throw new IllegalArgumentException(message);
	}

	public static interface Informer {

		public void inform(String str);

	}
	
	public static enum Mode {
		EXECUTE,
		HELP,
		VERSION;
	}

}
