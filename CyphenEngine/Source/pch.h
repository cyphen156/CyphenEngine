#pragma once
#ifndef PCH_H
#define PCH_H

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// C ǥ�� ��� �����Դϴ�.
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ ǥ�� ��� ���� �Դϴ�.
#include <iostream>
#include <string>


// Ŀ���� �ʼ� ��� ���� �Դϴ�.
#include "Build/Public/define.h"
#include "Build/Public/framework.h"
#include "Common/Public/types.h"

using namespace std;
#endif // PCH_H