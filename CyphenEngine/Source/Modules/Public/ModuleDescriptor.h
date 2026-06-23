#pragma once

#include "Core/Public/CString.h"

// ============================================================================
// ModuleDescriptor
// ----------------------------------------------------------------------------
// 이번 실행에서 사용할 논리 Module과 선택 구현을 기술합니다.
//
// moduleName:
//   Engine 시스템이 식별하는 논리 Module 이름입니다.
//
// implementationName:
//   이번 실행에서 선택된 구현 이름입니다.
//
// binaryName:
//   ModuleLoader가 적재할 Native Binary 이름입니다.
//
// isEnabled:
//   이번 실행에서 해당 Module을 활성 대상으로 볼지 여부입니다.
// ============================================================================
struct ModuleDescriptor
{
	CString moduleName;
	CString implementationName;
	CString binaryName;
	bool isEnabled = false;
};
