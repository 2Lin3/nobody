#pragma once
#ifndef __Nobody_Pawn__
#define __Nobody_Pawn__

#include	<GameObject.h>

namespace Nobody
{
	class Pawn : public GameObject
	{
	public:
		enum class PawnType { Player, AI };

		//! ���캯��
		Pawn(class Game* game, b2World* world, const Vector2& playerPos, PawnType type = PawnType::AI);

		bool isValidPosition(float x, float y, const Vector2& playerPos);
		Vector2 GenerateRandomPosition(const Vector2& playerPos);
		//void SetVelocity(const Vector2& velocity);
		void Update() override;
		void Draw(SDL_Renderer* renderer) override;
		void Drawattackan(SDL_Renderer* renderer, bool onCharging, float timeWidth);
	private:
		void UpdateAI();
		void AttackTarget(int Case, Vector2 pos);
		float mSpeed = 100.0f; // Pawn�ƶ��ٶ�
		float mDetectionRange = 300.0f; // �����ҵķ�Χ
		float mCurrentTime = 0.0f; // ��ǰʱ�䣨����Ϸ��ʼ����
		float mLastChargeTime = -5.0f; // �ϴγ�ײ��ʱ��
		float mScreenBottom = 750; // ��Ļ�ײ���y����
		PawnType mType;
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
	};
}

#endif	// __Nobody_Pawn__
