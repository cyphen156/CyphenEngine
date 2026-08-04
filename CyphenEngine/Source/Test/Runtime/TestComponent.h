#pragma once

#include "Runtime/Public/Component.h"
#include "Test/Runtime/TestUpdateCallCounts.h"

class ObjectManager;

// ============================================================================
// TestComponent_*
// ----------------------------------------------------------------------------
// Component를 직접 상속하여 UpdateParticipation의 모든 조합을 검증합니다.
//
// 각 최종 타입은 외부에서 참여 인수를 받지 않습니다.
// 생성자가 자신의 참여 조합을 고정하며 네 실행 함수를 모두 override합니다.
// 선언하지 않은 실행 단계가 잘못 호출된 경우에도 카운터로 검출할 수 있습니다.
// ============================================================================

class TestComponent_None final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_None(ObjectHandle objectHandle);
	~TestComponent_None() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU(ObjectHandle objectHandle);
	~TestComponent_GU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_U final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_U(ObjectHandle objectHandle);
	~TestComponent_U() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_FU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_FU(ObjectHandle objectHandle);
	~TestComponent_FU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GFU(ObjectHandle objectHandle);
	~TestComponent_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU_U final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU_U(ObjectHandle objectHandle);
	~TestComponent_GU_U() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU_FU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU_FU(ObjectHandle objectHandle);
	~TestComponent_GU_FU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU_GFU(ObjectHandle objectHandle);
	~TestComponent_GU_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_U_FU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_U_FU(ObjectHandle objectHandle);
	~TestComponent_U_FU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_U_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_U_GFU(ObjectHandle objectHandle);
	~TestComponent_U_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_FU_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_FU_GFU(ObjectHandle objectHandle);
	~TestComponent_FU_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU_U_FU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU_U_FU(ObjectHandle objectHandle);
	~TestComponent_GU_U_FU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU_U_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU_U_GFU(ObjectHandle objectHandle);
	~TestComponent_GU_U_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU_FU_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU_FU_GFU(ObjectHandle objectHandle);
	~TestComponent_GU_FU_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_U_FU_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_U_FU_GFU(ObjectHandle objectHandle);
	~TestComponent_U_FU_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

class TestComponent_GU_U_FU_GFU final : public Component
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestComponent_GU_U_FU_GFU(ObjectHandle objectHandle);
	~TestComponent_GU_U_FU_GFU() override;

	UpdateFunction CreateUpdateFunction(UpdateParticipation participation) override;

private:
	friend class ObjectManager;

public:
	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;
	void FinalUpdate(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;


private:
	TestUpdateCallCounts updateCallCounts;
};

