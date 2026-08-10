#pragma once

class Object;

using UpdateExecuteFunction = void(*)(Object*, double);

// ============================================================================
// ExecuteUpdateFunction
// ----------------------------------------------------------------------------
// 등록 시 확정된 대상 타입과 virtual 실행 진입점을 연결합니다.
//
// Function Group 실행 중에는 타입을 판단하지 않습니다.
// 저장된 함수 주소로 직접 진입한 뒤 target을 등록 당시 타입으로 복구하여
// virtual 함수를 호출합니다.
//
// 실행 함수는 대상의 활성 상태를 확인하지 않습니다.
// 활성 제어는 후속 Manager의 ActivationPivot 정책에서 처리합니다.
// ============================================================================

template<typename TargetType, void(TargetType::* Function)(double)>
void ExecuteUpdateFunction(Object* target, double deltaSeconds)
{
	TargetType* typedTarget = static_cast<TargetType*>(target);

	(typedTarget->*Function)(deltaSeconds);
}

// ============================================================================
// UpdateFunction
// ----------------------------------------------------------------------------
// UpdateManager 등록과 해제 요청에 사용하는 임시 전달값입니다.
//
// target은 실제 GameObject 또는 Component를 가리킵니다.
// execute는 target의 구체 타입과 실행 단계를 등록 시점에 확정합니다.
//
// UpdateManager는 execute별 Function Group에 target만 분리해 보관합니다.
// 실행 시 Function Group에 저장된 target과 execute를 직접 사용합니다.
// 실행 시 RTTI, kind switch 또는 dynamic_cast를 사용하지 않습니다.
// ============================================================================

struct UpdateFunction
{
	Object* target;
	UpdateExecuteFunction execute;
};
