#include "pch.h"

#include "Test/Runtime/TestComponent.h"

const TestUpdateCallCounts& TestComponent_None::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_None::TestComponent_None(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::None)
{
}

TestComponent_None::~TestComponent_None() = default;

UpdateFunction TestComponent_None::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_None::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_None::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_None::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_None::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU::TestComponent_GU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate)
{
}

TestComponent_GU::~TestComponent_GU() = default;

UpdateFunction TestComponent_GU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_U::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_U::TestComponent_U(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::Update)
{
}

TestComponent_U::~TestComponent_U() = default;

UpdateFunction TestComponent_U::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_U::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_U::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_U::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_U::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_FU::TestComponent_FU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::FinalUpdate)
{
}

TestComponent_FU::~TestComponent_FU() = default;

UpdateFunction TestComponent_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GFU::TestComponent_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_GFU::~TestComponent_GFU() = default;

UpdateFunction TestComponent_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU_U::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU_U::TestComponent_GU_U(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update)
{
}

TestComponent_GU_U::~TestComponent_GU_U() = default;

UpdateFunction TestComponent_GU_U::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU_U::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU_U::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU_U::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU_U::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU_FU::TestComponent_GU_FU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::FinalUpdate)
{
}

TestComponent_GU_FU::~TestComponent_GU_FU() = default;

UpdateFunction TestComponent_GU_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU_GFU::TestComponent_GU_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_GU_GFU::~TestComponent_GU_GFU() = default;

UpdateFunction TestComponent_GU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_U_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_U_FU::TestComponent_U_FU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate)
{
}

TestComponent_U_FU::~TestComponent_U_FU() = default;

UpdateFunction TestComponent_U_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_U_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_U_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_U_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_U_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_U_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_U_GFU::TestComponent_U_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_U_GFU::~TestComponent_U_GFU() = default;

UpdateFunction TestComponent_U_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_U_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_U_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_U_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_U_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_FU_GFU::TestComponent_FU_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_FU_GFU::~TestComponent_FU_GFU() = default;

UpdateFunction TestComponent_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU_U_FU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU_U_FU::TestComponent_GU_U_FU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate)
{
}

TestComponent_GU_U_FU::~TestComponent_GU_U_FU() = default;

UpdateFunction TestComponent_GU_U_FU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU_U_FU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU_U_FU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU_U_FU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU_U_FU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU_U_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU_U_GFU::TestComponent_GU_U_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_GU_U_GFU::~TestComponent_GU_U_GFU() = default;

UpdateFunction TestComponent_GU_U_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU_U_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU_U_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU_U_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU_U_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU_FU_GFU::TestComponent_GU_FU_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_GU_FU_GFU::~TestComponent_GU_FU_GFU() = default;

UpdateFunction TestComponent_GU_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_U_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_U_FU_GFU::TestComponent_U_FU_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_U_FU_GFU::~TestComponent_U_FU_GFU() = default;

UpdateFunction TestComponent_U_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_U_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_U_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_U_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_U_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

const TestUpdateCallCounts& TestComponent_GU_U_FU_GFU::GetUpdateCallCounts() const
{
	return updateCallCounts;
}

TestComponent_GU_U_FU_GFU::TestComponent_GU_U_FU_GFU(ObjectHandle objectHandle)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
			UpdateParticipation::Update |
			UpdateParticipation::FinalUpdate |
			UpdateParticipation::GlobalFinalUpdate)
{
}

TestComponent_GU_U_FU_GFU::~TestComponent_GU_U_FU_GFU() = default;

UpdateFunction TestComponent_GU_U_FU_GFU::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}

void TestComponent_GU_U_FU_GFU::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalUpdate;
}

void TestComponent_GU_U_FU_GFU::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.update;
}

void TestComponent_GU_U_FU_GFU::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.finalUpdate;
}

void TestComponent_GU_U_FU_GFU::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCallCounts.globalFinalUpdate;
}

