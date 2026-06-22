#pragma once

#include "Core/Public/CString.h"
#include "Core/Public/ModuleManager.h"

// ============================================================================
// ModuleBinding
// ----------------------------------------------------------------------------
// 하나의 Engine 시스템이 자신의 논리 Module 구현을 획득하고 해제하는
// 비가상 수명 Binding입니다.
//
// Bind:
//   - 논리 moduleName으로 ModuleManager에 구현 Binary 획득을 요청합니다.
//   - 같은 Binding에 같은 Module을 다시 Bind하는 호출은 성공합니다.
//   - 다른 Module로의 재Bind는 기존 Binding을 Release하기 전에는 실패합니다.
//
// Release:
//   - 이 Binding이 획득한 참조만 해제합니다.
//   - 실제 Binary Unload 여부는 ModuleManager가 결정합니다.
//
// 책임:
//   - 단일 시스템의 Module 참조 수명 보관
//   - Bound Module의 export symbol 조회
//
// 비책임:
//   - Descriptor 선택 정책
//   - Module별 API 버전 및 도메인 검증
//   - Engine 시스템 초기화 순서
// ============================================================================

class ModuleBinding final
{
public:
	bool Bind(const CString& moduleName);
	bool Release();

	ModuleSymbol FindSymbol(const char* symbolName) const;
	bool IsBound() const;

private:
	CString boundModuleName;
};
