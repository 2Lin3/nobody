#include	<Pawn.h>
#include	<SpriteComponent.h>
#include	<Game.h>
#include	<InputComponent.h>
#include	<MoveComponent.h>
#include	<RigidComponent.h>
#include	<box2d/box2d.h>
#include	<Player.h>
#include	<random>
namespace Nobody
{
	Pawn::Pawn(Game* game, b2World* world, const Vector2& playerPos) :
		GameObject(game, world)
	{
		Vector2 pos = GenerateRandomPosition(playerPos);
		SetPosition(pos);
		InputComponent* inputComponent = new InputComponent(this);
		RigidComponent* rigidComponent = new RigidComponent(this, b2_dynamicBody);
		rigidComponent->SetMass(0.5);
		b2CircleShape circle;
		circle.m_radius = 1.5f; // 设置圆的半径
		rigidComponent->SetShape(&circle);
		b2Body* mpawn = rigidComponent->GetmBody();
		mpawn->SetLinearDamping(0.8);
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
}