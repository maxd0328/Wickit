#pragma once

#include "include/definitions.h"

#define _VIS_NAMESPACE	wckt::type::
#define VIS_PUBLIC		_VIS_NAMESPACE Visibility(_VIS_NAMESPACE Visibility::PUBLIC)
#define VIS_RESTRICTED	_VIS_NAMESPACE Visibility(_VIS_NAMESPACE Visibility::RESTRICTED)
#define VIS_PRIVATE		_VIS_NAMESPACE Visibility(_VIS_NAMESPACE Visibility::PRIVATE)

#define _VIS_QTY	3
#define _VIS_VEC	{ VIS_PUBLIC, VIS_RESTRICTED, VIS_PRIVATE }

namespace wckt::type
{
	class Visibility
	{
		public:
			enum level_t
			{
				PRIVATE		= 0x0, // Values necessary for comparison
				RESTRICTED	= 0x1,
				PUBLIC		= 0x2
			};
		
		private:
			level_t value;

		public:
			Visibility(level_t value);
			~Visibility() = default;

			level_t getValue() const;
			std::string toString() const;

			bool operator==(Visibility other) const;
			bool operator!=(Visibility other) const;

			bool operator>(Visibility other) const;
			bool operator<(Visibility other) const;
			bool operator>=(Visibility other) const;
			bool operator<=(Visibility other) const;
	};
}
