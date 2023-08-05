#include "buildw/tokenizer.h"
#include "include/exception.h"
#include <regex>

using namespace wckt;
using namespace wckt::build;

#define __ENUM_TO_STRING_CASE(_Elem, _Null0, _Null1)	case _Elem: return #_Elem;
#define	__ENUM_TO_REGEX_INIT(_Elem, _Regex, _Null0)		{ _Elem, _Regex },
#define __ENUM_TO_NICKNAME_INIT(_Elem, _Null0, _Name)	{ _Elem, _Name },

#define __GET_CLASS_NAME(...)								\
		std::string Token::getClassName(class_t _class)		\
		{													\
			switch(_class)									\
			{												\
				__VA_ARGS__(__ENUM_TO_STRING_CASE)			\
				default: return "<unknown class>";			\
			}												\
		}

#define __REGEXPS(...)													\
		const std::map<Token::class_t, std::string> Token::REGEXPS = {	\
			__VA_ARGS__(__ENUM_TO_REGEX_INIT)							\
		};

#define __NICKNAMES(...)													\
		const std::map<Token::class_t, std::string> Token::NICKNAMES = {	\
			__VA_ARGS__(__ENUM_TO_NICKNAME_INIT)							\
		};

#define __CLASSES(_Macro)																						\
		_Macro(KEYW_CONTRACT,				"contract",									"\'contract\'")			\
		_Macro(KEYW_TEMPLATE,				"template",									"\'template\'")			\
		_Macro(KEYW_NAMESPACE,				"namespace",								"\'namespace\'")		\
		_Macro(KEYW_TYPE,					"type",										"\'type\'")				\
		_Macro(KEYW_AS,						"as",										"\'as\'")				\
		_Macro(KEYW_EXTENDS,				"extends",									"\'extends\'")			\
		_Macro(KEYW_FUNCTION,				"function",									"\'function\'")			\
		_Macro(KEYW_SWITCH,					"switch",									"\'switch\'")			\
		_Macro(KEYW_CONSTRUCTOR,			"constructor",								"\'constructor\'")		\
		_Macro(KEYW_IMPORT,					"import",									"\'import\'")			\
																												\
		_Macro(KEYW_PUBLIC,					"public",									"\'public\'")			\
		_Macro(KEYW_RESTRICTED,				"restricted",								"\'restricted\'")		\
		_Macro(KEYW_PRIVATE,				"private",									"\'private\'")			\
		_Macro(KEYW_PARTIAL,				"partial",									"\'partial\'")			\
		_Macro(KEYW_STATIC,					"static",									"\'static\'")			\
		_Macro(KEYW_DEFAULT,				"default",									"\'default\'")			\
																												\
		_Macro(KEYW_SATISFIES,				"satisfies",								"\'satisfies\'")		\
		_Macro(KEYW_NEW,					"new",										"\'new\'")				\
		_Macro(KEYW_THIS,					"this",										"\'this\'")				\
		_Macro(KEYW_CONFLICT,				"conflict",									"\'conflict\'")			\
		_Macro(KEYW_OPERATOR,				"operator",									"\'operator\'")			\
																												\
		_Macro(KEYW_IF,						"if",										"\'if\'")				\
		_Macro(KEYW_WHILE,					"while",									"\'while\'")			\
		_Macro(KEYW_FOR,					"for",										"\'for\'")				\
		_Macro(KEYW_DO,						"do",										"\'do\'")				\
		_Macro(KEYW_CASE,					"case",										"\'case\'")				\
		_Macro(KEYW_BREAK,					"break",									"\'break\'")			\
		_Macro(KEYW_CONTINUE,				"continue",									"\'continue\'")			\
		_Macro(KEYW_RETURN,					"return",									"\'return\'")			\
		_Macro(KEYW_THROW,					"throw",									"\'throw\'")			\
		_Macro(KEYW_TRY,					"try",										"\'try\'")				\
		_Macro(KEYW_CATCH,					"catch",									"\'catch\'")			\
		_Macro(KEYW_FINALLY,				"finally",									"\'finally\'")			\
		_Macro(KEYW_VAR,					"var",										"\'var\'")				\
		_Macro(KEYW_DELEGATE,				"delegate",									"\'delegate\'")			\
																												\
		_Macro(DELIM_OPEN_PARENTHESIS,		"\\(",										"\'(\'")				\
		_Macro(DELIM_CLOSE_PARENTHESIS,		"\\)",										"\')\'")				\
		_Macro(DELIM_OPEN_BRACKET,			"\\[",										"\'[\'")				\
		_Macro(DELIM_CLOSE_BRACKET,			"\\]",										"\']\'")				\
		_Macro(DELIM_OPEN_BRACE,			"\\{",										"\'{\'")				\
		_Macro(DELIM_CLOSE_BRACE,			"\\}",										"\'}\'")				\
		_Macro(DELIM_SEMICOLON,				";",										"\';\'")				\
		_Macro(DELIM_COLON,					":",										"\':\'")				\
		_Macro(DELIM_DOT,					"\\.",										"\'.\'")				\
		_Macro(DELIM_HASH,					"#",										"\'#\'")				\
																												\
		_Macro(OPERATOR_ADD,				"\\+",										"\'+\'")				\
		_Macro(OPERATOR_SUB,				"\\-",										"\'-\'")				\
		_Macro(OPERATOR_MUL,				"\\*",										"\'*\'")				\
		_Macro(OPERATOR_DIV,				"/",										"\'/\'")				\
		_Macro(OPERATOR_MOD,				"%",										"\'%\'")				\
		_Macro(OPERATOR_AND,				"&",										"\'&\'")				\
		_Macro(OPERATOR_OR,					"\\|",										"\'|\'")				\
		_Macro(OPERATOR_XOR,				"^",										"\'^\'")				\
		_Macro(OPERATOR_SHL,				"<<",										"\'<<\'")				\
		_Macro(OPERATOR_SHR,				">>",										"\'>>\'")				\
		_Macro(OPERATOR_LAZY_AND,			"&&",										"\'&&\'")				\
		_Macro(OPERATOR_LAZY_OR,			"\\|\\|",									"\'||\'")				\
		_Macro(OPERATOR_INC,				"\\+\\+",									"\'++\'")				\
		_Macro(OPERATOR_DEC,				"\\-\\-",									"\'--\'")				\
		_Macro(OPERATOR_LOGICAL_NOT,		"!",										"\'!\'")				\
		_Macro(OPERATOR_BITWISE_NOT,		"~",										"\'~\'")				\
		_Macro(OPERATOR_EQUALS,				"==",										"\'==\'")				\
		_Macro(OPERATOR_NOT_EQUALS,			"!=",										"\'!=\'")				\
		_Macro(OPERATOR_STRICT_EQUALS,		"===",										"\'===\'")				\
		_Macro(OPERATOR_STRICT_NOT_EQUALS,	"!==",										"\'!==\'")				\
		_Macro(OPERATOR_GREATER,			">",										"\'>\'")				\
		_Macro(OPERATOR_GREATER_OR_EQUAL,	">=",										"\'>=\'")				\
		_Macro(OPERATOR_LESS,				"<",										"\'<\'")				\
		_Macro(OPERATOR_LESS_OR_EQUAL,		"<=",										"\'<=\'")				\
		_Macro(OPERATOR_OPTIONAL,			"\\?",										"\'?\'")				\
		_Macro(OPERATOR_ASSIGN,				"=",										"\'=\'")				\
		_Macro(OPERATOR_OTHER_ASSIGN,		":?(\\+|\\-|\\*|/|%|&|\\||\\^|<<|>>)?=",	"assignment-operator")	\
																												\
		_Macro(NULL_LITERAL,				"(null)",									"\'null\'")				\
		_Macro(BOOL_LITERAL,				"(true|false)",								"bool-literal")			\
		_Macro(INT_LITERAL,																						\
			"((0x[0-9A-Fa-f]+)|(0b[01]+)|(0o[0-7]+)|([0-9]+))(L|U|u|s|b|us|ub)?",		"int-literal")			\
		_Macro(FLOAT_LITERAL,				"(\\d*\\.\\d+|\\d+\\.)[fd]?",				"float-literal")		\
		_Macro(CHARACTER_LITERAL,			"\'.*\'",									"char-literal")			\
		_Macro(STRING_LITERAL,				"\".*\"",									"string-literal")		\
		_Macro(IDENTIFIER,					"[A-Za-z$_][A-Za-z0-9$_]*",					"identifier")			\
																												\
		_Macro(COMMENT_SINGLELINE,			"//",										"comment")				\
		_Macro(COMMENT_MULTILINE,			"/\\*",										"comment")				\
		_Macro(END_OF_STREAM,				std::string(),								"end-of-stream")		\
		_Macro(__NULL__,					std::string(),								"<null>")

__GET_CLASS_NAME(__CLASSES)
__REGEXPS(__CLASSES)
__NICKNAMES(__CLASSES)

Token::Token(class_t _class, const std::string& value, size_t position)
: SourceSegment(position, value.length()), _class(_class), value(value)
{}

Token::class_t Token::getClass() const
{
	return this->_class;
}

std::string Token::getValue() const
{
	return this->value;
}

std::string Token::toString() const
{
	return "[" + getClassName(this->_class) + " " + this->value + "]";
}

namespace
{
	typedef struct
	{
		build_info_t& info;
		size_t pos;
		const std::string& src;
	} itr_t;
}

#define _IVEC		__itr__
#define _IVEC_ARG	itr_t& _IVEC
#define _IBINFO		_IVEC.info
#define _IPOS		_IVEC.pos
#define _ISRC		_IVEC.src

#define _ICHAR		( _ISRC[_IPOS] )

#define _WHITESPACE(ch)		( std::string("\n\r\t ").find(ch) != std::string::npos )
#define _ALPHANUMERIC(ch)	( (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '$' )
#define _SYMBOLIC(ch)		( std::string("~!@%^&*()-=+[]{}|;:,<>/?").find(ch) != std::string::npos )

static inline void jumpWhitespace(_IVEC_ARG)
{
	while(_IPOS < _ISRC.length() && _WHITESPACE(_ICHAR))
		_IPOS++;
}

static size_t nextLongest(err::ErrorSentinel& sentinel, char& repairChar, _IVEC_ARG)
{
	jumpWhitespace(_IVEC);
	if(_IPOS >= _ISRC.length())
		return 0;
	
	auto ch = _ICHAR;
	auto start = _IPOS;
	
	if(_ALPHANUMERIC(ch))
	{
		bool numeric = ch >= '0' && ch <= '9';
		for(_IPOS++ ; _IPOS < _ISRC.length() && (_ALPHANUMERIC(_ICHAR) || (numeric && _ICHAR == '.')) ; _IPOS++)
		{
			if(_ICHAR < '0' || _ICHAR > '9')
				numeric = false;
		}
	}
	else if(_SYMBOLIC(ch))
	{
		do { _IPOS++; }
		while(_IPOS < _ISRC.length() && _SYMBOLIC(_ICHAR));
	}
	else if(ch == '\'' || ch == '\"')
	{
		bool escape = false;
		for(_IPOS++ ;; _IPOS++)
		{
			if(_IPOS >= _ISRC.length() || _ICHAR == '\n')
			{
				sentinel.raise(_MAKE_STD_ERR(ch == '\'' ? "Unterminated character literal" : "Unterminated string literal"));
				repairChar = ch;
				break;
			}
			else if(_ICHAR == '\\')
				escape = !escape;
			else if(!escape && _ICHAR == ch)
			{
				_IPOS++;
				break;
			}
		}
	}
	else if(ch == '.')
	{
		_IPOS++;
		if(_IPOS < _ISRC.length() && _ICHAR >= '0' && _ICHAR <= '9')
			while(_IPOS < _ISRC.length() && _ALPHANUMERIC(_ICHAR))
				_IPOS++;
		else
			while(_IPOS < _ISRC.length() && _SYMBOLIC(_ICHAR))
				_IPOS++;
	}
	else
	{
		sentinel.raise(_MAKE_STD_ERR(std::string("Disallowed character \'") + ch + "\'"));
		_IPOS++;
	}
	
	auto end = _IPOS;
	_IPOS = start;
	return end - start;
}

static void findLongestMatch(Token::class_t& _class, size_t& len, err::ErrorSentinel& sentinel, char repairChar, _IVEC_ARG)
{
	std::string token = _ISRC.substr(_IPOS, len);
	std::smatch maxMatch;
	_class = Token::__NULL__;
	
	if(repairChar)
		token += repairChar;
	
	for(const auto& entry : Token::REGEXPS)
	{
		if(entry.second.empty())
			continue;
		
		std::smatch match;
		std::regex regex(entry.second);
		if(std::regex_search(token, match, regex, std::regex_constants::match_continuous) && (_class == Token::__NULL__ || match.length() > maxMatch.length()))
		{
			maxMatch = match;
			_class = entry.first;
		}
	}
	
	if(_class == Token::__NULL__)
		sentinel.raise(_MAKE_STD_ERR("\'" + token + "\' is not a token"));
	else
		len = (size_t) maxMatch.length() > len ? len : maxMatch.length();
}

static void nextReal(err::ErrorSentinel& parentSentinel, _IVEC_ARG)
{
	size_t len;
	char repairChar = 0;
	err::ErrorSentinel outerSentinel(&parentSentinel, err::ErrorSentinel::COLLECT, [&_IVEC, &len](err::PTR_ErrorContextLayer ptr) {
		return _MAKE_ERR(IntrasourceContextLayer, std::move(ptr), SourceSegment(_IPOS, len), _IBINFO.sourceTable);
	});
	
	{
		err::ErrorSentinel innerSentinel(&outerSentinel, err::ErrorSentinel::COLLECT, err::ErrorSentinel::NO_CONTEXT_FN);
		len = nextLongest(innerSentinel, repairChar, _IVEC);
		
		if(innerSentinel.hasErrors() && !repairChar)
		{
			_IPOS += len;
			return;
		}
	}
	
	if(!len)
		return;
	
	Token::class_t _class;
	findLongestMatch(_class, len, outerSentinel, repairChar, _IVEC);
	
	if(_class != Token::__NULL__)
	{
		if(_class == Token::COMMENT_SINGLELINE)
		{
			_IPOS += len;
			while(_IPOS < _ISRC.length() && _ICHAR != '\n')
				_IPOS++;
			return;
		}
		else if(_class == Token::COMMENT_MULTILINE)
		{
			auto start = _IPOS;
			_IPOS += len;
			bool ready = false;
			for(;; _IPOS++)
			{
				if(_IPOS >= _ISRC.length())
				{
					auto tmp = _IPOS;
					_IPOS = start;
					outerSentinel.raise(_MAKE_STD_ERR("Unterminated multiline comment"));
					_IPOS = tmp;
					return;
				}
				else if(_ICHAR == '*')
					ready = true;
				else if(ready && _ICHAR == '/')
				{
					_IPOS++;
					return;
				}
				else ready = false;
			}
		}
		
		Token token(_class, _ISRC.substr(_IPOS, len) + (repairChar ? std::string(1, repairChar) : ""), _IPOS);
		_IBINFO.tokenSequence->push_back(token);
	}
	_IPOS += len;
}

void services::tokenize(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel)
{
	assert(buildInfo.sourceTable != nullptr, "Build info must contain source table before tokenization");
	
	itr_t _IVEC = { buildInfo, 0, buildInfo.sourceTable->getSource() };
	buildInfo.tokenSequence = std::make_shared<std::vector<Token>>();
	
	err::ErrorSentinel sentinel(parentSentinel, err::ErrorSentinel::COLLECT, err::ErrorSentinel::NO_CONTEXT_FN);
	
	while(_IPOS < _ISRC.length())
		nextReal(sentinel, _IVEC);
}
