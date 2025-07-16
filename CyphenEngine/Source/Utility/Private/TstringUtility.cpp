#include "pch.h"
#include "Utility/Public/TstringUtility.h"

TSTRING TstringUtility::CharToTString(const char* str)
{
	TSTRING result;

	while (*str)
	{
		result += (TCHAR)(unsigned char)(*str++);
	}
	return result;
}
