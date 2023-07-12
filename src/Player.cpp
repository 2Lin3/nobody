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
		isPlayer = true;
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
		SetLife(30);
		//MoveComponent* moveComponent = new MoveComponent(this);
		//moveComponent->SetDir(Vector2(mplayer->GetLinearVelocity().x, mplayer->GetLinearVelocity().y));
	}
	void Player::Update()
	{
		GameObject::Update();

		if (mExp >= neededExp) {
			mLevel++;
			mExp = mExp - neededExp;
			neededExp = neededExp + 3;
			GetGame()->mIsChoosingProps = true;
		}
	}
	float* Player::GetBoostValue() {
		return &mBoost;
	}
	void Player::SetBoostValue(float newmBoost) {
		mBoost = newmBoost;
	}
	float* Player::GetExp() {
		return &mExp;
	}
	void Player::SetExp(float newmExp) {
		mExp = newmExp;
	}
	int* Player::GetLevel() {
		return &mLevel;
	}
	void Player::SetLevel(int newmLevel) {
		mLevel = newmLevel;
	}
	void Player::SetUpLimitLife(float life) {
		upLimitLife = life;
	}
	float* Player::getUpLimitLife() {
		return &upLimitLife;
	}
	void Player::SetCrit_rate(float rate) {
		Crit_rate = rate;
	}
	float* Player::GetCrit_rate() {
		return &Crit_rate;
	}
	void Player::SetCrit_damage(float damage) {
		Crit_damage = damage;
	}
	float* Player::GetCrit_damage() {
		return &Crit_damage;
	}
	void Player::SetDamage_rate(float rate) {
		Damage_rate = rate;
	}
	float* Player::GetDamage_rate() {
		return &Damage_rate;
	}
	float* Player::GetNeededExp() {
		return &neededExp;
	}
}