//! @file InputComponent.cpp

#include	<InputComponent.h>
#include	<MoveComponent.h>
#include	<GameObject.h>
#include	<SDL_scancode.h>
#include    <SDL.h>
#include	<Mathg.h>
#include	<Game.h>
#include	<RigidComponent.h>

namespace Nobody
{
	InputComponent::InputComponent(GameObject* gameObject) :
		Component(gameObject)
	{

	}

	void InputComponent::ProcessInput(const uint8_t* state)
	{

	
	}

	void InputComponent::ProcessInputMouse(float mousex, float mousey) {
		RigidComponent* mcc = mGameObject->GetComponent<RigidComponent>();
		float MaxChange = 20;
		last_mouse_x = scene_width/2;
		last_mouse_y = scene_height/2;
		// 计算鼠标位置的变化
		float dx = mousex - last_mouse_x;
		float dy = mousey - last_mouse_y;
		dx = dx > MaxChange ? MaxChange : dx;
		dx = dx < -MaxChange ? -MaxChange : dx;
		dy = dy > MaxChange ? MaxChange : dy;
		dy = dy < -MaxChange ? -MaxChange : dy;
		mcc->ApplyForce(Vector2(dx*100, dy*100));
	}

	void InputComponent::ProcessInputMouseDown(int state, float* Boost)
	{
		RigidComponent* mcc = mGameObject->GetComponent<RigidComponent>();
		// Define boost variables
		static const float kMaxBoost = 100.0f;
		static const float kBoostDecrease = 10.0f;	
		static const float kBoostRecoveryRate = 0.50f;
		static float mBoost = kMaxBoost;
		static bool mBoostEnabled = true;

		if (state == SDL_BUTTON_LEFT) {
			mcc->GetmBody()->ApplyAngularImpulse(10.0f,true);
		}
		// Decrease boost on right click if boost is enabled
		if (state == SDL_BUTTON_RIGHT && mBoostEnabled){
			b2Vec2 sjjs = mcc->GetmBody()->GetLinearVelocity();
			sjjs.x = 20 * sjjs.x / mcc->GetmBody()->GetLinearVelocity().Length();
			sjjs.y = 20 * sjjs.y / mcc->GetmBody()->GetLinearVelocity().Length();
			mcc->GetmBody()->SetLinearVelocity(mcc->GetmBody()->GetLinearVelocity() + sjjs);
			mBoost = std::max(0.0f, mBoost - kBoostDecrease);
			mBoostEnabled = (mBoost > 0.0f);
		}
		// Reset boost state when right click is released
		if (state == SDL_BUTTON_MIDDLE) {
			mBoostEnabled = true;
			mBoost = std::min(kMaxBoost, mBoost + kBoostRecoveryRate);
		}
		*Boost = mBoost;
	}

}