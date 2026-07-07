# CyphenEngine World Loop (#4 설계)

이 문서는 #4 2D 월드의 **실행 구조 토픽**을 모읍니다. 엔진 전체 흐름은 [Architecture.md](Architecture.md), 규칙 원문은 `CyphenEngine/DevLog/Decisions.txt`의 `CyphenEngine_World_Loop_Rule`입니다. 현재 설계 확정·구현 전 상태입니다.

> 파랑 = Object(OOP, 기존 정의 표면) · 노랑 = System/DOD 실행 계층(#4 신규) · 회색 = 공유 World storage / 프레임 경계.

## 두 실행 체계의 병존

CyphenEngine World는 순수 ECS가 아닙니다. OOP Object를 기본 정의 표면으로 두고, 다수·균일·hot-path 행동만 런타임에 System으로 enroll해 DOD로 실행합니다.

![World Loop 구조: Object(OOP)와 enroll된 System(DOD)이 World ComponentStorage(공유 SSOT)를 사이에 두고 병존하며, 프레임은 BeginFrame부터 EndFrame까지 이름 붙은 phase로 흐른다](Images/world-loop.svg)

## Tick Phase

- `SystemSync` = pending Register/Unregister·enroll/unenroll 확정 **안전 지점**.
- `SystemCompute` = **World/Object 직접 write 금지** (read-only 핸들만 받음).
- `Apply` = System 결과를 공유 storage에 **write-back하는 유일한 지점**.

## 대표 예시 — 투사체 System (Gameplay 계층, 엔진 코어 아님)

다수·균일·hot-path의 전형이라 규칙 전체를 한 바퀴 돕니다.

```
BulletRuntimeEntry            // Gameplay가 정의
    velocity, lifetime, damage    // System 단독 소유 hot data (inline, entry가 SSOT)
    transformIndex                // 공유 Transform storage로의 index (SSOT = World)
```

한 프레임 흐름:

```
ObjectUpdate    총구 Object가 발사 → BulletSystem.Register(spawn) → pending queue (storage 미변경)
SystemSync      pending Register 확정 → Enroll → RuntimeEntry 생성, transformIndex 바인딩
SystemCompute   entry 스트림: lifetime -= dt; pos = transforms[idx] 읽기; pos += velocity*dt
                (World 직접 write 금지 → 이동 결과는 커맨드로 넘김)
Resolve         충돌 판정을 모아 명시 규칙으로 해결 (누가 맞았는지)
Apply           transforms[transformIndex] = pos 반영, 피해 적용 (write-back 유일 지점)
—               lifetime<=0 / 범위 이탈 → Unregister → 다음 SystemSync에서 Unenroll
```

## 갖다쓸 때 주의점 (Tier 2 — 지키지 않으면 사용자 책임)

- 여러 System/Object가 읽는 데이터는 entry에 복사하지 말고 World storage index로 둔다. 애매하면 공유 storage.
- 같은 공유 컴포넌트를 OOP 즉시 write와 System Apply가 함께 쓰지 않는다 (phase write-ownership).
- 엔트리 수가 큰 System은 index를 정렬·조밀하게 유지해 접근 순서를 저장 순서에 맞춘다.

> Tier 1(지연-only Register, read-only compute 핸들, Apply-only write)은 엔진이 API 형태로 강제하므로 위반이 표현되지 않는다. Tier 2는 규약이며 강제하지 않는다.