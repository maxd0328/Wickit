package io;

import lalr.Grammar;
import lalr.LALRParseTable;
import lalr.Production;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class SourceWriter {

    private static final String[] PREFIX_LINES = {
        "/* ---------------------------------------------------------- *",
        " * LALR Parse Table Generator for Wickit                      *",
        " * Max Derbenwick 2023                                        *",
        " *                                                            *",
        " * This is an auto-generated source file                      *",
        " * ---------------------------------------------------------- */",
        "", 
        "#include \"buildw/tokenizer.h\"",
        "#include \"buildw/parser.h\""
    };

    private static final String[] SUFFIX_LINES = {
        "",
        "/* Parse table interface */",
        "",
        "uint32_t lalraction(uint32_t __row, uint32_t __col)",
        "{ return __ACTION_TABLE[__col][__row]; }",
        "uint32_t lalrgoto(uint32_t __row, uint32_t __col)",
        "{ return __GOTO_TABLE[__col][__row]; }",
        "production_t lalrprod(uint32_t __row)",
        "{ return __PROD_TABLE[__row]; }",
        "",
        "/* ---------------------------------------------------------- *",
        " * End of auto-generated source file                          *",
        " * ---------------------------------------------------------- */"
    };
	
	private static final String INCLUSION_LINE = "#include \"%s\"";
	private static final String USING_LINE = "using namespace %s;";
	private static final String[] DEFAULT_USINGS = {"wckt", "wckt::build"};
	
    private static final String ACTION_TABLE_HEADER = "uint32_t __ACTION_TABLE[MAX_TOKEN_PLUS_ONE][%d] = {";
    private static final String GOTO_TABLE_HEADER = "uint32_t __GOTO_TABLE[%d][%d] = {";
    private static final String PROD_TABLE_HEADER = "production_t __PROD_TABLE[%d] = {";
    private static final String TABLE_FOOTER = "};";

    private static final String ACTION_TABLE_ENTRY = "\t[Token::%s] = {%s},";
    private static final String GOTO_TABLE_ENTRY = "\t[%d] = {%s},";
    private static final String PROD_TABLE_ENTRY_NO_ACTION = "\t[%d] = {.nterm = %d, .length = %d, .action = nullptr},";
	private static final String PROD_TABLE_ENTRY_ACTION = "\t[%d] = {.nterm = %d, .length = %d, .action = __psem%d__},";
	
	private static final String PSEM_ACTION_HEADER = "PSEM_ACTION(__psem%d__)";
	private static final String PSEM_ACTION_BODY = "{ %s }";
	
    private final File outputFile;

    public SourceWriter(File outputFile) {
        assert outputFile != null;
        this.outputFile = outputFile;
    }

    public File getOutputFile() {
        return outputFile;
    }

    public void write(Grammar grammar, LALRParseTable table) throws IOException {
        Map<String, Integer> nterms = new HashMap<>();
        IntStream.range(0, table.getGotoColumnCount()).forEach(i -> nterms.put(table.getGotoColumn(i), i));

        List<String> lines = new ArrayList<>();
        lines.addAll(Arrays.asList(PREFIX_LINES));
		lines.addAll(grammar.getIncludeDirectives().stream().map(incl -> String.format(INCLUSION_LINE, incl)).toList());
		lines.add("");
		lines.addAll(Arrays.stream(DEFAULT_USINGS).map(use -> String.format(USING_LINE, use)).toList());
		lines.addAll(grammar.getUsingDirectives().stream().map(use -> String.format(USING_LINE, use)).toList());
		lines.add("");

        lines.add(String.format(ACTION_TABLE_HEADER, table.getRowCount()));
        for(int i = 0 ; i < table.getActionColumnCount() ; ++i) {
            final int _i = i;
            lines.add(String.format(ACTION_TABLE_ENTRY, table.getActionColumn(i), table.getOrderedRows()
                .stream().map(row -> String.valueOf(encodeAction(row.getAction(_i)))).collect(Collectors.joining(", "))));
        }
        lines.add(TABLE_FOOTER);
        lines.add("");

        lines.add(String.format(GOTO_TABLE_HEADER, table.getGotoColumnCount(), table.getRowCount()));
        for(int i = 0 ; i < table.getGotoColumnCount() ; ++i) {
            final int _i = i;
            lines.add(String.format(GOTO_TABLE_ENTRY, _i, table.getOrderedRows().stream()
                .map(row -> String.valueOf(row.getGoto(_i))).collect(Collectors.joining(", "))));
        }
        lines.add(TABLE_FOOTER);
        lines.add("");
		
		boolean addedAction = false;
		for(int i = 0 ; i < table.getProductionCount() ; ++i) {
			Production prod = table.getProduction(i);
			if(prod.getSemanticAction() == null)
				continue;
			addedAction = true;
			lines.add(String.format(PSEM_ACTION_HEADER, i));
			lines.add(String.format(PSEM_ACTION_BODY, expandSemanticAction(prod.getSemanticAction())));
		}
		if(addedAction) lines.add("");

        lines.add(String.format(PROD_TABLE_HEADER, table.getProductionCount()));
        for(int i = 0 ; i < table.getProductionCount() ; ++i) {
            Production prod = table.getProduction(i);
			if(prod.getSemanticAction() == null)
            	lines.add(String.format(PROD_TABLE_ENTRY_NO_ACTION, i, nterms.get(prod.getNonTerminal()), prod.getSymbolCount()));
			else
				lines.add(String.format(PROD_TABLE_ENTRY_ACTION, i, nterms.get(prod.getNonTerminal()), prod.getSymbolCount(), i));
        }
        lines.add(TABLE_FOOTER);

        lines.addAll(Arrays.asList(SUFFIX_LINES));

        BufferedWriter writer = new BufferedWriter(new FileWriter(outputFile));
        for (String line : lines) {
            writer.write(line);
            writer.newLine();
        }
        writer.close();
    }

    private int encodeAction(LALRParseTable.Action action) {
        if(action == null)
            return 0;
        switch(action.getType()) {
            case LALRParseTable.Action.ACCEPT:
                return 1;
            case LALRParseTable.Action.SHIFT:
                return 2 * action.getNumber() + 2;
            case LALRParseTable.Action.REDUCE:
                return 2 * action.getNumber() + 3;
            default:
                return 0;
        }
    }
	
	private String expandSemanticAction(String semanticAction) {
		semanticAction = semanticAction.replaceAll("\\$([0-9]+)", "(__xelems__[$1])");
		semanticAction = semanticAction.replaceAll("\\$NULL", "nullptr");
		semanticAction = semanticAction.replaceAll("\\$NEW", "PMAKE_UNIQUE");
		return semanticAction;
	}

}
