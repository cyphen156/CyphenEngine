#include "pch.h"
#include "Common/Public/Logger.h"
#include "Runtime/Public/Time.h"
#include "Utility/Public/TstringUtility.h"

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

void Logger::InternalLog(LogLevel level, const TSTRING& message, const char* file, int line , const char* func)
{
	TSTRING time = Time::SystemDateTime();
	TSTRING log = TTEXT("[") + time + TTEXT("]\n[") + ENUM_TO_TSTRING(level) + TTEXT("]\n") + message +
		TTEXT(" (") + TstringUtility::CharToTString(file) + TTEXT(":") + TO_TSTRING(line) +
		TTEXT(") <") + TstringUtility::CharToTString(func) + TTEXT(">");

#if defined(PLATFORM_WINDOWS)
	switch (level)
	{
	case LogLevel::Info:
		MessageBoxW(nullptr, log.c_str(), L"CyphenEngine Fatal Error", MB_ICONERROR | MB_OK);
		break;
	case LogLevel::Warning:
		MessageBoxW(nullptr, log.c_str(), L"CyphenEngine Fatal Error", MB_ICONERROR | MB_OK);
		break;
	case LogLevel::Error:
		MessageBoxW(nullptr, log.c_str(), L"CyphenEngine Fatal Error", MB_ICONERROR | MB_OK);
		break;
	case LogLevel::Fatal:
		MessageBoxW(nullptr, log.c_str(), L"CyphenEngine Fatal Error", MB_ICONERROR | MB_OK);
		break;
	default:
		break;
	}
#elif defined(PLATFORM_LINUX)

#else
#endif
}