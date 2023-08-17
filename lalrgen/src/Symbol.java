import java.util.Objects;

public class Symbol {
	
	public static final int NON_TERMINAL	= 0x0;
	public static final int TERMINAL		= 0x1;
	
	private final int type;
	private final String value;
	
	public Symbol(int type, String value) {
		assert type == NON_TERMINAL || type == TERMINAL;
		assert value != null && value.length() > 0;
		this.type = type;
		this.value = value;
	}
	
	public int getType() {
		return type;
	}
	
	public String getValue() {
		return value;
	}
	
	public boolean isTerminal() {
		return type == TERMINAL;
	}
	
	public boolean isNonTerminal() {
		return type == NON_TERMINAL;
	}
	
	@Override
	public String toString() {
		return switch(type) {
			case NON_TERMINAL -> "#" + value;
			case TERMINAL -> "$" + value;
			default -> "???" + value;
		};
	}
	
	@Override
	public boolean equals(Object o) {
		return o == this ? true : o.getClass() == getClass() ? ((Symbol) o).type == type && ((Symbol) o).value == value : false;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(type, value);
	}
	
}
