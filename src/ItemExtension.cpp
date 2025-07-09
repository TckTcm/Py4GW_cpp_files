#include "ItemExtension.h"

// Constructor
ItemExtension::ItemExtension(GW::Item* item) : gw_item(item) {}

// Retrieve modifier based on identifier
GW::ItemModifier* ItemExtension::GetModifier(uint32_t identifier) const {
    for (size_t i = 0; i < gw_item->mod_struct_size; ++i) {
        GW::ItemModifier* mod = &gw_item->mod_struct[i];
        if (mod->identifier() == identifier) {
            return mod;
        }
    }
    return nullptr;
}

// Retrieve item uses
uint32_t ItemExtension::GetUses() const {
    GW::ItemModifier* mod = GetModifier(0x2458);
    return mod ? mod->arg2() : gw_item->quantity;
}

// Check if item is a Tome
bool ItemExtension::IsTome() const {
    const GW::ItemModifier* mod = GetModifier(0x2788);
    const uint32_t use_id = mod ? mod->arg2() : 0;
    return use_id > 15 && use_id < 36;
}

// Check if item is an Identification Kit
bool ItemExtension::IsIdentificationKit() const {
    const GW::ItemModifier* mod = GetModifier(0x25E8);
    return mod && mod->arg1() == 1;
}

// Check if item is a Lesser Kit
bool ItemExtension::IsLesserKit() const {
    const GW::ItemModifier* mod = GetModifier(0x25E8);
    return mod && mod->arg1() == 3;
}

// Check if item is an Expert Salvage Kit
bool ItemExtension::IsExpertSalvageKit() const {
    const GW::ItemModifier* mod = GetModifier(0x25E8);
    return mod && mod->arg1() == 2;
}

// Check if item is a Perfect Salvage Kit
bool ItemExtension::IsPerfectSalvageKit() const {
    const GW::ItemModifier* mod = GetModifier(0x25E8);
    return mod && mod->arg1() == 6;
}

// Check if item is a Salvage Kit
bool ItemExtension::IsSalvageKit() const {
    return IsLesserKit() || IsExpertSalvageKit() || IsPerfectSalvageKit();
}

// Check if item is a Rare Material
bool ItemExtension::IsRareMaterial() const {
    const GW::ItemModifier* mod = GetModifier(0x2508);
    return mod && mod->arg1() > 11;
}

// Check if item is in inventory
bool ItemExtension::IsInventoryItem() const {
    return gw_item->bag && (gw_item->bag->IsInventoryBag() || gw_item->bag->bag_type == GW::Constants::BagType::Equipped);
}

// Check if item is in storage
bool ItemExtension::IsStorageItem() const {
    return gw_item->bag && (gw_item->bag->IsStorageBag() || gw_item->bag->IsMaterialStorage());
}

// Get item rarity
GW::Constants::Rarity ItemExtension::GetRarity() const {
    if (IsGreen()) return GW::Constants::Rarity::Green;
    if (IsGold()) return GW::Constants::Rarity::Gold;
    if (IsPurple()) return GW::Constants::Rarity::Purple;
    if (IsBlue()) return GW::Constants::Rarity::Blue;
    return GW::Constants::Rarity::White;
}

// Additional methods
bool ItemExtension::IsSparkly() const { return (gw_item->interaction & 0x2000) == 0; }
bool ItemExtension::GetIsIdentified() const { return (gw_item->interaction & 1) != 0; }
bool ItemExtension::IsPrefixUpgradable() const { return ((gw_item->interaction >> 14) & 1) == 0; }
bool ItemExtension::IsSuffixUpgradable() const { return ((gw_item->interaction >> 15) & 1) == 0; }
bool ItemExtension::IsStackable() const { return (gw_item->interaction & 0x80000) != 0; }
bool ItemExtension::IsUsable() const { return (gw_item->interaction & 0x1000000) != 0; }
bool ItemExtension::IsTradable() const { return (gw_item->interaction & 0x100) == 0; }
bool ItemExtension::IsInscription() const { return (gw_item->interaction & 0x25000000) == 0x25000000; }
bool ItemExtension::IsBlue() const { return gw_item->single_item_name && gw_item->single_item_name[0] == 0xA3F; }
bool ItemExtension::IsPurple() const { return (gw_item->interaction & 0x400000) != 0; }
bool ItemExtension::IsGreen() const { return (gw_item->interaction & 0x10) != 0; }
bool ItemExtension::IsGold() const { return (gw_item->interaction & 0x20000) != 0; }

// Check if item is a Weapon
bool ItemExtension::IsWeapon() {
    switch (static_cast<GW::Constants::ItemType>(gw_item->type)) {
    case GW::Constants::ItemType::Axe:
    case GW::Constants::ItemType::Sword:
    case GW::Constants::ItemType::Shield:
    case GW::Constants::ItemType::Scythe:
    case GW::Constants::ItemType::Bow:
    case GW::Constants::ItemType::Wand:
    case GW::Constants::ItemType::Staff:
    case GW::Constants::ItemType::Offhand:
    case GW::Constants::ItemType::Daggers:
    case GW::Constants::ItemType::Hammer:
    case GW::Constants::ItemType::Spear:
        return true;
    default:
        return false;
    }
}

// Check if item is an Armor
bool ItemExtension::IsArmor() {
    switch (static_cast<GW::Constants::ItemType>(gw_item->type)) {
    case GW::Constants::ItemType::Headpiece:
    case GW::Constants::ItemType::Chestpiece:
    case GW::Constants::ItemType::Leggings:
    case GW::Constants::ItemType::Boots:
    case GW::Constants::ItemType::Gloves:
        return true;
    default:
        return false;
    }
}

// Check if item is Salvageable
bool ItemExtension::IsSalvagable() {
    if (gw_item->item_formula == 0x5da) return false;
    if (IsUsable() || IsGreen()) return false;
    switch (static_cast<GW::Constants::ItemType>(gw_item->type)) {
    case GW::Constants::ItemType::Trophy:
        return GetRarity() == GW::Constants::Rarity::White && gw_item->info_string && gw_item->is_material_salvageable;
    case GW::Constants::ItemType::Salvage:
    case GW::Constants::ItemType::CC_Shards:
        return true;
    case GW::Constants::ItemType::Materials_Zcoins:
        return gw_item->is_material_salvageable != 0;
    default:
        break;
    }
    if (IsWeapon() || IsArmor()) return true;
    return false;
}

// Check if item is offered in trade
bool ItemExtension::IsOfferedInTrade() const {
    return GW::Trade::IsItemOffered(gw_item->item_id) != nullptr;
}
