#pragma once

#include "Headers.h"
namespace fs = std::filesystem; 
using json = nlohmann::json; 


//class HeroAI; //forward declaration fo i can create friendship

class CustomSkillClass {
private:
    static const int MaxSkillData = 1350;
    void serializeSpecialSkills(const std::string& filename);
    void deserializeSpecialSkills(const std::string& filename);
    void InitSkillData();
    
public:
    struct castConditions {
        bool IsAlive = true;
        bool HasCondition = false;
        bool HasDeepWound = false;
        bool HasCrippled = false;
        bool HasBleeding = false;
        bool HasPoison = false;
        bool HasWeaponSpell = false;
        bool HasEnchantment = false;
        bool HasHex = false;
        bool IsCasting = false;
        bool IsNockedDown = false;
        bool IsMoving = false;
        bool IsAttacking = false;
        bool IsHoldingItem = false;
        bool TargettingStrict = false;
        float LessLife = 0;
        float MoreLife = 0;
        float LessEnergy = 0;
        float SacrificeHealth = 0.5f;
        bool IsPartyWide = false;
        bool UniqueProperty = false;
        bool IsOutOfCombat = false;

    };

    struct CustomSkillDatatype {
        public:
            GW::Constants::SkillID SkillID = GW::Constants::SkillID::No_Skill;
            GW::Constants::SkillType SkillType;
            SkillTarget  TargetAllegiance;
            SkillNature Nature = Offensive;
            castConditions Conditions;
    };

private:

    CustomSkillDatatype Skills[MaxSkillData];

public:
    void Init(std::string file_dest);
 
    int GetMaxSkillData() {return MaxSkillData;}
    int GetPtrBySkillID(GW::Constants::SkillID SkillID);  
    CustomSkillDatatype GetSkillByPtr(int ptr) { return Skills[ptr]; }  
};

