#pragma once

#define NOMINMAX

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
