#pragma once

#define NOMINMAX

class Logger;

// Global Macro
// ============================================================================
// Logging Macro
// ----------------------------------------------------------------------------
// 로그 메시지를 출력할 때 사용하는 매크로입니다.
// 자동으로 파일명, 라인 번호, 함수명을 함께 출력합니다.
//
// 사용 예시:
//   LOG_INTERNAL(LogLevel::Info, "엔진 초기화 완료");
//   LOG_INTERNAL_T(LogLevel::Error, TTEXT("오류 발생: ") + 오류메시지);
//
// - LOG_INTERNAL(level, msg)
//   : ASCII 문자열(리터럴)을 로그로 출력할 때 사용
//
// - LOG_INTERNAL_T(level, tstr)
//   : TSTRING 또는 TTEXT(str) 기반 유니코드 문자열 출력 시 사용
//
// 주의:
//   LOG_INTERNAL_T는 문자열 결합 시 반드시 TSTRING으로 캐스팅해야 합니다.
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

#elif defined(__ANDROID__)	// 빌드환경에서 ANDROID 타겟을 지원해야함
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
