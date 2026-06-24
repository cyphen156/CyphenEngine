#include "pch.h"

#include <cstdio>
#include <cstring>

#include "Modules/Public/Command.h"
#include "Modules/Public/CommandBuffer.h"
#include "Modules/Public/ModuleDescriptor.h"
#include "Modules/Public/ModuleManager.h"

namespace
{
	struct TestContext
	{
		int32 passCount = 0;
		int32 failCount = 0;
	};

	enum class TestCommandType : uint32
	{
		None = 0,
		NoPayload = 1,
		OneWordPayload = 2,
		TwoWordPayload = 3,
		UnalignedPayload = 4
	};

	struct OneWordPayload
	{
		uint32 a = 0;
		uint32 b = 0;
	};

	struct TwoWordPayload
	{
		uint64 a = 0;
		uint64 b = 0;
	};

	struct UnalignedPayload
	{
		uint8 a = 0;
		uint8 b = 0;
		uint8 c = 0;
	};

	static_assert(sizeof(OneWordPayload) == 8, "OneWordPayload must be 8 bytes.");
	static_assert(sizeof(TwoWordPayload) == 16, "TwoWordPayload must be 16 bytes.");
	static_assert(sizeof(UnalignedPayload) == 3, "UnalignedPayload must be 3 bytes.");

	void WriteTestLine(const char* message)
	{
#if PLATFORM_WINDOWS
		OutputDebugStringA(message);
#endif
	}

	void WriteTestLineWithNewLine(const char* message)
	{
#if PLATFORM_WINDOWS
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
#endif
	}

	void Expect(
		TestContext& context,
		bool condition,
		const char* name)
	{
		if (condition)
		{
			++context.passCount;

#if PLATFORM_WINDOWS
			OutputDebugStringA("[PASS] ");
#endif
		}
		else
		{
			++context.failCount;

#if PLATFORM_WINDOWS
			OutputDebugStringA("[FAIL] ");
#endif
		}

		WriteTestLineWithNewLine(name);
	}

	CommandHeader ReadHeader(const CommandWord* words, uint32 index)
	{
		CommandHeader header = {};
		std::memcpy(&header, &words[index], sizeof(header));

		return header;
	}

	template<typename PayloadType>
	PayloadType ReadPayload(const CommandWord* words, uint32 index)
	{
		PayloadType payload = {};
		std::memcpy(&payload, &words[index], sizeof(payload));

		return payload;
	}

	void RunCommandLayoutTests(TestContext& context)
	{
		Expect(context, sizeof(CommandWord) == 8,
			"CommandWord is 8 bytes");

		Expect(context, sizeof(CommandHeader) == 8,
			"CommandHeader is 8 bytes");

		Expect(context, sizeof(CommandList) == 16,
			"CommandList is 16 bytes");
	}

	void RunCommandBufferEmptyTests(TestContext& context)
	{
		CommandBuffer<TestCommandType> commandBuffer;

		const CommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.words == nullptr,
			"CommandBuffer empty list has null words");

		Expect(context, commandList.wordCount == 0,
			"CommandBuffer empty list has zero word count");

		Expect(context, commandList.commandCount == 0,
			"CommandBuffer empty list has zero command count");
	}

	void RunCommandBufferAppendValidationTests(TestContext& context)
	{
		CommandBuffer<TestCommandType> commandBuffer;

		Expect(context, !commandBuffer.AppendCommand(
			(TestCommandType::None),
			nullptr,
			0),
			"CommandBuffer rejects command type zero");

		Expect(context, !commandBuffer.AppendCommand(
			(TestCommandType::OneWordPayload),
			nullptr,
			static_cast<uint32>(sizeof(OneWordPayload))),
			"CommandBuffer rejects missing payload pointer");

		const CommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 0 && commandList.commandCount == 0,
			"CommandBuffer validation failures do not append words");
	}

	void RunCommandBufferNoPayloadTests(TestContext& context)
	{
		CommandBuffer<TestCommandType> commandBuffer;

		Expect(context, commandBuffer.AppendCommand(
			(TestCommandType::NoPayload),
			nullptr,
			0),
			"CommandBuffer appends no-payload command");

		const CommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.words != nullptr,
			"CommandBuffer no-payload list has words");

		Expect(context, commandList.wordCount == 1,
			"CommandBuffer no-payload command uses one word");

		Expect(context, commandList.commandCount == 1,
			"CommandBuffer no-payload command count is one");

		const CommandHeader header = ReadHeader(commandList.words, 0);

		Expect(context, header.commandType == static_cast<uint32>(TestCommandType::NoPayload),
			"CommandBuffer no-payload header stores command type");

		Expect(context, header.payloadWordCount == 0,
			"CommandBuffer no-payload header stores zero payload word count");
	}

	void RunCommandBufferPayloadTests(TestContext& context)
	{
		CommandBuffer<TestCommandType> commandBuffer;

		OneWordPayload oneWordPayload = {};
		oneWordPayload.a = 10;
		oneWordPayload.b = 20;

		Expect(context, commandBuffer.AppendCommand(
			(TestCommandType::OneWordPayload),
			&oneWordPayload,
			static_cast<uint32>(sizeof(oneWordPayload))),
			"CommandBuffer appends one-word payload command");

		TwoWordPayload twoWordPayload = {};
		twoWordPayload.a = 100;
		twoWordPayload.b = 200;

		Expect(context, commandBuffer.AppendCommand(
			(TestCommandType::TwoWordPayload),
			&twoWordPayload,
			static_cast<uint32>(sizeof(twoWordPayload))),
			"CommandBuffer appends two-word payload command");

		const CommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 5,
			"CommandBuffer payload commands produce expected word count");

		Expect(context, commandList.commandCount == 2,
			"CommandBuffer payload commands produce expected command count");

		const CommandHeader firstHeader = ReadHeader(commandList.words, 0);

		Expect(context, firstHeader.commandType == static_cast<uint32>(TestCommandType::OneWordPayload),
			"CommandBuffer first payload header stores command type");

		Expect(context, firstHeader.payloadWordCount == 1,
			"CommandBuffer first payload header stores one payload word");

		const OneWordPayload firstPayload =
			ReadPayload<OneWordPayload>(commandList.words, 1);

		Expect(context, firstPayload.a == 10 && firstPayload.b == 20,
			"CommandBuffer first payload roundtrips");

		const CommandHeader secondHeader = ReadHeader(commandList.words, 2);

		Expect(context, secondHeader.commandType == static_cast<uint32>(TestCommandType::TwoWordPayload),
			"CommandBuffer second payload header stores command type");

		Expect(context, secondHeader.payloadWordCount == 2,
			"CommandBuffer second payload header stores two payload words");

		const TwoWordPayload secondPayload =
			ReadPayload<TwoWordPayload>(commandList.words, 3);

		Expect(context, secondPayload.a == 100 && secondPayload.b == 200,
			"CommandBuffer second payload roundtrips");
	}

	void RunCommandBufferPaddingTests(TestContext& context)
	{
		CommandBuffer<TestCommandType> commandBuffer;

		UnalignedPayload payload = {};
		payload.a = 1;
		payload.b = 2;
		payload.c = 3;

		Expect(context, commandBuffer.AppendCommand(
			(TestCommandType::UnalignedPayload),
			&payload,
			static_cast<uint32>(sizeof(payload))),
			"CommandBuffer appends unaligned payload command");

		const CommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 2,
			"CommandBuffer unaligned payload is padded to one word");

		Expect(context, commandList.commandCount == 1,
			"CommandBuffer unaligned payload command count is one");

		const CommandHeader header = ReadHeader(commandList.words, 0);

		Expect(context, header.payloadWordCount == 1,
			"CommandBuffer unaligned payload header stores padded word count");

		const uint8* payloadBytes =
			reinterpret_cast<const uint8*>(&commandList.words[1]);

		Expect(context,
			payloadBytes[0] == 1 &&
			payloadBytes[1] == 2 &&
			payloadBytes[2] == 3,
			"CommandBuffer unaligned payload bytes roundtrip");

		Expect(context,
			payloadBytes[3] == 0 &&
			payloadBytes[4] == 0 &&
			payloadBytes[5] == 0 &&
			payloadBytes[6] == 0 &&
			payloadBytes[7] == 0,
			"CommandBuffer unaligned payload padding is zeroed");
	}

	void RunCommandBufferResetTests(TestContext& context)
	{
		CommandBuffer<TestCommandType> commandBuffer;

		OneWordPayload payload = {};
		payload.a = 1;
		payload.b = 2;

		Expect(context, commandBuffer.AppendCommand(
			(TestCommandType::OneWordPayload),
			&payload,
			static_cast<uint32>(sizeof(payload))),
			"CommandBuffer reset setup append");

		commandBuffer.Reset();

		const CommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 0,
			"CommandBuffer reset clears word count");

		Expect(context, commandList.commandCount == 0,
			"CommandBuffer reset clears command count");
	}
}

void RunModuleTests()
{
	TestContext context;

	WriteTestLineWithNewLine("[ModuleTests] Begin");

	RunCommandLayoutTests(context);
	RunCommandBufferEmptyTests(context);
	RunCommandBufferAppendValidationTests(context);
	RunCommandBufferNoPayloadTests(context);
	RunCommandBufferPayloadTests(context);
	RunCommandBufferPaddingTests(context);
	RunCommandBufferResetTests(context);

	char summary[128] = {};
	std::snprintf(
		summary,
		sizeof(summary),
		"[ModuleTests] Summary PASS=%d FAIL=%d",
		context.passCount,
		context.failCount);

	WriteTestLineWithNewLine(summary);
	WriteTestLineWithNewLine("[ModuleTests] End");
}
