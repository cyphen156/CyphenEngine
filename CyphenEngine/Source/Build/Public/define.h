#pragma once

#define NOMINMAX

// Define the platform
#if defined(_WIN32)
	#define PLATFORM_WINDOWS 1
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
	#define TSTRING		wstring
	#define TCHAR		wchar_t
	#define TTEXT(str)	L##str
	#define TSLASH		L'\\'
	#define TSLASH_STR	L"\\"
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
	#define TSTRING		string
	#define TCHAR		char
	#define TTEXT(str)	str
	#define TSLASH		'/'
	#define TSLASH_STR	"/"
#else
	#error "Unsupported Platform"
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
