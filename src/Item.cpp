#include "Item.h"

namespace Nobody
{
    Item::Item(BoostType type) {
        this->type = type;

        switch (type) {
        case BoostType::Accelerate_Capacity_BOOST:
            this->value = 10.0; // 设定速度提升的数值
            break;
        case BoostType::MAX_HEALTH_BOOST:
            this->value = 20.0; // 设定最大生命值提升的数值
            break;
        case BoostType::ATTACK_BOOST:
            this->value = 0.1; // 设定攻击力提升的数值
            break;
        case BoostType::CRITICAL_HIT_CHANCE_BOOST:
            this->value = 0.1; // 设定暴击几率提升的数值
            break;
        case BoostType::CRITICAL_HIT_DAMAGE_BOOST:
            this->value = 0.3; // 设定暴击伤害提升的数值
            break;
        default:
            break;
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