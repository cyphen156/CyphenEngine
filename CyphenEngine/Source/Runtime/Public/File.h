#pragma once

#include <vector>

class File
{
public:
	static bool Exists(const TSTRING& path, const TSTRING& fileName);

	static void Create(const TSTRING& path, const TSTRING& fileName);
	static bool Delete(const TSTRING& path, const TSTRING& fileName);

	static bool ReadAll(const TSTRING& path, const TSTRING& fileName, TSTRING& outText);

	// Always override
	static bool WriteAll(const TSTRING& path, const TSTRING& fileName, const TSTRING& text);

	// AddData to File End
	static bool AddText(const TSTRING& path, const TSTRING& fileName, const TSTRING& text);

	static bool ReadAllBytes(const TSTRING& path, const TSTRING& fileName, std::vector<char>& outBytes);

	// Always override
	static bool WriteAllBytes(const TSTRING& path, const TSTRING& fileName, const std::vector<char>& bytes);
};

