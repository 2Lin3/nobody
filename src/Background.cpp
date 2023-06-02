#include <Background.h>
#include <SpriteComponent.h>
#include <Game.h>

namespace Nobody
{
	Background::Background(Game* game, b2World* world) : GameObject(game, world)
	{
		SpriteComponent* sprite = new SpriteComponent(this, 0); // 使用较低的绘制顺序（例如0）以确保背景位于其他游戏对象之后
		sprite->SetTexture(GetGame()->GetTexture("background"));
		// 将背景图片的位置设置为屏幕的左上角
		SetPosition(Vector2(0, 0));
		// 获取背景图片尺寸
		int bgWidth = sprite->GetTexWidth();
		int bgHeight = sprite->GetTexHeight();

		// 计算并设置背景缩放值
		int screenWidth = game->GetScreenWidth();
		int screenHeight = game->GetScreenHeight();
		float scaleX = static_cast<float>(screenWidth) / sprite->GetTexWidth();
		float scaleY = static_cast<float>(screenHeight) / sprite->GetTexHeight();
		SetScale(Vector2(scaleX, scaleY));
		// 设置 mIsBackground 为 true
		sprite->SetIsBackground(true);
	}
}
