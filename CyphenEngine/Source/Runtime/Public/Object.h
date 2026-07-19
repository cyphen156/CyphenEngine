#pragma once

#include <vector>

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
// Object는 Entity와 서로 독립된 Handle과 수명 체계를 가집니다.
// Object가 Entity composition에 참여하는 경우 두 정체성은 명시적으로 연결하며,
// Handle 내부 값의 일치만으로 관계를 추론하지 않습니다.
//
// Object는 하나의 Outer와 여러 SubObject로 구성되는 종속 수명 관계를 가질 수 있습니다.
// Outer/SubObject는 Object의 구조적 포함과 파괴 전파만을 표현합니다.
// World 소속, Transform 공간 계층과 Update 실행 순서는 이 관계로부터
// 추론하거나 자동으로 전파하지 않습니다.
//
// Handle 보유 여부는 객체의 실제 소유 위치를 결정하지 않습니다.
// 동일한 ObjectHandle을 가진 객체 복제와 주소 기반 소유 관계의 변경을
// 방지하기 위해 복사와 이동을 허용하지 않습니다.
//
// Destroy:
//   - 실제 소멸자를 직접 호출하지 않고 ObjectManager에 파괴를 요청합니다.
//   - 구체 Object는 자신에게 귀속된 관계를 먼저 정리한 뒤 공통 파괴 경로에 합류합니다.
//   - 관계 정리나 파괴 요청이 실패하면 false를 반환합니다.
//   - Outer/SubObject 종속 수명은 자식을 부모보다 먼저 정리합니다.
//   - Destroy가 성공한 객체를 다시 사용하지 않는 것은 User API 계약입니다.
// ============================================================================

class Object
{
public:
	template<typename ObjectType, typename... ArgumentTypes>
	static ObjectType* NewObject(ArgumentTypes&&... arguments);

	ObjectHandle GetHandle() const;

	Object* GetOuter();
	const Object* GetOuter() const;

	Object* GetSubObject(uint32 index);
	const Object* GetSubObject(uint32 index) const;
	uint32 GetSubObjectCount() const;

	virtual bool Destroy();

protected:
	explicit Object(ObjectHandle objectHandle);
	virtual ~Object();

	virtual bool CanAttachTo(const Object& outer) const;

	bool AttachSubObject(Object& subObject);
	bool DetachSubObject(Object& subObject);

private:
	friend class ObjectManager;

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object&&) = delete;

	Object* outer = nullptr;
	std::vector<Object*> subObjects;

	ObjectHandle handle;
};

template<typename ObjectType, typename... ArgumentTypes>
ObjectType* Object::NewObject(ArgumentTypes&&... arguments)
{
	return ObjectManager::NewObject<ObjectType>(
		std::forward<ArgumentTypes>(arguments)...);
}
