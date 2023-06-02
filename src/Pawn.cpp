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
		circle.m_radius = radius; // 设置圆的半径
		rigidComponent->SetShape(&circle);
		b2Body* mpawn = rigidComponent->GetmBody();
		mpawn->SetLinearDamping(dampv);
		SetLife(20);

	}

	// 生成随机位置
	Vector2 Pawn::GenerateRandomPosition(const Vector2& playerPos) {
		// 生成随机数生成器
		std::mt19937 generator(time(0));
		std::uniform_real_distribution<float> distribution(-100.0f, 100.0f);	
		float x, y;
		do {
			x = playerPos.x + distribution(generator);
			y = playerPos.y + distribution(generator);
		} while (!isValidPosition(x, y, playerPos));

		return Vector2(x, y);
	}
	// 判断距离是否大于100
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
    // 移动消耗疲劳值（ac条）
    //1.检测玩家位置，如果足够近
        //如果碰撞不在cd中，纪录当前玩家位置，红色渐变光效，然后对该位置冲撞 
        //如果在cd中，则随机选择 一：远离玩家位置 二：接近并环绕玩家，伺机等待攻击机会
        //如果离得足够远，则向下方行进，并检测离底部距离，如果足够近则冲撞底部
    //2.检测同伴位置，只需要保证非冲撞时不发生碰撞（保持一定距离）
	void Pawn::UpdateAI()
	{
            // 获取玩家位置
            Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

            // 使用 Game 类中的 GetDeltaTime() 方法
            float deltaTime = GetGame()->GetDeltaTime();

            // 更新当前时间
            mCurrentTime += deltaTime;

            // 获取 Pawn 当前位置
            Vector2 currentPosition = GetPosition();

            // 计算 Pawn 到玩家的距离
            float distanceToPlayer = currentPosition.Distance(playerPos);

            // 获取 Pawn 的刚体组件（RigidComponent）
            RigidComponent* rigidComponent = GetComponent<RigidComponent>();
            if (rigidComponent == nullptr)
            {
                return;
            }

            // 获取 Pawn 的刚体（b2Body）
            b2Body* body = rigidComponent->GetmBody();

            // 设置施加力的方向
            Vector2 forceDirection = Vector2::Zero;
            float forceMagnitude = 0.0f;

            // 检查上次冲撞时间是否大于5秒
            float timeSinceLastPlayerCharge = mCurrentTime - mLastPlayerChargeTime;
            if (distanceToPlayer <= mDetectionRange && timeSinceLastPlayerCharge >= 5.0f)
            {
                Charging = true;
                // 在检测范围内，朝玩家移动并尝试冲撞
                forceDirection = (playerPos - currentPosition).Normalized();
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
            else
            {
                // 检查上次冲撞时间是否大于3秒
                float timeSinceLastBottomCharge = mCurrentTime - mLastBottomChargeTime;
                if (currentPosition.y <= mScreenBottom && timeSinceLastBottomCharge >= 3.0f)
                {
                    // 到达屏幕底部，冲撞底部
                    forceDirection = Vector2(0, 1);
                    forceMagnitude = mLargeForceMagnitude;

                    // 更新上次冲撞时间
                    mLastBottomChargeTime = mCurrentTime;
                }
                else
                {
                    // 检查上次持续向下推进的时间是否大于3秒
                    float timeSinceLastDownForce = mCurrentTime - mLastDownForceTime;
                    if (timeSinceLastDownForce >= 3.0f)
                    {
                        // 向下持续的一个小力推进
                        forceDirection = Vector2(0, 1);
                        forceMagnitude = mSmallForceMagnitude;
                        if (timeSinceLastDownForce >= 4.0f)
                        {
                            // 更新上次向下推进的时间
                            mLastDownForceTime = mCurrentTime;
                        }
                    }
                    else
                    {
                        // 向下持续的一个小力推进
                        forceDirection = Vector2(0, 1);
                        forceMagnitude = mSmallForceMagnitude;
                    }
                }
            }

            // 将方向向量乘以力的大小以得到实际施加的力
            b2Vec2 force = b2Vec2(forceDirection.x * forceMagnitude, forceDirection.y * forceMagnitude);

            // 应用力
            body->ApplyForceToCenter(force, true);
    }

    void Pawn::Draw(SDL_Renderer* renderer) {
        //绘制碰撞主体
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // 设置黑色
        SDL_RenderDrawCircle(renderer, GetPosition().x, GetPosition().y, 15);

        int numRings = 5; // 圆环数量
        int ringWidth = 2; // 圆环宽度（以像素为单位）
        int initialRadius = 10; // 最外层圆环的半径

        SDL_SetRenderDrawColor(renderer, 135, 206, 250, 255); // 设置淡蓝色

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
    void Pawn::Drawattackan(SDL_Renderer* renderer, bool onCharging,float timeWidth) {
        if (onCharging && timeWidth > 0.0f) {
            // 获取 Pawn 当前位置
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
    void Pawn::AttackTarget(int Case, Vector2 pos) {
        switch (Case) {

        case 1:
        {

        }

        }
    }
}