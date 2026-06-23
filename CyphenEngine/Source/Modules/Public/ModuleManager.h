#pragma once

#include <vector>

#include "Core/Public/CString.h"
#include "Modules/Public/ModuleDescriptor.h"

using ModuleSymbol = void*;

// ============================================================================
// ModuleManager
// ----------------------------------------------------------------------------
// 프로세스 단위 논리 Module Runtime Manager입니다.
//
// Refresh:
//   - 외부에서 주입된 Desired ModuleDescriptor 목록을 갱신합니다.
//   - Descriptor를 검증하고 유효한 항목만 보관합니다.
//   - Binary Load / Unload는 수행하지 않습니다.
//
// Acquire:
//   - 논리 moduleName에 선택된 구현 Binary를 획득합니다.
//   - 첫 참조에서만 ModuleLoader::Load를 호출합니다.
//   - 이미 같은 구현이 로드된 경우 참조 횟수만 증가시킵니다.
//   - 로드된 구현과 Desired 구현이 다르면 교체하지 않고 실패합니다.
//
// Release:
//   - 해당 논리 Module의 참조 횟수를 감소시킵니다.
//   - 마지막 참조에서만 ModuleLoader::Unload를 호출합니다.
//
// Shutdown:
//   - 남아 있는 Binary를 역순으로 강제 Unload하는 종료 안전망입니다.
//
// 책임:
//   - Desired Descriptor 상태
//   - 선택 구현 Binary의 native handle
//   - 참조 횟수와 Load 순서
//   - export symbol 조회
//
// 비책임:
//   - 사용자 Preference 조회
//   - Renderer / Audio 등 도메인 API 검증
//   - Engine 시스템의 시작 / 종료 순서
// ============================================================================
class ModuleManager final
{
public:
	static bool Refresh(const std::vector<ModuleDescriptor>& moduleDescriptors);
	static bool Shutdown();

	static bool Acquire(const CString& moduleName);
	static bool Release(const CString& moduleName);

	static ModuleSymbol FindSymbol(const CString& moduleName, const char* symbolName);

	static bool IsLoaded(const CString& moduleName);
	static void GetLoadedModuleNames(std::vector<CString>& outModuleNames);

private:
	ModuleManager() = delete;
	~ModuleManager() = delete;

	ModuleManager(const ModuleManager& other) = delete;
	ModuleManager& operator=(const ModuleManager& other) = delete;

	ModuleManager(ModuleManager&& other) = delete;
	ModuleManager& operator=(ModuleManager&& other) = delete;

	static bool RemoveModuleRecord(const CString& moduleName);
	static bool UnloadAll();
};
