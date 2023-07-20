#pragma once

#include "include/definitions.h"

namespace wckt::base
{
	class URL;
	
    class URLProtocol
    {
        public:
            virtual ~URLProtocol() = default;
            virtual std::unique_ptr<std::istream> istream(const std::string& source, const URL* parent) const = 0;
			virtual std::unique_ptr<std::ostream> ostream(const std::string& source, const URL* parent) const = 0;
            virtual bool equals(const std::string& s0, const URL* p0, const std::string& s1, const URL* p1) const = 0;
			virtual std::string append(const std::string& source, const std::string& elem) const = 0;
    };
	
    class URL
    {
        public:
			static const std::shared_ptr<URLProtocol> STRING_PROTOCOL;
			static const std::shared_ptr<URLProtocol> FILE_PROTOCOL;
			
			static std::map<std::string, std::shared_ptr<URLProtocol>> knownProtocols;
			
        private:
            std::shared_ptr<URLProtocol> protocol;
            std::string source;
			const URL* parent;
        
        public:
            URL(std::shared_ptr<URLProtocol> protocol, const std::string& source, const URL* parent = nullptr);
            URL(const std::string& value, const URL* parent = nullptr);
            ~URL() = default;

            std::shared_ptr<URLProtocol> getProtocol() const;
            std::string getSource() const;
			const URL* getParent() const;

            std::unique_ptr<std::istream> toInputStream() const;
            std::string read() const;
			
			std::unique_ptr<std::ostream> toOutputStream() const;

            bool operator==(const URL& other) const;
            bool operator!=(const URL& other) const;
			
			URL operator+(const std::string& elem) const;
			URL& operator+=(const std::string& elem);
    };
}
