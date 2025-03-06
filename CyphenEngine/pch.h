#pragma once
#ifndef PCH_H
#define PCH_H

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <iostream>
#include "framework.h"
#include "define.h"
#include "types.h"

using namespace std;
#endif // PCH_H