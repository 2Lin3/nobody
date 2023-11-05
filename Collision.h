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
#include    "AudioManager.h"

namespace Nobody
{
	class Collision : public b2ContactListener 
	{
        AudioManager* mAudioManager;  // AudioManager 的指针
    public:
        void BeginContact(b2Contact* contact);
        Collision(AudioManager* audioManager);
        void EndContact(b2Contact* contact);

    private:
        struct CollisionEvent {
            uintptr_t  fixtureA;
            uintptr_t  fixtureB;
        };
        std::vector<CollisionEvent> collisionEvents;
        b2Body* myBody;
        const int scene_width = 1280;
        const int scene_height = 800;	//!<	窗口规格
	};
}

#endif	// 