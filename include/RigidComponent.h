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
		// ���캯��
		RigidComponent(class GameObject* owner, b2BodyType type);
		// ��������
		~RigidComponent();
		// ���º���
		void Update() override;
		// ���ø�����״
		void SetShape(b2Shape* shape);
		// ���ø���������͹���
		void SetMass(float mass);
		// ����ApplyForce����
		void ApplyForce(const Vector2& force);
		void ApplyForce(const Vector2& force, const Vector2& point);
		b2Body* GetmBody();
	private:
		// ��������
		b2BodyType mType;
		// Box2D����
		b2Body* mBody;
		// ������״
		b2Shape* mShape;
		// ��������
		float mMass;
		// ��������
	};
}
int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius);

#endif	