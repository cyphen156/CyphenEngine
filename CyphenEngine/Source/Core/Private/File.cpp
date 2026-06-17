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

	TextEncoding ResolveBomEncoding(const std::vector<uint8>& bytes, TextEncoding fallbackEncoding)
	{
		if (bytes.size() >= 3 &&
			bytes[0] == 0xEF &&
			bytes[1] == 0xBB &&
			bytes[2] == 0xBF)
		{
			return TextEncoding::Utf8;
		}

		if (bytes.size() >= 2 &&
			bytes[0] == 0xFF &&
			bytes[1] == 0xFE)
		{
			return TextEncoding::Utf16LE;
		}

		if (bytes.size() >= 2 &&
			bytes[0] == 0xFE &&
			bytes[1] == 0xFF)
		{
			return TextEncoding::Utf16BE;
		}

		return fallbackEncoding;
	}
}

bool File::ReadAllBytes(const CString& path, std::vector<uint8>& outBytes)
{
	return PlatformFile::ReadAllBytes(path, outBytes);
}

bool File::WriteAllBytes(const CString& path, const std::vector<uint8>& bytes)
{
	return PlatformFile::WriteAllBytes(path, bytes);
}

bool File::AppendAllBytes(const CString& path, const std::vector<uint8>& bytes)
{
	return PlatformFile::AppendAllBytes(path, bytes);
}

bool File::ReadAllText(const CString& path, CString& outText)
{
	return ReadAllText(
		path,
		outText,
		GetDefaultTextEncoding());
}

bool File::ReadAllText(const CString& path, CString& outText,
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

bool File::WriteAllText(const CString& path, const CString& text)
{
	return WriteAllText(
		path,
		text,
		GetDefaultTextEncoding(),
		LineEnding::LF);
}

bool File::WriteAllText(const CString& path, const CString& text,
	TextEncoding encoding)
{
	return WriteAllText(
		path,
		text,
		encoding,
		LineEnding::LF);
}

bool File::WriteAllText(const CString& path, const CString& text,
	TextEncoding encoding, LineEnding lineEnding)
{
	std::vector<uint8> bytes;

	if (!TextCodec::Encode(text, bytes, encoding, lineEnding))
	{
		return false;
	}

	return WriteAllBytes(path, bytes);
}

bool File::AppendAllText(const CString& path, const CString& text)
{
	TextEncoding encoding = GetDefaultTextEncoding();

	uint64 fileSize = 0;

	if (PlatformFile::GetSize(path, fileSize) && fileSize > 0)
	{
		std::vector<uint8> headBytes;

		if (PlatformFile::ReadHead(path, 3, headBytes))
		{
			encoding = ResolveBomEncoding(headBytes, encoding);
		}
	}

	std::vector<uint8> bytes;

	if (!TextCodec::Encode(text, bytes, encoding, LineEnding::Preserve))
	{
		return false;
	}

	return AppendAllBytes(path, bytes);
}
