#pragma once

#include <vector>

#include "Core/Public/CString.h"
#include "Core/Public/ModuleDescriptor.h"

using ModuleSymbol = void*;

// ============================================================================
// ModuleManager
// ----------------------------------------------------------------------------
// 프로세스 단위 엔진 모듈 관리자입니다.
//
// ModuleManager는 설치된 모듈 Descriptor와 로드된 모듈의 네이티브
// 핸들을 관리합니다.
//
// 책임:
//   - ModuleDescriptor 상태 동기화
//   - Enabled 모듈 로드
//   - Disabled / 제거 모듈 언로드
//   - 로드된 모듈의 심볼 조회
//   - 모듈 로드 순서 및 수명 관리
//
// 비책임:
//   - 사용자 Preference 조회
//   - 플랫폼 동적 라이브러리 API 호출
//   - 모듈별 도메인 및 API 버전 검증
//   - 엔진 상태 관리
//
// 플랫폼별 물리적 모듈 입출력은 ModuleLoader가 담당합니다.
// ============================================================================

class ModuleManager final
{
public:
	static bool Refresh(const std::vector<ModuleDescriptor>& moduleDescriptors);
	static bool Shutdown();

	static bool Load(const CString& moduleName);
	static bool Unload(const CString& moduleName);

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

	static bool UnloadAll();
};
