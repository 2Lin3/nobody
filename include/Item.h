#pragma once
#ifndef __Nobody_Pawn__
#define __Nobody_Pawn__

#include	<GameObject.h>
#include    <Player.h>

namespace Nobody
{
    enum class BoostType {
        SPEED_BOOST,
        MAX_HEALTH_BOOST,
        ATTACK_BOOST,
        CRITICAL_HIT_CHANCE_BOOST,
        CRITICAL_HIT_DAMAGE_BOOST
    };

    class Item {
    public:
        Item(BoostType type);
        BoostType GetType() const;
        void ApplyToPlayer(Player& player);

    private:
        BoostType type;
        float value;
        float uplife = 0;
    };
}

#endif	
