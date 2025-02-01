#pragma once

#include "Headers.h"

class NameClass {
public:
    struct NameStruct {
        std::string Name;
    };

    NameClass();
    NameClass(int id);

    void InitValues(int id);

    std::string GetSkillNameByID(GW::Constants::SkillID skillID);
    GW::Constants::SkillID GetSkillIDByName(const std::string& Name);

    std::string GetTypeByID(GW::Constants::SkillType typeID);
    GW::Constants::SkillType GetIDByType(const std::string& Name);

    std::string GetNatureByID(SkillNature natureID);
    SkillNature GetIDByNature(const std::string& Name);

    std::string GetTargetByID(SkillTarget targetID);
    SkillTarget GetIDByTarget(const std::string& Name);

    NameStruct Skills[3500];
    NameStruct SkillTypes[30];
    NameStruct SSkillTarget[20];
    NameStruct SSkillNature[15];
};


