#include "../Public/NetDriver.h"

void NetDriver::TickFlush(UNetDriver* Driver) {
	if (Driver->ReplicationDriver) Funcs::ServerReplicateActors(Driver->ReplicationDriver);

	return TickFlushOG(Driver);
}

void NetDriver::Patch() {
	Runtime::Hook(Runtime::Offsets::TickFlush, TickFlush, (void**)&TickFlushOG);
}