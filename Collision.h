#pragma once
#pragma once
//! @file Component.h

#ifndef __Nobody_Collision__
#define __Nobody_Collision__

#include	<string>
#include	<vector>
#include	<box2d/box2d.h>
#include    <GameObject.h>
#include    <RigidComponent.h>
namespace Nobody
{
	class Collision : public b2ContactListener 
	{
    public:
        void BeginContact(b2Contact* contact);

        void EndContact(b2Contact* contact);

    private:
        struct CollisionEvent {
            uintptr_t  fixtureA;
            uintptr_t  fixtureB;
        };
        std::vector<CollisionEvent> collisionEvents;
        b2Body* myBody;
	};
}

#endif	// 