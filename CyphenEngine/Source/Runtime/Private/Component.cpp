#include "pch.h"

#include "Runtime/Public/Component.h"
#include "Runtime/Public/GameObject.h"

void Component::GlobalUpdate(double deltaSeconds)
{
}

void Component::Update(double deltaSeconds)
{
}

void Component::FinalUpdate(double deltaSeconds)
{
}

void Component::GlobalFinalUpdate(double deltaSeconds)
{
}

bool Component::HasUpdateParticipation(UpdateParticipation participation) const
{
	return (static_cast<uint8>(updateParticipation) &
		static_cast<uint8>(participation)) != 0;
}

GameObject* Component::GetOwner()
{
	return static_cast<GameObject*>(GetOuter());
}

const GameObject* Component::GetOwner() const
{
	return static_cast<const GameObject*>(GetOuter());
}

Component::Component(ObjectHandle objectHandle)
	: Component(objectHandle, UpdateParticipation::None)
{
}

Component::Component(ObjectHandle objectHandle, UpdateParticipation updateParticipationValue)
	: Object(objectHandle),
	updateParticipation(updateParticipationValue)
{
}

Component::~Component() = default;

bool Component::CanAttachTo(const Object& outer) const
{
	return dynamic_cast<const GameObject*>(&outer) != nullptr;
}
