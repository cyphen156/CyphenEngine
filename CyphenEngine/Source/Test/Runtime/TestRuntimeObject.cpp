#include "pch.h"

#include "Test/Runtime/TestRuntimeObject.h"

const TestUpdateCallCounts& TestRuntimeObject_None::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_None::TestRuntimeObject_None(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::None)
{
}

TestRuntimeObject_None::~TestRuntimeObject_None() = default;

UpdateFunction TestRuntimeObject_None::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_None::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_None::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_None::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_None::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU::TestRuntimeObject_GU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate)
{
}

TestRuntimeObject_GU::~TestRuntimeObject_GU() = default;

UpdateFunction TestRuntimeObject_GU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_U::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_U::TestRuntimeObject_U(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::Update)
{
}

TestRuntimeObject_U::~TestRuntimeObject_U() = default;

UpdateFunction TestRuntimeObject_U::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_U::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_U::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_U::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_U::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_FU::TestRuntimeObject_FU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::FinalUpdate)
{
}

TestRuntimeObject_FU::~TestRuntimeObject_FU() = default;

UpdateFunction TestRuntimeObject_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GFU::TestRuntimeObject_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_GFU::~TestRuntimeObject_GFU() = default;

UpdateFunction TestRuntimeObject_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU_U::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU_U::TestRuntimeObject_GU_U(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update)
{
}

TestRuntimeObject_GU_U::~TestRuntimeObject_GU_U() = default;

UpdateFunction TestRuntimeObject_GU_U::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU_U::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU_U::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU_U::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU_U::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU_FU::TestRuntimeObject_GU_FU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::FinalUpdate)
{
}

TestRuntimeObject_GU_FU::~TestRuntimeObject_GU_FU() = default;

UpdateFunction TestRuntimeObject_GU_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU_GFU::TestRuntimeObject_GU_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_GU_GFU::~TestRuntimeObject_GU_GFU() = default;

UpdateFunction TestRuntimeObject_GU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_U_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_U_FU::TestRuntimeObject_U_FU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate)
{
}

TestRuntimeObject_U_FU::~TestRuntimeObject_U_FU() = default;

UpdateFunction TestRuntimeObject_U_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_U_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_U_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_U_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_U_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_U_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_U_GFU::TestRuntimeObject_U_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_U_GFU::~TestRuntimeObject_U_GFU() = default;

UpdateFunction TestRuntimeObject_U_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_U_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_U_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_U_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_U_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_FU_GFU::TestRuntimeObject_FU_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_FU_GFU::~TestRuntimeObject_FU_GFU() = default;

UpdateFunction TestRuntimeObject_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU_U_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU_U_FU::TestRuntimeObject_GU_U_FU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate)
{
}

TestRuntimeObject_GU_U_FU::~TestRuntimeObject_GU_U_FU() = default;

UpdateFunction TestRuntimeObject_GU_U_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU_U_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU_U_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU_U_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU_U_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU_U_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU_U_GFU::TestRuntimeObject_GU_U_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_GU_U_GFU::~TestRuntimeObject_GU_U_GFU() = default;

UpdateFunction TestRuntimeObject_GU_U_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU_U_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU_U_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU_U_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU_U_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU_FU_GFU::TestRuntimeObject_GU_FU_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_GU_FU_GFU::~TestRuntimeObject_GU_FU_GFU() = default;

UpdateFunction TestRuntimeObject_GU_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_U_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_U_FU_GFU::TestRuntimeObject_U_FU_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_U_FU_GFU::~TestRuntimeObject_U_FU_GFU() = default;

UpdateFunction TestRuntimeObject_U_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_U_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_U_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_U_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_U_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestRuntimeObject_GU_U_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestRuntimeObject_GU_U_FU_GFU::TestRuntimeObject_GU_U_FU_GFU(ObjectHandle objectHandle)
	: GameObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestRuntimeObject_GU_U_FU_GFU::~TestRuntimeObject_GU_U_FU_GFU() = default;

UpdateFunction TestRuntimeObject_GU_U_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestRuntimeObject_GU_U_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestRuntimeObject_GU_U_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestRuntimeObject_GU_U_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestRuntimeObject_GU_U_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

