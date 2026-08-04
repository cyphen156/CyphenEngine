#pragma once

#include "Runtime/Public/GameObject.h"
#include "Test/Runtime/TestUpdateCallCounts.h"

class ObjectManager;

// ============================================================================
// TestRuntimeObject_*
// ----------------------------------------------------------------------------
// GameObject를 직접 상속하여 UpdateParticipation의 모든 조합을 검증합니다.
//
// 각 최종 타입은 외부에서 참여 인수를 받지 않습니다.
// 생성자가 자신의 참여 조합을 고정하며 네 실행 함수를 모두 override합니다.
// 선언하지 않은 실행 단계가 잘못 호출된 경우에도 카운터로 검출할 수 있습니다.
// ============================================================================

class TestRuntimeObject_None final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_None(ObjectHandle objectHandle);
	~TestRuntimeObject_None() override;

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

class TestRuntimeObject_GU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU(ObjectHandle objectHandle);
	~TestRuntimeObject_GU() override;

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

class TestRuntimeObject_U final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_U(ObjectHandle objectHandle);
	~TestRuntimeObject_U() override;

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

class TestRuntimeObject_FU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_FU(ObjectHandle objectHandle);
	~TestRuntimeObject_FU() override;

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

class TestRuntimeObject_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_GFU() override;

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

class TestRuntimeObject_GU_U final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU_U(ObjectHandle objectHandle);
	~TestRuntimeObject_GU_U() override;

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

class TestRuntimeObject_GU_FU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU_FU(ObjectHandle objectHandle);
	~TestRuntimeObject_GU_FU() override;

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

class TestRuntimeObject_GU_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_GU_GFU() override;

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

class TestRuntimeObject_U_FU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_U_FU(ObjectHandle objectHandle);
	~TestRuntimeObject_U_FU() override;

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

class TestRuntimeObject_U_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_U_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_U_GFU() override;

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

class TestRuntimeObject_FU_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_FU_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_FU_GFU() override;

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

class TestRuntimeObject_GU_U_FU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU_U_FU(ObjectHandle objectHandle);
	~TestRuntimeObject_GU_U_FU() override;

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

class TestRuntimeObject_GU_U_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU_U_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_GU_U_GFU() override;

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

class TestRuntimeObject_GU_FU_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU_FU_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_GU_FU_GFU() override;

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

class TestRuntimeObject_U_FU_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_U_FU_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_U_FU_GFU() override;

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

class TestRuntimeObject_GU_U_FU_GFU final : public GameObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestRuntimeObject_GU_U_FU_GFU(ObjectHandle objectHandle);
	~TestRuntimeObject_GU_U_FU_GFU() override;

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

