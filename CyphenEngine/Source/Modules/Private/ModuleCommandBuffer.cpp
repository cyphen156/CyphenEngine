#include "pch.h"

#include <cstring>
#include <new>

#include "Modules/Public/ModuleCommandBuffer.h"

void ModuleCommandBuffer::Reset()
{
	words.clear();
	commandCount = 0;
}

bool ModuleCommandBuffer::AppendCommand(uint32 commandType, const void* payload, uint32 payloadByteSize)
{
	if (commandType == 0)
	{
		return false;
	}

	if (payloadByteSize > 0 && payload == nullptr)
	{
		return false;
	}

	if (commandCount == std::numeric_limits<uint32>::max())
	{
		return false;
	}
	const size_t wordByteSize = sizeof(ModuleCommandWord);

	const size_t payloadWordCount =
		(static_cast<size_t>(payloadByteSize) + wordByteSize - 1) /
		wordByteSize;

	if (payloadWordCount > static_cast<size_t>(std::numeric_limits<uint32>::max()))
	{
		return false;
	}
	const size_t appendedWordCount = 1 + payloadWordCount;
	const size_t currentWordCount = words.size();
	const size_t maxWordCount = static_cast<size_t>(std::numeric_limits<uint32>::max());

	if (currentWordCount > maxWordCount - appendedWordCount)
	{
		return false;
	}

	ModuleCommandHeader header = {};
	header.commandType = commandType;
	header.payloadWordCount = static_cast<uint32>(payloadWordCount);

	const size_t firstCommandWordIndex = currentWordCount;
	const size_t firstPayloadWordIndex = firstCommandWordIndex + 1;

	try
	{
		// Header + Payload 영역을 한 번에 확보합니다.
		// resize가 실패하면 기존 stream은 그대로 유지됩니다.
		// 새로 확보된 payload padding 영역은 0으로 채웁니다.
		words.resize(currentWordCount + appendedWordCount, 0);
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}

	std::memcpy(
		&words[firstCommandWordIndex],
		&header,
		sizeof(header));

	if (payloadWordCount > 0)
	{
		std::memcpy(
			&words[firstPayloadWordIndex],
			payload,
			payloadByteSize);
	}

	++commandCount;

	return true;
}

ModuleCommandList ModuleCommandBuffer::GetCommandList() const
{
	ModuleCommandList commandList = {};
	commandList.words = words.empty() ? nullptr : words.data();
	commandList.wordCount = static_cast<uint32>(words.size());
	commandList.commandCount = commandCount;

	return commandList;
}
