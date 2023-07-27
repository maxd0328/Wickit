#include "base/url.h"
#include "include/strutil.h"
#include "include/exception.h"

using namespace wckt::base;

namespace
{
	struct StringProtocol : public URLProtocol
	{
		~StringProtocol() override = default;
		
		std::unique_ptr<std::istream> istream(const std::string& source, std::shared_ptr<URL> parent, bool textMode) const override
		{
			return std::make_unique<std::istringstream>(source);
		}

		std::unique_ptr<std::ostream> ostream(const std::string& source, std::shared_ptr<URL> parent, bool textMode) const override
		{
			throw UnsupportedOperationError("StringProtocol::ostream");
		}

		bool equals(const std::string& s0, std::shared_ptr<URL> p0, const std::string& s1, std::shared_ptr<URL> p1) const override
		{
			return s0 == s1;
		}
		
		std::size_t hash(const std::string& source, std::shared_ptr<URL> parent) const override
		{
			return 4095 ^ (std::hash<std::string>()(source) << 1);
		}
		
		std::string append(const std::string& source, const std::string& elem) const override
		{
			throw UnsupportedOperationError("StringProtocol::append");
		}
	};

	struct FileProtocol : public URLProtocol
	{
		~FileProtocol() override = default;
		
		static inline std::filesystem::path computePath(const std::string& source, std::shared_ptr<URL> parent)
		{
			std::filesystem::path sourcepath = source;
			if(parent != nullptr && parent->getProtocol() == URL::FILE_PROTOCOL)
			{
				std::filesystem::path parentSourcepath = parent->getSource();
				sourcepath = parentSourcepath / sourcepath;
			}
			return sourcepath;
		}
		
		static inline std::filesystem::path canonical(const std::filesystem::path& path, std::shared_ptr<URL> parent)
		{
			try
			{
				return std::filesystem::canonical(path);
			}
			catch(const std::filesystem::filesystem_error& e)
			{
				throw IOError(std::string("Could not locate file: ") + e.what());
			}
		}
		
		std::unique_ptr<std::istream> istream(const std::string& source, std::shared_ptr<URL> parent, bool textMode) const override
		{
			auto sourcepath = computePath(source, parent);
			auto stream = std::make_unique<std::ifstream>(sourcepath, std::ios::in | (textMode ? 0 : std::ios::binary));
			if(!stream->is_open())
				throw IOError("Could not open file: " + sourcepath.string());
			return stream;
		}

		std::unique_ptr<std::ostream> ostream(const std::string& source, std::shared_ptr<URL> parent, bool textMode) const override
		{
			auto sourcepath = computePath(source, parent);
			auto stream = std::make_unique<std::ofstream>(sourcepath, std::ios::out | (textMode ? 0 : std::ios::binary));
			if(!stream->is_open())
				throw IOError("Could not open file: " + sourcepath.string());
			return stream;
		}

		bool equals(const std::string& s0, std::shared_ptr<URL> p0, const std::string& s1, std::shared_ptr<URL> p1) const override
		{
			auto path0 = computePath(s0, p0), path1 = computePath(s1, p1);
			auto norm0 = canonical(path0, p0), norm1 = canonical(path1, p1);
			return norm0 == norm1;
		}
		
		std::size_t hash(const std::string& source, std::shared_ptr<URL> parent) const override
		{
			auto path = computePath(source, parent);
			auto norm = canonical(path, parent);
			return 65535 ^ (std::hash<std::string>()(norm.string()) << 2);
		}
		
		std::string append(const std::string& source, const std::string& elem) const override
		{
			std::filesystem::path p = source;
			return (p / elem).string();
		}
	};	
}

const std::shared_ptr<URLProtocol> URL::STRING_PROTOCOL(new StringProtocol());
const std::shared_ptr<URLProtocol> URL::FILE_PROTOCOL(new FileProtocol());

std::map<std::string, std::shared_ptr<URLProtocol>> URL::knownProtocols = {
	{"str", URL::STRING_PROTOCOL},
	{"file", URL::FILE_PROTOCOL}
};

URL::URL()
{
	this->protocol = nullptr;
	this->source = "";
	this->parent = nullptr;
}

URL::URL(std::shared_ptr<URLProtocol> protocol, const std::string& source, std::shared_ptr<URL> parent)
{
    this->protocol = protocol;
    this->source = source;
	this->parent = parent;
}

URL::URL(const std::string& value, std::shared_ptr<URL> parent)
{
    std::string protocol;
    std::string source;

    uint32_t index = value.find("://");
    if(index == std::string::npos)
    {
        protocol = value;
        source = "";
    }
    else
    {
        protocol = value.substr(0, index);
        source = value.substr(index + 3, value.length() - (index + 3));
    }
    
    trim(protocol);
    trim(source);

    std::shared_ptr<URLProtocol> protocolObj;
    for(auto const& entry : knownProtocols)
    {
        if(entry.first == protocol)
        {
            protocolObj = entry.second;
            goto assignment;
        }
    }

    throw FormatError("No such protocol: " + protocol);

    assignment:
    this->source = source;
    this->protocol = protocolObj;
	this->parent = parent;
}

bool URL::isVoid() const
{
	return this->protocol == nullptr;
}

std::shared_ptr<URLProtocol> URL::getProtocol() const
{
    return this->protocol;
}

std::string URL::getSource() const
{
    return this->source;
}

std::shared_ptr<URL> URL::getParent() const
{
	return this->parent;
}

std::string URL::toString() const
{
	return this->protocol == nullptr ? "<null>"
		: getProtocolName(this->protocol) + "://" + this->source;
}

std::unique_ptr<std::istream> URL::toInputStream(bool textMode) const
{
	if(this->protocol == nullptr)
		throw BadStateError("Cannot read a void URL");
    return this->protocol->istream(this->source, this->parent, textMode);
}

std::string URL::read(bool textMode) const
{
    std::string result;
    result.assign(std::istreambuf_iterator<char>(*this->toInputStream(textMode)), std::istreambuf_iterator<char>());
    return result;
}

std::unique_ptr<std::ostream> URL::toOutputStream(bool textMode) const
{
	if(this->protocol == nullptr)
		throw BadStateError("Cannot write to a void URL");
	return this->protocol->ostream(this->source, this->parent, textMode);
}

bool URL::operator==(const URL& other) const
{
	if(this->protocol != other.protocol)
		return false;
	if(this->protocol == nullptr)
		return true;
	return this->protocol->equals(this->source, this->parent, other.source, other.parent);
}

bool URL::operator!=(const URL& other) const
{
	return !(*this == other);
}

URL URL::operator+(const std::string& elem) const
{
	URL newURL = *this;
	newURL += elem;
	return newURL;
}

URL& URL::operator+=(const std::string& elem)
{
	if(this->protocol == nullptr)
		return *this;
	this->source = this->protocol->append(this->source, elem);
	return *this;
}

std::string URL::getProtocolName(std::shared_ptr<URLProtocol> protocol)
{
	for(const auto& entry : knownProtocols)
	{
		if(entry.second == protocol)
			return entry.first;
	}
	return "<unknown protocol>";
}

std::size_t __impl_URLHasher__::operator()(const URL& elem) const
{
	if(elem.getProtocol() == nullptr)
		return 3947; // Random hash for void URLs
	return elem.getProtocol()->hash(elem.getSource(), elem.getParent());
}
