#pragma once
#include "Headers.h"

namespace GW {
    namespace Constants {
        enum class Rarity : uint8_t {
            White,
            Blue,
            Purple,
            Gold,
            Green
        };
        enum class Bag : uint8_t;
    }
    namespace UI {
        enum class UIMessage : uint32_t;
    }
}


class ItemExtension {
private:
    GW::Item* gw_item;  // Stores the pointer to the GW::Item object

    // Retrieve modifier based on identifier
    GW::ItemModifier* GetModifier(uint32_t identifier) const;

public:
    // Constructor
    ItemExtension(GW::Item* item);

    // Retrieve item uses
    uint32_t GetUses() const;

    // Item checks
    bool IsTome() const;
    bool IsIdentificationKit() const;
    bool IsLesserKit() const;
    bool IsExpertSalvageKit() const;
    bool IsPerfectSalvageKit() const;
    bool IsSalvageKit() const;
    bool IsRareMaterial() const;
    bool IsInventoryItem() const;
    bool IsStorageItem() const;

    // Item rarity
    GW::Constants::Rarity GetRarity() const;

    // Additional methods based on interaction flags
    bool IsSparkly() const;
    bool GetIsIdentified() const;
    bool IsPrefixUpgradable() const;
    bool IsSuffixUpgradable() const;
    bool IsStackable() const;
    bool IsUsable() const;
    bool IsTradable() const;
    bool IsInscription() const;
    bool IsBlue() const;
    bool IsPurple() const;
    bool IsGreen() const;
    bool IsGold() const;

    // Item type checks
    bool IsWeapon();
    bool IsArmor();
    bool IsSalvagable();
    bool IsOfferedInTrade() const;
};
