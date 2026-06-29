#include "pch.h"

#include <vector>

#include "Core/Public/TextCodec.h"
#include "Platform/Linux/Private/LinuxString.h"

bool LinuxString::ToUtf8String(
	const CString& text,
	std::string& outText)
{
	outText.clear();

#if CCHAR_IS_UTF8

	outText.assign(text.begin(), text.end());

	return true;

#else

	std::vector<uint8> bytes;

	if (!TextCodec::Encode(
		text,
		bytes,
		TextEncoding::Utf8,
		LineEnding::Preserve))
	{
		return false;
	}

	outText.assign(bytes.begin(), bytes.end());

	return true;

#endif
}
