#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "error/error.h"

namespace wckt::build
{
	class Token
	{
		public:
			/* Number represents matching priority (lower = high priority) */
			enum class_t
			{
				COMMENT_SINGLELINE		= 0x00,
				COMMENT_MULTILINE		= 0x01,
				BOOL_LITERAL			= 0x02,
				INT_LITERAL				= 0x03,
				FLOAT_LITERAL			= 0x04,
				CHARACTER_LITERAL		= 0x05,
				STRING_LITERAL			= 0x06,
				IDENTIFIER				= 0x07,
				__END__					= 0x08 // = last + 1, used for iteration
			};
			
			static std::string getClassName(class_t _class);
			static const std::map<class_t, std::string> REGEXPS;
			
		private:
			class_t _class;
			std::string value;
			size_t position;
			
		public:
			Token(class_t _class, const std::string& value, size_t position);
			~Token() = default;
			
			class_t getClass() const;
			std::string getValue() const;
			size_t getPosition() const;
			
			std::string toString() const;
	};
	
	namespace services
	{
		void tokenize(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel);
	}
}
