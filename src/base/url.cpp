#include "base/url.h"
#include "include/strutil.h"
#include "include/exception.h"

using namespace wckt::base;

namespace
{
	struct StringProtocol : URLProtocol
	{
		~StringProtocol() override = default;
		
		std::unique_ptr<std::istream> istream(const std::string& source, const URL* parent) const override
		{
			return std::make_unique<std::istringstream>(source);
		}

		std::unique_ptr<std::ostream> ostream(const std::string& source, const URL* parent) const override
		{
			throw UnsupportedOperationError("StringProtocol::ostream");
		}

		bool equals(const std::string& s0, const URL* p0, const std::string& s1, const URL* p1) const override
		{
			return s0 == s1;
		}
	};

	struct FileProtocol : URLProtocol
	{
		~FileProtocol() override = default;
		
		static inline std::filesystem::path computePath(const std::string& source, const URL* parent)
		{
			std::filesystem::path sourcepath = source;
			if(parent != nullptr && parent->getProtocol() == URL::FILE_PROTOCOL)
			{
				std::filesystem::path parentSourcepath = parent->getSource();
				sourcepath = parentSourcepath / sourcepath;
			}
			return sourcepath;
		}
		
		std::unique_ptr<std::istream> istream(const std::string& source, const URL* parent) const override
		{
			auto sourcepath = computePath(source, parent);
			auto stream = std::make_unique<std::ifstream>(sourcepath);
			if(!stream->is_open())
				throw std::runtime_error("Failed to open file: " + sourcepath.string());
			return stream;
		}

		std::unique_ptr<std::ostream> ostream(const std::string& source, const URL* parent) const override
		{
			auto sourcepath = computePath(source, parent);
			auto stream = std::make_unique<std::ofstream>(sourcepath, std::ios_base::binary);
			if(!stream->is_open())
				throw std::runtime_error("Failed to open file: " + sourcepath.string());
			return stream;
		}

		bool equals(const std::string& s0, const URL* p0, const std::string& s1, const URL* p1) const override
		{
			auto path0 = computePath(s0, p0), path1 = computePath(s1, p1);
			auto norm0 = std::filesystem::canonical(path0), norm1 = std::filesystem::canonical(path1);
			return norm0 == norm1;
		}
	};	
}

const std::shared_ptr<URLProtocol> URL::STRING_PROTOCOL(new StringProtocol());
const std::shared_ptr<URLProtocol> URL::FILE_PROTOCOL(new FileProtocol());

std::map<std::string, std::shared_ptr<URLProtocol>> URL::knownProtocols = {
	{"str", URL::STRING_PROTOCOL},
	{"file", URL::FILE_PROTOCOL}
};

URL::URL(std::shared_ptr<URLProtocol> protocol, const std::string& source, const URL* parent)
{
    this->protocol = protocol;
    this->source = source;
	this->parent = parent;
}

URL::URL(const std::string& value, const URL* parent)
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

    throw std::runtime_error("No such protocol: " + protocol);

    assignment:
    this->source = source;
    this->protocol = protocolObj;
	this->parent = parent;
}

std::shared_ptr<URLProtocol> URL::getProtocol() const
{
    return this->protocol;
}

std::string URL::getSource() const
{
    return this->source;
}

const URL* URL::getParent() const
{
	return this->parent;
}

std::unique_ptr<std::istream> URL::toInputStream() const
{
    return this->protocol->istream(this->source, this->parent);
}

std::string URL::read() const
{
    std::string result;
    result.assign(std::istreambuf_iterator<char>(*this->toInputStream()), std::istreambuf_iterator<char>());
    return result;
}

std::unique_ptr<std::ostream> URL::toOutputStream() const
{
	return this->protocol->ostream(this->source, this->parent);
}

bool URL::operator==(const URL& other) const
{
	if(this->protocol != other.protocol)
		return false;
	return this->protocol->equals(this->source, this->parent, other.source, other.parent);
}

bool URL::operator!=(const URL& other) const
{
	return !(*this == other);
}
