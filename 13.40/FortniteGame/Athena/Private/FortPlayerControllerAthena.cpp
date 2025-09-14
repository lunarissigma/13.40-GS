#include "../Public/FortPlayerControllerAthena.h"

void FortPlayerControllerAthena::ServerAcknowledgePossession(AFortPlayerControllerAthena* Controller, APawn* Pawn)
{
	Controller->AcknowledgedPawn = Pawn;
}

void FortPlayerControllerAthena::Patch()
{
	Runtime::Virtual<AFortPlayerControllerAthena>(Runtime::Offsets::ServerAcknowledgePossessionVft, ServerAcknowledgePossession);
}
