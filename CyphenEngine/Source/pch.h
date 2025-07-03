#pragma once
#ifndef PCH_H
#define PCH_H

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <iostream>
#include "Build/Public/framework.h"
#include "Build/Public/define.h"
#include "Common/Public/types.h"

using namespace std;
#endif // PCH_H