#pragma once

#include "Core/Public/CString.h"

class ModuleManager;

// ============================================================================
// ModuleLoader
// ----------------------------------------------------------------------------
// ModuleManager가 사용하는 플랫폼별 동적 모듈 입출력 HAL입니다.
//
// 책임:
//   - 논리적 모듈 이름을 플랫폼 파일명으로 변환
//   - 동적 모듈 바이너리 로드
//   - export 심볼 조회
//   - 동적 모듈 언로드
//
// 비책임:
//   - 모듈 Enabled 상태 결정
//   - Descriptor 및 Record 관리
//   - 모듈별 API와 버전 검증
// ============================================================================

class ModuleLoader final
{
private:
	friend class ModuleManager;

	static bool Load(const CString& moduleName, void*& outNativeHandle);

	static void* FindSymbol(void* nativeHandle, const char* symbolName);

	static bool Unload(void*& nativeHandle);

	ModuleLoader() = delete;
	~ModuleLoader() = delete;

	ModuleLoader(const ModuleLoader& other) = delete;
	ModuleLoader& operator=(const ModuleLoader& other) = delete;

	ModuleLoader(ModuleLoader&& other) = delete;
	ModuleLoader& operator=(ModuleLoader&& other) = delete;
};
