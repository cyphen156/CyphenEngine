#include "pch.h"

#include <cstdio>
#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"
#include "Core/Public/File.h"
#include "Core/Public/Path.h"
#include "Core/Public/TextCodec.h"

namespace
{
	struct TestContext
	{
		int32 passCount = 0;
		int32 failCount = 0;
	};

	void WriteTestLine(const char* message)
	{
#if PLATFORM_WINDOWS
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
#endif
	}

	void Expect(
		TestContext& context,
		bool condition,
		const char* name)
	{
		if (condition)
		{
			++context.passCount;

#if PLATFORM_WINDOWS
			OutputDebugStringA("[PASS] ");
#endif
		}
		else
		{
			++context.failCount;

#if PLATFORM_WINDOWS
			OutputDebugStringA("[FAIL] ");
#endif
		}

		WriteTestLine(name);
	}

	bool BytesEqual(
		const std::vector<uint8>& actual,
		const std::vector<uint8>& expected)
	{
		return actual == expected;
	}

	void RunPathTests(TestContext& context)
	{
		Expect(context, Path::Normalize(CTEXT("A\\B//C")) == CTEXT("A/B/C"),
			"Path.Normalize converts separators and collapses duplicates");

		Expect(context, Path::Combine(CTEXT("A/"), CTEXT("/B")) == CTEXT("A/B"),
			"Path.Combine trims boundary separators");

		Expect(context, Path::GetDirectoryName(CTEXT("A/B/C.txt")) == CTEXT("A/B"),
			"Path.GetDirectoryName returns parent path");

		Expect(context, Path::GetFileName(CTEXT("A/B/C.txt")) == CTEXT("C.txt"),
			"Path.GetFileName returns leaf name");

		Expect(context, Path::GetFileNameWithoutExtension(CTEXT("A/B/C.txt")) == CTEXT("C"),
			"Path.GetFileNameWithoutExtension removes extension");

		Expect(context, Path::GetExtension(CTEXT("A/B/C.txt")) == CTEXT(".txt"),
			"Path.GetExtension returns extension");

		Expect(context, Path::GetExtension(CTEXT(".gitignore")).empty(),
			"Path.GetExtension ignores leading dot file");

		Expect(context, Path::GetExtension(CTEXT("file.")).empty(),
			"Path.GetExtension ignores trailing dot");

		Expect(context, Path::GetFileName(CTEXT("folder/")).empty(),
			"Path.GetFileName returns empty for trailing separator");
	}

	void RunTextCodecTests(TestContext& context)
	{
		std::vector<uint8> bytes;

		bool bResult = TextCodec::Encode(
			CTEXT("A\nB"),
			bytes,
			TextEncoding::Utf8,
			LineEnding::LF);

		Expect(context, bResult && BytesEqual(bytes, { 0x41, 0x0A, 0x42 }),
			"TextCodec.Encode UTF-8 LF emits expected bytes");

		bResult = TextCodec::Encode(
			CTEXT("A\nB"),
			bytes,
			TextEncoding::Utf8,
			LineEnding::CRLF);

		Expect(context, bResult && BytesEqual(bytes, { 0x41, 0x0D, 0x0A, 0x42 }),
			"TextCodec.Encode UTF-8 CRLF emits expected bytes");

		CString decodedText;

		bResult = TextCodec::Decode(
			bytes,
			decodedText,
			TextEncoding::Utf8);

		Expect(context, bResult && decodedText == CTEXT("A\r\nB"),
			"TextCodec.Decode preserves CRLF without normalization");

		bResult = TextCodec::Encode(
			CTEXT("A"),
			bytes,
			TextEncoding::Utf8WithBom,
			LineEnding::LF);

		Expect(context, bResult && BytesEqual(bytes, { 0xEF, 0xBB, 0xBF, 0x41 }),
			"TextCodec.Encode UTF-8 BOM emits BOM prefix");

		bResult = TextCodec::Decode(
			bytes,
			decodedText,
			TextEncoding::Utf8WithBom);

		Expect(context, bResult && decodedText == CTEXT("A"),
			"TextCodec.Decode UTF-8 BOM skips BOM prefix");

		bResult = TextCodec::Encode(
			CTEXT("Ascii"),
			bytes,
			TextEncoding::Ansi,
			LineEnding::LF);

		Expect(context, bResult && BytesEqual(bytes, { 0x41, 0x73, 0x63, 0x69, 0x69 }),
			"TextCodec.Encode Ansi accepts 7-bit ASCII");

		bResult = TextCodec::Encode(
			CTEXT("한글"),
			bytes,
			TextEncoding::Ansi,
			LineEnding::LF);

		Expect(context, !bResult && bytes.empty(),
			"TextCodec.Encode Ansi rejects code points above 0x7F");

		const std::vector<uint8> invalidAnsiBytes =
		{
			0x80
		};

		bResult = TextCodec::Decode(
			invalidAnsiBytes,
			decodedText,
			TextEncoding::Ansi);

		Expect(context, !bResult && decodedText.empty(),
			"TextCodec.Decode Ansi rejects bytes above 0x7F");
	}

	void RunFileTests(TestContext& context)
	{
		const CString fixturePath =
			CTEXT("Resources/Test/CoreIo/ReadAllText_UTF8_LF.txt");

		CString fixtureText;

		const bool bFixtureRead = File::ReadAllText(
			fixturePath,
			fixtureText);

		Expect(context, bFixtureRead,
			"File.ReadAllText reads UTF-8 LF fixture");

		if (bFixtureRead)
		{
			const CString expectedFixtureText =
				CTEXT("ReadAllText_UTF8_LFCyphenEngine Core I/O Input\n")
				CTEXT("한글 입력 테스트\n");

			Expect(context, fixtureText == expectedFixtureText,
				"File.ReadAllText fixture text matches expected content");
		}

		const CString tempPath =
			CTEXT("CoreIo_WriteAllText_RoundTrip.tmp");

		const CString roundTripText =
			CTEXT("Core I/O roundtrip\n")
			CTEXT("한글 라운드트립\n");

		const bool bWriteResult = File::WriteAllText(
			tempPath,
			roundTripText,
			TextEncoding::Utf8,
			LineEnding::LF);

		Expect(context, bWriteResult,
			"File.WriteAllText writes UTF-8 LF temp file");

		CString readBackText;

		const bool bReadBackResult = File::ReadAllText(
			tempPath,
			readBackText,
			TextEncoding::Utf8);

		Expect(context, bReadBackResult,
			"File.ReadAllText reads temp file");

		if (bWriteResult && bReadBackResult)
		{
			Expect(context, readBackText == roundTripText,
				"File WriteAllText/ReadAllText roundtrip preserves text");
		}
	}
}

void RunCoreIoTests()
{
	TestContext context;

	WriteTestLine("[CoreIoTests] Begin");

	RunPathTests(context);
	RunTextCodecTests(context);
	RunFileTests(context);

	char summary[128] = {};
	std::snprintf(
		summary,
		sizeof(summary),
		"[CoreIoTests] Summary PASS=%d FAIL=%d",
		context.passCount,
		context.failCount);

	WriteTestLine(summary);
	WriteTestLine("[CoreIoTests] End");
}
