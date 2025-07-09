#pragma once
#include "Headers.h"


/////// INVENTORY MODULE ///////

class SafeBag {
public:
    int ID = 0;
    std::string Name = "";
    std::vector<SafeItem> items;
    int container_item = 0;
    int items_count = 0;
    bool IsInventoryBag = false;
    bool IsStorageBag = false;
    bool IsMaterialStorage = false;

    // Constructor
    SafeBag::SafeBag(int bag_id, std::string bag_name) : ID(bag_id), Name(bag_name) { GetContext(); }

    // Methods
    int GetItemCount() const;
    std::vector<SafeItem> GetItems() const;
    SafeItem GetItemByIndex(int index) const;
    int GetSize() const;
    SafeItem* FindItemById(int item_id);
    void GetContext();
    void ResetContext();
};

class Inventory {
public:

    void Salvage(int salv_kit_id, int item_id) {

        GW::Item* item = GW::Items::GetItemById(item_id);
        if (!item) return;

        GW::Item* kit = GW::Items::GetItemById(salv_kit_id);

        SafeItem Unsalv(item_id);
        SafeItem SalvKit(salv_kit_id);

        if ((item && kit) && (SalvKit.IsSalvageKit && Unsalv.IsSalvagable))
        {
            GW::Items::SalvageStart(kit->item_id, item->item_id);
        }
        return;

    }

    void AcceptSalvageWindow()
    {
        // Auto accept "you can only salvage materials with a lesser salvage kit"
        GW::GameThread::Enqueue([] {
            GW::UI::ButtonClick(GW::UI::GetChildFrame(GW::UI::GetFrameByLabel(L"Game"), { 0x6, 0x62, 0x6 }));
            });
    }

    void OpenXunlaiWindow() {
        GW::GameThread::Enqueue([] {
            GW::Items::OpenXunlaiWindow();
            });
    }

    bool GetIsStorageOpen() {
        return GW::Items::GetIsStorageOpen();
    }

    bool PickUpItem(int item_id, bool call_target) {
        GW::Item* item = GW::Items::GetItemById(item_id);
        if (!item) return false;
        return GW::Items::PickUpItem(item, call_target);
    }

    bool DropItem(int item_id, int quantity) {
        GW::Item* item = GW::Items::GetItemById(item_id);
        if (!item) return false;
        return GW::Items::DropItem(item, quantity);
    }

    bool EquipItem(int item_id, int agent_id) {
        GW::Item* item = GW::Items::GetItemById(item_id);
        if (!item) return false;
        return GW::Items::EquipItem(item, agent_id);
    }

    bool UseItem(int item_id) {
        GW::Item* item = GW::Items::GetItemById(item_id);
        if (!item) return false;
        return GW::Items::UseItem(item);
    }

    int GetHoveredItemId() {
        GW::Item* item = GW::Items::GetHoveredItem();
        if (!item) return 0;
        return item->item_id;
    }

    bool DropGold(int amount) {
        return GW::Items::DropGold(amount);
    }

    bool DestroyItem(int item_id) {
        return GW::Items::DestroyItem(item_id);
    }

    bool IdentifyItem(int id_kit_id, int item_id) {
        bool result = false;
        GW::GameThread::Enqueue([id_kit_id, item_id, &result] {
            result = GW::Items::IdentifyItem(id_kit_id, item_id);
            });
        return result;
    }


    int GetGoldAmount() {
        return GW::Items::GetGoldAmountOnCharacter();
    }

    int GetGoldAmountInStorage() {
        return GW::Items::GetGoldAmountInStorage();
    }

    int DepositGold(int amount) {
        return GW::Items::DepositGold(amount);
    }

    int WithdrawGold(int amount) {
        return GW::Items::WithdrawGold(amount);
    }

    bool MoveItem(int item_id, int bag_id, int slot, int quantity) {
        GW::Item* item = GW::Items::GetItemById(item_id);
        if (!item) return false;
        return GW::Items::MoveItem(item, static_cast<GW::Constants::Bag>(bag_id), slot, quantity);
    }



};



