#pragma once
#include "canvas_gui.h"
#include "options.hpp"
#include <fstream>

// Define skin arrays for each weapon type
static const std::vector<SkinData> vandalSkins = { 
{L"Phaseguard Vandal", L"Default__AK_Commando_PrimaryAsset_C", L"Default__Commando_v2_PrimaryAsset_C", 4},
{L"Divergence V4ndal", L"Default__AK_Anomaly_PrimaryAsset_C", L"Default__Anomaly_v1_PrimaryAsset_C", 4},
{L"CYRAX Vandal", L"Default__AK_Syndra_PrimaryAsset_C", L"Default__Syndra_v1_PrimaryAsset_C", 4},
{L"Primordium Vandal", L"Default__AK_Hellfire_PrimaryAsset_C", L"Default__Hellfire_v2_PrimaryAsset_C", 4},
{L"Singularity Vandal", L"Default__AK_Edge2_PrimaryAsset_C", L"Default__AK_Edge2_v1_PrimaryAsset_C", 4},
{L"Aemondir Vandal", L"Default__AK_Legion_PrimaryAsset_C", L"Default__AK_Legion_Lv3_PrimaryAsset_C", 4},
{L"Evori Dreamwings Vandal", L"Default__AK_StarPower_PrimaryAsset_C", L"Default__StarPower_v3_PrimaryAsset_C", 4},
{L"Prelude to Chaos Vandal", L"Default__AK_DemonStone_PrimaryAsset_C", L"Default__AK_DemonStone_Lv4_PrimaryAsset_C", 4},
{L"Reaver Vandal", L"Default__AK_Soulstealer_PrimaryAsset_C", L"Default__AK_Soulstealer_White_PrimaryAsset_C", 4},
{L"XERØFANG Vandal ", L"Default__AK_Hypedragon_PrimaryAsset_C", L"Default__AK_Hypedragon_v1_PrimaryAsset_C", 4},
{L"Kuronami Vandal", L"Default__AK_Ninja_PrimaryAsset_C", L"Default__AK_Ninja_v1_PrimaryAsset_C", 4},
{L"Imperium Vandal", L"Default__AK_Dynasty_PrimaryAsset_C", L"Default__AK_Dynasty_Standard_PrimaryAsset_C", 4},
{L"Champions 2023 Vandal", L"Default__AK_Champions2023_PrimaryAsset_C", L"Default__AK_Champions2023_Standard_PrimaryAsset_C", 4},
{L"Magepunk Vandal", L"Default__AK_Magepunk3_PrimaryAsset_C", L"Default__AK_Magepunk3_Standard_PrimaryAsset_C", 4},
{L"Gaia's Vengeance Vandal", L"Default__AK_Ashen_PrimaryAsset_C", L"Default__AK_Ashen_v2_PrimaryAsset_C", 4},
{L"RGX 11z Pro Vandal", L"Default__AK_Afterglow_PrimaryAsset_C", L"Default__AK_Afterglow_Blue_PrimaryAsset_C", 5},
{L"Champions 2021 Vandal", L"Default__AK_Champions_PrimaryAsset_C", L"Default__AK_Champions_Lv4_PrimaryAsset_C", 4},
{L"Elderflame Vandal", L"Default__AK_Dragon_PrimaryAsset_C", L"Default__AK_Dragon_Blue_PrimaryAsset_C", 4},
{L"Ion Vandal", L"Default__AK_Oblivion2_PrimaryAsset_C", L"Default__AK_Oblivion2_v1_PrimaryAsset_C", 4},
{L"ChronoVoid Vandal", L"Default__AK_Daedalus_PrimaryAsset_C", L"Default__AK_Daedalus_v2_PrimaryAsset_C", 4},
{L"Araxys Vandal", L"Default__AK_Antares_PrimaryAsset_C", L"Default__AK_Antares_Lv4_PrimaryAsset_C", 4},
{L"Origin Vandal", L"Default__AK_Circle_PrimaryAsset_C", L"Default__AK_Circle_Lv4_PrimaryAsset_C",4},
{L"Sentinels of Light Vandal", L"Default__AK_SOL_PrimaryAsset_C", L"Default__AK_SOL_Lv4_PrimaryAsset_C", 4},
{L"Glitchpop Vandal", L"Default__AK_Cyberpunk2_PrimaryAsset_C", L"Default__AK_Cyberpunk2_Red_PrimaryAsset_C", 4},
{L"Prime Vandal", L"Default__AssaultRifle_AK_HypeBeast_PrimaryAsset_C", L"Default__AssaultRifle_AK_HypeBeast_Blue_PrimaryAsset_C", 4},
{L"Oni Vandal", L"Default__AK_Oni2_PrimaryAsset_C", L"Default__AK_Oni2_Lv4_PrimaryAsset_C", 4},
{L"Overdrive Vandal", L"Default__AK_Motorbike_PrimaryAsset_C", L"Default__Motorbike_v2_PrimaryAsset_C", 4},
{L"EX.O Vandal", L"Default__AK_Hologram_PrimaryAsset_C", L"Default__Hologram_v3_PrimaryAsset_C", 4},
{L"Valiant Hero Vandal", L"Default__AK_MonkeyKing_PrimaryAsset_C", L"Default__MonkeyKing_v1_PrimaryAsset_C", 4},
{L"Cryostasis Vandal", L"Default__AK_Permafrost_PrimaryAsset_C", L"Default__Permafrost_v1_PrimaryAsset_C", 4},
{L"Forsaken Vandal", L"Default__AK_Fallen_PrimaryAsset_C", L"Default__Fallen_v2_PrimaryAsset_C", 4},

};
static const std::vector<SkinData> phantomSkins = {

{L"Bolt Phantom", L"Default__Carbine_Bolt_PrimaryAsset_C", L"Default__Carbine_Bolt_v1_PrimaryAsset_C", 4},
{L"Nocturnum Phantom", L"Default__Carbine_Coven_PrimaryAsset_C", L"Default__Carbine_Coven_V1_PrimaryAsset_C", 4},
{L"Champions 2024 Phantom", L"Default__Carbine_Champions2024_PrimaryAsset_C", L"Default__Carbine_Champions2024_Lv4_PrimaryAsset_C", 4},
{L"Mystbloom Phantom", L"Default__Carbine_Spirit_PrimaryAsset_C", L"Default__Carbine_Spirit_v2_PrimaryAsset_C", 4},
{L"Sovereign Phantom", L"Default__Carbine_Sovereign2_PrimaryAsset_C", L"Default__Carbine_Sovereign2_v1_PrimaryAsset_C", 4},
{L"Singularity Phantom", L"Default__AssaultRifle_ACR_Edge_PrimaryAsset_C", L"Default__AssaultRifle_ACR_Edge_Lv4_PrimaryAsset_C", 4},
{L"Spectrum Phantom", L"Default__Carbine_Atlas_PrimaryAsset_C", L"Default__Carbine_Atlas_v1_PrimaryAsset_C", 4},
{L"Reaver Phantom", L"Default__Carbine_Soulstealer2_PrimaryAsset_C", L"Default__Carbine_Soulstealer2_v1_PrimaryAsset_C", 4},
{L"RGX 11z Pro Phantom", L"Default__Carbine_Afterglow2_PrimaryAsset_C", L"Default__Carbine_Afterglow2_v2_PrimaryAsset_C", 4},
{L"Glitchpop Phantom", L"Default__Carbine_Cyberpunk2_PrimaryAsset_C", L"Default__Carbine_Cyberpunk2_Lv4_PrimaryAsset_C", 4},
{L"Ruination Phantom", L"Default__Carbine_King_PrimaryAsset_C", L"Default__Carbine_King_Lv4_PrimaryAsset_C", 4},
{L"ChronoVoid Phantom", L"Default__Carbine_Daedalus_PrimaryAsset_C", L"Default__Carbine_Daedalus_Lv4_PrimaryAsset_C", 4},
{L"Recon Phantom", L"Default__Carbine_SpecOps_PrimaryAsset_C", L"Default__Carbine_SpecOps_Lv4_PrimaryAsset_C", 4},
{L"BlastX Phantom", L"Default__Carbine_URF_PrimaryAsset_C", L"Default__Carbine_URF_Lv4_PrimaryAsset_C", 4},
{L"Protocol 781-A Phantom", L"Default__Carbine_Protocol_PrimaryAsset_C", L"Default__Carbine_Protocol_Lv4_PrimaryAsset_C", 4},
{L"Oni Phantom", L"Default__AssaultRifle_ACR_Oni_PrimaryAsset_C", L"Default__AssaultRifle_ACR_Oni_Black_PrimaryAsset_C", 4},
{L"Nebula Phantom", L"Default__Carbine_Cosmos_PrimaryAsset_C", L"Default__Carbine_Cosmos_Lv1_PrimaryAsset_C", 4},
{L"Celestial Phantom", L"Default__Carbine_Lunar_PrimaryAsset_C", L"Default__Carbine_Lunar_Lv1_PrimaryAsset_C", 4},

};
//
//
static const std::vector<SkinData> operatorSkins = {
{L"Divergence Operator", L"Default__BoltSniper_Anomaly_PrimaryAsset_C", L"Default__BoltSniper_Anomaly_v1_PrimaryAsset_C", 4},
{L"Mystbloom Operator", L"Default__BoltSniper_Spirit_PrimaryAsset_C", L"Default__BoltSniper_Spirit_v2_PrimaryAsset_C", 4},
{L"Ion Operator", L"Default__BoltSniper_Oblivion_PrimaryAsset_C", L"Default__BoltSniper_OblivionLv4_PrimaryAsset_C", 4},
{L"RGX 11z Pro Operator", L"Default__BoltSniper_Afterglow2_PrimaryAsset_C", L"Default__BoltSniper_Afterglow2_Lv5_PrimaryAsset_C", 4},
{L"Origin Operator", L"Default__BoltSniper_Circle_PrimaryAsset_C", L"Default__BoltSniper_Circle_Lv4_PrimaryAsset_C", 4},
{L"Prelude to Chaos Operator", L"Default__BoltSniper_DemonStone_PrimaryAsset_C", L"Default__BoltSniper_DemonStone_Lv4_PrimaryAsset_C", 4},
{L"Forsaken Operator", L"Default__BoltSniper_Fallen_PrimaryAsset_C", L"Default__BoltSniper_Fallen_Lv4_PrimaryAsset_C", 4},
{L"Glitchpop Operator", L"Default__BoltSniper_Cyberpunk2_PrimaryAsset_C", L"Default__BoltSniper_Cyberpunk2_Lv4_PrimaryAsset_C", 4},
{L"Radiant Entertainment System Operator", L"Default__BoltSniper_Arcade_PrimaryAsset_C", L"Default__BoltSniper_Arcade_Lv4_PrimaryAsset_C", 4},
{L"Elderflame Operator", L"Default__BoltSniper_Dragon_PrimaryAsset_C", L"Default__BoltSniper_Dragon_Dark_PrimaryAsset_C", 4},
{L"Sentinels of Light Operator", L"Default__BoltSniper_SOL_PrimaryAsset_C", L"Default__BoltSniper_SOL_Lv4_PrimaryAsset_C", 4},
{L"Reaver Operator", L"Default__BoltSniper_Soulstealer_PrimaryAsset_C", L"Default__BoltSniper_Soulstealer_Black_PrimaryAsset_C", 4},
{L"Tethered Realms Operator", L"Default__BoltSniper_SovWorld_PrimaryAsset_C", L"Default__BoltSniper_SovWorld_Lv1_PrimaryAsset_C", 4},
{L"Araxys Operator", L"Default__BoltSniper_Antares_PrimaryAsset_C", L"Default__BoltSniper_Antares_Lv4_PrimaryAsset_C", 4},
{L"Gravitational Uranium Neuroblaster Operator", L"Default__BoltSniper_Raygun_PrimaryAsset_C", L"Default__BoltSniper_Raygun_Lv2_PrimaryAsset_C", 4},
{L"Magepunk Operator", L"Default__BoltSniper_Magepunk2_v1_PrimaryAsset_C", L"Default__BoltSniper_Magepunk2_Lv4_PrimaryAsset_C", 4},

};
//
static const std::vector<SkinData> bulldogSkins = {
{L"Phaseguard Bulldog", L"Default__Burst_Commando_PrimaryAsset_C", L"Default__Commando_v1_PrimaryAsset_C", 4},
{L"Nocturnum Bulldog", L"Default__Burst_Coven_PrimaryAsset_C", L"Default__Burst_Coven_v1_PrimaryAsset_C", 4},
{L"Glitchpop Bulldog", L"Default__AssaultRifle_Burst_Cyberpunk_PrimaryAsset_C", L"Default__AssaultRifle_Burst_Cyberpunk_Lv4_PrimaryAsset_C", 4},
{L"Spectrum Bulldog", L"Default__Burst_Atlas_PrimaryAsset_C", L"Default__Burst_Atlas_v2_PrimaryAsset_C", 4},
{L"Radiant Entertainment System Bulldog", L"Default__Burst_Arcade_v2_PrimaryAsset_C", L"Default__Burst_Arcade_Lv4_PrimaryAsset_C", 4},
};
//
static const std::vector<SkinData> frenzySkins = {

{L"Sovereign Frenzy", L"Default__AutoPistol_Sovereign2_PrimaryAsset_C", L"Default__AutoPistol_Sovereign2_v1_PrimaryAsset_C", 4},
{L"Elderflame Frenzy", L"Default__AutomaticPistol_Dragon_PrimaryAsset_C", L"Default__AutomaticPistol_Dragon_Blue_PrimaryAsset_C", 4},
{L"Glitchpop Frenzy", L"Default__AutomaticPistol_Cyberpunk_PrimaryAsset_C", L"Default__AutomaticPistol_Cyberpunk_Blue_PrimaryAsset_C", 4},
{L"BlastX Frenzy", L"Default__AutoPistol_URF_PrimaryAsset_C", L"Default__AutoPistol_URF_Standard_PrimaryAsset_C", 4},
{L"Prime//2.0 Frenzy", L"Default__AutomaticPistol_HypeBeast2_PrimaryAsset_C", L"Default__AutomaticPistol_HypeBeast2_PrimaryAsset_C", 4},
{L"Ion Frenzy", L"Default__AutoPistol_Oblivion2_PrimaryAsset_C", L"Default__AutoPistol_Oblivion2_Lv4_PrimaryAsset_C", 4},
{L"Oni Frenzy", L"Default__AutoPistol_Oni2_PrimaryAsset_C", L"Default__AutoPistol_Oni2_PrimaryAsset_C", 4},
{L"Origin Frenzy", L"Default__AutoPistol_Circle_PrimaryAsset_C", L"Default__AutoPistol_Circle_Lv4_PrimaryAsset_C", 4},
{L"RGX 11z Pro Frenzy", L"Default__AutoPistol_Afterglow_PrimaryAsset_C", L"Default__AutoPistol_Afterglow_Lv4_PrimaryAsset_C", 4},

};
//
static const std::vector<SkinData> guardianSkins = {	

{L"Prime Guardian", L"Default__DMR_HypeBeast_PrimaryAsset_C", L"Default__DMR_HypeBeast_Lv4_PrimaryAsset_C", 4},
{L"Reaver Guardian", L"Default__DMR_SoulStealer_PrimaryAsset_C", L"Default__DMR_SoulStealer_Lv4_PrimaryAsset_C", 4},
{L"Ruination Guardian", L"Default__DMR_King_PrimaryAsset_C", L"Default__DMR_King_Lv4_PrimaryAsset_C", 4},
{L"Sovereign Guardian", L"Default__DMR_Sovereign_PrimaryAsset_C", L"Default__DMR_Sovereign_Lv4_PrimaryAsset_C", 4},
{L"RGX 11z Pro Guardian", L"Default__DMR_Afterglow_PrimaryAsset_C", L"Default__DMR_Afterglow_Lv4_PrimaryAsset_C", 4},
{L"Oni Guardian", L"Default__DMR_Oni_PrimaryAsset_C", L"Default__DMR_Oni_Lv4_PrimaryAsset_C", 4},

};
//
static const std::vector<SkinData> sheriffSkins = {

{L"Doombringer Sheriff", L"Default__Revolver_Cyberknight_PrimaryAsset_C", L"Default__Revolver_Cyberknight_v1_PrimaryAsset_C", 4},
{L"Neo Frontier Sheriff", L"Default__Revolver_Gunslinger_PrimaryAsset_C", L"Default__Revolver_Gunslinger_v1_PrimaryAsset_C", 4},
{L"RGX 11z Pro Sheriff", L"Default__Revolver_Afterglow3_PrimaryAsset_C", L"Default__Revolver_Afterglow3_v2_PrimaryAsset_C", 4},
{L"Mystbloom Sheriff", L"Default__Revolver_Spirit_PrimaryAsset_C", L"Default__Revolver_Spirit_v2_PrimaryAsset_C", 4},
{L"Kuronami Sheriff", L"Default__Revolver_Ninja_PrimaryAsset_C", L"Default__Revolver_Ninja_v1_PrimaryAsset_C", 4},
{L"Reaver Sheriff", L"Default__RevolverPistol_Soulstealer_PrimaryAsset_C", L"Default__RevolverPistol_Soulstealer_White_PrimaryAsset_C", 4},
{L"Singularity Sheriff", L"Default__RevolverPistol_Edge_PrimaryAsset_C", L"Default__RevolverPistol_Edge_Lv4_PrimaryAsset_C", 4},
{L"Protocol 781-A Sheriff", L"Default__Revolver_Protocol_PrimaryAsset_C", L"Default__Revolver_Protocol_Lv4_PrimaryAsset_C", 4},
{L"Sentinels of Light Sheriff", L"Default__Revolver_SOL_PrimaryAsset_C", L"Default__Revolver_SOL_Lv4_PrimaryAsset_C", 4},
{L"ChronoVoid Sheriff", L"Default__Revolver_Daedalus_PrimaryAsset_C", L"Default__Revolver_Daedalus_Lv4_PrimaryAsset_C", 4},
{L"Magepunk Sheriff", L"Default__Revolver_Magepunk2_PrimaryAsset_C", L"Default__Revolver_Magepunk2_Lv4_PrimaryAsset_C", 4},
{L"Nebula Sheriff", L"Default__Revolver_Cosmos_PrimaryAsset_C", L"Default__Revolver_Cosmos_Lv1_PrimaryAsset_C", 4},
{L"Arcane Sheriff", L"Default__Revolver_XP1_PrimaryAsset_C", L"Default__Revolver_XP1_Lv2_PrimaryAsset_C", 4},
{L"Schema Sheriff", L"Default__Revolver_Schema_PrimaryAsset_C", L"Default__Revolver_Schema_Lv1_PrimaryAsset_C", 4},
{L"Crimsonbeast Sheriff", L"Default__Revolver_Mythic_PrimaryAsset_C", L"Default__Revolver_Mythic_Lv2_PrimaryAsset_C", 4},
{L"Signature Sheriff", L"Default__Revolver_MidnightAerosol_PrimaryAsset_C", L"Default__Revolver_MidnightAerosol_PrimaryAsset_C", 4},
{L"Abyssal Sheriff", L"Default__Revolver_DeepSea_PrimaryAsset_C", L"Default__Revolver_DeepSea_Lv1_PrimaryAsset_C", 4},
{L"Ion Sheriff", L"Default__RevolverPistol_Oblivion_PrimaryAsset_C", L"Default__RevolverPistol_OblivionLv4_PrimaryAsset_C", 4},

};
static const std::vector<SkinData> ghostSkins = {
{L"Phaseguard Ghost", L"Default__Luger_Commando_PrimaryAsset_C", L"Default__Commando_v1_PrimaryAsset_C", 4},
{L"Reaver Ghost", L"Default__Luger_Soulstealer2_PrimaryAsset_C", L"Default__Luger_Soulstealer2_v1_PrimaryAsset_C", 4},
{L"XERØFANG Ghost", L"Default__Luger_Hypedragon_PrimaryAsset_C", L"Default__Luger_Hypedragon_v1_PrimaryAsset_C", 4},
{L"Sovereign Ghost", L"Default__LugerPistol_Sovereign_PrimaryAsset_C", L"Default__LugerPistol_Sovereign_Purple_PrimaryAsset_C", 4},
{L"Ruination Ghost", L"Default__Luger_King_PrimaryAsset_C", L"Default__Luger_King_Lv4_PrimaryAsset_C", 4},
{L"Magepunk Ghost", L"Default__Luger_Magepunk_PrimaryAsset_C", L"Default__Luger_Magepunk_Lv4_PrimaryAsset_C", 4},
{L"Gaia's Vengeance Ghost", L"Default__LugerPistol_Ashen_PrimaryAsset_C", L"Default__LugerPistol_Ashen_Lv4_PrimaryAsset_C", 4},

};
//
static const std::vector<SkinData> spectreSkins = {
{L"Kuronami Spectre", L"Default__MP5_Ninja_PrimaryAsset_C", L"Default__MP5_Ninja_v1_PrimaryAsset_C", 4},
{L"Ruination Spectre", L"Default__MP5_King_PrimaryAsset_C", L"Default__MP5_King_Lv4_PrimaryAsset_C", 4},
{L"Protocol 781-A Spectre", L"Default__MP5_Protocol_PrimaryAsset_C", L"Default__MP5_Protocol_Lv4_PrimaryAsset_C", 4},
{L"Reaver Spectre", L"Default__MP5_Soulstealer2_PrimaryAsset_C", L"Default__MP5_Soulstealer2_Lv4_PrimaryAsset_C", 4},
{L"Radiant Crisis 001 Spectre", L"Default__MP5_Comicbook_PrimaryAsset_C", L"Default__MP5_Comicbook_Lv3_PrimaryAsset_C", 4},
};
//
static const std::vector<SkinData> classicSkins = {
{L"VCT x SEN Classic", L"Default__BasePistol_VCTCaps24_SEN_PrimaryAsset_C", L"Default__BasePistol_VCTCaps24_SEN_V1_PrimaryAsset_C", 4},
{L"VCT x G2 Classic", L"Default__BasePistol_VCTCaps24_G2_PrimaryAsset_C", L"Default__BasePistol_VCTCaps24_G2_Lv3_PrimaryAsset_C", 4},
{L"VCT x BBL Classic", L"Default__BasePistol_VCTCaps24_BBL_PrimaryAsset_C", L"Default__BasePistol_VCTCaps24_BBL_Lv3_PrimaryAsset_C", 4},
{L"Spectrum Classic", L"Default__BasePistol_Atlas_PrimaryAsset_C", L"Default__BasePistol_Atlas_v1_PrimaryAsset_C", 4},
{L"Prime Classic", L"Default__BasePistol_HypeBeast_PrimaryAsset_C", L"Default__BasePistol_HypeBeast_Blue_PrimaryAsset_C", 4},
{L"RGX 11z Pro Classic", L"Default__BasePistol_Afterglow2_PrimaryAsset_C", L"Default__BasePistol_Afterglow2_Lv4_PrimaryAsset_C", 4},
{L"Forsaken Classic", L"Default__BasePistol_Fallen_PrimaryAsset_C", L"Default__BasePistol_Fallen_Lv3_PrimaryAsset_C", 4},
{L"Glitchpop Classic", L"Default__BasePistol_Cyberpunk2_PrimaryAsset_C", L"Default__BasePistol_Cyberpunk2_Lv4_PrimaryAsset_C", 4},
};
//
static const std::vector<SkinData> marshalSkins = {
{L"Nocturnum Marshal", L"Default__LeverSniper_Coven_PrimaryAsset_C", L"Default__LeverSniper_Coven_V1_PrimaryAsset_C", 4},
{L"Kuronami Marshal", L"Default__LeverSniper_Ninja_PrimaryAsset_C", L"Default__LeverSniper_Ninja_v1_PrimaryAsset_C", 4},
{L"Sovereign Marshal", L"Default__LeverSniperRifle_Sovereign_PrimaryAsset_C", L"Default__LeverSniperRifle_Sovereign_Purple_PrimaryAsset_C", 4},
};
//
//static const std::vector<SkinData> judgeSkins = {
//	{L"Default__AutomaticShotgun_Soulstealer_PrimaryAsset_C", L"Default__AutomaticShotgun_Soulstealer_Lv2_PrimaryAsset_C", 4},
//	{L"Default__AutomaticShotgun_Dragon_PrimaryAsset_C", L"Default__AutomaticShotgun_Dragon_Lv4_PrimaryAsset_C", 4},
//	{L"Default__AutomaticShotgun_Ninja_PrimaryAsset_C", L"Default__AutomaticShotgun_Ninja_v2_PrimaryAsset_C", 4},
//	{L"Default__AutomaticShotgun_Sovereign_PrimaryAsset_C", L"Default__AutomaticShotgun_Sovereign_Lv4_PrimaryAsset_C", 4},
//	{L"Default__AutomaticShotgun_Oni_PrimaryAsset_C", L"Default__AutomaticShotgun_Oni_Lv4_PrimaryAsset_C", 4},
//};
//
//static const std::vector<SkinData> ariesSkins = {
//	{L"Default__LMG_SOL_PrimaryAsset_C", L"Default__LMG_SOL_Lv4_PrimaryAsset_C", 4},
//	{L"Default__LMG_Dragon_PrimaryAsset_C", L"Default__LMG_Dragon_Lv4_PrimaryAsset_C", 4},
//	{L"Default__LMG_Ninja_PrimaryAsset_C", L"Default__LMG_Ninja_v2_PrimaryAsset_C", 4},
//	{L"Default__LMG_Soulstealer_PrimaryAsset_C", L"Default__LMG_Soulstealer_Lv4_PrimaryAsset_C", 4},
//	{L"Default__LMG_Oni_PrimaryAsset_C", L"Default__LMG_Oni_Lv4_PrimaryAsset_C", 4},
//};
//	
//static const std::vector<SkinData> odinSkins = {
//	{L"Default__HMG_Soulstealer_PrimaryAsset_C", L"Default__HMG_Soulstealer_Lv2_PrimaryAsset_C", 4},
//	{L"Default__HMG_Dragon_PrimaryAsset_C", L"Default__HMG_Dragon_Lv4_PrimaryAsset_C", 4},
//	{L"Default__HMG_Ninja_PrimaryAsset_C", L"Default__HMG_Ninja_v2_PrimaryAsset_C", 4},
//	{L"Default__HMG_Sovereign_PrimaryAsset_C", L"Default__HMG_Sovereign_Lv4_PrimaryAsset_C", 4},
//	{L"Default__HMG_Oni_PrimaryAsset_C", L"Default__HMG_Oni_Lv4_PrimaryAsset_C", 4},
//};
//
//static const std::vector<SkinData> buckySkins = {
//	{L"Default__PumpShotgun_Soulstealer_PrimaryAsset_C", L"Default__PumpShotgun_Soulstealer_Lv2_PrimaryAsset_C", 4},
//	{L"Default__PumpShotgun_Dragon_PrimaryAsset_C", L"Default__PumpShotgun_Dragon_Lv4_PrimaryAsset_C", 4},
//	{L"Default__PumpShotgun_Ninja_PrimaryAsset_C", L"Default__PumpShotgun_Ninja_v2_PrimaryAsset_C", 4},
//	{L"Default__PumpShotgun_Sovereign_PrimaryAsset_C", L"Default__PumpShotgun_Sovereign_Lv4_PrimaryAsset_C", 4},
//	{L"Default__PumpShotgun_Oni_PrimaryAsset_C", L"Default__PumpShotgun_Oni_Lv4_PrimaryAsset_C", 4},
//};
//
//static const std::vector<SkinData> shortySkins = {
//	{L"Default__SawedOffShotgun_Soulstealer_PrimaryAsset_C", L"Default__SawedOffShotgun_Soulstealer_Lv2_PrimaryAsset_C", 4},
//	{L"Default__SawedOffShotgun_Dragon_PrimaryAsset_C", L"Default__SawedOffShotgun_Dragon_Lv4_PrimaryAsset_C", 4},
//	{L"Default__SawedOffShotgun_Ninja_PrimaryAsset_C", L"Default__SawedOffShotgun_Ninja_v2_PrimaryAsset_C", 4},
//	{L"Default__SawedOffShotgun_Sovereign_PrimaryAsset_C", L"Default__SawedOffShotgun_Sovereign_Lv4_PrimaryAsset_C", 4},
//	{L"Default__SawedOffShotgun_Oni_PrimaryAsset_C", L"Default__SawedOffShotgun_Oni_Lv4_PrimaryAsset_C", 4},
//};
//
//static const std::vector<SkinData> stingerSkins = {
//	{L"Default__Vector_Soulstealer_PrimaryAsset_C", L"Default__Vector_Soulstealer_Lv2_PrimaryAsset_C", 4},
//	{L"Default__Vector_Dragon_PrimaryAsset_C", L"Default__Vector_Dragon_Lv4_PrimaryAsset_C", 4},
//	{L"Default__Vector_Ninja_PrimaryAsset_C", L"Default__Vector_Ninja_v2_PrimaryAsset_C", 4},
//	{L"Default__Vector_Sovereign_PrimaryAsset_C", L"Default__Vector_Sovereign_Lv4_PrimaryAsset_C", 4},
//	{L"Default__Vector_Oni_PrimaryAsset_C", L"Default__Vector_Oni_Lv4_PrimaryAsset_C", 4},
//};
//
static const std::vector<SkinData> knifeskins = {
{L"Phaseguard Splitter", L"Default__Melee_Commando_PrimaryAsset_C", L"Default__Melee_Commando_v1_PrimaryAsset_C", 2},
{L"Beta Remastered", L"Default__Melee_Warring_PrimaryAsset_C", L"Default__Melee_Warring_v1_PrimaryAsset_C", 2},
{L"Divergence St4ff", L"Default__Melee_Anomaly_PrimaryAsset_C", L"Default__Melee_Anomaly_v1_PrimaryAsset_C", 2},
{L"Helix Daggers", L"Default__Melee_Snake_PrimaryAsset_C", L"Default__Melee_Snake_v2_PrimaryAsset_C", 2},
{L"Blades of Primordia", L"Default__Melee_Hellfire_PrimaryAsset_C", L"Default__Melee_Hellfire_v2_PrimaryAsset_C", 2},
{L"Bolt Knife", L"Default__Melee_Bolt_PrimaryAsset_C", L"Default__Melee_Bolt_v1_PrimaryAsset_C", 2},
{L"CYRAX Fanblade", L"Default__Melee_Syndra_PrimaryAsset_C", L"Default__Melee_Syndra_v1_PrimaryAsset_C", 2},
{L"VCT 2025 Karambit", L"Default__Melee_VCT25Season_PrimaryAsset_C", L"Default__Melee_VCT25Season_v1_PrimaryAsset_C", 2},
{L"EX.O Edge", L"Default__Melee_Hologram_PrimaryAsset_C", L"Default__Melee_Hologram_v3_PrimaryAsset_C", 2},
{L"Doombringer Battleaxe", L"Default__Melee_Cyberknight_PrimaryAsset_C", L"Default__Melee_Cyberknight_v1_PrimaryAsset_C", 2},
{L"Neo Frontier Axe", L"Default__Melee_Gunslinger_PrimaryAsset_C", L"Default__Gunslinger_Syndra_v1_PrimaryAsset_C", 2},
{L"Evori's Spellcaster", L"Default__Melee_StarPower_PrimaryAsset_C", L"Default__Melee_StarPower_v1_PrimaryAsset_C", 2},
{L"Singularity Butterfly Knife", L"Default__Melee_Edge2_PrimaryAsset_C", L"Default__Melee_Edge2_v1_PrimaryAsset_C", 2},
{L"Nocturnum Scythe", L"Default__Melee_Coven_PrimaryAsset_C", L"Default__Melee_Coven_v1_PrimaryAsset_C", 2},
{L"Champions 2024 Blade", L"Default__Melee_Champions2024_PrimaryAsset_C", L"Default__Melee_Champions2024_Lv3_PrimaryAsset_C", 2},
{L"RGX 11z Pro Karambit", L"Default__Melee_Afterglow3_PrimaryAsset_C", L"Default__Melee_Afterglow3_v2_PrimaryAsset_C", 2},
{L"Mystbloom Kunai", L"Default__Melee_Spirit_PrimaryAsset_C", L"Default__Melee_Spirit_v2_PrimaryAsset_C", 2},
{L"Eternal Sovereign", L"Default__Melee_Sovereign2_PrimaryAsset_C", L"Default__Melee_Sovereign2_v1_PrimaryAsset_C", 2},
{L"Waveform", L"Default__Melee_Atlas_PrimaryAsset_C", L"Default__Melee_Atlas_v1_PrimaryAsset_C", 2},
{L"XERØFANG Knife", L"Default__Melee_Hypedragon_PrimaryAsset_C", L"Default__Melee_Hypedragon_v2_PrimaryAsset_C", 2},
{L"Kuronami no Yaiba", L"Default__Melee_Ninja_PrimaryAsset_C", L"Default__Melee_Ninja_V1_PrimaryAsset_C", 2},
{L"Champions 2023 Kunai", L"Default__Melee_Champions2023_PrimaryAsset_C", L"Default__Melee_Champions2023_PrimaryAsset_C", 2},
{L"Magepunk Sparkswitch", L"Default__Melee_Magepunk3_PrimaryAsset_C", L"Default__Melee_Magepunk3_PrimaryAsset_C", 2},
{L"VCT LOCK//IN Misericórdia", L"Default__Melee_Esportsinvitational_PrimaryAsset_C", L"Default__Melee_Esportsinvitational_Standard_PrimaryAsset_C", 2},
{L"Champions 2021 Karambit", L"Default__Melee_Champions_PrimaryAsset_C", L"Default__Melee_Champions_Lv2_PrimaryAsset_C", 2},
{L"Reaver Karambit", L"Default__Melee_Soulstealer2_PrimaryAsset_C", L"Default__Melee_Soulstealer2_v1_PrimaryAsset_C", 2},
{L"Black.Market Butterfly Knife", L"Default__Melee_Infantry2_Lv2_PrimaryAsset_C", L"Default__Melee_Infantry2_PrimaryAsset_C", 2},
{L"RGX 11z Pro Firefly", L"Default__Melee_Afterglow2_PrimaryAsset_C", L"Default__Melee_Afterglow2_v2_PrimaryAsset_C", 2},
{L"Xenohunter Knife", L"Default__Melee_Alien_PrimaryAsset_C", L"Default__Melee_Alien_Lv2_PrimaryAsset_C", 2},
{L"Prime//2.0 Karambit", L"Default__Melee_HypeBeast2_PrimaryAsset_C", L"Default__Melee_HypeBeast2_Lv2_PrimaryAsset_C", 2},
{L"Reaver Knife", L"Default__Melee_Soulstealer_PrimaryAsset_C", L"Default__Melee_Soulstealer_Lv2_PrimaryAsset_C", 2},
{L"Radiant Crisis 001 Baseball Bat", L"Default__Melee_Comicbook_PrimaryAsset_C", L"Default__Melee_Comicbook_Lv2_PrimaryAsset_C", 2},
//{L"NOT FOUND", L"Default__Melee_Dragon_PrimaryAsset_C", L"Default__Melee_Dragon_Lv2_PrimaryAsset_C", 2},
{L"Araxys Bio Harvester", L"Default__Melee_Antares_v2_PrimaryAsset_C", L"Default__Melee_Antares_Lv2_PrimaryAsset_C", 2},
{L"Soulstrife Scythe", L"Default__Melee_Haunted_PrimaryAsset_C", L"Default__Melee_Haunted_Lv2_PrimaryAsset_C", 2},
{L"Crimsonbeast Hammer", L"Default__Melee_Mythic_PrimaryAsset_C", L"Default__Melee_Mythic_Lv2_PrimaryAsset_C", 2},
{L"Magepunk Shock Gauntlet", L"Default__Melee_Magepunk2_PrimaryAsset_C", L"Default__Melee_Magepunk2_Lv2_PrimaryAsset_C", 2},
//{L"NOT FOUND", L"Default__Melee_Oni_PrimaryAsset_C", L"Default__Melee_Oni_Lv2_PrimaryAsset_C", 2},
{L"Onimaru Kunitsuna", L"Default__Melee_Oni2_PrimaryAsset_C", L"Default__Melee_Oni2_Lv2_PrimaryAsset_C", 2},
{L"Forsaken Ritual Blade", L"Default__Melee_Fallen_Lv2_PrimaryAsset_C", L"Default__Melee_Fallen_PrimaryAsset_C", 2},
{L"Blade of Chaos", L"Default__Melee_DemonStone_PrimaryAsset_C", L"Default__Melee_DemonStone_v2_PrimaryAsset_C", 2},
};

static const std::vector<SkinData> outlawskins = {
	{L"NIGGA",L"Default__DS_Gun_Afterglow3_PrimaryAsset_C", L"Default__DS_Gun_Afterglow3_v2_PrimaryAsset_C", 2},
};

static const std::vector<SkinData> chamspions_sheriff = {
	{L"NIGGA",L"Default__AK_Champions_PrimaryAsset_C", L"Default__AK_Champions_Lv4_PrimaryAsset_C", 2},
};

void ApplyWeaponSkin(AAresEquippable* myweapon, const std::vector<SkinData>& skins, int& skinOption) {
	UObject* skin_data_asset;
	UObject* chroma_data_asset;

	skin_data_asset = (UObject*)UObject::StaticFindObject(skins[skinOption].skin);
	chroma_data_asset = (UObject*)UObject::StaticFindObject(skins[skinOption].chroma);

	Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, skins[skinOption].level, UWorldSave);
}
	void ChangeWeaponSkin(AAresEquippable* myweapon, const std::vector<SkinData>& skins, int& skinOption) {
		UObject* skin_data_asset;
		UObject* chroma_data_asset;

		if (Globals::EnableSkin && GetAsyncKeyState(Globals::SkinChangerKey)) {
			skinOption = (skinOption + 1) % skins.size();
			Sleep(140);
		}

		skin_data_asset = (UObject*)UObject::StaticFindObject(skins[skinOption].skin);
		chroma_data_asset = (UObject*)UObject::StaticFindObject(skins[skinOption].chroma);

		Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, skins[skinOption].level, UWorldSave);
	}
void ApplySkinChanges(AAresEquippable* myweapon) {
	std::string weapon_name = kismentsystemlibrary::get_object_name((UObject*)myweapon).ToString();

	if (weapon_name.find("AssaultRifle_AK_C") != std::string::npos) {

		ChangeWeaponSkin(myweapon, vandalSkins, vandalSkinOption);
	}
	else if (weapon_name.find("AssaultRifle_ACR_C") != std::string::npos) {
		
		ChangeWeaponSkin(myweapon, phantomSkins, phantomSkinOption);
	}
	else if (weapon_name.find("BoltSniper_C") != std::string::npos) {
		
		ChangeWeaponSkin(myweapon, operatorSkins, operatorSkinOption);
	}
	else if (weapon_name.find("AssaultRifle_Burst_C") != std::string::npos) {
		
		ChangeWeaponSkin(myweapon, bulldogSkins, bulldogSkinOption);
	}
	else if (weapon_name.find("AutomaticPistol_C") != std::string::npos) {
		
		ChangeWeaponSkin(myweapon, frenzySkins, frenzySkinOption);
	}
	else if (weapon_name.find("DMR_C") != std::string::npos) {
	
		ChangeWeaponSkin(myweapon, guardianSkins, guardianSkinOption);
	}
	else if (weapon_name.find("RevolverPistol_C") != std::string::npos) {
		
		static int sheriff_champions = 0;
		if (!Globals::championssheriff) {
			ChangeWeaponSkin(myweapon, sheriffSkins, sheriffSkinOption);
		}
		else {
			ApplyWeaponSkin(myweapon, chamspions_sheriff, sheriff_champions);
		}
		
	}
	else if (weapon_name.find("LugerPistol_C") != std::string::npos) {
		
		ChangeWeaponSkin(myweapon, ghostSkins, ghostSkinOption);
	}
	else if (weapon_name.find("SubMachineGun_MP5_C") != std::string::npos) {
	
		ChangeWeaponSkin(myweapon, spectreSkins, spectreSkinOption);
	}
	else if (weapon_name.find("BasePistol_C") != std::string::npos) {
	
		ChangeWeaponSkin(myweapon, classicSkins, classicSkinOption);
	}
	else if (weapon_name.find("LeverSniperRifle_C") != std::string::npos) {
			
		ChangeWeaponSkin(myweapon, marshalSkins, marshalSkinOption);
	}
	else if (weapon_name.find("Ability_Melee_Base_C") != std::string::npos) {
		
		ChangeWeaponSkin(myweapon, knifeskins, knifeSkinOption);
	}
	else if (weapon_name.find("DS_Gun_C") != std::string::npos) {
			
		ApplyWeaponSkin(myweapon, outlawskins, outlawSkinOption);
	}
}
void ChangeKnown(AAresEquippable* myweapon) {
	UObject* skin_data_asset;
	UObject* chroma_data_asset;
	if (kismentsystemlibrary::get_object_name((UObject*)myweapon).ToString().find("AutomaticShotgun_C") != std::string::npos)
	{
		/*skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__AutomaticShotgun_Dragon_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__AutomaticShotgun_Dragon_Blue_PrimaryAsset_C");*/
		/*skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__AutoShotgun_Sovereign2_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__AutoShotgun_Sovereign1_v1_PrimaryAsset_C");*/

		skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__AutoShotgun_Cyberknight_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__AutoShotgun_Cyberknight_v1_PrimaryAsset_C");
		Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, 4 , UWorldSave);
	}

	if (kismentsystemlibrary::get_object_name((UObject*)myweapon).ToString().find("LightMachineGun_C") != std::string::npos)
	{
		skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__LMG_SOL_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__LMG_SOL_Lv4_PrimaryAsset_C");
		Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, 4, UWorldSave);
	}

	if (kismentsystemlibrary::get_object_name((UObject*)myweapon).ToString().find(("HeavyMachineGun_C")) != std::string::npos)
	{
		skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__HMG_Cyberknight_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__HMG_Cyberknight_v1_PrimaryAsset_C");
		Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, 4, UWorldSave);
	}

	if (kismentsystemlibrary::get_object_name((UObject*)myweapon).ToString().find("PumpShotgun_C") != std::string::npos)
	{
		skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__PumpShotgun_Oblivion_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__PumpShotgun_OblivionLv4_PrimaryAsset_C");
		Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, 4, UWorldSave);
	}

	if (kismentsystemlibrary::get_object_name((UObject*)myweapon).ToString().find("SawedOffShotgun_C") != std::string::npos)
	{
		skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__SawedOffShotgun_Oni_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__SawedOffShotgun_Oni_Lv4_PrimaryAsset_C");
		Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, 4, UWorldSave);
	}

	if (kismentsystemlibrary::get_object_name((UObject*)myweapon).ToString().find("Vector_C") != std::string::npos)
	{
		skin_data_asset = (UObject*)UObject::StaticFindObject(L"Default__Vector_Sovereign_PrimaryAsset_C");
		chroma_data_asset = (UObject*)UObject::StaticFindObject(L"Default__Vector_Sovereign_Lv4_PrimaryAsset_C");
		Skin::ChangeSkin(myweapon, skin_data_asset, chroma_data_asset, 4, UWorldSave);
	}
}

UObject* get_finisher_from_identifier(const std::string& skinIdentifier) {
	if (skinIdentifier.empty())
		return nullptr;

	// Construct primary finisher object name
	std::string finisherName = "FXC_Finisher_" + skinIdentifier + "_Victim_C";
	std::wstring wFinisherName(finisherName.begin(), finisherName.end());

	// Try to find it directly
	UObject* effect = UObject::find_object2<UObject*>(wFinisherName.c_str(), reinterpret_cast<UObject*>(-1));

	// If not found, try stripping trailing digits as a fallback (e.g., Oni2 → Oni)
	if (!effect) {
		std::string fallback = skinIdentifier;
		while (!fallback.empty() && std::isdigit(fallback.back())) {
			fallback.pop_back();
		}

		if (!fallback.empty()) {
			std::string fallbackName = "FXC_Finisher_" + fallback + "_Victim_C";
			std::wstring wFallbackName(fallbackName.begin(), fallbackName.end());
			effect = UObject::find_object2<UObject*>(wFallbackName.c_str(), reinterpret_cast<UObject*>(-1));
		}
	}

	// Check for validity
	if (effect && Memory::IsValidPointer((uintptr_t)effect)) {
		return effect;
	}

	return nullptr;
}
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <cstdio>
// ---------- storage ----------
struct SkinItem {
	std::wstring name;   // full asset name
};

static std::unordered_map<std::string, std::vector<SkinItem>> g_byFamily;
static std::unordered_map<std::string, int> g_selectedIndexForFamily;

// ---------- weapon identifier -> family key ----------
static const std::pair<const char*, const char*> kWeaponToFamily[] = {
	{"AssaultRifle_AK_C",           "AssaultRifle_AK"},
	{"AssaultRifle_ACR_C",          "AssaultRifle_ACR"},
	{"BoltSniper_C",                "BoltSniper"},
	{"AssaultRifle_Burst_C",        "AssaultRifle_Burst"},
	{"AutomaticPistol_C",           "AutomaticPistol"},
	{"DMR_C",                        "DMR"},
	{"RevolverPistol_C",            "Revolver"},
	{"LugerPistol_C",               "LugerPistol"},
	{"SubMachineGun_MP5_C",         "SubMachineGun_MP5"},
	{"Vector_C",                    "SubMachineGun_Vector"},
	{"BasePistol_C",                "BasePistol"},
	{"LeverSniperRifle_C",          "LeverSniper"},
	{"DS_Gun_C",                    "DS_Gun"},
	{"Ability_Melee_Base_C",        "Melee"},
	{"HeavyMachineGun_C",           "HeavyMachineGun"},
	{"LightMachineGun_C",           "LightMachineGun"},
	{"SawedOffShotgun_C",           "SawedOffShotgun"},
	{"AutomaticShotgun_C",          "AutomaticShotgun"},
	{"PumpShotgun_C",               "PumpShotgun"},
};

// ---------- normalize logged names -> family key ----------
static std::string family_from_logged_name(const std::wstring& wname) {
	std::string s(wname.begin(), wname.end());

	const std::string pre = "Default__";
	const std::string suf = "_PrimaryAsset_C";
	if (s.rfind(pre, 0) == 0) s.erase(0, pre.size());
	if (s.size() >= suf.size() && s.compare(s.size() - suf.size(), suf.size(), suf) == 0)
		s.erase(s.size() - suf.size());

	if (s.rfind("AK_", 0) == 0) return "AssaultRifle_AK";
	if (s.rfind("Melee_", 0) == 0) return "Melee";
	if (s.rfind("Vector_", 0) == 0) return "SubMachineGun_Vector";
	if (s.rfind("Luger_", 0) == 0) return "LugerPistol";

	auto u1 = s.find('_');
	if (u1 == std::string::npos) return s;
	auto u2 = s.find('_', u1 + 1);
	return (u2 == std::string::npos) ? s.substr(0, u1) : s.substr(0, u2);
}

// ---------- store skin ----------
static void store_skin_by_name(const std::wstring& fullName) {
	std::string fam = family_from_logged_name(fullName);
	auto& vec = g_byFamily[fam];

	// Log every skin added
	//wprintf(L"[STORE] Family: %S | Skin: %ls\n", fam.c_str(), fullName.c_str());

	if (std::none_of(vec.begin(), vec.end(), [&](const SkinItem& it) { return it.name == fullName; })) {
		vec.push_back(SkinItem{ fullName });

		if (vec.size() == 1) {
			g_selectedIndexForFamily[fam] = 0;
		//	printf("[INFO] Initialized selected index for family: %s -> 0\n", fam.c_str());
		}
	}
}

std::string normalize_weapon_class(const std::string& weapon) {
	size_t pos = weapon.find_last_of('_');
	if (pos != std::string::npos && pos + 1 < weapon.size() &&
		std::all_of(weapon.begin() + pos + 1, weapon.end(), ::isdigit)) {
		return weapon.substr(0, pos); // Strip _123456
	}
	return weapon;
}
std::wstring get_chosen_skin(const std::string& weapon_name) {
	//printf("[DEBUG] get_chosen_skin() called with weapon_name: %s\n", weapon_name.c_str());

	std::string family;
	for (const auto& [key, fam] : kWeaponToFamily) {
		if (weapon_name == key) {
			family = fam;
			break;
		}
	}

	if (family.empty()) {
		//printf("[WARN] Weapon not found in mapping: %s\n", weapon_name.c_str());
		return L"";
	}

	//printf("[DEBUG] Resolved family: %s\n", family.c_str());

	auto it = g_byFamily.find(family);
	if (it == g_byFamily.end()) {
		//printf("[WARN] No skins stored for family: %s\n", family.c_str());
		return L"";
	}

	int index = g_selectedIndexForFamily[family];
	auto& skins = it->second;

	if (index < 0 || index >= static_cast<int>(skins.size())) {
	//	printf("[WARN] Invalid index %d for family: %s (skin count: %zu)\n",
			//index, family.c_str(), skins.size());
		return L"";
	}

	//wprintf(L"[DEBUG] Returning chosen skin: %ls\n", skins[index].name.c_str());
	return skins[index].name;
}



// ---------- retrieve finisher from skin ----------
UObject* get_finisher_from_skin(std::wstring skinData) {
	// Step 1: Remove "Default__" if present
	const std::wstring prefix = L"Default__";
	if (skinData.rfind(prefix, 0) == 0) {
		skinData = skinData.substr(prefix.length());
	}

	// Step 2: Strip "_PrimaryAsset_C" suffix
	const std::wstring suffix = L"_PrimaryAsset_C";
	size_t suffixPos = skinData.rfind(suffix);
	if (suffixPos != std::wstring::npos) {
		skinData = skinData.substr(0, suffixPos);
	}
	else {
		return nullptr; // Invalid format
	}

	// Step 3: Extract only the LAST segment (skin theme name)
	size_t lastUnderscore = skinData.find_last_of(L'_');
	if (lastUnderscore == std::wstring::npos)
		return nullptr;

	std::wstring skinNameW = skinData.substr(lastUnderscore + 1);
	std::string skinName(skinNameW.begin(), skinNameW.end());

	// Step 4: Build finisher object name
	std::string obj = "FXC_Finisher_" + skinName + "_Victim_C";
	std::wstring wobj(obj.begin(), obj.end());
	UObject* effect = UObject::find_object2<UObject*>(wobj.c_str(), reinterpret_cast<UObject*>(-1));

	// Optional: Remove trailing digits (Prime2, etc.)
	if (!effect) {
		std::string cleaned = skinName;
		while (!cleaned.empty() && std::isdigit(cleaned.back()))
			cleaned.pop_back();

		if (cleaned != skinName) {
			obj = "FXC_Finisher_" + cleaned + "_Victim_C";
			wobj = std::wstring(obj.begin(), obj.end());
			effect = UObject::find_object2<UObject*>(wobj.c_str(), reinterpret_cast<UObject*>(-1));
		}
	}

	//printf("[get_finisher_from_skin] Input: %ws -> Skin: %s -> Search: %ws -> Found: %p\n",
		/*skinData.c_str(), skinName.c_str(), wobj.c_str(), effect);*/

	return effect;


}



static std::unordered_map<skin_data_asset*, std::wstring> g_skinNameCache;

// Get the name with caching (optional refresh flag)
static const std::wstring& get_skin_name_cached(skin_data_asset* p, bool refresh = false) {
	static const std::wstring kEmpty;
	if (!p) return kEmpty;

	if (!refresh) {
		auto it = g_skinNameCache.find(p);
		if (it != g_skinNameCache.end()) return it->second;
	}

	// Query once, then copy into a stable std::wstring
	FString f = kismentsystemlibrary::get_object_name(p);
	auto [it, _] = g_skinNameCache.emplace(p, std::wstring(f.c_str()));
	if (!_) it->second.assign(f.c_str()); // if already existed & refresh==true
	return it->second;
}


AAresEquippable* myweapon;
AAresEquippable* lastweapon;
// File: unlock_all_skins_optimized.cpp
#include <unordered_set>
#include <unordered_map>
std::unordered_set<uintptr_t> unlocked_skin_ptrs;
std::unordered_set<uintptr_t> unlocked_chroma_ptrs;
std::unordered_set<uintptr_t> unlocked_level_ptrs;
std::unordered_map<skin_data_asset*, std::wstring> cached_skin_names;

void unlock_all_skins(UWorld* world) {
	auto* game_instance = ares_instance::get_ares_client_game_instance(world);
	if (!game_instance) return;

	auto* manager = game_instance->get_inventory_manager();
	if (!manager) return;

	TArray<UObject*> equippable_models = Memory::R<TArray<UObject*>>(reinterpret_cast<uintptr_t>(manager) + 0xe0);

	for (auto* model : equippable_models) {
		if (!model) continue;

		// Unlock skins
		TArray<UObject*> skins = Memory::R<TArray<UObject*>>(reinterpret_cast<uintptr_t>(model) + 0x150);
		for (auto* skin : skins) {
			if (!skin) continue;

			uintptr_t skin_ptr = reinterpret_cast<uintptr_t>(skin);
			if (unlocked_skin_ptrs.insert(skin_ptr).second) {
				Memory::W<bool>(skin_ptr + 0xfe, true);
				Memory::W<bool>(skin_ptr + 0xf2, true);
			}

			// Unlock levels
			TArray<UObject*> levels = Memory::R<TArray<UObject*>>(skin_ptr + 0x138);
			for (auto* level : levels) {
				if (!level) continue;

				uintptr_t level_ptr = reinterpret_cast<uintptr_t>(level);
				if (unlocked_level_ptrs.insert(level_ptr).second) {
					Memory::W<bool>(level_ptr + 0xfe, true);
					Memory::W<bool>(level_ptr + 0xf2, true);
				}
			}

			// Unlock chromas
			TArray<UObject*> chromas = Memory::R<TArray<UObject*>>(skin_ptr + 0x128);
			for (auto* chroma : chromas) {
				if (!chroma) continue;

				uintptr_t chroma_ptr = reinterpret_cast<uintptr_t>(chroma);
				if (unlocked_chroma_ptrs.insert(chroma_ptr).second) {
					Memory::W<bool>(chroma_ptr + 0xfe, true);
					Memory::W<bool>(chroma_ptr + 0xf2, true);
				}
			}
		}
	}
}

void update_weapon_skin_if_needed(AAresEquippable* current_weapon, AAresEquippable*& last_weapon, UAresInventory* inventory) {
	if (!current_weapon || current_weapon == last_weapon || !inventory) return;

	skin_data_asset* skin_data = current_weapon->GetSkinDataAsset();
	if (!skin_data || skin_data->get_type() == 0) return;

	auto* arsenal_view_controller = ares_instance::get_ares_client_game_instance(UWorldSave)->get_arsenal_view_controller();
	if (!arsenal_view_controller) return;

	arsenal_view_model* view_model = arsenal_view_controller->get_arsenal_view_model();
	if (!view_model) return;

	TArray<invetory_model*> models = view_model->get_gun_models();
	for (auto* model : models) {
		if (!model) continue;

		skin_inventory_model* skin_model = model->get_equipped_skin();
		if (!skin_model) continue;

		skin_data_asset* model_skin_data = skin_model->get_skin_data_asset();
		if (!model_skin_data || model_skin_data->get_type() != skin_data->get_type()) continue;

		// Cache skin name
		const std::wstring& skin_name = cached_skin_names.find(model_skin_data) != cached_skin_names.end()
			? cached_skin_names[model_skin_data]
			: cached_skin_names[model_skin_data] = get_skin_name_cached(model_skin_data);

		store_skin_by_name(skin_name);

		int max_level = model_skin_data->get_levels().size();

		auto* chroma_asset = skin_model->get_skin_invetory_chroma_asset()->get_skin_chroma_asset();
		if (!chroma_asset) continue;

		if (Globals::BuddyChanger) {
			Skin::ChangeSkinbuddy(current_weapon, model_skin_data, chroma_asset, max_level, UWorldSave);
		}
		else {
			Skin::ChangeSkin(current_weapon, model_skin_data, chroma_asset, max_level, UWorldSave);
		}
		break; // Stop after applying one matching skin
	}

	last_weapon = current_weapon;
}

void tick_unlock_all_logic(AShooterCharacter* shooter, APawn* pawn) {
	if (!Globals::UnlockAll) return;

	SPOOF_FUNC;
	SPOOF_CALL(unlock_all_skins)(UWorldSave);

	if (!pawn || !shooter) return;

	UAresInventory* inventory = shooter->GetInventory();
	if (!inventory) return;

	AAresEquippable* current_weapon = inventory->GetCurrentWeapon();
	update_weapon_skin_if_needed(current_weapon, lastweapon, inventory);
}
