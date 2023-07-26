#include "type/access.h"
#include "include/exception.h"

using namespace wckt::type;

Visibility::Visibility(level_t value)
: value(value)
{}

Visibility::level_t Visibility::getValue() const
{
    return this->value;
}

std::string Visibility::toString() const
{
    switch(this->value)
    {
        case PRIVATE:
            return "private";
        case RESTRICTED:
            return "restricted";
        case PUBLIC:
            return "public";
        default: // Fallback
            return "[unknown]";
    }
}

bool Visibility::operator==(Visibility other) const { return this->value == other.value; }
bool Visibility::operator!=(Visibility other) const { return this->value != other.value; }

bool Visibility::operator>(Visibility other) const { return this->value > other.value; }
bool Visibility::operator<(Visibility other) const { return this->value < other.value; }
bool Visibility::operator>=(Visibility other) const { return this->value >= other.value; }
bool Visibility::operator<=(Visibility other) const { return this->value <= other.value; }

Visibility Visibility::fromString(const std::string& str)
{
	for(Visibility v : _VIS_VEC)
	{
		if(v.toString() == str)
			return v;
	}
	throw BadArgumentError("\'" + str + "\' is not a visibility modifier");
}
