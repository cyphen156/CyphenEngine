#pragma once

#include <cstring>
#include <limits>
#include <new>
#include <vector>

#include "Modules/Public/Command.h"

// ============================================================================
// CommandBuffer
// ----------------------------------------------------------------------------
// 특정 command enum type으로 고정되는 typed command buffer입니다.
//
// 책임:
//   - command header 작성
//   - payload를 64-bit word 단위로 padding
//   - command count 관리
//   - CommandList view 제공
//   - command type을 컴파일 타임 enum type으로 제한
//
// 비책임:
//   - commandType의 도메인 의미 해석
//   - Renderer / Resource 등 도메인별 payload 타입 정의
//   - Backend 실행
// ============================================================================

template <typename TCommandType>
class CommandBuffer final
{
public:
	CommandBuffer() = default;
	~CommandBuffer() = default;

	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer& operator=(const CommandBuffer& other) = delete;

	CommandBuffer(CommandBuffer&& other) noexcept = default;
	CommandBuffer& operator=(CommandBuffer&& other) noexcept = default;

	void Reset()
	{
		words.clear();
		commandCount = 0;
	}

	bool AppendCommand(TCommandType commandType, const void* payload, uint32 payloadByteSize)
	{
		if (commandType == TCommandType::None)
		{
			return false;
		}

		if (payloadByteSize > 0 && payload == nullptr)
		{
			return false;
		}

		if (commandCount == (std::numeric_limits<uint32>::max)())
		{
			return false;
		}

		const size_t wordByteSize = sizeof(CommandWord);
		const size_t payloadWordCount =
			(static_cast<size_t>(payloadByteSize) + wordByteSize - 1) /
			wordByteSize;

		if (payloadWordCount > static_cast<size_t>((std::numeric_limits<uint32>::max)()))
		{
			return false;
		}

		const size_t appendedWordCount = 1 + payloadWordCount;
		const size_t currentWordCount = words.size();
		const size_t maxWordCount = static_cast<size_t>((std::numeric_limits<uint32>::max)());

		if (currentWordCount > maxWordCount - appendedWordCount)
		{
			return false;
		}

		CommandHeader header = {};
		header.commandType = static_cast<uint32>(commandType);
		header.payloadWordCount = static_cast<uint32>(payloadWordCount);

		const size_t firstCommandWordIndex = currentWordCount;
		const size_t firstPayloadWordIndex = firstCommandWordIndex + 1;

		try
		{
			// Header와 payload 영역을 한 번에 확보합니다.
			// resize가 실패하면 기존 stream은 유지됩니다.
			// 새 payload padding 영역은 0으로 채웁니다.
			words.resize(currentWordCount + appendedWordCount, 0);
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}

		std::memcpy(&words[firstCommandWordIndex], &header, sizeof(header));

		if (payloadWordCount > 0)
		{
			std::memcpy(&words[firstPayloadWordIndex], payload, payloadByteSize);
		}

		++commandCount;

		return true;
	}

	CommandList GetCommandList() const
	{
		CommandList commandList = {};
		commandList.words = words.empty() ? nullptr : words.data();
		commandList.wordCount = static_cast<uint32>(words.size());
		commandList.commandCount = commandCount;

		return commandList;
	}

private:
	std::vector<CommandWord> words;
	uint32 commandCount = 0;
};
