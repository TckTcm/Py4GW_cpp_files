#pragma once
#include "Headers.h"

#ifndef HeroAI_CLASS_H
#define HeroAI_CLASS_H


struct PythonVarNode {
	bool value = false;
	bool command_issued = false;
	bool bool_parameter = false;
	uint32_t parameter_1 = 0;
	uint32_t parameter_2 = 0;
	float parameter_3 = 0;
	float parameter_4 = 0;
};

class PythonVarsClass {
public:
	bool is_active = false;
	int agent_id = 0;
	float energy = 0.0f;
	float energy_regen = 0.0f;
	PythonVarNode Looting;
	PythonVarNode Following;
	PythonVarNode Combat;
	PythonVarNode Collision;
	PythonVarNode Targetting;
	PythonVarNode Skills[8];
	PythonVarNode Flagging;
	PythonVarNode UnFlagging;
	PythonVarNode Salvage;
	PythonVarNode Identify;
	PythonVarNode Resign;
	PythonVarNode TakeQuest;
	PythonVarNode CustomMessage;
};

class HeroAI {
public:

	struct MapStruct {
		bool IsExplorable = false;
		bool IsLoading = false;
		bool IsOutpost = false;
		bool IsPartyLoaded = false;
		bool IsMapReady = false;
	};

	struct AgentTargetStruct {
		//GW::Agent* Agent = 0;
		//GW::AgentID ID = 0;
		int ID = 0;
		//GW::AgentLiving* Living = 0;
	};

	struct TargetStruct {
		AgentTargetStruct Self;
		AgentTargetStruct Called;
		AgentTargetStruct Nearest;
		AgentTargetStruct NearestMartial;
		AgentTargetStruct NearestMartialMelee;
		AgentTargetStruct NearestMartialRanged;
		AgentTargetStruct NearestCaster;
		AgentTargetStruct LowestAlly;
		AgentTargetStruct LowestAllyMartial;
		AgentTargetStruct LowestAllyMartialMelee;
		AgentTargetStruct LowestAllyMartialRanged;
		AgentTargetStruct LowestAllyCaster;
		AgentTargetStruct DeadAlly;
		AgentTargetStruct LowestOtherAlly;
		AgentTargetStruct LowestEnergyOtherAlly;
		AgentTargetStruct NearestSpirit;
		AgentTargetStruct LowestMinion;
		AgentTargetStruct NearestCorpse;
	};

	struct PartyStruct {
		//GW::PartyInfo* PartyInfo = 0;
		uint32_t LeaderID = 0;
		//GW::Agent* Leader = 0;
		//GW::AgentLiving* LeaderLiving = 0;
		int SelfPartyNumber = -1;
		float DistanceFromLeader = 0.0f;
		bool InAggro = false;
		bool Flags[8];
		bool IsFlagged(int hero) { return Flags[hero]; }
		GW::Vec2f FlagCoords[8];
	};

	struct SkillStruct {
		GW::SkillbarSkill Skill;
		GW::Skill Data;
		CustomSkillClass::CustomSkillDatatype CustomData;
		bool HasCustomData = false;
	};

	struct SkillBarStruct {
		GW::Skillbar* SkillBar;
		SkillStruct Skills[8];
		uint32_t SkillOrder[8];
		uint32_t Skillptr = 0;
	};

	struct GameVarsStruct {
		MapStruct Map;
		TargetStruct Target;
		PartyStruct Party;
		SkillBarStruct SkillBar;
		bool IsMemoryLoaded = false;
	};

	struct FollowTarget {
		GW::GamePos Pos;
		float DistanceFromLeader;
		bool IsCasting;
		float angle;
	};
	
	HeroAI::HeroAI() {
		//initialize();
	}

	HeroAI::~HeroAI() {
		cleanup();
	}

	void DebugMessage(const wchar_t* message);

	void HeroAI::initialize();
	void HeroAI::cleanup();
	bool HeroAI::UpdateGameVars();
	void HeroAI::Update();
	void HeroAI::render();

	int GetPartyNumber();

	int GetMemPosByID(GW::AgentID agentID);
	float CalculatePathDistance(float targetX, float targetY, ImDrawList* drawList = 0, bool draw = false);
	bool IsPointValid(float x, float y);

	bool HasEffect(const GW::Constants::SkillID skillID, uint32_t agentID);
	bool IsMelee(GW::AgentID AgentID);
	bool IsMartial(GW::AgentID AgentID);
	uint32_t TargetNearestEnemy(bool IsBigAggro = false);
	uint32_t TargetNearestEnemyCaster(bool IsBigAggro = false);
	uint32_t TargetNearestEnemyMartial(bool IsBigAggro = false);
	uint32_t TargetNearestEnemyMelee(bool IsBigAggro = false);
	uint32_t TargetNearestEnemyRanged(bool IsBigAggro = false);
	//uint32_t TargetLowestAllyInAggro(bool OtherAlly = false, bool CheckEnergy = false, SkillTarget Target = Ally);
	uint32_t TargetLowestAlly(bool OtherAlly = false);
	uint32_t TargetLowestAllyEnergy(bool OtherAlly = false);
	uint32_t TargetLowestAllyCaster(bool OtherAlly = false);
	uint32_t TargetLowestAllyMartial(bool OtherAlly = false);
	uint32_t TargetLowestAllyMelee(bool OtherAlly = false);
	uint32_t TargetLowestAllyRanged(bool OtherAlly = false);
	uint32_t TargetDeadAllyInAggro();
	uint32_t TargetNearestItem();
	uint32_t TargetNearestNpc();
	uint32_t TargetNearestSpirit();
	uint32_t TargetLowestMinion();
	uint32_t TargetNearestCorpse();

	uint32_t GetNextUnidentifiedItem(IdentifyAllType type);
	uint32_t GetNextUnsalvagedItem(uint32_t kit, SalvageAllType type);
	uint32_t GetIDKit();
	uint32_t GetSalvageKit();
	bool ThereIsSpaceInInventory();
	void IdentifyAll(IdentifyAllType type);
	void SalvageNext(SalvageAllType type);
	void HandleMessaging();

	void activateTickOnLoad();

	uint32_t GetPing() { return 750.0f; }
	bool InCastingRoutine();
	GW::AgentID PartyTargetIDChanged();
	void GetPartyTarget();
	GW::AgentID GetAppropiateTarget(uint32_t slot);
	bool doesSpiritBuffExists(GW::Constants::SkillID skillID);
	bool AreCastConditionsMet(uint32_t slot, GW::AgentID agent);
	bool IsReadyToCast(uint32_t slot);
	bool IsReadyToCast(uint32_t slot, GW::AgentID vTarget); //fulfill all requirements to castx
	void ChooseTarget();
	bool CastSkill(uint32_t slot);
	bool IsSkillready(uint32_t slot); // is recharged
	bool IsOOCSkill(uint32_t slot);
	void HandleOOCombat();
	void HandleCombat();
	FollowTarget GetFollowTarget(bool& FalseLeader, bool& DirectFollow);
	bool FollowLeader(FollowTarget TargetToFollow, float FollowDistanceOnCombat, bool DirectFollow, bool FalseLeader);
	float AngleChange();
	GW::Vec2f ConvertToGridCoords(float x, float y);
	GW::Vec2f ConvertToWorldCoords(int q, int r);
	GW::Vec2f PartyMassCenter();
	GW::Vec2f CalculateAttractionVector(const GW::Vec2f& entityPos, const GW::Vec2f& massCenter, float strength);
	GW::Vec2f CalculateRepulsionVector(const GW::Vec2f& entityPos);
	bool CheckCollision(const GW::GamePos& position, const GW::GamePos& agentpos, float radius1, float radius2);
	bool IsColliding(float x, float y);
	GW::Vec2f GetClosestValidCoordinate(float q, float r);
	bool AvoidCollision();
	

	bool AI_enabled = false;

	bool IsAIEnabled() { return AI_enabled; }
	void SetAIEnabled(bool state);
	int GetMyPartyPos();
	// ********** Python Vars **********
	bool IsActive(int party_pos);
	void SetLooting(int party_pos, bool state);
	bool GetLooting(int party_pos);	
	void SetFollowing(int party_pos, bool state);
	bool GetFollowing(int party_pos);
	void SetCombat(int party_pos, bool state);
	bool GetCombat(int party_pos);
	void SetCollision(int party_pos, bool state);
	bool GetCollision(int party_pos);
	void SetTargetting(int party_pos, bool state);
	bool GetTargetting(int party_pos);
	void SetSkill(int party_pos, int skill_pos, bool state);
	bool GetSkill(int party_pos, int skill_pos);

	bool IsHeroFlagged(int hero);
	void FlagAIHero(int party_pos, float x, float y);
	void UnFlagAIHero(int party_pos);
	float GetEnergy(int party_pos);
	float GetEnergyRegen(int party_pos);
	int GetPythonAgentID(int party_pos);
	void Resign(int party_pos);
	void TakeQuest(uint32_t party_pos, uint32_t quest_id);
	void Identify_cmd(int party_pos);
	void Salvage_cmd(int party_pos);

private:
	
	bool TickOnLoad = false;
	MemMgrClass MemMgr;
	CustomSkillClass CustomSkillData;
	Timer synchTimer;
	Timer ScatterRest;
	Timer Pathingdisplay;
	Timer BipFixtimer;
	GameStateClass GameState;
	GameStateClass ControlAll;
	GameVarsStruct GameVars;
	int CandidateUniqueKey = -1;
	int elapsedCycles = 0;
	int elapsedBuffCycles = 0;
	PythonVarsClass PythonVars[8];
	float RangedRangeValue = GW::Constants::Range::Spellcast;
	float MeleeRangeValue = GW::Constants::Range::Spellcast;
	float GridSize = 160.0f;
	float oldAngle;
	float ScatterPos[8] = { 0.0f, 45.0f, -45.0f, 90.0f, -90.0f, 135.0f, -135.0f, 180.0f };
	FieldsClass Fields;
	GW::Vec3f PlayerPos;
	IdentifyAllType identify_all_type = IdentifyAllType::None;
	bool want_to_salvage = false;
	SalvageAllType want_to_salvage_type;
	GW::AgentID oldCalledTarget;

	bool UnflagNeeded = false;
};


#endif // HeroAI_CLASS_H
