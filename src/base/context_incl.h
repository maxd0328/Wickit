/**
 * Unfortunately used to resolve cyclic dependencies between locators and engine context
 * Max Derbenwick was here
 */

#pragma once

#define __CTX_CONST		const wckt::base::EngineContext&
#define __CTX			wckt::base::EngineContext&

namespace wckt::base
{
	class EngineContext;
}
