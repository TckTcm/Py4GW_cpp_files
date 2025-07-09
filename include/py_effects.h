#pragma once
#include "Headers.h"


namespace GW {
    namespace Effects {
        inline Effect* GetAgentEffectBySkillId(uint32_t agent_id, Constants::SkillID skill_id) {
            auto* effects = GetAgentEffects(agent_id);
            if (!effects) return nullptr;
            for (auto& effect : *effects) {
                if (effect.skill_id == skill_id)
                    return &effect;
            }
            return nullptr;
        }

        inline Buff* GetAgentBuffBySkillId(uint32_t agent_id, Constants::SkillID skill_id) {
            auto* buffs = GetAgentBuffs(agent_id);
            if (!buffs) return nullptr;
            for (auto& buff : *buffs) {
                if (buff.skill_id == skill_id)
                    return &buff;
            }
            return nullptr;
        }
        inline bool EffectExists(uint32_t agent_id, Constants::SkillID skill_id) {
            auto* effects = GetAgentEffects(agent_id);
            if (!effects) return false;
            for (auto& effect : *effects) {
                if (effect.skill_id == skill_id)
                    return true;
            }
            return false;
        }

        inline bool BuffExists(uint32_t agent_id, Constants::SkillID skill_id) {
            auto* buffs = GetAgentBuffs(agent_id);
            if (!buffs) return false;
            for (auto& buff : *buffs) {
                if (buff.skill_id == skill_id)
                    return true;
            }
            return false;
        }
    }
}


class EffectType {
public:
    int skill_id;  // Handle as int for Python
    int attribute_level;
    int effect_id;
    int agent_id;
    float duration;
    DWORD timestamp;
    DWORD time_elapsed;
    DWORD time_remaining;

    // Constructor that takes all properties explicitly, using Python-friendly int for skill_id
    EffectType(int skill_id, int attribute_level, int effect_id, int agent_id, float duration, DWORD timestamp, DWORD time_elapsed, DWORD time_remaining)
        : skill_id(skill_id), attribute_level(attribute_level), effect_id(effect_id), agent_id(agent_id), duration(duration), timestamp(timestamp), time_elapsed(time_elapsed), time_remaining(time_remaining) {
    }
};

class BuffType {
public:
    int skill_id;
    int buff_id;
    int target_agent_id;

    // Constructor that takes all properties explicitly, using Python-friendly int for skill_id
    BuffType(int skill_id, int buff_id, int target_agent_id)
        : skill_id(skill_id), buff_id(buff_id), target_agent_id(target_agent_id) {
    }
};

class AgentEffects {
    int agent_id;
    std::vector<EffectType> Effects_list;
    std::vector<BuffType> Buffs_list;

public:
    // Constructor initializes agent_id and fetches both effects and buffs
    AgentEffects(int agentid) : agent_id(agentid) {
        // Fetch effects for the agent
        auto instance_type = GW::Map::GetInstanceType();
        bool is_map_ready = (GW::Map::GetIsMapLoaded()) && (!GW::Map::GetIsObserving()) && (instance_type != GW::Constants::InstanceType::Loading);

        if (!is_map_ready) {
            Effects_list.clear();
            Buffs_list.clear();
            return;
        }


        auto* effects = GW::Effects::GetAgentEffects(static_cast<uint32_t>(agent_id));
        if (effects) {
            for (const auto& effect : *effects) {
                Effects_list.emplace_back(
                    static_cast<int>(effect.skill_id),
                    effect.attribute_level,
                    effect.effect_id,
                    effect.agent_id,
                    effect.duration,
                    effect.timestamp,
                    effect.GetTimeElapsed(),
                    effect.GetTimeRemaining()
                );
            }
        }

        // Fetch buffs for the agent
        auto* buffs = GW::Effects::GetAgentBuffs(static_cast<uint32_t>(agent_id));
        if (buffs) {
            for (const auto& buff : *buffs) {
                Buffs_list.emplace_back(
                    static_cast<int>(buff.skill_id),
                    buff.buff_id,
                    buff.target_agent_id
                );
            }
        }
    }

    // Fetch effects for the current agent and return as std::vector<EffectType>
    std::vector<EffectType> GetEffects() const {
        return Effects_list;
    }

    // Fetch buffs for the current agent and return as std::vector<BuffType>
    std::vector<BuffType> GetBuffs() const {
        return Buffs_list;
    }

    // Return the count of effects for the current agent
    int GetEffectCount() const {
        return Effects_list.size();
    }

    // Return the count of buffs for the current agent
    int GetBuffCount() const {
        return Buffs_list.size();
    }

    // Check if a specific effect exists for the current agent and skill ID
    bool EffectExists(int skill_id) const {
        for (const auto& effect : Effects_list) {
            if (effect.skill_id == skill_id) {
                return true;
            }
        }
        return false;
    }

    // Check if a specific buff exists for the current agent and skill ID
    bool BuffExists(int skill_id) const {
        for (const auto& buff : Buffs_list) {
            if (buff.skill_id == skill_id) {
                return true;
            }
        }
        return false;
    }

    //drop buff
    void DropBuff(int skill_id) {
        GW::Effects::DropBuff(static_cast<uint32_t>(skill_id));
    }

    static int GetDrunkennessLevel() {
        return static_cast<int>(GW::Effects::GetAlcoholLevel());
    }
    static void ApplyDrunkEffect(int intensity, int tint) {
        GW::Effects::GetDrunkAf(static_cast<uint32_t>(intensity), static_cast<uint32_t>(tint));
    }
};

