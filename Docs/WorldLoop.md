# CyphenEngine World Loop (#4 실행 경계)

이 문서는 #4 2D 월드의 현재 실행 경계와 후속 System 방향을 정리합니다. 엔진 전체 흐름은 [Architecture.md](Architecture.md), 규칙 원문은 `CyphenEngine/DevLog/Decisions.txt`의 `CyphenEngine_World_Loop_Rule`입니다.

> 파랑 = OOP 실행 경계 · 노랑 = System 실행 경계 · 회색 = World 소속과 공유 상태.

## 두 실행 체계의 병존

CyphenEngine World는 순수 ECS가 아닙니다. OOP Object를 기본 정의 표면으로 두고, 다수·균일·hot-path 행동만 런타임에 System으로 enroll해 DOD로 실행합니다.

현재 #4_12까지 구현된 것은 Object 수명, Runtime과 World 소속, World 생성·파괴와 상위 실행 단계의 경계입니다. Update 참여 Scheduler와 System enroll 저장소는 아직 구현하지 않았습니다.

![Runtime 다섯 단계와 World 세 단계, Object 수명·World 소속·실행 참여를 분리한 현재 World Loop 경계](Images/world-loop.svg)

세 관계는 서로 자동 전파하지 않습니다.

- `Outer / SubObject`: Object의 종속 수명
- `World Join / Leave`: WorldObject의 논리 공간 소속과 Transform 정본
- `Update / System 참여`: 후속 Scheduler가 관리할 실행 관계

## Runtime 다섯 단계

`GameRuntime::Tick`은 다음 순서를 고정합니다.

```
GlobalUpdate
ProcessBeforeWorldTicks
각 World Tick
ProcessAfterWorldTicks
GlobalFinalUpdate
```

`Global`은 프로세스 전역 싱글턴이 아니라 하나의 `GameRuntime`이 소유한 모든 World에 공통인 실행 범위입니다. `ProcessBeforeWorldTicks`와 `ProcessAfterWorldTicks`는 여러 World를 사이에 둔 Runtime-global System 처리 경계입니다.

GameRuntime은 World를 자동 생성하지 않으며 명시적으로 생성한 World를 0개 이상 소유합니다. 소유한 World 수와 관계없이 이 다섯 단계의 상위 순서는 유지합니다.

## World 세 단계

각 `World::Tick`은 다음 순서를 고정합니다.

```
PreUpdate
ProcessAll
FinalUpdate
```

- `PreUpdate`: World-local OOP 선행 실행 경계
- `ProcessAll`: World-local System 실행 경계
- `FinalUpdate`: World-local OOP 후행 실행 경계

현재 함수는 단계와 호출 순서만 보장합니다. 실행 대상 목록, 활성 상태, Scheduler와 `ProcessAll` 내부의 세부 System phase는 실제 소비자가 생기는 후속 작업에서 추가합니다.

## Object와 World의 수명 경계

`ObjectManager`는 ObjectHandle 발급, 활성 Registry와 지연 파괴를 관리합니다. `Object`의 `Outer / SubObject`는 자식을 부모보다 먼저 파괴하기 위한 종속 수명 관계이며 World 소속이나 실행 순서를 뜻하지 않습니다.

`GameObject`는 직접 부착된 Component를 평면 목록으로 관리합니다. Component도 Object이므로 독립 ObjectHandle을 갖지만, GameObject composition에 참여할 때 직접 Outer는 GameObject여야 합니다. Component-in-Component는 허용하지 않습니다.

`WorldObject`는 생성 직후 Runtime과 World 어디에도 속하지 않을 수 있습니다. `GameRuntime::Admit`은 Runtime 소속을 확정하고, `World::Join`은 같은 Runtime에 속한 WorldObject의 비소유 참조와 Transform 정본을 연결합니다. `World::Spawn`은 생성, Runtime Admit과 World Join을 합성하며 복제 API가 아닙니다. `Leave`는 World 연결만 제거하며 Object 자체를 파괴하지 않습니다.

## Handle과 저장 위치

`Handle<Type>`은 관리 도메인 안의 타입 안전한 식별값입니다. `ObjectHandle`과 향후 `EntityHandle`은 서로 독립된 값 공간이며 내부 숫자의 일치로 관계를 추론하지 않습니다.

`StorageSlot<Type>`은 `ComponentDataStorage`의 `index + generation` 저장 위치입니다. Object 정체성과 Transform 저장 위치는 같은 Handle로 표현하지 않습니다.

현재 World storage의 확정 정본은 Join한 WorldObject의 Transform입니다. Sprite 등 다른 ComponentData의 소유 위치는 실제 소비 System이 생길 때 별도로 결정합니다.

## 후속 System 설계 기준

투사체처럼 다수·균일·hot-path인 대상은 다음 방향으로 System화합니다. 아래 이름과 세부 phase는 아직 구현 계약이 아닙니다.

```
OOP Object / Component
    Register 요청

안전 지점
    Enroll
    RuntimeEntry와 ComponentDataStorage 연결

System 실행
    연속 RuntimeEntry / ComponentData 범위 처리

안전 지점
    Unregister 요청 확정
    Unenroll
```

- Register / Unregister는 구조 변경 요청이고 Enroll / Unenroll은 안전 지점의 실제 구조 변경입니다.
- System hot loop는 Object Registry를 반복 조회하지 않고 연속 RuntimeEntry 또는 ComponentData 범위를 처리합니다.
- 여러 실행 주체가 공유하는 데이터는 복사본을 늘리기 전에 SSOT와 write ownership을 먼저 정합니다.
- SystemSync / Compute / Resolve / Apply 같은 세부 단계는 실제 System의 의존성과 데이터 흐름이 요구할 때 `ProcessAll` 안에서 확정합니다.

## #4_12 경계

GameObject의 Runtime Admit과 파괴 이탈, GameRuntime의 명시적 World 소유, WorldObject의 Join / Leave와 `World::Spawn` 합성 경로를 현재 수명·소속 경계로 고정합니다.

GameObject와 Component는 Update 함수와 생성 시점의 UpdateParticipation 계약만 제공합니다. 실행 목록, 활성 상태, Scheduler 등록·해제와 FixedUpdate는 후속 구현합니다.
