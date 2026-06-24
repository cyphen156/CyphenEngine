#pragma once

#include <vector>

#include "Modules/Public/ModuleCommand.h"

// ============================================================================
// ModuleCommandBuffer
// ----------------------------------------------------------------------------
// ModuleCommand word stream을 작성하는 공통 버퍼입니다.
//
// 책임:
//   - command header 작성
//   - payload를 64-bit word 단위로 padding
//   - command count 관리
//   - ModuleCommandList view 제공
//
// 비책임:
//   - commandType의 의미 해석
//   - Renderer / Audio 등 도메인별 payload 타입 정의
//   - Backend 실행
// ============================================================================
class ModuleCommandBuffer final
{
public:
	ModuleCommandBuffer() = default;
	~ModuleCommandBuffer() = default;

	ModuleCommandBuffer(const ModuleCommandBuffer& other) = delete;
	ModuleCommandBuffer& operator=(const ModuleCommandBuffer& other) = delete;

	void Reset();

	bool AppendCommand(uint32 commandType, const void* payload, uint32 payloadByteSize);

	ModuleCommandList GetCommandList() const;

private:
	std::vector<ModuleCommandWord> words;
	uint32 commandCount = 0;
};
