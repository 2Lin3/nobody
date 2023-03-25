#include	<RigidComponent.h>
#include	<GameObject.h>
#include	<Game.h>
#include	<SDL.h>
#include	<box2d/box2d.h>
#include	<iostream>

#include	<Collision.h>
using namespace std;
namespace Nobody
{

	RigidComponent::RigidComponent(class GameObject* owner, b2BodyType type)
		:Component(owner)
		, mType(type)
		, mBody(nullptr)
		, mShape(nullptr)
		, mMass(1.0f)
	{
		// �������嶨��
		b2BodyDef bodyDef;
		// ���ø�������
		bodyDef.type = mType;
		// ��ȡGameObjectλ��
		Vector2 position = owner->GetPosition();
		// ���ø���λ��
		bodyDef.position.Set(position.x/10, position.y/10);
		// ���ø�����ת
		bodyDef.angle = owner->GetRotation();
		// ��ȡGame��Box2D����
		b2World* world = owner->GetWorld();
		// ���ø����������ٶ�
		b2Vec2 maxVelocity(b2_maxFloat, b2_maxFloat);
		// ��������
		mBody = world->CreateBody(&bodyDef);
		mBody->SetLinearVelocity(maxVelocity);
		// ���ø���󶨵�GameObject��
		b2BodyUserData& userData = mBody->GetUserData();
		userData.pointer = reinterpret_cast<uintptr_t>(owner);
		// ��������

	}

	RigidComponent::~RigidComponent()
	{
		// ��ȡ�������ڵ�����
		b2World* world = mBody->GetWorld();
		// ��������ɾ������
		world->DestroyBody(mBody);
	}

	void RigidComponent::Update()
	{
		GameObject* mGameObject = GetGameObject();
		// ��ȡ����λ��
		b2Vec2 position = mBody->GetPosition();
		b2Vec2 v = mBody->GetLinearVelocity();
		//cout << v.Length()<<endl;
		// ����GameObjectλ��
		mGameObject->SetPosition(Vector2(10*position.x, 10*position.y));
		// ��ȡ������ת
		float rotation = mBody->GetAngle();
		//printf("%f\n", mBody->GetLinearVelocity().x, mBody->GetLinearVelocity().y);
		// ����GameObject��ת
		mGameObject->SetRotation(rotation*10);
		// ��������
	}
	void RigidComponent::SetShape(b2Shape* shape)
	{
		// ɾ��֮ǰ����״
		if (mShape != nullptr)
		{
			delete mShape;
			mShape = nullptr;
		}
		// �����µ���״
		mShape = shape;
		// ������״����
		b2FixtureDef fixtureDef;
		// ������״
		fixtureDef.shape = mShape;
		// ������״�ܶ�
		fixtureDef.density = 1.0f;
		// ������״Ħ��
		fixtureDef.friction = 0.0f;
		// ������״����
		fixtureDef.restitution = 0.5f;
		// ����״������ӵ�������
		mBody->CreateFixture(&fixtureDef);
	}
	void RigidComponent::ApplyForce(const Vector2& force)
	{
		mBody->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
	}

	void RigidComponent::ApplyForce(const Vector2& force, const Vector2& point)
	{
		mBody->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), true);
	}

	void RigidComponent::SetMass(float mass)
	{
		// ����������������
		b2MassData massData;
		// ��������
		massData.mass = mass;
		// �������״��������״������
		if (mShape)
		{
			mShape->ComputeMass(&massData, mass);
		}
		// ���ø������������
		mBody->SetMassData(&massData);
		// ��������
		mMass = mass;
	}
	b2Body* RigidComponent::GetmBody() {
		return mBody;
	}

}
int	SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}