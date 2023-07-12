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
		circle.m_radius = radius; // 设置圆的半径
		rigidComponent->SetShape(&circle);
		mpawn = rigidComponent->GetmBody();
		mpawn->SetLinearDamping(dampv);
        float scaleFactorX = (2 * radius * SCALE) / sprite->GetTexWidth();
        float scaleFactorY = (2 * radius * SCALE) / sprite->GetTexHeight();
        SetScale(Vector2(scaleFactorX, scaleFactorY));
		SetLife(300);
        mState = BossPawnState::Lurking;
	}

	// 生成随机位置
	Vector2 BossPawn::GenerateRandomPosition(const Vector2& GeneratePos) {
		// 生成随机数生成器
		std::mt19937 generator(time(0));
		std::uniform_real_distribution<float> distribution(-100.0f, 100.0f);	
		float x, y;
		do {
			x = GeneratePos.x + distribution(generator);
			y = GeneratePos.y + distribution(generator);
		} while (!isValidPosition(x, y, GeneratePos));

		return Vector2(x, y);
	}
	// 判断距离是否大于100
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
            // 获取玩家位置
            Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

            // 使用 Game 类中的 GetDeltaTime() 方法
            float deltaTime = GetGame()->GetDeltaTime();

            // 更新当前时间
            mCurrentTime += deltaTime;

            // 获取 BossPawn 当前位置
            Vector2 currentPosition = GetPosition();

            // 计算 BossPawn 到玩家的距离
            float distanceToPlayer = currentPosition.Distance(playerPos);

            // 获取 BossPawn 的刚体组件（RigidComponent）
            RigidComponent* rigidComponent = GetComponent<RigidComponent>();
            if (rigidComponent == nullptr)
            {
                return;
            }

            // 获取 BossPawn 的刚体（b2Body）
            b2Body* body = rigidComponent->GetmBody();

            // 检查上次冲撞时间是否大于5秒
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
            // 将方向向量乘以力的大小以得到实际施加的力
            b2Vec2 force = b2Vec2(forceDirection.x * forceMagnitude, forceDirection.y * forceMagnitude);

            // 应用力
            body->ApplyForceToCenter(force, true);
    }

    void BossPawn::UpdateState() {
        // 获取玩家位置
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

        // 使用 Game 类中的 GetDeltaTime() 方法
        float deltaTime = GetGame()->GetDeltaTime();

        // 更新当前时间
        mCurrentTime += deltaTime;

        // 获取 BossPawn 当前位置
        Vector2 currentPosition = GetPosition();

        // 计算 BossPawn 到玩家的距离
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
        float distanceThreshold = mDetectionRange / 3; // 设置一个合适的距离阈值，根据游戏需求调整
        // 获取 BossPawn 当前位置
        Vector2 currentPosition = GetPosition();
        // 获取玩家位置
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        // 计算 BossPawn 到玩家的距离
        float distanceToPlayer = currentPosition.Distance(playerPos);
        if (distanceToPlayer > mDetectionRange)
        {
            // 距离玩家过远，向玩家靠近
            forceDirection = (playerPos - currentPosition).Normalized();
            forceMagnitude = mSmallForceMagnitude; // 设置一个适当的力大小
        }
        else if (distanceToPlayer < distanceThreshold)
        {
            // 距离玩家过近，快速远离
            forceDirection = (currentPosition - playerPos).Normalized();
            forceMagnitude = mLargeForceMagnitude / 10; // 设置一个较大的力大小，以快速远离
        }
        else
        {
            // 距离玩家适中，环绕玩家
            Vector2 playerToPawn = currentPosition - playerPos;
            Vector2 rotatedDirection = Vector2(-playerToPawn.y, playerToPawn.x).Normalized(); // 将向量旋转90度（顺时针或逆时针）

            forceDirection = rotatedDirection;
            forceMagnitude = mSmallForceMagnitude; // 设置一个适当的力大小
        }


        Vector2 avoidanceForce = Vector2(0, 0);
        float viewAngle = M_PI;
        for (GameObject* enemy : enemies) {
            if (enemy == this) continue;  // 忽略自身
            Vector2 enemyPos = enemy->GetPosition();
            float distanceToEnemy = currentPosition.Distance(enemyPos);

            if (distanceToEnemy < 5 * SCALE * radius) {
                //std::cout << distanceToEnemy << std::endl;
                Vector2 toEnemy = enemyPos - currentPosition;
                // 计算垂直于自身与玩家连接方向的斥力方向
                Vector2 perpendicularForceDirection = Vector2(-toEnemy.y, toEnemy.x).Normalized();
                // 斥力大小与距离负相关
                float force = 3 * mSmallForceMagnitude +2 * mSmallForceMagnitude / distanceToEnemy;
                // 计算当前对象和 enemy 的夹角是否在扇形视野内
                float angleBetween = acos(Vector2::Dot((enemyPos - currentPosition).Normalized(), forceDirection.Normalized()));
                if (angleBetween <= viewAngle / 2) {
                    avoidanceForce = avoidanceForce + perpendicularForceDirection * force;
                }
            }
        }
        // 在原有的行为基础上添加斥力
        Vector2 totalForceDirection = forceDirection * forceMagnitude + avoidanceForce;
        //if (forceDirection.x != totalForceDirection.Normalized().x) std::cout << "有" << std::endl;
        // 归一化总的力的方向，这样大小不会影响物体的速度
        forceDirection = totalForceDirection.Normalized();

    }

    void BossPawn::AttackBehavior() {
        // 获取 BossPawn 当前位置
        Vector2 currentPosition = GetPosition();
        // 获取玩家位置
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        // 计算 BossPawn 到玩家的距离
        float distanceToPlayer = currentPosition.Distance(playerPos);
        Charging = true;
        // 在检测范围内，朝玩家移动并尝试冲撞
        forceDirection = (playerPos - currentPosition).Normalized();
        // 获取 BossPawn 的刚体组件（RigidComponent）
        RigidComponent* rigidComponent = GetComponent<RigidComponent>();
        // 检查上次冲撞时间是否大于5秒
        float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
        if (rigidComponent == nullptr)
        {
            return;
        }

        // 获取 BossPawn 的刚体（b2Body）
        b2Body* body = rigidComponent->GetmBody();
        body->SetLinearDamping(dampv * 5);
        if (timeSinceLastPlayerCharge >= cooltime + timeWidth) {
            body->SetLinearDamping(dampv);
            forceMagnitude = mLargeForceMagnitude;
        }
        if (timeSinceLastPlayerCharge >= cooltime + 0.2 + timeWidth) {
            // 更新上次冲撞时间
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
        //绘制碰撞主体
        SDL_SetRenderDrawColor(renderer, 255, 120, 0, 255); // 设置黑色
        SDL_RenderDrawCircle(renderer, GetPosition().x, GetPosition().y, radius*10);
        // 斥力的检测区域
        if (mpawn) {
            b2Vec2 velocity = mpawn->GetLinearVelocity();
            float currentAngle = atan2(velocity.y, velocity.x);
            currentAngleDegrees = currentAngle * 180.0 / M_PI;

        }
        //视野调试
        //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // 设置为绿色
        //DrawSector(renderer, GetPosition().x, GetPosition().y, 5 * SCALE * radius,
        //    currentAngleDegrees +135,
        //    currentAngleDegrees +235);

        int numRings = 8; // 圆环数量
        int ringWidth = 2; // 圆环宽度（以像素为单位）
        int initialRadius = 8; // 最外层圆环的半径

        SDL_SetRenderDrawColor(renderer, 255, 206, 255, 255); // 设置淡蓝色

        for (int i = 0; i < numRings; ++i)
        {
            int radius = initialRadius + i * ringWidth;
            SDL_RenderDrawCircle(renderer, GetPosition().x, GetPosition().y, radius);
        }
        Drawattackan(renderer, Charging, timeWidth);
        // 绘制生命值条
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 设置白色
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
            // 获取 BossPawn 当前位置
            Vector2 position = GetPosition();

            // 使用 Game 类中的 GetDeltaTime() 方法
            float deltaTime = GetGame()->GetDeltaTime();

            // 获取从开始蓄力到现在的时间
            leftime += deltaTime;
            float elapsedTime = timeWidth - leftime;
            if (elapsedTime < 0) {
                Charging = false;
                leftime = 0;
            }
            // 计算圆环的半径
            float chargeRadius = std::min(10 * radius, radius * (10 * elapsedTime / timeWidth));
            //std::cout << chargeRadius << std::endl;

            // 设置渲染器颜色为红色
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            // 绘制圆形
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