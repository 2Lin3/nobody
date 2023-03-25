//! @file Component.cpp

#include"Collision.h"
#include<GameObject.h>
#include<box2d/box2d.h>
#include<RigidComponent.h>
#include<iostream>
namespace Nobody
{
    void Collision::BeginContact(b2Contact* contact) {
        // 获取碰撞事件中涉及的两个fixture的唯一标识符
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();

        if (fixtureA == nullptr || fixtureB == nullptr) {
            return;
        }

        uintptr_t fixtureIdA = reinterpret_cast<uintptr_t>(fixtureA);
        uintptr_t fixtureIdB = reinterpret_cast<uintptr_t>(fixtureB);

        // 输出碰撞信息
        std::cout << "Collision detected between fixture " << fixtureIdA << " and fixture " << fixtureIdB << std::endl;

        // 将碰撞事件添加到数据结构中
        CollisionEvent event = { fixtureIdA, fixtureIdB };
        collisionEvents.push_back(event);
   
    }

    void Collision::EndContact(b2Contact* contact) {
        // 处理所有与fixture相关的碰撞事件
        for (const CollisionEvent& event : collisionEvents) {
            b2Fixture* fixtureA = reinterpret_cast<b2Fixture*>(event.fixtureA);
            b2Fixture* fixtureB = reinterpret_cast<b2Fixture*>(event.fixtureB);

            if (fixtureA == nullptr || fixtureB == nullptr) {
                continue;
            }

            GameObject* gameObjectA = reinterpret_cast<GameObject*>(fixtureA->GetBody()->GetUserData().pointer);
            GameObject* gameObjectB = reinterpret_cast<GameObject*>(fixtureB->GetBody()->GetUserData().pointer);

            if (fixtureA->GetBody()->GetLinearVelocity().Length() > fixtureB->GetBody()->GetLinearVelocity().Length() * 1.05) {
                float minus = fixtureA->GetBody()->GetLinearVelocity().Length() - fixtureB->GetBody()->GetLinearVelocity().Length();
                gameObjectB->SetLife(gameObjectB->GetLife() - minus);
            }

            if (fixtureA->GetBody()->GetLinearVelocity().Length() * 1.05 < fixtureB->GetBody()->GetLinearVelocity().Length()) {
                float minus = -fixtureA->GetBody()->GetLinearVelocity().Length() + fixtureB->GetBody()->GetLinearVelocity().Length();
                gameObjectA->SetLife(gameObjectA->GetLife() - 0.3 * minus);
            }
        }

        // 从数据结构中删除所有与fixture相关的碰撞事件
        collisionEvents.clear();
    }
}