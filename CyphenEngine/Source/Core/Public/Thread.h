#pragma once

#include <system_error>
#include <thread>
#include <utility>

// ============================================================================
// Thread
// ----------------------------------------------------------------------------
// Engine 시스템이 사용하는 공통 Thread primitive입니다.
//
// 책임:
//   - std::thread 생성
//   - Thread 생성 실패를 bool 결과로 변환
//   - Thread join
//   - Native Handle 제공
//
// 비책임:
//   - 시스템별 Loop
//   - 시스템 시작·종료 순서
//   - Thread 이름·우선순위·Affinity 정책
//
// Thread를 사용하는 시스템은 자신의 Loop와 종료 순서를 직접 소유합니다.
// ============================================================================

class Thread final
{
public:
	using NativeHandle = std::thread::native_handle_type;

	Thread() = default;
	~Thread();

	Thread(const Thread& other) = delete;
	Thread& operator=(const Thread& other) = delete;

	Thread(Thread&& other) = delete;
	Thread& operator=(Thread&& other) = delete;

	template<typename Function, typename... Arguments>
	bool Start(Function&& function, Arguments&&... arguments)
	{
		if (nativeThread.joinable())
		{
			return false;
		}

		try
		{
			nativeThread = std::thread(
				std::forward<Function>(function),
				std::forward<Arguments>(arguments)...);
		}
		catch (const std::system_error&)
		{
			return false;
		}

		return true;
	}

	void Join();
	bool IsJoinable() const;

	NativeHandle GetNativeHandle();

private:
	std::thread nativeThread;
};
