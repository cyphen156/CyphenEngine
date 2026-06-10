#include "pch.h"

#include <vector>

#include "Core/Public/File.h"
#include "Core/Public/TextCodec.h"
#include "HAL/Private/PlatformFile.h"

namespace
{
	TextEncoding GetDefaultTextEncoding()
	{
		return TextEncoding::Utf8;
	}
}

bool File::ReadAllBytes(
	const CString& path,
	std::vector<uint8>& outBytes)
{
	return PlatformFile::ReadAllBytes(path, outBytes);
}

bool File::WriteAllBytes(
	const CString& path,
	const std::vector<uint8>& bytes)
{
	return PlatformFile::WriteAllBytes(path, bytes);
}

bool File::ReadAllText(
	const CString& path,
	CString& outText)
{
	return ReadAllText(
		path,
		outText,
		GetDefaultTextEncoding());
}

bool File::ReadAllText(
	const CString& path,
	CString& outText,
	TextEncoding encoding)
{
	std::vector<uint8> bytes;

	if (!ReadAllBytes(path, bytes))
	{
		outText.clear();
		return false;
	}

	if (!TextCodec::Decode(bytes, outText, encoding))
	{
		outText.clear();
		return false;
	}

	return true;
}

bool File::WriteAllText(
	const CString& path,
	const CString& text)
{
	return WriteAllText(
		path,
		text,
		GetDefaultTextEncoding(),
		LineEnding::LF);
}

bool File::WriteAllText(
	const CString& path,
	const CString& text,
	TextEncoding encoding)
{
	return WriteAllText(
		path,
		text,
		encoding,
		LineEnding::LF);
}

bool File::WriteAllText(
	const CString& path,
	const CString& text,
	TextEncoding encoding,
	LineEnding lineEnding)
{
	std::vector<uint8> bytes;

	if (!TextCodec::Encode(text, bytes, encoding, lineEnding))
	{
		return false;
	}

	return WriteAllBytes(path, bytes);
}
