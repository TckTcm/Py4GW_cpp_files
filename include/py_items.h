#pragma once
#include "Headers.h"


enum class SalvageAllType : uint8_t {
    None,
    White,
    BlueAndLower,
    PurpleAndLower,
    GoldAndLower
};

enum class IdentifyAllType : uint8_t {
    None,
    All,
    Blue,
    Purple,
    Gold
};

enum class SafeItemType {
    Salvage,
    Axe,
    Bag,
    Boots,
    Bow,
    Bundle,
    Chestpiece,
    Rune_Mod,
    Usable,
    Dye,
    Materials_Zcoins,
    Offhand,
    Gloves,
    Hammer,
    Headpiece,
    CC_Shards,
    Key,
    Leggings,
    Gold_Coin,
    Quest_Item,
    Wand,
    Shield,
    Staff,
    Sword,
    Kit,
    Trophy,
    Scroll,
    Daggers,
    Present,
    Minipet,
    Scythe,
    Spear,
    Storybook,
    Costume,
    Costume_Headpiece,
    Unknown
};

enum class SafeDyeColor {
    NoColor,
    Blue,
    Green,
    Purple,
    Red,
    Yellow,
    Brown,
    Orange,
    Silver,
    Black,
    Gray,
    White,
    Pink
};



// SafeItemModifier class
class SafeItemModifier {
private:
    uint32_t mod;

public:
    // Constructor
    SafeItemModifier(uint32_t mod_value);

    // Getter methods
    uint32_t get_identifier() const;
    uint32_t get_arg1() const;
    uint32_t get_arg2() const;
    uint32_t get_arg() const;

    // Validation
    bool is_valid() const;

    // Binary representation methods
    std::string get_mod_bits() const;
    std::string get_identifier_bits() const;
    std::string get_arg1_bits() const;
    std::string get_arg2_bits() const;
    std::string get_arg_bits() const;

    // Convert to string
    std::string to_string() const;
};


class SafeItemTypeClass {
private:
    SafeItemType safe_item_type;

public:
    // Constructor
    SafeItemTypeClass(int item_type);

    // Convert to int
    int to_int() const;

    // Comparison operators
    bool SafeItemTypeClass::operator==(const SafeItemTypeClass& other) const { return safe_item_type == other.safe_item_type; }
    bool SafeItemTypeClass::operator!=(const SafeItemTypeClass& other) const { return safe_item_type != other.safe_item_type; }

    // Get item name as string
    std::string get_name() const;
};

// Wrapper class for DyeColor
class SafeDyeColorClass {
private:
    SafeDyeColor safe_dye_color;

public:
    // Constructor
    SafeDyeColorClass(int dye_color);

    // Convert to int
    int to_int() const;

    // Comparison operators
    bool operator==(const SafeDyeColorClass& other) const;
    bool operator!=(const SafeDyeColorClass& other) const;

    // Get color name as string
    std::string to_string() const;
};

// Wrapper class for DyeInfo
class SafeDyeInfoClass {
public:
    uint8_t dye_tint;
    SafeDyeColorClass dye1;
    SafeDyeColorClass dye2;
    SafeDyeColorClass dye3;
    SafeDyeColorClass dye4;

public:
    // Default constructor
    SafeDyeInfoClass();

    // Constructor from GW::DyeInfo
    explicit SafeDyeInfoClass(const GW::DyeInfo& dye_info);

    // Equality operator
    bool operator==(const SafeDyeInfoClass& other) const {
        return dye_tint == other.dye_tint &&
            dye1 == other.dye1 &&
            dye2 == other.dye2 &&
            dye3 == other.dye3 &&
            dye4 == other.dye4;
    }

    // Inequality operator
    bool operator!=(const SafeDyeInfoClass& other) const {
        return !(*this == other);
    }

    // Convert to string
    std::string to_string() const;
};


std::string item_WStringToString(const std::wstring& s)
{
    // @Cleanup: ASSERT used incorrectly here; value passed could be from anywhere!
    if (s.empty()) {
        return "Error In Wstring";
    }
    // NB: GW uses code page 0 (CP_ACP)
    const auto size_needed = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, s.data(), static_cast<int>(s.size()), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), strTo.data(), size_needed, NULL, NULL);
    return strTo;
}

// SafeItem class
class SafeItem {
public:
    int item_id = 0;
    int agent_id;
    int agent_item_id;

    std::string name;
    std::vector<SafeItemModifier> modifiers;
    bool IsCustomized = false;
    SafeItemTypeClass item_type = 0;
    SafeDyeInfoClass dye_info;
    int value = 0;
    uint32_t interaction;
    uint32_t model_id;
    int item_formula = 0;
    int is_material_salvageable = 0;
    int quantity = 0;
    int equipped = 0;
    int profession = 0;
    int slot = 0;
    bool is_stackable = false;
    bool is_inscribable = false;
    bool is_material = false;
    bool is_ZCoin = false;
    GW::Constants::Rarity rarity;
    int Uses = 0;
    bool IsIDKit = false;
    bool IsSalvageKit = false;
    bool IsTome = false;
    bool IsLesserKit = false;
    bool IsExpertSalvageKit = false;
    bool IsPerfectSalvageKit = false;
    bool IsWeapon = false;
    bool IsArmor = false;
    bool IsSalvagable = false;
    bool IsInventoryItem = false;
    bool IsStorageItem = false;
    bool IsRareMaterial = false;
    bool IsOfferedInTrade = false;
    bool CanOfferToTrade = false;
    bool IsSparkly = false;
    bool IsIdentified = false;
    bool IsPrefixUpgradable = false;
    bool IsSuffixUpgradable = false;
    bool IsStackable = false;
    bool IsUsable = false;
    bool IsTradable = false;
    bool IsInscription = false;
    bool IsRarityBlue = false;
    bool IsRarityPurple = false;
    bool IsRarityGreen = false;
    bool IsRarityGold = false;

    SafeItemModifier* GetModifier(uint32_t identifier) {
        for (auto& mod : modifiers) {
            if (mod.get_identifier() == identifier) {
                return &mod;
            }
        }
        return nullptr;  // Return nullptr if no matching modifier found
    }

    SafeItem(int item_id) : item_id(item_id) {
        GetContext();
    }

    void GetContext();
	void ResetContext();
	void RequestName();
    bool IsItemNameReady();
	std::string GetName();
	bool IsItemValid(uint32_t item_id);

};

