#pragma once

#include "Core/Public/CString.h"
#include "Modules/Public/ModuleManager.h"

// ============================================================================
// ModuleBinder
// ----------------------------------------------------------------------------
// 하나의 Engine 시스템이 자신이 사용할 논리 Module 구현을 획득하고,
// 자기 수명 종료 시 해제하는 바인더입니다.
//
// Bind:
//   - 논리 moduleName으로 ModuleManager에 구현 Binary 획득을 요청합니다.
//   - 이미 같은 Module에 Bind되어 있으면 성공으로 처리합니다.
//   - 다른 Module로의 재바인딩은 기존 Binding을 Release하기 전에는 실패합니다.
//
// Release:
//   - 현재 Binder가 획득한 Module 참조를 해제합니다.
//   - 실제 Binary Unload 여부는 ModuleManager가 결정합니다.
//
// 책임:
//   - 단일 시스템의 Module 참조 수명 보관
//   - Bound Module의 export symbol 조회
//
// 비책임:
//   - Descriptor 선택 정책
//   - Module별 API 버전 / 파라미터 검증
//   - Engine 시스템 초기화 순서
// ============================================================================
class ModuleBinder final
{
public:
	bool Bind(const CString& moduleName);
	bool Release();

	ModuleSymbol FindSymbol(const char* symbolName) const;
	bool IsBound() const;

private:
	CString boundModuleName;
};
