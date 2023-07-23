#include "base/context.h"
#include "base/modules/xmlrules.h"

using namespace wckt::base;

EngineContext::EngineContext()
{
	
}

uint32_t EngineContext::getContextID() const
{
	return this->contextID;
}
