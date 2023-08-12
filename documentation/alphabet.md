
# Wickit Alphabet

The following gives the alphabet of Wickit, and how it's processed. This is used during tokenization of the language, and note that anything which doesn't match the language's alphabet will result in a compile time error.

## 1. Tokenization

Wickit uses the longest match rule to match character streams to token classes. However, the longest match is delimited using the following rules, based on the first character in the token.

```
ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789$_ -> !@#$%^&*()[]{}:;"'`,<>/?~-=+\| whitespace

!@#$%^&*()[]{}:;`,<>/?~-=+\| -> ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789$_ whitespace

. -> whitespace

" -> " newline

' -> ' newline
```

Note that whitespace outside of strings is never included in tokens, and always used as a delimiter. For our purposes, spaces, tabs, and newlines are considered whitespace.

In general, symbols delimit alphanumeric characters, and alphanumeric characters delimit symbols, with the one exception of `"."`. This character is used in both alphanumeric and symbolic symbols, and hence delimits neither.

Additionally, string characters `"` and `'` have special delimitation rules. The longest match considers to match all subsequent characters until a newline or non-escaped `"` or `'` is reached, respectively. That is, a string using `"`, must match `"` or `\\"` but not `\"`. Note that a string that is terminated by a newline is considered an "unterminated string" by the compiler and an error is thrown.

## 2. Alphabet

Starting with the longest match, the tokenizer attempts to match the character sequence to a token class subtracting one character from the end until successful. If no token class can be matched, the longest match is considered an invalid token. If multiple token classes match, the first token class in declaration order is used.

The following gives all token classes and the regexp they match:

```
KEYW_CONTRACT: contract
KEYW_TEMPLATE: template
KEYW_NAMESPACE: namespace
KEYW_TYPE: type
KEYW_AS: as
KEYW_EXTENDS: extends
KEYW_FUNCTION: function
KEYW_SWITCH: switch
KEYW_CONSTRUCTOR: constructor
KEYW_IMPORT: import

KEYW_PUBLIC: public
KEYW_RESTRICTED: restricted
KEYW_PRIVATE: private
KEYW_PARTIAL: partial
KEYW_STATIC: static
KEYW_DEFAULT: default

KEYW_VOID: void
KEYW_SATISFIES: satisfies
KEYW_NEW: new
KEYW_THIS: this
KEYW_CONFLICT: conflict
KEYW_OPERATOR: operator

KEYW_IF: if
KEYW_WHILE: while
KEYW_FOR: for
KEYW_DO: do
KEYW_CASE: case
KEYW_BREAK: break
KEYW_CONTINUE: continue
KEYW_RETURN: return
KEYW_THROW: throw
KEYW_TRY: try
KEYW_CATCH: catch
KEYW_FINALLY: finally
KEYW_VAR: var
KEYW_DELEGATE: delegate

DELIM_OPEN_PARENTHESIS: \(
DELIM_CLOSE_PARENTHESIS: \)
DELIM_OPEN_BRACKET: \[
DELIM_CLOSE_BRACKET: \]
DELIM_OPEN_BRACE: \{
DELIM_CLOSE_BRACE: \}
DELIM_SEMICOLON: ;
DELIM_COLON: :
DELIM_DOT: \.
DELIM_COMMA: ,
DELIM_HASH: #

OPERATOR_ADD: \+
OPERATOR_SUB: \-
OPERATOR_MUL: \*
OPERATOR_DIV: /
OPERATOR_MOD: %
OPERATOR_AND: &
OPERATOR_OR: \|
OPERATOR_XOR: ^
OPERATOR_SHL: <<
OPERATOR_SHR: >>
OPERATOR_LAZY_AND: &&
OPERATOR_LAZY_OR: \|\|
OPERATOR_INC: \+\+
OPERATOR_DEC: \-\-
OPERATOR_LOGICAL_NOT: !
OPERATOR_BITWISE_NOT: ~
OPERATOR_EQUALS: ==
OPERATOR_NOT_EQUALS: !=
OPERATOR_STRICT_EQUALS: ===
OPERATOR_STRICT_NOT_EQUALS: !==
OPERATOR_GREATER: >
OPERATOR_GREATER_OR_EQUAL: >=
OPERATOR_LESS: <
OPERATOR_LESS_OR_EQUAL: <=
OPERATOR_OPTIONAL: \?
OPERATOR_ARROW: \->
OPERATOR_ASSIGN: =
OPERATOR_OTHER_ASSIGN:
    :?(\+|\-|\*|/|%|&|\||\^|<<|>>)?=

NULL_LITERAL:
	(null)

BOOL_LITERAL:
    (true|false)

INT_LITERAL:
    ((0x[0-9A-Fa-f]+)|(0b[01]+)|(0o[0-7]+)|([0-9]+))(L|U|u|s|b|us|ub)?

FLOAT_LITERAL:
    (\d*\.\d+|\d+\.)[fd]?

CHARACTER_LITERAL:
    '.*'

STRING_LITERAL:
    ".*"

IDENTIFIER:
    [A-Za-z$_][A-Za-z0-9$_]*
```
