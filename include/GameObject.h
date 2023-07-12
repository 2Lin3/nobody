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
		//! ����״̬
		enum class State
		{
			EActive,
			EDead
		};

		//! ���캯��
		GameObject(class Game* game, b2World* world);
		//! ��������
		virtual ~GameObject();
		//! �������
		virtual void Draw(SDL_Renderer* renderer);
		//! �����������
		void	ProcessInput(const uint8_t* state);
		//����������ƶ�����
		void	ProcessInputMouse(float mousex, float mousey);
		//! ������갴������
		void	ProcessInputMouseDown(int state, float* mBoost);
		//! ÿ֡����
		virtual void	Update();
		//! ������
		void	AddComponent(class Component* component);
		//! �Ƴ����
		void	RemoveComponent(class Component* component);

		//! ��ȡ�ض����͵���������õ���������͵�ָ��
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
		//! game�����get
		Game* GetGame() const;
		//! Box2D�����get
		b2World* GetWorld() const;
		//! ����״̬��get��set
		State		GetState() const;
		void		SetState(State state);
		//! λ�õ�get��set
		Vector2		GetPosition() const;
		void		SetPosition(const Vector2& p);
		//! ���Ŵ�С��get��set
		Vector2		GetScale() const;
		void		SetScale(Vector2 s);
		//! ��ת��get��set
		float		GetRotation() const;
		void		SetRotation(float r);
		//! ����ֵ��get��set
		float		GetLife() const;
		void		SetLife(float r);


		bool isPlayer = false;
	protected:
		std::vector<class Component*>	mComponents;	//!<	�������

	private:
		class Game* mGame;			//!<	Game����
		b2World* mWorld;			//!<	Box2D����
		State							mState;			//!<	���嵱ǰ״̬
		Vector2							mPosition;		//!<	λ������
		Vector2							mScale;			//!<	����
		float							mRotation;		//!<	��ת
		float							mLife;			//!<	����ֵ

	};
}

#endif	// 