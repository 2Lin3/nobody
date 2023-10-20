#pragma once
#include <GameObject.h>

namespace Nobody
{
	class Background : public GameObject
	{
	public:
		Background(Game* game, b2World* world);
	};
}
