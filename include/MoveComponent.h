//! @file MoveComponent.h

#ifndef __Nobody_MoveComponent__
#define __Nobody_MoveComponent__

#include	<Component.h>
#include<Mathg.h>
namespace Nobody
{
	class GameObject;

	class MoveComponent : public Component
	{
	public:
		//! 构造函数
		MoveComponent(GameObject* gameObject);

		//! 每帧更新（重写）
		void 	Update() override;

		//! 移动方向的set
		void	SetDir(const Vector2& dir);

	private:
		float	mSpeed;		//!<	移动速度
		float   friction;   //!<    摩擦
		Vector2	mDir;		//!<	移动方向
		Vector2 LjDir;		//!<	累计速度向量
		float	MaxChange;	//!<	最大速度
	};
}

#endif	// 