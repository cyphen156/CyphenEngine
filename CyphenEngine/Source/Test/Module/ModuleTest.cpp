#include "pch.h"

#include <cstdio>
#include <cstring>

#include "Modules/Public/ModuleCommand.h"
#include "Modules/Public/ModuleCommandBuffer.h"
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

	ModuleCommandHeader ReadHeader(const ModuleCommandWord* words, uint32 index)
	{
		ModuleCommandHeader header = {};
		std::memcpy(&header, &words[index], sizeof(header));

		return header;
	}

	template<typename PayloadType>
	PayloadType ReadPayload(const ModuleCommandWord* words, uint32 index)
	{
		PayloadType payload = {};
		std::memcpy(&payload, &words[index], sizeof(payload));

		return payload;
	}

	void RunModuleCommandLayoutTests(TestContext& context)
	{
		Expect(context, sizeof(ModuleCommandWord) == 8,
			"ModuleCommandWord is 8 bytes");

		Expect(context, sizeof(ModuleCommandHeader) == 8,
			"ModuleCommandHeader is 8 bytes");

		Expect(context, sizeof(ModuleCommandList) == 16,
			"ModuleCommandList is 16 bytes");
	}

	void RunModuleCommandBufferEmptyTests(TestContext& context)
	{
		ModuleCommandBuffer commandBuffer;

		const ModuleCommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.words == nullptr,
			"ModuleCommandBuffer empty list has null words");

		Expect(context, commandList.wordCount == 0,
			"ModuleCommandBuffer empty list has zero word count");

		Expect(context, commandList.commandCount == 0,
			"ModuleCommandBuffer empty list has zero command count");
	}

	void RunModuleCommandBufferAppendValidationTests(TestContext& context)
	{
		ModuleCommandBuffer commandBuffer;

		Expect(context, !commandBuffer.AppendCommand(
			static_cast<uint32>(TestCommandType::None),
			nullptr,
			0),
			"ModuleCommandBuffer rejects command type zero");

		Expect(context, !commandBuffer.AppendCommand(
			static_cast<uint32>(TestCommandType::OneWordPayload),
			nullptr,
			static_cast<uint32>(sizeof(OneWordPayload))),
			"ModuleCommandBuffer rejects missing payload pointer");

		const ModuleCommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 0 && commandList.commandCount == 0,
			"ModuleCommandBuffer validation failures do not append words");
	}

	void RunModuleCommandBufferNoPayloadTests(TestContext& context)
	{
		ModuleCommandBuffer commandBuffer;

		Expect(context, commandBuffer.AppendCommand(
			static_cast<uint32>(TestCommandType::NoPayload),
			nullptr,
			0),
			"ModuleCommandBuffer appends no-payload command");

		const ModuleCommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.words != nullptr,
			"ModuleCommandBuffer no-payload list has words");

		Expect(context, commandList.wordCount == 1,
			"ModuleCommandBuffer no-payload command uses one word");

		Expect(context, commandList.commandCount == 1,
			"ModuleCommandBuffer no-payload command count is one");

		const ModuleCommandHeader header = ReadHeader(commandList.words, 0);

		Expect(context, header.commandType == static_cast<uint32>(TestCommandType::NoPayload),
			"ModuleCommandBuffer no-payload header stores command type");

		Expect(context, header.payloadWordCount == 0,
			"ModuleCommandBuffer no-payload header stores zero payload word count");
	}

	void RunModuleCommandBufferPayloadTests(TestContext& context)
	{
		ModuleCommandBuffer commandBuffer;

		OneWordPayload oneWordPayload = {};
		oneWordPayload.a = 10;
		oneWordPayload.b = 20;

		Expect(context, commandBuffer.AppendCommand(
			static_cast<uint32>(TestCommandType::OneWordPayload),
			&oneWordPayload,
			static_cast<uint32>(sizeof(oneWordPayload))),
			"ModuleCommandBuffer appends one-word payload command");

		TwoWordPayload twoWordPayload = {};
		twoWordPayload.a = 100;
		twoWordPayload.b = 200;

		Expect(context, commandBuffer.AppendCommand(
			static_cast<uint32>(TestCommandType::TwoWordPayload),
			&twoWordPayload,
			static_cast<uint32>(sizeof(twoWordPayload))),
			"ModuleCommandBuffer appends two-word payload command");

		const ModuleCommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 5,
			"ModuleCommandBuffer payload commands produce expected word count");

		Expect(context, commandList.commandCount == 2,
			"ModuleCommandBuffer payload commands produce expected command count");

		const ModuleCommandHeader firstHeader = ReadHeader(commandList.words, 0);

		Expect(context, firstHeader.commandType == static_cast<uint32>(TestCommandType::OneWordPayload),
			"ModuleCommandBuffer first payload header stores command type");

		Expect(context, firstHeader.payloadWordCount == 1,
			"ModuleCommandBuffer first payload header stores one payload word");

		const OneWordPayload firstPayload =
			ReadPayload<OneWordPayload>(commandList.words, 1);

		Expect(context, firstPayload.a == 10 && firstPayload.b == 20,
			"ModuleCommandBuffer first payload roundtrips");

		const ModuleCommandHeader secondHeader = ReadHeader(commandList.words, 2);

		Expect(context, secondHeader.commandType == static_cast<uint32>(TestCommandType::TwoWordPayload),
			"ModuleCommandBuffer second payload header stores command type");

		Expect(context, secondHeader.payloadWordCount == 2,
			"ModuleCommandBuffer second payload header stores two payload words");

		const TwoWordPayload secondPayload =
			ReadPayload<TwoWordPayload>(commandList.words, 3);

		Expect(context, secondPayload.a == 100 && secondPayload.b == 200,
			"ModuleCommandBuffer second payload roundtrips");
	}

	void RunModuleCommandBufferPaddingTests(TestContext& context)
	{
		ModuleCommandBuffer commandBuffer;

		UnalignedPayload payload = {};
		payload.a = 1;
		payload.b = 2;
		payload.c = 3;

		Expect(context, commandBuffer.AppendCommand(
			static_cast<uint32>(TestCommandType::UnalignedPayload),
			&payload,
			static_cast<uint32>(sizeof(payload))),
			"ModuleCommandBuffer appends unaligned payload command");

		const ModuleCommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 2,
			"ModuleCommandBuffer unaligned payload is padded to one word");

		Expect(context, commandList.commandCount == 1,
			"ModuleCommandBuffer unaligned payload command count is one");

		const ModuleCommandHeader header = ReadHeader(commandList.words, 0);

		Expect(context, header.payloadWordCount == 1,
			"ModuleCommandBuffer unaligned payload header stores padded word count");

		const uint8* payloadBytes =
			reinterpret_cast<const uint8*>(&commandList.words[1]);

		Expect(context,
			payloadBytes[0] == 1 &&
			payloadBytes[1] == 2 &&
			payloadBytes[2] == 3,
			"ModuleCommandBuffer unaligned payload bytes roundtrip");

		Expect(context,
			payloadBytes[3] == 0 &&
			payloadBytes[4] == 0 &&
			payloadBytes[5] == 0 &&
			payloadBytes[6] == 0 &&
			payloadBytes[7] == 0,
			"ModuleCommandBuffer unaligned payload padding is zeroed");
	}

	void RunModuleCommandBufferResetTests(TestContext& context)
	{
		ModuleCommandBuffer commandBuffer;

		OneWordPayload payload = {};
		payload.a = 1;
		payload.b = 2;

		Expect(context, commandBuffer.AppendCommand(
			static_cast<uint32>(TestCommandType::OneWordPayload),
			&payload,
			static_cast<uint32>(sizeof(payload))),
			"ModuleCommandBuffer reset setup append");

		commandBuffer.Reset();

		const ModuleCommandList commandList = commandBuffer.GetCommandList();

		Expect(context, commandList.wordCount == 0,
			"ModuleCommandBuffer reset clears word count");

		Expect(context, commandList.commandCount == 0,
			"ModuleCommandBuffer reset clears command count");
	}
}

void RunModuleTests()
{
	TestContext context;

	WriteTestLineWithNewLine("[ModuleTests] Begin");

	RunModuleCommandLayoutTests(context);
	RunModuleCommandBufferEmptyTests(context);
	RunModuleCommandBufferAppendValidationTests(context);
	RunModuleCommandBufferNoPayloadTests(context);
	RunModuleCommandBufferPayloadTests(context);
	RunModuleCommandBufferPaddingTests(context);
	RunModuleCommandBufferResetTests(context);

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
