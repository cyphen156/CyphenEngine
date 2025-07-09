#include "pch.h"
#include "Common/Public/Logger.h"

#if defined(_DEBUG)
void Logger::Log(const TSTRING& message)
{

}

void Logger::Warning(const TSTRING& message)
{

}

void Logger::Error(const TSTRING& message)
{

}

#elif defined(_RELEASE)
// ������ ���� ���� ���� �� �ƹ��͵� ���ϴ� �Լ�ȣ���ڷ� ���� 
// �����Ϸ��� ȣ�� �����ϵ��� ����
void Logger::Log(const TSTRING& message) {}

void Logger::Warning(const TSTRING& message) {}

void Logger::Error(const TSTRING& message) {}
#endif

void Logger::ForceLog(const TSTRING& message)
{

}

void Logger::ForceWarning(const TSTRING& message)
{

}

void Logger::ForceError(const TSTRING& message)
{

}

void Logger::Warning(const TSTRING& message, const char* file, int line, const char* func)
{

}

void Logger::Error(const TSTRING& message, const char* file, int line, const char* func)
{

}

void Logger::FatalError(const TSTRING& message, const char* file, int line, const char* func)
{
#if defined(PLATFORM_WINDOWS)
	MessageBoxW(nullptr, message.c_str(), L"CyphenEngine Fatal Error", MB_ICONERROR | MB_OK);
#elif defined(PLATFORM_LINUX)

#else
#endif
}
