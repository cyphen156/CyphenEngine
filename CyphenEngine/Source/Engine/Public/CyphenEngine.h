#pragma once

#include <atomic>

#include "Core/Public/types.h"
#include "Engine/Public/EngineContext.h"

/**
 * 플랫폼 독립 애플리케이션 호스트입니다.
 *
 * CyphenEngine은 GameRuntime 자체가 아니라,
 * 실행 모드에 따라 Runtime / Editor / Modules를 구성하고 실행할
 * 프로세스 단위 최상위 인스턴스입니다.
 *
 * 외부에서는 CyphenEngine 인스턴스를 직접 생성하거나 파괴할 수 없습니다.
 * 생명주기 제어는 Launch 계층에서만 수행합니다.
 */

class Launch;

class CyphenEngine
{
public:
	EngineStatus GetEngineStatus() const;

private:
	friend class Launch;

	CyphenEngine();
	~CyphenEngine();

	CyphenEngine(const CyphenEngine& other) = delete;
	CyphenEngine& operator=(const CyphenEngine& other) = delete;

	CyphenEngine(CyphenEngine&& other) = delete;
	CyphenEngine& operator=(CyphenEngine&& other) = delete;

	bool InitEngine(const LaunchContext& launchContext);
	void Run();
	void ShutdownEngine();

	bool RequestShutdown();
	bool ChangeEngineStatus(EngineStatus expected, EngineStatus desired);

private:
	std::atomic<EngineStatus> engineStatus;
	EngineContext engineContext;
};

extern const CyphenEngine* const GEngine;
