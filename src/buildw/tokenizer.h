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
				OPEN_PARENTHESIS		= 0x00,
				CLOSE_PARENTHESIS		= 0x01,
				OPEN_BRACKET			= 0x02,
				CLOSE_BRACKET			= 0x03,
				OPEN_BRACE				= 0x04,
				CLOSE_BRACE				= 0x05,
				COMMENT_SINGLELINE		= 0x06,
				COMMENT_MULTILINE		= 0x07,
				BOOL_LITERAL			= 0x08,
				INT_LITERAL				= 0x09,
				FLOAT_LITERAL			= 0x0a,
				CHARACTER_LITERAL		= 0x0b,
				STRING_LITERAL			= 0x0c,
				IDENTIFIER				= 0x0d,
				END_OF_STREAM			= 0x0e,
				__END__					= 0x0f // = last + 1, used for iteration
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
