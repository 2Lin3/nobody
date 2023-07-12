//! @file GameObject.cpp

#include	<GameObject.h>
#include	<Game.h>
#include	<Component.h>
#include	<RigidComponent.h>
#include	<iostream>
namespace Nobody
{
	GameObject::GameObject(Game* game, b2World* world) :
		mGame(game),
		mWorld(world),
		mState(State::EActive),
		mPosition(Vector2::Zero),
		mScale(Vector2::One),
		mRotation(0.0f),
		mLife(100.0f)
	{
		mGame->CreateGameObject(this);
	}

	GameObject::~GameObject()
	{
		mGame->RemoveGameObject(this);
		while (!mComponents.empty())
		{
			delete mComponents.back();
		}
	}
	void GameObject::ProcessInput(const uint8_t* state)
	{
		if (mState == State::EActive)
		{
			// 每帧执行，处理所有组件的输入逻辑
			for (auto component : mComponents)
			{
				component->ProcessInput(state);
			}
		}
	}

	void GameObject::ProcessInputMouse(float mousex, float mousey)
	{
		if (mState == State::EActive)
		{
			// 每帧执行，处理所有组件的输入逻辑
			for (auto& component : mComponents)
			{
				if (component == nullptr) {
					continue; // skip this component
				}
				component->ProcessInputMouse(mousex, mousey);
			}
		}
	}

	void GameObject::ProcessInputMouseDown(int state, float* mBoost)
	{
		if (mState == State::EActive)
		{
			// 每帧执行，处理所有组件的输入逻辑
			for (auto& component : mComponents)
			{
				if (component == nullptr)
				{
					continue; // skip this component
				}
				component->ProcessInputMouseDown(state, mBoost);
			}
		}
	}

	void GameObject::Update()
	{
		if (mState == State::EActive)
		{
			// 每帧执行，更新所有的组件
			for (auto component : mComponents)
			{
				component->Update();
			}
		}

		if (mLife == 0) {
			mState = State::EDead;
		}
	}

	void GameObject::AddComponent(Component* component)
	{
		// 获取该组件的更新顺序
		int order = component->GetUpdateOrder();
		auto iter = mComponents.begin();
		// 新组件需要按顺序插入到对应位置
		for (; iter != mComponents.end(); ++iter)
		{
			if (order < (*iter)->GetUpdateOrder())
			{
				break;
			}
		}
		mComponents.insert(iter, component);
	}

	void GameObject::RemoveComponent(Component* component)
	{
		auto iter = std::find(mComponents.begin(), mComponents.end(), component);
		if (iter != mComponents.end())
		{
			mComponents.erase(iter);
		}
	}

	void GameObject::Draw(SDL_Renderer* renderer)
	{
		
	}

	Game* GameObject::GetGame() const
	{
		return mGame;
	}

	b2World* GameObject::GetWorld() const
	{
		return mWorld;
	}

	GameObject::State GameObject::GetState() const
	{
		return mState;
	}

	void GameObject::SetState(State state)
	{
		mState = state;
	}

	Vector2 GameObject::GetPosition() const
	{
		return mPosition;
	}

	void GameObject::SetPosition(const Vector2& p)
	{
		mPosition = p;
	}

	Vector2 GameObject::GetScale() const
	{
		return mScale;
	}

	void GameObject::SetScale(Vector2 s)
	{
		mScale = s;
	}

	float GameObject::GetRotation() const
	{
		return mRotation;
	}

	void GameObject::SetRotation(float r)
	{
		mRotation = r;
	}
	float GameObject::GetLife() const
	{
		return mLife;
	}

	void GameObject::SetLife(float r)
	{
		mLife = r;
		if (mLife < 0) mLife = 0;
	}
}