#include	<TrickPawn.h>
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
	TrickPawn::TrickPawn(Game* game, b2World* world, const Vector2& playerPos, TrickPawnType type) :
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
		SetLife(100);
        mState = TrickPawnState::Lurking;
	}

	// �������λ��
	Vector2 TrickPawn::GenerateRandomPosition(const Vector2& playerPos) {
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
	bool TrickPawn::isValidPosition(float x, float y, const Vector2& playerPos) {
		return std::sqrt(std::pow(x - playerPos.x, 2) + std::pow(y - playerPos.y, 2)) > 100.0f;
	}
	void TrickPawn::Update()
	{
		GameObject::Update();
        if (AttactCase != 0) {

        }
		if (mType == TrickPawnType::AI)
		{
			UpdateAI();
		}
	}

	void TrickPawn::UpdateAI()
	{
            // ��ȡ���λ��
            Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

            // ʹ�� Game ���е� GetDeltaTime() ����
            float deltaTime = GetGame()->GetDeltaTime();

            // ���µ�ǰʱ��
            mCurrentTime += deltaTime;

            // ��ȡ TrickPawn ��ǰλ��
            Vector2 currentPosition = GetPosition();

            // ���� TrickPawn ����ҵľ���
            float distanceToPlayer = currentPosition.Distance(playerPos);

            // ��ȡ TrickPawn �ĸ��������RigidComponent��
            RigidComponent* rigidComponent = GetComponent<RigidComponent>();
            if (rigidComponent == nullptr)
            {
                return;
            }

            // ��ȡ TrickPawn �ĸ��壨b2Body��
            b2Body* body = rigidComponent->GetmBody();

            // ����ϴγ�ײʱ���Ƿ����5��
            float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
            // Update state
            UpdateState();

            // Execute behavior based on state
            switch (mState)
            {
            case TrickPawnState::Lurking:
                // Maintain a medium distance from the player
                LurkBehavior();
                break;
            case TrickPawnState::Attacking:
                // Charge towards the player
                AttackBehavior();
                break;
            case TrickPawnState::Escaping:
                // Hide behind minions
                EscapeBehavior();
                break;
            case TrickPawnState::Deceiving:
                // Implement your deceiving behavior
                DeceiveBehavior();
                break;
            case TrickPawnState::Resting:
                // Implement your resting behavior
                RestBehavior();
                break;
            }
            // �����������������Ĵ�С�Եõ�ʵ��ʩ�ӵ���
            b2Vec2 force = b2Vec2(forceDirection.x * forceMagnitude, forceDirection.y * forceMagnitude);

            // Ӧ����
            body->ApplyForceToCenter(force, true);
    }

    void TrickPawn::UpdateState() {
        // ��ȡ���λ��
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

        // ʹ�� Game ���е� GetDeltaTime() ����
        float deltaTime = GetGame()->GetDeltaTime();

        // ���µ�ǰʱ��
        mCurrentTime += deltaTime;

        // ��ȡ TrickPawn ��ǰλ��
        Vector2 currentPosition = GetPosition();

        // ���� TrickPawn ����ҵľ���
        float distanceToPlayer = currentPosition.Distance(playerPos);

        float mHealth = GetLife();

        // ����ϴγ�ײʱ���Ƿ����5��
        float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
        if (timeSinceLastPlayerCharge < 5.0f) {
            mState = TrickPawnState::Lurking;
        }
        // 2. Check if we are in attack range and not in CD.
        else if (distanceToPlayer <= mDetectionRange) {
            mState = TrickPawnState::Attacking;
        }
        // 3. If we are low on health, try to escape.
        else if (mHealth < 0.3 * mHealth) {
            mState = TrickPawnState::Escaping;
        }
        // 4. In other cases, we are probably resting or deceiving.
        else {

        }
    }
    void TrickPawn::LurkBehavior() {
        float distanceThreshold = mDetectionRange / 3; // ����һ�����ʵľ�����ֵ��������Ϸ�������
        // ��ȡ TrickPawn ��ǰλ��
        Vector2 currentPosition = GetPosition();
        // ��ȡ���λ��
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        // ���� TrickPawn ����ҵľ���
        float distanceToPlayer = currentPosition.Distance(playerPos);
        if (distanceToPlayer > mDetectionRange)
        {
            // ������ҹ�Զ������ҿ���
            forceDirection = (playerPos - currentPosition).Normalized();
            forceMagnitude = mSmallForceMagnitude; // ����һ���ʵ�������С
        }
        else if (distanceToPlayer < distanceThreshold)
        {
            // ������ҹ���������Զ��
            forceDirection = (currentPosition - playerPos).Normalized();
            forceMagnitude = mLargeForceMagnitude / 10; // ����һ���ϴ������С���Կ���Զ��
        }
        else
        {
            // ����������У��������
            Vector2 playerToPawn = currentPosition - playerPos;
            Vector2 rotatedDirection = Vector2(-playerToPawn.y, playerToPawn.x).Normalized(); // ��������ת90�ȣ�˳ʱ�����ʱ�룩

            forceDirection = rotatedDirection;
            forceMagnitude = mSmallForceMagnitude; // ����һ���ʵ�������С
        }
    }

    void TrickPawn::AttackBehavior() {
        // ��ȡ TrickPawn ��ǰλ��
        Vector2 currentPosition = GetPosition();
        // ��ȡ���λ��
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        // ���� TrickPawn ����ҵľ���
        float distanceToPlayer = currentPosition.Distance(playerPos);
        Charging = true;
        // �ڼ�ⷶΧ�ڣ�������ƶ������Գ�ײ
        forceDirection = (playerPos - currentPosition).Normalized();
        // ��ȡ TrickPawn �ĸ��������RigidComponent��
        RigidComponent* rigidComponent = GetComponent<RigidComponent>();
        // ����ϴγ�ײʱ���Ƿ����5��
        float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
        if (rigidComponent == nullptr)
        {
            return;
        }

        // ��ȡ TrickPawn �ĸ��壨b2Body��
        b2Body* body = rigidComponent->GetmBody();
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

    void TrickPawn::EscapeBehavior() {
        // Implement your escaping behavior, e.g., hiding behind minions
    }

    void TrickPawn::DeceiveBehavior() {
        // Implement your deceiving behavior
    }

    void TrickPawn::RestBehavior() {
        // Implement your resting behavior
    }

    void TrickPawn::Draw(SDL_Renderer* renderer) {
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
    void TrickPawn::Drawattackan(SDL_Renderer* renderer, bool onCharging,float timeWidth) {
        if (onCharging && timeWidth > 0.0f) {
            // ��ȡ TrickPawn ��ǰλ��
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
    void TrickPawn::AttackTarget(int Case, Vector2 pos) {
        switch (Case) {

        case 1:
        {

        }

        }
    }
}