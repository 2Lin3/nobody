//! @file SpriteComponent.h

#ifndef __Nobody_SpriteComponent__
#define __Nobody_SpriteComponent__

#include	<Component.h>

struct SDL_Texture;
struct SDL_Renderer;

namespace Nobody
{
	class GameObject;

	class SpriteComponent : public Component
	{
	public:
		//! ���캯��
		SpriteComponent(GameObject* gameObject, int drawOrder);
		//! ��������
		~SpriteComponent();

		//! ���ƺ���
		virtual void Draw(SDL_Renderer* renderer);
		//! ������ͼ
		virtual void SetTexture(SDL_Texture* texture);

		//! ��ͼ��ȵ�get
		int		GetTexWidth() const;
		//! ��ͼ�߶ȵ�get
		int		GetTexHeight() const;
		//! ����˳���get��set
		int		GetDrawOrder() const;
		void	SetDrawOrder(int order);

	protected:
		SDL_Texture* mTexture;		//!<	��ͼ		
		int				mDrawOrder;		//!<	����˳��
		int				mTexWidth;		//!<	��ͼ��
		int				mTexHeight;		//!<	��ͼ��
	};
}

#endif	// __Nobody_SpriteComponent__