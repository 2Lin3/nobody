//! @file Player.h

#ifndef __Nobody_Boundary__
#define __Nobody_Boundary__

#include	<GameObject.h>

namespace Nobody
{
	class Boundary : public GameObject
	{
	public:
		//! 构造函数
		Boundary(class Game* game, b2World* world);

	private:
		b2FixtureDef m_boundaryFixtureDef;
	};
}
#endif	