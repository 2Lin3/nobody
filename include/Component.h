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
		//! ���캯��
		Component(class GameObject* gameObject);
		//! ��������
		virtual ~Component();
		//! �����������
		virtual void	ProcessInput(const uint8_t* state);
		//����������ƶ�����
		virtual void	ProcessInputMouse(float mousex, float mousey);
		//! ������갴������
		virtual void	ProcessInputMouseDown(int state, float* mBoost);
		//! ÿ֡����
		virtual void		Update();

		//! ����˳���get��set
		int				GetUpdateOrder() const;
		void				SetUpdateOrder(int order);
		//! ��Ϸ�����get
		class GameObject* GetGameObject();

	protected:
		class GameObject* mGameObject;		//!<	�����ŵ���Ϸ����
		int					mUpdateOrder;		//!<	����˳��
	};
}

#endif	// 