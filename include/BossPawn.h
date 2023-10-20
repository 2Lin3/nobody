#pragma once
#ifndef __Nobody_BossPawn__
#define __Nobody_BossPawn__

#include	<GameObject.h>

namespace Nobody
{
	class BossPawn : public GameObject
	{
	public:
		enum class BossPawnType { Player, AI };
		enum class BossPawnState {
			Lurking, // 伺机待发
			Attacking, // 进攻
			Escaping, // 逃亡
			Deceiving, // 诡计
			Resting // 休息
		};
		//! 构造函数
		BossPawn(class Game* game, b2World* world, const Vector2& GeneratePos, const std::vector<GameObject*>& enemies, BossPawnType type = BossPawnType::AI);

		bool isValidPosition(float x, float y, const Vector2& GeneratePos);
		Vector2 GenerateRandomPosition(const Vector2& GeneratePos);
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
		void DrawSector(SDL_Renderer* renderer, int x, int y, int radius, int start_angle, int end_angle)
		{
			for (int i = start_angle; i < end_angle; ++i)
			{
				int dx = radius * -cos(i * M_PI / 180.0);
				int dy = radius * -sin(i * M_PI / 180.0);
				SDL_RenderDrawLine(renderer, x, y, x + dx, y + dy);
			}
		}

	private:
		BossPawnState mState;// 当前状态
		std::vector<GameObject*>enemies;
		int currentAngleDegrees = 0;
		void UpdateAI();
		void AttackTarget(int Case, Vector2 pos);
		float mSpeed = 150.0f; // BossPawn移动速度
		float mDetectionRange = 300.0f;  // 检测玩家的范围
		float mCurrentTime = 0.0f; // 当前时间（从游戏开始算起）
		float mLastChargeTime = -5.0f; // 上次冲撞的时间
		float mScreenBottom = 750; // 屏幕底部的y坐标
		BossPawnType mType;
		float radius = 3.0f;
		float mSmallForceMagnitude = 1000;
		float mLargeForceMagnitude = 30000;
		float mLastPlayerChargeTime; // 上次冲撞玩家的时间
		float mLastBottomChargeTime; // 上次冲撞底部的时间
		float mLastDownForceTime; // 上次向下施加力的时间
		bool Charging = false;
		int AttactCase = 0;//碰撞情况
		float leftime = 0;
		float timeWidth = 0.4;//蓄力动画时长
		float dampv = 0.4;
		float cooltime = 3.0f;//冲击CD
		Vector2 forceDirection = Vector2::Zero; // 设置施加力的方向
		float forceMagnitude = 0.0f;
		b2Body* mpawn;
	};
}

#endif	// __Nobody_BossPawn__
