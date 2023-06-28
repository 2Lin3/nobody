//! @file Player.h

#include	<Player.h>
#include	<SpriteComponent.h>
#include	<Game.h>
#include	<InputComponent.h>
#include	<MoveComponent.h>
#include	<RigidComponent.h>
#include	<box2d/box2d.h>

namespace Nobody
{
	Player::Player(Game* game, b2World* world):
		GameObject(game,world)
	{
		SpriteComponent* sprite = new SpriteComponent(this, 100);
		sprite->SetTexture(GetGame()->GetTexture("player"));
		SetPosition(Vector2(640, 600));
		InputComponent* inputComponent = new InputComponent(this);
		RigidComponent* rigidComponent = new RigidComponent(this,b2_dynamicBody);
		rigidComponent->SetMass(2);
		b2CircleShape circle;
		mCircleRadius = 2.0f;
		circle.m_radius = mCircleRadius; // ÉèÖÃÔ²µÄ°ë¾¶
		rigidComponent->SetShape(&circle);
		b2Body* mplayer = rigidComponent->GetmBody();
		mplayer->SetLinearDamping(0.8);
		mplayer->SetAngularDamping(0.5);
		float scaleFactorX = (2 * mCircleRadius * 10) / sprite->GetTexWidth();
		float scaleFactorY = (2 * mCircleRadius * 10) / sprite->GetTexHeight();
		SetScale(Vector2(scaleFactorX, scaleFactorY));
		SetLife(10000);
		//MoveComponent* moveComponent = new MoveComponent(this);
		//moveComponent->SetDir(Vector2(mplayer->GetLinearVelocity().x, mplayer->GetLinearVelocity().y));
	}
	float* Player::GetBoostValue() {
		return &mBoost;
	}
	void Player::SetBoostValue(float newmBoost) {
		mBoost = newmBoost;
	}

}