#pragma once
#ifndef __Nobody_Item__
#define __Nobody_Item__

#include	<GameObject.h>
#include    <Player.h>

namespace Nobody
{
    enum class BoostType {
        Accelerate_Capacity_BOOST,
        MAX_HEALTH_BOOST,
        ATTACK_BOOST,
        CRITICAL_HIT_CHANCE_BOOST,
        CRITICAL_HIT_DAMAGE_BOOST,
        End  
    };


    class Item {
    public:
        Item(BoostType type);
        BoostType GetType() const;
        void ApplyToPlayer(Player& player);
        std::string getName() const;

    private:
        BoostType type;
        float value;
        float uplife = 0;
    };
}

#endif	
