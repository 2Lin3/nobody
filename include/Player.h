//! @file Player.h

#ifndef __Nobody_Player__
#define __Nobody_Player__

#include	<GameObject.h>
namespace Nobody
{
	class Player : public GameObject
	{
	public:
		//! 构造函数
		Player(class Game* game, b2World* world);
		void Update() override;

		float* GetBoostValue();
		void SetBoostValue(float newmBoost);
		float* GetExp();
		void SetExp(float newmExp);
		int* GetLevel();
		void SetLevel(int newmLevel);
		float* getUpLimitLife();
		void SetUpLimitLife(float life);
		void SetCrit_rate(float rate);
		float* GetCrit_rate();
		void SetCrit_damage(float damage);
		float* GetCrit_damage();
		void SetDamage_rate(float rate);
		float* GetDamage_rate();
		float* GetNeededExp();
	private:
		float mBoost = 0.0;
		float mCircleRadius;
		float mExp = 0;
		int	  mLevel = 0;
		float upLimitLife = 100;
		float Crit_rate = 0.1;
		float Crit_damage = 1.5;
		float Damage_rate = 0.5;
		float neededExp = 1;
	};
}

#endif	// 