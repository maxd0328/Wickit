#include "base/url.h"

using namespace wckt::base;

std::unique_ptr<std::istream> StringProtocol::stream(const std::string& source) const
{
    return std::make_unique<std::istringstream>(source);
}

std::unique_ptr<std::istream> FileProtocol::stream(const std::string& source) const
{
    auto stream = std::make_unique<std::ifstream>(source);
    if(!stream->is_open())
        throw std::runtime_error("Failed to open file: " + source);
    return stream;
}

auto URL::knownProtocols = std::map<std::string, std::shared_ptr<URLProtocol>>();

URL::URL(std::shared_ptr<URLProtocol> protocol, const std::string& source)
{
    this->protocol = protocol;
    this->source = source;
}

URL::URL(const std::string& value)
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
}

std::shared_ptr<URLProtocol> URL::getProtocol() const
{
    return this->protocol;
}

std::string URL::getSource() const
{
    return this->source;
}

std::unique_ptr<std::istream> URL::toStream() const
{
    return this->protocol->stream(this->source);
}

std::string URL::read() const
{
    std::string result;
    result.assign(std::istreambuf_iterator<char>(*this->toStream()), std::istreambuf_iterator<char>());
    return result;
}
