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
		// ���õ���λ��
		SetPosition(Vector2(640, 800));
		// ����RigidComponent������Ϊ��̬����
		RigidComponent* groundComponent = new RigidComponent(this, b2_staticBody);
		// ����������״
		b2PolygonShape groundShape;
		groundShape.SetAsBox(64, 5);
		// ��������о�
		b2FixtureDef groundFixtureDef;
		groundFixtureDef.shape = &groundShape;
		b2Body* groundBody = groundComponent->GetmBody();
		groundBody->CreateFixture(&groundFixtureDef);
		groundFixtureDef.density = 1.0f;
		groundFixtureDef.friction = 0.5f;

		//���
		b2PolygonShape groundShapeleft;
		groundShapeleft.SetAsBox(5, 40, b2Vec2(-64, -40), 0);
		b2FixtureDef leftgroundFixtureDef;
		leftgroundFixtureDef.shape = &groundShapeleft;
		groundBody->CreateFixture(&leftgroundFixtureDef);
		leftgroundFixtureDef.density = 1.0f;
		leftgroundFixtureDef.friction = 0.5f;

		//�Ҳ�
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