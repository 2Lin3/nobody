//! @file SpriteComponent.h

#ifndef __Nobody_RigidComponent__
#define __Nobody_RigidComponent__

#include	<Component.h>
#include	<box2d/box2d.h>
#include	<Mathg.h>

struct SDL_Renderer;
namespace Nobody
{
	class GameObject;

	class RigidComponent : public Component
	{
	public:
		// 构造函数
		RigidComponent(class GameObject* owner, b2BodyType type);
		// 析构函数
		~RigidComponent();
		// 更新函数
		void Update() override;
		// 设置刚体形状
		void SetShape(b2Shape* shape);
		// 设置刚体的质量和惯性
		void SetMass(float mass);
		// 声明ApplyForce函数
		void ApplyForce(const Vector2& force);
		void ApplyForce(const Vector2& force, const Vector2& point);
		b2Body* GetmBody();
	private:
		// 刚体类型
		b2BodyType mType;
		// Box2D刚体
		b2Body* mBody;
		// 刚体形状
		b2Shape* mShape;
		// 刚体质量
		float mMass;
		// 其他代码
	};
}
int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius);

#endif	