#pragma once
#include "framework.h"
#include "Offsets.h"
#include <map>
#include <set>

namespace Funcs {
	static inline auto StaticFindObject = (SDK::UObject * (*)(SDK::UClass*, SDK::UObject*, const wchar_t*, bool)) (Runtime::Offsets::StaticFindObject);
	static inline auto StaticLoadObject = (SDK::UObject * (*)(SDK::UClass*, SDK::UObject*, const TCHAR*, const TCHAR*, uint32_t, SDK::UObject*, bool)) uint64_t(Runtime::Offsets::StaticLoadObject);
	static void(*ServerReplicateActors)(UReplicationDriver* Driver) = decltype(ServerReplicateActors)(Runtime::Offsets::ServerReplicateActors);
};

namespace Runtime {
	static inline void* bignullboy = nullptr;
	static void Hook(uint64 Address, void* Detour, void** OG = nullptr)
	{
		MH_CreateHook(LPVOID(Address), Detour, OG);
		MH_EnableHook(LPVOID(Address));
	}

	template<typename U, typename T = void*>
	__forceinline void Virtual(uint32_t idx, void* detour, T& og = bignullboy) {
		auto VTable = (void**)U::GetDefaultObj()->VTable;
		if (!std::is_same_v<T, void*>) og = (T)VTable[idx];

		DWORD vpog;
		VirtualProtect(VTable + idx, 8, PAGE_EXECUTE_READWRITE, &vpog);
		VTable[idx] = detour;
		VirtualProtect(VTable + idx, 8, vpog, &vpog);
	}

	static void Patch(uintptr_t ptr, uint8_t byte)
	{
		DWORD og;
		VirtualProtect(LPVOID(ptr), sizeof(byte), PAGE_EXECUTE_READWRITE, &og);
		*(uint8_t*)ptr = byte;
		VirtualProtect(LPVOID(ptr), sizeof(byte), og, &og);
	}

	template <typename T>
	T* StaticFindObject(std::string ObjectPath, UClass* Class = nullptr) {
		return (T*)Funcs::StaticFindObject(Class, nullptr, std::wstring(ObjectPath.begin(), ObjectPath.end()).c_str(), false);
	}

	inline UObject* StaticFindObjectT(std::string ObjectPath, UClass* Class = nullptr) {
		return Funcs::StaticFindObject(Class, nullptr, std::wstring(ObjectPath.begin(), ObjectPath.end()).c_str(), false);
	}

	template<typename T>
	T* StaticLoadObject(std::string name) {
		T* Object = StaticFindObject<T>(name);
		if (!Object) {
			auto Name = std::wstring(name.begin(), name.end()).c_str();
			UObject* BaseObject = Funcs::StaticLoadObject(T::StaticClass(), nullptr, Name, nullptr, 0, nullptr, false);
			Object = static_cast<T*>(BaseObject);
		}
		return Object;
	}

	template <typename _Ot = void*>
	__forceinline static void Exec(const char* _Name, void* _Detour, _Ot& _Orig = bignullboy) {
		auto _Fn = StaticFindObject<UFunction>(_Name);
		if (!_Fn) return;
		if (!std::is_same_v<_Ot, void*>)
			_Orig = (_Ot)_Fn->ExecFunction;
		_Fn->ExecFunction = reinterpret_cast<UFunction::FNativeFuncPtr>(_Detour);
	}

	__forceinline TArray<AActor*> GetAll(UClass* Class)
	{
		TArray<AActor*> ret;
		UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), Class, &ret);
		return ret;
	}

	template <typename _At = AActor>
	__forceinline static TArray<_At*> GetAll(UClass* Class)
	{
		return GetAll(Class);
	}

	template <typename _At = AActor>
	__forceinline static TArray<_At*> GetAll()
	{
		TArray<_At*> Result;
		for (AActor* Actor : GetAll(AActor::StaticClass()))
		{
			if (auto CastedActor = Actor->Cast<_At>())
			{
				Result.Add(CastedActor);
			}
		}
		return Result;
	}
}

#define DefHookOg(_Rt, _Name, ...) static inline _Rt (*_Name##OG)(##__VA_ARGS__); static _Rt _Name(##__VA_ARGS__); 
#define callOG(_Tr, _Pt, _Th, ...) ([&](){ auto _Fn = Runtime::StaticFindObject<UFunction>(_Pt "." # _Th); _Fn->ExecFunction = (UFunction::FNativeFuncPtr) _Th##OG; _Tr->_Th(##__VA_ARGS__); _Fn->ExecFunction = (UFunction::FNativeFuncPtr) _Th; })()
