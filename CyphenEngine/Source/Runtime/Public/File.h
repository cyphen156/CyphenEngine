#pragma once

#include <vector>

class File
{
public:
	static FILE* OpenFile(const TSTRING& path, const TSTRING& fileName);

	static bool Exists(const TSTRING& path, const TSTRING& fileName);

	static void Create(const TSTRING& path, const TSTRING& fileName);
	static bool Delete(const TSTRING& path, const TSTRING& fileName);

	static bool ReadAll(const TSTRING& path, const TSTRING& fileName, TSTRING& outdata);

	// Always override
	static bool WriteAll(const TSTRING& path, const TSTRING& fileName, const TSTRING& data);

	// AddData to File End
	static bool AppendData(const TSTRING& path, const TSTRING& fileName, const TSTRING& data);

	static bool ReadAllBytes(const TSTRING& path, const TSTRING& fileName, vector<char>& outBytes);

	// Always override
	static bool WriteAllBytes(const TSTRING& path, const TSTRING& fileName, const vector<char>& bytes);
};

