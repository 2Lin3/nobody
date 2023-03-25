#pragma once
#ifndef __Nobody_Pawn__
#define __Nobody_Pawn__

#include	<GameObject.h>
namespace Nobody
{
	class Pawn : public GameObject
	{
	public:
		//! ¹¹Ôìº¯Êý
		Pawn(class Game* game, b2World* world, const Vector2& playerPos);

		bool isValidPosition(float x, float y, const Vector2& playerPos);
		Vector2 GenerateRandomPosition(const Vector2& playerPos);
	};
}

#endif	// 