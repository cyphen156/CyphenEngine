#pragma once
#ifndef PCH_H
#define PCH_H

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// 커스텀 필수 헤더
#include "Build/Public/PlatformDefine.h"
#include "Build/Public/framework.h"
#include "Build/Public/define.h"

#endif // PCH_H
