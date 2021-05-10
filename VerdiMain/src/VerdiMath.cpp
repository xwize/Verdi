
#include "VerdiMath.h"

namespace Verdi {

	#undef max
	#undef min

	const float Math::INFINITY_POSITIVE = std::numeric_limits<float>::max();
	const float Math::INFINITY_NEGATIVE = -std::numeric_limits<float>::max();
	
	const float Math::EPSILON = std::numeric_limits<float>::min();

	const float Math::PI = 3.14159265f;
	const float Math::PI2 = 6.28318531f;
}