#pragma once

#include "Runtime/Public/ObjectManager.h"

// ============================================================================
// Object
// ----------------------------------------------------------------------------
// 엔진의 OOP 객체가 공유하는 정체성과 다형적 수명 기반입니다.
//
// 모든 Object는 정적 ObjectManager가 발급한 유효한 ObjectHandle을 생성 시 주입받으며,
// 생존하는 동안 동일한 ObjectHandle을 통해 식별할 수 있습니다.
//
// ObjectHandle은 ObjectManager가 관리하는 OOP Object의 정체성입니다.
// Object와 Entity는 서로 독립된 Handle과 수명 체계를 가집니다.
// Object가 Entity composition에 참여하는 경우 두 정체성은 명시적으로 연결하며,
// Handle 내부 값의 일치만으로 관계를 추론하지 않습니다.
//
// Handle 보유 여부는 객체의 실제 소유 위치를 결정하지 않습니다.
// 동일한 ObjectHandle을 가진 객체 복제와 주소 기반 소유 관계의 변경을
// 방지하기 위해 복사와 이동을 허용하지 않습니다.
//
// Destroy:
//   - 실제 소멸자를 직접 호출하지 않고 ObjectManager에 파괴를 요청합니다.
//   - Object의 구성 관계와 실행 등록은 각 관계의 관리 경계에서 정리합니다.
//   - ObjectManager는 모든 정리가 끝나 수집 가능한 객체만 안전 지점에서 소멸합니다.
//   - Destroy가 호출된 객체를 다시 사용하지 않는 것은 User API 계약입니다.
// ============================================================================

class Object
{
public:
	template<typename ObjectType, typename... ArgumentTypes>
	static ObjectType* NewObject(ArgumentTypes&&... arguments);

	ObjectHandle GetHandle() const;

	void Destroy();

protected:
	explicit Object(ObjectHandle objectHandle);
	virtual ~Object();

private:
	friend class ObjectManager;

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object&&) = delete;

	ObjectHandle handle;
};

template<typename ObjectType, typename... ArgumentTypes>
ObjectType* Object::NewObject(ArgumentTypes&&... arguments)
{
	return ObjectManager::NewObject<ObjectType>(
		std::forward<ArgumentTypes>(arguments)...);
}
