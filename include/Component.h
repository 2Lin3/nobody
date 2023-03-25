#pragma once
//! @file Component.h

#ifndef __Nobody_Component__
#define __Nobody_Component__

#include	<string>
#include	<vector>

namespace Nobody
{
	class Component
	{
	public:
		//! 构造函数
		Component(class GameObject* gameObject);
		//! 析构函数
		virtual ~Component();
		//! 处理键盘输入
		virtual void	ProcessInput(const uint8_t* state);
		//！处理鼠标移动输入
		virtual void	ProcessInputMouse(float mousex, float mousey);
		//! 处理鼠标按键输入
		virtual void	ProcessInputMouseDown(int state, float* mBoost);
		//! 每帧更新
		virtual void		Update();

		//! 更新顺序的get和set
		int				GetUpdateOrder() const;
		void				SetUpdateOrder(int order);
		//! 游戏物体的get
		class GameObject* GetGameObject();

	protected:
		class GameObject* mGameObject;		//!<	自身附着的游戏物体
		int					mUpdateOrder;		//!<	更新顺序
	};
}

#endif	// 