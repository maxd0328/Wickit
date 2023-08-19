#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "error/error.h"

/* ID represents matching priority (lower = high priority) */
#define __CLASSES(_MacroO, _MacroI)																										\
		_MacroO(_MacroI, END_OF_STREAM,					0x00,		std::string(),								"end-of-stream")		\
		_MacroO(_MacroI, __NULL__,						0x01,		std::string(),								"<null>")				\
		_MacroO(_MacroI, KEYW_CONTRACT,					0x02,		"contract",									"\'contract\'")			\
		_MacroO(_MacroI, KEYW_TEMPLATE,					0x03,		"template",									"\'template\'")			\
		_MacroO(_MacroI, KEYW_NAMESPACE,				0x04,		"namespace",								"\'namespace\'")		\
		_MacroO(_MacroI, KEYW_TYPE,						0x05,		"type",										"\'type\'")				\
		_MacroO(_MacroI, KEYW_AS,						0x06,		"as",										"\'as\'")				\
		_MacroO(_MacroI, KEYW_EXTENDS,					0x07,		"extends",									"\'extends\'")			\
		_MacroO(_MacroI, KEYW_FUNCTION,					0x08,		"function",									"\'function\'")			\
		_MacroO(_MacroI, KEYW_SWITCH,					0x09,		"switch",									"\'switch\'")			\
		_MacroO(_MacroI, KEYW_CONSTRUCTOR,				0x0a,		"constructor",								"\'constructor\'")		\
		_MacroO(_MacroI, KEYW_IMPORT,					0x0b,		"import",									"\'import\'")			\
																																		\
		_MacroO(_MacroI, KEYW_PUBLIC,					0x20,		"public",									"\'public\'")			\
		_MacroO(_MacroI, KEYW_RESTRICTED,				0x21,		"restricted",								"\'restricted\'")		\
		_MacroO(_MacroI, KEYW_PRIVATE,					0x22,		"private",									"\'private\'")			\
		_MacroO(_MacroI, KEYW_PARTIAL,					0x23,		"partial",									"\'partial\'")			\
		_MacroO(_MacroI, KEYW_STATIC,					0x24,		"static",									"\'static\'")			\
		_MacroO(_MacroI, KEYW_DEFAULT,					0x25,		"default",									"\'default\'")			\
																																		\
		_MacroO(_MacroI, KEYW_VOID,						0x40,		"void",										"\'void\'")				\
		_MacroO(_MacroI, KEYW_SATISFIES,				0x41,		"satisfies",								"\'satisfies\'")		\
		_MacroO(_MacroI, KEYW_NEW,						0x42,		"new",										"\'new\'")				\
		_MacroO(_MacroI, KEYW_THIS,						0x43,		"this",										"\'this\'")				\
		_MacroO(_MacroI, KEYW_CONFLICT,					0x44,		"conflict",									"\'conflict\'")			\
		_MacroO(_MacroI, KEYW_OPERATOR,					0x45,		"operator",									"\'operator\'")			\
																																		\
		_MacroO(_MacroI, KEYW_IF,						0x60,		"if",										"\'if\'")				\
		_MacroO(_MacroI, KEYW_WHILE,					0x61,		"while",									"\'while\'")			\
		_MacroO(_MacroI, KEYW_FOR,						0x62,		"for",										"\'for\'")				\
		_MacroO(_MacroI, KEYW_DO,						0x63,		"do",										"\'do\'")				\
		_MacroO(_MacroI, KEYW_CASE,						0x64,		"case",										"\'case\'")				\
		_MacroO(_MacroI, KEYW_BREAK,					0x65,		"break",									"\'break\'")			\
		_MacroO(_MacroI, KEYW_CONTINUE,					0x66,		"continue",									"\'continue\'")			\
		_MacroO(_MacroI, KEYW_RETURN,					0x67,		"return",									"\'return\'")			\
		_MacroO(_MacroI, KEYW_THROW,					0x68,		"throw",									"\'throw\'")			\
		_MacroO(_MacroI, KEYW_TRY,						0x69,		"try",										"\'try\'")				\
		_MacroO(_MacroI, KEYW_CATCH,					0x6a,		"catch",									"\'catch\'")			\
		_MacroO(_MacroI, KEYW_FINALLY,					0x6b,		"finally",									"\'finally\'")			\
		_MacroO(_MacroI, KEYW_VAR,						0x6c,		"var",										"\'var\'")				\
		_MacroO(_MacroI, KEYW_DELEGATE,					0x6d,		"delegate",									"\'delegate\'")			\
																																		\
		_MacroO(_MacroI, DELIM_OPEN_PARENTHESIS,		0x80,		"\\(",										"\'(\'")				\
		_MacroO(_MacroI, DELIM_CLOSE_PARENTHESIS,		0x81,		"\\)",										"\')\'")				\
		_MacroO(_MacroI, DELIM_OPEN_BRACKET,			0x82,		"\\[",										"\'[\'")				\
		_MacroO(_MacroI, DELIM_CLOSE_BRACKET,			0x83,		"\\]",										"\']\'")				\
		_MacroO(_MacroI, DELIM_OPEN_BRACE,				0x84,		"\\{",										"\'{\'")				\
		_MacroO(_MacroI, DELIM_CLOSE_BRACE,				0x85,		"\\}",										"\'}\'")				\
		_MacroO(_MacroI, DELIM_SEMICOLON,				0x86,		";",										"\';\'")				\
		_MacroO(_MacroI, DELIM_COLON,					0x87,		":",										"\':\'")				\
		_MacroO(_MacroI, DELIM_DOT,						0x88,		"\\.",										"\'.\'")				\
		_MacroO(_MacroI, DELIM_COMMA,					0x89,		",",										"\',\'")				\
		_MacroO(_MacroI, DELIM_HASH,					0x8a,		"#",										"\'#\'")				\
		_MacroO(_MacroI, DELIM_BACKSLASH,				0x8b,		"\\\\",										"\'\\\'")				\
																																		\
		_MacroO(_MacroI, OPERATOR_ADD,					0xa0,		"\\+",										"\'+\'")				\
		_MacroO(_MacroI, OPERATOR_SUB,					0xa1,		"\\-",										"\'-\'")				\
		_MacroO(_MacroI, OPERATOR_MUL,					0xa2,		"\\*",										"\'*\'")				\
		_MacroO(_MacroI, OPERATOR_DIV,					0xa3,		"/",										"\'/\'")				\
		_MacroO(_MacroI, OPERATOR_MOD,					0xa4,		"%",										"\'%\'")				\
		_MacroO(_MacroI, OPERATOR_AND,					0xa5,		"&",										"\'&\'")				\
		_MacroO(_MacroI, OPERATOR_OR,					0xa6,		"\\|",										"\'|\'")				\
		_MacroO(_MacroI, OPERATOR_XOR,					0xa7,		"\\^",										"\'^\'")				\
		_MacroO(_MacroI, OPERATOR_SHL,					0xa8,		"<<",										"\'<<\'")				\
		_MacroO(_MacroI, OPERATOR_SHR,					0xa9,		">>",										"\'>>\'")				\
		_MacroO(_MacroI, OPERATOR_LAZY_AND,				0xaa,		"&&",										"\'&&\'")				\
		_MacroO(_MacroI, OPERATOR_LAZY_OR,				0xab,		"\\|\\|",									"\'||\'")				\
		_MacroO(_MacroI, OPERATOR_INC,					0xac,		"\\+\\+",									"\'++\'")				\
		_MacroO(_MacroI, OPERATOR_DEC,					0xad,		"\\-\\-",									"\'--\'")				\
		_MacroO(_MacroI, OPERATOR_LOGICAL_NOT,			0xae,		"!",										"\'!\'")				\
		_MacroO(_MacroI, OPERATOR_BITWISE_NOT,			0xaf,		"~",										"\'~\'")				\
		_MacroO(_MacroI, OPERATOR_EQUALS,				0xb0,		"==",										"\'==\'")				\
		_MacroO(_MacroI, OPERATOR_NOT_EQUALS,			0xb1,		"!=",										"\'!=\'")				\
		_MacroO(_MacroI, OPERATOR_STRICT_EQUALS,		0xb2,		"===",										"\'===\'")				\
		_MacroO(_MacroI, OPERATOR_STRICT_NOT_EQUALS,	0xb3,		"!==",										"\'!==\'")				\
		_MacroO(_MacroI, OPERATOR_GREATER,				0xb4,		">",										"\'>\'")				\
		_MacroO(_MacroI, OPERATOR_GREATER_OR_EQUAL,		0xb5,		">=",										"\'>=\'")				\
		_MacroO(_MacroI, OPERATOR_LESS,					0xb6,		"<",										"\'<\'")				\
		_MacroO(_MacroI, OPERATOR_LESS_OR_EQUAL,		0xb7,		"<=",										"\'<=\'")				\
		_MacroO(_MacroI, OPERATOR_OPTIONAL,				0xb8,		"\\?",										"\'?\'")				\
		_MacroO(_MacroI, OPERATOR_ARROW,				0xb9,		"\\->",										"\'->\'")				\
		_MacroO(_MacroI, OPERATOR_ASSIGN,				0xba,		"=",										"\'=\'")				\
		_MacroO(_MacroI, OPERATOR_OTHER_ASSIGN,			0xbb,		":?(\\+|\\-|\\*|/|%|&|\\||\\^|<<|>>)?=",	"assignment-operator")	\
																																		\
		_MacroO(_MacroI, NO_NAME,						0xd0,		"\\-\\-\\-",								"\'---\'")				\
		_MacroO(_MacroI, NULL_LITERAL,					0xd1,		"(null)",									"\'null\'")				\
		_MacroO(_MacroI, BOOL_LITERAL,					0xd2,		"(true|false)",								"bool-literal")			\
		_MacroO(_MacroI, INT_LITERAL,					0xd3,																			\
			"((0x[0-9A-Fa-f]+)|(0b[01]+)|(0o[0-7]+)|([0-9]+))(L|U|u|s|b|us|ub)?",		"int-literal")									\
		_MacroO(_MacroI, FLOAT_LITERAL,					0xd4,		"(\\d*\\.\\d+|\\d+\\.)[fd]?",				"float-literal")		\
		_MacroO(_MacroI, CHARACTER_LITERAL,				0xd5,		"\'.*\'",									"char-literal")			\
		_MacroO(_MacroI, STRING_LITERAL,				0xd6,		"\".*\"",									"string-literal")		\
		_MacroO(_MacroI, IDENTIFIER,					0xd7,		"[A-Za-z$_][A-Za-z0-9$_]*",					"identifier")			\
																																		\
		_MacroO(_MacroI, COMMENT_SINGLELINE,			0xf0,		"//",										"comment")				\
		_MacroO(_MacroI, COMMENT_MULTILINE,				0xf1,		"/\\*",										"comment")

#define MAX_TOKEN_PLUS_ONE	0xf2

#define __FUNC_TOKEN_CLASS(_Fn, _Cl, _Id, _Rgx, _Nn)			_Fn(_Cl)
#define __FUNC_TOKEN_CLASS_ID(_Fn, _Cl, _Id, _Rgx, _Nn)			_Fn(_Cl, _Id)
#define __FUNC_TOKEN_CLASS_REGEXP(_Fn, _Cl, _Id, _Rgx, _Nn)		_Fn(_Cl, _Rgx)
#define __FUNC_TOKEN_CLASS_NICKNAME(_Fn, _Cl, _Id, _Rgx, _Nn)	_Fn(_Cl, _Nn)
#define __FUNC_TOKEN_CLASS_ALL(_Fn, _Cl, _Id, _Rgx, _Nn)		_Fn(_Cl, _Id, _Rgx, _Nn)

#define FOREACH_TOKEN_CLASS(__Func)								__CLASSES(__FUNC_TOKEN_CLASS, __Func)
#define FOREACH_TOKEN_CLASS_ID(__Func)							__CLASSES(__FUNC_TOKEN_CLASS_ID, __Func)
#define FOREACH_TOKEN_CLASS_REGEXP(__Func)						__CLASSES(__FUNC_TOKEN_CLASS_REGEXP, __Func)
#define FOREACH_TOKEN_CLASS_NICKNAME(__Func)					__CLASSES(__FUNC_TOKEN_CLASS_NICKNAME, __Func)
#define FOREACH_TOKEN_CLASS_ALL(__Func)							__CLASSES(__FUNC_TOKEN_CLASS_ALL, __Func)

#define __DECLARE_TOK_ENUM_VALUE(_Cl, _Id)						_Cl = _Id,

namespace wckt::build
{
	class Token : public SourceSegment
	{
		public:
			enum class_t
			{ FOREACH_TOKEN_CLASS_ID(__DECLARE_TOK_ENUM_VALUE) };
			
			static std::string getClassName(class_t _class);
			
			static const std::map<class_t, std::string> REGEXPS;
			static const std::map<class_t, std::string> NICKNAMES;
			
		private:
			class_t _class;
			std::string value;
			
		public:
			Token(class_t _class, const std::string& value, size_t position);
			~Token() = default;
			
			class_t getClass() const;
			std::string getValue() const;
			
			std::string toString() const;
	};
	
	namespace services
	{
		void tokenize(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel);
	}
}
