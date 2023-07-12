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
		float* GetBoostValue();
		void SetBoostValue(float newmBoost);
		float* GetExp();
		void SetExp(float newmExp);
		int* GetLevel();
		void SetLevel(int newmLevel);
		float* getUpLimitLife();
		void SetUpLimitLife(float life);
	private:
		float mBoost = 0.0;
		float mCircleRadius;
		float mExp = 0;
		int	  mLevel = 0;
		float upLimitLife = 100;
	};
}

#endif	// 