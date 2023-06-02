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
			Lurking, // �Ż�����
			Attacking, // ����
			Escaping, // ����
			Deceiving, // ���
			Resting // ��Ϣ
		};
		//! ���캯��
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
		TrickPawnState mState;// ��ǰ״̬

		void UpdateAI();
		void AttackTarget(int Case, Vector2 pos);
		float mSpeed = 100.0f; // TrickPawn�ƶ��ٶ�
		float mDetectionRange = 300.0f; // �����ҵķ�Χ
		float mCurrentTime = 0.0f; // ��ǰʱ�䣨����Ϸ��ʼ����
		float mLastChargeTime = -5.0f; // �ϴγ�ײ��ʱ��
		float mScreenBottom = 750; // ��Ļ�ײ���y����
		TrickPawnType mType;
		float radius = 1.5f;
		float mSmallForceMagnitude = 50;
		float mLargeForceMagnitude = 5000;
		float mLastPlayerChargeTime; // �ϴγ�ײ��ҵ�ʱ��
		float mLastBottomChargeTime; // �ϴγ�ײ�ײ���ʱ��
		float mLastDownForceTime; // �ϴ�����ʩ������ʱ��;
		bool Charging = false;
		int AttactCase = 0;//��ײ���
		float leftime = 0;
		float timeWidth = 0.8;//��������ʱ��
		float dampv = 0.8;
		float cooltime = 5.0f;//���CD
		Vector2 forceDirection = Vector2::Zero; // ����ʩ�����ķ���
		float forceMagnitude = 0.0f;
	};
}

#endif	// __Nobody_TrickPawn__
