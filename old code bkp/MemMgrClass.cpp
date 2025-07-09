#include "MemMgrClass.h"

// Declare global variables (if required)

uintptr_t MemMgrClass::GetBasePtr() {
    if (ptrBase_address == 0) {
        uintptr_t address = GW::Scanner::Find("\x50\x6A\x0F\x6A\x00\xFF\x35", "xxxxxxx", +7);
        ptrBase_address = *(uintptr_t*)address;
        if (ptrBase_address == 0) {
            WriteChat(GW::Chat::CHANNEL_GWCA1, L"Failed to find base address.", L"Gwtoolbot");
        }
    }
    return ptrBase_address;
}

uintptr_t MemMgrClass::GetNdcScreenCoordsPtr() {
    if (ptrNdcScreenCoords == 0) {
        uintptr_t address = GW::Scanner::Find("\x8B\xE5\x5D\xC3\x8B\x4F\x10\x83\xC7\x08", "xxxxxxxxxx", 0xC);
        ptrNdcScreenCoords = address;
        if (ptrNdcScreenCoords == 0) {
            WriteChat(GW::Chat::CHANNEL_GWCA1, L"Failed to find Ndc Screen Coords.", L"Gwtoolbot");
        }
    }
    return ptrNdcScreenCoords;
}

ScreenToWorldPoint_pt MemMgrClass::GetScreenToWorldPtr() {
    if (ptrScreenToWorldPoint_Func == 0) {
        ptrScreenToWorldPoint_Func = (ScreenToWorldPoint_pt)GW::Scanner::Find("\xD9\x5D\x14\xD9\x45\x14\x83\xEC\x0C", "xxxxxxxx", -0x2F);
        if (ptrScreenToWorldPoint_Func == 0) {
            WriteChat(GW::Chat::CHANNEL_GWCA1, L"Failed to find ptrScreenToWorldPoint_Func address.", L"Gwtoolbot");
        }
    }
    return ptrScreenToWorldPoint_Func;
}

MapIntersect_pt MemMgrClass::GetMapIntersectPtr() {
    if (ptrMapIntersect_Func == 0) {
        ptrMapIntersect_Func = (MapIntersect_pt)GW::Scanner::Find("\xff\x75\x08\xd9\x5d\xfc\xff\x77\x7c", "xxxxxxxxx", -0x27);
        if (ptrMapIntersect_Func == 0) {
            WriteChat(GW::Chat::CHANNEL_GWCA1, L"Failed to find base address.", L"Gwtoolbot");
        }
    }
    return ptrMapIntersect_Func;
}

float* MemMgrClass::GetC2M_MouseX() {
    uintptr_t basePointer = GetBasePtr();
    if (basePointer == 0) return nullptr;

    return reinterpret_cast<float*>(basePointer + 0x4ECC);
}

float* MemMgrClass::GetC2M_MouseY() {
    uintptr_t basePointer = GetBasePtr();
    if (basePointer == 0) return nullptr;

    return reinterpret_cast<float*>(basePointer + 0x4ED0);
}

GW::Vec2f MemMgrClass::GetClickToMoveCoords() {
    GW::Vec2f res;
    res.x = *GetC2M_MouseX();
    res.y = *GetC2M_MouseY(); // Corrected from res.x
    return res;
}

void MemMgrClass::InitializeSharedMemory(MemPlayerStruct*& pData) {
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,    // read/write access
        FALSE,                  // do not inherit the name
        SHM_NAME                // name of mapping object
    );

    if (hMapFile == NULL) {
        // If opening failed, create the file mapping object
        hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,   // use paging file
            NULL,                   // default security
            PAGE_READWRITE,         // read/write access
            0,                      // maximum object size (high-order DWORD)
            GAMEVARS_SIZE,          // maximum object size (low-order DWORD)
            SHM_NAME                // name of mapping object
        );

        if (hMapFile == NULL) {
            //std::cerr << "Could not create or open file mapping object (" << GetLastError() << ")." << std::endl;
            WriteChat(GW::Chat::CHANNEL_GWCA1, L"Could not create or open file mapping object", L"Hero AI");
            return;
        }

        // Map the file mapping object into the process's address space
        pData = (MemPlayerStruct*)MapViewOfFile(
            hMapFile,               // handle to map object
            FILE_MAP_ALL_ACCESS,    // read/write permission
            0,                      // offset high
            0,                      // offset low
            GAMEVARS_SIZE
        );

        if (pData == NULL) {
            //std::cerr << "Could not map view of file (" << GetLastError() << ")." << std::endl;
            WriteChat(GW::Chat::CHANNEL_GWCA1, L"Could not map view of file", L"Hero AI");
            CloseHandle(hMapFile);
            return;
        }

        // Initialize shared memory data if this is the first creation
        // pData->some_variable = 0;
        for (int i = 0; i < 8; i++) {
            pData->MemPlayers[i].ID = 0;
            pData->MemPlayers[i].energyRegen = 0.0f;
            pData->MemPlayers[i].Energy = 0.0f;
            pData->MemPlayers[i].IsActive = false;
            pData->MemPlayers[i].LastActivity.start();
            pData->MemPlayers[i].LastActivity.stop();
            pData->MemPlayers[i].IsHero = false;
            pData->MemPlayers[i].IsFlagged = false;
            pData->MemPlayers[i].FlagPos.x = 0.0f;
            pData->MemPlayers[i].FlagPos.y = 0.0f;
            pData->MemPlayers[i].BuffList.Clear();

            pData->GameState[i].state.Following = true;
            pData->GameState[i].state.Collision = true;
            pData->GameState[i].state.Looting = true;
            pData->GameState[i].state.Combat = true;
            pData->GameState[i].state.Targetting = true;
            pData->GameState[i].state.RangedRangeValue = GW::Constants::Range::Spellcast;
            pData->GameState[i].state.MeleeRangeValue = GW::Constants::Range::Spellcast;
            pData->GameState[i].state.MemFields.Agent.CollisionRadius = 80.0f;
            pData->GameState[i].state.MemFields.Agent.RepulsionStrength = 100.0f;
            pData->GameState[i].state.MemFields.Agent.AttractionStrength = 0.0f;
            pData->GameState[i].state.MemFields.Enemy.CollisionRadius = 100.0f;
            pData->GameState[i].state.MemFields.Enemy.RepulsionStrength = 120.0f;
            pData->GameState[i].state.MemFields.Enemy.AttractionStrength = 0.0f;
            pData->GameState[i].state.MemFields.Spirit.CollisionRadius = 40.0f;
            pData->GameState[i].state.MemFields.Spirit.RepulsionStrength = 60.0f;
            pData->GameState[i].state.MemFields.Spirit.AttractionStrength = 0.0f;

            for (int j = 0; j < 8; j++) {
                pData->GameState[i].CombatSkillState[j] = true;
            }
        }
        // Initialize other variables as needed

    }
    else {
        // Map the existing file mapping object into the process's address space
        pData = (MemPlayerStruct*)MapViewOfFile(
            hMapFile,               // handle to map object
            FILE_MAP_ALL_ACCESS,    // read/write permission
            0,                      // offset high
            0,                      // offset low
            GAMEVARS_SIZE
        );

        if (pData == NULL) {
            //std::cerr << "Could not map view of file (" << GetLastError() << ")." << std::endl;
            WriteChat(GW::Chat::CHANNEL_GWCA1, L"Could not map view of file2", L"Hero AI");
            CloseHandle(hMapFile);
            return;

        }
    }
    // Create or open the named mutex
    hMutex = CreateMutexA(NULL, FALSE, "HeroAIMutex");
    if (hMutex == NULL) {
        WriteChat(GW::Chat::CHANNEL_GWCA1, L"Could not create or open mutex", L"Hero AI");
        return;
    }
}

void MemMgrClass::MutexAquire() {
    WaitForSingleObject(hMutex, INFINITE);
}

void MemMgrClass::MutexRelease() {
    ReleaseMutex(hMutex);
}
