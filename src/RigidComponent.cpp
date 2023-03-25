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
		// 创建刚体定义
		b2BodyDef bodyDef;
		// 设置刚体类型
		bodyDef.type = mType;
		// 获取GameObject位置
		Vector2 position = owner->GetPosition();
		// 设置刚体位置
		bodyDef.position.Set(position.x/10, position.y/10);
		// 设置刚体旋转
		bodyDef.angle = owner->GetRotation();
		// 获取Game的Box2D世界
		b2World* world = owner->GetWorld();
		// 设置刚体的最大线速度
		b2Vec2 maxVelocity(b2_maxFloat, b2_maxFloat);
		// 创建刚体
		mBody = world->CreateBody(&bodyDef);
		mBody->SetLinearVelocity(maxVelocity);
		// 将该刚体绑定到GameObject上
		b2BodyUserData& userData = mBody->GetUserData();
		userData.pointer = reinterpret_cast<uintptr_t>(owner);
		// 其他代码

	}

	RigidComponent::~RigidComponent()
	{
		// 获取刚体所在的世界
		b2World* world = mBody->GetWorld();
		// 从世界中删除刚体
		world->DestroyBody(mBody);
	}

	void RigidComponent::Update()
	{
		GameObject* mGameObject = GetGameObject();
		// 获取刚体位置
		b2Vec2 position = mBody->GetPosition();
		b2Vec2 v = mBody->GetLinearVelocity();
		//cout << v.Length()<<endl;
		// 更新GameObject位置
		mGameObject->SetPosition(Vector2(10*position.x, 10*position.y));
		// 获取刚体旋转
		float rotation = mBody->GetAngle();
		//printf("%f\n", mBody->GetLinearVelocity().x, mBody->GetLinearVelocity().y);
		// 更新GameObject旋转
		mGameObject->SetRotation(rotation*10);
		// 其他代码
	}
	void RigidComponent::SetShape(b2Shape* shape)
	{
		// 删除之前的形状
		if (mShape != nullptr)
		{
			delete mShape;
			mShape = nullptr;
		}
		// 设置新的形状
		mShape = shape;
		// 创建形状定义
		b2FixtureDef fixtureDef;
		// 设置形状
		fixtureDef.shape = mShape;
		// 设置形状密度
		fixtureDef.density = 1.0f;
		// 设置形状摩擦
		fixtureDef.friction = 0.0f;
		// 设置形状弹性
		fixtureDef.restitution = 0.5f;
		// 将形状定义添加到刚体上
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
		// 创建刚体质量数据
		b2MassData massData;
		// 设置质量
		massData.mass = mass;
		// 如果有形状，计算形状的质心
		if (mShape)
		{
			mShape->ComputeMass(&massData, mass);
		}
		// 设置刚体的质量数据
		mBody->SetMassData(&massData);
		// 保存质量
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