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

		// 贴图即将被绘制的位置
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

			// 获取物体的速度向量
			b2Vec2 velocity = body->GetLinearVelocity();

			// 计算速度向量的角度（弧度）
			float angleInRadians = atan2(velocity.y, velocity.x);

			// 将弧度转换为度
			float desiredRotationInDegrees = angleInRadians * (180.0f / M_PI);

			// 绘制贴图（考虑旋转）
			SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstrect, desiredRotationInDegrees - 90, nullptr, SDL_FLIP_NONE);
		}
		else
		{
			// 如果 RigidComponent 不存在，可以根据需要添加一些逻辑，或者直接返回
			SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstrect, 0, nullptr, SDL_FLIP_NONE);
		}
		// 绘制贴图（考虑旋转）
		//SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstrect, mGameObject->GetRotation(), nullptr, SDL_FLIP_NONE);
	}


	void SpriteComponent::SetTexture(SDL_Texture* texture)
	{
		mTexture = texture;
		// 从贴图中查询贴图的宽和高
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