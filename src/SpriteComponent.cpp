#include	<SpriteComponent.h>
#include	<GameObject.h>
#include	<Game.h>
#include	<SDL.h>
#include	<RigidComponent.h>
namespace Nobody
{

	SpriteComponent::SpriteComponent(GameObject* gameObject, int drawOrder) :
		Component(gameObject),
		mTexture(nullptr),
		mTexWidth(0),
		mTexHeight(0),
		mDrawOrder(drawOrder),
		mIsBackground(false)
	{
		mGameObject->GetGame()->CreateSprite(this);
	}

	SpriteComponent::~SpriteComponent()
	{
		mGameObject->GetGame()->RemoveSprite(this);
	}

	void SpriteComponent::Draw(SDL_Renderer* renderer)
	{
		if (!mTexture)
		{
			return;
		}

		// ��ͼ���������Ƶ�λ��
		SDL_Rect dstrect;

		if (!mIsBackground)
		{
			dstrect.w = static_cast<int>(mTexWidth * mGameObject->GetScale().x);
			dstrect.h = static_cast<int>(mTexHeight * mGameObject->GetScale().y);
			dstrect.x = static_cast<int>(mGameObject->GetPosition().x - mTexWidth * mGameObject->GetScale().x / 2);
			dstrect.y = static_cast<int>(mGameObject->GetPosition().y - mTexHeight * mGameObject->GetScale().y / 2);

		}
		else
		{
			dstrect.w = 1280;
			dstrect.h = 800;
			dstrect.x = static_cast<int>(mGameObject->GetPosition().x);
			dstrect.y = static_cast<int>(mGameObject->GetPosition().y);
		}

		RigidComponent* rc = mGameObject->GetComponent<RigidComponent>();
		if (rc)
		{
			b2Body* body = rc->GetmBody();

			// ��ȡ������ٶ�����
			b2Vec2 velocity = body->GetLinearVelocity();

			// �����ٶ������ĽǶȣ����ȣ�
			float angleInRadians = atan2(velocity.y, velocity.x);

			// ������ת��Ϊ��
			float desiredRotationInDegrees = angleInRadians * (180.0f / M_PI);

			// ������ͼ��������ת��
			SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstrect, desiredRotationInDegrees - 90, nullptr, SDL_FLIP_NONE);
		}
		else
		{
			// ��� RigidComponent �����ڣ����Ը�����Ҫ���һЩ�߼�������ֱ�ӷ���
			SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstrect, 0, nullptr, SDL_FLIP_NONE);
		}
		// ������ͼ��������ת��
		//SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstrect, mGameObject->GetRotation(), nullptr, SDL_FLIP_NONE);
	}


	void SpriteComponent::SetTexture(SDL_Texture* texture)
	{
		mTexture = texture;
		// ����ͼ�в�ѯ��ͼ�Ŀ�͸�
		SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
	}

	int SpriteComponent::GetTexWidth() const
	{
		return mTexWidth;
	}

	int SpriteComponent::GetTexHeight() const
	{
		return mTexHeight;
	}

	int SpriteComponent::GetDrawOrder() const
	{
		return mDrawOrder;
	}

	void SpriteComponent::SetDrawOrder(int order)
	{
		mDrawOrder = order;
	}

	void SpriteComponent::SetIsBackground(bool IsBackground) {
		mIsBackground = IsBackground;
	}

}