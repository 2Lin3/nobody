#include <Background.h>
#include <SpriteComponent.h>
#include <Game.h>

namespace Nobody
{
	Background::Background(Game* game, b2World* world) : GameObject(game, world)
	{
		SpriteComponent* sprite = new SpriteComponent(this, 0); // ʹ�ýϵ͵Ļ���˳������0����ȷ������λ��������Ϸ����֮��
		sprite->SetTexture(GetGame()->GetTexture("background"));
		// ������ͼƬ��λ������Ϊ��Ļ�����Ͻ�
		SetPosition(Vector2(0, 0));
		// ��ȡ����ͼƬ�ߴ�
		int bgWidth = sprite->GetTexWidth();
		int bgHeight = sprite->GetTexHeight();

		// ���㲢���ñ�������ֵ
		int screenWidth = game->GetScreenWidth();
		int screenHeight = game->GetScreenHeight();
		float scaleX = static_cast<float>(screenWidth) / sprite->GetTexWidth();
		float scaleY = static_cast<float>(screenHeight) / sprite->GetTexHeight();
		SetScale(Vector2(scaleX, scaleY));
		// ���� mIsBackground Ϊ true
		sprite->SetIsBackground(true);
	}
}
