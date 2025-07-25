#pragma once

#define NOMINMAX

class Logger;

// Global Macro
// ============================================================================
// Logging Macro
// ----------------------------------------------------------------------------
// �α� �޽����� ����� �� ����ϴ� ��ũ���Դϴ�.
// �ڵ����� ���ϸ�, ���� ��ȣ, �Լ����� �Բ� ����մϴ�.
//
// ��� ����:
//   LOG_INTERNAL(LogLevel::Info, "���� �ʱ�ȭ �Ϸ�");
//   LOG_INTERNAL_T(LogLevel::Error, TTEXT("���� �߻�: ") + �����޽���);
//
// - LOG_INTERNAL(level, msg)
//   : ASCII ���ڿ�(���ͷ�)�� �α׷� ����� �� ���
//
// - LOG_INTERNAL_T(level, tstr)
//   : TSTRING �Ǵ� TTEXT(str) ��� �����ڵ� ���ڿ� ��� �� ���
//
// ����:
//   LOG_INTERNAL_T�� ���ڿ� ���� �� �ݵ�� TSTRING���� ĳ�����ؾ� �մϴ�.
//
// ============================================================================
#define LOG_INTERNAL(level, msg)	Logger::InternalLog(level, TTEXT(msg), __FILE__, __LINE__, __func__)
#define LOG_INTERNAL_T(level, tstr)	Logger::InternalLog(level, tstr, __FILE__, __LINE__, __func__)

// Define the platform
#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS 1
	
// Path
	/**
	* PlatForm == WINDOWS
	* DO NOT USE	"/"
	* Just Use		"\\"
	* TSTRING		wstirng,
	* TCHAR			char
	* TTEXT(str)	L##str
	* TSLASH		L'\\'
	* TSLASH_STR	L"\\"
	*/
	#define TSTRING					wstring
	#define TCHAR					wchar_t
	#define TTEXT(str)				L##str
	#define TSLASH					L'\\'
	#define TSLASH_STR				L"\\"
	#define TO_TSTRING(str)			to_wstring(str)

	// Time
	#define LARGEINTEGER	LARGE_INTEGER

#elif defined(__linux__)
	#define PLATFORM_LINUX 1
/**
* PlatForm == LINUX
* DO NOT USE	"\\"
* Just Use		"/" 
* TSTRING		string
* TCHAR			char
* TTEXT(str)	str
* TSLASH		'/'
* TSLASH_STR	"/"
*/
	#define TSTRING			string
	#define TCHAR			char
	#define TTEXT(str)		str
	#define TSLASH			'/'
	#define TSLASH_STR		"/"

	#define LARGEINTEGER	int64_t

#elif defined(__ANDROID__)	// ����ȯ�濡�� ANDROID Ÿ���� �����ؾ���
	#define PLATFORM_ANDROID 1

#elif defined(__APPLE__) && defined(__MACH__)	
	#define PLATFORM_MAC 1

#else
	#define PLATFORM_INDEPENDENT 1
#endif

// Define Macro for Singletons
#define SINGLE(type)	public:\
							static type* GetInstance()\
							{\
								static type typeInstance;\
								return &typeInstance;\
							}\
						private:\
							type();\
							~type();\
							static type typeInstance; \
							type(const type&) = delete; \
							type& operator=(const type&) = delete; \
							type(type&&) = delete; \
							type& operator=(type&&) = delete;
