#pragma once

#define NOMINMAX

#define SINGLE(type);	public:\
							static type* GetInstance()\
							{\
								static type typeInstance;\
								return &typeInstance;\
							}\
						private:\
							type();\
							~type();
