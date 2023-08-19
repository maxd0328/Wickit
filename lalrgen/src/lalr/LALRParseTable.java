package lalr;

import java.util.Map;
import java.util.TreeMap;
import java.util.Objects;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;

public class LALRParseTable {
	
	private static final int ACTION_COLUMN_LENGTH	= 7;
	private static final int GOTO_COLUMN_LENGTH		= 6;
	private static final int PRODUCTION_SPACING		= 5;
	
	private final String[] actionColumns;
	private final String[] gotoColumns;
	private final Production[] productions;
	
	private final Map<Integer, Row> rows;
	
	public LALRParseTable(String[] actionColumns, String[] gotoColumns, Production[] productions) {
		assert actionColumns != null && gotoColumns != null && productions != null;
		this.actionColumns = actionColumns;
		this.gotoColumns = gotoColumns;
		this.productions = productions;
		this.rows = new TreeMap<>();
	}
	
	public String[] getActionColumns() {
		return actionColumns;
	}
	
	public String[] getGotoColumns() {
		return gotoColumns;
	}
	
	public Production[] geProductions() {
		return productions;
	}
	
	public Map<Integer, Row> getRows() {
		return Collections.unmodifiableMap(rows);
	}
	
	public Collection<Row> getOrderedRows() {
		return Collections.unmodifiableCollection(rows.values());
	}

	public int getActionColumnCount() {
		return actionColumns.length;
	}
	
	public int getGotoColumnCount() {
		return gotoColumns.length;
	}
	
	public int getProductionCount() {
		return productions.length;
	}
	
	public int getColumnCount() {
		return actionColumns.length + gotoColumns.length;
	}
	
	public String getActionColumn(int index) {
		assert index >= 0 && index < actionColumns.length;
		return actionColumns[index];
	}
	
	public String getGotoColumn(int index) {
		assert index >= 0 && index < gotoColumns.length;
		return gotoColumns[index];
	}
	
	public Production getProduction(int index) {
		assert index >= 0 && index < productions.length;
		return productions[index];
	}
	
	public void insertRow(int stateNumber, Action[] actions, int[] gotos) {
		insertRow(stateNumber, new Row(actions, gotos));
	}
	
	public void insertRow(int stateNumber, Row row) {
		assert row != null;
		assert stateNumber >= 0;
		assert row.getActionCount() == actionColumns.length && row.getGotoCount() == gotoColumns.length;
		
		this.rows.put(stateNumber, row);
	}
	
	public void removeRow(int stateNumber) {
		assert rows.containsKey(stateNumber);
		this.rows.remove(stateNumber);
	}
	
	public int getRowCount() {
		return rows.size();
	}

	public Row getRow(int stateNumber) {
		assert rows.containsKey(stateNumber);
		return rows.get(stateNumber);
	}
	
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder("| State No. ");
		int[] columnLengths = new int[getColumnCount()];
		for(int i = 0 ; i < actionColumns.length ; ++i) {
			int spaceCount = Math.max(0, ACTION_COLUMN_LENGTH - 2 - actionColumns[i].length());
			sb.append("| ").append(actionColumns[i]).append(new String(new char[spaceCount + 1]).replace("\0", " "));
			columnLengths[i] = actionColumns[i].length() + spaceCount + 2;
		}
		for(int i = 0 ; i < gotoColumns.length ; ++i) {
			int spaceCount = Math.max(0, ACTION_COLUMN_LENGTH - 2 - gotoColumns[i].length());
			sb.append("| ").append(gotoColumns[i]).append(new String(new char[spaceCount + 1]).replace("\0", " "));
			columnLengths[actionColumns.length + i] = gotoColumns[i].length() + spaceCount + 2;
		}
		sb.append("|");
		for(Map.Entry<Integer, Row> entry : rows.entrySet()) {
			String stateNo = String.valueOf(entry.getKey());
			sb.append("\n| ").append(stateNo).append(new String(new char[10 - stateNo.length()])
				.replace("\0", " ")).append(entry.getValue().toString(columnLengths));
		}
		
		sb.append("\n\nProductions:");
		if(productions.length == 0)
			sb.append("\n(None)");
		else {
			for(int i = 0 ; i < productions.length ; ++i) {
				String index = String.valueOf(i);
				int numSpaces = Math.max(0, PRODUCTION_SPACING - index.length() - 1);
				sb.append("\n").append(i).append(":").append(new String(new char[numSpaces]).replace("\0", " ")).append(productions[i].toString());
			}
		}
		
		return sb.toString();
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? Arrays.equals(((LALRParseTable) o).actionColumns, actionColumns)
			&& Arrays.equals(((LALRParseTable) o).gotoColumns, gotoColumns) && ((LALRParseTable) o).rows.equals(rows) : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(actionColumns, gotoColumns, rows);
	}
	
	public static class Row {
		
		private final Action[] actions;
		private final int[] gotos;
		
		public Row(Action[] actions, int[] gotos) {
			assert actions != null && gotos != null;
			this.actions = actions;
			this.gotos = gotos;
		}
		
		public Action[] getActions() {
			return actions;
		}
		
		public int[] getGotos() {
			return gotos;
		}
		
		public int getActionCount() {
			return actions.length;
		}
		
		public int getGotoCount() {
			return gotos.length;
		}
		
		public Action getAction(int index) {
			assert index >= 0 && index < actions.length;
			return actions[index];
		}
		
		public int getGoto(int index) {
			assert index >= 0 && index < gotos.length;
			return gotos[index];
		}
		
		public String toString(int[] columnLengths) {
			StringBuilder sb = new StringBuilder();
			for(int i = 0 ; i < actions.length ; ++i) {
				String action = actions[i] == null ? new Action(Action.ERROR).toString() : actions[i].toString();
				int columnLength = Math.max(action.length() + 2, columnLengths == null || i >= columnLengths.length ? ACTION_COLUMN_LENGTH : columnLengths[i]);
				String spaces = new String(new char[columnLength - (action.length() + 2)]).replace("\0", " ");
				sb.append("| ").append(action).append(spaces).append(" ");
			}
			for(int i = 0 ; i < gotos.length ; ++i) {
				String value = gotos[i] == 0 ? "" : String.valueOf(gotos[i]);
				int columnLength = Math.max(value.length() + 2, columnLengths == null || actions.length + i
					>= columnLengths.length ? GOTO_COLUMN_LENGTH : columnLengths[actions.length + i]);
				String spaces = new String(new char[columnLength - (value.length() + 2)]).replace("\0", " ");
				sb.append("| ").append(value).append(spaces).append(" ");
			}
			sb.append("|");
			return sb.toString();
		}
		
		@Override
		public String toString() {
			return toString(null);
		}
		
		@Override
		public boolean equals(Object o) {
			return o == this ? true : o.getClass() == getClass() ? Arrays.equals(((Row) o).actions, actions) && Arrays.equals(((Row) o).gotos, gotos) : false;
		}
		
		@Override
		public int hashCode() {
			return Objects.hash(actions, gotos);
		}
		
	}
	
	public static class Action implements Comparable<Action> {
		
		public static final int SHIFT	= 0x0;
		public static final int ACCEPT	= 0x1;
		public static final int REDUCE	= 0x2;
		public static final int ERROR	= 0x3;
		
		private final int type;
		private final int number;
		
		public Action(int type, int number) {
			this.type = type;
			this.number = number;
		}
		
		public Action(int type) {
			assert type != SHIFT && type != REDUCE;
			this.type = type;
			this.number = 0;
		}
		
		public int getType() {
			return type;
		}
		
		public int getNumber() {
			return number;
		}
		
		public String getDetailString() {
			return switch(type) {
				case SHIFT -> "SHIFT to state " + number;
				case REDUCE -> "REDUCE by production " + number;
				case ACCEPT -> "ACCEPT";
				default -> "<err>";
			};
		}
		
		@Override
		public String toString() {
			return switch(type) {
				case SHIFT -> "S" + number;
				case REDUCE -> "R" + number;
				case ACCEPT -> "Accept";
				default -> "";
			};
		}
		
		@Override
		public boolean equals(Object o) {
			return o == this ? true : o.getClass() == getClass() ? ((Action) o).type == type && ((Action) o).number == number : false;
		}
		
		@Override
		public int hashCode() {
			return Objects.hash(type, number);
		}
		
		@Override
		public int compareTo(Action other) {
			if(type == other.type)
				return Integer.compare(number, other.number);
			else
				return Integer.compare(type, other.type);
		}
		
	}
	
}
