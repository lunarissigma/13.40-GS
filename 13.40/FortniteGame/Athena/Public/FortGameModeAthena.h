#pragma once
#include "../Runtime.h"

namespace FortGameModeAthena {
	static bool ReadyToStartMatch(AFortGameModeAthena* GameMode);
	static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* Controller, AActor* StartSpot);
	void Patch();;
}