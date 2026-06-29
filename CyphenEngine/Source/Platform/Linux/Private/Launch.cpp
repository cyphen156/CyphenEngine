#include "pch.h"

#ifdef _DEBUG
#include "Test/CoreIo/CoreIoTests.h"
#include "Test/Module/ModuleTest.h"
#endif

// CommonHeader
#include "HAL/Public/Launch.h"
#include "Core/Public/Thread.h"
#include "Engine/Public/CyphenEngine.h"
#include "Engine/Public/EngineContext.h"
#include "Modules/Public/ModuleDescriptor.h"
#include "Modules/Public/ModuleManager.h"

// Platform Header
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cstdint>
#include <cstdlib>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

constexpr uint32 DEFAULT_WINDOW_WIDTH = 1280;
constexpr uint32 DEFAULT_WINDOW_HEIGHT = 720;

// 전역 변수
Display* g_display = nullptr;
Window g_hMainWindow = 0;
Atom g_wmDeleteWindow = 0;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다.
bool MyRegisterClass();
Window InitInstance(int showCommand);
void DestroyInstance();
bool WndProc(const XEvent& event);

int main(int argc, char** argv);

class Launch
{
public:
	static const CyphenEngine& GetEngine();

private:
	friend int main(int argc, char** argv);

	static LaunchContext CreateLaunchContext(Window windowHandle);

	static void EngineThreadEntry();
	static bool StartEngineThread(const LaunchContext& launchContext);
	static void RequestEngineShutdown();
	static void JoinEngineThread();

private:
	static CyphenEngine engineInstance;
	static Thread engineThread;
	static int engineThreadExitPipe[2];
};

CyphenEngine Launch::engineInstance;
Thread Launch::engineThread;
int Launch::engineThreadExitPipe[2] = { -1, -1 };

const CyphenEngine* const GEngine = &Launch::GetEngine();

const CyphenEngine& Launch::GetEngine()
{
	return engineInstance;
}

LaunchContext Launch::CreateLaunchContext(Window windowHandle)
{
	LaunchContext launchContext;

	launchContext.windowInfo.nativeWindowHandle =
		reinterpret_cast<void*>(static_cast<std::uintptr_t>(windowHandle));

	launchContext.windowInfo.windowWidth = DEFAULT_WINDOW_WIDTH;
	launchContext.windowInfo.windowHeight = DEFAULT_WINDOW_HEIGHT;

	XWindowAttributes windowAttributes = {};

	if (XGetWindowAttributes(g_display, windowHandle, &windowAttributes) != 0)
	{
		launchContext.windowInfo.windowWidth =
			static_cast<uint32>(windowAttributes.width);

		launchContext.windowInfo.windowHeight =
			static_cast<uint32>(windowAttributes.height);
	}

	// Descriptor 파일과 UserPreference Resolver가 생기기 전까지 사용하는
	// Renderer Module 부트스트랩 구성입니다.
	ModuleDescriptor rendererModule;
	rendererModule.moduleName = CTEXT("Renderer");
	rendererModule.implementationName = CTEXT("Vulkan");
	rendererModule.binaryName = CTEXT("CyphenRendererVulkan");
	rendererModule.isEnabled = true;

	launchContext.moduleDescriptors.push_back(rendererModule);

	return launchContext;
}

void Launch::EngineThreadEntry()
{
	engineInstance.Run();

	if (engineThreadExitPipe[1] != -1)
	{
		const std::uint8_t signalValue = 1;
		ssize_t writeResult = 0;

		do
		{
			writeResult = ::write(
				engineThreadExitPipe[1],
				&signalValue,
				sizeof(signalValue));
		} while (writeResult < 0 && errno == EINTR);

		(void)writeResult;
	}
}

bool Launch::StartEngineThread(const LaunchContext& launchContext)
{
	// Refresh 실패는 유효한 Descriptor까지 폐기하지 않습니다.
	// 개별 시스템이 자신에게 필요한 Module을 Initialize에서 검증합니다.
	ModuleManager::Refresh(launchContext.moduleDescriptors);

	if (engineInstance.InitEngine(launchContext) == false)
	{
		return false;
	}

	if (::pipe(engineThreadExitPipe) != 0)
	{
		engineInstance.ShutdownEngine();

		return false;
	}

	if (::fcntl(engineThreadExitPipe[0], F_SETFD, FD_CLOEXEC) == -1 ||
		::fcntl(engineThreadExitPipe[1], F_SETFD, FD_CLOEXEC) == -1)
	{
		::close(engineThreadExitPipe[0]);
		::close(engineThreadExitPipe[1]);
		engineThreadExitPipe[0] = -1;
		engineThreadExitPipe[1] = -1;

		engineInstance.ShutdownEngine();

		return false;
	}

	if (engineThread.Start(&Launch::EngineThreadEntry) == false)
	{
		::close(engineThreadExitPipe[0]);
		::close(engineThreadExitPipe[1]);
		engineThreadExitPipe[0] = -1;
		engineThreadExitPipe[1] = -1;

		engineInstance.ShutdownEngine();

		return false;
	}

	return true;
}

void Launch::RequestEngineShutdown()
{
	engineInstance.RequestShutdown();
}

void Launch::JoinEngineThread()
{
	engineThread.Join();

	if (engineThreadExitPipe[0] != -1)
	{
		::close(engineThreadExitPipe[0]);
		engineThreadExitPipe[0] = -1;
	}

	if (engineThreadExitPipe[1] != -1)
	{
		::close(engineThreadExitPipe[1]);
		engineThreadExitPipe[1] = -1;
	}

	const bool isModuleShutdownSuccessful =
		ModuleManager::Shutdown();

#ifdef _DEBUG
	_ASSERT(isModuleShutdownSuccessful);
#endif

	(void)isModuleShutdownSuccessful;
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	if (MyRegisterClass() == false)
	{
		return EXIT_FAILURE;
	}

	g_hMainWindow = InitInstance(1);

	if (g_hMainWindow == 0)
	{
		DestroyInstance();

		return EXIT_FAILURE;
	}

#ifdef _DEBUG
	RunCoreIoTests();
#endif

	const LaunchContext launchContext =
		Launch::CreateLaunchContext(g_hMainWindow);

	const bool isEngineStarted =
		Launch::StartEngineThread(launchContext);

	if (isEngineStarted == false)
	{
		ModuleManager::Shutdown();
		DestroyInstance();

		return EXIT_FAILURE;
	}

#ifdef _DEBUG
	RunModuleTests();
#endif

	const int x11ConnectionHandle = ConnectionNumber(g_display);
	const int engineThreadExitHandle = Launch::engineThreadExitPipe[0];

	bool canAcceptMessage = true;

	while (canAcceptMessage)
	{
		while (XPending(g_display) > 0)
		{
			XEvent event = {};
			XNextEvent(g_display, &event);

			if (WndProc(event) == false)
			{
				Launch::RequestEngineShutdown();
				canAcceptMessage = false;

				break;
			}
		}

		if (canAcceptMessage == false)
		{
			break;
		}

		pollfd waitHandles[2] = {};

		waitHandles[0].fd = x11ConnectionHandle;
		waitHandles[0].events = POLLIN;

		waitHandles[1].fd = engineThreadExitHandle;
		waitHandles[1].events = POLLIN;

		const int waitResult = ::poll(waitHandles, 2, -1);

		if (waitResult < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}

			Launch::RequestEngineShutdown();

			break;
		}

		if ((waitHandles[1].revents & (POLLIN | POLLHUP | POLLERR | POLLNVAL)) != 0)
		{
			break;
		}

		if ((waitHandles[0].revents & (POLLHUP | POLLERR | POLLNVAL)) != 0)
		{
			Launch::RequestEngineShutdown();

			break;
		}
	}

	Launch::JoinEngineThread();

	DestroyInstance();

	return EXIT_SUCCESS;
}

bool MyRegisterClass()
{
	if (XInitThreads() == 0)
	{
		return false;
	}

	g_display = XOpenDisplay(nullptr);

	return g_display != nullptr;
}

Window InitInstance(int showCommand)
{
	(void)showCommand;

	const int screen = DefaultScreen(g_display);
	const Window rootWindow = RootWindow(g_display, screen);

	const Window windowHandle = XCreateSimpleWindow(
		g_display,
		rootWindow,
		0,
		0,
		DEFAULT_WINDOW_WIDTH,
		DEFAULT_WINDOW_HEIGHT,
		1,
		BlackPixel(g_display, screen),
		WhitePixel(g_display, screen));

	if (windowHandle == 0)
	{
		return 0;
	}

	XSelectInput(
		g_display,
		windowHandle,
		ExposureMask |
		StructureNotifyMask |
		KeyPressMask |
		KeyReleaseMask);

	g_wmDeleteWindow = XInternAtom(
		g_display,
		"WM_DELETE_WINDOW",
		False);

	if (g_wmDeleteWindow == 0)
	{
		XDestroyWindow(g_display, windowHandle);

		return 0;
	}

	if (XSetWMProtocols(g_display, windowHandle, &g_wmDeleteWindow, 1) == 0)
	{
		XDestroyWindow(g_display, windowHandle);

		return 0;
	}

	XStoreName(g_display, windowHandle, "CyphenEngine");

	XMapWindow(g_display, windowHandle);
	XFlush(g_display);

	return windowHandle;
}

void DestroyInstance()
{
	if (g_display != nullptr && g_hMainWindow != 0)
	{
		XDestroyWindow(g_display, g_hMainWindow);
		g_hMainWindow = 0;
	}

	if (g_display != nullptr)
	{
		XCloseDisplay(g_display);
		g_display = nullptr;
	}

	g_wmDeleteWindow = 0;
}

bool WndProc(const XEvent& event)
{
	switch (event.type)
	{
	case ClientMessage:
		if (static_cast<Atom>(event.xclient.data.l[0]) == g_wmDeleteWindow)
		{
			return false;
		}

		break;

	case ConfigureNotify:
		// 이후 OS event -> Command IR 경로에서 resize event로 넘깁니다.
		break;

	case KeyPress:
	case KeyRelease:
		// 이후 OS event -> Command IR 경로에서 input event로 넘깁니다.
		break;

	default:
		break;
	}

	return true;
}
