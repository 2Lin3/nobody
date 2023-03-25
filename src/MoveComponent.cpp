//! @file MoveComponent.cpp

#include	<MoveComponent.h>
#include	<GameObject.h>
#include	<Timer.h>
#include	<algorithm>
#include	<Game.h>
#include <iostream>
namespace Nobody
{
	MoveComponent::MoveComponent(GameObject* gameObject) :
		Component(gameObject),
		mSpeed(10),
		friction(0.95 * Timer::deltaTime),
		MaxChange(50),
		mDir(Vector2::Zero)
	{
	}

	void MoveComponent::Update()
	{
		Vector2 curPos = mGameObject->GetPosition();
		Vector2 newPos = curPos + mDir * mSpeed ;

		mGameObject->SetPosition(newPos);
	}

	void MoveComponent::SetDir(const Vector2& dir)
	{
		mDir = dir;
	}
}