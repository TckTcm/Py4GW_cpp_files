#pragma once
#include "py_map.h"

constexpr std::array outpost_ids = {
    GW::Constants::MapID::Abaddons_Gate,
    GW::Constants::MapID::Abaddons_Mouth,
    GW::Constants::MapID::Altrumm_Ruins,
    GW::Constants::MapID::Amatz_Basin,
    GW::Constants::MapID::The_Amnoon_Oasis_outpost,
    GW::Constants::MapID::Arborstone_outpost_mission,
    GW::Constants::MapID::Ascalon_City_outpost,
    GW::Constants::MapID::Aspenwood_Gate_Kurzick_outpost,
    GW::Constants::MapID::Aspenwood_Gate_Luxon_outpost,
    GW::Constants::MapID::The_Astralarium_outpost,
    GW::Constants::MapID::Augury_Rock_outpost,
    GW::Constants::MapID::The_Aurios_Mines,
    GW::Constants::MapID::Aurora_Glade,
    GW::Constants::MapID::Bai_Paasu_Reach_outpost,
    GW::Constants::MapID::Basalt_Grotto_outpost,
    GW::Constants::MapID::Beacons_Perch_outpost,
    GW::Constants::MapID::Beetletun_outpost,
    GW::Constants::MapID::Beknur_Harbor,
    GW::Constants::MapID::Bergen_Hot_Springs_outpost,
    GW::Constants::MapID::Blacktide_Den,
    GW::Constants::MapID::Bloodstone_Fen,
    GW::Constants::MapID::Bone_Palace_outpost,
    GW::Constants::MapID::Boreal_Station_outpost,
    GW::Constants::MapID::Boreas_Seabed_outpost_mission,
    GW::Constants::MapID::Borlis_Pass,
    GW::Constants::MapID::Brauer_Academy_outpost,
    GW::Constants::MapID::Breaker_Hollow_outpost,
    GW::Constants::MapID::Camp_Hojanu_outpost,
    GW::Constants::MapID::Camp_Rankor_outpost,
    GW::Constants::MapID::Cavalon_outpost,
    GW::Constants::MapID::Central_Transfer_Chamber_outpost,
    GW::Constants::MapID::Chahbek_Village,
    GW::Constants::MapID::Champions_Dawn_outpost,
    GW::Constants::MapID::Chantry_of_Secrets_outpost,
    GW::Constants::MapID::Codex_Arena_outpost,
    GW::Constants::MapID::Consulate_Docks,
    GW::Constants::MapID::Copperhammer_Mines_outpost,
    GW::Constants::MapID::DAlessio_Seaboard,
    GW::Constants::MapID::Dajkah_Inlet,
    GW::Constants::MapID::Dasha_Vestibule,
    GW::Constants::MapID::The_Deep,
    GW::Constants::MapID::Deldrimor_War_Camp_outpost,
    GW::Constants::MapID::Destinys_Gorge_outpost,
    GW::Constants::MapID::Divinity_Coast,
    GW::Constants::MapID::Doomlore_Shrine_outpost,
    GW::Constants::MapID::The_Dragons_Lair,
    GW::Constants::MapID::Dragons_Throat,
    GW::Constants::MapID::Droknars_Forge_outpost,
    GW::Constants::MapID::Druids_Overlook_outpost,
    GW::Constants::MapID::Dunes_of_Despair,
    GW::Constants::MapID::Durheim_Archives_outpost,
    GW::Constants::MapID::Dzagonur_Bastion,
    GW::Constants::MapID::Elona_Reach,
    GW::Constants::MapID::Embark_Beach,
    GW::Constants::MapID::Ember_Light_Camp_outpost,
    GW::Constants::MapID::Eredon_Terrace_outpost,
    GW::Constants::MapID::The_Eternal_Grove_outpost_mission,
    GW::Constants::MapID::Eye_of_the_North_outpost,
    GW::Constants::MapID::Fishermens_Haven_outpost,
    GW::Constants::MapID::Fort_Aspenwood_Kurzick_outpost,
    GW::Constants::MapID::Fort_Aspenwood_Luxon_outpost,
    GW::Constants::MapID::Fort_Ranik,
    GW::Constants::MapID::Frontier_Gate_outpost,
    GW::Constants::MapID::The_Frost_Gate,
    GW::Constants::MapID::Gadds_Encampment_outpost,
    GW::Constants::MapID::Domain_of_Anguish,
    GW::Constants::MapID::Gate_of_Desolation,
    GW::Constants::MapID::Gate_of_Fear_outpost,
    GW::Constants::MapID::Gate_of_Madness,
    GW::Constants::MapID::Gate_of_Pain,
    GW::Constants::MapID::Gate_of_Secrets_outpost,
    GW::Constants::MapID::Gate_of_the_Nightfallen_Lands_outpost,
    GW::Constants::MapID::Gate_of_Torment_outpost,
    GW::Constants::MapID::Gates_of_Kryta,
    GW::Constants::MapID::Grand_Court_of_Sebelkeh,
    GW::Constants::MapID::The_Granite_Citadel_outpost,
    GW::Constants::MapID::The_Great_Northern_Wall,
    GW::Constants::MapID::Great_Temple_of_Balthazar_outpost,
    GW::Constants::MapID::Grendich_Courthouse_outpost,
    GW::Constants::MapID::Gunnars_Hold_outpost,
    GW::Constants::MapID::Gyala_Hatchery_outpost_mission,
    GW::Constants::MapID::Harvest_Temple_outpost,
    GW::Constants::MapID::Hells_Precipice,
    GW::Constants::MapID::Henge_of_Denravi_outpost,
    GW::Constants::MapID::Heroes_Ascent_outpost,
    GW::Constants::MapID::Heroes_Audience_outpost,
    GW::Constants::MapID::Honur_Hill_outpost,
    GW::Constants::MapID::House_zu_Heltzer_outpost,
    GW::Constants::MapID::Ice_Caves_of_Sorrow,
    GW::Constants::MapID::Ice_Tooth_Cave_outpost,
    GW::Constants::MapID::Imperial_Sanctum_outpost_mission,
    GW::Constants::MapID::Iron_Mines_of_Moladune,
    GW::Constants::MapID::Jade_Flats_Kurzick_outpost,
    GW::Constants::MapID::Jade_Flats_Luxon_outpost,
    GW::Constants::MapID::The_Jade_Quarry_Kurzick_outpost,
    GW::Constants::MapID::The_Jade_Quarry_Luxon_outpost,
    GW::Constants::MapID::Jennurs_Horde,
    GW::Constants::MapID::Jokanur_Diggings,
    GW::Constants::MapID::Kaineng_Center_outpost,
    GW::Constants::MapID::Kamadan_Jewel_of_Istan_outpost,
    GW::Constants::MapID::The_Kodash_Bazaar_outpost,
    GW::Constants::MapID::Kodlonu_Hamlet_outpost,
    GW::Constants::MapID::Kodonur_Crossroads,
    GW::Constants::MapID::Lair_of_the_Forgotten_outpost,
    GW::Constants::MapID::Leviathan_Pits_outpost,
    GW::Constants::MapID::Lions_Arch_outpost,
    GW::Constants::MapID::Longeyes_Ledge_outpost,
    GW::Constants::MapID::Lutgardis_Conservatory_outpost,
    GW::Constants::MapID::Maatu_Keep_outpost,
    GW::Constants::MapID::Maguuma_Stade_outpost,
    GW::Constants::MapID::Marhans_Grotto_outpost,
    GW::Constants::MapID::The_Marketplace_outpost,
    GW::Constants::MapID::Mihanu_Township_outpost,
    GW::Constants::MapID::Minister_Chos_Estate_outpost_mission,
    GW::Constants::MapID::Moddok_Crevice,
    GW::Constants::MapID::The_Mouth_of_Torment_outpost,
    GW::Constants::MapID::Nahpui_Quarter_outpost_mission,
    GW::Constants::MapID::Nolani_Academy,
    GW::Constants::MapID::Nundu_Bay,
    GW::Constants::MapID::Olafstead_outpost,
    GW::Constants::MapID::Piken_Square_outpost,
    GW::Constants::MapID::Pogahn_Passage,
    GW::Constants::MapID::Port_Sledge_outpost,
    GW::Constants::MapID::Quarrel_Falls_outpost,
    GW::Constants::MapID::Raisu_Palace_outpost_mission,
    GW::Constants::MapID::Ran_Musu_Gardens_outpost,
    GW::Constants::MapID::Random_Arenas_outpost,
    GW::Constants::MapID::Rata_Sum_outpost,
    GW::Constants::MapID::Remains_of_Sahlahja,
    GW::Constants::MapID::Rilohn_Refuge,
    GW::Constants::MapID::Ring_of_Fire,
    GW::Constants::MapID::Riverside_Province,
    GW::Constants::MapID::Ruins_of_Morah,
    GW::Constants::MapID::Ruins_of_Surmia,
    GW::Constants::MapID::Saint_Anjekas_Shrine_outpost,
    GW::Constants::MapID::Sanctum_Cay,
    GW::Constants::MapID::Sardelac_Sanitarium_outpost,
    GW::Constants::MapID::Seafarers_Rest_outpost,
    GW::Constants::MapID::Seekers_Passage_outpost,
    GW::Constants::MapID::Seitung_Harbor_outpost,
    GW::Constants::MapID::Senjis_Corner_outpost,
    GW::Constants::MapID::Serenity_Temple_outpost,
    GW::Constants::MapID::The_Shadow_Nexus,
    GW::Constants::MapID::Shing_Jea_Arena,
    GW::Constants::MapID::Shing_Jea_Monastery_outpost,
    GW::Constants::MapID::Sifhalla_outpost,
    GW::Constants::MapID::Sunjiang_District_outpost_mission,
    GW::Constants::MapID::Sunspear_Arena,
    GW::Constants::MapID::Sunspear_Great_Hall_outpost,
    GW::Constants::MapID::Sunspear_Sanctuary_outpost,
    GW::Constants::MapID::Tahnnakai_Temple_outpost_mission,
    GW::Constants::MapID::Tanglewood_Copse_outpost,
    GW::Constants::MapID::Tarnished_Haven_outpost,
    GW::Constants::MapID::Temple_of_the_Ages,
    GW::Constants::MapID::Thirsty_River,
    GW::Constants::MapID::Thunderhead_Keep,
    GW::Constants::MapID::Tihark_Orchard,
    GW::Constants::MapID::Tomb_of_the_Primeval_Kings,
    GW::Constants::MapID::Tsumei_Village_outpost,
    GW::Constants::MapID::Umbral_Grotto_outpost,
    GW::Constants::MapID::Unwaking_Waters_Kurzick_outpost,
    GW::Constants::MapID::Unwaking_Waters_Luxon_outpost,
    GW::Constants::MapID::Urgozs_Warren,
    GW::Constants::MapID::Vasburg_Armory_outpost,
    GW::Constants::MapID::Venta_Cemetery,
    GW::Constants::MapID::Ventaris_Refuge_outpost,
    GW::Constants::MapID::Vizunah_Square_Foreign_Quarter_outpost,
    GW::Constants::MapID::Vizunah_Square_Local_Quarter_outpost,
    GW::Constants::MapID::Vloxs_Falls,
    GW::Constants::MapID::Wehhan_Terraces_outpost,
    GW::Constants::MapID::The_Wilds,
    GW::Constants::MapID::Yahnur_Market_outpost,
    GW::Constants::MapID::Yaks_Bend_outpost,
    GW::Constants::MapID::Yohlon_Haven_outpost,
    GW::Constants::MapID::Zaishen_Challenge_outpost,
    GW::Constants::MapID::Zaishen_Elite_outpost,
    GW::Constants::MapID::Zaishen_Menagerie_outpost,
    GW::Constants::MapID::Zen_Daijun_outpost_mission,
    GW::Constants::MapID::Zin_Ku_Corridor_outpost,
    GW::Constants::MapID::Zos_Shivros_Channel
};

constexpr std::array outpost_names = {
    "Abaddon's Gate",
    "Abaddon's Mouth",
    "Altrumm Ruins",
    "Amatz Basin",
    "Amnoon Oasis, The",
    "Arborstone",
    "Ascalon City",
    "Aspenwood Gate (Kurzick)",
    "Aspenwood Gate (Luxon)",
    "Astralarium, The",
    "Augury Rock",
    "Aurios Mines, The",
    "Aurora Glade",
    "Bai Paasu Reach",
    "Basalt Grotto",
    "Beacon's Perch",
    "Beetletun",
    "Beknur Harbor",
    "Bergen Hot Springs",
    "Blacktide Den",
    "Bloodstone Fen",
    "Bone Palace",
    "Boreal Station",
    "Boreas Seabed",
    "Borlis Pass",
    "Brauer Academy",
    "Breaker Hollow",
    "Camp Hojanu",
    "Camp Rankor",
    "Cavalon",
    "Central Transfer Chamber",
    "Chahbek Village",
    "Champion's Dawn",
    "Chantry of Secrets",
    "Codex Arena",
    "Consulate Docks",
    "Copperhammer Mines",
    "D'Alessio Seaboard",
    "Dajkah Inlet",
    "Dasha Vestibule",
    "Deep, The",
    "Deldrimor War Camp",
    "Destiny's Gorge",
    "Divinity Coast",
    "Doomlore Shrine",
    "Dragon's Lair, The",
    "Dragon's Throat",
    "Droknar's Forge",
    "Druid's Overlook",
    "Dunes of Despair",
    "Durheim Archives",
    "Dzagonur Bastion",
    "Elona Reach",
    "Embark Beach",
    "Ember Light Camp",
    "Eredon Terrace",
    "Eternal Grove, The",
    "Eye of the North",
    "Fishermen's Haven",
    "Fort Aspenwood (Kurzick)",
    "Fort Aspenwood (Luxon)",
    "Fort Ranik",
    "Frontier Gate",
    "Frost Gate, The",
    "Gadd's Encampment",
    "Gate of Anguish",
    "Gate of Desolation",
    "Gate of Fear",
    "Gate of Madness",
    "Gate of Pain",
    "Gate of Secrets",
    "Gate of the Nightfallen Lands",
    "Gate of Torment",
    "Gates of Kryta",
    "Grand Court of Sebelkeh",
    "Granite Citadel, The",
    "Great Northern Wall, The",
    "Great Temple of Balthazar",
    "Grendich Courthouse",
    "Gunnar's Hold",
    "Gyala Hatchery",
    "Harvest Temple",
    "Hell's Precipice",
    "Henge of Denravi",
    "Heroes' Ascent",
    "Heroes' Audience",
    "Honur Hill",
    "House zu Heltzer",
    "Ice Caves of Sorrow",
    "Ice Tooth Cave",
    "Imperial Sanctum",
    "Iron Mines of Moladune",
    "Jade Flats (Kurzick)",
    "Jade Flats (Luxon)",
    "Jade Quarry (Kurzick), The",
    "Jade Quarry (Luxon), The",
    "Jennur's Horde",
    "Jokanur Diggings",
    "Kaineng Center",
    "Kamadan, Jewel of Istan",
    "Kodash Bazaar, The",
    "Kodlonu Hamlet",
    "Kodonur Crossroads",
    "Lair of the Forgotten",
    "Leviathan Pits",
    "Lion's Arch",
    "Longeye's Ledge",
    "Lutgardis Conservatory",
    "Maatu Keep",
    "Maguuma Stade",
    "Marhan's Grotto",
    "Marketplace, The",
    "Mihanu Township",
    "Minister Cho's Estate",
    "Moddok Crevice",
    "Mouth of Torment, The",
    "Nahpui Quarter",
    "Nolani Academy",
    "Nundu Bay",
    "Olafstead",
    "Piken Square",
    "Pogahn Passage",
    "Port Sledge",
    "Quarrel Falls",
    "Raisu Palace",
    "Ran Musu Gardens",
    "Random Arenas",
    "Rata Sum",
    "Remains of Sahlahja",
    "Rilohn Refuge",
    "Ring of Fire",
    "Riverside Province",
    "Ruins of Morah",
    "Ruins of Surmia",
    "Saint Anjeka's Shrine",
    "Sanctum Cay",
    "Sardelac Sanitarium",
    "Seafarer's Rest",
    "Seeker's Passage",
    "Seitung Harbor",
    "Senji's Corner",
    "Serenity Temple",
    "Shadow Nexus, The",
    "Shing Jea Arena",
    "Shing Jea Monastery",
    "Sifhalla",
    "Sunjiang District",
    "Sunspear Arena",
    "Sunspear Great Hall",
    "Sunspear Sanctuary",
    "Tahnnakai Temple",
    "Tanglewood Copse",
    "Tarnished Haven",
    "Temple of the Ages",
    "Thirsty River",
    "Thunderhead Keep",
    "Tihark Orchard",
    "Tomb of the Primeval Kings",
    "Tsumei Village",
    "Umbral Grotto",
    "Unwaking Waters (Kurzick)",
    "Unwaking Waters (Luxon)",
    "Urgoz's Warren",
    "Vasburg Armory",
    "Venta Cemetery",
    "Ventari's Refuge",
    "Vizunah Square (Foreign)",
    "Vizunah Square (Local)",
    "Vlox's Falls",
    "Wehhan Terraces",
    "Wilds, The",
    "Yahnur Market",
    "Yak's Bend",
    "Yohlon Haven",
    "Zaishen Challenge",
    "Zaishen Elite",
    "Zaishen Menagerie",
    "Zen Daijun",
    "Zin Ku Corridor",
    "Zos Shivros Channel"
};

std::unordered_map<GW::Constants::MapID, std::string> CreateOutpostMap() {
    std::unordered_map<GW::Constants::MapID, std::string> outpost_map;
    size_t min_size = std::min(outpost_ids.size(), outpost_names.size());

    for (size_t i = 0; i < min_size; ++i) {
        outpost_map[outpost_ids[i]] = outpost_names[i];
    }
    return outpost_map;
}

const auto outpost_map = CreateOutpostMap();

std::optional<std::string> GetOutpostName(GW::Constants::MapID map_id) {
    auto it = outpost_map.find(map_id);
    if (it != outpost_map.end()) {
        return it->second;
    }
    return std::nullopt;
}

// InstanceType Implementation
InstanceType::InstanceType(GW::Constants::InstanceType type) : instance_type(type) {}
InstanceType& InstanceType::operator=(GW::Constants::InstanceType type) {
    Set(type);
    return *this;
}
void InstanceType::Set(GW::Constants::InstanceType type) { instance_type = type; }
GW::Constants::InstanceType InstanceType::Get() const { return instance_type; }

int InstanceType::ToInt() const { return static_cast<int>(instance_type); }
std::string InstanceType::GetName() const {
    switch (instance_type) {
    case GW::Constants::InstanceType::Outpost: return "Outpost";
    case GW::Constants::InstanceType::Explorable: return "Explorable";
    case GW::Constants::InstanceType::Loading: return "Loading";
    default: return "Unknown";
    }
}

MapID::MapID(GW::Constants::MapID id) : id(id) {}
MapID::MapID(int id) : id(static_cast<GW::Constants::MapID>(id)) {}

void MapID::Set(GW::Constants::MapID new_id) {
    this->id = new_id;
}

GW::Constants::MapID MapID::Get() const {
    return id;
}



int MapID::ToInt() const {
    return static_cast<int>(id);
}

std::string MapID::GetName() const {
    auto outpost_name = GetOutpostName(id);
    if (outpost_name) {
        return *outpost_name;
    }
    else {
        return "undefined";
    }
}

std::vector<int> MapID::GetOutpostIDs() {
    std::vector<int> result;
    result.reserve(outpost_ids.size()); // Reserve space for efficiency

    for (const auto& id : outpost_ids) {
        result.push_back(static_cast<int>(id)); // Cast each ID during iteration
    }
    return result;
}

std::vector<std::string> MapID::GetOutpostNames() {
    return std::vector<std::string>(outpost_names.begin(), outpost_names.end());
}


SafeServerRegion ServerRegion::ToSafeRegion(int region) const {
    switch (region) {
    case -2: return SafeServerRegion::International;
    case 0:  return SafeServerRegion::America;
    case 1:  return SafeServerRegion::Korea;
    case 2:  return SafeServerRegion::Europe;
    case 3:  return SafeServerRegion::China;
    case 4:  return SafeServerRegion::Japan;
    case 255: return SafeServerRegion::Unknown;
    default: return SafeServerRegion::Unknown;
    }
}

// Helper method to convert from SafeServerRegion to int
int ServerRegion::ToServerRegion(SafeServerRegion region) const {
    switch (region) {
    case SafeServerRegion::International: return -2;
    case SafeServerRegion::America:       return 0;
    case SafeServerRegion::Korea:         return 1;
    case SafeServerRegion::Europe:        return 2;
    case SafeServerRegion::China:         return 3;
    case SafeServerRegion::Japan:         return 4;
    case SafeServerRegion::Unknown:       return 255;
    default: return 255;
    }
}

// Constructor
ServerRegion::ServerRegion(int region) {
    safe_region = ToSafeRegion(region);
}

// Set the region
void ServerRegion::Set(int region) {
    safe_region = ToSafeRegion(region);
}

// Get the region
SafeServerRegion ServerRegion::Get() const {
    return safe_region;
}



// Convert SafeServerRegion to int
int ServerRegion::ToInt() const {
    return static_cast<int>(ToServerRegion(safe_region));
}

// Get the name of the region as a string
std::string ServerRegion::GetName() const {
    switch (safe_region) {
    case SafeServerRegion::International: return "International";
    case SafeServerRegion::America:       return "America";
    case SafeServerRegion::Korea:         return "Korea";
    case SafeServerRegion::Europe:        return "Europe";
    case SafeServerRegion::China:         return "China";
    case SafeServerRegion::Japan:         return "Japan";
    case SafeServerRegion::Unknown:       return "Unknown";
    default: return "Undefined";
    }
}

SafeLanguage Language::ToSafeLanguage(int lang) const {
    switch (lang) {
    case 0:  return SafeLanguage::English;
    case 1:  return SafeLanguage::Korean;
    case 2:  return SafeLanguage::French;
    case 3:  return SafeLanguage::German;
    case 4:  return SafeLanguage::Italian;
    case 5:  return SafeLanguage::Spanish;
    case 6:  return SafeLanguage::TraditionalChinese;
    case 8:  return SafeLanguage::Japanese;
    case 9:  return SafeLanguage::Polish;
    case 10: return SafeLanguage::Russian;
    case 17: return SafeLanguage::BorkBorkBork;
    case 255: return SafeLanguage::Unknown;
    default: return SafeLanguage::Unknown;
    }
}

// Convert SafeLanguage to int
int Language::ToServerLanguage(SafeLanguage safe_lang) const {
    switch (safe_lang) {
    case SafeLanguage::English:            return 0;
    case SafeLanguage::Korean:             return 1;
    case SafeLanguage::French:             return 2;
    case SafeLanguage::German:             return 3;
    case SafeLanguage::Italian:            return 4;
    case SafeLanguage::Spanish:            return 5;
    case SafeLanguage::TraditionalChinese: return 6;
    case SafeLanguage::Japanese:           return 8;
    case SafeLanguage::Polish:             return 9;
    case SafeLanguage::Russian:            return 10;
    case SafeLanguage::BorkBorkBork:       return 17;
    case SafeLanguage::Unknown:            return 255;
    default:                               return 255;
    }
}

// Constructor
Language::Language(int lang) {
    language = ToSafeLanguage(lang);
}

// Set the language
void Language::Set(int lang) {
    language = ToSafeLanguage(lang);
}

// Get the language
SafeLanguage Language::Get() const {
    return language;
}



// Convert SafeLanguage to int
int Language::ToInt() const {
    return ToServerLanguage(language);
}

// Get the name of the language as a string
std::string Language::GetName() const {
    switch (language) {
    case SafeLanguage::English:            return "English";
    case SafeLanguage::Korean:             return "Korean";
    case SafeLanguage::French:             return "French";
    case SafeLanguage::German:             return "German";
    case SafeLanguage::Italian:            return "Italian";
    case SafeLanguage::Spanish:            return "Spanish";
    case SafeLanguage::TraditionalChinese: return "Traditional Chinese";
    case SafeLanguage::Japanese:           return "Japanese";
    case SafeLanguage::Polish:             return "Polish";
    case SafeLanguage::Russian:            return "Russian";
    case SafeLanguage::BorkBorkBork:       return "Bork Bork Bork";
    case SafeLanguage::Unknown:            return "Unknown";
    default:                               return "Undefined";
    }
}

SafeCampaign CampaignWrapper::ToSafeCampaign(int campaign) const {
    switch (campaign) {
    case 0:  return SafeCampaign::Core;
    case 1:  return SafeCampaign::Prophecies;
    case 2:  return SafeCampaign::Factions;
    case 3:  return SafeCampaign::Nightfall;
    case 4:  return SafeCampaign::EyeOfTheNorth;
    case 5:  return SafeCampaign::BonusMissionPack;
    default: return SafeCampaign::Undefined;
    }
}

// Convert SafeCampaign to int
int CampaignWrapper::ToCampaignValue(SafeCampaign safe_campaign) const {
    switch (safe_campaign) {
    case SafeCampaign::Core:              return 0;
    case SafeCampaign::Prophecies:        return 1;
    case SafeCampaign::Factions:          return 2;
    case SafeCampaign::Nightfall:         return 3;
    case SafeCampaign::EyeOfTheNorth:     return 4;
    case SafeCampaign::BonusMissionPack:  return 5;
    default:                              return 0;
    }
}

// Constructor
CampaignWrapper::CampaignWrapper(int campaign) {
    safe_campaign_value = ToSafeCampaign(campaign);
}

// Set the campaign
void CampaignWrapper::Set(int campaign) {
    safe_campaign_value = ToSafeCampaign(campaign);
}

// Get the campaign
SafeCampaign CampaignWrapper::Get() const {
    return safe_campaign_value;
}



// Convert SafeCampaign to int
int CampaignWrapper::ToInt() const {
    return ToCampaignValue(safe_campaign_value);
}

// Get the name of the campaign as a string
std::string CampaignWrapper::GetName() const {
    switch (safe_campaign_value) {
    case SafeCampaign::Core:              return "Core";
    case SafeCampaign::Prophecies:        return "Prophecies";
    case SafeCampaign::Factions:          return "Factions";
    case SafeCampaign::Nightfall:         return "Nightfall";
    case SafeCampaign::EyeOfTheNorth:     return "Eye of the North";
    case SafeCampaign::BonusMissionPack:  return "Bonus Mission Pack";
    case SafeCampaign::Undefined:         return "Undefined";
    default:                              return "Undefined";
    }
}

SafeRegionType RegionType::ToSafeRegionType(int region_type) const {
    switch (region_type) {
    case 0:  return SafeRegionType::AllianceBattle;
    case 1:  return SafeRegionType::Arena;
    case 2:  return SafeRegionType::ExplorableZone;
    case 3:  return SafeRegionType::GuildBattleArea;
    case 4:  return SafeRegionType::GuildHall;
    case 5:  return SafeRegionType::MissionOutpost;
    case 6:  return SafeRegionType::CooperativeMission;
    case 7:  return SafeRegionType::CompetitiveMission;
    case 8:  return SafeRegionType::EliteMission;
    case 9:  return SafeRegionType::Challenge;
    case 10: return SafeRegionType::Outpost;
    case 11: return SafeRegionType::ZaishenBattle;
    case 12: return SafeRegionType::HeroesAscent;
    case 13: return SafeRegionType::City;
    case 14: return SafeRegionType::MissionArea;
    case 15: return SafeRegionType::HeroBattleOutpost;
    case 16: return SafeRegionType::HeroBattleArea;
    case 17: return SafeRegionType::EotnMission;
    case 18: return SafeRegionType::Dungeon;
    case 19: return SafeRegionType::Marketplace;
    case 20: return SafeRegionType::Unknown;
    case 21: return SafeRegionType::DevRegion;
    default: return SafeRegionType::Unknown;
    }
}

// Convert SafeRegionType to int
int RegionType::ToRegionTypeValue(SafeRegionType safe_region_type) const {
    switch (safe_region_type) {
    case SafeRegionType::AllianceBattle:        return 0;
    case SafeRegionType::Arena:                 return 1;
    case SafeRegionType::ExplorableZone:        return 2;
    case SafeRegionType::GuildBattleArea:       return 3;
    case SafeRegionType::GuildHall:             return 4;
    case SafeRegionType::MissionOutpost:        return 5;
    case SafeRegionType::CooperativeMission:    return 6;
    case SafeRegionType::CompetitiveMission:    return 7;
    case SafeRegionType::EliteMission:          return 8;
    case SafeRegionType::Challenge:             return 9;
    case SafeRegionType::Outpost:               return 10;
    case SafeRegionType::ZaishenBattle:         return 11;
    case SafeRegionType::HeroesAscent:          return 12;
    case SafeRegionType::City:                  return 13;
    case SafeRegionType::MissionArea:           return 14;
    case SafeRegionType::HeroBattleOutpost:     return 15;
    case SafeRegionType::HeroBattleArea:        return 16;
    case SafeRegionType::EotnMission:           return 17;
    case SafeRegionType::Dungeon:               return 18;
    case SafeRegionType::Marketplace:           return 19;
    case SafeRegionType::Unknown:               return 20;
    case SafeRegionType::DevRegion:             return 21;
    default:                                    return 20;  // Unknown
    }
}

// Constructor
RegionType::RegionType(int region_type) {
    safe_region_type_value = ToSafeRegionType(region_type);
}

// Set the region type
void RegionType::Set(int region_type) {
    safe_region_type_value = ToSafeRegionType(region_type);
}

// Get the region type
SafeRegionType RegionType::Get() const {
    return safe_region_type_value;
}

// Equality operator
bool RegionType::operator==(int region_type) const {
    return safe_region_type_value == ToSafeRegionType(region_type);
}

// Inequality operator
bool RegionType::operator!=(int region_type) const {
    return safe_region_type_value != ToSafeRegionType(region_type);
}

// Convert SafeRegionType to int
int RegionType::ToInt() const {
    return ToRegionTypeValue(safe_region_type_value);
}

// Get the name of the region type as a string
std::string RegionType::GetName() const {
    switch (safe_region_type_value) {
    case SafeRegionType::AllianceBattle:        return "Alliance Battle";
    case SafeRegionType::Arena:                 return "Arena";
    case SafeRegionType::ExplorableZone:        return "Explorable Zone";
    case SafeRegionType::GuildBattleArea:       return "Guild Battle Area";
    case SafeRegionType::GuildHall:             return "Guild Hall";
    case SafeRegionType::MissionOutpost:        return "Mission Outpost";
    case SafeRegionType::CooperativeMission:    return "Cooperative Mission";
    case SafeRegionType::CompetitiveMission:    return "Competitive Mission";
    case SafeRegionType::EliteMission:          return "Elite Mission";
    case SafeRegionType::Challenge:             return "Challenge";
    case SafeRegionType::Outpost:               return "Outpost";
    case SafeRegionType::ZaishenBattle:         return "Zaishen Battle";
    case SafeRegionType::HeroesAscent:          return "Heroes' Ascent";
    case SafeRegionType::City:                  return "City";
    case SafeRegionType::MissionArea:           return "Mission Area";
    case SafeRegionType::HeroBattleOutpost:     return "Hero Battle Outpost";
    case SafeRegionType::HeroBattleArea:        return "Hero Battle Area";
    case SafeRegionType::EotnMission:           return "EotN Mission";
    case SafeRegionType::Dungeon:               return "Dungeon";
    case SafeRegionType::Marketplace:           return "Marketplace";
    case SafeRegionType::Unknown:               return "Unknown";
    case SafeRegionType::DevRegion:             return "Dev Region";
    default:                                    return "Undefined";
    }
}

SafeContinent Continent::ToSafeContinent(uint32_t continent) const {
    switch (continent) {
    case 0:  return SafeContinent::Kryta;
    case 1:  return SafeContinent::DevContinent;
    case 2:  return SafeContinent::Cantha;
    case 3:  return SafeContinent::BattleIsles;
    case 4:  return SafeContinent::Elona;
    case 5:  return SafeContinent::RealmOfTorment;
    default: return SafeContinent::Undefined;
    }
}

// Convert SafeContinent to uint32_t
uint32_t Continent::ToContinentValue(SafeContinent safe_continent) const {
    switch (safe_continent) {
    case SafeContinent::Kryta:           return 0;
    case SafeContinent::DevContinent:    return 1;
    case SafeContinent::Cantha:          return 2;
    case SafeContinent::BattleIsles:     return 3;
    case SafeContinent::Elona:           return 4;
    case SafeContinent::RealmOfTorment:  return 5;
    default:                             return 6;  // Undefined
    }
}

// Constructor
Continent::Continent(int continent) {
    safe_continent_value = ToSafeContinent(continent);
}

// Set the continent
void Continent::Set(int continent) {
    safe_continent_value = ToSafeContinent(continent);
}

// Get the continent
SafeContinent Continent::Get() const {
    return safe_continent_value;
}

// Equality operator
bool Continent::operator==(int continent) const {
    return safe_continent_value == ToSafeContinent(continent);
}

// Inequality operator
bool Continent::operator!=(int continent) const {
    return safe_continent_value != ToSafeContinent(continent);
}

// Convert SafeContinent to int
int Continent::ToInt() const {
    return ToContinentValue(safe_continent_value);
}

// Get the name of the continent as a string
std::string Continent::GetName() const {
    switch (safe_continent_value) {
    case SafeContinent::Kryta:            return "Kryta";
    case SafeContinent::DevContinent:     return "DevContinent";
    case SafeContinent::Cantha:           return "Cantha";
    case SafeContinent::BattleIsles:      return "Battle Isles";
    case SafeContinent::Elona:            return "Elona";
    case SafeContinent::RealmOfTorment:   return "Realm of Torment";
    default:                              return "Undefined";
    }
}


PyMap::PyMap() {
    GetContext();
}

void PyMap::ResetContext() {
	instance_time = 0;
	map_id.Set(GW::Constants::MapID::None);
    server_region = 255;
	district = 0;
    language = 255;
	foes_killed = 0;
	foes_to_kill = 0;
	is_in_cinematic = false;
    campaign = static_cast<uint32_t>(GW::Constants::Campaign::Core);
    continent = static_cast<uint32_t>(GW::Continent::DevContinent);
    region_type = static_cast<uint32_t>(GW::RegionType::Unknown);
	has_enter_button = false;
	is_on_world_map = false;
	is_pvp = false;
	is_guild_hall = false;
	is_vanquishable_area = false;
    amount_of_players_in_instance = 0;

    flags = 0;
    thumbnail_id = 0;
    min_party_size = 0;
    max_party_size = 0;
    min_player_size = 0;
    max_player_size = 0;
    controlled_outpost_id = 0;
    fraction_mission = 0;
    min_level = 0;
    max_level = 0;
    needed_pq = 0;
    mission_maps_to = 0;
    icon_position_x = 0;
    icon_position_y = 0;
    icon_start_x = 0;
    icon_start_y = 0;
    icon_end_x = 0;
    icon_end_y = 0;
    icon_start_x_dupe = 0;
    icon_start_y_dupe = 0;
    icon_end_x_dupe = 0;
    icon_end_y_dupe = 0;
    file_id = 0;
    mission_chronology = 0;
    ha_map_chronology = 0;
    name_id = 0;
    description_id = 0;
	
}
void PyMap::GetContext() {
    
    instance_type = GW::Map::GetInstanceType();
    is_map_ready = (GW::Map::GetIsMapLoaded()) && (!GW::Map::GetIsObserving()) && (instance_type != GW::Constants::InstanceType::Loading);
    
    
	if (!is_map_ready) {
		ResetContext();
		return;
	}
    

    instance_time = GW::Map::GetInstanceTime();
    map_id = GW::Map::GetMapID();

    server_region = static_cast<int>(GW::Map::GetRegion());
    district = GW::Map::GetDistrict();
    language = static_cast<int>(GW::Map::GetLanguage());

    foes_killed = GW::Map::GetFoesKilled();
    foes_to_kill = GW::Map::GetFoesToKill();
    is_in_cinematic = GW::Map::GetIsInCinematic();
    auto info = GW::Map::GetMapInfo(map_id.Get());

    if (info) {
        campaign = static_cast<int>(info->campaign);
        continent = static_cast<int>(info->continent);
        region_type = static_cast<int>(info->type);
        has_enter_button = info->GetHasEnterButton();
        is_on_world_map = info->GetIsOnWorldMap();
        is_pvp = info->GetIsPvP();
        is_guild_hall = info->GetIsGuildHall();
        is_vanquishable_area = info->GetIsVanquishableArea();
        //added for map handling
		flags = info->flags;
		thumbnail_id = info->thumbnail_id;
		min_party_size = info->min_party_size;
		max_party_size = info->max_party_size;
		min_player_size = info->min_player_size;
		max_player_size = info->max_player_size;
		controlled_outpost_id = info->controlled_outpost_id;
		fraction_mission = info->fraction_mission;
		min_level = info->min_level;
		max_level = info->max_level;
		needed_pq = info->needed_pq;
		mission_maps_to = info->mission_maps_to;
		icon_position_x = info->x;
		icon_position_y = info->y;
		icon_start_x = info->icon_start_x;
		icon_start_y = info->icon_start_y;
		icon_end_x = info->icon_end_x;
		icon_end_y = info->icon_end_y;
		icon_start_x_dupe = info->icon_start_x_dupe;
		icon_start_y_dupe = info->icon_start_y_dupe;
		icon_end_x_dupe = info->icon_end_x_dupe;
		icon_end_y_dupe = info->icon_end_y_dupe;
		file_id = info->file_id;
		mission_chronology = info->mission_chronology;
		ha_map_chronology = info->ha_map_chronology;
		name_id = info->name_id;
		description_id = info->description_id;
    }

    amount_of_players_in_instance = static_cast<int>(GW::Agents::GetAmountOfPlayersInInstance());

    const auto current_map_context = GW::GetMapContext();
    if (current_map_context)
        map_boundaries.assign(current_map_context->map_boundaries, current_map_context->map_boundaries + 5);



}

bool PyMap::Travel(int new_map_id) {
    return GW::Map::Travel(
        static_cast<GW::Constants::MapID>(new_map_id), // Extract the GW::Constants::MapID from MapID
        static_cast<GW::Constants::District>(0),    // Cast int to GW::Constants::District
        0
    );
}

bool PyMap::Travel(int new_map_id, int new_district, int district_number)
{
    return GW::Map::Travel(
        static_cast<GW::Constants::MapID>(new_map_id),  // Extract the GW::Constants::MapID from MapID
        static_cast<GW::Constants::District>(new_district),  // Cast int to GW::Constants::District
        district_number
    );
}

bool PyMap::Travel(int new_map_id, int region, int district, int language) {
    return GW::Map::Travel(
        static_cast<GW::Constants::MapID>(new_map_id),  // Extract the GW::Constants::MapID from MapID
        static_cast<GW::Constants::ServerRegion>(region),  // Cast int to GW::Constants::ServerRegion
        static_cast<int>(district),  // Cast int to GW::Constants::District
        static_cast<GW::Constants::Language>(language)  // Cast int to GW::Constants::Language
    );
}

bool PyMap::TravelGH() {
	return GW::GuildMgr::TravelGH();
}

bool PyMap::LeaveGH() {
	return GW::GuildMgr::LeaveGH();
}

uint32_t PyMap::GetMapID() {
	return static_cast<uint32_t>(GW::Map::GetMapID());
}


GW::Constants::ServerRegion PyMap::RegionFromDistrict(GW::Constants::District _district) {
    return GW::Map::RegionFromDistrict(_district);
}

GW::Constants::Language PyMap::LanguageFromDistrict(const GW::Constants::District _district) {
    return GW::Map::LanguageFromDistrict(_district);
}

bool PyMap::GetIsMapUnlocked(int new_map_id) {
    return GW::Map::GetIsMapUnlocked(static_cast<GW::Constants::MapID>(new_map_id));
}

bool PyMap::SkipCinematic() {
    return GW::Map::SkipCinematic();
}

bool PyMap::EnterChallenge() {
    return GW::Map::EnterChallenge();
}

bool PyMap::CancelEnterChallenge() {
    return GW::Map::CancelEnterChallenge();
}

uintptr_t  PyMap::GetWorldMapContextPtr()
{
    return reinterpret_cast<uintptr_t>(GW::Map::GetWorldMapContext());
}

uintptr_t  PyMap::GetMissionMapContextPtr()
{
	return reinterpret_cast<uintptr_t>(GW::Map::GetMissionMapContext());
}

uintptr_t PyMap::GetGameplayContextPtr()
{
	return reinterpret_cast<uintptr_t>(GW::GetGameplayContext());
}

uintptr_t PyMap::GetAreaInfoPtr()
{
	return reinterpret_cast<uintptr_t>(GW::Map::GetMapInfo(map_id.Get()));
}

uintptr_t PyMap::GetMapContextPtr()
{
	return reinterpret_cast<uintptr_t>(GW::GetMapContext());
}


void bind_InstanceType(py::module_& m) {
    py::enum_<GW::Constants::InstanceType>(m, "InstanceType")
        .value("Outpost", GW::Constants::InstanceType::Outpost)
        .value("Explorable", GW::Constants::InstanceType::Explorable)
        .value("Loading", GW::Constants::InstanceType::Loading)
        .export_values();
}

void bind_Instance(py::module_& m) {
    py::class_<InstanceType>(m, "Instance")
        .def(py::init<GW::Constants::InstanceType>())  // Constructor
        .def("Set", &InstanceType::Set)                // Set method
        .def("Get", &InstanceType::Get)                // Get method
        .def("ToInt", &InstanceType::ToInt)            // ToInt method
        .def("GetName", &InstanceType::GetName)        // GetName method
        .def("__eq__", &InstanceType::operator==)      // Equality operator
        .def("__ne__", &InstanceType::operator!=);     // Inequality operator
}

/////////////// Server Region Binds ///////////////
void bind_ServerRegionType(py::module_& m) {
    py::enum_<SafeServerRegion>(m, "ServerRegionType")
        .value("International", SafeServerRegion::International)
        .value("America", SafeServerRegion::America)
        .value("Korea", SafeServerRegion::Korea)
        .value("Europe", SafeServerRegion::Europe)
        .value("China", SafeServerRegion::China)
        .value("Japan", SafeServerRegion::Japan)
        .value("Unknown", SafeServerRegion::Unknown)
        .export_values();
}

void bind_ServerRegion(py::module_& m) {
    py::class_<ServerRegion>(m, "ServerRegion")
        .def(py::init<int>())  // Constructor with int
        .def("Set", &ServerRegion::Set)         // Set method with int
        .def("Get", &ServerRegion::Get)         // Get method returning SafeServerRegion
        .def("ToInt", &ServerRegion::ToInt)     // Converts SafeServerRegion to int
        .def("GetName", &ServerRegion::GetName) // Returns name of SafeServerRegion
        .def("__eq__", &ServerRegion::operator==)  // Equality operator with int
        .def("__ne__", &ServerRegion::operator!=);  // Inequality operator with int
}


/////////////// Language Binds ///////////////
void bind_LanguageType(py::module_& m) {
    py::enum_<SafeLanguage>(m, "LanguageType")
        .value("English", SafeLanguage::English)
        .value("Korean", SafeLanguage::Korean)
        .value("French", SafeLanguage::French)
        .value("German", SafeLanguage::German)
        .value("Italian", SafeLanguage::Italian)
        .value("Spanish", SafeLanguage::Spanish)
        .value("TraditionalChinese", SafeLanguage::TraditionalChinese)
        .value("Japanese", SafeLanguage::Japanese)
        .value("Polish", SafeLanguage::Polish)
        .value("Russian", SafeLanguage::Russian)
        .value("BorkBorkBork", SafeLanguage::BorkBorkBork)
        .value("Unknown", SafeLanguage::Unknown)
        .export_values();
}

void bind_Language(py::module_& m) {
    py::class_<Language>(m, "Language")
        .def(py::init<int>())  // Constructor with int
        .def("Set", &Language::Set)             // Set method with int
        .def("Get", &Language::Get)             // Get method returning SafeLanguage
        .def("ToInt", &Language::ToInt)         // Converts SafeLanguage to int
        .def("GetName", &Language::GetName)     // Returns name of SafeLanguage
        .def("__eq__", &Language::operator==)   // Equality operator with int
        .def("__ne__", &Language::operator!=);  // Inequality operator with int
}

/////////////// Campaign Binds ///////////////
void bind_CampaignType(py::module_& m) {
    py::enum_<SafeCampaign>(m, "CampaignType")
        .value("Core", SafeCampaign::Core)
        .value("Prophecies", SafeCampaign::Prophecies)
        .value("Factions", SafeCampaign::Factions)
        .value("Nightfall", SafeCampaign::Nightfall)
        .value("EyeOfTheNorth", SafeCampaign::EyeOfTheNorth)
        .value("BonusMissionPack", SafeCampaign::BonusMissionPack)
        .value("Undefined", SafeCampaign::Undefined)
        .export_values();
}

void bind_Campaign(py::module_& m) {
    py::class_<CampaignWrapper>(m, "Campaign")
        .def(py::init<int>())  // Constructor with int
        .def("Set", &CampaignWrapper::Set)             // Set method with int
        .def("Get", &CampaignWrapper::Get)             // Get method returning SafeCampaign
        .def("ToInt", &CampaignWrapper::ToInt)         // Converts SafeCampaign to int
        .def("GetName", &CampaignWrapper::GetName)     // Returns name of SafeCampaign
        .def("__eq__", &CampaignWrapper::operator==)   // Equality operator with int
        .def("__ne__", &CampaignWrapper::operator!=);  // Inequality operator with int
}

//////////////// region binds///////////////
void bind_SafeRegionType(py::module_& m) {
    py::enum_<SafeRegionType>(m, "RegionType")
        .value("AllianceBattle", SafeRegionType::AllianceBattle)
        .value("Arena", SafeRegionType::Arena)
        .value("ExplorableZone", SafeRegionType::ExplorableZone)
        .value("GuildBattleArea", SafeRegionType::GuildBattleArea)
        .value("GuildHall", SafeRegionType::GuildHall)
        .value("MissionOutpost", SafeRegionType::MissionOutpost)
        .value("CooperativeMission", SafeRegionType::CooperativeMission)
        .value("CompetitiveMission", SafeRegionType::CompetitiveMission)
        .value("EliteMission", SafeRegionType::EliteMission)
        .value("Challenge", SafeRegionType::Challenge)
        .value("Outpost", SafeRegionType::Outpost)
        .value("ZaishenBattle", SafeRegionType::ZaishenBattle)
        .value("HeroesAscent", SafeRegionType::HeroesAscent)
        .value("City", SafeRegionType::City)
        .value("MissionArea", SafeRegionType::MissionArea)
        .value("HeroBattleOutpost", SafeRegionType::HeroBattleOutpost)
        .value("HeroBattleArea", SafeRegionType::HeroBattleArea)
        .value("EotnMission", SafeRegionType::EotnMission)
        .value("Dungeon", SafeRegionType::Dungeon)
        .value("Marketplace", SafeRegionType::Marketplace)
        .value("Unknown", SafeRegionType::Unknown)
        .value("DevRegion", SafeRegionType::DevRegion)
        .export_values();
}

void bind_Region(py::module_& m) {
    py::class_<RegionType>(m, "Region")
        .def(py::init<int>())  // Constructor with int
        .def("Set", &RegionType::Set)           // Set method with int
        .def("Get", &RegionType::Get)           // Get method returning SafeRegionType
        .def("ToInt", &RegionType::ToInt)       // Converts SafeRegionType to int
        .def("GetName", &RegionType::GetName)   // Returns name of SafeRegionType
        .def("__eq__", &RegionType::operator==) // Equality operator with int
        .def("__ne__", &RegionType::operator!=); // Inequality operator with int
}

/////////////// Continent Binds ///////////////

void bind_SafeContinent(py::module_& m) {
    py::enum_<SafeContinent>(m, "ContinentType")
        .value("Kryta", SafeContinent::Kryta)
        .value("DevContinent", SafeContinent::DevContinent)
        .value("Cantha", SafeContinent::Cantha)
        .value("BattleIsles", SafeContinent::BattleIsles)
        .value("Elona", SafeContinent::Elona)
        .value("RealmOfTorment", SafeContinent::RealmOfTorment)
        .value("Undefined", SafeContinent::Undefined)
        .export_values();
}

void bind_Continent(py::module_& m) {
    py::class_<Continent>(m, "Continent")
        .def(py::init<int>())  // Constructor with uint32_t
        .def("Set", &Continent::Set)            // Set method with uint32_t
        .def("Get", &Continent::Get)            // Get method returning SafeContinent
        .def("ToInt", &Continent::ToInt)        // Converts SafeContinent to int
        .def("GetName", &Continent::GetName)    // Returns name of SafeContinent
        .def("__eq__", &Continent::operator==)  // Equality operator with uint32_t
        .def("__ne__", &Continent::operator!=); // Inequality operator with uint32_t
}



void bind_MapID(py::module_& m) {
    py::class_<MapID>(m, "MapID")
        .def(py::init<GW::Constants::MapID>(), "Constructor that accepts a MapID enum.")
        .def(py::init<int>(), "Constructor that accepts an integer representing a MapID.")

        .def("Set", &MapID::Set)                // Set method
        .def("Get", &MapID::Get)                // Get method
        .def("ToInt", &MapID::ToInt)            // ToInt method
        .def("GetName", &MapID::GetName)        // GetName method
		.def("GetOutpostIDs", &MapID::GetOutpostIDs)  // GetOutpostIDs method
		.def("GetOutpostNames", &MapID::GetOutpostNames)  // GetOutpostNames method
        .def("__eq__", &MapID::operator==)      // Equality operator
        .def("__ne__", &MapID::operator!=);     // Inequality operator
}



PYBIND11_EMBEDDED_MODULE(PyMap, m) {
    bind_InstanceType(m);
    bind_Instance(m);
    bind_ServerRegionType(m);
    bind_ServerRegion(m);
    bind_LanguageType(m);
    bind_Language(m);
    bind_CampaignType(m);
    bind_Campaign(m);
    bind_SafeRegionType(m);
    bind_Region(m);
    bind_SafeContinent(m);
    bind_Continent(m);
    bind_MapID(m);

    py::class_<PyMap>(m, "PyMap")
        // Expose the public member variables
        .def_readonly("instance_type", &PyMap::instance_type)
        .def_readonly("is_map_ready", &PyMap::is_map_ready)
        .def_readonly("instance_time", &PyMap::instance_time)
        .def_readonly("map_id", &PyMap::map_id)
        .def_readonly("server_region", &PyMap::server_region)
        .def_readonly("district", &PyMap::district)
        .def_readonly("language", &PyMap::language)
        .def_readonly("foes_killed", &PyMap::foes_killed)
        .def_readonly("foes_to_kill", &PyMap::foes_to_kill)
        .def_readonly("is_in_cinematic", &PyMap::is_in_cinematic)
        .def_readonly("campaign", &PyMap::campaign)
        .def_readonly("continent", &PyMap::continent)
        .def_readonly("region_type", &PyMap::region_type)
        .def_readonly("has_enter_button", &PyMap::has_enter_button)
        .def_readonly("is_on_world_map", &PyMap::is_on_world_map)
        .def_readonly("is_pvp", &PyMap::is_pvp)
        .def_readonly("is_guild_hall", &PyMap::is_guild_hall)
        .def_readonly("is_vanquishable_area", &PyMap::is_vanquishable_area)
        .def_readonly("amount_of_players_in_instance", &PyMap::amount_of_players_in_instance)
		.def_readonly("flags", &PyMap::flags)
		.def_readonly("thumbnail_id", &PyMap::thumbnail_id)
		.def_readonly("min_party_size", &PyMap::min_party_size)
		.def_readonly("max_party_size", &PyMap::max_party_size)
		.def_readonly("min_player_size", &PyMap::min_player_size)
		.def_readonly("max_player_size", &PyMap::max_player_size)
		.def_readonly("controlled_outpost_id", &PyMap::controlled_outpost_id)
		.def_readonly("fraction_mission", &PyMap::fraction_mission)
		.def_readonly("min_level", &PyMap::min_level)
		.def_readonly("max_level", &PyMap::max_level)
		.def_readonly("needed_pq", &PyMap::needed_pq)
		.def_readonly("mission_maps_to", &PyMap::mission_maps_to)
		.def_readonly("icon_position_x", &PyMap::icon_position_x)
		.def_readonly("icon_position_y", &PyMap::icon_position_y)
		.def_readonly("icon_start_x", &PyMap::icon_start_x)
		.def_readonly("icon_start_y", &PyMap::icon_start_y)
		.def_readonly("icon_end_x", &PyMap::icon_end_x)
		.def_readonly("icon_end_y", &PyMap::icon_end_y)
		.def_readonly("icon_start_x_dupe", &PyMap::icon_start_x_dupe)
		.def_readonly("icon_start_y_dupe", &PyMap::icon_start_y_dupe)
		.def_readonly("icon_end_x_dupe", &PyMap::icon_end_x_dupe)
		.def_readonly("icon_end_y_dupe", &PyMap::icon_end_y_dupe)
		.def_readonly("file_id", &PyMap::file_id)
		.def_readonly("mission_chronology", &PyMap::mission_chronology)
		.def_readonly("ha_map_chronology", &PyMap::ha_map_chronology)
		.def_readonly("name_id", &PyMap::name_id)
		.def_readonly("description_id", &PyMap::description_id)
		.def_readonly("map_boundaries", &PyMap::map_boundaries)  // Expose the map_boundaries vector

        // Expose the constructor
        .def(py::init<>())
        .def("GetContext", &PyMap::GetContext, "Updates the PyMap instance with the current game map context")
        .def("Travel", py::overload_cast<int>(&PyMap::Travel))  // Binding for the single-parameter Travel
        .def("Travel", py::overload_cast<int, int, int>(&PyMap::Travel))  // Binding for the three-parameter Travel
		.def("Travel", py::overload_cast<int, int, int, int>(&PyMap::Travel))  // Binding for the four-parameter Travel
		.def("TravelGH", &PyMap::TravelGH)
		.def("LeaveGH", &PyMap::LeaveGH)
        .def("RegionFromDistrict", &PyMap::RegionFromDistrict)
        .def("LanguageFromDistrict", &PyMap::LanguageFromDistrict)
        .def("GetIsMapUnlocked", &PyMap::GetIsMapUnlocked)
        .def("SkipCinematic", &PyMap::SkipCinematic)
        .def("EnterChallenge", &PyMap::EnterChallenge)
        .def("CancelEnterChallenge", &PyMap::CancelEnterChallenge)
		.def("GetMapID", &PyMap::GetMapID)

		.def("GetMissionMapContextPtr", &PyMap::GetMissionMapContextPtr)
		.def("GetWorldMapContextPtr", &PyMap::GetWorldMapContextPtr)
		.def("GetGameplayContextPtr", &PyMap::GetGameplayContextPtr)
		.def("GetMapContextPtr", &PyMap::GetMapContextPtr)
        ;


}

