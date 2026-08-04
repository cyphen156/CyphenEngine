#include "pch.h"

#include "Test/Runtime/TestWorldObject.h"

const TestUpdateCallCounts& TestWorldObject_None::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_None::TestWorldObject_None(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::None)
{
}

TestWorldObject_None::~TestWorldObject_None() = default;

UpdateFunction TestWorldObject_None::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_None::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_None::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_None::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_None::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU::TestWorldObject_GU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate)
{
}

TestWorldObject_GU::~TestWorldObject_GU() = default;

UpdateFunction TestWorldObject_GU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_U::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_U::TestWorldObject_U(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::Update)
{
}

TestWorldObject_U::~TestWorldObject_U() = default;

UpdateFunction TestWorldObject_U::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_U::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_U::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_U::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_U::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_FU::TestWorldObject_FU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::FinalUpdate)
{
}

TestWorldObject_FU::~TestWorldObject_FU() = default;

UpdateFunction TestWorldObject_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GFU::TestWorldObject_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_GFU::~TestWorldObject_GFU() = default;

UpdateFunction TestWorldObject_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU_U::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU_U::TestWorldObject_GU_U(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update)
{
}

TestWorldObject_GU_U::~TestWorldObject_GU_U() = default;

UpdateFunction TestWorldObject_GU_U::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU_U::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU_U::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU_U::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU_U::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU_FU::TestWorldObject_GU_FU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::FinalUpdate)
{
}

TestWorldObject_GU_FU::~TestWorldObject_GU_FU() = default;

UpdateFunction TestWorldObject_GU_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU_GFU::TestWorldObject_GU_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_GU_GFU::~TestWorldObject_GU_GFU() = default;

UpdateFunction TestWorldObject_GU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_U_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_U_FU::TestWorldObject_U_FU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate)
{
}

TestWorldObject_U_FU::~TestWorldObject_U_FU() = default;

UpdateFunction TestWorldObject_U_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_U_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_U_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_U_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_U_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_U_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_U_GFU::TestWorldObject_U_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_U_GFU::~TestWorldObject_U_GFU() = default;

UpdateFunction TestWorldObject_U_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_U_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_U_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_U_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_U_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_FU_GFU::TestWorldObject_FU_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_FU_GFU::~TestWorldObject_FU_GFU() = default;

UpdateFunction TestWorldObject_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU_U_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU_U_FU::TestWorldObject_GU_U_FU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate)
{
}

TestWorldObject_GU_U_FU::~TestWorldObject_GU_U_FU() = default;

UpdateFunction TestWorldObject_GU_U_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU_U_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU_U_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU_U_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU_U_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU_U_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU_U_GFU::TestWorldObject_GU_U_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_GU_U_GFU::~TestWorldObject_GU_U_GFU() = default;

UpdateFunction TestWorldObject_GU_U_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU_U_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU_U_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU_U_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU_U_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU_FU_GFU::TestWorldObject_GU_FU_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_GU_FU_GFU::~TestWorldObject_GU_FU_GFU() = default;

UpdateFunction TestWorldObject_GU_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_U_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_U_FU_GFU::TestWorldObject_U_FU_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_U_FU_GFU::~TestWorldObject_U_FU_GFU() = default;

UpdateFunction TestWorldObject_U_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_U_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_U_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_U_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_U_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestWorldObject_GU_U_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestWorldObject_GU_U_FU_GFU::TestWorldObject_GU_U_FU_GFU(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestWorldObject_GU_U_FU_GFU::~TestWorldObject_GU_U_FU_GFU() = default;

UpdateFunction TestWorldObject_GU_U_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestWorldObject_GU_U_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestWorldObject_GU_U_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestWorldObject_GU_U_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestWorldObject_GU_U_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

