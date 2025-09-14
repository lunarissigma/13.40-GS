#pragma once
#include "../Runtime.h"

namespace NetDriver {
	DefHookOg(void, TickFlush, UNetDriver*);

	void Patch();
}