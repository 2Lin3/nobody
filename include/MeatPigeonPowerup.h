#pragma once

#include "Powerup.h"

class MeatPigeonPowerup : public Powerup {
public:
    enum Type {
        
        SPEED_BOOST,
        INVINCIBILITY,
        HEALTH_BOOST,
        MAX_HEALTH_BOOST,
        ATTACK_BOOST,
        DEFENSE_BOOST,
        CRITICAL_HIT_CHANCE_BOOST,
        CRITICAL_HIT_DAMAGE_BOOST,
        ATTACK_SPEED_BOOST,
        MOVEMENT_SPEED_BOOST
    };

    MeatPigeonPowerup(Type type);

    void ApplyPowerup(Player& player) override;

private:
    Type _type;
};
