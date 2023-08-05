#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "error/error.h"

namespace wckt::build
{
	class Token : public SourceSegment
	{
		public:
			/* Number represents matching priority (lower = high priority) */
			enum class_t
			{
				KEYW_CONTRACT				= 0x00,
				KEYW_TEMPLATE				= 0x01,
				KEYW_NAMESPACE				= 0x02,
				KEYW_TYPE					= 0x03,
				KEYW_AS						= 0x04,
				KEYW_EXTENDS				= 0x05,
				KEYW_FUNCTION				= 0x06,
				KEYW_SWITCH					= 0x07,
				KEYW_CONSTRUCTOR			= 0x08,
				KEYW_IMPORT					= 0x09,

				KEYW_PUBLIC					= 0x20,
				KEYW_RESTRICTED				= 0x21,
				KEYW_PRIVATE				= 0x22,
				KEYW_PARTIAL				= 0x23,
				KEYW_STATIC					= 0x24,
				KEYW_DEFAULT				= 0x25,

				KEYW_SATISFIES				= 0x40,
				KEYW_NEW					= 0x41,
				KEYW_THIS					= 0x42,
				KEYW_CONFLICT				= 0x43,
				KEYW_OPERATOR				= 0x44,

				KEYW_IF						= 0x60,
				KEYW_WHILE					= 0x61,
				KEYW_FOR					= 0x62,
				KEYW_DO						= 0x63,
				KEYW_CASE					= 0x64,
				KEYW_BREAK					= 0x65,
				KEYW_CONTINUE				= 0x66,
				KEYW_RETURN					= 0x67,
				KEYW_THROW					= 0x68,
				KEYW_TRY					= 0x69,
				KEYW_CATCH					= 0x6a,
				KEYW_FINALLY				= 0x6b,
				KEYW_VAR					= 0x6c,
				KEYW_DELEGATE				= 0x6d,

				DELIM_OPEN_PARENTHESIS		= 0x80,
				DELIM_CLOSE_PARENTHESIS		= 0x81,
				DELIM_OPEN_BRACKET			= 0x82,
				DELIM_CLOSE_BRACKET			= 0x83,
				DELIM_OPEN_BRACE			= 0x84,
				DELIM_CLOSE_BRACE			= 0x85,
				DELIM_SEMICOLON				= 0x86,
				DELIM_COLON					= 0x87,
				DELIM_DOT					= 0x88,
				DELIM_HASH					= 0x89,

				OPERATOR_ADD				= 0xa0,
				OPERATOR_SUB				= 0xa1,
				OPERATOR_MUL				= 0xa2,
				OPERATOR_DIV				= 0xa3,
				OPERATOR_MOD				= 0xa4,
				OPERATOR_AND				= 0xa5,
				OPERATOR_OR					= 0xa6,
				OPERATOR_XOR				= 0xa7,
				OPERATOR_SHL				= 0xa8,
				OPERATOR_SHR				= 0xa9,
				OPERATOR_LAZY_AND			= 0xaa,
				OPERATOR_LAZY_OR			= 0xab,
				OPERATOR_INC				= 0xac,
				OPERATOR_DEC				= 0xad,
				OPERATOR_LOGICAL_NOT		= 0xae,
				OPERATOR_BITWISE_NOT		= 0xaf,
				OPERATOR_EQUALS				= 0xb0,
				OPERATOR_NOT_EQUALS			= 0xb1,
				OPERATOR_STRICT_EQUALS		= 0xb2,
				OPERATOR_STRICT_NOT_EQUALS	= 0xb3,
				OPERATOR_GREATER			= 0xb4,
				OPERATOR_GREATER_OR_EQUAL	= 0xb5,
				OPERATOR_LESS				= 0xb6,
				OPERATOR_LESS_OR_EQUAL		= 0xb7,
				OPERATOR_OPTIONAL			= 0xb8,
				OPERATOR_ASSIGN				= 0xb9,
				OPERATOR_OTHER_ASSIGN		= 0xba,
				
				NULL_LITERAL				= 0xd0,
				BOOL_LITERAL				= 0xd1,
				INT_LITERAL					= 0xd2,
				FLOAT_LITERAL				= 0xd3,
				CHARACTER_LITERAL			= 0xd4,
				STRING_LITERAL				= 0xd5,
				IDENTIFIER					= 0xd6,
				
				COMMENT_SINGLELINE			= 0xf0,
				COMMENT_MULTILINE			= 0xf1,
				END_OF_STREAM				= 0xf2,
				__NULL__					= 0xf3 // used to indicate no such token
			};
			
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
