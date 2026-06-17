#include "pch.h"

#include <cstdio>
#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"
#include "Core/Public/File.h"
#include "Core/Public/FileSystem.h"
#include "Core/Public/Path.h"
#include "Core/Public/Separator.h"
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

	bool BytesEqual(const std::vector<uint8>& actual, const std::vector<uint8>& expected)
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

		Expect(context, Separators::Convert(CTEXT("A/B/C"), Separators::Engine, Separators::Windows) == CTEXT("A\\B\\C"),
			"Separators.Convert changes engine separators to Windows separators");
	}

	void RunTextCodecTests(TestContext& context)
	{
		std::vector<uint8> bytes;

		bool result = TextCodec::Encode(
			CTEXT("A\nB"),
			bytes,
			TextEncoding::Utf8,
			LineEnding::LF);

		Expect(context, result && BytesEqual(bytes, { 0x41, 0x0A, 0x42 }),
			"TextCodec.Encode UTF-8 LF emits expected bytes");

		result = TextCodec::Encode(
			CTEXT("A\nB"),
			bytes,
			TextEncoding::Utf8,
			LineEnding::CRLF);

		Expect(context, result && BytesEqual(bytes, { 0x41, 0x0D, 0x0A, 0x42 }),
			"TextCodec.Encode UTF-8 CRLF emits expected bytes");

		result = TextCodec::Encode(
			CTEXT("A\r\nB\nC"),
			bytes,
			TextEncoding::Utf8,
			LineEnding::Preserve);

		Expect(context, result && BytesEqual(bytes, { 0x41, 0x0D, 0x0A, 0x42, 0x0A, 0x43 }),
			"TextCodec.Encode Preserve keeps source line endings");

		CString decodedText;

		result = TextCodec::Decode(bytes, decodedText, TextEncoding::Utf8);

		Expect(context, result && decodedText == CTEXT("A\r\nB\nC"),
			"TextCodec.Decode preserves line endings without normalization");

		result = TextCodec::Encode(
			CTEXT("A"),
			bytes,
			TextEncoding::Utf8WithBom,
			LineEnding::LF);

		Expect(context, result && BytesEqual(bytes, { 0xEF, 0xBB, 0xBF, 0x41 }),
			"TextCodec.Encode UTF-8 BOM emits BOM prefix");

		result = TextCodec::Decode(bytes, decodedText, TextEncoding::Utf8WithBom);

		Expect(context, result && decodedText == CTEXT("A"),
			"TextCodec.Decode UTF-8 BOM skips BOM prefix");

		result = TextCodec::Encode(
			CTEXT("Ascii"),
			bytes,
			TextEncoding::Ansi,
			LineEnding::LF);

		Expect(context, result && BytesEqual(bytes, { 0x41, 0x73, 0x63, 0x69, 0x69 }),
			"TextCodec.Encode Ansi accepts 7-bit ASCII");

		result = TextCodec::Encode(
			CTEXT("한글"),
			bytes,
			TextEncoding::Ansi,
			LineEnding::LF);

		Expect(context, !result && bytes.empty(),
			"TextCodec.Encode Ansi rejects code points above 0x7F");

		const std::vector<uint8> invalidAnsiBytes =
		{
			0x80
		};

		result = TextCodec::Decode(invalidAnsiBytes, decodedText, TextEncoding::Ansi);

		Expect(context, !result && decodedText.empty(),
			"TextCodec.Decode Ansi rejects bytes above 0x7F");
	}

	void RunFileTests(TestContext& context)
	{
		const CString fixturePath =
			CTEXT("Resources/Test/CoreIo/ReadAllText_UTF8_LF.txt");

		CString fixtureText;

		const bool fixtureRead = File::ReadAllText(fixturePath, fixtureText);

		Expect(context, fixtureRead,
			"File.ReadAllText reads UTF-8 LF fixture");

		if (fixtureRead)
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

		const bool writeResult = File::WriteAllText(
			tempPath,
			roundTripText,
			TextEncoding::Utf8,
			LineEnding::LF);

		Expect(context, writeResult,
			"File.WriteAllText writes UTF-8 LF temp file");

		CString readBackText;

		const bool readBackResult = File::ReadAllText(tempPath, readBackText, TextEncoding::Utf8);

		Expect(context, readBackResult,
			"File.ReadAllText reads temp file");

		if (writeResult && readBackResult)
		{
			Expect(context, readBackText == roundTripText,
				"File WriteAllText/ReadAllText roundtrip preserves text");
		}

		const CString appendBytesPath =
			CTEXT("CoreIo_AppendAllBytes.tmp");

		FileSystem::RemoveFile(appendBytesPath);

		Expect(context, File::WriteAllBytes(appendBytesPath, { 0x41 }),
			"File.AppendAllBytes setup writes initial byte");

		Expect(context, File::AppendAllBytes(appendBytesPath, { 0x42, 0x43 }),
			"File.AppendAllBytes appends bytes");

		std::vector<uint8> appendedBytes;

		Expect(context, File::ReadAllBytes(appendBytesPath, appendedBytes) &&
			BytesEqual(appendedBytes, { 0x41, 0x42, 0x43 }),
			"File.AppendAllBytes preserves existing bytes and appends new bytes");

		const CString appendTextPath =
			CTEXT("CoreIo_AppendAllText.tmp");

		FileSystem::RemoveFile(appendTextPath);

		Expect(context, File::WriteAllText(
			appendTextPath,
			CTEXT("A\r\n"),
			TextEncoding::Utf8,
			LineEnding::Preserve),
			"File.AppendAllText setup writes preserved CRLF");

		Expect(context, File::AppendAllText(appendTextPath, CTEXT("B\nC")),
			"File.AppendAllText appends text");

		std::vector<uint8> appendTextBytes;

		Expect(context, File::ReadAllBytes(appendTextPath, appendTextBytes) &&
			BytesEqual(appendTextBytes, { 0x41, 0x0D, 0x0A, 0x42, 0x0A, 0x43 }),
			"File.AppendAllText preserves appended source line endings");

		const CString appendNewTextPath =
			CTEXT("CoreIo_AppendAllText_New.tmp");

		FileSystem::RemoveFile(appendNewTextPath);

		Expect(context, File::AppendAllText(appendNewTextPath, CTEXT("X\nY")),
			"File.AppendAllText creates absent file");

		std::vector<uint8> appendNewTextBytes;

		Expect(context, File::ReadAllBytes(appendNewTextPath, appendNewTextBytes) &&
			BytesEqual(appendNewTextBytes, { 0x58, 0x0A, 0x59 }),
			"File.AppendAllText absent file writes UTF-8 without BOM");
	}

	void CleanupFileSystemTestArtifacts()
	{
		FileSystem::RemoveFile(CTEXT("CoreIo_FileSystem_TempDir/inner.txt"));
		FileSystem::DeleteDirectory(CTEXT("CoreIo_FileSystem_TempDir"));
		FileSystem::RemoveFile(CTEXT("CoreIo_FileSystem_TempFile.txt"));
		FileSystem::RemoveFile(CTEXT("CoreIo_WriteAllText_RoundTrip.tmp"));
		FileSystem::RemoveFile(CTEXT("CoreIo_AppendAllBytes.tmp"));
		FileSystem::RemoveFile(CTEXT("CoreIo_AppendAllText.tmp"));
		FileSystem::RemoveFile(CTEXT("CoreIo_AppendAllText_New.tmp"));
		FileSystem::RemoveFile(CTEXT("CoreIo_FileSystem_CopySource.txt"));
		FileSystem::RemoveFile(CTEXT("CoreIo_FileSystem_CopyTarget.txt"));
		FileSystem::RemoveFile(CTEXT("CoreIo_FileSystem_MoveSource.txt"));
		FileSystem::RemoveFile(CTEXT("CoreIo_FileSystem_MoveTarget.txt"));
		FileSystem::DeleteDirectoryRecursively(CTEXT("CoreIo_FileSystem_Tree"));
	}

	void RunFileSystemTests(TestContext& context)
	{
		const CString tempDirectory =
			CTEXT("CoreIo_FileSystem_TempDir");

		const CString tempFile =
			CTEXT("CoreIo_FileSystem_TempFile.txt");

		const CString innerFile =
			CTEXT("CoreIo_FileSystem_TempDir/inner.txt");

		const std::vector<uint8> bytes =
		{
			0x41
		};

		CleanupFileSystemTestArtifacts();

		Expect(context, FileSystem::MakeDirectory(tempDirectory),
			"FileSystem.MakeDirectory creates directory");

		Expect(context, FileSystem::DirectoryExists(tempDirectory),
			"FileSystem.DirectoryExists true after create");

		Expect(context, FileSystem::MakeDirectory(tempDirectory),
			"FileSystem.MakeDirectory is idempotent");

		Expect(context, FileSystem::DeleteDirectory(tempDirectory),
			"FileSystem.DeleteDirectory removes empty directory");

		Expect(context, !FileSystem::DirectoryExists(tempDirectory),
			"FileSystem.DirectoryExists false after delete");

		Expect(context, !FileSystem::RemoveFile(tempFile),
			"FileSystem.RemoveFile fails for missing file");

		Expect(context, File::WriteAllBytes(tempFile, bytes),
			"FileSystem temp file setup write");

		Expect(context, FileSystem::FileExists(tempFile),
			"FileSystem.FileExists true for file");

		Expect(context, !FileSystem::DirectoryExists(tempFile),
			"FileSystem.DirectoryExists false for file");

		Expect(context, FileSystem::RemoveFile(tempFile),
			"FileSystem.RemoveFile deletes file");

		Expect(context, !FileSystem::FileExists(tempFile),
			"FileSystem.FileExists false after delete");

		Expect(context, FileSystem::MakeDirectory(tempDirectory),
			"FileSystem non-empty directory setup");

		Expect(context, File::WriteAllBytes(innerFile, bytes),
			"FileSystem inner file setup write");

		Expect(context, !FileSystem::DeleteDirectory(tempDirectory),
			"FileSystem.DeleteDirectory fails on non-empty directory");

		Expect(context, FileSystem::RemoveFile(innerFile),
			"FileSystem cleanup inner file");

		Expect(context, FileSystem::DeleteDirectory(tempDirectory),
			"FileSystem cleanup directory");

		Expect(context, FileSystem::Create(tempFile),
			"FileSystem.Create creates a new empty file");

		Expect(context, !FileSystem::Create(tempFile),
			"FileSystem.Create fails when file already exists");

		uint64 fileSize = 1;

		Expect(context, FileSystem::GetSize(tempFile, fileSize) && fileSize == 0,
			"FileSystem.GetSize reports empty file size");

		uint64 missingFileSize = 1;

		Expect(context, !FileSystem::GetSize(CTEXT("CoreIo_FileSystem_Missing.txt"), missingFileSize),
			"FileSystem.GetSize fails for missing file");

		Expect(context, FileSystem::RemoveFile(tempFile),
			"FileSystem.RemoveFile deletes created empty file");

		const CString copySource =
			CTEXT("CoreIo_FileSystem_CopySource.txt");

		const CString copyTarget =
			CTEXT("CoreIo_FileSystem_CopyTarget.txt");

		Expect(context, File::WriteAllBytes(copySource, { 0x41 }),
			"FileSystem.Copy setup writes source");

		Expect(context, FileSystem::Copy(copySource, copyTarget),
			"FileSystem.Copy copies source to missing target");

		Expect(context, !FileSystem::Copy(copySource, copyTarget),
			"FileSystem.Copy without canReplace fails for existing target");

		Expect(context, File::WriteAllBytes(copySource, { 0x42 }) &&
			FileSystem::Copy(copySource, copyTarget, true),
			"FileSystem.Copy with canReplace replaces existing target");

		std::vector<uint8> copiedBytes;

		Expect(context, File::ReadAllBytes(copyTarget, copiedBytes) &&
			BytesEqual(copiedBytes, { 0x42 }),
			"FileSystem.Copy replacement writes source bytes to target");

		Expect(context, FileSystem::FileExists(copySource),
			"FileSystem.Copy preserves source file");

		const CString moveSource =
			CTEXT("CoreIo_FileSystem_MoveSource.txt");

		const CString moveTarget =
			CTEXT("CoreIo_FileSystem_MoveTarget.txt");

		Expect(context, File::WriteAllBytes(moveSource, { 0x43 }),
			"FileSystem.Move setup writes source");

		Expect(context, FileSystem::Move(moveSource, moveTarget),
			"FileSystem.Move moves source to missing target");

		Expect(context, !FileSystem::FileExists(moveSource) &&
			FileSystem::FileExists(moveTarget),
			"FileSystem.Move removes source and creates target");

		const CString treeLeafDirectory =
			CTEXT("CoreIo_FileSystem_Tree/A/B");

		const CString treeLeafFile =
			CTEXT("CoreIo_FileSystem_Tree/A/B/leaf.txt");

		Expect(context, FileSystem::MakeDirectoryTree(treeLeafDirectory),
			"FileSystem.MakeDirectoryTree creates nested directories");

		Expect(context, FileSystem::MakeDirectoryTree(treeLeafDirectory),
			"FileSystem.MakeDirectoryTree is idempotent");

		Expect(context, !FileSystem::GetSize(CTEXT("CoreIo_FileSystem_Tree"), fileSize),
			"FileSystem.GetSize fails for directory");

		Expect(context, File::WriteAllBytes(treeLeafFile, { 0x44 }),
			"FileSystem recursive delete setup writes leaf file");

		Expect(context, !FileSystem::DeleteDirectory(CTEXT("CoreIo_FileSystem_Tree")),
			"FileSystem.DeleteDirectory fails for non-empty tree");

		Expect(context, FileSystem::DeleteDirectoryRecursively(CTEXT("CoreIo_FileSystem_Tree")),
			"FileSystem.DeleteDirectoryRecursively deletes non-empty tree");

		Expect(context, !FileSystem::DirectoryExists(CTEXT("CoreIo_FileSystem_Tree")),
			"FileSystem.DeleteDirectoryRecursively removes root directory");

		CleanupFileSystemTestArtifacts();
	}
}

void RunCoreIoTests()
{
	TestContext context;

	WriteTestLine("[CoreIoTests] Begin");

	RunPathTests(context);
	RunTextCodecTests(context);
	RunFileTests(context);
	RunFileSystemTests(context);

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
