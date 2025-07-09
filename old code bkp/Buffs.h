#pragma once

#include "Headers.h"

#ifndef BUFFS_H
#define BUFFS_H

class BuffManager {
public:
    static const size_t MAX_BUFFS = 30;

    struct BuffData {
        GW::Constants::SkillID skillID;
    };

    BuffManager() {
        Clear();
    }

    void Register(GW::Constants::SkillID skillID) {
        Exists(skillID);
        for (size_t i = 0; i < MAX_BUFFS; i++) {
            if (buffData[i].skillID == GW::Constants::SkillID::No_Skill) {
                buffData[i].skillID = skillID;
                return;
            }
        }
    }

    void Clear() {
        for (size_t i = 0; i < MAX_BUFFS; i++) {
            buffData[i].skillID = GW::Constants::SkillID::No_Skill;
        }
    }

    bool Exists(GW::Constants::SkillID skillID) const {
        for (size_t i = 0; i < MAX_BUFFS; i++) {
            if (buffData[i].skillID == skillID) {
                return true;
            }
        }
        return false;
    }

    int GetBuffCount() const {
        int count = 0;
        for (size_t i = 0; i < MAX_BUFFS; i++) {
            if (buffData[i].skillID != GW::Constants::SkillID::No_Skill) {
                count++;
            }
        }
        return count;
    }

    void DisplayAllBuffs() {
        for (size_t i = 0; i < MAX_BUFFS; i++) {
            if (buffData[i].skillID != GW::Constants::SkillID::No_Skill) {
                ImGui::Text("Index: %d - Buff : % d", i, buffData[i].skillID);
            }
        }
    }

    void DisplayOrderedBuffs() const {
        ImGui::Text("Buff display Start");
        std::vector<BuffData> sortedBuffs;
        for (size_t i = 0; i < MAX_BUFFS; i++) {
            if (buffData[i].skillID != GW::Constants::SkillID::No_Skill) {
                sortedBuffs.push_back(buffData[i]);
            }
        }

        std::sort(sortedBuffs.begin(), sortedBuffs.end(), [](const BuffData& a, const BuffData& b) {
            return a.skillID < b.skillID;
            });

        for (const auto& buff : sortedBuffs) {
            NameClass skillnames;
            ImGui::Text("Buff: %s", skillnames.GetSkillNameByID(buff.skillID).c_str());
        }
        ImGui::Text("Buff display End");
    }


private:
    BuffData buffData[MAX_BUFFS];
};

#endif // BUFFS_H


#ifndef HERO_BUFF_MANAGER_H
#define HERO_BUFF_MANAGER_H

class HeroBuffManager {
public:
    static const size_t MAX_BUFFS = 30;

    struct BuffData {
        GW::AgentID agentID;
        GW::Constants::SkillID skillID;
    };

    HeroBuffManager() {
        Clear();
    }

    void Register(GW::AgentID agentID, GW::Constants::SkillID skillID) {
        //RemoveDuplicates();
        if (Exists(agentID, skillID)) {
            return; // Buff already exists, no need to add
        }

        for (size_t i = 0; i < MAX_BUFFS; ++i) {
            if (buffData[i].skillID == GW::Constants::SkillID::No_Skill) {
                buffData[i].agentID = agentID;
                buffData[i].skillID = skillID;
                return;
            }
        }
    }

    void Clear() {
        for (size_t i = 0; i < MAX_BUFFS; ++i) {
            buffData[i].agentID = 0;
            buffData[i].skillID = GW::Constants::SkillID::No_Skill;
        }
    }

    bool Exists(GW::AgentID agentID, GW::Constants::SkillID skillID) const {
        for (size_t i = 0; i < MAX_BUFFS; ++i) {
            if (buffData[i].agentID == agentID && buffData[i].skillID == skillID) {
                return true;
            }
        }
        return false;
    }

    int GetBuffCount() const {
        int count = 0;
        for (size_t i = 0; i < MAX_BUFFS; ++i) {
            if (buffData[i].agentID != 0 && buffData[i].skillID != GW::Constants::SkillID::No_Skill) {
                count++;
            }
        }
        return count;
    }

    void DisplayAllBuffs() {
        for (size_t i = 0; i < MAX_BUFFS; ++i) {
            if (buffData[i].agentID != 0 && buffData[i].skillID != GW::Constants::SkillID::No_Skill) {
                ImGui::Text("Index: %d  - AgentID: %d - Buff : % d",i, buffData[i].agentID, buffData[i].skillID);
            }
        }
    }

    void DisplayOrderedBuffs() const {
        ImGui::Text("Buff display Start");
        std::vector<BuffData> sortedBuffs;
        for (size_t i = 0; i < MAX_BUFFS; ++i) {
            if (buffData[i].agentID != 0 && buffData[i].skillID != GW::Constants::SkillID::No_Skill) {
                sortedBuffs.push_back(buffData[i]);
            }
        }

        std::sort(sortedBuffs.begin(), sortedBuffs.end(), [](const BuffData& a, const BuffData& b) {
            return a.agentID < b.agentID;
            });

        GW::AgentID currentAgentID = 0;
        for (const auto& buff : sortedBuffs) {
            if (buff.agentID != currentAgentID) {
                currentAgentID = buff.agentID;
                std::string skillheader = "AgentID: ";
                skillheader += std::to_string(static_cast<int>(currentAgentID));
                if (ImGui::CollapsingHeader(skillheader.c_str())) {
                    for (const auto& innerBuff : sortedBuffs) {
                        if (innerBuff.agentID == currentAgentID) {
                            NameClass skillnames;
                            ImGui::Text("Buff: %s", skillnames.GetSkillNameByID(innerBuff.skillID).c_str());
                        }
                    }
                }
                ImGui::Separator();
            }
        }
        ImGui::Text("Buff display End");
    }


private:
    BuffData buffData[MAX_BUFFS];
};

#endif // HERO_BUFF_MANAGER_H

