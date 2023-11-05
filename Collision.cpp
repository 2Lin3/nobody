//! @file Component.cpp

#include"Collision.h"
#include<GameObject.h>
#include<box2d/box2d.h>
#include<RigidComponent.h>
#include<iostream>
#include<Player.h>
#include "AudioManager.h"
namespace Nobody
{
    Collision::Collision(AudioManager* audioManager) : mAudioManager(audioManager) {}
    void Collision::BeginContact(b2Contact* contact) {
        // ��ȡ��ײ�¼����漰������fixture��Ψһ��ʶ��
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();
        if (fixtureA == nullptr || fixtureB == nullptr) {
            return;
        }
        uintptr_t fixtureIdA = reinterpret_cast<uintptr_t>(fixtureA);
        uintptr_t fixtureIdB = reinterpret_cast<uintptr_t>(fixtureB);
        
        // ����ײ�¼���ӵ����ݽṹ��
        CollisionEvent event = { fixtureIdA, fixtureIdB };
        collisionEvents.push_back(event);
        // ����������fixture��ص���ײ�¼�
        for (const CollisionEvent& event : collisionEvents) {
            b2Fixture* fixtureA = reinterpret_cast<b2Fixture*>(event.fixtureA);
            b2Fixture* fixtureB = reinterpret_cast<b2Fixture*>(event.fixtureB);

            if (fixtureA == nullptr || fixtureB == nullptr) {
                continue;
            }
            // ��������Ӵ�����
            b2WorldManifold worldManifold;
            contact->GetWorldManifold(&worldManifold);

            // ȡ��һ����ײ��
            if (contact->GetManifold()->pointCount > 0) {
                const b2Vec2& collisionPoint = worldManifold.points[0];
                float x = collisionPoint.x; // ��ײ���X����
                float y = collisionPoint.y; // ��ײ���Y����

                // ��������ƽ��
                Uint8 left, right;
                // ����listener��λ������Ļ���ģ���Ҫ������Ӧ��x��y���� 
                float listenerX = scene_width / 2;  // ��Ļ��ȵ�һ��
                float listenerY = scene_height / 2; // ��Ļ�߶ȵ�һ��
                AudioManager::Instance().CalculateStereoPan(x, y, listenerX, listenerY, left, right);

                // �������Ч��
                Mix_Chunk* soundEffect = mAudioManager->GetRandomSoundEffect();
                //soundEffect = AudioManager::Instance().LoadSoundEffect("music/Sound effects/2.wav");
                // ����������ƽ��
                int channel = Mix_PlayChannel(-1, soundEffect, 0);
                if (channel != -1) {
                    Mix_SetPanning(channel, left, right);
                }
            }
            
            GameObject* gameObjectA = reinterpret_cast<GameObject*>(fixtureA->GetBody()->GetUserData().pointer);
            GameObject* gameObjectB = reinterpret_cast<GameObject*>(fixtureB->GetBody()->GetUserData().pointer);
            bool AisPlayer = false;
            bool BisPlayer = false;
            Player* playerA = dynamic_cast<Player*>(gameObjectA);
            Player* playerB = dynamic_cast<Player*>(gameObjectB);
            float damage;
            if (playerA) { // ��� gameObjectA �� Player
                AisPlayer = true;
                float critChance = *playerA->GetCrit_rate();
                float critDamage = *playerA->GetCrit_damage();
                float damageRate = *playerA->GetDamage_rate();
                // �����жϺ��˺�����
                if (static_cast<float>(rand()) / RAND_MAX < critChance) {
                    damage = critDamage * damageRate;
                    //std::cout << "Crit" << std::endl;
                }
                else {
                    damage = damageRate;
                }
            }
            if (playerB) { // ��� gameObjectB �� Player
                BisPlayer = true;
                float critChance = *playerB->GetCrit_rate();
                float critDamage = *playerB->GetCrit_damage();
                float damageRate = *playerB->GetDamage_rate();
                // �����жϺ��˺�����
                if (static_cast<float>(rand()) / RAND_MAX < critChance) {
                    damage = critDamage * damageRate;
                    //std::cout << "Crit" << std::endl;
                }
                else {
                    damage = damageRate;
                }
            }
            //AudioManager::Instance().PlaySoundEffect(AudioManager::Instance().LoadSoundEffect("music/Sound effects/1.wav"), 0);
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


        // �����ݽṹ��ɾ��������fixture��ص���ײ�¼�
        collisionEvents.clear();
    }
}