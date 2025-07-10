#include "pch.h"
#include "Utility/Public/TstringUtility.h"

TSTRING TstringUtility::CharToTString(const char* str)
{
	const char* src = str;
	TSTRING result;

	while (*src)
	{
		result += (TCHAR)(unsigned char)(*src++);
	}
	return result;
}
