//! @file SpriteComponent.h

#ifndef __Nobody_SpriteComponent__
#define __Nobody_SpriteComponent__

#include	<Component.h>
#include	<SDL.h>

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
		void	SetIsBackground(bool IsBackground);
		void DrawSector(SDL_Renderer* renderer, int x, int y, int radius, int start_angle, int end_angle)
		{
			for (int i = start_angle; i < end_angle; ++i)
			{
				int dx = radius * -cos(i * M_PI / 180.0);
				int dy = radius * -sin(i * M_PI / 180.0);
				SDL_RenderDrawLine(renderer, x, y, x + dx, y + dy);
			}
		}


	private:

		bool mIsBackground = false; // �Ǳ�����
	protected:
		SDL_Texture* mTexture;		//!<	��ͼ		
		int				mDrawOrder;		//!<	����˳��
		int				mTexWidth;		//!<	��ͼ��
		int				mTexHeight;		//!<	��ͼ��
	};
}

#endif	// __Nobody_SpriteComponent__