#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "error/error.h"

namespace wckt::build
{
	class ParseObject
	{
		public:
			virtual ~ParseObject() = default;
	};
	
	class ParseOutput : public ParseObject
	{
		private:
			std::unique_ptr<ParseObject> object;
			
		public:
			ParseOutput(std::unique_ptr<ParseObject> object);
			~ParseOutput() override = default;
			
			const ParseObject& getObject() const;
			ParseObject& getObject();
			
			std::unique_ptr<ParseObject>&& releaseObject();
	};
	
	typedef std::unique_ptr<ParseObject>(*psem_action_t)(std::vector<std::unique_ptr<ParseObject>>&&);
	
	#define PMAKE_UNIQUE(_Class, _Args...) std::make_unique<_Class>(_Args)
	#define PSEM_ACTION(__Name) \
		std::unique_ptr<ParseObject> __Name(std::vector<std::unique_ptr<ParseObject>>&& __xelems__)
	
    typedef struct
    {
        uint32_t nterm;
        uint32_t length;
		psem_action_t action;
    } production_t;

    class TokenIterator
    {
        private:
            std::shared_ptr<std::vector<Token>> tokenSequence;
            size_t position;

        public:
            TokenIterator(std::shared_ptr<std::vector<Token>> tokenSequence, size_t position = 0);
            ~TokenIterator() = default;

            std::shared_ptr<std::vector<Token>> getTokenSequence() const;
            size_t getPosition() const;

            Token next();
            Token lookAhead() const;
            Token latest() const;
    };

    namespace services
    {
        void parse(build_info_t& buildInfo, err::ErrorSentinel* parentSentinel);
    }
}
