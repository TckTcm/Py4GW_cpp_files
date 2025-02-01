#pragma once

#include "Headers.h"

typedef float(__cdecl* ScreenToWorldPoint_pt)(GW::Vec3f* vec, float screen_x, float screen_y, int unk1);
typedef uint32_t(__cdecl* MapIntersect_pt)(GW::Vec3f* origin, GW::Vec3f* unit_direction, GW::Vec3f* hit_point, int* propLayer);


struct vectorFieldStruct {
    float CollisionRadius;
    float AttractionStrength;
    float RepulsionStrength;
};

struct FieldsClass {
    vectorFieldStruct Agent;
    vectorFieldStruct Party;
    vectorFieldStruct Leader;
    vectorFieldStruct Spirit;
    vectorFieldStruct Item;
    vectorFieldStruct Npc;
    vectorFieldStruct Enemy;
};

struct ExperimentalFlagsClass {
    bool LeaderPing;
    bool DumbFollow;
    bool Debug;
    bool Bypass;
    bool AreaRings;
    bool Energy;
    bool SkillData;
    bool MemMon;
    bool SinStrikeMon;
    bool PartyCandidates;
    bool CreateParty;
    bool Pcons;
    bool Title;
    bool resign;
    bool TakeQuest;
    bool Openchest;
    bool IdItems;
    bool SalvageItems;
    bool CallWin;
    bool Pathing;
    bool Collisions;
    bool TargettingDebug;
    bool BuffDebug;
    bool FormationA;
    bool FormationB;
    bool FormationC;
    bool FormationD;
    bool FormationE;
};

class GameStateClass {
private:
    struct StateMachineStruct {
    public:
        eState State = Idle;
        clock_t LastActivity;

        bool Following = true;
        bool Looting = true;
        bool Combat = true;
        bool Targetting = true;
        bool Collision = true;
        bool HeroFlag = false;
        bool isFollowingEnabled() const { return Following; }
        bool isLootingEnabled() const { return Looting; }
        bool isCombatEnabled() const { return Combat; }
        bool isTargettingEnabled() const { return Targetting; }
        bool isCollisionEnabled() const { return Collision; }
        bool isHeroFlagEnabled() const { return HeroFlag; }

        void toggleFollowing() { Following = !Following; }
        void toggleLooting() { Looting = !Looting; }
        void toggleCombat() { Combat = !Combat; }
        void toggleTargetting() { Targetting = !Targetting; }
        void toggleCollision() { Collision = !Collision; }
        void toggleHeroFlag() { HeroFlag = !HeroFlag; }
        float RangedRangeValue = GW::Constants::Range::Area;
        float MeleeRangeValue = GW::Constants::Range::Spellcast;
        FieldsClass MemFields;
    };

    struct CombatStateMachine {
        combatState State;
        Timer LastActivity;
        Timer StayAlert;
        //uint32_t castingptr = 0;
        uint32_t currentCastingSlot;
        bool IsSkillCasting = false;
        float IntervalSkillCasting;
    };
public:
    StateMachineStruct state;
    CombatStateMachine combat;
    bool CombatSkillState[8] = { true,true,true,true,true,true,true,true };

};


class MemMgrClass {
private:
    ScreenToWorldPoint_pt ptrScreenToWorldPoint_Func = 0;
    MapIntersect_pt ptrMapIntersect_Func = 0;

    float* GetC2M_MouseX();
    float* GetC2M_MouseY();

public:
    uintptr_t GetBasePtr();
    uintptr_t GetNdcScreenCoordsPtr();
    ScreenToWorldPoint_pt GetScreenToWorldPtr();
    MapIntersect_pt GetMapIntersectPtr();
    GW::Vec2f GetClickToMoveCoords();

    
    struct MemSinglePlayer {
        GW::AgentID ID = 0;
        float Energy = 0.0f;
        float energyRegen = 0.0f;
        bool IsActive = false;
        bool IsHero = false;
        bool IsFlagged;
        bool IsAllFlag;
        GW::Vec2f FlagPos;
        Timer LastActivity;
        uint32_t Keepalivecounter = 0;
        BuffManager BuffList;
    };

    struct MemPlayerStruct {
        MemSinglePlayer MemPlayers[8];
        HeroBuffManager HeroBuffs;
        GameStateClass GameState[8];
        RemoteCommand command[8];
        CandidateArray Candidates;
        PartyCandidates  NewCandidates;
        uint32_t LeaderID = 0;
    };

    HANDLE hMapFile;
    HANDLE hMutex;
    
    void InitializeSharedMemory(MemPlayerStruct*& pData);

    MemPlayerStruct* MemData;

    void MutexAquire();
    void MutexRelease();
    
};
