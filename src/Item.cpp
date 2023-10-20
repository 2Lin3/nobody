#include "Item.h"

namespace Nobody
{
    Item::Item(BoostType type) {
        this->type = type;

        switch (type) {
        case BoostType::Accelerate_Capacity_BOOST:
            this->value = 10.0; // 扢隅厒僅枑汔腔杅硉
            break;
        case BoostType::MAX_HEALTH_BOOST:
            this->value = 20.0; // 扢隅郔湮汜韜硉枑汔腔杅硉
            break;
        case BoostType::ATTACK_BOOST:
            this->value = 0.1; // 扢隅馴僻薯枑汔腔杅硉
            break;
        case BoostType::CRITICAL_HIT_CHANCE_BOOST:
            this->value = 0.1; // 扢隅惟僻撓薹枑汔腔杅硉
            break;
        case BoostType::CRITICAL_HIT_DAMAGE_BOOST:
            this->value = 0.3; // 扢隅惟僻夼漲枑汔腔杅硉
            break;
        default:
            break;
        }
    }

    std::string Item::getName() const{
        switch (type) {
        case BoostType::Accelerate_Capacity_BOOST:
            return "Accelerate Capacity Boost";
        case BoostType::MAX_HEALTH_BOOST:
            return "Max Health Boost";
        case BoostType::ATTACK_BOOST:
			return "Attack Boost";
        case BoostType::CRITICAL_HIT_CHANCE_BOOST:
			return "Critical Hit Chance Boost";
        case BoostType::CRITICAL_HIT_DAMAGE_BOOST:
            return "Critical Hit Damage Boost";
        default:
            return "Unknown Item";
        }

    }


    BoostType Item::GetType() const
    {
        return this->type;
    }

    void Item::ApplyToPlayer(Player& player) {
        switch (type) {
        case BoostType::Accelerate_Capacity_BOOST:
            player.SetBoostValue(*player.GetBoostValue() + value);
            break;
        case BoostType::MAX_HEALTH_BOOST:
            uplife = *player.getUpLimitLife() - player.GetLife();
            player.SetUpLimitLife(*player.getUpLimitLife() + value);
            player.SetLife(player.GetLife() + uplife);
            break;
        case BoostType::ATTACK_BOOST:
            player.SetDamage_rate(*player.GetDamage_rate() + value);
            break;
        case BoostType::CRITICAL_HIT_CHANCE_BOOST:
            player.SetCrit_rate(*player.GetCrit_rate() + value);
            break;
        case BoostType::CRITICAL_HIT_DAMAGE_BOOST:
            player.SetCrit_damage(*player.GetCrit_damage() + value);
            break;
        default:
            break;
        }
    }

}