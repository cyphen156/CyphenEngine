#pragma once

#include "Runtime/Public/WorldObject.h"
#include "Test/Runtime/TestUpdateCallCounts.h"

class ObjectManager;

// ============================================================================
// TestWorldObject_*
// ----------------------------------------------------------------------------
// WorldObject를 직접 상속하여 UpdateParticipation의 모든 조합을 검증합니다.
//
// 각 최종 타입은 외부에서 참여 인수를 받지 않습니다.
// 생성자가 자신의 참여 조합을 고정하며 네 실행 함수를 모두 override합니다.
// 선언하지 않은 실행 단계가 잘못 호출된 경우에도 카운터로 검출할 수 있습니다.
// ============================================================================

class TestWorldObject_None final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_None(ObjectHandle objectHandle);
	~TestWorldObject_None() override;

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

class TestWorldObject_GU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU(ObjectHandle objectHandle);
	~TestWorldObject_GU() override;

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

class TestWorldObject_U final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_U(ObjectHandle objectHandle);
	~TestWorldObject_U() override;

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

class TestWorldObject_FU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_FU(ObjectHandle objectHandle);
	~TestWorldObject_FU() override;

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

class TestWorldObject_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GFU(ObjectHandle objectHandle);
	~TestWorldObject_GFU() override;

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

class TestWorldObject_GU_U final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU_U(ObjectHandle objectHandle);
	~TestWorldObject_GU_U() override;

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

class TestWorldObject_GU_FU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU_FU(ObjectHandle objectHandle);
	~TestWorldObject_GU_FU() override;

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

class TestWorldObject_GU_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU_GFU(ObjectHandle objectHandle);
	~TestWorldObject_GU_GFU() override;

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

class TestWorldObject_U_FU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_U_FU(ObjectHandle objectHandle);
	~TestWorldObject_U_FU() override;

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

class TestWorldObject_U_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_U_GFU(ObjectHandle objectHandle);
	~TestWorldObject_U_GFU() override;

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

class TestWorldObject_FU_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_FU_GFU(ObjectHandle objectHandle);
	~TestWorldObject_FU_GFU() override;

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

class TestWorldObject_GU_U_FU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU_U_FU(ObjectHandle objectHandle);
	~TestWorldObject_GU_U_FU() override;

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

class TestWorldObject_GU_U_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU_U_GFU(ObjectHandle objectHandle);
	~TestWorldObject_GU_U_GFU() override;

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

class TestWorldObject_GU_FU_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU_FU_GFU(ObjectHandle objectHandle);
	~TestWorldObject_GU_FU_GFU() override;

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

class TestWorldObject_U_FU_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_U_FU_GFU(ObjectHandle objectHandle);
	~TestWorldObject_U_FU_GFU() override;

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

class TestWorldObject_GU_U_FU_GFU final : public WorldObject
{
public:
	const TestUpdateCallCounts& GetUpdateCallCounts() const;

protected:
	explicit TestWorldObject_GU_U_FU_GFU(ObjectHandle objectHandle);
	~TestWorldObject_GU_U_FU_GFU() override;

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

