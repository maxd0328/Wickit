#pragma once

#include "include/definitions.h"

namespace wckt::base
{
    class URLProtocol
    {
        public:
            virtual ~URLProtocol() = default;
            virtual std::unique_ptr<std::istream> stream(const std::string& source) const = 0;
    };

    class StringProtocol : URLProtocol
    {
        public:
            ~StringProtocol() override = default;
            std::unique_ptr<std::istream> stream(const std::string& source) const override;
    };

    class FileProtocol : URLProtocol
    {
        public:
            ~FileProtocol() override = default;
            std::unique_ptr<std::istream> stream(const std::string& source) const override;
    };

    class URL
    {
        public:
            static std::map<std::string, std::shared_ptr<URLProtocol>> knownProtocols;

        private:
            std::shared_ptr<URLProtocol> protocol;
            std::string source;
        
        public:
            URL(std::shared_ptr<URLProtocol> protocol, const std::string& source);
            URL(const std::string& value);
            ~URL() = default;

            std::shared_ptr<URLProtocol> getProtocol() const;
            std::string getSource() const;

            std::unique_ptr<std::istream> toStream() const;
            std::string read() const;
    };
}
