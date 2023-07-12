//! @file Component.cpp

#include"Collision.h"
#include<GameObject.h>
#include<box2d/box2d.h>
#include<RigidComponent.h>
#include<iostream>
#include<Player.h>
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
        //std::cout << "Collision detected between fixture " << fixtureIdA << " and fixture " << fixtureIdB << std::endl;

        // 将碰撞事件添加到数据结构中
        CollisionEvent event = { fixtureIdA, fixtureIdB };
        collisionEvents.push_back(event);
        // 处理所有与fixture相关的碰撞事件
        for (const CollisionEvent& event : collisionEvents) {
            b2Fixture* fixtureA = reinterpret_cast<b2Fixture*>(event.fixtureA);
            b2Fixture* fixtureB = reinterpret_cast<b2Fixture*>(event.fixtureB);

            if (fixtureA == nullptr || fixtureB == nullptr) {
                continue;
            }

            GameObject* gameObjectA = reinterpret_cast<GameObject*>(fixtureA->GetBody()->GetUserData().pointer);
            GameObject* gameObjectB = reinterpret_cast<GameObject*>(fixtureB->GetBody()->GetUserData().pointer);
            bool AisPlayer = false;
            bool BisPlayer = false;
            Player* playerA = dynamic_cast<Player*>(gameObjectA);
            Player* playerB = dynamic_cast<Player*>(gameObjectB);
            float damage;
            if (playerA) { // 如果 gameObjectA 是 Player
                AisPlayer = true;
                float critChance = *playerA->GetCrit_rate();
                float critDamage = *playerA->GetCrit_damage();
                float damageRate = *playerA->GetDamage_rate();
                // 暴击判断和伤害计算
                if (static_cast<float>(rand()) / RAND_MAX < critChance) {
                    damage = critDamage * damageRate;
                    //std::cout << "Crit" << std::endl;
                }
                else {
                    damage = damageRate;
                }
            }
            if (playerB) { // 如果 gameObjectB 是 Player
                BisPlayer = true;
                float critChance = *playerB->GetCrit_rate();
                float critDamage = *playerB->GetCrit_damage();
                float damageRate = *playerB->GetDamage_rate();
                // 暴击判断和伤害计算
                if (static_cast<float>(rand()) / RAND_MAX < critChance) {
                    damage = critDamage * damageRate;
                    //std::cout << "Crit" << std::endl;
                }
                else {
                    damage = damageRate;
                }
            }

            if (fixtureA->GetBody()->GetLinearVelocity().Length() > fixtureB->GetBody()->GetLinearVelocity().Length() * 1.05) {
                float minus = fixtureA->GetBody()->GetLinearVelocity().Length() - fixtureB->GetBody()->GetLinearVelocity().Length();
                if(!AisPlayer) damage = 1;
				gameObjectB->SetLife(gameObjectB->GetLife() - 0.3 * minus * damage);
                //std::cout << "B attaced" << fixtureA->GetBody()->GetLinearVelocity().Length() << "   " << fixtureB->GetBody()->GetLinearVelocity().Length() << std::endl;
            }

            if (fixtureA->GetBody()->GetLinearVelocity().Length() * 1.05 < fixtureB->GetBody()->GetLinearVelocity().Length()) {
                float minus = -fixtureA->GetBody()->GetLinearVelocity().Length() + fixtureB->GetBody()->GetLinearVelocity().Length();
                if (!BisPlayer) damage = 1;
                gameObjectA->SetLife(gameObjectA->GetLife() - 0.3 * minus * damage);
                //std::cout << "A attaced" << fixtureA->GetBody()->GetLinearVelocity().Length() << "   " << fixtureB->GetBody()->GetLinearVelocity().Length() << std::endl;
            }
        }
    }

    void Collision::EndContact(b2Contact* contact) {


        // 从数据结构中删除所有与fixture相关的碰撞事件
        collisionEvents.clear();
    }
}