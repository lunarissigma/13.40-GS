#include "../Public/FortGameModeAthena.h"
#include "../Public/FortPlaylistAthena.h"

bool FortGameModeAthena::ReadyToStartMatch(AFortGameModeAthena* GameMode)
{
    auto GameState = (AFortGameStateAthena*)GameMode->GameState;
    if (!GameState->MapInfo) return false;

    static bool bPlaylist = false;
    if (!bPlaylist) {
        FortPlaylistAthena::AssignPlaylist(Runtime::StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo"), GameMode);
        bPlaylist = true;
    }

	if (!UWorld::GetWorld()->NetDriver)
	{
		static UNetDriver* (*CreateNetDriver) (UEngine * Engine, UWorld * World, FName NetDriverDefinition) = decltype(CreateNetDriver) (Runtime::Offsets::CreateNetDriver);
		static bool (*InitListen) (UNetDriver * Driver, UWorld * World, FURL&, bool, FString&) = decltype(InitListen) (Runtime::Offsets::InitListen);
		static void (*SetWorld) (UNetDriver * Driver, UWorld * World) = decltype(SetWorld) (Runtime::Offsets::SetWorld);

		FName NetDriverDefinintion = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

		auto Driver = CreateNetDriver(UEngine::GetEngine(), UWorld::GetWorld(), NetDriverDefinintion);
		if (Driver) {
			UWorld::GetWorld()->NetDriver = Driver;
			UWorld::GetWorld()->NetDriver->NetDriverName = NetDriverDefinintion;
			UWorld::GetWorld()->NetDriver->World = UWorld::GetWorld();

			FString Error;
			FURL URL;
			URL.Port = 7777;
			InitListen(UWorld::GetWorld()->NetDriver, UWorld::GetWorld(), URL, false, Error);
			SetWorld(UWorld::GetWorld()->NetDriver, UWorld::GetWorld());

			for (auto& LevelCollection : UWorld::GetWorld()->LevelCollections) {
				LevelCollection.NetDriver = UWorld::GetWorld()->NetDriver;
			}

			GameMode->bWorldIsReady = true;
			SetConsoleTitleA("13.40 | Listening");
		}
	}

	return GameMode->AlivePlayers.Num() > 0;
}

APawn* FortGameModeAthena::SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* Controller, AActor* StartSpot)
{
	auto Pawn = GameMode->SpawnDefaultPawnAtTransform(Controller, StartSpot->GetTransform());
	return Pawn;
}

void FortGameModeAthena::Patch()
{
	Runtime::Virtual<AFortGameModeAthena>(Runtime::Offsets::ReadyToStartMatchVft, ReadyToStartMatch);
	Runtime::Virtual<AFortGameModeAthena>(Runtime::Offsets::SpawnDefaultPawnForVft, SpawnDefaultPawnFor);
}