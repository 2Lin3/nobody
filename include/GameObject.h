#pragma once
//! @file GameObject.h

#ifndef __Nobody_GameObject__
#define __Nobody_GameObject__

#include	<string>
#include	<vector>
#include	<mathg.h>
#include	<box2d/box2d.h>
#include	<SDL.h>
namespace Nobody
{
	class GameObject
	{
	public:
		//! 物体状态
		enum class State
		{
			EActive,
			EDead
		};

		//! 构造函数
		GameObject(class Game* game, b2World* world);
		//! 析构函数
		virtual ~GameObject();
		//! 特殊绘制
		virtual void Draw(SDL_Renderer* renderer);
		//! 处理键盘输入
		void	ProcessInput(const uint8_t* state);
		//！处理鼠标移动输入
		void	ProcessInputMouse(float mousex, float mousey);
		//! 处理鼠标按键输入
		void	ProcessInputMouseDown(int state, float* mBoost);
		//! 每帧更新
		virtual void	Update();
		//! 添加组件
		void	AddComponent(class Component* component);
		//! 移除组件
		void	RemoveComponent(class Component* component);

		//! 获取特定类型的组件，并得到该组件类型的指针
		template<typename T> T* GetComponent()
		{
			for (auto com : mComponents)
			{
				T* component = dynamic_cast<T*>(com);
				if (component != nullptr)
				{
					return component;
				}
			}	
			return nullptr;
		}
		//! game对象的get
		Game* GetGame() const;
		//! Box2D世界的get
		b2World* GetWorld() const;
		//! 物体状态的get和set
		State		GetState() const;
		void		SetState(State state);
		//! 位置的get和set
		Vector2		GetPosition() const;
		void		SetPosition(const Vector2& p);
		//! 缩放大小的get和set
		Vector2		GetScale() const;
		void		SetScale(Vector2 s);
		//! 旋转的get和set
		float		GetRotation() const;
		void		SetRotation(float r);
		//! 生命值的get和set
		float		GetLife() const;
		void		SetLife(float r);


		bool isPlayer = false;
	protected:
		std::vector<class Component*>	mComponents;	//!<	组件数组

	private:
		class Game* mGame;			//!<	Game对象
		b2World* mWorld;			//!<	Box2D世界
		State							mState;			//!<	物体当前状态
		Vector2							mPosition;		//!<	位置坐标
		Vector2							mScale;			//!<	缩放
		float							mRotation;		//!<	旋转
		float							mLife;			//!<	生命值

	};
}

#endif	// 