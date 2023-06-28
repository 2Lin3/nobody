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
		//! 构造函数
		SpriteComponent(GameObject* gameObject, int drawOrder);
		//! 析构函数
		~SpriteComponent();

		//! 绘制函数
		virtual void Draw(SDL_Renderer* renderer);
		//! 设置贴图
		virtual void SetTexture(SDL_Texture* texture);

		//! 贴图宽度的get
		int		GetTexWidth() const;
		//! 贴图高度的get
		int		GetTexHeight() const;
		//! 绘制顺序的get和set
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

		bool mIsBackground = false; // 是背景吗
	protected:
		SDL_Texture* mTexture;		//!<	贴图		
		int				mDrawOrder;		//!<	绘制顺序
		int				mTexWidth;		//!<	贴图宽
		int				mTexHeight;		//!<	贴图高
	};
}

#endif	// __Nobody_SpriteComponent__