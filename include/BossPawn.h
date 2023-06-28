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
			Lurking, // �Ż�����
			Attacking, // ����
			Escaping, // ����
			Deceiving, // ���
			Resting // ��Ϣ
		};
		//! ���캯��
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
		BossPawnState mState;// ��ǰ״̬
		std::vector<GameObject*>enemies;
		int currentAngleDegrees = 0;
		void UpdateAI();
		void AttackTarget(int Case, Vector2 pos);
		float mSpeed = 150.0f; // BossPawn�ƶ��ٶ�
		float mDetectionRange = 300.0f; // �����ҵķ�Χ
		float mCurrentTime = 0.0f; // ��ǰʱ�䣨����Ϸ��ʼ����
		float mLastChargeTime = -5.0f; // �ϴγ�ײ��ʱ��
		float mScreenBottom = 750; // ��Ļ�ײ���y����
		BossPawnType mType;
		float radius = 3.0f;
		float mSmallForceMagnitude = 1000;
		float mLargeForceMagnitude = 30000;
		float mLastPlayerChargeTime; // �ϴγ�ײ��ҵ�ʱ��
		float mLastBottomChargeTime; // �ϴγ�ײ�ײ���ʱ��
		float mLastDownForceTime; // �ϴ�����ʩ������ʱ��;
		bool Charging = false;
		int AttactCase = 0;//��ײ���
		float leftime = 0;
		float timeWidth = 0.4;//��������ʱ��
		float dampv = 0.4;
		float cooltime = 3.0f;//���CD
		Vector2 forceDirection = Vector2::Zero; // ����ʩ�����ķ���
		float forceMagnitude = 0.0f;
		b2Body* mpawn;
	};
}

#endif	// __Nobody_BossPawn__
