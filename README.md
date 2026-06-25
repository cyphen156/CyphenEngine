# CyphenEngine

CyphenEngine은 개인 엔진 개발 프로젝트입니다.

목표는 거대한 범용 엔진을 복제하는 것이 아니라, 엔진의 핵심 책임을 직접 설계하고 검증하면서 **DOD(Data-Oriented Design), 모듈식 구조, 빌드 타임 추상화**를 중심으로 한 실행 기반을 만드는 것입니다.

이 프로젝트는 Unity보다는 Unreal 쪽의 저수준 통제와 엔진 중심 철학을 더 강하게 참고하지만, Unreal을 그대로 복제하지는 않습니다. 특히 플랫폼 추상화는 런타임 인터페이스보다 빌드 타임 선택을 우선합니다.

## 설계 방향

- **DOD 지향**
	- 데이터 배치, 실행 흐름, 비용 모델을 중요하게 봅니다.
	- 객체 모델은 부정하지 않지만, 성능과 책임 경계를 흐리는 추상화는 피합니다.

- **모듈식 엔진**
	- Core, Platform, Engine, Runtime, Editor, Modules 계층을 분리합니다.
	- 각 계층은 자신의 책임만 갖고, 다른 계층의 정책을 대신 결정하지 않습니다.
	- Renderer는 Engine에 고정된 구현이 아니라, Module ABI와 Backend DLL을 통해 연결되는 방향으로 구축합니다.

- **빌드 타임 추상화**
	- 플랫폼처럼 빌드 시점에 고정 가능한 차이는 빌드 시스템이 선택합니다.
	- 같은 계약의 플랫폼별 concrete 구현을 두고, 타겟 빌드가 필요한 구현만 링크합니다.
	- 이 경우 런타임 인터페이스 / vtable / 가상 디스패치를 기본값으로 두지 않습니다.

- **OOP와 DOD의 공존**
	- 인터페이스와 OOP를 전역 금지하지 않습니다.
	- 에디터 런타임 구성, 게임 런타임 구성, 교체 가능한 상위 시스템처럼 런타임에만 결정되는 영역에서는 OOP를 사용할 수 있습니다.
	- 빌드가 이미 아는 것은 빌드로 고정하고, 런타임에만 아는 것은 런타임 구조로 풉니다.

## 참고하는 방향

CyphenEngine은 다음 방향을 참고합니다.

- Unreal Engine
	- 엔진 중심 구조
	- 플랫폼 계층과 Core 계층 분리
	- 파일 / 리소스 / 런타임 책임의 명확한 경계
	- 저수준 API에 대한 명시적 통제

- Data-Oriented Design
	- 불필요한 간접 참조와 런타임 추상화 비용 회피
	- 데이터 흐름과 실행 비용을 우선하는 설계

- 과거 Skull 프로젝트 경험
	- SubSystem류의 OOP + DOD 혼합 구조는 참고합니다.
	- 반대로 과도한 컨테이너화, 스트림화, 추상화 선행 설계는 경계합니다.

## 핵심 원칙

- Core는 OS API를 직접 호출하지 않습니다.
- Platform 계층은 OS 종속 구현을 담당합니다.
- HAL은 Core와 Platform 사이의 내부 계약입니다.
- Runtime과 Editor는 분리합니다.
- Runtime은 Editor를 알지 못합니다.
- Path는 순수 문자열 유틸리티입니다.
- File은 파일 내용 I/O helper입니다.
- FileSystem은 파일 시스템 네임스페이스 관리 API입니다.
- Renderer는 Frame을 RenderCommand IR로 변환합니다.
- Backend는 RenderCommand / ResourceCommand를 실제 그래픽 API 호출로 변환합니다.
- ResourceManager는 아직 정식화하지 않습니다.
- RuntimePath 같은 정책 경로 계층은 실제 트리거가 올 때 만듭니다.
- 테스트 편의를 위해 미확정 계층을 앞당기지 않습니다.

## 구조 시각화

현재 계층, Renderer 모듈 기초 구조, Command Stream, Texture2D 업로드, Windows / Linux 빌드 경계는 [Docs/Architecture.md](Docs/Architecture.md)에 정리합니다.

## 현재 상태

현재 작업은 Core 기반 정리 이후, Renderer 모듈 기초 구축을 마감하고 Linux 전환 준비 단계로 넘어가는 상태입니다.

완료된 주요 기반은 다음과 같습니다.

- Time 시스템 Core / Platform 분리
- Path 경로 문자열 유틸리티 정리
- File byte/text I/O 정리
- TextCodec 인코딩 변환 정리
- FileSystem 파일 / 디렉터리 관리 API 추가
- Debug 전용 Core I/O 회귀 테스트 추가
- LaunchContext / EngineContext 기반 실행 정보 전달
- ModuleDescriptor / ModuleManager / ModuleLoader 기반 동적 모듈 관리
- Renderer Module ABI와 DX11 Renderer Backend DLL 분리
- Engine Thread -> Render Thread -> Backend DLL 실행 흐름 구성
- Frame -> RenderCommand -> executeCommandList 경로 구성
- ClearRenderTarget / Present command stream 구현
- Content Codec / Resource / Texture2D 기초 경로 추가
- Texture2D 업로드용 ResourceCommand 경로 추가
- ResourceId 기반 Backend texture table 구성
- DrawTexturedQuad와 DX11 fullscreen textured quad 렌더링 구현
- Debug fixture에서 Profile.jpg / Profile2.jpg 교체 표시 확인
- 플랫폼 컴파일 환경과 디버그 출력 경계를 framework.h로 정리
- Linux 빌드 준비용 CMake 경로 추가

자세한 폴더 책임은 `폴더 기능 정리.txt`를 기준으로 관리합니다.

현재 주요 폴더는 다음과 같습니다.

- `Build`
- `Core`
- `HAL`
- `Platform`
- `Engine`
- `Runtime`
- `Editor`
- `Modules`
- `Test`
- `Content`
- `Resource`
- `Resources`
- `DevLog`

## 테스트

Debug 빌드에서 현재 기준선을 검증합니다.

- Core I/O 회귀 테스트
	- Path
	- TextCodec
	- File
	- FileSystem
- Module 테스트
	- ModuleCommand / ModuleCommandBuffer
	- ModuleManager / Renderer Module 계약
- Renderer Debug fixture
	- Profile.jpg / Profile2.jpg 로드
	- Texture2D 업로드
	- TexturedQuad 표시

현재 확인된 기준선은 다음과 같습니다.

- CoreIoTests: `PASS=69 / FAIL=0`
- ModuleTests: `PASS=34 / FAIL=0`
- Debug x64에서 CyphenEngine / CyphenRendererDx11 빌드 성공
- Debug 실행에서 Profile.jpg / Profile2.jpg 1초 단위 교체 렌더링 확인

테스트와 진단 출력은 Debug 기준으로 운용합니다. 디버그 출력은 `PRINT_DEBUG_OUTPUT` 경유로 정리했으며, 플랫폼별 실제 출력 경계는 `framework.h`가 소유합니다.

## 빌드

Windows 프로덕션 빌드는 Visual Studio `.sln` / `.vcxproj` 기반입니다.

Linux 빌드 준비를 위해 CMake 경로를 추가했습니다. CMake는 Linux 빌드 전용으로 운용하며, Windows에서의 CMake 빌드는 Linux 환경 없이 CMake 기술서를 검증하기 위한 프록시 성격입니다.

빌드 산출물은 `BuildArtifacts` 아래에 정리합니다.

- `BuildArtifacts/Binaries/<OS>/<Config>/`
- `BuildArtifacts/Intermediate/<OS>/<Config>/`

현재 #2와 #3의 경계는 "Linux에서 컴파일된다"입니다. 실제 Linux 빌드, Platform/Linux 구현, Linux Renderer Backend, first-light, 통합 테스트는 #3에서 진행합니다.

## 다음 단계

#3에서는 Linux 빌드와 통합 검증을 진행합니다.

- WSL2 / Linux 환경에서 실제 CMake 빌드 수행
- Platform/Linux 구현
	- ModuleLoader: dlopen / dlsym / dlclose
	- PlatformFile: POSIX file descriptor 기반 구현
	- PlatformTime: clock_gettime
	- Linux Launch / main
- OpenGL ES / EGL 기반 Linux Renderer Backend first-light
- Linux JPEG leaf 또는 대체 이미지 decode 경로
- Linux 통합 테스트
- Linux 빌드 가능성 확인 이후 ResourceManager, FrameQueue, Mesh / Material 확장 검토
- Vulkan은 이후 단계로 유지

## 개발 방식

- 작은 단위로 설계하고 검증합니다.
- 구현보다 책임 경계를 먼저 봅니다.
- 트리거가 오기 전까지 계층을 만들지 않습니다.
- 자동화보다 실제 엔진 설계를 우선합니다.
- DevLog는 작업 흐름 단위로 핵심 결정만 압축해 남깁니다.
