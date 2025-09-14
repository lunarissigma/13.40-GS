#include "../Public/FortPlaylistAthena.h"

void FortPlaylistAthena::AssignPlaylist(SDK::UFortPlaylistAthena* Playlist, SDK::AFortGameModeAthena* GameMode)
{
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)GameMode->GameState;
	GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
	GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
	GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
	GameState->CurrentPlaylistInfo.MarkArrayDirty();
	GameState->OnRep_CurrentPlaylistInfo();

	GameState->CurrentPlaylistId = GameMode->CurrentPlaylistId = Playlist->PlaylistId;
	GameState->OnRep_CurrentPlaylistId();

	GameMode->CurrentPlaylistName = Playlist->PlaylistName;

	GameState->OnRep_CurrentPlaylistInfo();
	GameState->OnRep_CurrentPlaylistId();

	GameMode->GameSession->MaxPlayers = Playlist->MaxPlayers;

	GameState->AirCraftBehavior = Playlist->AirCraftBehavior;
	GameState->CachedSafeZoneStartUp = Playlist->SafeZoneStartUp;
	GameState->WorldLevel = Playlist->LootLevel;

	for (auto& Level : Playlist->AdditionalLevels)
	{
		bool Success = false;

		ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), Level, FVector(), FRotator(), &Success, FString());
		FAdditionalLevelStreamed level{};
		level.bIsServerOnly = false;
		level.LevelName = Level.ObjectID.AssetPathName;
		if (Success) GameState->AdditionalPlaylistLevelsStreamed.Add(level);
	}

	for (auto& Level : Playlist->AdditionalLevelsServerOnly)
	{
		bool Success = false;
		ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), Level, FVector(), FRotator(), &Success, FString());
		FAdditionalLevelStreamed level{};
		level.bIsServerOnly = true;
		level.LevelName = Level.ObjectID.AssetPathName;
		if (Success) GameState->AdditionalPlaylistLevelsStreamed.Add(level);
	}

	((AFortGameStateAthena*)GameMode->GameState)->OnRep_AdditionalPlaylistLevelsStreamed();
	GameState->OnRep_AdditionalPlaylistLevelsStreamed();
}