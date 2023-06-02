#pragma once
#ifndef __Nobody_TrickPawn__
#define __Nobody_TrickPawn__

#include	<GameObject.h>

namespace Nobody
{
	class TrickPawn : public GameObject
	{
	public:
		enum class TrickPawnType { Player, AI };
		enum class TrickPawnState {
			Lurking, // 伺机待发
			Attacking, // 进攻
			Escaping, // 逃亡
			Deceiving, // 诡计
			Resting // 休息
		};
		//! 构造函数
		TrickPawn(class Game* game, b2World* world, const Vector2& playerPos, TrickPawnType type = TrickPawnType::AI);

		bool isValidPosition(float x, float y, const Vector2& playerPos);
		Vector2 GenerateRandomPosition(const Vector2& playerPos);
		//void SetVelocity(const Vector2& velocity);
		void Update() override;
		void Draw(SDL_Renderer* renderer) override;
		void Drawattackan(SDL_Renderer* renderer, bool onCharging, float timeWidth);
		void UpdateState();

		void LurkBehavior();
		void AttackBehavior();
		void EscapeBehavior();
		void DeceiveBehavior();
		void RestBehavior();

	private:
		TrickPawnState mState;// 当前状态

		void UpdateAI();
		void AttackTarget(int Case, Vector2 pos);
		float mSpeed = 100.0f; // TrickPawn移动速度
		float mDetectionRange = 300.0f; // 检测玩家的范围
		float mCurrentTime = 0.0f; // 当前时间（从游戏开始算起）
		float mLastChargeTime = -5.0f; // 上次冲撞的时间
		float mScreenBottom = 750; // 屏幕底部的y坐标
		TrickPawnType mType;
		float radius = 1.5f;
		float mSmallForceMagnitude = 50;
		float mLargeForceMagnitude = 5000;
		float mLastPlayerChargeTime; // 上次冲撞玩家的时间
		float mLastBottomChargeTime; // 上次冲撞底部的时间
		float mLastDownForceTime; // 上次向下施加力的时间;
		bool Charging = false;
		int AttactCase = 0;//碰撞情况
		float leftime = 0;
		float timeWidth = 0.8;//蓄力动画时长
		float dampv = 0.8;
		float cooltime = 5.0f;//冲击CD
		Vector2 forceDirection = Vector2::Zero; // 设置施加力的方向
		float forceMagnitude = 0.0f;
	};
}

#endif	// __Nobody_TrickPawn__
