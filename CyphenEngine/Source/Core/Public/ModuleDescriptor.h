#pragma once

#include "Core/Public/CString.h"

// ============================================================================
// ModuleDescriptor
// ----------------------------------------------------------------------------
// 이번 실행에서 사용할 논리 모듈과 선택 구현을 기술합니다.
//
// moduleName:
//   상태와 수명을 식별하는 논리 모듈 이름입니다.
//
// implementationName:
//   이번 실행에서 선택한 구현 이름입니다.
//
// binaryName:
//   ModuleLoader가 적재할 네이티브 Binary 이름입니다.
//
// isEnabled:
//   이번 실행에서 모듈을 활성화할지 나타냅니다.
// ============================================================================

struct ModuleDescriptor
{
	CString moduleName;
	CString implementationName;
	CString binaryName;
	bool isEnabled = false;
};
