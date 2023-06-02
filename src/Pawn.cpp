#include	<Pawn.h>
#include	<SpriteComponent.h>
#include	<Game.h>
#include	<InputComponent.h>
#include	<MoveComponent.h>
#include	<RigidComponent.h>
#include	<box2d/box2d.h>
#include	<Player.h>
#include	<random>
#include	<iostream>
namespace Nobody
{
	Pawn::Pawn(Game* game, b2World* world, const Vector2& playerPos, PawnType type) :
		GameObject(game, world),
		mType(type),
        mLastPlayerChargeTime(0.0f),
        mLastBottomChargeTime(0.0f),
        mLastDownForceTime(0.0f)
	{
		Vector2 pos = GenerateRandomPosition(playerPos);
		SetPosition(playerPos);
		InputComponent* inputComponent = new InputComponent(this);
		RigidComponent* rigidComponent = new RigidComponent(this, b2_dynamicBody);
		rigidComponent->SetMass(0.5);
		b2CircleShape circle;
		circle.m_radius = radius; // ����Բ�İ뾶
		rigidComponent->SetShape(&circle);
		b2Body* mpawn = rigidComponent->GetmBody();
		mpawn->SetLinearDamping(dampv);
		SetLife(20);

	}

	// �������λ��
	Vector2 Pawn::GenerateRandomPosition(const Vector2& playerPos) {
		// ���������������
		std::mt19937 generator(time(0));
		std::uniform_real_distribution<float> distribution(-100.0f, 100.0f);	
		float x, y;
		do {
			x = playerPos.x + distribution(generator);
			y = playerPos.y + distribution(generator);
		} while (!isValidPosition(x, y, playerPos));

		return Vector2(x, y);
	}
	// �жϾ����Ƿ����100
	bool Pawn::isValidPosition(float x, float y, const Vector2& playerPos) {
		return std::sqrt(std::pow(x - playerPos.x, 2) + std::pow(y - playerPos.y, 2)) > 100.0f;
	}
	void Pawn::Update()
	{
		GameObject::Update();
        if (AttactCase != 0) {

        }
		if (mType == PawnType::AI)
		{
			UpdateAI();
		}
	}
    // �ƶ�����ƣ��ֵ��ac����
    //1.������λ�ã�����㹻��
        //�����ײ����cd�У���¼��ǰ���λ�ã���ɫ�����Ч��Ȼ��Ը�λ�ó�ײ 
        //�����cd�У������ѡ�� һ��Զ�����λ�� �����ӽ���������ң��Ż��ȴ���������
        //�������㹻Զ�������·��н����������ײ����룬����㹻�����ײ�ײ�
    //2.���ͬ��λ�ã�ֻ��Ҫ��֤�ǳ�ײʱ��������ײ������һ�����룩
	void Pawn::UpdateAI()
	{
            // ��ȡ���λ��
            Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

            // ʹ�� Game ���е� GetDeltaTime() ����
            float deltaTime = GetGame()->GetDeltaTime();

            // ���µ�ǰʱ��
            mCurrentTime += deltaTime;

            // ��ȡ Pawn ��ǰλ��
            Vector2 currentPosition = GetPosition();

            // ���� Pawn ����ҵľ���
            float distanceToPlayer = currentPosition.Distance(playerPos);

            // ��ȡ Pawn �ĸ��������RigidComponent��
            RigidComponent* rigidComponent = GetComponent<RigidComponent>();
            if (rigidComponent == nullptr)
            {
                return;
            }

            // ��ȡ Pawn �ĸ��壨b2Body��
            b2Body* body = rigidComponent->GetmBody();

            // ����ʩ�����ķ���
            Vector2 forceDirection = Vector2::Zero;
            float forceMagnitude = 0.0f;

            // ����ϴγ�ײʱ���Ƿ����5��
            float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
            if (distanceToPlayer <= mDetectionRange && timeSinceLastPlayerCharge >= 5.0f)
            {
                Charging = true;
                // �ڼ�ⷶΧ�ڣ�������ƶ������Գ�ײ
                forceDirection = (playerPos - currentPosition).Normalized();
                body->SetLinearDamping(dampv * 5);
                if (timeSinceLastPlayerCharge >= cooltime + timeWidth) {
                    body->SetLinearDamping(dampv);
                    forceMagnitude = mLargeForceMagnitude;
                }
                if (timeSinceLastPlayerCharge >= cooltime + 0.2 + timeWidth) {
                    // �����ϴγ�ײʱ��
                    body->SetLinearDamping(dampv);
                    mLastPlayerChargeTime = mCurrentTime;

                }
            }
            else
            {
                // ����ϴγ�ײʱ���Ƿ����3��
                float timeSinceLastBottomCharge = mCurrentTime - mLastBottomChargeTime;
                if (currentPosition.y <= mScreenBottom && timeSinceLastBottomCharge >= 3.0f)
                {
                    // ������Ļ�ײ�����ײ�ײ�
                    forceDirection = Vector2(0, 1);
                    forceMagnitude = mLargeForceMagnitude;

                    // �����ϴγ�ײʱ��
                    mLastBottomChargeTime = mCurrentTime;
                }
                else
                {
                    // ����ϴγ��������ƽ���ʱ���Ƿ����3��
                    float timeSinceLastDownForce = mCurrentTime - mLastDownForceTime;
                    if (timeSinceLastDownForce >= 3.0f)
                    {
                        // ���³�����һ��С���ƽ�
                        forceDirection = Vector2(0, 1);
                        forceMagnitude = mSmallForceMagnitude;
                        if (timeSinceLastDownForce >= 4.0f)
                        {
                            // �����ϴ������ƽ���ʱ��
                            mLastDownForceTime = mCurrentTime;
                        }
                    }
                    else
                    {
                        // ���³�����һ��С���ƽ�
                        forceDirection = Vector2(0, 1);
                        forceMagnitude = mSmallForceMagnitude;
                    }
                }
            }

            // �����������������Ĵ�С�Եõ�ʵ��ʩ�ӵ���
            b2Vec2 force = b2Vec2(forceDirection.x * forceMagnitude, forceDirection.y * forceMagnitude);

            // Ӧ����
            body->ApplyForceToCenter(force, true);
    }

    void Pawn::Draw(SDL_Renderer* renderer) {
        //������ײ����
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // ���ú�ɫ
        SDL_RenderDrawCircle(renderer, GetPosition().x, GetPosition().y, 15);

        int numRings = 5; // Բ������
        int ringWidth = 2; // Բ����ȣ�������Ϊ��λ��
        int initialRadius = 10; // �����Բ���İ뾶

        SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255); // ���õ���ɫ

        for (int i = 0; i < numRings; ++i)
        {
            int radius = initialRadius + i * ringWidth;
            SDL_RenderDrawCircle(renderer, GetPosition().x, GetPosition().y, radius);
        }
        Drawattackan(renderer, Charging, timeWidth);
        // ��������ֵ��
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // ���ð�ɫ
        SDL_Rect enemyLife = {
            static_cast<int>(GetPosition().x - GetLife() / 2),
            static_cast<int>(GetPosition().y - 30),
            static_cast<int>(GetLife()),
            3
        };
        SDL_RenderFillRect(renderer, &enemyLife);
    }
    void Pawn::Drawattackan(SDL_Renderer* renderer, bool onCharging,float timeWidth) {
        if (onCharging && timeWidth > 0.0f) {
            // ��ȡ Pawn ��ǰλ��
            Vector2 position = GetPosition();

            // ʹ�� Game ���е� GetDeltaTime() ����
            float deltaTime = GetGame()->GetDeltaTime();

            // ��ȡ�ӿ�ʼ���������ڵ�ʱ��
            leftime += deltaTime;
            float elapsedTime = timeWidth - leftime;
            if (elapsedTime < 0) {
                Charging = false;
                leftime = 0;
            }
            // ����Բ���İ뾶
            float chargeRadius = std::min(10 * radius, radius * (10 * elapsedTime / timeWidth));
            //std::cout << chargeRadius << std::endl;

            // ������Ⱦ����ɫΪ��ɫ
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            // ����Բ��
            SDL_RenderDrawCircle(renderer, static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(chargeRadius));

        }
    }
    void Pawn::AttackTarget(int Case, Vector2 pos) {
        switch (Case) {

        case 1:
        {

        }

        }
    }
}