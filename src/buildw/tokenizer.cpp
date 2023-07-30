#include "buildw/tokenizer.h"
#include "include/exception.h"
#include <regex>

using namespace wckt;
using namespace wckt::build;

#define __ENUM_TO_STRING_CASE(_Elem, _Null)	case _Elem: return #_Elem;
#define	__ENUM_TO_ENTRY_INIT(_Elem, _Regex)	{ _Elem, _Regex },

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
			__VA_ARGS__(__ENUM_TO_ENTRY_INIT)							\
		};

#define __CLASSES(_Macro)																					\
			_Macro(COMMENT_SINGLELINE, "//")																\
			_Macro(COMMENT_MULTILINE, "/\\*")																\
			_Macro(BOOL_LITERAL, "(true|false)")															\
			_Macro(INT_LITERAL, "((0x[0-9A-Fa-f]+)|(0b[01]+)|(0o[0-7]+)|([0-9]+))(L|U|u|s|b|us|ub)?")		\
			_Macro(FLOAT_LITERAL, "(\\d*\\.\\d+|\\d+\\.)[fd]?")												\
			_Macro(CHARACTER_LITERAL, "\'.*\'")																\
			_Macro(STRING_LITERAL, "\".*\"")																\
			_Macro(IDENTIFIER, "[A_Za-z$_][A_Za-z0-9$_]*")

__GET_CLASS_NAME(__CLASSES)
__REGEXPS(__CLASSES)

Token::Token(class_t _class, const std::string& value, size_t position)
: _class(_class), value(value), position(position)
{}

Token::class_t Token::getClass() const
{
	return this->_class;
}

std::string Token::getValue() const
{
	return this->value;
}

size_t Token::getPosition() const
{
	return this->position;
}

std::string Token::toString() const
{
	return "[" + getClassName(this->_class) + " \"" + this->value + "\"]";
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
#define _SYMBOLIC(ch)		( std::string("~!@%^&*()-=+[]{}|;:,<.>/?").find(ch) != std::string::npos )

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
			if(_IPOS >= _ISRC.length() || _IPOS == '\n')
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
	_class = Token::__END__;
	
	if(repairChar)
		token += repairChar;
	
	for(const auto& entry : Token::REGEXPS)
	{
		std::smatch match;
		std::regex regex(entry.second);
		if(std::regex_search(token, match, regex, std::regex_constants::match_continuous) && match.length() > maxMatch.length())
		{
			maxMatch = match;
			_class = entry.first;
		}
	}
	
	if(_class == Token::__END__)
		sentinel.raise(_MAKE_STD_ERR("\'" + token + "\' is not a token"));
	else
		len = (size_t) maxMatch.length() > len ? len : maxMatch.length();
}

static void nextReal(err::ErrorSentinel& parentSentinel, _IVEC_ARG)
{
	size_t len;
	char repairChar = 0;
	err::ErrorSentinel outerSentinel(&parentSentinel, err::ErrorSentinel::COLLECT, [&_IVEC, &len](err::PTR_ErrorContextLayer ptr) {
		return _MAKE_ERR(IntrasourceContextLayer, std::move(ptr), _IPOS, len, _IBINFO.sourceTable);
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
	
	if(_class != Token::__END__)
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
			_IPOS += len;
			bool ready = false;
			for(;; _IPOS++)
			{
				if(_IPOS >= _ISRC.length())
				{
					outerSentinel.raise(_MAKE_STD_ERR("Unterminated multiline comment"));
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
		
		Token token(_class, _ISRC.substr(_IPOS, len), _IPOS);
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