//! @file InputComponent.h

#ifndef __Nobody_InputComponent__
#define __Nobody_InputComponent__

#include	<Component.h>

namespace Nobody
{
	class GameObject;
	class InputComponent : public Component
	{
	public:
		//! 构造函数
		InputComponent(GameObject* gameObject);
		// 记录上一帧的鼠标位置
		int last_mouse_x = 0;
		int last_mouse_y = 0;
		//! 处理输入（重写）
		void	ProcessInput(const uint8_t* state) override;
		void	ProcessInputMouse(float mousex, float mousey)override;
		void	ProcessInputMouseDown(int state, float* mBoost)override;

	};
}

#endif