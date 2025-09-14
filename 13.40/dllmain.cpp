#include "framework.h"
#include "Offsets.h"
#include "Runtime.h"
#include "FortniteGame/Athena/Public/FortGameModeAthena.h"
#include "FortniteGame/Athena/Public/FortPlayerControllerAthena.h"
#include "Engine/NetDriver/Public/NetDriver.h"

int RetTrue() { return true; }
__int64(*DispatchRequestOG)(__int64 a1, __int64* a2, int a3);
__int64 DispatchRequest(__int64 a1, __int64* a2, int a3) { a3 = 3; return DispatchRequestOG(a1, a2, a3); }

void Main() {
    AllocConsole();
    FILE* F;
    freopen_s(&F, "CONOUT$", "w", stdout);
    SetConsoleTitleA("13.40 | Setting Up");
    MH_Initialize();
    Sleep(5000);

    NetDriver::Patch();
    FortGameModeAthena::Patch();
    FortPlayerControllerAthena::Patch();

    for (auto& NullFunc : Runtime::Offsets::NullFuncs) Runtime::Patch(NullFunc, 0xC3);
    for (auto& RetTrueFunc : Runtime::Offsets::RetTrueFuncs) Runtime::Hook(RetTrueFunc, RetTrue);
    Runtime::Hook(Runtime::Offsets::DispatchRequest, DispatchRequest, (void**)&DispatchRequestOG);
    Runtime::Hook(Runtime::Offsets::ImageBase + 0x2519860, RetTrue);

    MH_EnableHook(MH_ALL_HOOKS);

    *(bool*)(Runtime::Offsets::GIsClient) = false;
    *(bool*)(Runtime::Offsets::GIsClient + 1) = true;

    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);
}

BOOL APIENTRY DllMain(HMODULE Module, DWORD Reason, LPVOID Reserved)
{
    switch (Reason)
    {
    case 1: std::thread(Main).detach();
    case 0: break;
    }
    return 1;
}

