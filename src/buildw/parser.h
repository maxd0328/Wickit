#pragma once

#include "include/definitions.h"
#include "buildw/build.h"
#include "error/error.h"

namespace wckt::build
{
    typedef struct
    {
        uint32_t nterm;
        uint32_t length;
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
