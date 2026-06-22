#include "pch.h"

#include "Core/Public/Thread.h"

Thread::~Thread()
{
#ifdef _DEBUG
	// 실행 중인 Thread를 암묵적으로 detach하거나 join하지 않습니다.
	// Thread 소유자가 자신의 종료 순서를 통해 먼저 Join해야 합니다.
	_ASSERT(nativeThread.joinable() == false);
#endif
}

void Thread::Join()
{
	if (nativeThread.joinable() == false)
	{
		return;
	}

	try
	{
		nativeThread.join();
	}
	catch (const std::system_error&)
	{
		// Join 실패 후에는 Thread 수명을 안전하게 회수할 수 없습니다.
		std::terminate();
	}
}

bool Thread::IsJoinable() const
{
	return nativeThread.joinable();
}

Thread::NativeHandle Thread::GetNativeHandle()
{
	return nativeThread.native_handle();
}
