#pragma once

#include "include/definitions.h"

namespace wckt::base
{
	class URL;
	
    class URLProtocol
    {
        public:
            virtual ~URLProtocol() = default;
            virtual std::unique_ptr<std::istream> istream(const std::string& source, std::shared_ptr<URL> parent, bool textMode) const = 0;
			virtual std::unique_ptr<std::ostream> ostream(const std::string& source, std::shared_ptr<URL> parent, bool textMode) const = 0;
            virtual bool equals(const std::string& s0, std::shared_ptr<URL> p0, const std::string& s1, std::shared_ptr<URL> p1) const = 0;
			virtual std::size_t hash(const std::string& source, std::shared_ptr<URL> parent) const = 0;
			virtual std::string append(const std::string& source, const std::string& elem) const = 0;
    };
	
    class URL
    {
        public:
			typedef __impl_URLHasher__ hasher_t;
		
			static const std::shared_ptr<URLProtocol> STRING_PROTOCOL;
			static const std::shared_ptr<URLProtocol> FILE_PROTOCOL;
			
			static std::map<std::string, std::shared_ptr<URLProtocol>> knownProtocols;
			
        private:
            std::shared_ptr<URLProtocol> protocol;
            std::string source;
			std::shared_ptr<URL> parent;
        
        public:
            URL(std::shared_ptr<URLProtocol> protocol, const std::string& source, std::shared_ptr<URL> parent = nullptr);
            URL(const std::string& value, std::shared_ptr<URL> parent = nullptr);
            ~URL() = default;

            std::shared_ptr<URLProtocol> getProtocol() const;
            std::string getSource() const;
			std::shared_ptr<URL> getParent() const;
			
			std::string toString() const;

            std::unique_ptr<std::istream> toInputStream(bool textMode = false) const;
            std::string read(bool textMode = false) const;
			
			std::unique_ptr<std::ostream> toOutputStream(bool textMode = false) const;

            bool operator==(const URL& other) const;
            bool operator!=(const URL& other) const;
			
			URL operator+(const std::string& elem) const;
			URL& operator+=(const std::string& elem);
			
			static std::string getProtocolName(std::shared_ptr<URLProtocol> protocol);
    };
	
	struct __impl_URLHasher__
	{
		std::size_t operator()(const URL& elem) const;
	};
}
