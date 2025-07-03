#pragma once

#define NOMINMAX

// Define the platform
#if defined(_WIN32)
	#define PLATFORM_WINDOWS 1
#elif defined(__linux__)
	#define PLATFORM_LINUX 1
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
