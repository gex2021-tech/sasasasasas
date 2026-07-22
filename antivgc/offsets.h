#include "canvas_gui.h"
#include "unrealengine.h"
#pragma once
static int vandalSkinOption = 0;
static int sheriffSkinOption = 0;
static int guardianSkinOption = 0;
static int frenzySkinOption = 0;
static int bulldogSkinOption = 0;
static int operatorSkinOption = 0;
static int phantomSkinOption = 0;
static int ghostSkinOption = 0;
static int spectreSkinOption = 0;
static int classicSkinOption = 0;
static int marshalSkinOption = 0;
static int knifeSkinOption = 0;
static int outlawSkinOption = 0;
static int selectedSkinIndex = 0; // default to first
namespace menusize {
	float xsize = 595.0f, ysize = 400.0f;
}

FVector2D menu_size = FVector2D{ 620, 450 };
namespace Antivgc {
	float EmissiveIntensity = 3.0f;
	float LogoGlowIntensity = 2.0f;
	float LogoMetallic = 0.95f;
	float LogoRoughness = 0.15f;
	float GlowColor[3] = { 1.5f, 0.0f, 1.5f };  // RGB
}
namespace	Globals {
	inline float ViewmodelOffsetX = 0.0f;
	inline float ViewmodelOffsetY = 0.0f;
	inline float ViewmodelOffsetZ = 0.0f;
	float PredictionPeekMS = 90.f; // tune between 50 - 150 for peek prediction timing
	bool EnablePrediction = false;
	bool manualoverride = false;
	inline std::string chat_message = "Ego Destroyed!";
	static bool spin_paused = false;
	static bool waiting_for_kill = false;
	static DWORD resume_spin_time = 0;
	float ProjectileSpeed = 1.0f;
	bool self_fresnel = false;
	bool auto_wall = false;
	bool chatspam = false;
	bool auto_wall_visualize = false;
	float auto_wall_min_damage = 1.0f;
	bool auto_wall_autoshoot = false;
	int auto_wall_mode = 0;
	bool boxoutline = false;
	inline int BuddySearchScroll = 0;
	inline std::wstring BuddySearch = L""; // default empty
	bool antiflash = false;
	bool unlocklevels = false;
	bool unlockchromas = false;
	bool party_hat = false;
	bool usepresetedoutlines = false;
	bool killsay = false;
	bool killsounds = false;
	bool lineupesp = false;
	bool antivgcgun = false;
	bool customgun = false;
	bool autoapply = false;
	bool ragemode = false;
	bool skyboxRGB = false;
	bool galaxychams = false;
	bool galaxychams2 = false;
	bool galaxychams3 = false;
	bool skybox = false;
	bool skyboxmat = false;
	bool targetline = false;
	bool monkeymode = false;
	bool bPlayFinisherOnLastKillOnly = true;
	bool bPlayFinisherOnAnyKillWithEnemies = false;
	inline bool bOnlyLastKill = false;
	int skin_key = 0;
	inline bool customshit = false;
	inline bool finisher = false;
	inline bool hand_fresnel = false;
	inline bool nightmode = false;
	inline bool outlineRainbow = false;
	inline FVector2D WidgetOrigin; // Used for widget layout
	int outlinetype = 0;
	int visiblepreset = 0;
	int invisiblepreset = 0;
	bool threesixtyfov = false;
	inline int anti_aim_pitch = 0;
	inline int anti_aim_yaw = 0;
	bool bullettracers = false;
	bool FadeMaterialTest = false;
	bool AntiAim = false;
	bool SovaMaterialTest = false;
	bool ReynaMaterialTest = false;

	static float AutoshootFloat = 20.0f;
	inline float ScaleX = 2.5f;
	inline float ScaleY = 2.5f;
	inline float ScaleZ = 2.5f;

	// Hand offset (forward/down)
	inline float OffsetX = 20.0f;
	inline float OffsetY = 0.0f;
	inline float OffsetZ = -5.0f;

	// Weapon scale (default = 1.0)
	inline float WScaleX = 1.0f;
	inline float WScaleY = 1.0f;
	inline float WScaleZ = 1.0f;
	inline float intesityinner = 1.0f;
	inline float intesityoutter = 1.0f;
	inline float intesityedger = 1.0f;

	inline float scaleinspectx = 0.423117;
	inline float scaleinspecty = 1;
	inline float scaleinspectz = 1;
	inline float handheldx = 2.6;
	inline float handheldy = 1;
	inline float handheldz = 1;
	inline float knifex = 0.483117;
	inline float knifey = 1;
	inline float knifez = 1;
	inline bool bEnableAspectRatioChanger = false;
	float DesiredAspectRatio = 1.0f; // e.g., 2.33f
	bool handChamsRGB = false;
	bool gunChamsRGB = false;
	bool freecam = false;
	bool crazychams = false;
	bool handchams = false;
	bool crosshairrainbow = false;
	bool riotid = false;
	bool selfchams = false;
	bool gunmaterial = false;
	bool CoolFov = false;
	bool WeaponIcon = false;
	bool ShowCheatOptions = false;
	bool InvisibleEnemy = false;
	bool glowenemy = false;
	bool SpinBot = false;
	bool outline = false;
	bool outline2 = false;
	bool NoSmoke = false;
	bool FireSilent = false;
	bool Mesh3PModifed = false;
	bool dynamicrecoil = false;
	bool GadgetESP = false;
	bool player_view_angle = false;
	bool autoknife = false;
	bool CrossBool = true;
	int spinbotkey;
	bool tsgamerdoc = false;
	bool menucheck = true;
	bool WeaponChams = false;
	bool InvisibleWeapon = false;
	bool WireframePlayer = false;
	float glowenemyfloat = 1.0f;
	bool AgentName = false;
	bool AgentIcon = false;
	bool Aimlock = false;
	bool bigweapon = false;
	bool ClientSideSpinbot = false;
	bool InivisiblePlayer = false;
	bool ChamsPlayer = false;
	bool tpbool = false;
	bool spinbot = false;
	float spinbot_speed = 100;
	float third_person_float = 100;
	float view = 1;
	inline FVector2D current_subtab_origin = { 0, 0 };  // global or namespace

	bool bigplayer = false;
	bool UnlockAll = false;
	bool Thirdperson = false;
	float TPDistance = 300;
	bool HandGlow = false;
	bool TeamCheck = false;
	bool BuddyChanger = false;
	bool Buddytext = true;
	bool DistangeEsp = false;
	bool invisiblehands = false;
	bool EnableESP = true;
	bool RecoilCrosshair = false;
	bool BunnyHop = false;
	bool Box = false;
	bool Box2D = false;
	bool Silent = false;
	bool AntiAFK = false;
	bool HeadESP = false;
	bool BottomSnapline = false;
	bool WeaponEsp = false;
	bool HealthText = false;
	bool EnableSkin = true;
	bool EnableSkin2 = false;
	bool EnableSkinBool = false;
	bool SpikeEsp = false;
	bool Snapline = false;
	bool skill_esp = false;
	bool dropped_gun = false;
	bool Menu_Open = true;
	bool kekIsOff = 0;
	bool Shieldbar = false;
	bool CorneredBox = false;
	bool Healthbar = false;
	bool Box3D = false;
	bool ChamsESP = false;
	float fresnelBaseR = 2.093f;
	float fresnelBaseG = 0.019f;
	float fresnelBaseB = 20.0f;
	float Glow1 = 1.0f;
	float intesity = 1.0f;
	float CloudSpeed = 1.0f;
	float CloudOpacity = 1.0f;
	float StarsBrightness = 1.0f;
	wchar_t killsay_input[128] = L"acktwo howd you die to me"; // default killsay
	FLinearColor handcolor = { 1,1,1, Globals::intesity };
	FLinearColor guncolor = { 1,1,1, Globals::intesity };
	FLinearColor Overall = { 1,1,1, 1 };
	FLinearColor Zenith = { 1,1,1, 1 };
	FLinearColor Horizon = { 1,1,1, 1 };
	FLinearColor Cloud = { 1,1,1, 1 };
	FLinearColor glowcolor2 = { 1,1,1, 1 };
	bool custom_fresnel = false;
	bool rainbow_fresnel = false;
	bool rainbow_chams = false;
	bool visibleCheckChams = false;
	bool ChamsESP_not = true;
	bool ChamsESP2 = true;
	bool NoSpreadAimbot = false;
	bool NoSpreadAimbot_not = false;
	bool EnableAim = true;
	bool VisibleCheck = true;
	bool AccurateVisibleCheck = true;
	bool AutoShoot = false;
	bool FastCrouch = false;

	float GlowVisible = 10.f;
	float GlowInvisible = 10.f;
	float sCenterEdgeR_Visible = 0.0f;
	float sCenterEdgeG_Visible = 0.01f;
	float sCenterEdgeB_Visible = 0.0f;

	float sInnerEdgeR_Visible = 0.0f;
	float sInnerEdgeG_Visible = 0.1f;
	float sInnerEdgeB_Visible = 0.0f;

	float sOuterEdgeR_Visible = 0.0f;
	float sOuterEdgeG_Visible = 1.0f;
	float sOuterEdgeB_Visible = 0.0f;
	// === VISIBLE (Green Full Outline Glow) ===
	float CenterEdgeR_Visible = 0.0f;
	float CenterEdgeG_Visible = 0.01f;
	float CenterEdgeB_Visible = 0.0f;

	float InnerEdgeR_Visible = 0.0f;
	float InnerEdgeG_Visible = 0.1f;
	float InnerEdgeB_Visible = 0.0f;

	float OuterEdgeR_Visible = 0.0f;
	float OuterEdgeG_Visible = 1.0f;
	float OuterEdgeB_Visible = 0.0f;

	// === INVISIBLE (Hard Red Glow Through Walls) ===
	float CenterEdgeR_Invisible = 0.01f;
	float CenterEdgeG_Invisible = 0.0f;
	float CenterEdgeB_Invisible = 0.0f;

	float InnerEdgeR_Invisible = 0.1f;
	float InnerEdgeG_Invisible = 0.0f;
	float InnerEdgeB_Invisible = 0.0f;

	float OuterEdgeR_Invisible = 1.0f;
	float OuterEdgeG_Invisible = 0.0f;
	float OuterEdgeB_Invisible = 0.0f;
	inline float cachepitch = 0.0f;
	inline float cachebaseyaw = 0.0f;
	inline float cacheoffset = 0.0f;
	inline int pitchmode = 0;
	inline int baseyaw = 0;
	bool desync_enabled = false;
	float desync_amount = 30.f;

	bool oscillate_enabled = false;
	float oscillation_speed = 5.f;
	float oscillation_range = 60.f;
	int antiaimtype = 0; // 0=Static, 1=Jitter, 2=Spin, 3=Random
	int viewmodeltype = 0; // 0=Long, 1=CSGO, 2=Spin, 3=Random
	float speed = 10.f;
	inline float offset = 118;
	inline int yawmodifier = 0;
	inline float modifieroffset = 139;

	/*	float OuterEdgeR_Invisible, OuterEdgeG_Invisible, OuterEdgeB_Invisible;*/
	bool FovChangerBool = false;
	bool Always = false;
	bool Behind = true;
	bool DrawFov = false;
	bool HealthEsp = false;
	bool SkipTutorial = false;
	bool ViewModelChanger = false;
	bool championssheriff = false;
	bool Minimap = false;
	bool weaponesp = false;
	bool Watermark = true;
	bool streammode = false;
	bool Visible_Check_ESP = false;
	bool SkeletonEsp = false;
	bool WireFrameWeapon = false;
	bool OwnWireframe = false;
	bool null = false;
	bool big_gun = false;
	bool WireFrameHand = false;
	bool RecoilControl = false;
	int LegitBotKey;
	int chatpam;
	int left;
	int right;
	int backwards;
	int autoshootnigga;
	int MenuKey = VK_INSERT;
	int Silentaim;
	int SkinChangerKey;
	int ApplySkinKey;
	int Width = GetSystemMetrics(SM_CXSCREEN);
	int Height = GetSystemMetrics(SM_CYSCREEN);
	int AimBone = 0;
	int WatermarkType = 0;
	int tpkey;
	int aakey;
	int SelectSnapline = 1;
	static float FovChangerValue = 105.0f;
	static float LegitFOvValue = 30.0f;
	float Glow = 2.0f;
	float skeleton_size = 1.7f;
	float biggunvalue = 1.0f;
	float bigplayervalue = 1.0f;
	float ColorGlow = 4.0f;
	float ESPThickness = 1.1f;
	float FovCircle = 200.0f;
	float PlayerDistange = 400.0f;
	float ScreenCenterX = Width / 2;
	float ScreenCenterY = Height / 2;
	float smooth = 1.0f;
	float ViewmodelX = 2.6f;
	float ViewmodelY = 1.0f;
	float TestKnifeX = 2.6f;
	float TestKnifeY = 1.0f;
	float WeaponKnifeX = 2.6f;
	float WeaponKnifeY = 1.0f;
	float testnigga = 1.0f;
	//Skin Value
	int SkinKnife = 0;
	int SkinVandal = 0;
	int SkinSheriff = 0;
	int SkinSheriff2 = 0;
	int SkinOperator = 0;
	int SkinSpectre = 0;
	int SkinPhantom = 0;
	int SkinBulldog = 0;
	int SkinFrenzy = 0;
	int SkinGuardian = 0;
	int SkinGhost = 0;
	int SkinClassic = 0;
	inline bool ShowPlayerName = false;
	inline bool ShowRank = false;
	inline bool ShowLevel = false;
	inline bool ShowPing = false;
	inline bool DrawLeaderboard = false;

	static bool t = true;

	float spinMULTIX = 3;
	static float spinY = 10;
	static float spinX = 10;
	static float spinMULTIY = 10.f;
	static float spinMULTIZ = 10.f;
	float Pitch = 1;
	float Yaw = 1;
	float Roll = 1;
}
namespace f {

	FVector	LocalCameraRotation;
	FVector	LocalCameraRotation2;
	FVector	CurrentTargetAimAngle;
	FVector	LocalCameraLocation;

	static bool bLockedCameraRotation = false;
	static bool bLockedCameraRotation_2 = false;
	static bool bFlickSilent = true;
	float LocalCameraFOV;
	float PreviousRotationX = 0.0f;
	float PreviousRotationY = 0.0f;
	int bone = 0;

	float OldLocalCameraFOV;
	FVector OldCameraPos;
	FVector OldCameraRot;
}
namespace Silent_f {

	FVector	LocalCameraRotation;
	FVector	LocalCameraRotation2;
	FVector	CurrentTargetAimAngle;
	FVector	LocalCameraLocation;

	static bool bLockedCameraRotation = false;
	static bool bLockedCameraRotation_2 = false;

	static bool bFlickSilent = true;
	float LocalCameraFOV;
	FVector OldCameraPos = f::LocalCameraLocation;
	FVector OldCameraRot = f::LocalCameraRotation;
	float PreviousRotationX = 0.0f;
	float PreviousRotationY = 0.0f;
	int bone = 0;
}
namespace Colors {
	FLinearColor ChamsColor{ 2.093f, 0.019f, 20.0f, 2.6f };
	FLinearColor WeaponColor{ 2.093f, 0.019f, 20.0f, 1.0f };
	FLinearColor FovColor = { 255.0f, 255.0f, 255.0f, 0.1f };
	FLinearColor CrosshairColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	FLinearColor BoxVisible = { 1,1,1,1 };
	FLinearColor Boxinvisible = { 1,0,0,1 };
	FLinearColor ChamsVisible = { 0.0f, 1.0f, 0.0f, 1.0f }; // Green
	FLinearColor ChamsInvisible = { 1.0f, 0.0f, 0.0f, 1.0f }; // Red

	FLinearColor LineVisible = { 1,0,0,1 };
	FLinearColor LineINvisible = { 1,1,1,1 };

	FLinearColor SkeletonVisible = { 1,1,1,1 };
	FLinearColor SkeletonInvisible = { 1,0,0,1 };

	FLinearColor WeaponNameColor = { 25,25,25,2.5f };
	FLinearColor AgentNameColor = { 25,25,25,2.5f };
	FLinearColor PlayerColor{ 2.093f, 0.019f, 20.0f, 1.0f };
	FLinearColor ChamsColor1;	
	FLinearColor BoxColor;
	FLinearColor Skeleton;
	FLinearColor LineColor;
}
constexpr uint64_t camerapos = 0x11e0;
constexpr uint64_t camerafov = 0x11d0;
constexpr uint64_t camerarot = 0x11ec;
namespace Offsets {
	uintptr_t Mesh = 0x468;
	uintptr_t portrait_map = 0x1210;
	uintptr_t character_map = 0x1218;
	constexpr uint64_t PlayerState = 0x480;
	constexpr uint64_t PlatformPlayer = 0x678;
	constexpr uint64_t CompetetiveTier = 0x6a0;
	constexpr uint64_t AccountLevel = 0x69c;
	constexpr uint64_t get_ping = 0x464;
	constexpr uint64_t RiotName = 0x3d8;
	constexpr uint64_t RelativeScale3D = 0x1a0;
	constexpr uint64_t WireFrame = 0x71e;
	constexpr uint64_t WireFrame2 = 0xc0;
	constexpr uint64_t WireFrame3 = 0xff;
	constexpr uint64_t Inventory = 0xbf0;
	constexpr uint64_t Equippable = 0x248;


	// Core Offset	
	constexpr uint64_t State = 0xB74B5E0;

	std::uintptr_t process_event = 0x19A1120;
	std::uintptr_t static_find_object = 0x19CBEC0;
	std::uintptr_t static_load_object = 0x19CF9F0;
	std::uint64_t MagicOffsets = 0x46C4660;
	std::uintptr_t play_finisher_effect = 0x5B42E40;
	std::uintptr_t set_ares_outline = 0x3E36840;
	std::uintptr_t bone_matrix = 0x3EA23F0;
	std::uintptr_t MarkDirtRenderState = 0x1571ab0;
	std::uintptr_t fmemory_malloc = 0x1581310;
	std::uintptr_t OwningGameInstance = 0x1D8;
	std::uintptr_t Triggerveh = 0x15B8876;

	// No spread offsets
	std::uintptr_t get_spread_values = 0x5BCFD10;
	std::uintptr_t get_spread_angles = 0x66E0A20;
	std::uintptr_t get_firing_location_direction = 0x3BF2000; //get_firing
	std::uintptr_t tovector_and_normalize = 0x167D8D0; //to_vector
	std::uintptr_t toangle_and_normalize = 0x16780D0; //to_angle
	std::uintptr_t error_power = 0x49c;
	std::uintptr_t error_retries = 0x470; 
	std::uintptr_t firing_state_component = 0x1200;
	std::uintptr_t seed_data = 0x4A0;
	constexpr uint64_t camerapos = 900;
	constexpr uint64_t camerafov = 930;
	constexpr uint64_t camerarot = 918;
	std::uintptr_t seed_addsomeshit = 0xD8;//https://dumpspace.spuckwaffel.com/Games/?hash=81c67bd2&type=classes&idx=UFiringStateComponent&member=NetworkedRandomNumberGeneratorComponent
	std::uintptr_t stability_component = 0x490; //https://dumpspace.spuckwaffel.com/Games/?hash=81c67bd2&type=classes&idx=UFiringStateComponent&member=RegisteredStabilityComponent

	// Mesh Components
	constexpr uint64_t Mesh3pGun = 0xde8;
	constexpr uint64_t mesh1pgun = 0xdd8;
	std::uintptr_t mesh1p = 0xf10;
	std::uintptr_t death_reaction_component_offset = 0xCb8;
	std::uintptr_t montage_effect_override_offset = 0x100;
	std::uintptr_t montage_effect_override_context_offset = 0x108;
	std::uintptr_t skyboxmeshcomponent = 0x288;
	std::uintptr_t mesh1p_overlay = 0xf18;
	std::uintptr_t Mesh3P = 0x0D88;
	std::uintptr_t mesh_cosmetic_3p = 0xf20; //		
	std::uintptr_t thirdpersoncosmetic = 0xf20; //mesh_cosmetic
	std::uintptr_t bone_cout = 0x730 + 0x8;
	std::uintptr_t real_time_dormant = 0x101;
	std::uintptr_t mesh3p_mids = 0xF68;
	std::uintptr_t mesh1p_mids = 0xF78;
	std::uintptr_t Mesh1POverlayMIDs = 0x0F88;
	//struct UTexture* InventoryIcon;	
	std::uintptr_t InventoryIcon = 0x0EB0;

	// Class Engine.SkinnedMeshComponent
	std::uintptr_t bForceWireframe = 0x8fe;
	std::uintptr_t wireframe_num_1 = 0xC0;
	std::uintptr_t ClipX = 0x0038;
	std::uintptr_t ClipY = 0x003C;
	//struct UEquippableSkinDataAsset *SkinDataAsset;
	std::uintptr_t skin_data_assets = 0xDC8;
	std::uintptr_t skin_pointer = 0x3a0;
	std::uintptr_t skin_pointer_2 = 0xB0;
	std::uintptr_t skin_pointer_3 = 0x80;
	//Spike Timer/Etc
	std::uintptr_t defuse_percentage = 0x05D0;  //https://dumpspace.spuckwaffel.com/Games/?hash=81c67bd2&type=classes&idx=APlantedBomb_C&member=DefuseProgress
	std::uintptr_t bomb_time_remaining = 0x05A8; ///https://dumpspace.spuckwaffel.com/Games/?hash=81c67bd2&type=classes&idx=APlantedBomb_C&member=	
	std::uintptr_t viewport_world = 0x78;

	std::uintptr_t uworld_pointer = 0x80;
	std::uintptr_t draw_transition = 0x68;
}

