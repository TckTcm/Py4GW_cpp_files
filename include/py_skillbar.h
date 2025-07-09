#pragma once
#include "Headers.h"


struct SkillbarSkill {
    SkillID ID;         // Skill ID
    int adrenaline_a;   // Adrenaline (part A)
    int adrenaline_b;   // Adrenaline (part B)
    int recharge;       // Recharge time
    int event;          // Event associated with the skill
    uint32_t get_recharge; // Get recharge time

    // Constructor to initialize the SkillbarSkill
    SkillbarSkill(SkillID id, int adrenaline_a = 0, int adrenaline_b = 0, int recharge = 0, int event = 0, uint32_t get_recharge = 0)
        : ID(id), adrenaline_a(adrenaline_a), adrenaline_b(adrenaline_b), recharge(recharge), event(event), get_recharge(get_recharge)
    {
    }
};



class Skillbar {
public:
    std::vector<SkillbarSkill> skills;
    uint32_t agent_id = 0;
    uint32_t disabled = 0;
    uint32_t casting = 0;


    Skillbar() {
        GetContext();
    }

    void ResetContext() {
        agent_id = 0;
        disabled = 0;
        casting = 0;
        skills.clear();
    }

    void GetContext() {
        auto instance_type = GW::Map::GetInstanceType();
        bool is_map_ready = (GW::Map::GetIsMapLoaded()) && (!GW::Map::GetIsObserving()) && (instance_type != GW::Constants::InstanceType::Loading);

        if (!is_map_ready) {
            ResetContext();
            return;
        }

        auto player_skillbar = GW::SkillbarMgr::GetPlayerSkillbar();
        if (!player_skillbar) return;

        agent_id = player_skillbar->agent_id;
        disabled = player_skillbar->disabled;
        casting = player_skillbar->casting;

        skills.clear();
        for (int i = 0; i < 8; ++i) {
            SkillID skill_id(static_cast<int>(player_skillbar->skills[i].skill_id));
            int adrenaline_a = player_skillbar->skills[i].adrenaline_a;
            int adrenaline_b = player_skillbar->skills[i].adrenaline_b;
            int recharge = player_skillbar->skills[i].recharge;
            int event = player_skillbar->skills[i].event;
            int get_recharge = player_skillbar->skills[i].GetRecharge();
            skills.emplace_back(SkillbarSkill(skill_id, adrenaline_a, adrenaline_b, recharge, event, get_recharge));

        }
    }

    SkillbarSkill get_skill(int index) const {
        if (index < 1 || index > 8) {
            throw std::out_of_range("Skill index out of range (must be between 1 and 8) Given: " + std::to_string(index));
        }
        return skills[index - 1];  // Convert to 0-based index
    }

    bool LoadSkilltemplate(std::string skill_template) {
        return GW::SkillbarMgr::LoadSkillTemplate(skill_template.c_str());
    }

    bool LoadHeroSkillTemplate(uint32_t hero_index, std::string skill_template) {
        return GW::SkillbarMgr::LoadSkillTemplate(skill_template.c_str(), hero_index);
    }

    void UseSkill(uint32_t slot, uint32_t target = 0) {

        if (slot < 1 || slot > 8) {
            throw std::out_of_range("Skill index out of range (must be between 1 and 8) Given: " + std::to_string(slot));
        }
        uint32_t zeroslot = slot - 1;

        GW::GameThread::Enqueue([zeroslot, target] {
            GW::SkillbarMgr::UseSkill(zeroslot, target);
            });

    }

    void PointBlankUseSkill(uint32_t slot) {
        if (slot < 1 || slot > 8) {
            throw std::out_of_range("Skill index out of range (must be between 1 and 8) Given: " + std::to_string(slot));
        }
        uint32_t zeroslot = slot - 1;
        GW::GameThread::Enqueue([zeroslot] {
            GW::SkillbarMgr::PointBlankUseSkill(zeroslot);
            });
    }

    bool HeroUseSkill(const uint32_t target_agent_id, const uint32_t skill_number, const uint32_t hero_idx) {
        uint32_t skill_idx = skill_number - 1;
        uint32_t hero_idx_zero_based = hero_idx - 1;
        auto hero_action = GW::UI::ControlAction_Hero1Skill1;
        if (hero_idx_zero_based == 0)
            hero_action = GW::UI::ControlAction_Hero1Skill1;
        else if (hero_idx_zero_based == 1)
            hero_action = GW::UI::ControlAction_Hero2Skill1;
        else if (hero_idx_zero_based == 2)
            hero_action = GW::UI::ControlAction_Hero3Skill1;
        else if (hero_idx_zero_based == 3)
            hero_action = GW::UI::ControlAction_Hero4Skill1;
        else if (hero_idx_zero_based == 4)
            hero_action = GW::UI::ControlAction_Hero5Skill1;
        else if (hero_idx_zero_based == 5)
            hero_action = GW::UI::ControlAction_Hero6Skill1;
        else if (hero_idx_zero_based == 6)
            hero_action = GW::UI::ControlAction_Hero7Skill1;
        else
            return false;

        const auto curr_target_id = GW::Agents::GetTargetId();
        auto success = true;

        GW::GameThread::Enqueue([=, &success] {
            if (target_agent_id && target_agent_id != GW::Agents::GetTargetId())
                success &= GW::Agents::ChangeTarget(target_agent_id);
            const auto keypress_id = (GW::UI::ControlAction)(static_cast<uint32_t>(hero_action) + skill_idx);
            success &= GW::UI::Keypress(keypress_id);
            if (curr_target_id && target_agent_id != curr_target_id)
                success &= GW::Agents::ChangeTarget(curr_target_id);
            });

        return success;

    }

    bool ChangeHeroSecondary(uint32_t hero_index, uint32_t profession) {
        return GW::SkillbarMgr::ChangeSecondProfession(static_cast<GW::Constants::Profession>(profession), hero_index);
    }

    std::vector<SkillbarSkill> GetHeroSkillbar(uint32_t hero_index) {
        std::vector<SkillbarSkill> hero_skills;
        auto hero_skillbar = GW::SkillbarMgr::GetHeroSkillbar(hero_index);
        if (!hero_skillbar) return hero_skills;

        for (int i = 0; i < 8; ++i) {
            SkillID skill_id(static_cast<int>(hero_skillbar->skills[i].skill_id));
            int adrenaline_a = hero_skillbar->skills[i].adrenaline_a;
            int adrenaline_b = hero_skillbar->skills[i].adrenaline_b;
            int recharge = hero_skillbar->skills[i].recharge;
            int event = hero_skillbar->skills[i].event;
            hero_skills.emplace_back(SkillbarSkill(skill_id, adrenaline_a, adrenaline_b, recharge, event));
        }
        return hero_skills;
    }

    int GetHoveredSkill() {
        auto skill = GW::SkillbarMgr::GetHoveredSkill();
        if (skill) {
            return static_cast<int>(skill->skill_id);
        }
        return 0;
    }

    bool IsSkillUnlocked(int skill_id) {
        return GW::SkillbarMgr::GetIsSkillUnlocked(static_cast<GW::Constants::SkillID>(skill_id));
    }

    bool IsSkillLearnt(int skill_id) {
        return GW::SkillbarMgr::GetIsSkillLearnt(static_cast<GW::Constants::SkillID>(skill_id));
    }
};
