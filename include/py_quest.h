#pragma once
#include "Headers.h"

using namespace DirectX;


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
};

