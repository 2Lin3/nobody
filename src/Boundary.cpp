//! @file Player.h

#include	<Boundary.h>
#include	<Game.h>
#include	<InputComponent.h>
#include	<MoveComponent.h>
#include	<RigidComponent.h>	
#include	<box2d/box2d.h>

namespace Nobody
{
	Boundary::Boundary(Game* game, b2World* world):
		GameObject(game,world)
	{
		// 设置地面位置
		SetPosition(Vector2(640, 800));
		// 创建RigidComponent，设置为静态刚体
		RigidComponent* groundComponent = new RigidComponent(this, b2_staticBody);
		// 创建地面形状
		b2PolygonShape groundShape;
		groundShape.SetAsBox(64, 5);
		// 创建地面夹具
		b2FixtureDef groundFixtureDef;
		groundFixtureDef.shape = &groundShape;
		b2Body* groundBody = groundComponent->GetmBody();
		groundBody->CreateFixture(&groundFixtureDef);
		groundFixtureDef.density = 1.0f;
		groundFixtureDef.friction = 0.5f;

		//左侧
		b2PolygonShape groundShapeleft;
		groundShapeleft.SetAsBox(5, 40, b2Vec2(-64, -40), 0);
		b2FixtureDef leftgroundFixtureDef;
		leftgroundFixtureDef.shape = &groundShapeleft;
		groundBody->CreateFixture(&leftgroundFixtureDef);
		leftgroundFixtureDef.density = 1.0f;
		leftgroundFixtureDef.friction = 0.5f;

		//右侧
		b2PolygonShape groundShaperight;
		groundShaperight.SetAsBox(5, 40, b2Vec2(64, -40), 0);
		b2FixtureDef rightgroundFixtureDef;
		rightgroundFixtureDef.shape = &groundShaperight;
		groundBody->CreateFixture(&rightgroundFixtureDef);
		rightgroundFixtureDef.density = 1.0f;
		rightgroundFixtureDef.friction = 0.5f;
		SetLife(100000);
	}
}