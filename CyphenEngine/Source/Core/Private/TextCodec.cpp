#include "pch.h"

#include <vector>

#include "Core/Public/TextCodec.h"

namespace
{
	constexpr char32_t ReplacementCodePoint = 0xFFFD;

	bool IsHighSurrogate(char32_t codePoint)
	{
		return codePoint >= 0xD800 && codePoint <= 0xDBFF;
	}

	bool IsLowSurrogate(char32_t codePoint)
	{
		return codePoint >= 0xDC00 && codePoint <= 0xDFFF;
	}

	bool IsValidCodePoint(char32_t codePoint)
	{
		if (codePoint > 0x10FFFF)
		{
			return false;
		}

		if (IsHighSurrogate(codePoint) || IsLowSurrogate(codePoint))
		{
			return false;
		}

		return true;
	}

	bool AppendCodePointToCString(
		char32_t codePoint,
		CString& outText)
	{
		if (!IsValidCodePoint(codePoint))
		{
			return false;
		}

#if CCHAR_IS_UTF8

		if (codePoint <= 0x7F)
		{
			outText.push_back(static_cast<CChar>(codePoint));
			return true;
		}

		if (codePoint <= 0x7FF)
		{
			outText.push_back(static_cast<CChar>(0xC0 | (codePoint >> 6)));
			outText.push_back(static_cast<CChar>(0x80 | (codePoint & 0x3F)));
			return true;
		}

		if (codePoint <= 0xFFFF)
		{
			outText.push_back(static_cast<CChar>(0xE0 | (codePoint >> 12)));
			outText.push_back(static_cast<CChar>(0x80 | ((codePoint >> 6) & 0x3F)));
			outText.push_back(static_cast<CChar>(0x80 | (codePoint & 0x3F)));
			return true;
		}

		outText.push_back(static_cast<CChar>(0xF0 | (codePoint >> 18)));
		outText.push_back(static_cast<CChar>(0x80 | ((codePoint >> 12) & 0x3F)));
		outText.push_back(static_cast<CChar>(0x80 | ((codePoint >> 6) & 0x3F)));
		outText.push_back(static_cast<CChar>(0x80 | (codePoint & 0x3F)));
		return true;

#elif CCHAR_IS_UTF16 || CCHAR_IS_WCHAR

		if constexpr (sizeof(CChar) == 2)
		{
			if (codePoint <= 0xFFFF)
			{
				outText.push_back(static_cast<CChar>(codePoint));
				return true;
			}

			codePoint -= 0x10000;

			const CChar highSurrogate = static_cast<CChar>(0xD800 + (codePoint >> 10));
			const CChar lowSurrogate = static_cast<CChar>(0xDC00 + (codePoint & 0x3FF));

			outText.push_back(highSurrogate);
			outText.push_back(lowSurrogate);
			return true;
		}
		else
		{
			outText.push_back(static_cast<CChar>(codePoint));
			return true;
		}

#else

		return false;

#endif
	}

	bool DecodeUtf8CodePoint(
		const std::vector<uint8>& bytes,
		std::size_t& index,
		char32_t& outCodePoint)
	{
		if (index >= bytes.size())
		{
			return false;
		}

		const uint8 firstByte = bytes[index];

		if (firstByte <= 0x7F)
		{
			outCodePoint = firstByte;
			++index;
			return true;
		}

		std::size_t byteCount = 0;
		char32_t codePoint = 0;
		char32_t minCodePoint = 0;

		if ((firstByte & 0xE0) == 0xC0)
		{
			byteCount = 2;
			codePoint = firstByte & 0x1F;
			minCodePoint = 0x80;
		}
		else if ((firstByte & 0xF0) == 0xE0)
		{
			byteCount = 3;
			codePoint = firstByte & 0x0F;
			minCodePoint = 0x800;
		}
		else if ((firstByte & 0xF8) == 0xF0)
		{
			byteCount = 4;
			codePoint = firstByte & 0x07;
			minCodePoint = 0x10000;
		}
		else
		{
			return false;
		}

		if (index + byteCount > bytes.size())
		{
			return false;
		}

		for (std::size_t i = 1; i < byteCount; ++i)
		{
			const uint8 currentByte = bytes[index + i];

			if ((currentByte & 0xC0) != 0x80)
			{
				return false;
			}

			codePoint = (codePoint << 6) | (currentByte & 0x3F);
		}

		if (codePoint < minCodePoint)
		{
			return false;
		}

		if (!IsValidCodePoint(codePoint))
		{
			return false;
		}

		outCodePoint = codePoint;
		index += byteCount;
		return true;
	}

	uint16 ReadUint16(
		const std::vector<uint8>& bytes,
		std::size_t index,
		bool bLittleEndian)
	{
		if (bLittleEndian)
		{
			return static_cast<uint16>(
				bytes[index] |
				(static_cast<uint16>(bytes[index + 1]) << 8));
		}

		return static_cast<uint16>(
			(static_cast<uint16>(bytes[index]) << 8) |
			bytes[index + 1]);
	}

	void WriteUint16(
		std::vector<uint8>& outBytes,
		uint16 value,
		bool bLittleEndian)
	{
		if (bLittleEndian)
		{
			outBytes.push_back(static_cast<uint8>(value & 0xFF));
			outBytes.push_back(static_cast<uint8>((value >> 8) & 0xFF));
			return;
		}

		outBytes.push_back(static_cast<uint8>((value >> 8) & 0xFF));
		outBytes.push_back(static_cast<uint8>(value & 0xFF));
	}

	bool DecodeUtf16CodePoint(
		const std::vector<uint8>& bytes,
		std::size_t& index,
		bool bLittleEndian,
		char32_t& outCodePoint)
	{
		if (index + 2 > bytes.size())
		{
			return false;
		}

		const uint16 firstUnit = ReadUint16(bytes, index, bLittleEndian);
		index += 2;

		if (IsHighSurrogate(firstUnit))
		{
			if (index + 2 > bytes.size())
			{
				return false;
			}

			const uint16 secondUnit = ReadUint16(bytes, index, bLittleEndian);

			if (!IsLowSurrogate(secondUnit))
			{
				return false;
			}

			index += 2;

			outCodePoint =
				0x10000 +
				(((static_cast<char32_t>(firstUnit) - 0xD800) << 10) |
					(static_cast<char32_t>(secondUnit) - 0xDC00));

			return true;
		}

		if (IsLowSurrogate(firstUnit))
		{
			return false;
		}

		outCodePoint = firstUnit;
		return true;
	}

	bool AppendCodePointAsUtf8Bytes(
		char32_t codePoint,
		std::vector<uint8>& outBytes)
	{
		if (!IsValidCodePoint(codePoint))
		{
			return false;
		}

		if (codePoint <= 0x7F)
		{
			outBytes.push_back(static_cast<uint8>(codePoint));
			return true;
		}

		if (codePoint <= 0x7FF)
		{
			outBytes.push_back(static_cast<uint8>(0xC0 | (codePoint >> 6)));
			outBytes.push_back(static_cast<uint8>(0x80 | (codePoint & 0x3F)));
			return true;
		}

		if (codePoint <= 0xFFFF)
		{
			outBytes.push_back(static_cast<uint8>(0xE0 | (codePoint >> 12)));
			outBytes.push_back(static_cast<uint8>(0x80 | ((codePoint >> 6) & 0x3F)));
			outBytes.push_back(static_cast<uint8>(0x80 | (codePoint & 0x3F)));
			return true;
		}

		outBytes.push_back(static_cast<uint8>(0xF0 | (codePoint >> 18)));
		outBytes.push_back(static_cast<uint8>(0x80 | ((codePoint >> 12) & 0x3F)));
		outBytes.push_back(static_cast<uint8>(0x80 | ((codePoint >> 6) & 0x3F)));
		outBytes.push_back(static_cast<uint8>(0x80 | (codePoint & 0x3F)));
		return true;
	}

	bool AppendCodePointAsUtf16Bytes(
		char32_t codePoint,
		std::vector<uint8>& outBytes,
		bool bLittleEndian)
	{
		if (!IsValidCodePoint(codePoint))
		{
			return false;
		}

		if (codePoint <= 0xFFFF)
		{
			WriteUint16(outBytes, static_cast<uint16>(codePoint), bLittleEndian);
			return true;
		}

		codePoint -= 0x10000;

		const uint16 highSurrogate = static_cast<uint16>(0xD800 + (codePoint >> 10));
		const uint16 lowSurrogate = static_cast<uint16>(0xDC00 + (codePoint & 0x3FF));

		WriteUint16(outBytes, highSurrogate, bLittleEndian);
		WriteUint16(outBytes, lowSurrogate, bLittleEndian);
		return true;
	}

	bool AppendCodePointAsAnsiByte(
		char32_t codePoint,
		std::vector<uint8>& outBytes)
	{
		if (codePoint > 0x7F)
		{
			return false;
		}

		outBytes.push_back(static_cast<uint8>(codePoint));
		return true;
	}

	bool AppendCodePointAsEncodedBytes(
		char32_t codePoint,
		std::vector<uint8>& outBytes,
		TextEncoding encoding)
	{
		switch (encoding)
		{
		case TextEncoding::Utf8:
		case TextEncoding::Utf8WithBom:
			return AppendCodePointAsUtf8Bytes(codePoint, outBytes);

		case TextEncoding::Utf16LE:
			return AppendCodePointAsUtf16Bytes(codePoint, outBytes, true);

		case TextEncoding::Utf16BE:
			return AppendCodePointAsUtf16Bytes(codePoint, outBytes, false);

		case TextEncoding::Ansi:
			return AppendCodePointAsAnsiByte(codePoint, outBytes);

		default:
			return false;
		}
	}

	bool ReadNextCodePointFromCString(
		const CString& text,
		std::size_t& index,
		char32_t& outCodePoint)
	{
#if CCHAR_IS_UTF8

		if (index >= text.length())
		{
			return false;
		}

		const uint8 firstByte = static_cast<uint8>(text[index]);

		if (firstByte <= 0x7F)
		{
			outCodePoint = firstByte;
			++index;
			return true;
		}

		std::size_t byteCount = 0;
		char32_t codePoint = 0;
		char32_t minCodePoint = 0;

		if ((firstByte & 0xE0) == 0xC0)
		{
			byteCount = 2;
			codePoint = firstByte & 0x1F;
			minCodePoint = 0x80;
		}
		else if ((firstByte & 0xF0) == 0xE0)
		{
			byteCount = 3;
			codePoint = firstByte & 0x0F;
			minCodePoint = 0x800;
		}
		else if ((firstByte & 0xF8) == 0xF0)
		{
			byteCount = 4;
			codePoint = firstByte & 0x07;
			minCodePoint = 0x10000;
		}
		else
		{
			return false;
		}

		if (index + byteCount > text.length())
		{
			return false;
		}

		for (std::size_t i = 1; i < byteCount; ++i)
		{
			const uint8 currentByte = static_cast<uint8>(text[index + i]);

			if ((currentByte & 0xC0) != 0x80)
			{
				return false;
			}

			codePoint = (codePoint << 6) | (currentByte & 0x3F);
		}

		if (codePoint < minCodePoint)
		{
			return false;
		}

		if (!IsValidCodePoint(codePoint))
		{
			return false;
		}

		outCodePoint = codePoint;
		index += byteCount;
		return true;

#elif CCHAR_IS_UTF16 || CCHAR_IS_WCHAR

		if (index >= text.length())
		{
			return false;
		}

		if constexpr (sizeof(CChar) == 2)
		{
			const char32_t firstUnit = static_cast<char32_t>(text[index]);
			++index;

			if (IsHighSurrogate(firstUnit))
			{
				if (index >= text.length())
				{
					return false;
				}

				const char32_t secondUnit = static_cast<char32_t>(text[index]);

				if (!IsLowSurrogate(secondUnit))
				{
					return false;
				}

				++index;

				outCodePoint =
					0x10000 +
					(((firstUnit - 0xD800) << 10) |
						(secondUnit - 0xDC00));

				return true;
			}

			if (IsLowSurrogate(firstUnit))
			{
				return false;
			}

			outCodePoint = firstUnit;
			return true;
		}
		else
		{
			outCodePoint = static_cast<char32_t>(text[index]);
			++index;
			return IsValidCodePoint(outCodePoint);
		}

#else

		return false;

#endif
	}

	bool AppendLineEnding(
		std::vector<uint8>& outBytes,
		TextEncoding encoding,
		LineEnding lineEnding)
	{
		if (lineEnding == LineEnding::CRLF)
		{
			if (!AppendCodePointAsEncodedBytes(U'\r', outBytes, encoding))
			{
				return false;
			}
		}

		return AppendCodePointAsEncodedBytes(U'\n', outBytes, encoding);
	}

	bool DecodeUtf8(
		const std::vector<uint8>& bytes,
		CString& outText)
	{
		std::size_t index = 0;

		if (bytes.size() >= 3 &&
			bytes[0] == 0xEF &&
			bytes[1] == 0xBB &&
			bytes[2] == 0xBF)
		{
			index = 3;
		}

		while (index < bytes.size())
		{
			char32_t codePoint = 0;

			if (!DecodeUtf8CodePoint(bytes, index, codePoint))
			{
				return false;
			}

			if (!AppendCodePointToCString(codePoint, outText))
			{
				return false;
			}
		}

		return true;
	}

	bool DecodeUtf16(
		const std::vector<uint8>& bytes,
		CString& outText,
		bool bLittleEndian)
	{
		if ((bytes.size() % 2) != 0)
		{
			return false;
		}

		std::size_t index = 0;

		if (bytes.size() >= 2)
		{
			if (bLittleEndian && bytes[0] == 0xFF && bytes[1] == 0xFE)
			{
				index = 2;
			}
			else if (!bLittleEndian && bytes[0] == 0xFE && bytes[1] == 0xFF)
			{
				index = 2;
			}
		}

		while (index < bytes.size())
		{
			char32_t codePoint = 0;

			if (!DecodeUtf16CodePoint(bytes, index, bLittleEndian, codePoint))
			{
				return false;
			}

			if (!AppendCodePointToCString(codePoint, outText))
			{
				return false;
			}
		}

		return true;
	}

	bool DecodeAnsi(
		const std::vector<uint8>& bytes,
		CString& outText)
	{
		for (uint8 byte : bytes)
		{
			if (byte > 0x7F)
			{
				return false;
			}

			if (!AppendCodePointToCString(static_cast<char32_t>(byte), outText))
			{
				return false;
			}
		}

		return true;
	}
}

bool TextCodec::Encode(
	const CString& text,
	std::vector<uint8>& outBytes,
	TextEncoding encoding,
	LineEnding lineEnding)
{
	outBytes.clear();

	if (encoding == TextEncoding::Utf8WithBom)
	{
		outBytes.push_back(0xEF);
		outBytes.push_back(0xBB);
		outBytes.push_back(0xBF);
	}

	std::size_t index = 0;

	while (index < text.length())
	{
		char32_t codePoint = 0;

		if (!ReadNextCodePointFromCString(text, index, codePoint))
		{
			outBytes.clear();
			return false;
		}

		if (lineEnding == LineEnding::Preserve)
		{
			if (!AppendCodePointAsEncodedBytes(codePoint, outBytes, encoding))
			{
				outBytes.clear();
				return false;
			}

			continue;
		}

		if (codePoint == U'\r')
		{
			const std::size_t nextIndex = index;
			char32_t nextCodePoint = 0;

			if (ReadNextCodePointFromCString(text, index, nextCodePoint))
			{
				if (nextCodePoint != U'\n')
				{
					index = nextIndex;
				}
			}
			else
			{
				index = nextIndex;
			}

			if (!AppendLineEnding(outBytes, encoding, lineEnding))
			{
				outBytes.clear();
				return false;
			}

			continue;
		}

		if (codePoint == U'\n')
		{
			if (!AppendLineEnding(outBytes, encoding, lineEnding))
			{
				outBytes.clear();
				return false;
			}

			continue;
		}

		if (!AppendCodePointAsEncodedBytes(codePoint, outBytes, encoding))
		{
			outBytes.clear();
			return false;
		}
	}

	return true;
}

bool TextCodec::Decode(
	const std::vector<uint8>& bytes,
	CString& outText,
	TextEncoding encoding)
{
	outText.clear();

	switch (encoding)
	{
	case TextEncoding::Utf8:
	case TextEncoding::Utf8WithBom:
		return DecodeUtf8(bytes, outText);

	case TextEncoding::Utf16LE:
		return DecodeUtf16(bytes, outText, true);

	case TextEncoding::Utf16BE:
		return DecodeUtf16(bytes, outText, false);

	case TextEncoding::Ansi:
		return DecodeAnsi(bytes, outText);

	default:
		return false;
	}
}
