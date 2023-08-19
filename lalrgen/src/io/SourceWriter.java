package io;

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
        "#include \"buildw/parser.h\"",
        "",
        "using namespace wckt::build;",
        ""
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

    private static final String ACTION_TABLE_HEADER = "uint32_t __ACTION_TABLE[MAX_TOKEN_PLUS_ONE][%d] = {";
    private static final String GOTO_TABLE_HEADER = "uint32_t __GOTO_TABLE[%d][%d] = {";
    private static final String PROD_TABLE_HEADER = "production_t __PROD_TABLE[%d] = {";
    private static final String TABLE_FOOTER = "};";

    private static final String ACTION_TABLE_ENTRY = "\t[Token::%s] = {%s},";
    private static final String GOTO_TABLE_ENTRY = "\t[%d] = {%s},";
    private static final String PROD_TABLE_ENTRY = "\t[%d] = {.nterm = %d, .length = %d},";

    private final File outputFile;

    public SourceWriter(File outputFile) {
        assert outputFile != null;
        this.outputFile = outputFile;
    }

    public File getOutputFile() {
        return outputFile;
    }

    public void write(LALRParseTable table) throws IOException {
        Map<String, Integer> nterms = new HashMap<>();
        IntStream.range(0, table.getGotoColumnCount()).forEach(i -> nterms.put(table.getGotoColumn(i), i));

        List<String> lines = new ArrayList<>();
        lines.addAll(Arrays.asList(PREFIX_LINES));

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

        lines.add(String.format(PROD_TABLE_HEADER, table.getProductionCount()));
        for(int i = 0 ; i < table.getProductionCount() ; ++i) {
            Production prod = table.getProduction(i);
            lines.add(String.format(PROD_TABLE_ENTRY, i, nterms.get(prod.getNonTerminal()), prod.getSymbolCount()));
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
                return 2 * action.getNumber();
            case LALRParseTable.Action.REDUCE:
                return 2 * action.getNumber() + 1;
            default:
                return 0;
        }
    }

}
