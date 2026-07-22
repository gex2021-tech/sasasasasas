#include "windows.h"
#include <stdint.h>
#include <iostream>
#include <Winternl.h>
#include "canvas_gui.h"
#include "decryption.h"
#include "hook.h"
#include "Decryptor.h"
#include <thread>
#include <chrono>
#include <stdlib.h>
#include "skinchanging.h"
#include <stdio.h>
#include "Hookign/spoofer.h"
#include "options.hpp"
float TPDistance = 500; // Third Person Distance
#include "offsets.h"
#pragma comment(lib, "user32.lib")
#include "json.hpp"
#include <cmath>
using json = nlohmann::json;
float time_ = 0.0f;
std::wstring BuddyNameYAY;
static float X = 610, Y = 390.0f;
APawn* MyPawn;
USkeletalMeshComponent* mesh3p;
AShooterCamera* Ashootercamera;
AShooterCharacter* MyShooter;
UCanvas* canvas;

APlayerState* state;
APlayerController* MyController;
AShooterCharacter* Char;
AShooterCharacter* Actor;
APlayerCameraManager* Cameracache;
USkeletalMeshComponent* Mesh;
APawn* ThirdPersonMesh;
using PostRenderHook = void(*)(uintptr_t _this, UCanvas* canvas, std::uintptr_t a3);
PostRenderHook pRender = 0;
FVector2D head_screen;
UWorld* WORLDDD = 0;
static char chatMessage[256] = "";
static int chatMode = 1; // default to team chat
auto enc = (L"Fresnel Enemy Tint");
auto enc2 = (L"Fresnel Ally Tint");
bool antiaimenabled = false;
bool antiflash = false;
int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);

float ScreenCenterX = Width / 2;
float ScreenCenterY = Height / 2;
class ConfigSystem {
public:
    static void Save(const std::string& file);
    static void Load(const std::string& file);
};
#include "Config.hpp"
void ApplyCustomFOV(UCameraComponent* camera, float fov)
{
    uintptr_t base = (uintptr_t)GetModuleHandleA("Valorant-Win64-Shipping.exe");
    uintptr_t fnAddress = base + 0x62468b0;

    using tSetFOV = void(__fastcall*)(UCameraComponent*, float);
    auto SetFOV = reinterpret_cast<tSetFOV>(fnAddress);

    if (camera)
        SetFOV(camera, fov);
}


bool Dormant(AShooterCharacter* Actor) {

    return Memory::R<bool>(reinterpret_cast<uintptr_t>(Actor) + Offsets::real_time_dormant);
}

bool DormantServer(AShooterCharacter* Actor) {

    return Memory::R<bool>(reinterpret_cast<uintptr_t>(Actor) + 0x100);
}

namespace spin_tool {
    static float Pitch = 50;
    static float Roll = 50;
    static auto OldAimAngles = FVector();
    static float spinX = 20;

    static float spinMULTIX = -2;
    static float spinMULTIY = 10.f;
    static float spinMULTIZ = 10.f;

    static bool one = true;
}
namespace bones {
    FVector2D vHipOut; FVector2D vNeckOut;
    FVector2D vUpperArmLeftOut; FVector2D vLeftHandOut;
    FVector2D vLeftHandOut1; FVector2D vUpperArmRightOut;
    FVector2D vRightHandOut; FVector2D vRightHandOut1;
    FVector2D vLeftThighOut; FVector2D vLeftCalfOut;
    FVector2D vLeftFootOut; FVector2D vRightThighOut;
    FVector2D vRightCalfOut; FVector2D vRightFootOut;
}

static bool one;
static bool firstTime2 = true;
static bool firstTime3 = true;
static FVector OldAimAngles2 = FVector();
float pitch = -90.f;
float yaw = 0.f;
static bool aimbot_key_pressed_last_frame = false;

static bool lock_screen = false;
static FVector OldAimAngles3 = FVector();
static FVector nigga = FVector();
static bool bLockedCameraRotation = false;
constexpr uintptr_t FIELD_OF_VIEW_OFFSET = 0x28C;
constexpr uintptr_t ASPECT_RATIO_OFFSET = 0x29C;
constexpr uintptr_t CONSTRAIN_AR_OFFSET = 0x2A0;
// Scan range in player class (adjust if needed)
constexpr uintptr_t SCAN_START = 0x100;
constexpr uintptr_t SCAN_END = 0x400;
constexpr uintptr_t SCAN_STEP = 0x8;
float LocalCameraFOV;
FVector    LocalCameraLocation;
FVector    LocalCameraRotation;
static float offsetX = 0.0f;
static float offsetY = 0.0f;

namespace AntiAim {
    static float jitter_speed = 15.0f;
    static float micro_jitter_amount = 2.0f;
    static float desync_offset = 0.0f;
    static int tick_counter = 0;
    static float last_real_yaw = 0.0f;
    static float fake_yaw_offset = 0.0f;
    static bool direction_switch = false;
    static float smooth_transition = 0.0f;

    // Variables pour le 3-way jitter
    static int three_way_state = 0;
    static float three_way_angles[3] = { -90.0f, 0.0f, 90.0f };

    // Variables pour le jitter avancé
    static float jitter_base = 0.0f;
    static int jitter_tick = 0;

    // Variables pour les patterns complexes
    static float pattern_time = 0.0f;
    static int pattern_state = 0;
}
void(*SetCameraCachePOVOriginal)(uintptr_t, FMinimalViewInfo*) = nullptr;
auto SetCameraCachePOVHook(uintptr_t PlayerCameraManager, FMinimalViewInfo* ViewInfo)
{
    static bool happybirthday1 = false;

   





    
    if (Globals::Silent)
    {
        SPOOF_FUNC;

        float deltaX, deltaY;
        MyController->GetInputMouseDelta(deltaX, deltaY);

        float sensitivity = MyController->GetMouseSensitivity();
        LocalCameraRotation.X += deltaY * sensitivity;
        LocalCameraRotation.Y += deltaX * sensitivity;

        MyShooter->K2_SetActorRelativeRotationnigger(FRotator{ 0, LocalCameraRotation.Y, 0 }, false, true);

        if (!GetAsyncKeyState(Globals::LegitBotKey))
        {
            if (!GetAsyncKeyState(VK_LBUTTON))
            {
                LocalCameraLocation = ViewInfo->Location;
                LocalCameraRotation = ViewInfo->Rotation;
                LocalCameraFOV = ViewInfo->FOV;

                bLockedCameraRotation = false;
            }
        }
        else
        {
            SPOOF_FUNC;

            if (GetAsyncKeyState(VK_LBUTTON) &&
                in_rect(ScreenCenterX, ScreenCenterY, Globals::FovCircle, head_screen.X, head_screen.Y))
            {
                bLockedCameraRotation = true;
            }

            if (f::bFlickSilent && LocalCameraRotation != FVector())
            {
                ViewInfo->Rotation = LocalCameraRotation;
            }
        }
    }
    if (!Globals::SpinBot) {
        LocalCameraRotation = ViewInfo->Rotation;

    }
    else {
        float deltaX, deltaY;
        MyController->GetInputMouseDelta(deltaX, deltaY);
        float sensitivity = MyController->GetMouseSensitivity();

        //std::cout << "[DEBUG] Mouse input - deltaX: " << deltaX << ", deltaY: " << deltaY << ", sensitivity: " << sensitivity << std::endl;

        LocalCameraRotation.X += deltaY * sensitivity;
        LocalCameraRotation.Y += deltaX * sensitivity;

       

        MyShooter->K2_SetActorRelativeRotationFVector(FVector{ 0, LocalCameraRotation.Y, 0 }, false, false);

        ViewInfo->Rotation = LocalCameraRotation;
       // std::cout << "[DEBUG] ViewInfo rotation updated." << std::endl;
    }

    if (Globals::SpinBot && antiaimenabled && ViewInfo) {
        DWORD tick = GetTickCount64();
        auto Mesh = MyShooter->get_mesh();
        if (!Mesh)
            return;
        //if (!Memory::IsValidPointer(uintptr_t(Mesh)))
        //    return;
        // Check if we should resume spin
        if (Globals::waiting_for_kill && (!Actor || !Actor->IsAlive())) {
            Globals::waiting_for_kill = false;
            Globals::resume_spin_time = GetTickCount() + 15; // wait ~30ms before resuming
        }

        // Resume spin after slight delay
        if (Globals::resume_spin_time && GetTickCount() >= Globals::resume_spin_time) {
            Globals::spin_paused = false;
            Globals::resume_spin_time = 0;
        }
        if (!happybirthday1) {
            happybirthday1 = true;
            LocalCameraRotation = MyController->GetControlRotation();
        }

        if (LocalCameraRotation == FVector()) {
            LocalCameraRotation = MyController->GetControlRotation();
            if (LocalCameraRotation == FVector()) return;
        }

   
            if (!happybirthday1) {
                LocalCameraRotation = MyController->GetControlRotation();
                Globals::cacheoffset = 0.0f;
                happybirthday1 = true;
            }
            if (happybirthday1) {
                // === PITCH MODES ===
                switch (static_cast<int>(Globals::pitchmode)) {
                case 0: Globals::cachepitch = 89.f; break;
                case 1: Globals::cachepitch = -89.f; break;
                case 2: Globals::cachepitch = 0.f; break;
                case 3: Globals::cachepitch = ((tick / 250) % 2 == 0) ? 89.f : -89.f; break;
                case 4: Globals::cachepitch = sinf(tick * 0.004f) * 89.f; break;
                case 5: Globals::cachepitch = rand() % 178 - 89.f; break;
                case 6: Globals::cachepitch = ((tick / 200) % 3 == 0) ? 89.f : -30.f; break;
                case 7: Globals::cachepitch = cosf(tick * 0.002f) * 89.f; break;
                case 8: Globals::cachepitch = ((tick / 100) % 4 == 0) ? -89.f : 0.f; break;
                case 9: Globals::cachepitch = ((tick / 80) % 2) ? -45.f : 89.f; break;
                case 10: Globals::cachepitch = sinf(tick * 0.005f) * 89.f; break;
                case 11: Globals::cachepitch = (rand() % 2) ? 89.f : -89.f; break;
                case 12: Globals::cachepitch = ((tick / 20) % 2 == 0) ? -89.f : 89.f; break;
                case 13: Globals::cachepitch = sinf(tick * 0.015f) * 45.f; break;
                case 14: Globals::cachepitch = ((tick / 60) % 3 == 0) ? 88.f : -88.f; break;
                default: Globals::cachepitch = 89.f; break;
                }

                // === YAW MODES ===
                switch (static_cast<int>(Globals::baseyaw)) {
                case 0: Globals::cachebaseyaw = LocalCameraRotation.Y; break;
                case 1: Globals::cachebaseyaw = fmod(tick * 0.3f, 360.f); break;
                case 2: Globals::cachebaseyaw = fmod(-tick * 0.3f, 360.f); break;
                case 3: Globals::cachebaseyaw = sinf(tick * 0.003f) * 360.f; break;
                case 4: Globals::cachebaseyaw = rand() % 360 - 180.f; break;
                case 5: Globals::cachebaseyaw = ((tick / 100) % 2 == 0) ? 180.f : -180.f; break;
                case 6: Globals::cachebaseyaw = ((tick / 120) % 3) * 90.f - 90.f; break;
                case 7: Globals::cachebaseyaw = cosf(tick * 0.004f) * 180.f; break;
                case 8: Globals::cachebaseyaw = ((tick / 10) % 2) ? 270.f : 90.f; break;
                case 9: Globals::cachebaseyaw = ((tick / 25) % 2) ? 0.f : 180.f; break;
                case 10: Globals::cachebaseyaw = sinf(tick * 0.02f) * 180.f; break;
                case 11: Globals::cachebaseyaw = (rand() % 2) ? 179.9f : -179.9f; break;
                case 12: Globals::cachebaseyaw = ((tick / 33) % 4) * 90.f; break;
                case 13: Globals::cachebaseyaw = rand() % 100 > 50 ? 0.f : 360.f; break;
                case 14: Globals::cachebaseyaw = ((tick / 15) % 2) ? -90.f : 270.f; break;
                default: Globals::cachebaseyaw = 180.f; break;
                }

                // === OFFSET / DESYNC MODES ===
                switch (static_cast<int>(Globals::antiaimtype)) {
                case 0: Globals::cacheoffset = 0.f; break;
                case 1: Globals::cacheoffset = (tick / 60 % 2 ? 1 : -1) * Globals::speed * 1.8f; break;
                case 2: Globals::cacheoffset = fmod(tick * 0.2f, 360.f); break;
                case 3: Globals::cacheoffset = rand() % 180 - 90.f; break;
                case 4: Globals::cacheoffset = ((tick / 30) % 2) ? 180.f : -180.f; break;
                case 5: Globals::cacheoffset = sinf(tick * 0.01f) * 135.f; break;
                case 6: Globals::cacheoffset = ((tick / 25) % 4 - 2) * Globals::speed; break;
                case 7: Globals::cacheoffset = (tick % 100 < 50) ? 120.f : -60.f; break;
                case 8: Globals::cacheoffset = ((tick / 3) % 2) ? 45.f : -135.f; break;
                case 9: Globals::cacheoffset = (tick % 2) ? 90.f : -90.f; break;
                case 10: Globals::cacheoffset = sinf(tick * 0.02f) * 160.f; break;
                case 11: Globals::cacheoffset = (tick % 500 < 250) ? 0.f : 179.f; break;
                case 12: Globals::cacheoffset = ((tick / 100) % 2) ? -179.f : 179.f; break;
                case 13: Globals::cacheoffset = (rand() % 2) ? 30.f : -30.f; break;
                case 14: Globals::cacheoffset = cosf(tick * 0.004f) * 180.f; break;
                default: Globals::cacheoffset = 0.f; break;
                }

                // Oscillation (if enabled)
                if (Globals::oscillate_enabled) {
                    static float osc_time = 0.f;
                    osc_time += Globals::oscillation_speed * Globals::speed;
                    Globals::cacheoffset = sinf(osc_time) * Globals::oscillation_range;
                }

                // Final Yaw Calculation
                float current_yaw = LocalCameraRotation.Y;
                float new_yaw = current_yaw + Globals::cachebaseyaw;
                if (new_yaw > 360.f) new_yaw -= 360.f;
                else if (new_yaw < 0.f) new_yaw += 360.f;

                float final_yaw = new_yaw - Globals::cacheoffset;
                if (Globals::manualoverride)
                {
                    // === Manual Direction Override ===
                    if (GetAsyncKeyState(0x45) & 0x8000) // Mouse 1 → Left
                        final_yaw -= 90.f;
                    else if (GetAsyncKeyState(0x51) & 0x8000) // Mouse 2 → Right
                        final_yaw += 90.f;
                    else if (GetAsyncKeyState(0x46) & 0x8000) // 'F' Key → Backwards
                        final_yaw += 180.f;
                }

                // Normalize
                final_yaw = fmod(final_yaw + 360.f, 360.f);


                MyController->SetControlRotation(FVector(Globals::cachepitch, final_yaw, 0.f));

                float mesh_yaw = final_yaw;
                if (Globals::desync_enabled)
                    mesh_yaw += Globals::desync_amount;

                if (Globals::spin_paused) {
                    Mesh->set_world_rotation(FVector(1, LocalCameraRotation.Y, 1.f), 0, 0); // paused aim
                }
                else {
                    Mesh->set_world_rotation(FVector(1, mesh_yaw, 1.f), 0, 0); // normal spin
                }

            }
        
    }
    else {
        happybirthday1 = false;
        Globals::spin_paused = false;
        Globals::waiting_for_kill = false;
        Globals::resume_spin_time = 0;

    }



    FVector CameraPos = ViewInfo->Location;
    FVector CameraRot = ViewInfo->Rotation;

    if (Globals::Thirdperson) {
        float TPDistance = Globals::third_person_float;
        float FixZAngle = (-TPDistance - (sin(DegreeToRadian(-CameraRot.X)) * (TPDistance - (TPDistance / 3.5))));

        if (CameraRot.X < 0.0f)
            FixZAngle = -(TPDistance - (sin(DegreeToRadian(-CameraRot.X)) * (TPDistance - (TPDistance / 3.5))));

        FVector CamOff = FVector{ cos(DegreeToRadian(CameraRot.Y)) * FixZAngle, sin(DegreeToRadian(CameraRot.Y)) * FixZAngle, sin(DegreeToRadian(-CameraRot.X)) * TPDistance };
        ViewInfo->Location = CameraPos + CamOff;
    }

    if (Globals::bEnableAspectRatioChanger)
    {
        if (ViewInfo)
        {
            ViewInfo->bConstrainAspectRatio = true;
            ViewInfo->AspectRatio = Globals::DesiredAspectRatio;

        }

    }


    SetCameraCachePOVOriginal(PlayerCameraManager, ViewInfo);
}

void disable_input_keys(APlayerController* MyController)
{

    /* KeyEvent(0x41, false);
     KeyEvent(0x44, false);
     KeyEvent(0x57, false);
     KeyEvent(0x53, false);
     KeyEvent(0x20, false);*/

    MyController->simulate_input_key(keys::w, false);
    MyController->simulate_input_key(keys::A, false);
    MyController->simulate_input_key(keys::s, false);
    MyController->simulate_input_key(keys::d, false);
    MyController->simulate_input_key(keys::space, false);
}
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdlib>

class CConfig
{
protected:
    class C_ConfigItem
    {
    public:
        std::string name;
        void* pointer;
        std::string type;

        C_ConfigItem(std::string name, void* pointer, std::string type)
        {
            this->name = name;
            this->pointer = pointer;
            this->type = type;
        }
    };
    std::vector <C_ConfigItem*> items;

private:
    void AddItem(void* pointer, const char* name, const std::string& type);
    void setup_item(int*, int, const std::string&);
    void setup_item(bool*, bool, const std::string&);
    void setup_item(float*, float, const std::string&);
    void setup_item(std::vector< int >*, int, const std::string&);
    void setup_item(std::vector< std::string >*, const std::string&);
    void setup_item(std::string*, const std::string&, const std::string&);
    void SetupCustomShit();
    void SetupAimbot();
    void SetupVisuals();
    void SetupMisc();
    void SetupColors();
    void SetupSkinChanger();
    void SetupChams();
    void SetupBuddy();
    void SetupOutlines();
public:
    void Initialize();
    void LoadSettings(const std::string& szIniFile);
    void SaveSettings(const std::string& szIniFile);

    std::string folder;
};

extern CConfig* Config;
#include <ShlObj.h>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <fstream>



using namespace std;

void CConfig::AddItem(void* pointer, const char* name, const std::string& type) {
    items.push_back(new C_ConfigItem(std::string(name), pointer, type));
}

void CConfig::setup_item(int* pointer, int value, const std::string& name)
{
    AddItem(pointer, name.c_str(), ("int"));
    *pointer = value;
}

void CConfig::setup_item(bool* pointer, bool value, const std::string& name)
{
    AddItem(pointer, name.c_str(), ("bool"));
    *pointer = value;
}

void CConfig::setup_item(float* pointer, float value, const std::string& name)
{
    AddItem(pointer, name.c_str(), ("float"));
    *pointer = value;
}

void CConfig::setup_item(std::vector< int >* pointer, int size, const std::string& name)
{
    AddItem(pointer, name.c_str(), ("vector<int>"));
    pointer->clear();
    for (int i = 0; i < size; i++)
        pointer->push_back(FALSE);
}

void CConfig::setup_item(std::vector< std::string >* pointer, const std::string& name)
{
    AddItem(pointer, name.c_str(), ("vector<string>"));
}

void CConfig::setup_item(std::string* pointer, const std::string& value, const std::string& name)
{
    AddItem(pointer, name.c_str(), ("string"));
    *pointer = value;
}

void CConfig::SetupAimbot() {
    setup_item(&Globals::EnableAim, false, "aimbot_enabled");
    setup_item(&Globals::VisibleCheck, false, "aimbot_vischeck");
    setup_item(&Globals::RecoilControl, false, "aimbot_recoil");
    setup_item(&Globals::DrawFov, false, "aimbot_draw_fov");
    setup_item(&Globals::smooth, 15.0f, "aimbot_smooth");
    setup_item(&Globals::LegitFOvValue, 70.0f, "aimbot_fov");
    setup_item(&Globals::AimBone, 0, "aimbot_bone");
    setup_item(&Globals::LegitBotKey, 0, "aimbot_key");
    setup_item(&Globals::Silentaim, 0, "aimbot_silent_key");
    setup_item(&Globals::Silent, false, "aimbot_silent");
    setup_item(&Globals::AutoShoot, false, "aimbot_autoshoot");
    setup_item(&Globals::AutoshootFloat, 1.0f, "aimbot_autoshoot_delay");
    setup_item(&Globals::CoolFov, false, "aimbot_rgb_fov");
    setup_item(&Globals::RecoilCrosshair, false, "aimbot_recoil_crosshair");
    setup_item(&Globals::bullettracers, false, "aimbot_bullet_tracers");
    setup_item(&Globals::NoSpreadAimbot, false, "aimbot_nospread");
    setup_item(&Globals::targetline, false, "aimbot_targetline");
    setup_item(&Globals::auto_wall, false, "aimbot_autowall");
}


void CConfig::SetupVisuals() {
    setup_item(&Globals::Box2D, false, "visuals_box2d");
    setup_item(&Globals::Box3D, false, "visuals_box3d");
    setup_item(&Globals::CorneredBox, false, "visuals_corner");
    setup_item(&Globals::HeadESP, false, "visuals_headbox");
    setup_item(&Globals::SkeletonEsp, false, "visuals_skeleton");
    setup_item(&Globals::Snapline, false, "visuals_snapline");
    setup_item(&Globals::Healthbar, false, "visuals_healthbar");
    setup_item(&Globals::dropped_gun, false, "visuals_booms");
    setup_item(&Globals::WeaponEsp, false, "visuals_weapons");
    setup_item(&Globals::DistangeEsp, false, "visuals_distance");
    setup_item(&Globals::SpikeEsp, false, "visuals_spike");
    setup_item(&Globals::skill_esp, false, "visuals_skillname");
    setup_item(&Globals::HealthText, false, "visuals_healthtext");
    setup_item(&Globals::AgentIcon, false, "visuals_agenticon");
    setup_item(&Globals::AgentName, false, "visuals_agentname");
    setup_item(&Globals::riotid, false, "visuals_riotid");
    setup_item(&Globals::SelectSnapline, 0, "visuals_snapline_pos");
    setup_item(&Globals::Minimap, false, "visuals_radar");
    setup_item(&Globals::ClientSideSpinbot, false, "visuals_playerspin");
    setup_item(&Globals::ShowPing, false, "visuals_ping");
    setup_item(&Globals::party_hat, false, "visuals_hat");
    setup_item(&Globals::boxoutline, false, "visuals_box_outline");
}


void CConfig::SetupMisc() {
    setup_item(&Globals::BunnyHop, false, "misc_bhop");
    setup_item(&Globals::AntiAFK, false, "misc_antiafk");
    setup_item(&Globals::crosshairrainbow, false, "misc_crosshair_rgb");
    setup_item(&Globals::SkipTutorial, false, "misc_skip_tutorial");
    setup_item(&Globals::MenuKey, 0, "misc_menu_key");
    setup_item(&Globals::Watermark, true, "watermark");
    setup_item(&Globals::WatermarkType, 0, "watermark_type");
    setup_item(&Globals::TeamCheck, false, "misc_teamcheck");
    setup_item(&Globals::chatspam, false, "misc_chatspam");
}


void CConfig::SetupCustomShit() {
    setup_item(&Globals::killsay, false, "custom_killsay");
    setup_item(&Globals::chatspam, false, "custom_chatspam");
    setup_item(&Globals::galaxychams, false, "custom_galaxychams");
    //setup_item(&Globals::killsay_input, L"antivgc lol", "custom_killsay_input");
    setup_item(&Globals::customgun, false, "custom_customgun");
    //setup_item(&Globals::Horizon, FLinearColor(1, 1, 1, 1), "custom_sky_horizon");
   // setup_item(&Globals::Overall, FLinearColor(1, 1, 1, 1), "custom_sky_overall");
    //setup_item(&Globals::Zenith, FLinearColor(1, 1, 1, 1), "custom_sky_zenith");
   // setup_item(&Globals::Cloud, FLinearColor(1, 1, 1, 1), "custom_sky_cloud");
    setup_item(&Globals::CloudSpeed, 1.0f, "custom_sky_cloud_speed");
    setup_item(&Globals::StarsBrightness, 1.0f, "custom_sky_stars_brightness");
    setup_item(&Globals::CloudOpacity, 1.0f, "custom_sky_cloud_opacity");
}


void CConfig::SetupChams() {
    setup_item(&Globals::ChamsESP, false, "chams_enabled");
    setup_item(&Globals::rainbow_chams, false, "chams_rainbow");
    setup_item(&Globals::glowenemyfloat, 1.0f, "chams_glow");
    setup_item(&Globals::custom_fresnel, false, "chams_custom_fresnel");
    setup_item(&Globals::FadeMaterialTest, false, "chams_fade");
    setup_item(&Globals::SovaMaterialTest, false, "chams_sova");
    setup_item(&Globals::crazychams, false, "chams_cool");
    setup_item(&Globals::outline, false, "chams_outline");
    setup_item(&Globals::WireFrameHand, false, "chams_wireframe_hand");
    setup_item(&Globals::WireFrameWeapon, false, "chams_wireframe_weapon");
    setup_item(&Globals::HandGlow, false, "chams_hand_material");
    setup_item(&Globals::handchams, false, "chams_handchams");
    setup_item(&Globals::handChamsRGB, false, "chams_handchams_rgb");
    setup_item(&Globals::gunmaterial, false, "chams_gun_material");
    setup_item(&Globals::Glow1, 1.0f, "chams_fresnel_brightness");
    setup_item(&Globals::OwnWireframe, false, "chams_wireframe_self");
    setup_item(&Globals::gunChamsRGB, false, "chams_gunchams_rgb");
   // setup_item(&Globals::guncolor, FLinearColor(1, 1, 1), "chams_gun_color");
   // setup_item(&Globals::handcolor, FLinearColor(1, 1, 1), "chams_hand_color");
    setup_item(&Globals::self_fresnel, false, "chams_self_fresnel");
    setup_item(&Globals::hand_fresnel, false, "chams_hand_fresnel");
    //setup_item(&Globals::fresnel, FLinearColor(1, 1, 1), "chams_fresnel_color");
    setup_item(&Globals::rainbow_fresnel, false, "chams_rainbow_fresnel");
    setup_item(&Globals::intesity, 1.0f, "chams_intensity");
    setup_item(&Globals::galaxychams2, false, "chams_galaxy_self");
    setup_item(&Globals::galaxychams3, false, "chams_galaxy_enemy");
}

void CConfig::SetupOutlines() {
    setup_item(&Globals::outline2, false, "outline_enable");
    setup_item(&Globals::GlowVisible, 1.0f, "outline_visible_intensity");
    setup_item(&Globals::GlowInvisible, 1.0f, "outline_invisible_intensity");
    setup_item(&Globals::outlinetype, 0, "outline_type");
    setup_item(&Globals::outlineRainbow, false, "outline_rainbow");
    setup_item(&Globals::usepresetedoutlines, false, "outline_use_presets");
    setup_item(&Globals::visiblepreset, 0, "outline_visible_preset");
    setup_item(&Globals::invisiblepreset, 0, "outline_invisible_preset");
    setup_item(&Globals::intesityinner, 1.0f, "outline_intensity_inner");
    setup_item(&Globals::intesityedger, 1.0f, "outline_intensity_edge");
    setup_item(&Globals::intesityoutter, 1.0f, "outline_intensity_outer");

    // Visible colors
    setup_item(&Globals::CenterEdgeR_Visible, 1.0f, "outline_center_r_visible");
    setup_item(&Globals::CenterEdgeG_Visible, 1.0f, "outline_center_g_visible");
    setup_item(&Globals::CenterEdgeB_Visible, 1.0f, "outline_center_b_visible");
    setup_item(&Globals::InnerEdgeR_Visible, 1.0f, "outline_inner_r_visible");
    setup_item(&Globals::InnerEdgeG_Visible, 1.0f, "outline_inner_g_visible");
    setup_item(&Globals::InnerEdgeB_Visible, 1.0f, "outline_inner_b_visible");
    setup_item(&Globals::OuterEdgeR_Visible, 1.0f, "outline_outer_r_visible");
    setup_item(&Globals::OuterEdgeG_Visible, 1.0f, "outline_outer_g_visible");
    setup_item(&Globals::OuterEdgeB_Visible, 1.0f, "outline_outer_b_visible");

    // Invisible colors
    setup_item(&Globals::CenterEdgeR_Invisible, 1.0f, "outline_center_r_invisible");
    setup_item(&Globals::CenterEdgeG_Invisible, 1.0f, "outline_center_g_invisible");
    setup_item(&Globals::CenterEdgeB_Invisible, 1.0f, "outline_center_b_invisible");
    setup_item(&Globals::InnerEdgeR_Invisible, 1.0f, "outline_inner_r_invisible");
    setup_item(&Globals::InnerEdgeG_Invisible, 1.0f, "outline_inner_g_invisible");
    setup_item(&Globals::InnerEdgeB_Invisible, 1.0f, "outline_inner_b_invisible");
    setup_item(&Globals::OuterEdgeR_Invisible, 1.0f, "outline_outer_r_invisible");
    setup_item(&Globals::OuterEdgeG_Invisible, 1.0f, "outline_outer_g_invisible");
    setup_item(&Globals::OuterEdgeB_Invisible, 1.0f, "outline_outer_b_invisible");
}


void CConfig::SetupBuddy() {
    setup_item(&Globals::UnlockAll, false, "unlock_all");
    setup_item(&Globals::BuddyChanger, false, "buddy_enabled");
    setup_item(&index, 0, "buddy_index");
    setup_item(&Globals::Buddytext, false, "buddy_text");
    setup_item(&Globals::autoapply, false, "buddy_autoapply"); // <== Added
}


void CConfig::SetupColors() {
    setup_item(&Globals::ViewModelChanger, false, "exploit_viewmodel");
    setup_item(&Globals::bEnableAspectRatioChanger, false, "exploit_aspect_ratio");
    setup_item(&Globals::Thirdperson, false, "exploit_thirdperson");
    setup_item(&Globals::FovChangerBool, false, "exploit_fovchanger");
    setup_item(&Globals::SpinBot, false, "exploit_spinbot");
    setup_item(&Globals::championssheriff, false, "exploit_champions_sheriff");
    setup_item(&Globals::big_gun, false, "exploit_bigweapon_1p");
    setup_item(&Globals::bigweapon, false, "exploit_bigweapon_3p");
    setup_item(&Globals::FastCrouch, false, "exploit_fastcrouch");
    setup_item(&Globals::biggunvalue, 0.0f, "exploit_biggun_size");
    setup_item(&Globals::FovChangerValue, 0.0f, "exploit_fov_slider");
    setup_item(&Globals::spinbot_speed, 0.0f, "exploit_spinbot_speed");
    setup_item(&Globals::third_person_float, 0.0f, "exploit_thirdperson_dist");
    setup_item(&Globals::ScaleX, 1.0f, "exploit_viewmodel_x");
    setup_item(&Globals::ScaleY, 1.0f, "exploit_viewmodel_y");
    setup_item(&Globals::ScaleZ, 1.0f, "exploit_viewmodel_z");
    setup_item(&Globals::WScaleX, 1.0f, "exploit_weapon_x");
    setup_item(&Globals::WScaleY, 1.0f, "exploit_weapon_y");
    setup_item(&Globals::WScaleZ, 1.0f, "exploit_weapon_z");
    setup_item(&Globals::DesiredAspectRatio, 1.0f, "exploit_aspect_ratio_value");

    // Added from Exploits & CustomShit
    setup_item(&Globals::ragemode, false, "exploit_ragemode");
    setup_item(&Globals::nightmode, false, "exploit_nightmode");
    setup_item(&Globals::tpkey, 0, "exploit_tpkey");

    setup_item(&Globals::finisher, false, "custom_finishers");
    setup_item(&Globals::bOnlyLastKill, false, "custom_only_last_kill");
    setup_item(&Globals::customshit, false, "custom_gun_model");
    setup_item(&Globals::skybox, false, "custom_skybox");
    setup_item(&Globals::killsounds, false, "custom_killsounds");
    setup_item(&Globals::skyboxmat, false, "custom_skybox_mat");
    setup_item(&Globals::skyboxRGB, false, "custom_skybox_rgb");
    /* setup_item(&Globals::Horizon, ImColor(255, 255, 255), "custom_horizon_color");
     setup_item(&Globals::Zenith, ImColor(255, 255, 255), "custom_zenith_color");
     setup_item(&Globals::Overall, ImColor(255, 255, 255), "custom_overall_color");
     */
}


void CConfig::SetupSkinChanger() {
    setup_item(&vandalSkinOption, 0, "skin_vandal");
    setup_item(&phantomSkinOption, 0, "skin_phantom");
    setup_item(&operatorSkinOption, 0, "skin_operator");
    setup_item(&bulldogSkinOption, 0, "skin_bulldog");
    setup_item(&frenzySkinOption, 0, "skin_frenzy");
    setup_item(&guardianSkinOption, 0, "skin_guardian");
    setup_item(&sheriffSkinOption, 0, "skin_sheriff");
    setup_item(&ghostSkinOption, 0, "skin_ghost");
    setup_item(&spectreSkinOption, 0, "skin_spectre");
    setup_item(&classicSkinOption, 0, "skin_classic");
    setup_item(&marshalSkinOption, 0, "skin_marshal");
    setup_item(&knifeSkinOption, 0, "skin_knife");
}




// Place this in your CConfig::Initialize():
void CConfig::Initialize() {
   /* folder = "C:\\Internal\\";
    CreateDirectoryA(folder.c_str(), nullptr);*/
    SetupSkinChanger();
    SetupAimbot();
    SetupVisuals();
    SetupMisc();
    SetupChams();
    SetupBuddy();
    SetupColors();
    SetupCustomShit();
    SetupOutlines(); // <--- add this
}


// Dans config.cpp - Nouvelle fonction SaveSettings pour JSON
void CConfig::SaveSettings(const std::string& szIniFile) {
    std::ofstream file(szIniFile);

    if (!file.is_open()) {
        // Debug : vérifier si le fichier peut être ouvert
        MessageBoxA(NULL, ("Cannot open file: " + szIniFile).c_str(), "Error", MB_OK);
        return;
    }

    // Format JSON
    file << "{\n";

    bool first = true;
    for (auto item : items) {
        if (!first) file << ",\n";
        first = false;

        file << "  \"" << item->name << "\": ";

        if (item->type == "bool") {
            file << (*(bool*)item->pointer ? "true" : "false");
        }
        else if (item->type == "int") {
            file << *(int*)item->pointer;
        }
        else if (item->type == "float") {
            file << *(float*)item->pointer;
        }
        else if (item->type == "string") {
            file << "\"" << *(std::string*)item->pointer << "\"";
        }
    }

    file << "\n}";
    file.close();


}

// Nouvelle fonction LoadSettings pour JSON
void CConfig::LoadSettings(const std::string& szIniFile) {
    std::ifstream file(szIniFile);

    if (!file.is_open()) {
        MessageBoxA(NULL, ("Cannot open file: " + szIniFile).c_str(), "Error", MB_OK);
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();

    // Parse JSON simple (sans bibliothèque)
    for (auto item : items) {
        std::string search = "\"" + item->name + "\":";
        size_t pos = content.find(search);

        if (pos != std::string::npos) {
            pos += search.length();

            // Ignorer les espaces
            while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t')) pos++;

            if (item->type == "bool") {
                if (content.substr(pos, 4) == "true") {
                    *(bool*)item->pointer = true;
                }
                else if (content.substr(pos, 5) == "false") {
                    *(bool*)item->pointer = false;
                }
            }
            else if (item->type == "int") {
                std::string value;
                while (pos < content.length() && isdigit(content[pos])) {
                    value += content[pos++];
                }
                if (!value.empty()) {
                    *(int*)item->pointer = std::stoi(value);
                }
            }
            else if (item->type == "float") {
                std::string value;
                while (pos < content.length() && (isdigit(content[pos]) || content[pos] == '.')) {
                    value += content[pos++];
                }
                if (!value.empty()) {
                    *(float*)item->pointer = std::stof(value);
                }
            }
        }
    }


}

CConfig* Config = new CConfig();
FLinearColor rainbow = {

    (sinf(time_ * 2.0f) + 1.0f) * 0.5f,
    (sinf(time_ * 2.0f + 2.0f) + 1.0f) * 0.5f,
    (sinf(time_ * 2.0f + 4.0f) + 1.0f) * 0.5f,
    1.0f
};

static bool showVisibleColors = false;
static bool showInvisibleColors = false;
static bool showVisibleCenter = false;
static bool showVisibleInner = false;
static bool showVisibleOuter = false;
static bool showInvisibleCenter = false;
static bool showInvisibleInner = false;
static bool showInvisibleOuter = false;

void A1M() {
    // --- Constants ---
    const float sidebarWidth = 110;
    const float headerHeight = 48;       // adjust if needed (height of your top ANTI VGC bar)
    const float contentGap = 10;
    const float panelWidth = 260;
    const float panelHeight = 440;
    const float panelGap = 20;
    const float panelPad = 15;
    const float panelHeaderH = 28;
    const float controlStartGap = 8;

    // --- Panel positions ---
    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT SUBTAB ("Main") ---
    menu::BeginSubTab(L"Main", FVector2D(leftPanelX, leftPanelY), FVector2D(panelWidth, panelHeight));
    menu::offset_x = leftPanelX + panelPad;
    menu::offset_y = leftPanelY + panelHeaderH + controlStartGap;

    // ---- Use your custom checkbox here ----
    static bool enableAimbot = false;
    static bool silentAimbot = false;
    static bool recoilControl = false;
    static bool vishCheck = false;

    menu::Checkbox(ez(L"Enable aimbot").d(), &Globals::EnableAim);
    menu::Checkbox(ez(L"Enable Silent Aimbot").d(), &Globals::Silent);
    menu::Checkbox(ez(L"Enable AutoShoot").d(), &Globals::AutoShoot);
    menu::Checkbox(ez(L"Enable AutoWall - WIP").d(), &Globals::auto_wall);
    menu::Checkbox(ez(L"Enable Prediction").d(), &Globals::EnablePrediction);
    menu::Checkbox(ez(L"Recoil control").d(), &Globals::RecoilControl);
   // menu::Checkbox(ez(L"Accurate Vischeck").d(), &Globals::AccurateVisibleCheck);
    menu::Checkbox(ez(L"Vischeck").d(), &Globals::VisibleCheck);
    menu::SliderFloat(ez(L"FOV Size").d(), &Globals::LegitFOvValue, 1, 1500);
    menu::SliderFloat(ez(L"Smooth").d(), &Globals::smooth, 1, 40);
    menu::SliderFloat(ez(L"Shoot Delay").d(), &Globals::AutoshootFloat, 1, 20);
    menu::SliderFloat(ez(L"PredictionPeek MS").d(), &Globals::PredictionPeekMS, 20, 200);
    menu::offset_y += 10;
    menu::Combobox(ez(L"Bone").d(), { 95, 27 }, &Globals::AimBone, L"Head", L"Chest", L"Leg", NULL);


    // --- RIGHT SUBTAB ("Other") ---
    menu::BeginSubTab(L"Other", FVector2D(rightPanelX, rightPanelY), FVector2D(panelWidth, panelHeight));
    menu::offset_x = rightPanelX + panelPad;
    menu::offset_y = rightPanelY + panelHeaderH + controlStartGap;
    menu::Checkbox(ez(L"Recoil crosshair").d(), &Globals::RecoilCrosshair);
    menu::Checkbox(ez(L"Draw fov").d(), &Globals::DrawFov);
    menu::Checkbox(ez(L"Draw RGB fov").d(), &Globals::CoolFov);
    menu::Checkbox(ez(L"Bullet Tracers").d(), &Globals::bullettracers);
    menu::Checkbox(ez(L"Target Line").d(), &Globals::targetline);
   menu::Checkbox(ez(L"No Spread").d(), &Globals::NoSpreadAimbot);
    menu::offset_y += 10;
  
    menu::Hotkey("Aim Key", FVector2D(83, 28.0f), &Globals::LegitBotKey);
    // TODO: Add more controls as needed, using same offset pattern
}




void E3P() {
    // --- Layout Constants ---
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT PANEL ("Main") ---
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;

    menu::Checkbox(ez(L"D1stange").d(), &Globals::DistangeEsp);
    menu::Checkbox(ez(L"Snapl1ne").d(), &Globals::Snapline);
    menu::Checkbox(ez(L"2D b0x").d(), &Globals::Box2D);
   menu::Checkbox(ez(L"2D BOX Outlines").d(), &Globals::boxoutline);
    menu::Checkbox(ez(L"3D b0x").d(), &Globals::Box3D);
    menu::Checkbox(ez(L"C0rner box").d(), &Globals::CorneredBox);
    menu::Checkbox(ez(L"Dr0pped gun").d(), &Globals::dropped_gun);
    menu::Checkbox(ez(L"Ske1eton").d(), &Globals::SkeletonEsp);
    if (Globals::SkeletonEsp)
    {
        menu::Checkbox(ez(L"Head Esp").d(), &Globals::HeadESP);
    }
    menu::Checkbox(ez(L"Hea1thbar").d(), &Globals::Healthbar);
    menu::Checkbox(ez(L"Hea1thbar T4xt").d(), &Globals::HealthText);
    menu::Checkbox(ez(L"Tr4ps & sk1ll n4me").d(), &Globals::skill_esp);
    menu::Checkbox(ez(L"Sp1ke inf0").d(), &Globals::SpikeEsp);
    menu::Checkbox(ez(L"Weap0n n4me").d(), &Globals::WeaponEsp);
    menu::Checkbox(ez(L"Ag3nt Ic0n").d(), &Globals::AgentIcon); 
    menu::Checkbox(ez(L"Ag3nt n4me").d(), &Globals::AgentName);
    menu::Checkbox(ez(L"Chinese Hat").d(), &Globals::party_hat);
    menu::Checkbox(ez(L"Radar").d(), &Globals::Minimap);
    menu::Checkbox(ez(L"Player Info").d(), &Globals::ClientSideSpinbot);
    if (Globals::ClientSideSpinbot)
    {
        Globals::ShowPlayerName = true;
        menu::Checkbox(ez(L"ShowPing").d(), &Globals::ShowPing);
    }
    else
    {
        Globals::ShowPlayerName = false;
    }

    // --- RIGHT PANEL ("Others") ---
    menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Others", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
  
    menu::ColorPicker(ez(L"Fov color").d(), &Colors::FovColor);
    menu::ColorPicker(ez(L"Box visible color").d(), &Colors::BoxVisible);
    menu::ColorPicker(ez(L"Line visible color").d(), &Colors::LineVisible);
    menu::ColorPicker(ez(L"Skeleton visible color").d(), &Colors::SkeletonVisible);
    menu::ColorPicker(ez(L"Weapon color").d(), &Colors::WeaponNameColor);
    menu::ColorPicker(ez(L"Agent color").d(), &Colors::AgentNameColor);
    menu::ColorPicker(ez(L"Box invisible color").d(), &Colors::Boxinvisible);
    menu::ColorPicker(ez(L"Line invisible color").d(), &Colors::LineINvisible);
    menu::ColorPicker(ez(L"Skeleton invisible color").d(), &Colors::SkeletonInvisible);
    menu::Combobox((L""), { 95, 27 }, &Globals::SelectSnapline, L"bottom", L"middle", L"top", NULL);
    menu::offset_y += 15;
    menu::SliderFloat(L"Radar Range", &Globals::ScaleX, 1.f, 2000.f);
   
}




void OUTLINES() {
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // === Main (Left Panel) ===
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;

    menu::Checkbox(L"Outline Chams", &Globals::outline2);
    menu::Checkbox(L"Rainbow Outlines", &Globals::outlineRainbow);
    menu::Checkbox(L"Use Preseted Outlines", &Globals::usepresetedoutlines);
    menu::offset_y += 28.f;
    menu::Combobox(L"Outline Type", { 95, 27 }, &Globals::outlinetype, L"Always Outlined", L"Only Behind", NULL);
    if (Globals::usepresetedoutlines)
    {
        menu::Combobox(L"Visible Preset", { 95, 27 }, &Globals::visiblepreset,
            L"Default Green",
            L"Toxic Glow",
            L"Neon Ice",
            L"Solar Flare",
            L"Void Spark",
            L"Ghost White",
            L"Aurora Dream",
            L"Cyber Ember",
            L"Lunar Fade",
            L"Mystic Flame",
            L"Arc Light",
            L"Burning Core",
            L"Deep Ocean",
            L"Crystal Edge",
            L"Magma Burn",
            L"Galaxy",
            NULL);

        menu::Combobox(L"InVisible Preset", { 95, 27 }, &Globals::invisiblepreset,
            L"Default Red",
            L"Inferno",
            L"Phantom Pulse",
            L"Voltage Spike",
            L"Frostbite",
            L"Undead Mist",
            L"Blood Moon",
            L"Spectral Veil",
            L"Grave Ember",
            L"Arc Phantom",
            L"Wraithfire",
            L"Pale Storm",
            L"Dark Volt",
            L"Ether Smoke",
            L"Poison Chill",
            L"Galaxy",
            NULL);
    }
    menu::SliderFloat(L"Intesity Inner", &Globals::intesityinner, 1.f, 60);
   menu::SliderFloat(L"Intesity Edge", &Globals::intesityedger, 1.f, 60);
   menu::SliderFloat(L"Intesity Outter", &Globals::intesityoutter, 1.f, 60);

    // === Color/Misc (Right Panel) ===
    menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Color/Misc", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;

    menu::Checkbox(L"Show Visible Colors", &showVisibleColors);
    if (showVisibleColors) {
        menu::Checkbox(L" Center Edgevis", &showVisibleCenter);
        if (showVisibleCenter) {
            menu::offset_y += 10;
            menu::SliderFloat(L"RedCenterVis", &Globals::CenterEdgeR_Visible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"GreenCenterVis", &Globals::CenterEdgeG_Visible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"BlueCenterVis", &Globals::CenterEdgeB_Visible, 0.f, 1.f);

        }
        menu::Checkbox(L" Inner Edgevis", &showVisibleInner);
        if (showVisibleInner) {
            menu::offset_y += 10;
            menu::SliderFloat(L"RedInnerVis", &Globals::InnerEdgeR_Visible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"GreenInnerVis", &Globals::InnerEdgeG_Visible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"BlueInnerVis", &Globals::InnerEdgeB_Visible, 0.f, 1.f);

        }
        menu::Checkbox(L" Outer Edgevis", &showVisibleOuter);
        if (showVisibleOuter) {
            menu::offset_y += 10;
            menu::SliderFloat(L"RedOuterVis", &Globals::OuterEdgeR_Visible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"GreenOuterVis", &Globals::OuterEdgeG_Visible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"BlueOuterVis", &Globals::OuterEdgeB_Visible, 0.f, 1.f);

        }
    }

    menu::Checkbox(L"Show Invisible Colors", &showInvisibleColors);
    if (showInvisibleColors) {
        menu::Checkbox(L" Center Edge invis", &showInvisibleCenter);
        if (showInvisibleCenter) {
            menu::offset_y += 10;
            menu::SliderFloat(L"RedCenterInvis", &Globals::CenterEdgeR_Invisible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"GreenCenterInvis", &Globals::CenterEdgeG_Invisible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"BlueCenterInvis", &Globals::CenterEdgeB_Invisible, 0.f, 1.f);

        }
        menu::Checkbox(L" Inner Edgeinvis", &showInvisibleInner);
        if (showInvisibleInner) {
            menu::offset_y += 10;
            menu::SliderFloat(L"RedInnerInvis", &Globals::InnerEdgeR_Invisible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"GreenInnerInvis", &Globals::InnerEdgeG_Invisible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"BlueInnerInvis", &Globals::InnerEdgeB_Invisible, 0.f, 1.f);

        }
        menu::Checkbox(L" Outer Edgeinvis", &showInvisibleOuter);
        if (showInvisibleOuter) {
            menu::offset_y += 10;
            menu::SliderFloat(L"RedOuterInvis", &Globals::OuterEdgeR_Invisible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"GreenOuterInvis", &Globals::OuterEdgeG_Invisible, 0.f, 1.f);
            menu::offset_y += 10;
            menu::SliderFloat(L"BlueOuterInvis", &Globals::OuterEdgeB_Invisible, 0.f, 1.f);

        }
    }
}

void ANTIAIM() {
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;
    const int spacing_y = 25;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // === Main (Left Panel) ===
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;

    menu::Checkbox(ez(L"Enable AntiAim").d(), &Globals::SpinBot);
    menu::Checkbox(L"Enable Manual Override", &Globals::manualoverride);
    menu::Checkbox(L"Enable Oscillation", &Globals::oscillate_enabled);
    menu::Hotkey("AntiAim Key", FVector2D(83, 28.0f), &Globals::aakey);
    menu::offset_y += 28.f;

    // === Pitch Control (0–14)
    menu::Combobox(L"Anti Aim Pitch", FVector2D(100, 24), &Globals::pitchmode,
        L"Up", L"Down", L"None", L"Fast Jitter", L"Aggressive Wave", L"Full Random Flick",
        L"Tick Flick Swap", L"Cos Glide", L"Drop Flick", L"Soft Flicker", L"Sin Sweep",
        L"Hard Snap", L"Tick Flick", L"Smooth Bounce", L"Swing Flick", NULL);
    menu::offset_y -= 6;

    // === Base Yaw Control (0–14)
    menu::Combobox(L"Anti Base Yaw", FVector2D(100, 24), &Globals::baseyaw,
        L"View Yaw", L"Rapid Spin", L"Reverse Spin", L"Sway Yaw", L"Pure Random",
        L"180 Flick", L"Yaw Cycle", L"Wave Glide", L"Side Snap", L"Tick Flip",
        L"Extreme Wave", L"Edge Flick", L"Sharp Jitter", L"Zero/Full Flip", L"Sharp Side Mix", NULL);
    menu::offset_y -= 6;

    // === Desync / Offset Modes (0–14)
    menu::Combobox(L"Anti-Aim Type", FVector2D(100, 24), &Globals::antiaimtype,
        L"Static", L"Fast Jitter", L"Smooth Spin", L"Random",
        L"Tick Flick", L"Sin Drift", L"Cycle Jitter", L"Fake Lag Pulse",
        L"Manual Swap", L"90 Swap", L"Sin Pulse", L"Fake LBY Break",
        L"Hard Flip", L"Soft Offset", L"Cosine Trick", NULL);
    menu::offset_y -= 6;

    // === Right Panel ===
    menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Other", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;

    menu::SliderFloat(L"Oscillation Speed", &Globals::oscillation_speed, 1.f, 30.f, "%.1f");
    menu::offset_y -= 6;
    menu::SliderFloat(L"Oscillation Range", &Globals::oscillation_range, 1.f, 180.f, "%.0f");
    menu::offset_y -= 6;
    menu::SliderFloat(L"Speed", &Globals::speed, 1.f, 50.f, "%.0f");
    //menu::offset_y -= 6;
    //menu::Hotkey("Left Key", FVector2D(83, 28.0f), &Globals::left, 1003);
    //menu::offset_y += 10;
    //menu::Hotkey("Right Key", FVector2D(83, 28.0f), &Globals::right, 1004);
    //menu::offset_y += 10;
    //menu::Hotkey("Backwards Key", FVector2D(83, 28.0f), &Globals::backwards, 1005);
 

}



void W4ALL() {
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT PANEL ("Main") ---
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
    menu::Checkbox(ez(L"Galaxy Chams Self").d(), &Globals::galaxychams2);
    //menu::Checkbox(ez(L"Galaxy Chams Enemy").d(), &Globals::galaxychams3);
    menu::Checkbox(ez(L"Chams").d(), &Globals::ChamsESP);
    if (Globals::ChamsESP) {
        menu::Checkbox(ez(L"add color").d(), &Globals::outline);
        if (Globals::outline) {
            menu::Checkbox(ez(L"Rainbow Chams").d(), &Globals::rainbow_chams);
            menu::ColorPicker(ez(L"rgb color").d(), &Colors::ChamsColor);
            menu::SliderFloat(ez(L"Brightness").d(), &Globals::glowenemyfloat, 1, 40);
        }
    }

    // --- RIGHT PANEL ("Other") ---
    menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Other", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;

    menu::Checkbox(ez(L"wireframe hand").d(), &Globals::WireFrameHand);
    menu::Checkbox(ez(L"wireframe weapon").d(), &Globals::WireFrameWeapon);
    menu::Checkbox(ez(L"wireframe Self").d(), &Globals::OwnWireframe);
    menu::Checkbox(ez(L"Gun Chams").d(), &Globals::gunmaterial);
    menu::Checkbox(L"Enable Hand Chams", &Globals::handchams);

    if (Globals::gunmaterial) {
        menu::Checkbox(L"Enable Rainbow Gun Chams", &Globals::gunChamsRGB);
        menu::ColorPicker(ez(L"Gun Chams color").d(), &Globals::guncolor);
    }
    if (Globals::handchams) {
        menu::Checkbox(L"Enable Rainbow Hand Chams", &Globals::handChamsRGB);
        menu::ColorPicker(ez(L"Hand Chams color").d(), &Globals::handcolor);
        menu::offset_y += 25;
    }
    menu::Checkbox(ez(L"Custom fresnel").d(), &Globals::custom_fresnel);
    menu::Checkbox(ez(L"Self fresnel").d(), &Globals::self_fresnel);
    menu::Checkbox(ez(L"Hand fresnel").d(), &Globals::hand_fresnel);
    menu::ColorPicker(ez(L"fresnel color").d(), &fresnel);
    menu::Checkbox(ez(L"Rainbow Fresnel").d(), &Globals::rainbow_fresnel);
    menu::SliderFloat(ez(L"Brightness").d(), &Globals::intesity, 1, 60);
    menu::SliderFloat(ez(L"Intesity").d(), &Globals::Glow1, 1, 20);
}




void Misc() {
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT PANEL ("Main") ---
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
    menu::Checkbox(ez(L"bunny hop").d(), &Globals::BunnyHop);
    menu::Checkbox(ez(L"TeamCheck").d(), &Globals::TeamCheck);
    menu::Checkbox(ez(L"anti afk").d(), &Globals::AntiAFK);
    menu::Checkbox(ez(L"Draw RGB Crosshair").d(), &Globals::crosshairrainbow);
    menu::Checkbox(ez(L"skip tutorial( in-game )").d(), &Globals::SkipTutorial);
    if (menu::Button(ez(L"Unload").d(), FVector2D{ 83, 30 })) {
        static shadow_vmt viewport_hook2;
        viewport_hook2.unhook();
    }
    //menu::Hotkey("Men4 Keybind", FVector2D(83, 28.0f), &Globals::MenuKey);

    // --- RIGHT PANEL ("Other") ---
    menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Other", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
    menu::offset_y += 10.f;
    menu::Checkbox(ez(L"Watermark").d(), &Globals::Watermark);
    menu::offset_y += 24.f;
    menu::Combobox(ez(L"Watermark Types").d(), { 95, 27 }, &Globals::WatermarkType, L"Basic", L"Advanced", NULL);

    menu::offset_y += 25;

    std::filesystem::path path = std::getenv(ez("USERPROFILE").d());
    path /= ez("config.json").d();

    if (menu::Button(ez(L"Save Config").d(), FVector2D{ 83, 30 })) {
        Config->SaveSettings(path.string());
    }
    menu::SameLine();
    if (menu::Button(ez(L"Load Config").d(), FVector2D{ 83, 30 })) {
        Config->LoadSettings(path.string());
    }
    //menu::Combobox(ez(L"Skybox Types").d(), { 95, 27 }, &Globals::skybox, L"1", L"2", NULL);
}


void Exploits() {
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;
    const int spacing_y = 25;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT PANEL ("Main") ---
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
    menu::Checkbox(ez(L"RageMode").d(), &Globals::ragemode);
    if (Globals::ragemode)
    {
        //menu::Checkbox(ez(L"NightMode").d(), &Globals::nightmode);
        menu::Checkbox(ez(L"View Model Changer").d(), &Globals::ViewModelChanger);
        menu::Checkbox(ez(L"Aspect Ratio Changer").d(), &Globals::bEnableAspectRatioChanger);
        menu::Checkbox(ez(L"Thirdperson - (key T)").d(), &Globals::Thirdperson);
        menu::Checkbox(ez(L"Fov Changer").d(), &Globals::FovChangerBool);
        menu::Checkbox(ez(L"Anti Flash").d(), &Globals::antiflash);
        //menu::Checkbox(ez(L"No Smoke").d(), &Globals::NoSmoke);
       /* menu::Checkbox(ez(L"Big Weapon 1P").d(), &Globals::big_gun);
        menu::Checkbox(ez(L"Big Weapon 3P").d(), &Globals::bigweapon);*/
        //menu::Checkbox(ez(L"Fast Crouch").d(), &Globals::FastCrouch);
        menu::Hotkey("TP KEY", FVector2D(83, 28.0f), &Globals::tpkey);

        // --- RIGHT PANEL ("Adjust") ---
        menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
        menu::BeginSubTab(L"Adjust", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
        menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
        menu::offset_y += 18.f;
        menu::Combobox(L"VMC Type", { 95, 27 }, &Globals::viewmodeltype, L"LongArms", L"CSGO", NULL);
        if (Globals::big_gun || Globals::bigweapon) {
            menu::SliderFloat(ez(L"Size").d(), &Globals::biggunvalue, 0, 170);
        }
        menu::SliderFloat(ez(L"Fov Slider").d(), &Globals::FovChangerValue, 0, 170);
       // menu::SliderFloat(ez(L"NoSmoke Slider").d(), &Globals::testnigga, 1, 300);
        menu::SliderFloat(ez(L"Thirdperson Dis").d(), &Globals::third_person_float, 0, 500);
        if (Globals::bEnableAspectRatioChanger) {
            menu::SliderFloat(ez(L"Aspect Ratio").d(), &Globals::DesiredAspectRatio, 0, 1);
        }

    }
}
void CustomShit() {
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;
    const int spacing_y = 12.5;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT PANEL ("Main") ---
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;

    menu::Checkbox(ez(L"Finishers").d(), &Globals::finisher);
    menu::Checkbox(ez(L"Only Last Kill").d(), &Globals::bOnlyLastKill);
   menu::Checkbox(ez(L"SkyBox").d(), &Globals::skybox);
    menu::Checkbox(ez(L"KillSound").d(), &Globals::killsounds);
    menu::Checkbox(ez(L"KillSay").d(), &Globals::killsay);
    menu::Checkbox(ez(L"ChatSpam - key F1").d(), &Globals::chatspam);
     menu::Checkbox(ez(L"Custom Gun").d(), &Globals::customgun);
    menu::Checkbox(ez(L"Custom Sky").d(), &Globals::skyboxmat);
    menu::Checkbox(ez(L"SkyBox RGB").d(), &Globals::skyboxRGB);
  
    menu::Checkbox(ez(L"Galaxy Chams").d(), &Globals::galaxychams);
 //   menu::Hotkey("Skin key", FVector2D(83, 28.0f), &Globals::skin_key);
    // --- RIGHT PANEL ("Adjust") ---
    menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Adjust", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
    std::vector<std::wstring> skinNamesW;
    std::vector<const wchar_t*> skinNamePtrs;
    menu::InputText(L"Killsay Message", Globals::killsay_input, 128);
    menu::offset_y += 12;
        menu::ColorPicker(ez(L"Horizon Color").d(), &Globals::Horizon);
        menu::ColorPicker(ez(L"Overall Color").d(), &Globals::Overall);
        menu::ColorPicker(ez(L"Zenith Color").d(), &Globals::Zenith);
        menu::ColorPicker(ez(L"Cloud Color").d(), &Globals::Cloud);
        menu::SliderFloat(ez(L"Cloud Speed").d(), &Globals::CloudSpeed, 1, 500);
        menu::offset_y += spacing_y;
        menu::SliderFloat(ez(L"Stars Brightness").d(), &Globals::StarsBrightness, 1, 500);
        menu::offset_y += spacing_y;
        menu::SliderFloat(ez(L"Cloud Opacity").d(), &Globals::CloudOpacity, 0, 2);
    



}
void SK1NCH4NG3R() {
    const float sidebarWidth = 110;
    const float headerHeight = 48;
    const float contentGap = 10;
    const float panelWidth = 260;
    const float panelHeight = 440; // you used 440 for skins
    const float panelGap = 20;
    const float panelPad = 15;
    const float panelHeaderH = 28;
    const float controlStartGap = 8;
    const float comboSpacing = 1.5;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT PANEL ("One") ---
    menu::offset_x = leftPanelX;
    menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"One", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad;
    menu::offset_y += panelHeaderH + controlStartGap;
    menu::Checkbox(ez(L"Buddy Changer").d(), &Globals::BuddyChanger);
    menu::Checkbox(ez(L"Unlock All").d(), &Globals::UnlockAll);
    menu::offset_y += 28.f;
    /*  static wchar_t buddySearch[64] = L"";
      menu::InputText(L"Buddy Search", buddySearch, 64);
      Globals::BuddySearch = std::wstring(buddySearch);*/

      // --- RIGHT PANEL ("Second") ---
     // --- RIGHT PANEL ("Second") ---
    menu::offset_x = rightPanelX;
    menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Second", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad;
    menu::offset_y += panelHeaderH + controlStartGap;

    if (Globals::BuddyChanger)
    {
        // --- Search box in LEFT panel ---
        static wchar_t buddySearch[64] = L"";
        menu::InputText(L"Search Buddy", buddySearch, 64);
        menu::offset_x += 12.5f;
        Globals::BuddySearch = std::wstring(buddySearch);

        // --- Position buttons directly below in LEFT panel ---
        const float buttonW = 180.f;
        const float buttonH = 20.f;
        const float spacingY = 6.f;
        const float maxPanelHeight = panelHeight - panelPad - 60.f;

        float currentX = menu::offset_x;          // Use current menu X
        float currentY = menu::offset_y;          // Use updated offset after InputText

        int shown = 0;

        inventory_manager* manager = ares_instance::get_ares_client_game_instance(UWorldSave)->get_inventory_manager();
        auto charm_instances = Memory::R<TArray<tmap<UObject*, UObject*>>>(std::uintptr_t(manager) + 0x430);

        for (int i = 0; i < charm_instances.size(); ++i)
        {
            UObject* current = charm_instances[i].Key;
            std::wstring rawName = kismentsystemlibrary::get_object_name(current).wide();

            size_t start = rawName.find(L"GunBuddy_");
            size_t end = rawName.find(L"_PrimaryAsset");
            if (start == std::wstring::npos || end == std::wstring::npos) continue;

            std::wstring buddyName = rawName.substr(start + 9, end - (start + 9));
            std::wstring loweredName = ToLower(buddyName);
            std::wstring loweredSearch = ToLower(Globals::BuddySearch);

            if (loweredSearch.empty() || loweredName.find(loweredSearch) != std::wstring::npos)
            {
                menu::offset_x = currentX;
                menu::offset_y = currentY;

                if (menu::Button(buddyName.c_str(), FVector2D{ buttonW, buttonH })) {
                    buddy = current;
                    index = i;
                    BuddyNameYAY = rawName;
                }

                currentY += buttonH + spacingY;
                ++shown;

                if (currentY - menu::offset_y > maxPanelHeight)
                    break; // avoid overflow
            }
        }

        // Restore menu offset to below last button
        menu::offset_y = currentY + 10.f;

        // Show selected buddy
        std::wstring fullName = BuddyNameYAY;
        size_t start = fullName.find(L"GunBuddy_");
        size_t end = fullName.find(L"_PrimaryAsset");
        if (start != std::wstring::npos && end != std::wstring::npos) {
            std::wstring buddyName = fullName.substr(start + 9, end - (start + 9));
            std::wstring displayText = L"Buddy: " + buddyName;
            menu::Text(displayText.c_str());
        }
    }






}

void SaveScalesToFile(const std::string& path)
{
    std::ofstream file(path, std::ios::trunc); // overwrite
    if (!file.is_open()) return;

    file << "Handheld Scale: "
        << Globals::handheldx << ", "
        << Globals::handheldy << ", "
        << Globals::handheldz << "\n";

    file << "Inspect Scale: "
        << Globals::scaleinspectx << ", "
        << Globals::scaleinspecty << ", "
        << Globals::scaleinspectz << "\n";

    file << "Knife Scale: "
        << Globals::knifex << ", "
        << Globals::knifey << ", "
        << Globals::knifez << "\n";

    file.close();
}
void debug() {
    const float sidebarWidth = 110, headerHeight = 48, contentGap = 10;
    const float panelWidth = 260, panelHeight = 440, panelGap = 20;
    const float panelPad = 15, panelHeaderH = 28, controlStartGap = 8;
    const int spacing_y = 12.5;

    float leftPanelX = G::pos.X + sidebarWidth + contentGap;
    float leftPanelY = G::pos.Y + headerHeight + 8;
    float rightPanelX = leftPanelX + panelWidth + panelGap;
    float rightPanelY = leftPanelY;

    // --- LEFT PANEL ("Main") ---
    menu::offset_x = leftPanelX; menu::offset_y = leftPanelY;
    menu::BeginSubTab(L"Main", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
    menu::SliderFloat(ez(L"scaleinspectx").d(), &Globals::scaleinspectx, 0, 5);
    menu::SliderFloat(ez(L"scaleinspecty").d(), &Globals::scaleinspecty, 0, 5);
    menu::SliderFloat(ez(L"scaleinspectz").d(), &Globals::scaleinspectz, 0, 5);
    menu::offset_y += spacing_y;
    menu::SliderFloat(ez(L"handheldx").d(), &Globals::handheldx, 0, 4);
    menu::SliderFloat(ez(L"handheldy").d(), &Globals::handheldy, 0, 4);
    menu::SliderFloat(ez(L"handheldz").d(), &Globals::handheldz, 0, 4);
    menu::offset_y += spacing_y;
    menu::SliderFloat(ez(L"knifex").d(), &Globals::knifex, 0, 3);
    menu::SliderFloat(ez(L"knifey").d(), &Globals::knifey, 0, 3);
    menu::SliderFloat(ez(L"knifez").d(), &Globals::knifez, 0, 3);

    menu::offset_x = rightPanelX; menu::offset_y = rightPanelY;
    menu::BeginSubTab(L"Adjust", FVector2D(menu::offset_x, menu::offset_y), FVector2D(panelWidth, panelHeight));
    menu::offset_x += panelPad; menu::offset_y += panelHeaderH + controlStartGap;
    std::vector<std::wstring> skinNamesW;
    std::vector<const wchar_t*> skinNamePtrs;
    if (menu::Button(ez(L"Save Values").d(), FVector2D{ 83, 30 })) {
        SaveScalesToFile("scales.txt");
    }




}






void ConfigSystem::Save(const std::string& file) {
    try {
        json j;

        j["aimbot"] = {
            {"EnableAim", true},
            {"Silent", false},
            {"RecoilControl", false},
            {"VisibleCheck", true},
            {"FOV", 30.0},
            {"Smooth", 1.0},
            {"AimBone", 0},
            {"RecoilCrosshair", false},
            {"DrawFov", false},
            {"CoolFov", false},
            {"NoSpread", false},
            {"Aimlock", false},
            {"LegitBotKey", 0},
            {"SecondKey", 0}
        };

        j["esp"] = {
            {"DistangeEsp", false},
            {"Snapline", false},
            {"Box2D", false},
            {"Box3D", false},
            {"CorneredBox", false},
            {"dropped_gun", false},
            {"SkeletonEsp", false},
            {"Healthbar", false},
            {"HealthText", false},
            {"skill_esp", false},
            {"HeadESP", false},
            {"SpikeEsp", false},
            {"WeaponEsp", false},
            {"AgentName", false},
            {"riotid", false},
            {"SelectSnapline", 1}
        };

        j["chams"] = {
            {"ChamsESP", false},
            {"outline", false},
            {"rainbow_chams", false},
            {"glowenemyfloat", 1.0},
            {"handchams", false},
            {"handChamsRGB", false}
        };

        j["misc"] = {
            {"WireFrameHand", false},
            {"WireFrameWeapon", false},
            {"HandGlow", false},
            {"custom_fresnel", false},
            {"BunnyHop", false},
            {"AntiAFK", false},
            {"Watermark", true},
            {"WatermarkType", 0},
            {"MenuKey", 45},
            {"crosshairrainbow", false}
        };

        j["exploits"] = {
            {"SkipTutorial", false},
            {"Thirdperson", false},
            {"SpinBot", false},
            {"championssheriff", false},
            {"big_gun", false},
            {"bigweapon", false},
            {"biggunvalue", 1.0},
            {"FovChangerValue", 105.0},
            {"spinbot_speed", 100.0},
            {"third_person_float", 100.0},
            {"ViewModelChanger", false},
            {"bEnableAspectRatioChanger", false},
            {"FovChangerBool", false},
            {"FastCrouch", false},
            {"ScaleX", 1.0},
            {"ScaleY", 1.0},
            {"ScaleZ", 1.0},
            {"WScaleX", 1.0},
            {"WScaleY", 1.0},
            {"WScaleZ", 1.0},
            {"DesiredAspectRatio", 1.25}
        };

        std::ofstream out(file);
        if (!out.is_open()) {
            MessageBoxA(0, "Failed to create C:\\Dsync.json", "Error", MB_ICONERROR);
            return;
        }

        out << j.dump(4);
        MessageBoxA(0, "Config created at C:\\Dsync.json", "Success", MB_OK | MB_ICONINFORMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(0, e.what(), "Exception", MB_ICONERROR);
    }
}


void ConfigSystem::Load(const std::string& file) {
    try {
        std::ifstream check(file);
        std::string content((std::istreambuf_iterator<char>(check)), std::istreambuf_iterator<char>());
        MessageBoxA(0, content.c_str(), "DEBUG: File Content", MB_OK);
        std::filesystem::path full = std::filesystem::absolute(file);
        MessageBoxA(0, full.string().c_str(), "LOADING FROM FULL FILE PATH", MB_OK);
        std::ifstream in(file);
        if (!in.is_open()) {
            MessageBoxA(0, ("Failed to open config file for loading:\n" + file).c_str(), "Config Load Error", MB_ICONERROR);
            return;
        }

        std::stringstream buffer;
        buffer << in.rdbuf();
        json j;

        try {
            j = json::parse(buffer.str());
        }
        catch (const std::exception& e) {
            MessageBoxA(0, e.what(), "JSON Parse Error", MB_ICONERROR);
            return;
        }

        auto safe = [&](const json& obj, const std::string& key, auto def) -> decltype(def) {
            if (obj.contains(key) && !obj[key].is_null()) {
                try {
                    return obj[key].get<decltype(def)>();
                }
                catch (...) {
                    std::string err = "Failed to parse key: " + key;
                    MessageBoxA(0, err.c_str(), "Config Parse Error", MB_ICONWARNING);
                    return def;
                }
            }
            return def;
            };

        // Aimbot
        Globals::EnableAim = safe(j["aimbot"], "EnableAim", false);
        Globals::Silent = safe(j["aimbot"], "Silent", false);
        Globals::RecoilControl = safe(j["aimbot"], "RecoilControl", false);
        Globals::VisibleCheck = safe(j["aimbot"], "VisibleCheck", false);
        Globals::LegitFOvValue = safe(j["aimbot"], "FOV", 90.0f);
        Globals::smooth = safe(j["aimbot"], "Smooth", 1.0f);
        Globals::AimBone = safe(j["aimbot"], "AimBone", 0);
        Globals::RecoilCrosshair = safe(j["aimbot"], "RecoilCrosshair", false);
        Globals::DrawFov = safe(j["aimbot"], "DrawFov", false);
        Globals::CoolFov = safe(j["aimbot"], "CoolFov", false);
        Globals::NoSpreadAimbot = safe(j["aimbot"], "NoSpread", false);
        Globals::Aimlock = safe(j["aimbot"], "Aimlock", false);
        Globals::LegitBotKey = safe(j["aimbot"], "LegitBotKey", 0);
        Globals::Silentaim = safe(j["aimbot"], "SecondKey", 0);

        // ESP
        Globals::DistangeEsp = safe(j["esp"], "DistangeEsp", false);
        Globals::Snapline = safe(j["esp"], "Snapline", false);
        Globals::Box2D = safe(j["esp"], "Box2D", false);
        Globals::Box3D = safe(j["esp"], "Box3D", false);
        Globals::CorneredBox = safe(j["esp"], "CorneredBox", false);
        Globals::dropped_gun = safe(j["esp"], "dropped_gun", false);
        Globals::SkeletonEsp = safe(j["esp"], "SkeletonEsp", false);
        Globals::Healthbar = safe(j["esp"], "Healthbar", false);
        Globals::HealthText = safe(j["esp"], "HealthText", false);
        Globals::skill_esp = safe(j["esp"], "skill_esp", false);
        Globals::HeadESP = safe(j["esp"], "HeadESP", false);
        Globals::SpikeEsp = safe(j["esp"], "SpikeEsp", false);
        Globals::WeaponEsp = safe(j["esp"], "WeaponEsp", false);
        Globals::AgentName = safe(j["esp"], "AgentName", false);
        Globals::riotid = safe(j["esp"], "riotid", false);
        Globals::SelectSnapline = safe(j["esp"], "SelectSnapline", 0);

        // Chams
        Globals::ChamsESP = safe(j["chams"], "ChamsESP", false);
        Globals::outline = safe(j["chams"], "outline", false);
        Globals::rainbow_chams = safe(j["chams"], "rainbow_chams", false);
        Globals::glowenemyfloat = safe(j["chams"], "glowenemyfloat", 1.0f);
        Globals::handchams = safe(j["chams"], "handchams", false);
        Globals::handChamsRGB = safe(j["chams"], "handChamsRGB", false);
        // Uncomment and handle color parsing if needed
        // Globals::handcolor        = safe(j["chams"], "handcolor", FLinearColor(255, 255, 255));

        // Misc
        Globals::WireFrameHand = safe(j["misc"], "WireFrameHand", false);
        Globals::WireFrameWeapon = safe(j["misc"], "WireFrameWeapon", false);
        Globals::HandGlow = safe(j["misc"], "HandGlow", false);
        Globals::custom_fresnel = safe(j["misc"], "custom_fresnel", false);
        Globals::BunnyHop = safe(j["misc"], "BunnyHop", false);
        Globals::AntiAFK = safe(j["misc"], "AntiAFK", false);
        Globals::Watermark = safe(j["misc"], "Watermark", false);
        Globals::WatermarkType = safe(j["misc"], "WatermarkType", 0);
        Globals::MenuKey = safe(j["misc"], "MenuKey", 0);
        Globals::crosshairrainbow = safe(j["misc"], "crosshairrainbow", false);

        // Exploits
        Globals::SkipTutorial = safe(j["exploits"], "SkipTutorial", false);
        Globals::Thirdperson = safe(j["exploits"], "Thirdperson", false);
        Globals::SpinBot = safe(j["exploits"], "SpinBot", false);
        Globals::championssheriff = safe(j["exploits"], "championssheriff", false);
        Globals::big_gun = safe(j["exploits"], "big_gun", false);
        Globals::bigweapon = safe(j["exploits"], "bigweapon", false);
        Globals::biggunvalue = safe(j["exploits"], "biggunvalue", 0.0f);
        Globals::FovChangerValue = safe(j["exploits"], "FovChangerValue", 90.0f);
        Globals::spinbot_speed = safe(j["exploits"], "spinbot_speed", 0.0f);
        Globals::third_person_float = safe(j["exploits"], "third_person_float", 0.0f);
        Globals::ViewModelChanger = safe(j["exploits"], "ViewModelChanger", false);
        Globals::bEnableAspectRatioChanger = safe(j["exploits"], "bEnableAspectRatioChanger", false);
        Globals::FovChangerBool = safe(j["exploits"], "FovChangerBool", false);
        Globals::FastCrouch = safe(j["exploits"], "FastCrouch", false);
        Globals::ScaleX = safe(j["exploits"], "ScaleX", 1.0f);
        Globals::ScaleY = safe(j["exploits"], "ScaleY", 1.0f);
        Globals::ScaleZ = safe(j["exploits"], "ScaleZ", 1.0f);
        Globals::WScaleX = safe(j["exploits"], "WScaleX", 1.0f);
        Globals::WScaleY = safe(j["exploits"], "WScaleY", 1.0f);
        Globals::WScaleZ = safe(j["exploits"], "WScaleZ", 1.0f);
        Globals::DesiredAspectRatio = safe(j["exploits"], "DesiredAspectRatio", 1.0f);

        MessageBoxA(0, "Configuration loaded successfully!", "Load Config", MB_OK | MB_ICONINFORMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(0, e.what(), "Exception in ConfigSystem::Load()", MB_ICONERROR);
    }
}



auto isActorValid = [](AShooterCharacter* Actor, AShooterCharacter* MyShooter) -> bool {

    return Actor && MyShooter;
    };
void(__fastcall* oHkDeath)(void*, void*);


void Rehook(UWorld* Uworld, APlayerController* PlayerController) {

    if (Uworld == nullptr || PlayerController == nullptr)
        return;

    APlayerCameraManager* CameraManager = PlayerController->GetPlayerCameraManager();

    if (CameraManager == nullptr)
        return;

    try {
        if (G::UWorldClass != Uworld && CameraManager) {

            Hook::vhook((void*)CameraManager, (void*)SetCameraCachePOVHook, 0xf2, (void**)&SetCameraCachePOVOriginal);

            G::UWorldClass = Uworld;
        }

    }
    catch (const std::exception& e) {
        return;
    }

}
