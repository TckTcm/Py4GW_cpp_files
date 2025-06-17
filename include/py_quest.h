#pragma once
#include "Headers.h"

using namespace DirectX;

struct QuestData {
	uint32_t quest_id = 0;
	uint32_t log_state = 0;
	std::string location = "";
	std::string name = "";
	std::string npc = "";
	uint32_t map_from = 0;
	uint32_t marker_x = 0;
	uint32_t marker_y = 0;
	uint32_t h0024 = 0;
	uint32_t map_to = 0;
	std::string description = "";
	std::string objectives = "";

	bool is_completed = false;
	bool is_current_mission_quest = false;
	bool is_area_primary = false;
	bool is_primary = false;
};

class Quest {
public:
    Quest() {}

    ~Quest() {}

    void SetActiveQuestId(uint32_t quest_id) {
        GW::GameThread::Enqueue([quest_id] {
            GW::QuestMgr::SetActiveQuestId(static_cast<GW::Constants::QuestID>(quest_id));
            });
        
    }

    uint32_t GetActiveQuestId() {
        return static_cast<int>(GW::QuestMgr::GetActiveQuestId());
    }

    void AbandonQuestId(uint32_t quest_id){
        GW::GameThread::Enqueue([quest_id] {
            GW::QuestMgr::AbandonQuestId(static_cast<GW::Constants::QuestID>(quest_id));
            });
    }

	bool IsQuestCompleted(uint32_t quest_id) {
		GW::Quest* quest = GW::QuestMgr::GetQuest(static_cast<GW::Constants::QuestID>(quest_id));
		if (!quest) return false;
		return quest->IsCompleted();
	}

    bool IsQuestPrimary(uint32_t quest_id) {
        GW::Quest* quest = GW::QuestMgr::GetQuest(static_cast<GW::Constants::QuestID>(quest_id));
        if (!quest) return false;
        return quest->IsPrimary();
    }

	QuestData GetQuest(uint32_t quest_id) {
		QuestData quest_data;
		
		auto* w = GW::QuestMgr::GetQuestLog();
		if (!w) return quest_data;

		for (auto& q : *w) {
			if (static_cast<uint32_t>(q.quest_id) == quest_id) {
				quest_data.quest_id = static_cast<uint32_t>(q.quest_id);
				quest_data.log_state = q.log_state;
				
				//quest_data.location = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(q.location);
				quest_data.name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(q.name);
				//quest_data.npc = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(q.npc);
				quest_data.map_from = static_cast<uint32_t>(q.map_from);
				quest_data.marker_x = static_cast<uint32_t>(q.marker.x);
				quest_data.marker_y = static_cast<uint32_t>(q.marker.y);
				quest_data.h0024 = q.h0024;
				quest_data.map_to = static_cast<uint32_t>(q.map_to);
				//quest_data.description = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(q.description);
				//quest_data.objectives = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(q.objectives);
				quest_data.is_completed = q.IsCompleted();
				quest_data.is_current_mission_quest = q.IsCurrentMissionQuest();
				quest_data.is_area_primary = q.IsAreaPrimary();
				quest_data.is_primary = q.IsPrimary();
				
			}
		}

		return quest_data;

	}

	bool RequestQuestInfo(uint32_t quest_id, bool update_markers = false) {
		GW::Quest* quest = GW::QuestMgr::GetQuest(static_cast<GW::Constants::QuestID>(quest_id));
		if (!quest) return false;
		return GW::QuestMgr::RequestQuestInfo(quest, update_markers);

	}

	std::vector<QuestData> GetQuestLog() {
		std::vector<QuestData> quest_log;
		auto* w = GW::QuestMgr::GetQuestLog();
		if (!w) return quest_log;
		for (auto& q : *w) {
			quest_log.push_back(QuestData{
				static_cast<uint32_t>(q.quest_id),
				q.log_state,
				"",
				std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(q.name),
				"",
				static_cast<uint32_t>(q.map_from),
				static_cast<uint32_t>(q.marker.x),
				static_cast<uint32_t>(q.marker.y),
				q.h0024,
				static_cast<uint32_t>(q.map_to),
				"",
				"",
				q.IsCompleted(),
				q.IsCurrentMissionQuest(),
				q.IsAreaPrimary(),
				q.IsPrimary()
				});
		}
		return quest_log;
	}
};

