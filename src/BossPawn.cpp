#include	<BossPawn.h>
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
	BossPawn::BossPawn(Game* game, b2World* world, const Vector2& GeneratePos, const std::vector<GameObject*>& enemies, BossPawnType type) :
		GameObject(game, world),
		mType(type),
        mLastPlayerChargeTime(0.0f),
        mLastBottomChargeTime(0.0f),
        mLastDownForceTime(0.0f),
        enemies(enemies)
	{
        SpriteComponent* sprite = new SpriteComponent(this, 100);
        sprite->SetTexture(GetGame()->GetTexture("BossPawn"));
		Vector2 pos = GenerateRandomPosition(GeneratePos);
		SetPosition(GeneratePos);
		InputComponent* inputComponent = new InputComponent(this);
		RigidComponent* rigidComponent = new RigidComponent(this, b2_dynamicBody);
		rigidComponent->SetMass(0.3);
		b2CircleShape circle;
		circle.m_radius = radius; // ����Բ�İ뾶
		rigidComponent->SetShape(&circle);
		mpawn = rigidComponent->GetmBody();
		mpawn->SetLinearDamping(dampv);
        float scaleFactorX = (2 * radius * SCALE) / sprite->GetTexWidth();
        float scaleFactorY = (2 * radius * SCALE) / sprite->GetTexHeight();
        SetScale(Vector2(scaleFactorX, scaleFactorY));
		SetLife(300);
        mState = BossPawnState::Lurking;
	}

	// �������λ��
	Vector2 BossPawn::GenerateRandomPosition(const Vector2& GeneratePos) {
		// ���������������
		std::mt19937 generator(time(0));
		std::uniform_real_distribution<float> distribution(-100.0f, 100.0f);	
		float x, y;
		do {
			x = GeneratePos.x + distribution(generator);
			y = GeneratePos.y + distribution(generator);
		} while (!isValidPosition(x, y, GeneratePos));

		return Vector2(x, y);
	}
	// �жϾ����Ƿ����100
	bool BossPawn::isValidPosition(float x, float y, const Vector2& GeneratePos) {
		return std::sqrt(std::pow(x - GeneratePos.x, 2) + std::pow(y - GeneratePos.y, 2)) > 100.0f;
	}
	void BossPawn::Update()
	{
		GameObject::Update();
        if (AttactCase != 0) {

        }
		if (mType == BossPawnType::AI)
		{
			UpdateAI();
		}
	}

	void BossPawn::UpdateAI()
	{
            // ��ȡ���λ��
            Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

            // ʹ�� Game ���е� GetDeltaTime() ����
            float deltaTime = GetGame()->GetDeltaTime();

            // ���µ�ǰʱ��
            mCurrentTime += deltaTime;

            // ��ȡ BossPawn ��ǰλ��
            Vector2 currentPosition = GetPosition();

            // ���� BossPawn ����ҵľ���
            float distanceToPlayer = currentPosition.Distance(playerPos);

            // ��ȡ BossPawn �ĸ��������RigidComponent��
            RigidComponent* rigidComponent = GetComponent<RigidComponent>();
            if (rigidComponent == nullptr)
            {
                return;
            }

            // ��ȡ BossPawn �ĸ��壨b2Body��
            b2Body* body = rigidComponent->GetmBody();

            // ����ϴγ�ײʱ���Ƿ����5��
            float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
            // Update state
            UpdateState();

            // Execute behavior based on state
            switch (mState)
            {
            case BossPawnState::Lurking:
                // Maintain a medium distance from the player
                LurkBehavior();
                break;
            case BossPawnState::Attacking:
                // Charge towards the player
                AttackBehavior();
                break;
            case BossPawnState::Escaping:
                // Hide behind minions
                EscapeBehavior();
                break;
            case BossPawnState::Deceiving:
                // Implement your deceiving behavior
                DeceiveBehavior();
                break;
            case BossPawnState::Resting:
                // Implement your resting behavior
                RestBehavior();
                break;
            }
            // �����������������Ĵ�С�Եõ�ʵ��ʩ�ӵ���
            b2Vec2 force = b2Vec2(forceDirection.x * forceMagnitude, forceDirection.y * forceMagnitude);

            // Ӧ����
            body->ApplyForceToCenter(force, true);
    }

    void BossPawn::UpdateState() {
        // ��ȡ���λ��
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

        // ʹ�� Game ���е� GetDeltaTime() ����
        float deltaTime = GetGame()->GetDeltaTime();

        // ���µ�ǰʱ��
        mCurrentTime += deltaTime;

        // ��ȡ BossPawn ��ǰλ��
        Vector2 currentPosition = GetPosition();

        // ���� BossPawn ����ҵľ���
        float distanceToPlayer = currentPosition.Distance(playerPos);

        float mHealth = GetLife();

        float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
        if (timeSinceLastPlayerCharge < cooltime) {
            mState = BossPawnState::Lurking;
        }
        // 2. Check if we are in attack range and not in CD.
        else if (distanceToPlayer <= mDetectionRange) {
            mState = BossPawnState::Attacking;
        }
        // 3. If we are low on health, try to escape.
        else if (mHealth < 0.3 * mHealth) {
            mState = BossPawnState::Escaping;
        }
        // 4. In other cases, we are probably resting or deceiving.
        else {

        }
    }
    void BossPawn::LurkBehavior() {
        float distanceThreshold = mDetectionRange / 3; // ����һ�����ʵľ�����ֵ��������Ϸ�������
        // ��ȡ BossPawn ��ǰλ��
        Vector2 currentPosition = GetPosition();
        // ��ȡ���λ��
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        // ���� BossPawn ����ҵľ���
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


        Vector2 avoidanceForce = Vector2(0, 0);
        float viewAngle = M_PI;
        for (GameObject* enemy : enemies) {
            if (enemy == this) continue;  // ��������
            Vector2 enemyPos = enemy->GetPosition();
            float distanceToEnemy = currentPosition.Distance(enemyPos);

            if (distanceToEnemy < 5 * SCALE * radius) {
                //std::cout << distanceToEnemy << std::endl;
                Vector2 toEnemy = enemyPos - currentPosition;
                // ���㴹ֱ��������������ӷ���ĳ�������
                Vector2 perpendicularForceDirection = Vector2(-toEnemy.y, toEnemy.x).Normalized();
                // ������С����븺���
                float force = 3 * mSmallForceMagnitude +2 * mSmallForceMagnitude / distanceToEnemy;
                // ���㵱ǰ����� enemy �ļн��Ƿ���������Ұ��
                float angleBetween = acos(Vector2::Dot((enemyPos - currentPosition).Normalized(), forceDirection.Normalized()));
                if (angleBetween <= viewAngle / 2) {
                    avoidanceForce = avoidanceForce + perpendicularForceDirection * force;
                }
            }
        }
        // ��ԭ�е���Ϊ��������ӳ���
        Vector2 totalForceDirection = forceDirection * forceMagnitude + avoidanceForce;
        //if (forceDirection.x != totalForceDirection.Normalized().x) std::cout << "��" << std::endl;
        // ��һ���ܵ����ķ���������С����Ӱ��������ٶ�
        forceDirection = totalForceDirection.Normalized();

    }

    void BossPawn::AttackBehavior() {
        // ��ȡ BossPawn ��ǰλ��
        Vector2 currentPosition = GetPosition();
        // ��ȡ���λ��
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        // ���� BossPawn ����ҵľ���
        float distanceToPlayer = currentPosition.Distance(playerPos);
        Charging = true;
        // �ڼ�ⷶΧ�ڣ�������ƶ������Գ�ײ
        forceDirection = (playerPos - currentPosition).Normalized();
        // ��ȡ BossPawn �ĸ��������RigidComponent��
        RigidComponent* rigidComponent = GetComponent<RigidComponent>();
        // ����ϴγ�ײʱ���Ƿ����5��
        float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
        if (rigidComponent == nullptr)
        {
            return;
        }

        // ��ȡ BossPawn �ĸ��壨b2Body��
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

    void BossPawn::EscapeBehavior() {
        // Implement your escaping behavior, e.g., hiding behind minions
    }

    void BossPawn::DeceiveBehavior() {
        // Implement your deceiving behavior
    }

    void BossPawn::RestBehavior() {
        // Implement your resting behavior
    }

    void BossPawn::Draw(SDL_Renderer* renderer) {
        //������ײ����
        SDL_SetRenderDrawColor(renderer, 255, 120, 0, 255); // ���ú�ɫ
        SDL_RenderDrawCircle(renderer, GetPosition().x, GetPosition().y, radius*10);
        // �����ļ������
        if (mpawn) {
            b2Vec2 velocity = mpawn->GetLinearVelocity();
            float currentAngle = atan2(velocity.y, velocity.x);
            currentAngleDegrees = currentAngle * 180.0 / M_PI;

        }
        //��Ұ����
        //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // ����Ϊ��ɫ
        //DrawSector(renderer, GetPosition().x, GetPosition().y, 5 * SCALE * radius,
        //    currentAngleDegrees +135,
        //    currentAngleDegrees +235);

        int numRings = 8; // Բ������
        int ringWidth = 2; // Բ����ȣ�������Ϊ��λ��
        int initialRadius = 8; // �����Բ���İ뾶

        SDL_SetRenderDrawColor(renderer, 255, 206, 255, 255); // ���õ���ɫ

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
    void BossPawn::Drawattackan(SDL_Renderer* renderer, bool onCharging,float timeWidth) {
        if (onCharging && timeWidth > 0.0f) {
            // ��ȡ BossPawn ��ǰλ��
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
    void BossPawn::AttackTarget(int Case, Vector2 pos) {
        switch (Case) {

        case 1:
        {

        }

        }
    }
}