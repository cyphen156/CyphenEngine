#include "pch.h"

#include "Engine/Public/CyphenEngine.h"
#include "Core/Public/Time.h"
#include "Runtime/Public/ObjectManager.h"

#ifdef _DEBUG
#include <vector>

#include "Content/Public/Codec.h"
#include "Core/Public/File.h"
#include "Core/Public/FileSystem.h"
#include "Resource/Public/Texture.h"
#include "Modules/Resource/Public/ResourceCommand.h"
#include "Test/Runtime/RuntimeTest.h"

constexpr CChar DebugProfileTexturePath[] = CTEXT("Resources/Thumbnail/Profile.jpg");
constexpr CChar DebugProfile2TexturePath[] = CTEXT("Resources/Thumbnail/Profile2.jpg");
std::vector<ResourceId> debugTexturedQuadResourceIds;

#endif

CyphenEngine::CyphenEngine()
	: engineStatus(Initializing)
{
}

CyphenEngine::~CyphenEngine()
{
}

EngineStatus CyphenEngine::GetEngineStatus() const
{
	return engineStatus.load();
}

bool CyphenEngine::InitEngine(const LaunchContext& launchContext)
{
	if (engineStatus.load() != Initializing)
	{
		return false;
	}

	engineContext.windowInfo = launchContext.mainWindowInfo;

	if (Time::Init() == false)
	{
		return false;
	}

	if (gameRuntime.Initialize() == false)
	{
		return false;
	}

	if (renderer.Initialize(launchContext.nativeRenderContextHandle, engineContext.windowInfo) == false)
	{
		return false;
	}

	engineStatus.store(Ready);

	return true;
}

void CyphenEngine::Run()
{
	if (ChangeEngineStatus(Ready, Running) == false)
	{
		if (engineStatus.load() == Terminating)
		{
			ShutdownEngine();
		}

		return;
	}

#ifdef _DEBUG
#pragma region Debug Resource Bootstrap
	{
		debugTexturedQuadResourceIds.clear();
		World* debugWorld = gameRuntime.CreateWorld();

		if (debugWorld == nullptr)
		{
			PRINT_DEBUG_OUTPUT("[RuntimeTest] World 생성에 실패했습니다.\n");
		}

		ResourceCommandBuffer resourceCommands;
		ResourceId nextResourceId = 1;

		const CChar* fixturePaths[] = {DebugProfileTexturePath, DebugProfile2TexturePath};

		for (const CChar* path : fixturePaths)
		{
			Texture2D texture;
			texture.resourceId = nextResourceId++;

			const ResourceId textureId = texture.resourceId;

			std::vector<uint8> bytes;
			if (FileSystem::FileExists(path) == false ||
				File::ReadAllBytes(path, bytes) == false ||
				Codec::Decode(path, bytes, texture) == false)
			{
				PRINT_DEBUG_OUTPUT(
					"[Resource] texture load failed.\n");

				continue;
			}

			const uint64 pixelBytes = static_cast<uint64>(texture.width) * texture.height * 4;

			if (texture.format != TextureFormat::Rgba8 ||
				texture.width == 0 ||
				texture.height == 0 ||
				static_cast<uint64>(texture.pixels.size()) != pixelBytes)
			{
				PRINT_DEBUG_OUTPUT("[Resource] texture invalid.\n");

				continue;
			}

			Texture2DUploadPayload texturePayload = {};
			texturePayload.format = texture.format;
			texturePayload.width = texture.width;
			texturePayload.height = texture.height;

			UploadResourceCommand command = {};
			command.resourceId = textureId;
			command.resourceKind = ResourceKind::Texture2D;
			command.payloadByteCount = sizeof(texturePayload) + pixelBytes;

			std::vector<uint8> payload(sizeof(command) + static_cast<size_t>(command.payloadByteCount));

			uint8* cursor = payload.data();

			std::memcpy(
				cursor,
				&command,
				sizeof(command));

			cursor += sizeof(command);

			std::memcpy(
				cursor,
				&texturePayload,
				sizeof(texturePayload));

			cursor += sizeof(texturePayload);

			std::memcpy(
				cursor,
				texture.pixels.data(),
				texture.pixels.size());

			if (resourceCommands.AppendCommand(
				ResourceCommandType::UploadResource,
				payload.data(),
				static_cast<uint32>(payload.size())) == false)
			{
				continue;
			}

			debugTexturedQuadResourceIds.push_back(textureId);
		}

		if (renderer.ExecuteDebugResourceCommandList(resourceCommands) == false)
		{
			PRINT_DEBUG_OUTPUT("[Resource] Resource command execution failed.\n");
		}

		if (debugTexturedQuadResourceIds.size() < 2)
		{
			PRINT_DEBUG_OUTPUT("[Resource] Runtime fixture requires two ResourceIds.\n");
		}

		if (debugWorld != nullptr)
		{
			RunRuntimeTests(gameRuntime, *debugWorld, debugTexturedQuadResourceIds);
		}
	}

	double lastEngineLogTime = Time::ElapsedTime();
	uint64 lastEngineLogFrameNumber = 0;
#pragma endregion
#endif

	uint64 frameNumber = 0;

	while (engineStatus.load() == Running)
	{
		Time::Tick();

		// TODO:
		gameRuntime.Tick(Time::DeltaTime());

		// 렌더링을 위한 프레임 생산
		Frame frame = {};
		frame.frameNumber = frameNumber++;
		
#ifdef _DEBUG
		if (debugTexturedQuadResourceIds.empty() == false)
		{
			const uint64 textureIndex =
				static_cast<uint64>(Time::ElapsedTime()) %
				static_cast<uint64>(debugTexturedQuadResourceIds.size());

			TexturedQuadDrawItem drawItem = {};
			drawItem.textureId =
				debugTexturedQuadResourceIds[
					static_cast<size_t>(textureIndex)];

			frame.texturedQuadDrawItems.push_back(drawItem);
		}
#endif

		if (renderer.BeginRenderingFrame(frame) == false)
		{
#ifdef _DEBUG
			PRINT_DEBUG_OUTPUT("[CyphenEngine] BeginRenderingFrame failed.\n");
#endif
			continue;
		}

#ifdef _DEBUG
		//// Debug Log Per 1000 FPS
		//if ((frame.frameNumber % 1000) == 0)
		//{
		//	char message[128] = {};
		//	std::snprintf(
		//		message,
		//		sizeof(message),
		//		"[Renderer] Frame submitted: %llu\n"
		//		"[Time] ElapsedTime: %.6f\n",
		//		static_cast<unsigned long long>(
		//			frame.frameNumber),
		//		Time::ElapsedTime());
		//	PRINT_DEBUG_OUTPUT(message);
		//}

		// Debug Log Per sec(FPS)
		const double currentEngineLogTime = Time::ElapsedTime();
		const double engineLogDeltaTime =
			currentEngineLogTime - lastEngineLogTime;

		if (engineLogDeltaTime >= 1.0)
		{
			const uint64 submittedFrameCount = frameNumber - lastEngineLogFrameNumber;

			const double engineFrameRate = static_cast<double>(submittedFrameCount) / engineLogDeltaTime;

			char message[160] = {};
			std::snprintf(
				message,
				sizeof(message),
				"[Engine] SubmittedFrames=%llu "
				"FPS=%.2f ElapsedTime=%.6f\n",
				static_cast<unsigned long long>(
					submittedFrameCount),
				engineFrameRate,
				currentEngineLogTime);

			PRINT_DEBUG_OUTPUT(message);

			lastEngineLogTime = currentEngineLogTime;
			lastEngineLogFrameNumber = frameNumber;
		}
#endif
	}

	ShutdownEngine();
}

void CyphenEngine::ShutdownEngine()
{
	if (engineStatus.load() == Terminated)
	{
		return;
	}

	// Renderer 내부에서 Render Thread와 GPU 인스턴스를 먼저 종료합니다.
	renderer.Shutdown();

	// GameRuntime에 소속된 GameObject와 모든 World를 파괴합니다.
	gameRuntime.Shutdown();

	ObjectManager::Clear();

	engineStatus.store(Terminated);
}

bool CyphenEngine::RequestShutdown()
{
	EngineStatus currentStatus = engineStatus.load();

	while (currentStatus != Terminating &&
		currentStatus != Terminated)
	{
		if (engineStatus.compare_exchange_strong(currentStatus, Terminating))
		{
			return true;
		}
	}

	return false;
}

bool CyphenEngine::ChangeEngineStatus(EngineStatus expected, EngineStatus desired)
{
	return engineStatus.compare_exchange_strong(expected, desired);
}
