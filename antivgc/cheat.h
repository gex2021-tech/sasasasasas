#include <cfloat>
#include "./includes.h"
#include "Hookign/ret_spoofing.h"
#include "canvas_gui.h" 
#include <unordered_set>
#pragma comment(lib, "Winmm.lib")
// Somewhere global
skin_data_asset* G_LastEquippedSkinData = nullptr;
enum class EAresEquippableState : uint8_t {
    Unknown = 0,
    Idle = 1,
    Equipping = 2,
    Inspecting = 3,
    Attacking = 4,
    Firing = 5,
    Reloading = 6,
    ADS_Idle = 7,
    ADS_Firing = 8,
    Count = 9,
    EAresEquippableState_MAX = 10
};
struct FProjectileThrowTuning {
   
    float SpeedScale; // 0x20(0x04)
    float UpwardArc; // 0x24(0x04)
    float UpwardShift; // 0x28(0x04)
    char pad_2C[0x4]; // 0x2c(0x04)
    struct FVector ThrowOffset; // 0x30(0x18)
    float ThrowDelay; // 0x48(0x04)
    bool bJumpThrowNormalization_LaunchPoint; // 0x4c(0x01)
    bool bJumpThrowNormalization_ImpartedVelocity; // 0x4d(0x01)
    char pad_4E[0x2]; // 0x4e(0x02)
};
//void ReadGrenadeThrowInfo() {
//    if (!MyShooter) return;
//
//    auto inventory = MyShooter->GetInventory();
//    if (!inventory) return;
//
//    auto weapon = inventory->GetCurrentWeapon();
//    if (!weapon) return;
//
//    AGameObject* Ability = nullptr;
//    static AGameObject* CachedAbility = nullptr;
//
//    // ✅ Use cached Ability object if valid and name matches
//    if (CachedAbility && Memory::IsValidPointer((uintptr_t)CachedAbility)) {
//        auto name = kismentsystemlibrary::get_object_name((UObject*)CachedAbility);
//        if (name.IsValid() && name.ToString() == "Ability_Sarge_Q_Molotov_Production_C") {
//            Ability = CachedAbility;
//        }
//    }
//
//    // ✅ Otherwise, find the ability dynamically
//    if (!Ability) {
//        TArray<AGameObject*> Objects;
//        GameplayStatics::GetAllActorsOfClass(UWorldSave, Class::ShooterGameObject());
//
//        for (auto* Object : Objects) {
//            if (!Object || !Memory::IsValidPointer((uintptr_t)Object)) continue;
//
//            auto name = kismentsystemlibrary::get_object_name((UObject*)Object);
//            if (!name.IsValid()) continue;
//
//            if (name.ToString() == "Ability_Sarge_Q_Molotov_Production_C") {
//                Ability = Object;
//                CachedAbility = Object;
//                break;
//            }
//        }
//    }
//
//    if (!Ability) return;
//
//    // ✅ Use correct offset: 0x1188 for UProjectileThrowStateComponent*
//    uintptr_t throwState = Memory::R<uintptr_t>((uintptr_t)Ability + 0x1228);
//    if (!throwState) return;
//
//    // ✅ FProjectileThrowTuning* at 0x310
//    uintptr_t tuning = Memory::R<uintptr_t>(throwState + 0x310);
//    if (!tuning) return;
//
//    // === Correct FProjectileThrowTuning layout ===
//    // skip 0x00 → 0x18 (ProjectileType*)
//    float speedScale = Memory::R<float>(tuning + 0x20);
//    float upwardArc = Memory::R<float>(tuning + 0x24);
//    float upwardShift = Memory::R<float>(tuning + 0x28);
//    FVector throwOffset = Memory::R<FVector>(tuning + 0x30);
//    float throwDelay = Memory::R<float>(tuning + 0x48);
//    bool bNormLaunchPoint = Memory::R<bool>(tuning + 0x4C);
//    bool bNormVelocity = Memory::R<bool>(tuning + 0x4D);
//
//    // === Output ===
//    printf("Grenade Tuning Info:\n");
//    printf("  SpeedScale       : %.2f\n", speedScale);
//    printf("  UpwardArc        : %.2f\n", upwardArc);
//    printf("  UpwardShift      : %.2f\n", upwardShift);
//    printf("  ThrowOffset      : X: %.2f Y: %.2f Z: %.2f\n", throwOffset.X, throwOffset.Y, throwOffset.Z);
//    printf("  ThrowDelay       : %.2f sec\n", throwDelay);
//    printf("  Normalize Point  : %s\n", bNormLaunchPoint ? "true" : "false");
//    printf("  Normalize Velocity: %s\n", bNormVelocity ? "true" : "false");
//}


inline FVector RotationToVector(const FVector& rot) {
    double radPitch = rot.X * PI / 180;
    double radYaw = rot.Y * PI / 180;

    return FVector(
        cos(radPitch) * cos(radYaw),
        cos(radPitch) * sin(radYaw),
        sin(radPitch)
    );
}
static std::unordered_map<UObject*, std::string> objectNameCache;
inline std::string get_cached_name(UObject* obj) {
    auto it = objectNameCache.find(obj);
    if (it != objectNameCache.end()) return it->second;
    std::string name = kismentsystemlibrary::get_object_name(obj).ToString();
    objectNameCache[obj] = name;
    return name;
}
static int killSoundIndex = 0;
const wchar_t* killSounds[] = {
    L"C:\\AntiVGCKillSounds\\kill1.wav",
    L"C:\\AntiVGCKillSounds\\kill2.wav",
    L"C:\\AntiVGCKillSounds\\kill3.wav",
    L"C:\\AntiVGCKillSounds\\kill4.wav",
    L"C:\\AntiVGCKillSounds\\kill5.wav"
};
enum class EChatMode

    : uint8

{


    None

    = 0,

    Team

    = 1,

    All

    = 2,

    Ping

    = 3,

    Count

    = 4,

    EChatMode_MAX

    = 5

};
void HSVtoRGB(float h, float s, float v, uint8_t& R, uint8_t& G, uint8_t& B) {
    float c = v * s;
    float X = c * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
    float m = v - c;

    float r_, g_, b_;
    if (h < 60) { r_ = c; g_ = X; b_ = 0; }
    else if (h < 120) { r_ = X; g_ = c; b_ = 0; }
    else if (h < 180) { r_ = 0; g_ = c; b_ = X; }
    else if (h < 240) { r_ = 0; g_ = X; b_ = c; }
    else if (h < 300) { r_ = X; g_ = 0; b_ = c; }
    else { r_ = c; g_ = 0; b_ = X; }

    R = static_cast<uint8_t>((r_ + m) * 255);
    G = static_cast<uint8_t>((g_ + m) * 255);
    B = static_cast<uint8_t>((b_ + m) * 255);
}

struct bitset {
    char pad_716_0 : 3;
    char bOverrideMinLod : 1;
    char bUseBoundsFromMasterPoseComponent : 1;
    char bForceWireframe : 1;
    char bDisplayBones : 1;
    char bDisableMorphTarget : 1;
} mesh_bitset;
bool IsValidPointer(uintptr_t ptr)
{
    return ptr > 0x1000 && ptr < 0x7FFFFFFFFFFF;
}



struct BulletTracer {
    FVector Start;
    FVector End;
    float Lifetime;
};
inline std::chrono::steady_clock::time_point g_LastFrameTime;
inline float GetManualDeltaTime()
{
    using namespace std::chrono;

    static bool initialized = false;
    if (!initialized)
    {
        g_LastFrameTime = steady_clock::now();
        initialized = true;
        return 0.016f; // Default to ~60 FPS on first call
    }

    auto now = steady_clock::now();
    duration<float> delta = now - g_LastFrameTime;
    g_LastFrameTime = now;

    return delta.count(); // Seconds
}
inline std::vector<BulletTracer> BulletTracerList;
inline void DrawBulletTracers(UCanvas* canvas, APlayerController* controller, float DeltaTime)
{
    if (!canvas || !controller) return;

    for (int i = 0; i < BulletTracerList.size(); )
    {
        auto& tracer = BulletTracerList[i];

        FVector2D ScreenStart, ScreenEnd;
        if (controller->ProjectWorldLocationToScreen(tracer.Start, ScreenStart, false) &&
            controller->ProjectWorldLocationToScreen(tracer.End, ScreenEnd, false))
        {
            canvas->K2_DrawLine(ScreenStart, ScreenEnd, 1.5f, FLinearColor(1.f, 1.f, 0.f, 1.f)); // Yellow
        }

        tracer.Lifetime -= DeltaTime;
        if (tracer.Lifetime <= 0.f)
            BulletTracerList.erase(BulletTracerList.begin() + i);
        else
            ++i;
    }
}

UObject* world1337;

static std::chrono::steady_clock::time_point lastSaveTime = std::chrono::steady_clock::now();
void DrawBulletTracers2(float DeltaTime, UCanvas* Canva)
{
    for (int i = 0; i < BulletTracerList.size(); ++i)
    {
        BulletTracer& tracer = BulletTracerList[i];
        tracer.Lifetime -= DeltaTime;

        if (tracer.Lifetime <= 0.0f)
        {
            BulletTracerList.erase(BulletTracerList.begin() + i);
            --i;
            continue;
        }

        // Fade based on remaining life
        float alpha = tracer.Lifetime / 1.0f;
        FLinearColor color = FLinearColor(1.f, 0.f, 0.f, alpha); // red → fades out

        // Draw line using KismetSystemLibrary (requires UObject*)
        Canva->DrawBulletTracer(
            world1337,         // WorldContextObject
            tracer.Start,
            tracer.End,
            color,
            tracer.Lifetime,     // Line duration
            1.5f                      // Thickness
        );
    }
}
float GetRainbowHue() {
    static auto start = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    float seconds = std::chrono::duration<float>(now - start).count();
    return fmodf(seconds * 60.0f, 360.0f); // 60 degrees per second
}

inline void* PlayFinisherEffect(uintptr_t effect)
{
    using FinisherFn = void* (__fastcall*)(uintptr_t);

    static FinisherFn fn = reinterpret_cast<FinisherFn>(VALORANT::Module + Offsets::play_finisher_effect);
    if (!fn) return nullptr;
    return reinterpret_cast<void*(__fastcall*)(uintptr_t, uintptr_t, void*)>(spoofcall_stub)(
        effect,
        Offsets::MagicOffsets,
        (void*)fn
    );

}


bool skinapplied = false;
FText CreateFText(const wchar_t* text)
{
    static UObject* string_table = nullptr;
    static UObject* function = nullptr;

    if (!function)
        function = UObject::find_object<UObject>(L"Engine.KismetTextLibrary.Conv_StringToText");

    struct {
        FString InString;
        FText ReturnValue;
    } params;

    params.InString = FString(text);

    // You can use any valid UObject here to call the function, often the default of the library
    UObject* context = UObject::find_object<UObject>(L"Engine.Default__KismetTextLibrary");

    context->ProcessEvent_(function, &params);

    return params.ReturnValue;
}

void hk_death(AShooterCharacter* shooter_character, UDamageResponse* a2) {
    try {
        if (!shooter_character || !Memory::IsValidPointer((uintptr_t)shooter_character)) {
            return oHkDeath(shooter_character, a2);
        }







        APawn* pawn = MyController->get_acknowledged_pawn();
        AShooterCharacter* CharacterContext = (AShooterCharacter*)MyShooter;
        APawn* LPawnContext = (APawn*)pawn;
        auto DamageResponse = a2;

        if (!CharacterContext || !LPawnContext || !DamageResponse) {
            return oHkDeath(shooter_character, a2);
        }

        if (!Memory::IsValidPointer((uintptr_t)CharacterContext) ||
            !Memory::IsValidPointer((uintptr_t)LPawnContext) ||
            !Memory::IsValidPointer((uintptr_t)DamageResponse)) {
            return oHkDeath(shooter_character, a2);
        }

        auto component = (uintptr_t)Memory::R<UObject*>((uintptr_t)shooter_character + Offsets::death_reaction_component_offset);
        if (!component || !Memory::IsValidPointer(component)) {
            return oHkDeath(shooter_character, a2);
        }

        BYTE b1 = Memory::R<BYTE>(component + 0x15A);
        BYTE b2 = Memory::R<BYTE>(component + 0x168);

        if (!(b1 == 0 || b2 == 1)) {
            return oHkDeath(shooter_character, a2);
        }

        auto Killer = DamageResponse->GetDamageCauser();
        auto Equippable = DamageResponse->GetEquippableUsed();

        if (!Killer || !Equippable ||
            !Memory::IsValidPointer((uintptr_t)Killer) ||
            !Memory::IsValidPointer((uintptr_t)Equippable)) {
            return oHkDeath(shooter_character, a2);
        }

        auto ItemSlot = Equippable->GetItemSlot();

        if (!UWorldSave || !Memory::IsValidPointer((uintptr_t)UWorldSave)) {
            return oHkDeath(shooter_character, a2);
        }

        TArray<AShooterCharacter*> enemiese = ShooterGameBlueprints::FindAllShooterCharactersWithAlliance(
            UWorldSave, MyShooter, EAresAlliance::Alliance_Enemy, false, true);
        G::MyWeapon = MyShooter->GetInventory()->GetCurrentWeapon();

        if (Globals::finisher || Globals::killsounds || Globals::killsay && MyShooter->IsAlive() && MyShooter->GetHealth() > 0 && MyShooter && Memory::IsValidPointer((uintptr_t)MyShooter)) {
            if (Killer == LPawnContext) {
                int numEnemies = enemiese.size();
                // Manually check if shooter_character is in the enemy list
                for (int i = 0; i < enemiese.size(); ++i) {
                    if (enemiese[i] == shooter_character) {
                        numEnemies -= 1;
                        break;
                       }
                }
                if (Globals::killsounds)
                {
                    PlaySoundW(killSounds[killSoundIndex], NULL, SND_FILENAME | SND_ASYNC);
                    killSoundIndex = (killSoundIndex + 1) % 5;
                }
                if (Globals::killsay)
                {
                    auto world = UWorld::GetWorld();
                    if (!world)
                    {
                        //printf("[killsay]  UWorldSave is null\n");
                        return;
                    }

                    UThreadedChatManager* chat_manager = UThreadedChatManager::GetThreadedChatManager(world);
                    if (!chat_manager)
                    {
                        //printf("[killsay]  Failed to get UThreadedChatManager\n");
                        return;
                    }

                    FString raw = FString(Globals::killsay_input);
                 
                    FText msg = kismentsystemlibrary::Conv_StringToText(raw);

                    if (!msg.Data)
                    {
                       // printf("[killsay] FText conversion failed\n");
                        return;
                    }

                    //printf("[killsay]  Sending message: %ws\n", raw.c_str());
                    chat_manager->SendChatMessageV2(EChatRoomType::All, msg);
                }




                bool shouldPlayFinisher = Globals::bOnlyLastKill ? (numEnemies == 0) : true;

                //skin_data_asset* skinData = G::MyWeapon->GetSkinDataAsset();
                std::string raw_weapon = get_cached_name(G::MyWeapon);
                std::string weapon_name = normalize_weapon_class(raw_weapon.c_str());
                if (Globals::finisher)
                {
                    auto applyFinisher = [&]() {


                        std::wstring skin = get_chosen_skin(weapon_name);
                        //wprintf(L"[DEBUG] Chosen skin: %ls\n", skin.empty() ? L"(none)" : skin.c_str());
                        UObject* finisher = get_finisher_from_skin(skin.c_str());

                        static UObject* dummyFinisher = UObject::find_object2<UObject*>(L"FXC_Finisher_Invalid_Victim_C", reinterpret_cast<UObject*>(-1));
                        Memory::W<UObject*>(component + Offsets::montage_effect_override_offset, dummyFinisher);
                        Memory::W<UObject*>(component + Offsets::montage_effect_override_context_offset, nullptr);

                        Memory::W<UObject*>(component + Offsets::montage_effect_override_offset, nullptr);
                        Memory::W<UObject*>(component + Offsets::montage_effect_override_context_offset, nullptr);

                        Memory::W<UObject*>(component + Offsets::montage_effect_override_offset, finisher);
                        Memory::W<UObject*>(component + Offsets::montage_effect_override_context_offset, LPawnContext);

                        PlayFinisherEffect(component);
                        };

                    if (shouldPlayFinisher) {
                        if (weapon_name.find("AssaultRifle_AK_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("AssaultRifle_ACR_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("BoltSniper_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("AssaultRifle_Burst_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("AutomaticPistol_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("DMR_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("RevolverPistol_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("LugerPistol_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("SubMachineGun_MP5_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("BasePistol_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("LeverSniperRifle_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("DS_Gun_C") != std::string::npos) {
                            applyFinisher();
                        }
                        else if (weapon_name.find("Ability_Melee_Base_C") != std::string::npos) {
                            Memory::W<UObject*>(component + Offsets::montage_effect_override_offset, nullptr);
                            Memory::W<UObject*>(component + Offsets::montage_effect_override_context_offset, nullptr);
                        }
                    }
                }
            }
        }



        return oHkDeath(shooter_character, a2);
    }
    catch (...) {
        return oHkDeath(shooter_character, a2);
    }
}
void hk_death_trampoline_hook(AShooterCharacter* shooter_character, UDamageResponse* a2) {
    return reinterpret_cast<void(*)(AShooterCharacter * shooter_character, UDamageResponse * a2, uintptr_t, void*)>(spoofcall_stub)(shooter_character, a2, Offsets::MagicOffsets, (void*)hk_death);
}
constexpr float RadToDeg = 57.295779513082f;
constexpr float DegToRad = 0.017453292519943f;

// Rotator -> Forward Vector
inline FVector RotationToVector2(const FRotator& rot)
{
    float CP = cosf(rot.Pitch * DegToRad);
    float SP = sinf(rot.Pitch * DegToRad);
    float CY = cosf(rot.Yaw * DegToRad);
    float SY = sinf(rot.Yaw * DegToRad);

    return FVector(CP * CY, CP * SY, SP);
}

// Vector -> Rotator
inline FRotator VectorToRotation(const FVector& v)
{
    FRotator rot;
    rot.Yaw = atan2f(v.Y, v.X) * RadToDeg;
    rot.Pitch = atan2f(v.Z, sqrtf(v.X * v.X + v.Y * v.Y)) * RadToDeg;
    rot.Roll = 0.f;
    return rot;
}
FVector RotatorToVector(const FRotator& rot)
{
    float radPitch = rot.Pitch * PI / 180.f;
    float radYaw = rot.Yaw * PI / 180.f;

    float CP = cosf(radPitch);
    float SP = sinf(radPitch);
    float CY = cosf(radYaw);
    float SY = sinf(radYaw);

    return FVector(CP * CY, CP * SY, SP);
}
#define MAX_TRACERS 100
#define TRACER_LIFETIME 7.0f
#define MAX_COLORS 20

struct Tracer {
    FVector worldStart;
    FVector worldEnd;
    float timeCreated;
    FLinearColor color;
    FVector finalDirection;  // optional, for debug
    FVector spreadOffset;    // optional
};
std::vector<Tracer> tracerQueue;

Tracer tracers[MAX_TRACERS];
int currentTracerIndex = 0;
int currentColorIndex = 0;

float lastTracerTime = 0.0f;
void draw_enemy_tracers(
    UCanvas* canvas,
    APlayerController* controllers,
    FVector eyeLocation,
    FRotator viewRotation,
    FVector spreadOffset,
    bool create_new
)
{
    static Tracer tracers[MAX_TRACERS];
    static int currentTracerIndex = 0;

    float currentTime = GetTickCount64() / 1000.0f;

    if (create_new)
    {



        FRotator bulletRot = viewRotation;
        bulletRot.Pitch += spreadOffset.X;
        bulletRot.Yaw += spreadOffset.Y;
        bulletRot.Roll += spreadOffset.Z;

        FVector preciseDir = RotatorToVector(bulletRot);

        FVector start = eyeLocation;
        FVector end = start + preciseDir * 5000.f;

        // ✅ Optional line trace to stop at impact
        FHitResult hit{};
        TArray<AActor*> ignoreActors;
        bool didHit = kismentsystemlibrary::LineTraceSingle(
            UWorld::GetWorld(),
            start,
            end,
            ETraceTypeQuery::TraceTypeQuery1,
            true,
            ignoreActors,
            EDrawDebugTrace::None,
            hit,
            true,
            FLinearColor(0.f, 1.f, 0.f, 1.f),
            FLinearColor(1.f, 0.f, 0.f, 1.f),
            0.f
        );

        FVector worldEnd = (didHit && hit.bBlockingHit) ? hit.ImpactPoint : end;
        // 🔽 Darker color range: 0.05 to 0.3
        FLinearColor tracerColor;

        int choice = rand() % 3; // 0 = red, 1 = green, 2 = blue

        switch (choice) {
        case 0: tracerColor = FLinearColor(1.0f, 0.2f, 0.2f, 1.0f); break; // bright red
        case 1: tracerColor = FLinearColor(0.2f, 1.0f, 0.2f, 1.0f); break; // bright green
        case 2: tracerColor = FLinearColor(0.2f, 0.2f, 1.0f, 1.0f); break; // bright blue
        }



        tracers[currentTracerIndex] = {
            start,
            worldEnd,
            currentTime,
            tracerColor
        };

        currentTracerIndex = (currentTracerIndex + 1) % MAX_TRACERS;
    }

    

    for (int i = 0; i < MAX_TRACERS; ++i)
    {
        Tracer& tracer = tracers[i];
        float age = currentTime - tracer.timeCreated;

        if (age < TRACER_LIFETIME)
        {
            FVector2D screenStart, screenEnd;

            if (controllers->ProjectWorldLocationToScreen(tracer.worldStart, screenStart, true) &&
                controllers->ProjectWorldLocationToScreen(tracer.worldEnd, screenEnd, true))
            {
                float alpha = 1.0f - (age / TRACER_LIFETIME);
                FLinearColor tracerColor = tracer.color;
                tracerColor.A = alpha;

                canvas->K2_DrawLine(screenStart, screenEnd, 1.5f, tracerColor);
            }
        }
    }

    //// Assuming controller or HUD has this method
    //controllers->ClientDrawDebugLines(debugLines);

}



int check1 = false;
// Manual bounce-based trajectory simulation
// Manual bounce simulation (keep this outside your main loop)
std::vector<FVector> simulate_trajectory_manual_bounce(
    FVector origin,
    FVector velocity,
    float gravityZ,
    float timeStep,
    int maxSteps,
    float groundZ,
    float elasticity = 0.4f,
    float stopThreshold = 90.0f
) {
    std::vector<FVector> path;
    FVector position = origin;

    for (int i = 0; i < maxSteps; ++i) {
        path.push_back(position);

        velocity.Z += gravityZ * timeStep;
        FVector nextPos = position + velocity * timeStep;

        if (position.Z > groundZ && nextPos.Z <= groundZ) {
            nextPos.Z = groundZ;
            velocity.Z *= -elasticity;
            velocity.X *= 0.75f;
            velocity.Y *= 0.75f;

            if (velocity.Size() < stopThreshold) break;
        }

        position = nextPos;
    }

    return path;
}



std::vector<FVector> simulate_trajectory(
    const FVector& Origin,
    const FVector& Direction,  // normalized
    float Speed,
    const FVector& Gravity,
    float TimeStep,
    int MaxSteps
)
{
    std::vector<FVector> Path;
    FVector Position = Origin;
    FVector Velocity = Direction * Speed;

    for (int i = 0; i < MaxSteps; ++i)
    {
        Path.push_back(Position);
        Position = Position + Velocity * TimeStep;
        Velocity = Velocity + Gravity * TimeStep;
    }

    return Path;
}
// Tries to solve for initial velocity to hit target from origin, with gravity and speed
// Returns true if solution found, false otherwise
bool solve_projectile_arc(
    const FVector& origin,
    const FVector& target,
    float speed,
    float gravity,
    FVector& outInitialVelocity)
{
    // Simplified 2D projectile motion formula on flat terrain for demonstration:

    FVector displacement = target - origin;
    float dx = sqrtf(displacement.X * displacement.X + displacement.Y * displacement.Y);
    float dy = displacement.Z;

    float g = fabsf(gravity);
    float speedSq = speed * speed;
    float underSqrt = speedSq * speedSq - g * (g * dx * dx + 2 * dy * speedSq);

    if (underSqrt < 0) return false; // no solution

    float sqrtVal = sqrtf(underSqrt);

    float angle1 = atanf((speedSq + sqrtVal) / (g * dx));
    // float angle2 = atanf((speedSq - sqrtVal) / (g * dx)); // alternative angle

    // Compose velocity vector
    float vx = speed * cosf(angle1) * (displacement.X / dx);
    float vy = speed * cosf(angle1) * (displacement.Y / dx);
    float vz = speed * sinf(angle1);

    outInitialVelocity = FVector(vx, vy, vz);
    return true;
}



FVector2D pos = { ((double)GetSystemMetrics(SM_CXSCREEN) / 2), ((double)GetSystemMetrics(SM_CYSCREEN) / 2) };
bool IsInGame(UWorld* World) {
    auto controller = ShooterGameBlueprints::GetFirstLocalPlayerController(World);
    if (!controller) return false;

    auto pawn = controller->K2_GetPawn();
    return pawn && Memory::IsValidPointer2(pawn);
}

void PostRender(uintptr_t _this, UCanvas* canvas, uintptr_t a3) {


    static UCanvas* cachedCanvas = nullptr;
    if (!cachedCanvas)
        cachedCanvas = UObject::find_object2<UCanvas*>(ez(L"/Engine/Transient.DebugCanvasObject").d(), (UObject*)-1);

    canvas = cachedCanvas;

    menu::SetupCanvas(canvas);
    UWorld* World = Memory::R<UWorld*>(_this + 0x80); //0x68 
    static UWorld* LastWorld = nullptr;

    MyController = ShooterGameBlueprints::GetFirstLocalPlayerController(World);
    Ashootercamera = MyController->GetShooterCamera();
    MyShooter = MyController->GetShooterCharacter();
    TArray<AShooterCharacter*> Actors = ShooterGameBlueprints::FindAllShooterCharactersWithAlliance(World, MyShooter, EAresAlliance::Alliance_Any, false, true);
    TArray<AShooterCharacter*> enemies = ShooterGameBlueprints::FindAllShooterCharactersWithAlliance(World, MyShooter, EAresAlliance::Alliance_Enemy, false, true);
    TArray<AShooterCharacter*> allys = ShooterGameBlueprints::FindAllShooterCharactersWithAlliance(World, MyShooter, EAresAlliance::Alliance_Ally, false, true);
    //printf("[+] Actor Count: %d\n", Actors.Num());
    APlayerCameraManager* MyCamera = MyController->GetPlayerCameraManager();
    MyPawn = MyController->K2_GetPawn();

    //printf("[+] Actor Count: %d\n", Actors.Num());

    // Store previous states
    static bool wasDisabled = false;

    // Backup states
    static bool prev_auto_wall = false;
    static bool prev_NoSpreadAimbot = false;
    static bool prev_bullettracers = false;
    static bool prev_outline2 = false;
    static bool prev_SpinBot = false;
    static bool prev_BunnyHop = false;
    static bool prev_ViewModelChanger = false;
    static bool prev_antiflash = false;
    static bool prev_finisher = false;
    static bool prev_skybox = false;
    static bool prev_killsounds = false;
    static bool prev_killsay = false;
    static bool prev_UnlockAll = false;
    static bool prev_BuddyChanger = false;

    // Add all visual checkboxes (new from menu)
    static bool prev_galaxychams2 = false;
    static bool prev_ChamsESP = false;
    static bool prev_outline = false;
    static bool prev_rainbow_chams = false;
    static bool prev_WireFrameHand = false;
    static bool prev_WireFrameWeapon = false;
    static bool prev_OwnWireframe = false;
    static bool prev_gunmaterial = false;
    static bool prev_gunChamsRGB = false;
    static bool prev_handchams = false;
    static bool prev_handChamsRGB = false;
    static bool prev_custom_fresnel = false;
    static bool prev_self_fresnel = false;
    static bool prev_hand_fresnel = false;
    static bool prev_rainbow_fresnel = false;

    if (allys.Num() <= 0 || enemies.Num() <= 0 && !wasDisabled) {
        // Backup only if not already disabled
        prev_auto_wall = Globals::auto_wall;
        prev_NoSpreadAimbot = Globals::NoSpreadAimbot;
        prev_bullettracers = Globals::bullettracers;
        prev_outline2 = Globals::outline2;
        prev_SpinBot = Globals::SpinBot;
        prev_BunnyHop = Globals::BunnyHop;
        prev_ViewModelChanger = Globals::ViewModelChanger;
        prev_antiflash = Globals::antiflash;
        prev_finisher = Globals::finisher;
        prev_skybox = Globals::skybox;
        prev_killsounds = Globals::killsounds;
        prev_killsay = Globals::killsay;
        prev_UnlockAll = Globals::UnlockAll;
        prev_BuddyChanger = Globals::BuddyChanger;

        prev_galaxychams2 = Globals::galaxychams2;
        prev_ChamsESP = Globals::ChamsESP;
        prev_outline = Globals::outline;
        prev_rainbow_chams = Globals::rainbow_chams;
        prev_WireFrameHand = Globals::WireFrameHand;
        prev_WireFrameWeapon = Globals::WireFrameWeapon;
        prev_OwnWireframe = Globals::OwnWireframe;
        prev_gunmaterial = Globals::gunmaterial;
        prev_gunChamsRGB = Globals::gunChamsRGB;
        prev_handchams = Globals::handchams;
        prev_handChamsRGB = Globals::handChamsRGB;
        prev_custom_fresnel = Globals::custom_fresnel;
        prev_self_fresnel = Globals::self_fresnel;
        prev_hand_fresnel = Globals::hand_fresnel;
        prev_rainbow_fresnel = Globals::rainbow_fresnel;

        // Disable all
        Globals::auto_wall = false;
        Globals::NoSpreadAimbot = false;
        Globals::bullettracers = false;
        Globals::outline2 = false;
        Globals::SpinBot = false;

        Globals::BunnyHop = false;
        Globals::ViewModelChanger = false;
        Globals::antiflash = false;
        Globals::finisher = false;
        Globals::skybox = false;
        Globals::killsounds = false;
        Globals::killsay = false;
        Globals::UnlockAll = false;
        Globals::BuddyChanger = false;

        Globals::galaxychams2 = false;
        Globals::ChamsESP = false;
        Globals::outline = false;
        Globals::rainbow_chams = false;
        Globals::WireFrameHand = false;
        Globals::WireFrameWeapon = false;
        Globals::OwnWireframe = false;
        Globals::gunmaterial = false;
        Globals::gunChamsRGB = false;
        Globals::handchams = false;
        Globals::handChamsRGB = false;
        Globals::custom_fresnel = false;
        Globals::self_fresnel = false;
        Globals::hand_fresnel = false;
        Globals::rainbow_fresnel = false;

        wasDisabled = true;
    }


    UWorldSave = World;
    menu::input::handle(); //-> crash

    bool hasTarget = false;
    bool hasTarget3 = false;
    bool hasTarget2 = false;

    static int tab = 0;

    static bool firstTime3 = true;
    static bool firstTime5 = true;
    static bool one3 = false;
    static bool one2 = false;
    int target_id = -1;
    float closest_distance = FLT_MAX;

    if (GetAsyncKeyState(VK_INSERT) & 1) {
        Globals::Menu_Open = !Globals::Menu_Open;
    }

    time_ += 0.01f;
    float R = (sin(time_ * 2.0f) + 1.0f) / 2.0f;
    float G = (sin(time_ * 2.0f + 2.0f) + 1.0f) / 2.0f;
    float B = (sin(time_ * 2.0f + 4.0f) + 1.0f) / 2.0f;
    float a = 1.0f;
    if (Globals::Watermark)
    {
        if (Globals::WatermarkType == 0)
        {
            // BASE MODE (shifted right)
            menu::Watermark(FVector2D(70, 50), FVector2D(200, 30), true, 0);
        }
        else if (Globals::WatermarkType == 1)
        {
            // BOX MODE (top-left)
            menu::Watermark(FVector2D(20, 20), FVector2D(200, 30), true, 1);
        }

    }

    //menu::UpdateAnimation();






    constexpr float SIDEBAR_WIDTH = 110.0f;
    constexpr float HEADER_HEIGHT = 45.0f;  // height of title + rainbow
    constexpr float CONTENT_PAD_X = 15.0f;  // margin from the sidebar edge
    constexpr float CONTENT_PAD_Y = 10.0f;  // margin from the header

    if (menu::Window(&G::pos, FVector2D(700, 500), Globals::Menu_Open))
    {
        static int tab = 0;
        // Reset layout offset cleanly after     title
        menu::offset_y = 2;

        menu::SidebarCategory(L"AIMBOT", FVector2D(G::pos.X, G::pos.Y + menu::offset_y), 110);
        menu::offset_y += 20;
        if (menu::ButtonTab2(L"Aimbot", FVector2D(105, 26), tab == 0)) tab = 0;
        menu::offset_y += 14;
        menu::SidebarCategory(L"VISUALS", FVector2D(G::pos.X, G::pos.Y + menu::offset_y), 110);
        menu::offset_y += 20;
        if (menu::ButtonTab2(L"Esp", FVector2D(105, 26), tab == 1)) tab = 1;
        if (menu::ButtonTab2(L"Chams", FVector2D(105, 26), tab == 2)) tab = 2;
        if (menu::ButtonTab2(L"Outlines", FVector2D(105, 26), tab == 3)) tab = 3;
        menu::offset_y += 14;
        menu::SidebarCategory(L"EXTRA", FVector2D(G::pos.X, G::pos.Y + menu::offset_y), 110);
        menu::offset_y += 20;
        if (menu::ButtonTab2(L"Antiaim", FVector2D(105, 26), tab == 5)) tab = 5;
        if (menu::ButtonTab2(L"Exploits", FVector2D(105, 26), tab == 6)) tab = 6;
        if (menu::ButtonTab2(L"Skins", FVector2D(105, 26), tab == 7)) tab = 7;
        if (menu::ButtonTab2(L"CustomStuff", FVector2D(105, 26), tab == 9)) tab = 9;
        menu::SidebarCategory(L"MISC", FVector2D(G::pos.X, G::pos.Y + menu::offset_y), 110);
        menu::offset_y += 20;
        if (menu::ButtonTab2(L"Settings", FVector2D(105, 26), tab == 4)) tab = 4;
        //if (menu::ButtonTab2(L"debugdev", FVector2D(105, 26), tab == 10)) tab = 10;
        if (tab == 0) { SPOOF_CALL(A1M)(); }
        if (tab == 1) { SPOOF_CALL(E3P)(); }
        if (tab == 2) { SPOOF_CALL(W4ALL)(); }
        if (tab == 3) { SPOOF_CALL(OUTLINES)(); }
        if (tab == 4) { SPOOF_CALL(Misc)(); }
        if (tab == 5) { SPOOF_CALL(ANTIAIM)(); }
        if (tab == 6) { SPOOF_CALL(Exploits)(); }
        if (tab == 7) { SPOOF_CALL(SK1NCH4NG3R)(); }
        if (tab == 9) { SPOOF_CALL(CustomShit)(); }
        // if (tab == 10) { SPOOF_CALL(debug)(); }




    }

    // Called from your main loop, frame tick, or wherever logic updates run
    if (Globals::UnlockAll) {
        SPOOF_CALL(tick_unlock_all_logic)(MyShooter, MyPawn);
    }


    static UObject* cached_world1337 = nullptr;
    static std::wstring cached_world1337_name;

    world1337 = Memory::R<UObject*>(std::uintptr_t(_this) + 0x80);

    if (world1337 && world1337 != cached_world1337) {
        cached_world1337 = world1337;
        cached_world1337_name = std::wstring(kismentsystemlibrary::get_object_name(world1337).c_str());

    }

    if (cached_world1337) {
        static const std::wstring target_name = ez(L"MainMenuv2").d();

        if (kismentsystemlibrary::contains(cached_world1337_name.c_str(), target_name.c_str())) {
            if (G::UWorldClass != nullptr)
                G::UWorldClass = nullptr;
        }
    }



    menu::Render();
    menu::Draw_Cursor(Globals::Menu_Open);

    static float font_size = 1.08f;

#define DefusePercentageMax 6.984602
#define CurrentDefuseSectionMax 2
    FLinearColor maincolor{ 1.0f,1.0f,1.0f,1.0f };


  
    static int frameCounter = 0;
    frameCounter++;  // Called every frame

    if (Globals::SpikeEsp && MyShooter && Memory::IsValidPointer((uintptr_t)MyShooter)) {

        TArray<AGameObject*> Objects = GameplayStatics::GetAllActorsOfClass(World, Class::ShooterGameObject());

        for (int index = 0; index < Objects.size(); index++) {
            if (!Objects.is_valid_index(index)) continue;

            AGameObject* Object = Objects[index];
            if (!Object) continue;

            FVector ObjectLocation3D = Object->GetObjectLocation();
            FVector2D ObjectLocation = MyController->ProjectWorldToScreen(ObjectLocation3D);

            if (!ObjectLocation.IsValid() || !ObjectLocation3D.IsValid()) continue;

            std::string ObjectName = kismentsystemlibrary::get_object_name((UObject*)Object).ToString();

            // 🔴 Spike Planted
            if (ObjectName.find("TimedBomb_C") != std::string::npos) {

                double BombTimeRemaining = *(double*)((uintptr_t)Object + Offsets::bomb_time_remaining);
                double DefusePercentage = *(double*)((uintptr_t)Object + Offsets::defuse_percentage) * 100 / 6.984602;

                // Text: Bomb timer
                std::string BombText = "Spike: " + std::to_string((int)BombTimeRemaining) + "s";
                std::wstring WBombText(BombText.begin(), BombText.end());
                FString BombFStr = FString{ WBombText.c_str() };

                // Text: Defuse percentage out of 100
                int DefusePercentInt = (int)DefusePercentage;
                std::string DefuseText = "Defusing: " + std::to_string(DefusePercentInt) + "/100";
                std::wstring WDefuseText(DefuseText.begin(), DefuseText.end());
                FString DefuseFStr = FString{ WDefuseText.c_str() };

                // Choose flashing color when time is low
                FLinearColor FlashColor = RGBtoFLC(255, 255, 255);  // Default white

                if (BombTimeRemaining < 5.0f) {
                    bool flashOn = (frameCounter / 15) % 2 == 0;
                    FlashColor = flashOn ? RGBtoFLC(255, 50, 50) : RGBtoFLC(120, 10, 10);  // Flashing red
                }
                else if (BombTimeRemaining < 10.0f) {
                    FlashColor = RGBtoFLC(255, 165, 0);  // Static orange
                }

                // Draw spike info
                DrawEspText(canvas, BombFStr, ObjectLocation - FVector2D(0, 15), FVector2D(1.15f, 1.15f), FlashColor, 0.75f);

                if (DefusePercentage > 0.0f && DefusePercentage <= 100.0f) {
                    DrawEspText(canvas, DefuseFStr, ObjectLocation + FVector2D(0, 8), FVector2D(1.0f, 1.0f), RGBtoFLC(0, 120, 255), 0.65f);
                }

                // Draw flashing diamond
                FLinearColor FlashColorRaw = {
                    (uint8)(FlashColor.R * 255),
                    (uint8)(FlashColor.G * 255),
                    (uint8)(FlashColor.B * 255),
                    255
                };

                Draw3DDiamond(canvas, MyController, ObjectLocation3D, 60, 70, FlashColor);
            }


            // 🟡 Dropped Spike
            else if (ObjectName.find("X_Bomb") != std::string::npos) {
                FString DroppedStr = FString(L"Spike Not Planted!");
                FVector2D screenCenter(canvas->GetScreenSize().X * 0.5f, 50.0f);
                DrawEspText(canvas, DroppedStr, screenCenter, FVector2D(1.05f, 1.05f), RGBtoFLC(255, 255, 0), 0.7f);
                Draw3DDiamond(canvas, MyController, ObjectLocation3D, 50, 60, { 255, 255, 0, 200 });
            }
        }
    }

    



    if (Globals::dropped_gun) {

        TArray<AGameObject*> Objects = GameplayStatics::GetAllActorsOfClass(World, Class::ShooterGameObject());
        for (int index = 0; index < Objects.size(); index++) {
            if (!Objects.is_valid_index(index)) { continue; }
            AGameObject* Object = Objects[index];

            if (Object == nullptr) { continue; }

            FVector ObjectLocation3D = Object->GetObjectLocation();
            FVector2D ObjectLocation = MyController->ProjectWorldToScreen(Object->GetObjectLocation());
            if (!ObjectLocation.IsValid() || !ObjectLocation3D.IsValid()) { continue; }
            std::string ObjectName = kismentsystemlibrary::get_object_name((UObject*)Object).ToString();

            if (ObjectName.find(("EquippableGroundPickup_C")) != std::string::npos) {
                if (MyShooter->IsAlive()) {
                    Draw3DWeapon(canvas, MyController, ObjectLocation3D, { 255, 255, 255, 255 });
                    canvas->K2_DrawText(
                        FString{ L"Weapon" },
                        ObjectLocation,
                        FVector2D{ font_size, font_size },
                        { 1, 1, 1, 2.5f },
                        false,
                        menu::colors::Text_Shadow,
                        FVector2D{ G::pos.X + 1, G::pos.Y + 1 },
                        true,
                        true,
                        true,
                        { 0, 0, 0, 0.65f }
                    );
                }
            }
        }
    }

    if (Globals::skill_esp && MyShooter && Memory::IsValidPointer((uintptr_t)MyShooter)) {

        TArray<AGameObject*> Objects = GameplayStatics::GetAllActorsOfClass(World, Class::ShooterGameObject());

        for (int index = 0; index < Objects.size(); index++) {
            if (!Objects.is_valid_index(index)) { continue; }
            AGameObject* Object = Objects[index];

            if (Object == nullptr) { continue; }

            FVector2D ObjectLocation = MyController->ProjectWorldToScreen(Object->GetObjectLocation());
            if (!ObjectLocation.IsValid()) { continue; }
            std::string ObjectName = kismentsystemlibrary::get_object_name((UObject*)Object).ToString();

            if (ObjectName.find(("GameObject_Thorne_E_Wall")) != std::string::npos) {
                float ObjectHealth = Object->GetObjectHealth();
                if (ObjectHealth == 0) { continue; }
                std::string Health = std::to_string((int)ObjectHealth) + "HP";
                std::wstring WHealth(Health.begin(), Health.end());
                FString HealthFString = FString{ WHealth.c_str() };
                DrawEspText(
                    canvas,
                    HealthFString,                              // Health info string
                    ObjectLocation,                             // World-to-screen projected position
                    FVector2D(1.15f, 1.15f),                     // Slightly larger scale
                    RGBtoFLC(255, 255, 255),                    // Proper white color
                    0.74f                                       // Shadow opacity
                );

            }

            skills_names(ObjectName, MyShooter, canvas, ObjectLocation);
        }
    }

    if (Globals::EnableAim && Globals::DrawFov) {

        if (Globals::CoolFov)
        {
            rainbowTimeZFov += 0.02f; // Faster update for animated effect
            FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZFov);

            menu::DrawCircleGradient(
                { (float)canvas->GetScreenSize().X / 2, (float)canvas->GetScreenSize().Y / 2 },
                Globals::LegitFOvValue,
                100.0f,
                rainbowTimeZFov
            );
        }
        else
        {
            menu::DrawCircle({ (float)canvas->GetScreenSize().X / 2, (float)canvas->GetScreenSize().Y / 2 }, Globals::LegitFOvValue, 100.0f, Colors::FovColor);
        }


    }
    if (Globals::crosshairrainbow)
    {

        rainbowTimeZCH += 0.005f;
        FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZCH);
        FVector2D ScreenSize;
        ScreenSize = canvas->GetScreenSize();

        float CenterX = ScreenSize.X / 2.0f;
        float CenterY = ScreenSize.Y / 2.0f;

        float Offset = 5.0f; // tiny like TenZ's
        float Thickness = 2.0f; // Line thickness
        //FLinearColor CrosshairColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); // Cyan

        // Draw horizontal line
        canvas->K2_DrawLine(
            FVector2D(CenterX - Offset, CenterY),
            FVector2D(CenterX + Offset, CenterY),
            Thickness,
            RainbowColor
        );

        // Draw vertical line
        canvas->K2_DrawLine(
            FVector2D(CenterX, CenterY - Offset),
            FVector2D(CenterX, CenterY + Offset),
            Thickness,
            RainbowColor
        );
    }
    if (Globals::SkipTutorial) {

        MyController->DisconnectFromServer();
        SPOOF_CALL(Beep)(300, 300);
        Globals::SkipTutorial = false;
    }

    for (int Index = 0; Index < Actors.size(); Index++) {


        if (!Actors.is_valid_index(Index)) { continue; }
        Char = MyController->K2_GetPawn_in();

        Actor = Actors[Index];
        if (Globals::ragemode)
        {
            Rehook(World, MyController);
        }

        if (Globals::finisher && antiflash || Globals::killsounds || Globals::killsay && Actor->IsAlive())
        {
            static shadow_vmt viewport_hook;
            viewport_hook.hook2<decltype(oHkDeath)>(
                VALORANT::Module,                                    // module_base
                (uintptr_t)Actor,                     // address de l'objet viewport
                0x159,                                    // index dans la VMT
                (void*)hk_death_trampoline_hook,        // fonction de hook
                &oHkDeath);
        }






        if (Actor == nullptr || !MyShooter || !Char) { continue; }

        if (auto eq = MyShooter->GetInventory()->GetCurrentWeapon()) {
            FString obj_name = helper::convert_weapon_name(kismentsystemlibrary::get_object_name(eq));
            G::IsKnife = (obj_name.wide() == L"Melee");
        }

        if (Globals::FovChangerBool) {

            FovChanger(MyController);
            //Globals::FovChangerBool = false;
        }

        if (Globals::RecoilCrosshair && !G::IsKnife) {

            float screen_width = canvas->GetScreenSize().X;
            float screen_height = canvas->GetScreenSize().Y;
            if (!hasTarget2) {
                FVector punchAngle = (MyCamera->GetCameraRotation() - MyController->GetControlRotation()).ClampFVector();
                float centerX = screen_width / 2 + (screen_width / 90 * punchAngle.Y * 0.75f);
                float centerY = screen_height / 2 - (screen_height / 90 * punchAngle.X * 0.75f);

                FLinearColor circleColor = { 255, 255, 255, 255 };
                menu::DrawCircle({ centerX, centerY }, 5.25f, 20, circleColor);

                hasTarget2 = true;
            }
        }
        static float spin_yaw = 0.f;




        if (Globals::AntiAFK) {

            MyController->simulate_input_key(keys::space, true);
            MyController->simulate_input_key(keys::space, false);
        }

        if (Globals::BunnyHop && !Globals::AntiAFK && MyController->IsInputKeyDown(keys::space) && Char->CanJump()) {

            MyController->simulate_input_key(keys::space, true);
            MyController->simulate_input_key(keys::space, false);
        }

        if (MyPawn != nullptr) {

            G::MyWeapon = MyShooter->GetInventory()->GetCurrentWeapon();
        }

        /*   if (Globals::EnableSkin && G::MyWeapon != nullptr && MyPawn != nullptr && G::MyWeapon != G::LastWeapon) {

               ApplySkinChanges(G::MyWeapon);
               ChangeKnown(G::MyWeapon);
               G::LastWeapon = MyShooter->GetInventory()->GetCurrentWeapon();
           }
           if (Globals::EnableSkin && GetAsyncKeyState(Globals::SkinChangerKey)) {

               ApplySkinChanges(G::MyWeapon);
               ChangeKnown(G::MyWeapon);
           }*/

        static std::unordered_map<std::wstring, int> lastAppliedSkins;

        // Only process these weapons:
        static const std::unordered_set<std::wstring> allowedWeapons = {
            L"Vandal", L"Phantom", L"Operator", L"Bulldog",
            L"Frenzy", L"Guardian", L"Sheriff", L"Ghost",
            L"Spectre", L"Classic", L"Marshal", L"Knife"
        };

        //if (Globals::autoapply)
        //{
        //    if (Memory::IsValidPointer((uintptr_t)MyShooter))
        //    {
        //        if (auto inventory = MyShooter->GetInventory())
        //        {
        //            if (auto weapon = inventory->GetCurrentWeapon())
        //            {
        //                G::MyWeapon = weapon;

        //                std::string weapon_name = kismentsystemlibrary::get_object_name((UObject*)G::MyWeapon).ToString();

        //                static std::wstring last_weapon_name;
        //                std::wstring weapon_wname = std::wstring(weapon_name.begin(), weapon_name.end());

        //                int selectedSkin = -1;

        //                if (weapon_name.find("AssaultRifle_AK_C") != std::string::npos) selectedSkin = vandalSkinOption;
        //                else if (weapon_name.find("AssaultRifle_ACR_C") != std::string::npos) selectedSkin = phantomSkinOption;
        //                else if (weapon_name.find("BoltSniper_C") != std::string::npos) selectedSkin = operatorSkinOption;
        //                else if (weapon_name.find("AssaultRifle_Burst_C") != std::string::npos) selectedSkin = bulldogSkinOption;
        //                else if (weapon_name.find("AutomaticPistol_C") != std::string::npos) selectedSkin = frenzySkinOption;
        //                else if (weapon_name.find("DMR_C") != std::string::npos) selectedSkin = guardianSkinOption;
        //                else if (weapon_name.find("RevolverPistol_C") != std::string::npos) selectedSkin = sheriffSkinOption;
        //                else if (weapon_name.find("LugerPistol_C") != std::string::npos) selectedSkin = ghostSkinOption;
        //                else if (weapon_name.find("SubMachineGun_MP5_C") != std::string::npos) selectedSkin = spectreSkinOption;
        //                else if (weapon_name.find("BasePistol_C") != std::string::npos) selectedSkin = classicSkinOption;
        //                else if (weapon_name.find("LeverSniperRifle_C") != std::string::npos) selectedSkin = marshalSkinOption;
        //                else if (weapon_name.find("Ability_Melee_Base_C") != std::string::npos) selectedSkin = knifeSkinOption;
        //                else if (weapon_name.find("DS_Gun_C") != std::string::npos) selectedSkin = vandalSkinOption;
        //                else if (weapon_name.find("TrainingBotBasePistol_C") != std::string::npos) selectedSkin = classicSkinOption;
        //                else if (weapon_name.find("SawedOffShotgun_C") != std::string::npos) selectedSkin = bulldogSkinOption;
        //                else if (weapon_name.find("Vector_C") != std::string::npos) selectedSkin = spectreSkinOption;
        //                else if (weapon_name.find("PumpShotgun_C") != std::string::npos) selectedSkin = bulldogSkinOption;
        //                else if (weapon_name.find("AutomaticShotgun_C") != std::string::npos) selectedSkin = bulldogSkinOption;
        //                else if (weapon_name.find("LightMachineGun_C") != std::string::npos) selectedSkin = bulldogSkinOption;
        //                else if (weapon_name.find("HeavyMachineGun_C") != std::string::npos) selectedSkin = bulldogSkinOption;

        //                //// Skip unknown weapons
        //                //if (selectedSkin == -1)
        //                //    return;

        //                bool weaponChanged = (weapon_wname != last_weapon_name);
        //                bool skinChanged = (lastAppliedSkins[weapon_wname] != selectedSkin);

        //                if (weaponChanged || skinChanged)
        //                {
        //                    last_weapon_name = weapon_wname;
        //                    lastAppliedSkins[weapon_wname] = selectedSkin;

        //                    SPOOF_CALL(ApplySkinChanges)(weapon);
        //                    SPOOF_CALL(ChangeKnown)(weapon);

        //                    G::LastWeapon = weapon;
        //                }
        //            }
        //        }
        //    }
        //}


   /*     if (GetAsyncKeyState(VK_DELETE) & 1)
        {


            G::LastWeapon = MyShooter->GetInventory()->GetCurrentWeapon();
        }*/




        if (Globals::custom_fresnel) {
            static FName FresnelIntensity = kismentsystemlibrary::string_to_name(L"Fresnel Intensity");
            TArray<UMaterialInstanceDynamic*> mesh3p_mids = Memory::R<TArray<UMaterialInstanceDynamic*>>(std::uintptr_t(Actor) + Offsets::mesh3p_mids);
            for (int a = 0; a < mesh3p_mids.size(); a++) {
                if (Globals::rainbow_fresnel) {


                    rainbowTimeZRGB += 0.002f;
                    FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZRGB);

                    // Check if the instance exists and then apply the color
                    if (auto instance = mesh3p_mids[a]) {
                        // Set the rainbow color using the decrypted parameter name
                        instance->SetVectorParameterValue(kismentsystemlibrary::Conv_StringToName(enc), FLinearColor(RainbowColor.R* Globals::intesity, RainbowColor.G* Globals::intesity, RainbowColor.B* Globals::intesity)), enc;
                        instance->set_scalar_parameter_value(FresnelIntensity, Globals::Glow1);
                        // Re-encrypt the string (optional)
                        enc;
                    }
                }
                else
                {

                    if (auto instance = mesh3p_mids[a]) {
                        instance->SetVectorParameterValue(kismentsystemlibrary::Conv_StringToName(enc), FLinearColor(fresnel.R * Globals::intesity, fresnel.G * Globals::intesity, fresnel.B * Globals::intesity)), enc;
                        instance->set_scalar_parameter_value(FresnelIntensity, Globals::Glow1);
                    }
                }

            }
        }
        if (Globals::self_fresnel && antiaimenabled) {
            static FName FresnelIntensity = kismentsystemlibrary::string_to_name(L"Fresnel Intensity");
            TArray<UMaterialInstanceDynamic*> mesh3p_mids = Memory::R<TArray<UMaterialInstanceDynamic*>>(std::uintptr_t(MyShooter) + Offsets::mesh3p_mids);
            for (int a = 0; a < mesh3p_mids.size(); a++) {
                if (Globals::rainbow_fresnel) {


                    rainbowTimeZRGB2 += 0.002f;
                    FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZRGB2);

                    // Check if the instance exists and then apply the color
                    if (auto instance = mesh3p_mids[a]) {
                        // Set the rainbow color using the decrypted parameter name
                        instance->SetVectorParameterValue(kismentsystemlibrary::Conv_StringToName(enc2), FLinearColor(RainbowColor.R * Globals::intesity, RainbowColor.G * Globals::intesity, RainbowColor.B * Globals::intesity)), enc;
                        instance->set_scalar_parameter_value(FresnelIntensity,Globals::Glow1) ;
                        // Re-encrypt the string (optional)
                        enc2;
                    }
                }
                else
                {

                    if (auto instance = mesh3p_mids[a]) {
                        instance->SetVectorParameterValue(kismentsystemlibrary::Conv_StringToName(enc2), FLinearColor(fresnel.R * Globals::intesity, fresnel.G * Globals::intesity, fresnel.B * Globals::intesity)), enc;
                        instance->set_scalar_parameter_value(FresnelIntensity, Globals::Glow1);
                    }
                }

            }
        }
        //if (Globals::hand_fresnel) {

        //    TArray<UMaterialInstanceDynamic*> mesh1p_mids = Memory::R<TArray<UMaterialInstanceDynamic*>>(std::uintptr_t(MyShooter) + 0xf78);
        //    for (int a = 0; a < mesh1p_mids.size(); a++) {
        //        if (Globals::rainbow_fresnel) {


        //            rainbowTimeZRGB3 += 0.002f;
        //            FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZRGB3);

        //            // Check if the instance exists and then apply the color
        //            if (auto instance = mesh1p_mids[a]) {
        //                // Set the rainbow color using the decrypted parameter name
        //                instance->SetVectorParameterValue(kismentsystemlibrary::Conv_StringToName(enc2), FLinearColor(RainbowColor.R * Globals::Glow1, RainbowColor.G * Globals::Glow1, RainbowColor.B * Globals::Glow1)), enc2;

        //                // Re-encrypt the string (optional)
        //                enc2;
        //            }
        //        }
        //        else
        //        {

        //            if (auto instance = mesh1p_mids[a]) {
        //                instance->SetVectorParameterValue(kismentsystemlibrary::Conv_StringToName(enc2), FLinearColor(fresnel.R * Globals::Glow1, fresnel.G * Globals::Glow1, fresnel.B * Globals::Glow1)), enc2;
        //            }
        //        }

        //    }
        //}

        if (Globals::hand_fresnel)
        {
            auto negro = MyShooter->GetPawnMesh();
            if (negro)
            {
                auto negro2 = MyShooter->GetMesh1P();
                if (negro2)
                {
                    fresnel_handchams(MyPawn, MyShooter);
                }
            }
            
        }
        APawn* pawn = MyController->get_acknowledged_pawn();
        Mesh = Actor->GetPawnMesh2();
        AAresEquippable* Equippable = Actor->GetInventory()->GetCurrentWeapon();

        if (!Mesh || !Equippable) { continue; }
        static bool thirdperson_enabled = Globals::Thirdperson;
        static bool last_key_state = false;

        bool current_key_state = GetAsyncKeyState(Globals::tpkey); // 'T' key
        if (current_key_state && !last_key_state) {
            thirdperson_enabled = !thirdperson_enabled;
            Globals::Thirdperson = thirdperson_enabled;
        }
        last_key_state = current_key_state;


        static bool thirdperson_enabled2 = Globals::SpinBot;
        static bool last_key_state2 = false;

        bool current_key_state2 = GetAsyncKeyState(Globals::aakey); // 'T' key
        if (current_key_state2 && !last_key_state2) {
            thirdperson_enabled2 = !thirdperson_enabled2;
            Globals::SpinBot = thirdperson_enabled2;
        }
        last_key_state2 = current_key_state2;
        /*     if (Globals::HandGlow && MyShooter->IsAlive() && MyShooter->GetHealth() > 0 && MyShooter && Memory::IsValidPointer((uintptr_t)MyShooter))
             {
                 if (auto handmesh = MyShooter->GetOverlayMesh1P()) {
                     meshp1_material(MyPawn, Actor);
                 }
             }*/








        if (Globals::handchams && MyShooter->IsAlive())
        {
            if (auto handmesh = MyShooter->GetOverlayMesh1P())
            {
                meshp1_material1(MyPawn, Actor);
            }
        }
           /* if (Globals::galaxychams && MyShooter->IsAlive())
            {
                if (auto handmesh = MyShooter->GetOverlayMesh1P())
                {
                    galaxychams(MyPawn, Actor);
                }

            }*/
        if (Globals::gunmaterial && MyShooter->IsAlive())
        {
            if (auto inventory = MyShooter->GetInventory())
            {
                if (auto currentWeapon = inventory->GetCurrentWeapon())
                {
                    if (auto weaponMesh1P = currentWeapon->GetMesh1P())
                    {
                        UPrimitiveComponent* gunChams = Memory::R<UPrimitiveComponent*>(uintptr_t(currentWeapon) + Offsets::mesh1pgun);
                  

                        UPrimitiveComponent* gunChams3P = Memory::R<UPrimitiveComponent*>(uintptr_t(currentWeapon) + Offsets::Mesh3pGun);
                     
                       // USkeletalMeshComponent* Gunchams = currentWeapon->GetEquippableMesh();
                        static UObject* material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI");
                        static FName baseColorName = kismentsystemlibrary::string_to_name(L"Base Color");
                        static FName emissiveColorName = kismentsystemlibrary::string_to_name(L"Emissive Color");

                        // Determine color
                        FLinearColor color;
                        if (Globals::gunChamsRGB) {
                            rainbowTimeZHandChams += 0.002f;
                            color = BOSSRAINBOMAW(rainbowTimeZHandChams);
                        }
                        else {
                            color = FLinearColor{ Globals::guncolor.R, Globals::guncolor.G, Globals::guncolor.B };
                        }
                        color = FLinearColor{
                            color.R * 8,
                            color.G * 8,
                            color.B * 8,
                            color.A
                        };

                        // Apply material
                        int numMaterials = gunChams->GetNumMaterials();
                        for (int i = 0; i < numMaterials; ++i)
                        {
                            if (auto dynMat = gunChams->create_and_set_material_instance_dynamic_from_material(i, material))
                            {
                                auto matInst = dynMat->cast<UMaterialInstanceDynamic>();
                                if (!matInst) continue;

                                matInst->set_vector_parameter_value(baseColorName, color);
                                matInst->set_vector_parameter_value(emissiveColorName, color);
                            }
                        }
                        if (Globals::Thirdperson)
                        {
                            int numMaterials2 = gunChams3P->GetNumMaterials();
                            for (int i = 0; i < numMaterials2; ++i)
                            {
                                if (auto dynMat2 = gunChams3P->create_and_set_material_instance_dynamic_from_material(i, material))
                                {
                                    auto matInst2 = dynMat2->cast<UMaterialInstanceDynamic>();
                                    if (!matInst2) continue;

                                    matInst2->set_vector_parameter_value(baseColorName, color);
                                    matInst2->set_vector_parameter_value(emissiveColorName, color);
                                }
                            }
                        }
                    }

                }
            }
        }






      


        if (Globals::Thirdperson)
        {
            if (MyShooter)
            {
                MyShooter->Set3pMeshVisible(true);
            }
           

        }
        else
        {


            if (MyShooter)
            {
                MyShooter->Set3pMeshVisible(false);
            }


        }


        if (Globals::WireFrameHand) {


            if (auto handmesh = MyShooter->GetOverlayMesh1P()) {
                constexpr uint8_t WireframeFlag = 1 << 5;
                uint8_t* HandmeshFlags = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(handmesh) + Offsets::bForceWireframe);

                bool alreadyAddedWireframe = (*HandmeshFlags & WireframeFlag) != 0;
                //SPOOF_CALL(meshp1_material)(MyPawn, Actor);

                if (!alreadyAddedWireframe) {
                    auto mesh1poverlay = Memory::R<uintptr_t>(reinterpret_cast<uintptr_t>(handmesh) + Offsets::bForceWireframe);
                    if (mesh1poverlay) {
                        *HandmeshFlags |= WireframeFlag;
                        *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(handmesh) + Offsets::wireframe_num_1) = 0xff;
                    }
                }
            }
        }

        if (Globals::OwnWireframe && MyShooter->IsAlive()) {

            if (auto inventory = MyShooter->GetInventory()) {
                if (auto weapon = inventory->GetCurrentWeapon()) {
                    if (auto weapon_mesh_1p = weapon->GetEquippableMesh()) {
                        USkeletalMeshComponent* myselfchams = Memory::R<USkeletalMeshComponent*>(uintptr_t(pawn) + Offsets::thirdpersoncosmetic);

                        if (weapon_mesh_1p) {
                            constexpr uint8_t WireframeFlag = 1 << 5;
                            uint8_t* MeshFlags = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(myselfchams) + Offsets::bForceWireframe);
                            uint8_t* MeshColor = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(myselfchams) + Offsets::wireframe_num_1);

                            *MeshFlags |= WireframeFlag;
                            *MeshColor = 0xff;
                        }
                    }
                }
            }
        }
        if (Globals::WireFrameWeapon) {

            if (auto inventory = MyShooter->GetInventory()) {
                if (auto weapon = inventory->GetCurrentWeapon()) {
                    if (auto weapon_mesh_1p = weapon->GetMesh1P()) {
                        if (weapon_mesh_1p) {
                            constexpr uint8_t WireframeFlag = 1 << 5;
                            uint8_t* MeshFlags = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(weapon_mesh_1p) + Offsets::bForceWireframe);
                            uint8_t* MeshColor = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(weapon_mesh_1p) + Offsets::wireframe_num_1);

                            *MeshFlags |= WireframeFlag;
                            *MeshColor = 0xff;
                        }
                    }
                }
            }
        }

        if (Globals::WireFrameWeapon) {
            if (auto inventory = MyShooter->GetInventory()) {
                if (auto weapon = inventory->GetCurrentWeapon()) {
                    if (auto weapon_mesh_1p = weapon->GetEquippableMesh()) {
                        constexpr uint8_t WireframeFlag = 1 << 5;

                        uint8_t* MeshFlags = Memory::R<uint8_t*>(
                            uintptr_t(weapon_mesh_1p) + Offsets::bForceWireframe);
                        *MeshFlags |= WireframeFlag;

                        Memory::W<uint8_t>(
                            uintptr_t(weapon_mesh_1p) + Offsets::wireframe_num_1, 0xFF);
                    }
                }
            }
        }


        if (!Globals::TeamCheck) {

            if (BaseTeamComponent::IsAlly(Actor, MyShooter)) { continue; }
        }

        if (!Actor->IsAlive()) { continue; }
        if (Actor->IsDeadAndSettled()) { continue; }
        if (!Actor->IsDormant()) { continue; }

        auto Head = GetBoneMatrix(Mesh, Bones::head);
        auto Base = GetBoneMatrix(Mesh, Bones::base);
        auto CestBone = GetBoneMatrix(Mesh, 6);

        auto HeadOut = MyController->ProjectWorldToScreen(Head);
        auto HeadLongOut = MyController->ProjectWorldToScreen(FVector(Head.X, Head.Y, Head.Z + 15));
        auto HeadLongOut2 = MyController->ProjectWorldToScreen(FVector(Head.X, Head.Y, Head.Z + 17.5f));
        auto BaseOut = MyController->ProjectWorldToScreen(Base);
        auto Cest = MyController->ProjectWorldToScreen(CestBone);
        state = Actor->GetplayerState();
        if (!HeadOut.IsValid() || !BaseOut.IsValid())
            continue;

        float BoxHeight = abs(HeadLongOut.Y - BaseOut.Y);
        float BoxWidth = BoxHeight * 0.55;

        float Health = Actor->GetHealth();
        float Shield = Actor->GetShield();

        auto RelativeLocation = Actor->K2_GetActorLocation();
        auto MyShooterRelativeLocation = MyShooter->K2_GetActorLocation();
        auto distance2 = MyShooterRelativeLocation.Distance(RelativeLocation);
        auto distance = distance2 / 5;

        if (Health <= 0) { continue; }

        FString ObjectName = kismentsystemlibrary::get_object_name(reinterpret_cast<UObject*>(Actor));
        FString WeaponName = kismentsystemlibrary::get_object_name(reinterpret_cast<UObject*>(Equippable));

        bool IsAlive = Actor->IsAlive();

        bool IsDormant = Dormant(Actor);
        if (!IsDormant) continue;

        bool IsDormantServer = DormantServer(Actor);
        if (!IsDormantServer) continue;

        if (Health <= 100 && Health > 0) {

            if (Mesh && IsAlive) {

                if (Health >= 75) { health::HealthColor = health::HighHealth; }
                else if (Health <= 74 && Health >= 44) { health::HealthColor = health::NormalHealth; }
                else if (Health <= 45) { health::HealthColor = health::LowHealth; }
                FVector eyeLocation;
                FRotator eyeRotation;
                MyShooter->GetActorEyesViewPoint(&eyeLocation, &eyeRotation);
             
               
                    bool bVisible = !Globals::VisibleCheck || (
                       MyController->LineOfSightTo2(Actor)
                        );
                
  
                bool bCanPenetrate = false;

                if (!bVisible && Globals::auto_wall) {
                    // Perform autowall penetration check for ESP coloring
                    AutoWallSystem::WallPenetrationResult PenResult = AutoWallSystem::CalculateWallPenetration(
                        MyShooter, Actor,
                        GetBoneMatrix(Mesh, Globals::AimBone == 0 ? 8 : (Globals::AimBone == 1 ? 6 : 3)), // Use head bone or any other
                        0 // Bone index (0 = Head, 1 = Neck, etc.)
                    );

                    bCanPenetrate = PenResult.CanPenetrate;
                }

                if (bVisible || bCanPenetrate) {
                    Colors::LineColor = Colors::LineVisible;
                    Colors::BoxColor = Colors::BoxVisible;
                    Colors::Skeleton = Colors::SkeletonVisible;
                    Colors::ChamsColor1 = Colors::ChamsVisible;
                }
                else {
                    Colors::BoxColor = Colors::Boxinvisible;
                    Colors::LineColor = Colors::LineINvisible;
                    Colors::Skeleton = Colors::SkeletonInvisible;
                    Colors::ChamsColor1 = Colors::ChamsInvisible;
                }
                FVector2D HeadPos;

                if (!MyController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 8), HeadPos, 0) || !HeadPos.IsValid()) {
                    return;
                }

                FVector2D FootPos;
                if (!MyController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 0), FootPos, 0) || !FootPos.IsValid()) {
                    return;
                }
                if (Globals::EnableESP && Health <= 100) {
                    if (Globals::DistangeEsp) {
                        FVector2D HeadPos;

                        if (MyController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 8), HeadPos, 0) && HeadPos.IsValid()) {
                            char buffer[32];
                            sprintf_s(buffer, ez("[%.fM]").d(), distance);
                            if (distance < 38) {
                                if (Globals::WeaponEsp) {
                                    canvas->K2_DrawText(s2wc(buffer), FVector2D(BaseOut.X, BaseOut.Y + 35),
                                        FVector2D{ 0.8f, 0.8f }, { 1, 1, 1, 2.5f }, false, menu::colors::Text_Shadow,
                                        FVector2D{ G::pos.X + 1, G::pos.Y + 1 }, true, true, false, { 0, 0, 0, 0.49 });
                                }
                                else {
                                    canvas->K2_DrawText(s2wc(buffer), FVector2D(BaseOut.X, BaseOut.Y + 25),
                                        FVector2D{ 0.8f, 0.8f }, { 1, 1, 1, 2.5f }, false, menu::colors::Text_Shadow,
                                        FVector2D{ G::pos.X + 1, G::pos.Y + 1 }, true, true, false, { 0, 0, 0, 0.49 });
                                }
                            }
                        }
                    }



                    
                    if (Globals::ClientSideSpinbot)
                    {
                        FVector2D name_position = { HeadOut.X, HeadOut.Y - 30.5f };
                        uintptr_t playerStatePtr = Memory::R<uintptr_t>((uintptr_t)Actor + Offsets::PlayerState);
                        if (playerStatePtr)
                        {



                            uint16_t ping = Memory::R<uint16_t>(playerStatePtr + Offsets::get_ping);
                            //uintptr_t platformPlayerPtr = Memory::R<uintptr_t>(playerStatePtr + Offsets::PlatformPlayer);


                            //if (platformPlayerPtr)
                            {

                               /* int32_t competitiveTier = Memory::R<int32_t>(platformPlayerPtr + Offsets::CompetetiveTier);
                                int32_t accountLevel = Memory::R<int32_t>(platformPlayerPtr + Offsets::AccountLevel);
                                int32_t leaderboardPos = Memory::R<int32_t>(platformPlayerPtr + 0x6a4);*/


                                const wchar_t* nameToDraw = nullptr;
                                if (state) {
                                    auto name = state->GetPlayerName();
                                    if (name.is_valid())
                                        nameToDraw = name.c_str();
                                }
                                if (!nameToDraw)
                                    nameToDraw = ez(L"BOT").d();


                                const wchar_t* rankName = L"[ Unranked ]";
                                FLinearColor rankColor = { 0.71f, 0.71f, 0.71f, 1.0f };


                               /* switch (static_cast<ECompetitiveTier>(competitiveTier)) {
                                case ECompetitiveTier::Iron1: rankName = L"[ Iron 1 ]"; rankColor = { 0.43f, 0.43f, 0.43f, 1.0f }; break;
                                case ECompetitiveTier::Iron2: rankName = L"[ Iron 2 ]"; rankColor = { 0.43f, 0.43f, 0.43f, 1.0f }; break;
                                case ECompetitiveTier::Iron3: rankName = L"[ Iron 3 ]"; rankColor = { 0.43f, 0.43f, 0.43f, 1.0f }; break;
                                case ECompetitiveTier::Bronze1: rankName = L"[ Bronze 1 ]"; rankColor = { 0.78f, 0.49f, 0.24f, 1.0f }; break;
                                case ECompetitiveTier::Bronze2: rankName = L"[ Bronze 2 ]"; rankColor = { 0.78f, 0.49f, 0.24f, 1.0f }; break;
                                case ECompetitiveTier::Bronze3: rankName = L"[ Bronze 3 ]"; rankColor = { 0.78f, 0.49f, 0.24f, 1.0f }; break;
                                case ECompetitiveTier::Silver1: rankName = L"[ Silver 1 ]"; rankColor = { 0.71f, 0.71f, 0.78f, 1.0f }; break;
                                case ECompetitiveTier::Silver2: rankName = L"[ Silver 2 ]"; rankColor = { 0.71f, 0.71f, 0.78f, 1.0f }; break;
                                case ECompetitiveTier::Silver3: rankName = L"[ Silver 3 ]"; rankColor = { 0.71f, 0.71f, 0.78f, 1.0f }; break;
                                case ECompetitiveTier::Gold1: rankName = L"[ Gold 1 ]"; rankColor = { 1.0f, 0.84f, 0.0f, 1.0f }; break;
                                case ECompetitiveTier::Gold2: rankName = L"[ Gold 2 ]"; rankColor = { 1.0f, 0.84f, 0.0f, 1.0f }; break;
                                case ECompetitiveTier::Gold3: rankName = L"[ Gold 3 ]"; rankColor = { 1.0f, 0.84f, 0.0f, 1.0f }; break;
                                case ECompetitiveTier::Platinum1: rankName = L"[ Platinum 1 ]"; rankColor = { 0.33f, 0.86f, 0.75f, 1.0f }; break;
                                case ECompetitiveTier::Platinum2: rankName = L"[ Platinum 2 ]"; rankColor = { 0.33f, 0.86f, 0.75f, 1.0f }; break;
                                case ECompetitiveTier::Platinum3: rankName = L"[ Platinum 3 ]"; rankColor = { 0.33f, 0.86f, 0.75f, 1.0f }; break;
                                case ECompetitiveTier::Diamond1: rankName = L"[ Diamond 1 ]"; rankColor = { 0.45f, 0.39f, 1.0f, 1.0f }; break;
                                case ECompetitiveTier::Diamond2: rankName = L"[ Diamond 2 ]"; rankColor = { 0.45f, 0.39f, 1.0f, 1.0f }; break;
                                case ECompetitiveTier::Diamond3: rankName = L"[ Diamond 3 ]"; rankColor = { 0.45f, 0.39f, 1.0f, 1.0f }; break;
                                case ECompetitiveTier::Immortal1: rankName = L"[ Immortal 1 ]"; rankColor = { 0.67f, 0.18f, 0.35f, 1.0f }; break;
                                case ECompetitiveTier::Immortal2: rankName = L"[ Immortal 2 ]"; rankColor = { 0.67f, 0.18f, 0.35f, 1.0f }; break;
                                case ECompetitiveTier::Immortal3: rankName = L"[ Immortal 3 ]"; rankColor = { 0.67f, 0.18f, 0.35f, 1.0f }; break;
                                case ECompetitiveTier::Radiant: rankName = L"[ Radiant ]"; rankColor = { 1.0f, 0.78f, 0.04f, 1.0f }; break;
                                }*/


                                float yOffset = 0.0f;
                                float scaleFactor = 0.85f;


                                if (distance < 38.0f) {
                                    scaleFactor = 0.85f * (distance / 38.0f);
                                    if (scaleFactor < 0.55f) scaleFactor = 0.55f;
                                }


                                // Draw Name
                                if (Globals::ShowPlayerName) {
                                    canvas->K2_DrawText(
                                        nameToDraw,
                                        name_position + FVector2D(0, yOffset),
                                        FVector2D(scaleFactor, scaleFactor),
                                        rankColor,
                                        0.0f,
                                        RGBtoFLC(0, 0, 0),
                                        FVector2D(0, 0),
                                        false, false, false,
                                        RGBtoFLC(0, 0, 0)
                                    );
                                    yOffset += 15.0f * scaleFactor;
                                }


                                //// Draw Rank
                                //if (Globals::ShowRank) {
                                //    canvas->K2_DrawText(
                                //        rankName,
                                //        name_position + FVector2D(0, yOffset),
                                //        FVector2D(scaleFactor, scaleFactor),
                                //        rankColor,
                                //        0.0f,
                                //        RGBtoFLC(0, 0, 0),
                                //        FVector2D(0, 0),
                                //        false, false, false,
                                //        RGBtoFLC(0, 0, 0)
                                //    );
                                //    yOffset += 15.0f * scaleFactor;
                                //}


                                //// Draw Account Level
                                //if (Globals::ShowLevel && accountLevel > 0) {
                                //    std::wstring levelInfo = L"Level: " + std::to_wstring(accountLevel);
                                //    canvas->K2_DrawText(
                                //        levelInfo.c_str(),
                                //        name_position + FVector2D(0, yOffset),
                                //        FVector2D(scaleFactor, scaleFactor),
                                //        rankColor,
                                //        0.0f,
                                //        RGBtoFLC(0, 0, 0),
                                //        FVector2D(0, 0),
                                //        false, false, false,
                                //        RGBtoFLC(0, 0, 0)
                                //    );
                                //    yOffset += 15.0f * scaleFactor;
                                //}


                                // Draw Ping
                                if (Globals::ShowPing) {
                                    std::wstring pingInfo = L"Ping: " + std::to_wstring(ping) + L"ms";
                                    canvas->K2_DrawText(
                                        pingInfo.c_str(),
                                        name_position + FVector2D(0, yOffset),
                                        FVector2D(scaleFactor, scaleFactor),
                                        rankColor,
                                        0.0f,
                                        RGBtoFLC(0, 0, 0),
                                        FVector2D(0, 0),    
                                        false, false, false,
                                        RGBtoFLC(0, 0, 0)
                                    );
                                    yOffset += 15.0f * scaleFactor;
                                }


                                // Draw Leaderboard Position
                                /*if (Globals::DrawLeaderboard && leaderboardPos > 0) {
                                    std::wstring lbInfo = L"Leaderboard Rank: #" + std::to_wstring(leaderboardPos);
                                    canvas->K2_DrawText(
                                        lbInfo.c_str(),
                                        name_position + FVector2D(0, yOffset),
                                        FVector2D(scaleFactor, scaleFactor),
                                        rankColor,
                                        0.0f,
                                        RGBtoFLC(0, 0, 0),
                                        FVector2D(0, 0),
                                        false, false, false,
                                        RGBtoFLC(0, 0, 0)
                                    );

                                }*/
                            }
                        }
                    }



                    if (Globals::SkeletonEsp && Actor->IsAlive()) {

                        int32_t bone_count = Memory::R<int32_t>((uintptr_t)Mesh + Offsets::bone_cout);
                        if (!bone_count)
                            return;
                        SPOOF_CALL(DrawSkeleton)(MyController, Mesh, bone_count, canvas);
                    }

                    if (Globals::CorneredBox && Actor->IsAlive()) {

                        SPOOF_CALL(DrawCorner)(canvas, MyController, Mesh, Colors::BoxColor, Globals::ESPThickness);
                    }
                    if (Globals::Box3D && Actor->IsAlive()) {

                        SPOOF_CALL(Draw3D)(canvas, MyController, Mesh, Colors::BoxColor);
                    }
                    if (Globals::Box2D) {
                        if (Globals::boxoutline)
                        {
                            SPOOF_CALL(Draw2DBox)(canvas, MyController, Mesh, Colors::BoxColor,true);
                        }
                        else
                        {
                            SPOOF_CALL(Draw2DBox)(canvas, MyController, Mesh, Colors::BoxColor,false);
                        }
                    }
                    if (Globals::targetline) {
                        auto Head = GetBoneMatrix(Mesh, Bones::head);
                        SPOOF_CALL(TargetLine)(canvas, MyController, Mesh, Head);
                    }
                    if (Globals::Snapline && !ObjectName.IsValid() && Actor->IsAlive()) {

                        if (Globals::SelectSnapline == 0) {
                            SPOOF_CALL(DrawLineBottomLocation)(canvas, MyController, Mesh);
                        }
                        else if (Globals::SelectSnapline == 1) {
                            SPOOF_CALL(DrawSnapline)(canvas, MyShooter, HeadOut, Colors::LineColor, Globals::ESPThickness);
                        }
                        else if (Globals::SelectSnapline == 2) {
                            SPOOF_CALL(DrawLineTopLocation)(canvas, MyController, Mesh);
                        }
                    }



                    if (Globals::Healthbar && Health <= 100 && distance >= 1.3f) {

                        SPOOF_CALL(DrawHealthAndShieldBars)(canvas,MyController, Mesh, Health, Shield);
                    }
                    if (Globals::HealthText && Health <= 100 && distance >= 1.3f)
                    {


                        const int boxHeight = abs(FootPos.Y - HeadPos.Y);
                        const int boxWidth = boxHeight * 0.45f;

                        const int barHeight = boxHeight;
                        const int barThickness = 2;  // Thinner bars
                        const int barOffset = 5;

                        const int barX_Health = HeadPos.X - (boxWidth * 0.5f) - barOffset - barThickness;
                        const int barX_Shield = barX_Health - barThickness - 2;
                        const int barY = HeadPos.Y;

                        const int healthHeight = FMath::Clamp(int((Health / 100.0f) * barHeight), 0, barHeight);
                        const int shieldHeight = FMath::Clamp(int((Shield / 50.0f) * barHeight), 0, barHeight);

                        const FLinearColor outlineColor = FLinearColor(0, 0, 0, 0.5f); // Lower opacity outline

                        FLinearColor healthColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // green
                        if (Health <= 25)
                            healthColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // red
                        else if (Health <= 50)
                            healthColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); // yellow

                        FLinearColor shieldColor = FLinearColor(0.25f, 0.6f, 1.0f, 1.0f); // light blue
                        // Calculate total health + shield
                        float totalHealth = Health + Shield;
                        //const float textScale = FMath::Clamp(0.85f / (distance / 15.0f), 0.5f, 0.85f);  // Scale text size based on distance

                        // Draw the combined health + shield text
                        wchar_t healthTextBuffer[8];
                        swprintf_s(healthTextBuffer, (L"%d"), static_cast<int>(totalHealth)); // Use combined total health/shield value
                        FString healthText(healthTextBuffer);
                        int healthTextX = barX_Health + (barThickness / 2) - (healthText.size() * 4); // Adjust for text length (approximate width)
                        int shieldTextX = barX_Shield + (barThickness / 2) - (healthText.size() * 4); // Similarly for shield
                        int textCenterY = barY + (barHeight / 2) - 10; // Slightly adjust Y for centering
                        DrawEspText(
                            canvas,
                            healthText,                                      // FString with health info
                            FVector2D(healthTextX, textCenterY),             // Center of health bar
                            FVector2D(1, 1),                 // Dynamic scale
                            RGBtoFLC(255, 255, 255),                         // White text
                            0.75f                                            // Shadow opacity
                        );

                    }


                    if (Globals::WeaponEsp && Actor->IsAlive()) {

                        DrawEspText(
                            canvas,
                            Filters::Names::WeaponName(WeaponName),           // Display weapon name
                            FVector2D(BaseOut.X, BaseOut.Y + 20),              // Slightly below the player
                            FVector2D(0.8f, 0.8f),                             // Smaller scale
                            Colors::WeaponNameColor,                           // Your defined color for weapons
                            0.75f                                              // Shadow opacity
                        );

                    }

                    if (Globals::AgentName && !ObjectName.IsValid() && Actor->IsAlive()) {




                        DrawAgentNameEsp(canvas, Actor, FVector2D(HeadLongOut.X, HeadLongOut.Y - 70), Mesh, MyController, BoxWidth, distance);


                    }


                    //if (Globals::riotid && Actor->IsAlive())
                    //{
                    //    FVector2D textPos = { HeadLongOut.X, HeadLongOut.Y - 40.f };

                    //    // Read PlayerState
                    //    uintptr_t playerState = Memory::R<uintptr_t>((uintptr_t)Actor + Offsets::PlayerState);
                    //    if (!playerState) return;

                    //    // Get PlatformPlayer UObject
                    //    UObject* platformPlayer = Memory::R<UObject*>((uintptr_t)playerState + Offsets::PlatformPlayer);
                    //    if (!platformPlayer) return;

                    //    // Setup function pointer (no UFunction needed)
                    //    using GetTrueDisplayNameFn = void(*)(UObject*, void*);
                    //    GetTrueDisplayNameFn CallGetName = (GetTrueDisplayNameFn)(0x44C8C50); // Make sure this is an absolute address

                    //    struct {
                    //        FString ReturnValue;
                    //    } Params;

                    //    // Call the function directly
                    //    CallGetName(platformPlayer, &Params);

                    //    // Convert and draw
                    //    std::string riotNameStr = Params.ReturnValue.ToString();
                    //    std::wstring wideName;

                    //    if (!riotNameStr.empty())
                    //    {
                    //        wideName = std::wstring(riotNameStr.begin(), riotNameStr.end());
                    //    }
                    //    else
                    //    {
                    //        wideName = L"BOT";
                    //    }

                    //    FString displayName(wideName.c_str());
                    //    DrawEspText(canvas, displayName, textPos, FVector2D(1.f, 1.f), FLinearColor(1.f, 1.f, 1.f, 1.f), 0.75f);
                    //}











                    if (Globals::party_hat && Actor->IsAlive()) {
                        auto Head = GetBoneMatrix(Mesh, Bones::head);
                        Mesh = Actor->GetPawnMesh();
                        SPOOF_CALL(partyhat)(MyController, Mesh, canvas, Head);
                    }
          
                    if (Globals::AgentIcon) {
                        float yPos = g_Options.healthbar || g_Options.AgentNameESP ?
                            (distance <= 10 ? 115 : (distance >= 20 ? 200 : 163)) : // Lowered yPos if distance >= 24
                            (g_Options.healthbar ? 85 : 20);

                        auto HeadLongOut4 = MyController->ProjectWorldToScreen(FVector(Base.X - 10.0f, Base.Y, Base.Z - yPos));
                        SPOOF_CALL(EnemyIcon)(canvas, Actor, FVector2D(HeadLongOut4.X - 6.0f, HeadLongOut4.Y), distance);
                        // CustomIcon(canvas, Actor, FVector2D(HeadLongOut.X - 6.0f, HeadLongOut.Y), distance);
                    }










                    if (Globals::ChamsESP && Actor->IsAlive()) {

                        if (!Globals::outline) {
                            using SetAresOutlineModeFunc = bool(__fastcall*)(USkeletalMeshComponent*, int, bool);
                            USkeletalMeshComponent* myselfchams = Memory::R<USkeletalMeshComponent*>(uintptr_t(pawn) + Offsets::thirdpersoncosmetic);
                            SetAresOutlineModeFunc outlineFunc = reinterpret_cast<SetAresOutlineModeFunc>(VALORANT::Module + Offsets::set_ares_outline);
                            bool isCharacterAlive = outlineFunc(Mesh, 4, true);

                        }
                        else if (Globals::outline && Actor->IsAlive()) {




                            reinterpret_cast<USkeletalMeshComponent* (__fastcall*)(USkeletalMeshComponent*, int, bool)>(VALORANT::Module + Offsets::set_ares_outline)(Mesh, 4, true);


                            if (Globals::rainbow_chams)
                            {
                                rainbowTimeZChams += 0.014f;
                                FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZChams);

                                AresOutlineRendering::SetOutlineColorsForRender(World, { RainbowColor.R * Globals::glowenemyfloat, RainbowColor.G * Globals::glowenemyfloat, RainbowColor.B * Globals::glowenemyfloat ,RainbowColor.A * Globals::glowenemyfloat }, { RainbowColor.R * Globals::glowenemyfloat, RainbowColor.G * Globals::glowenemyfloat, RainbowColor.B * Globals::glowenemyfloat, RainbowColor.A * Globals::glowenemyfloat });
                            }


                        }
                        else {

                            Mesh->SetOutlineMode(EAresOutlineMode::None);
                        }
                    }
                    if (Globals::usepresetedoutlines)
                    {
                        // VISIBLE PRESETS
                        if (Globals::visiblepreset == 0) // Default Green
                        {
                            Globals::CenterEdgeR_Visible = 0.0f;
                            Globals::CenterEdgeG_Visible = 0.01f;
                            Globals::CenterEdgeB_Visible = 0.0f;

                            Globals::InnerEdgeR_Visible = 0.0f;
                            Globals::InnerEdgeG_Visible = 0.1f;
                            Globals::InnerEdgeB_Visible = 0.0f;

                            Globals::OuterEdgeR_Visible = 0.0f;
                            Globals::OuterEdgeG_Visible = 1.0f;
                            Globals::OuterEdgeB_Visible = 0.0f;
                        }
                        else if (Globals::visiblepreset == 1) // Toxic Glow
                        {
                            Globals::CenterEdgeR_Visible = 0.0f;
                            Globals::CenterEdgeG_Visible = 0.4f;
                            Globals::CenterEdgeB_Visible = 0.2f;

                            Globals::InnerEdgeR_Visible = 0.0f;
                            Globals::InnerEdgeG_Visible = 0.7f;
                            Globals::InnerEdgeB_Visible = 0.3f;

                            Globals::OuterEdgeR_Visible = 0.1f;
                            Globals::OuterEdgeG_Visible = 1.0f;
                            Globals::OuterEdgeB_Visible = 0.4f;
                        }
                        else if (Globals::visiblepreset == 2) // Neon Ice
                        {
                            Globals::CenterEdgeR_Visible = 0.0f;
                            Globals::CenterEdgeG_Visible = 0.8f;
                            Globals::CenterEdgeB_Visible = 1.0f;

                            Globals::InnerEdgeR_Visible = 0.0f;
                            Globals::InnerEdgeG_Visible = 0.6f;
                            Globals::InnerEdgeB_Visible = 1.0f;

                            Globals::OuterEdgeR_Visible = 0.1f;
                            Globals::OuterEdgeG_Visible = 1.0f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                        }
                        else if (Globals::visiblepreset == 3) // Solar Flare
                        {
                            Globals::CenterEdgeR_Visible = 0.6f;
                            Globals::CenterEdgeG_Visible = 0.5f;
                            Globals::CenterEdgeB_Visible = 0.0f;

                            Globals::InnerEdgeR_Visible = 0.9f;
                            Globals::InnerEdgeG_Visible = 0.8f;
                            Globals::InnerEdgeB_Visible = 0.2f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 1.0f;
                            Globals::OuterEdgeB_Visible = 0.4f;
                        }
                        else if (Globals::visiblepreset == 4) // Void Spark
                        {
                            Globals::CenterEdgeR_Visible = 0.4f;
                            Globals::CenterEdgeG_Visible = 0.0f;
                            Globals::CenterEdgeB_Visible = 0.6f;

                            Globals::InnerEdgeR_Visible = 0.6f;
                            Globals::InnerEdgeG_Visible = 0.2f;
                            Globals::InnerEdgeB_Visible = 1.0f;

                            Globals::OuterEdgeR_Visible = 0.9f;
                            Globals::OuterEdgeG_Visible = 0.3f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                        }
                        else if (Globals::visiblepreset == 5) // Ghost White
                        {
                            Globals::CenterEdgeR_Visible = 0.7f;
                            Globals::CenterEdgeG_Visible = 0.7f;
                            Globals::CenterEdgeB_Visible = 0.7f;

                            Globals::InnerEdgeR_Visible = 0.9f;
                            Globals::InnerEdgeG_Visible = 0.9f;
                            Globals::InnerEdgeB_Visible = 0.9f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 1.0f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                        }
                        else if (Globals::visiblepreset == 6) // Aurora Dream
                        {
                            Globals::CenterEdgeR_Visible = 0.6f;
                            Globals::CenterEdgeG_Visible = 0.6f;
                            Globals::CenterEdgeB_Visible = 1.0f;

                            Globals::InnerEdgeR_Visible = 0.7f;
                            Globals::InnerEdgeG_Visible = 0.8f;
                            Globals::InnerEdgeB_Visible = 1.0f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 1.0f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                        }
                        else if (Globals::visiblepreset == 7) // Cyber Ember
                        {
                            Globals::CenterEdgeR_Visible = 1.0f;
                            Globals::CenterEdgeG_Visible = 0.3f;
                            Globals::CenterEdgeB_Visible = 0.0f;

                            Globals::InnerEdgeR_Visible = 1.0f;
                            Globals::InnerEdgeG_Visible = 0.5f;
                            Globals::InnerEdgeB_Visible = 0.1f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 0.7f;
                            Globals::OuterEdgeB_Visible = 0.2f;
                        }
                        else if (Globals::visiblepreset == 8) // Nebula Fade (Blue to Violet)
                        {
                            Globals::CenterEdgeR_Visible = 0.2f;
                            Globals::CenterEdgeG_Visible = 0.4f;
                            Globals::CenterEdgeB_Visible = 0.9f;

                            Globals::InnerEdgeR_Visible = 0.4f;
                            Globals::InnerEdgeG_Visible = 0.2f;
                            Globals::InnerEdgeB_Visible = 0.8f;

                            Globals::OuterEdgeR_Visible = 0.1f;
                            Globals::OuterEdgeG_Visible = 0.0f;
                            Globals::OuterEdgeB_Visible = 0.3f;
                            }

                        else if (Globals::visiblepreset == 9) // Mystic Flame
                        {
                            Globals::CenterEdgeR_Visible = 0.6f;
                            Globals::CenterEdgeG_Visible = 0.2f;
                            Globals::CenterEdgeB_Visible = 0.6f;

                            Globals::InnerEdgeR_Visible = 0.8f;
                            Globals::InnerEdgeG_Visible = 0.4f;
                            Globals::InnerEdgeB_Visible = 0.9f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 0.6f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                        }
                        else if (Globals::visiblepreset == 10) // Black Hole Spark (Void core + soft galaxy glow)
                        {
                            Globals::CenterEdgeR_Visible = 0.05f;
                            Globals::CenterEdgeG_Visible = 0.0f;
                            Globals::CenterEdgeB_Visible = 0.1f;

                            Globals::InnerEdgeR_Visible = 0.2f;
                            Globals::InnerEdgeG_Visible = 0.1f;
                            Globals::InnerEdgeB_Visible = 0.5f;

                            Globals::OuterEdgeR_Visible = 0.7f;
                            Globals::OuterEdgeG_Visible = 0.4f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                            }

                        else if (Globals::visiblepreset == 11) // Burning Core
                        {
                            Globals::CenterEdgeR_Visible = 1.0f;
                            Globals::CenterEdgeG_Visible = 0.2f;
                            Globals::CenterEdgeB_Visible = 0.0f;

                            Globals::InnerEdgeR_Visible = 1.0f;
                            Globals::InnerEdgeG_Visible = 0.4f;
                            Globals::InnerEdgeB_Visible = 0.1f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 0.6f;
                            Globals::OuterEdgeB_Visible = 0.2f;
                        }
                        else if (Globals::visiblepreset == 12) // Deep Ocean
                        {
                            Globals::CenterEdgeR_Visible = 0.0f;
                            Globals::CenterEdgeG_Visible = 0.3f;
                            Globals::CenterEdgeB_Visible = 0.5f;

                            Globals::InnerEdgeR_Visible = 0.0f;
                            Globals::InnerEdgeG_Visible = 0.5f;
                            Globals::InnerEdgeB_Visible = 0.7f;

                            Globals::OuterEdgeR_Visible = 0.0f;
                            Globals::OuterEdgeG_Visible = 0.8f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                        }
                        else if (Globals::visiblepreset == 13) // Crystal Edge
                        {
                            Globals::CenterEdgeR_Visible = 0.5f;
                            Globals::CenterEdgeG_Visible = 0.8f;
                            Globals::CenterEdgeB_Visible = 1.0f;

                            Globals::InnerEdgeR_Visible = 0.7f;
                            Globals::InnerEdgeG_Visible = 0.9f;
                            Globals::InnerEdgeB_Visible = 1.0f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 1.0f;
                            Globals::OuterEdgeB_Visible = 1.0f;
                        }
                        else if (Globals::visiblepreset == 14) // Magma Burn
                        {
                            Globals::CenterEdgeR_Visible = 1.0f;
                            Globals::CenterEdgeG_Visible = 0.2f;
                            Globals::CenterEdgeB_Visible = 0.0f;

                            Globals::InnerEdgeR_Visible = 1.0f;
                            Globals::InnerEdgeG_Visible = 0.5f;
                            Globals::InnerEdgeB_Visible = 0.2f;

                            Globals::OuterEdgeR_Visible = 1.0f;
                            Globals::OuterEdgeG_Visible = 0.8f;
                            Globals::OuterEdgeB_Visible = 0.4f;
                        }
                        else if (Globals::visiblepreset == 15) // Galaxy Chams
                        {
                            // Galaxy color scheme: Cyan Core, Purple Mid, Deep Violet Outer
                            Globals::CenterEdgeR_Visible = 0.2f;
                            Globals::CenterEdgeG_Visible = 0.85f;
                            Globals::CenterEdgeB_Visible = 1.0f;

                            Globals::InnerEdgeR_Visible = 0.75f;
                            Globals::InnerEdgeG_Visible = 0.3f;
                            Globals::InnerEdgeB_Visible = 0.9f;

                            Globals::OuterEdgeR_Visible = 0.1f;
                            Globals::OuterEdgeG_Visible = 0.0f;
                            Globals::OuterEdgeB_Visible = 0.25f;

                       /*     Globals::intesityedger = 2.5f;
                            Globals::intesityinner = 1.8f;
                            Globals::intesityoutter = 1.3f;*/
                            }

                        // INVISIBLE PRESETS
                        if (Globals::invisiblepreset == 0) // Default Red
                        {
                            Globals::CenterEdgeR_Invisible = 0.01f;
                            Globals::CenterEdgeG_Invisible = 0.0f;
                            Globals::CenterEdgeB_Invisible = 0.0f;

                            Globals::InnerEdgeR_Invisible = 0.1f;
                            Globals::InnerEdgeG_Invisible = 0.0f;
                            Globals::InnerEdgeB_Invisible = 0.0f;

                            Globals::OuterEdgeR_Invisible = 1.0f;
                            Globals::OuterEdgeG_Invisible = 0.0f;
                            Globals::OuterEdgeB_Invisible = 0.0f;
                        }
                        else if (Globals::invisiblepreset == 1) // Inferno
                        {
                            Globals::CenterEdgeR_Invisible = 0.6f;
                            Globals::CenterEdgeG_Invisible = 0.1f;
                            Globals::CenterEdgeB_Invisible = 0.0f;

                            Globals::InnerEdgeR_Invisible = 0.8f;
                            Globals::InnerEdgeG_Invisible = 0.2f;
                            Globals::InnerEdgeB_Invisible = 0.0f;

                            Globals::OuterEdgeR_Invisible = 1.0f;
                            Globals::OuterEdgeG_Invisible = 0.3f;
                            Globals::OuterEdgeB_Invisible = 0.0f;
                        }
                        else if (Globals::invisiblepreset == 2) // Nova Surge (Bright pink/cyan flash)
                        {
                            Globals::CenterEdgeR_Invisible = 0.6f;
                            Globals::CenterEdgeG_Invisible = 0.3f;
                            Globals::CenterEdgeB_Invisible = 1.0f;

                            Globals::InnerEdgeR_Invisible = 0.8f;
                            Globals::InnerEdgeG_Invisible = 0.4f;
                            Globals::InnerEdgeB_Invisible = 1.0f;

                            Globals::OuterEdgeR_Invisible = 1.0f;
                            Globals::OuterEdgeG_Invisible = 0.8f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }

                        else if (Globals::invisiblepreset == 3) // Voltage Spike
                        {
                            Globals::CenterEdgeR_Invisible = 0.2f;
                            Globals::CenterEdgeG_Invisible = 0.0f;
                            Globals::CenterEdgeB_Invisible = 0.5f;

                            Globals::InnerEdgeR_Invisible = 0.3f;
                            Globals::InnerEdgeG_Invisible = 0.0f;
                            Globals::InnerEdgeB_Invisible = 0.9f;

                            Globals::OuterEdgeR_Invisible = 0.6f;
                            Globals::OuterEdgeG_Invisible = 0.0f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }
                        else if (Globals::invisiblepreset == 4) // Frostbite
                        {
                            Globals::CenterEdgeR_Invisible = 0.0f;
                            Globals::CenterEdgeG_Invisible = 0.3f;
                            Globals::CenterEdgeB_Invisible = 0.6f;

                            Globals::InnerEdgeR_Invisible = 0.0f;
                            Globals::InnerEdgeG_Invisible = 0.6f;
                            Globals::InnerEdgeB_Invisible = 1.0f;

                            Globals::OuterEdgeR_Invisible = 0.3f;
                            Globals::OuterEdgeG_Invisible = 1.0f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }
                        else if (Globals::invisiblepreset == 5) // Frozen Galaxy (icy nebula glow)
                        {
                            Globals::CenterEdgeR_Invisible = 0.0f;
                            Globals::CenterEdgeG_Invisible = 0.5f;
                            Globals::CenterEdgeB_Invisible = 0.8f;

                            Globals::InnerEdgeR_Invisible = 0.2f;
                            Globals::InnerEdgeG_Invisible = 0.7f;
                            Globals::InnerEdgeB_Invisible = 1.0f;

                            Globals::OuterEdgeR_Invisible = 0.6f;
                            Globals::OuterEdgeG_Invisible = 1.0f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }

                        else if (Globals::invisiblepreset == 6) // Blood Moon
                        {
                            Globals::CenterEdgeR_Invisible = 0.3f;
                            Globals::CenterEdgeG_Invisible = 0.0f;
                            Globals::CenterEdgeB_Invisible = 0.0f;

                            Globals::InnerEdgeR_Invisible = 0.6f;
                            Globals::InnerEdgeG_Invisible = 0.0f;
                            Globals::InnerEdgeB_Invisible = 0.0f;

                            Globals::OuterEdgeR_Invisible = 1.0f;
                            Globals::OuterEdgeG_Invisible = 0.0f;
                            Globals::OuterEdgeB_Invisible = 0.1f;
                        }
                        else if (Globals::invisiblepreset == 7) // Spectral Veil
                        {
                            Globals::CenterEdgeR_Invisible = 0.2f;
                            Globals::CenterEdgeG_Invisible = 0.2f;
                            Globals::CenterEdgeB_Invisible = 0.5f;

                            Globals::InnerEdgeR_Invisible = 0.3f;
                            Globals::InnerEdgeG_Invisible = 0.3f;
                            Globals::InnerEdgeB_Invisible = 0.7f;

                            Globals::OuterEdgeR_Invisible = 0.5f;
                            Globals::OuterEdgeG_Invisible = 0.5f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }
                        else if (Globals::invisiblepreset == 8) // Grave Ember
                        {
                            Globals::CenterEdgeR_Invisible = 0.5f;
                            Globals::CenterEdgeG_Invisible = 0.2f;
                            Globals::CenterEdgeB_Invisible = 0.0f;

                            Globals::InnerEdgeR_Invisible = 0.7f;
                            Globals::InnerEdgeG_Invisible = 0.3f;
                            Globals::InnerEdgeB_Invisible = 0.0f;

                            Globals::OuterEdgeR_Invisible = 1.0f;
                            Globals::OuterEdgeG_Invisible = 0.5f;
                            Globals::OuterEdgeB_Invisible = 0.2f;
                        }
                        else if (Globals::invisiblepreset == 9) // Arc Phantom
                        {
                            Globals::CenterEdgeR_Invisible = 0.0f;
                            Globals::CenterEdgeG_Invisible = 0.3f;
                            Globals::CenterEdgeB_Invisible = 0.5f;

                            Globals::InnerEdgeR_Invisible = 0.0f;
                            Globals::InnerEdgeG_Invisible = 0.6f;
                            Globals::InnerEdgeB_Invisible = 0.9f;

                            Globals::OuterEdgeR_Invisible = 0.0f;
                            Globals::OuterEdgeG_Invisible = 0.9f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }
                        else if (Globals::invisiblepreset == 10) // Wraithfire
                        {
                            Globals::CenterEdgeR_Invisible = 0.4f;
                            Globals::CenterEdgeG_Invisible = 0.1f;
                            Globals::CenterEdgeB_Invisible = 0.5f;

                            Globals::InnerEdgeR_Invisible = 0.6f;
                            Globals::InnerEdgeG_Invisible = 0.2f;
                            Globals::InnerEdgeB_Invisible = 0.8f;

                            Globals::OuterEdgeR_Invisible = 0.9f;
                            Globals::OuterEdgeG_Invisible = 0.4f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }
                        else if (Globals::invisiblepreset == 11) // Event Horizon (black void + soft color)
                        {
                            Globals::CenterEdgeR_Invisible = 0.0f;
                            Globals::CenterEdgeG_Invisible = 0.0f;
                            Globals::CenterEdgeB_Invisible = 0.0f;

                            Globals::InnerEdgeR_Invisible = 0.3f;
                            Globals::InnerEdgeG_Invisible = 0.0f;
                            Globals::InnerEdgeB_Invisible = 0.5f;

                            Globals::OuterEdgeR_Invisible = 0.9f;
                            Globals::OuterEdgeG_Invisible = 0.4f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                            }

                        else if (Globals::invisiblepreset == 12) // Dark Volt
                        {
                            Globals::CenterEdgeR_Invisible = 0.2f;
                            Globals::CenterEdgeG_Invisible = 0.0f;
                            Globals::CenterEdgeB_Invisible = 0.4f;

                            Globals::InnerEdgeR_Invisible = 0.3f;
                            Globals::InnerEdgeG_Invisible = 0.0f;
                            Globals::InnerEdgeB_Invisible = 0.7f;

                            Globals::OuterEdgeR_Invisible = 0.5f;
                            Globals::OuterEdgeG_Invisible = 0.0f;
                            Globals::OuterEdgeB_Invisible = 1.0f;
                        }
                        else if (Globals::invisiblepreset == 13) // Ether Smoke
                        {
                            Globals::CenterEdgeR_Invisible = 0.3f;
                            Globals::CenterEdgeG_Invisible = 0.3f;
                            Globals::CenterEdgeB_Invisible = 0.3f;

                            Globals::InnerEdgeR_Invisible = 0.6f;
                            Globals::InnerEdgeG_Invisible = 0.6f;
                            Globals::InnerEdgeB_Invisible = 0.6f;

                            Globals::OuterEdgeR_Invisible = 0.9f;
                            Globals::OuterEdgeG_Invisible = 0.9f;
                            Globals::OuterEdgeB_Invisible = 0.9f;
                        }
                        else if (Globals::invisiblepreset == 14) // Poison Chill
                        {
                            Globals::CenterEdgeR_Invisible = 0.1f;
                            Globals::CenterEdgeG_Invisible = 0.6f;
                            Globals::CenterEdgeB_Invisible = 0.3f;

                            Globals::InnerEdgeR_Invisible = 0.3f;
                            Globals::InnerEdgeG_Invisible = 0.8f;
                            Globals::InnerEdgeB_Invisible = 0.5f;

                            Globals::OuterEdgeR_Invisible = 0.5f;
                            Globals::OuterEdgeG_Invisible = 1.0f;
                            Globals::OuterEdgeB_Invisible = 0.7f;
                        }
                        else if (Globals::invisiblepreset == 15) // Galaxy Chams
                        {
                            // ⚡ BETTER MATCH FOR DARK GALAXY/WIREFRAME NEON LOOK

                            Globals::CenterEdgeR_Visible = 0.2f;
                            Globals::CenterEdgeG_Visible = 0.85f;
                            Globals::CenterEdgeB_Visible = 1.0f;

                            Globals::InnerEdgeR_Visible = 0.75f;
                            Globals::InnerEdgeG_Visible = 0.3f;
                            Globals::InnerEdgeB_Visible = 0.9f;

                            Globals::OuterEdgeR_Visible = 0.1f;
                            Globals::OuterEdgeG_Visible = 0.0f;
                            Globals::OuterEdgeB_Visible = 0.25f;

                            

                        }

                    }


                   /* if (Globals::galaxychams2 && MyShooter->IsAlive() && Globals::Thirdperson)
                    {
                        UPrimitiveComponent* myselfchams = Memory::R<UPrimitiveComponent*>(uintptr_t(pawn) + Offsets::thirdpersoncosmetic);
                        if (myselfchams)
                        {
                            galaxychamsplayer(MyPawn, myselfchams);
                        }
                        



                    }*/
                   /* if (Globals::galaxychams3 && Actor->IsAlive())
                    {
                        UPrimitiveComponent* mainMesh = Memory::R<UPrimitiveComponent*>(uintptr_t(Actor) + 0x468);

                       
                        if (mainMesh)
                        {
                            galaxychamsplayer(MyPawn, mainMesh);
                        }


                    }*/
                    if (Globals::outline2 && Actor->IsAlive()) {
                        static float rainbowTime = 0.0f;
                        rainbowTime += 0.02f;

                        const bool isVisible = MyController->LineOfSightTo2(Actor);

                        // === Static resources ===
                        static const FName silohuetteColorParam = kismentsystemlibrary::string_to_name(L"SilohuetteColor");
                        static const FName centerEdgeParam = kismentsystemlibrary::string_to_name(L"CenterEdgeColor");
                        static const FName innerEdgeParam = kismentsystemlibrary::string_to_name(L"InnerEdgeColor");
                        static const FName outerEdgeParam = kismentsystemlibrary::string_to_name(L"OuterEdgeColor");

                        static UObject* visibleMat = UObject::StaticLoadObject(L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI");
                        static UObject* invisibleMat = UObject::StaticLoadObject(L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI");

                        if (!visibleMat || !invisibleMat)
                            return;

                        // === Color calculation ===
                        FLinearColor centerColor, innerColor, outerColor;

                        if (Globals::outlineRainbow) {
                            auto GalaxyPulse = [](float t, int offset = 0) -> FLinearColor {
                                float shift = t * 1.3f + offset;
                                float twinkle = 0.85f + 0.3f * sin(t * 6.5f + offset * 1.37f);
                                float R = (0.2f + 0.3f * sin(shift + 0.5f)) * twinkle;
                                float G = (0.3f + 0.4f * sin(shift + 1.0f)) * twinkle;
                                float B = (0.6f + 0.4f * sin(shift)) * twinkle;
                                return FLinearColor(R, G, B, 1.0f);
                                };

                            centerColor = GalaxyPulse(rainbowTime, 0);
                            innerColor = GalaxyPulse(rainbowTime, 1);
                            outerColor = GalaxyPulse(rainbowTime, 2);
                        }
                        else {
                            if (isVisible) {
                                centerColor = { Globals::CenterEdgeR_Visible, Globals::CenterEdgeG_Visible, Globals::CenterEdgeB_Visible, Globals::intesityedger };
                                innerColor = { Globals::InnerEdgeR_Visible,  Globals::InnerEdgeG_Visible,  Globals::InnerEdgeB_Visible,  Globals::intesityinner };
                                outerColor = { Globals::OuterEdgeR_Visible,  Globals::OuterEdgeG_Visible,  Globals::OuterEdgeB_Visible,  Globals::intesityoutter };
                            }
                            else {
                                centerColor = { Globals::CenterEdgeR_Invisible, Globals::CenterEdgeG_Invisible, Globals::CenterEdgeB_Invisible, Globals::intesityedger };
                                innerColor = { Globals::InnerEdgeR_Invisible,  Globals::InnerEdgeG_Invisible,  Globals::InnerEdgeB_Invisible,  Globals::intesityinner };
                                outerColor = { Globals::OuterEdgeR_Invisible,  Globals::OuterEdgeG_Invisible,  Globals::OuterEdgeB_Invisible,  Globals::intesityoutter };
                            }
                        }

                        // === Material application helper ===
                        auto ApplyOutlineMaterial = [&](USkeletalMeshComponent* mesh, UObject* mat) {
                            if (!mesh || !mat) return;
                            int matCount = mesh->GetNumMaterials();

                            for (int i = 0; i < matCount; ++i) {
                                auto dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, mat);
                                auto matInst = dynMat ? dynMat->cast<UMaterialInstanceDynamic>() : nullptr;
                                if (!matInst) continue;

                                matInst->set_vector_parameter_value(silohuetteColorParam, outerColor);
                                matInst->set_vector_parameter_value(centerEdgeParam, centerColor);
                                matInst->set_vector_parameter_value(innerEdgeParam, innerColor);
                                matInst->set_vector_parameter_value(outerEdgeParam, outerColor);
                            }
                            };

                        // === Apply or reset ===
                        UObject* selectedMat = isVisible ? visibleMat : invisibleMat;

                        if (Globals::outlinetype == 0 || !isVisible) {
                            ApplyOutlineMaterial(Actor->GetPawnMesh(), selectedMat);
                            ApplyOutlineMaterial(Actor->GetCosmeticMesh3P(), selectedMat);
                        }
                        else if (Globals::outlinetype == 1 && isVisible) {
                            if (auto mesh = Actor->GetPawnMesh2())
                                Actor->ResetCharacterMaterials_Internal(mesh);
                            if (auto meshCosmetic = Actor->GetCosmeticMesh3P())
                                Actor->ResetCharacterMaterials_Internal(meshCosmetic);
                        }
                    }




                    if (Globals::ChamsESP) {

                        if (Globals::rainbow_chams) {

                            rainbowTimeZRGB += 0.004f;
                            FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZRGB);
                            auto niga = VALORANT::Module + Offsets::set_ares_outline;
                            USkeletalMeshComponent* myselfchams = Memory::R<USkeletalMeshComponent*>(uintptr_t(pawn) + Offsets::thirdpersoncosmetic);
                            reinterpret_cast<void (*)(USkeletalMeshComponent*, int, bool, uintptr_t, void*)>(spoofcall_stub)(myselfchams, 4, true, Offsets::MagicOffsets, (void*)niga);
                            reinterpret_cast<void (*)(USkeletalMeshComponent*, int, bool, uintptr_t, void*)>(spoofcall_stub)(Mesh, 4, true, Offsets::MagicOffsets, (void*)niga);

                            AresOutlineRendering::SetOutlineColorsForRender(World, { RainbowColor.R * Globals::glowenemyfloat, RainbowColor.G * Globals::glowenemyfloat, RainbowColor.B * Globals::glowenemyfloat ,RainbowColor.A * Globals::glowenemyfloat }, { RainbowColor.R * Globals::glowenemyfloat, RainbowColor.G * Globals::glowenemyfloat, RainbowColor.B * Globals::glowenemyfloat, RainbowColor.A * Globals::glowenemyfloat });
                        }
                        else {
                            USkeletalMeshComponent* myselfchams = Memory::R<USkeletalMeshComponent*>(uintptr_t(pawn) + Offsets::thirdpersoncosmetic);
                            auto niga = VALORANT::Module + Offsets::set_ares_outline;
                            reinterpret_cast<void (*)(USkeletalMeshComponent*, int, bool, uintptr_t, void*)>(spoofcall_stub)(myselfchams, 4, true, Offsets::MagicOffsets, (void*)niga);
                            reinterpret_cast<void (*)(USkeletalMeshComponent*, int, bool, uintptr_t, void*)>(spoofcall_stub)(Mesh, 4, true, Offsets::MagicOffsets, (void*)niga);
                            AresOutlineRendering::SetOutlineColorsForRender(World,
                                { Colors::ChamsColor.R * Globals::glowenemyfloat, Colors::ChamsColor.G * Globals::glowenemyfloat, Colors::ChamsColor.B * Globals::glowenemyfloat, Colors::ChamsColor.A * Globals::glowenemyfloat },
                                { Colors::ChamsColor.R * Globals::glowenemyfloat, Colors::ChamsColor.G * Globals::glowenemyfloat, Colors::ChamsColor.B * Globals::glowenemyfloat, Colors::ChamsColor.A * Globals::glowenemyfloat });
                        }

                    }
                    else {
                        Mesh->SetOutlineMode(EAresOutlineMode::None);
                    }


                }


                if (Globals::EnableAim) {

                    double screen_center_x = canvas->GetScreenSize().X / 2;
                    double screen_center_y = canvas->GetScreenSize().Y / 2;

                    double delta_x = HeadOut.X - screen_center_x;
                    double delta_y = HeadOut.Y - screen_center_y;

                    double distance = sqrt(delta_x * delta_x + delta_y * delta_y);
                    double screen_distance = KismetMathLibrary::distance_2d(HeadOut, { screen_center_x, screen_center_y });
                    double max_distance = Globals::NoSpreadAimbot ? 1300 : Globals::LegitFOvValue;

                    if (distance < closest_distance && screen_distance < max_distance) {
                        target_id = Index;

                        closest_distance = screen_distance;
                    }

                }
            }
        }
    }

   

    if (Globals::Minimap) {
        const float radarSize = 200.0f;
        const float radarRange = Globals::ScaleX;
        const float radarPosX = canvas->GetScreenSize().X - radarSize - 20;
        const float radarPosY = 20;

        // Background fill
        for (float y = 0; y < radarSize; y += 1.0f) {
            canvas->K2_DrawLine(
                FVector2D(radarPosX, radarPosY + y),
                FVector2D(radarPosX + radarSize, radarPosY + y),
                1.0f, FLinearColor(0, 0, 0, 0.7f));
        }

        FLinearColor gray = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
        FLinearColor white = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Border box
        canvas->K2_DrawLine(FVector2D(radarPosX, radarPosY), FVector2D(radarPosX + radarSize, radarPosY), 1.0f, gray);
        canvas->K2_DrawLine(FVector2D(radarPosX + radarSize, radarPosY), FVector2D(radarPosX + radarSize, radarPosY + radarSize), 1.0f, gray);
        canvas->K2_DrawLine(FVector2D(radarPosX + radarSize, radarPosY + radarSize), FVector2D(radarPosX, radarPosY + radarSize), 1.0f, gray);
        canvas->K2_DrawLine(FVector2D(radarPosX, radarPosY + radarSize), FVector2D(radarPosX, radarPosY), 1.0f, gray);

        FVector localPos = MyShooter->K2_GetActorLocation();
        FVector localRot = MyController->GetControlRotation();
        float yawRad = FMath::DegreesToRadians(localRot.Y);

        // Direction arrow
        float indicatorLength = 15.0f;
        float indicatorX = radarPosX + radarSize / 2 + sinf(yawRad) * indicatorLength;
        float indicatorY = radarPosY + radarSize / 2 - cosf(yawRad) * indicatorLength;
        //canvas->K2_DrawLine(
        //    FVector2D(radarPosX + radarSize / 2, radarPosY + radarSize / 2),
        //    FVector2D(indicatorX, indicatorY),
        //    2.0f, FLinearColor(0, 1, 0, 1)
        //);
        // Draw "North" arrow (compass direction indicator)
        {
            float arrowLength = 12.0f;
            float angleToNorth = -yawRad; // because radar rotates with player

            FVector2D center = FVector2D(radarPosX + radarSize / 2, radarPosY + radarSize / 2);

            FVector2D tip = center + FVector2D(sinf(angleToNorth), -cosf(angleToNorth)) * arrowLength;
            FVector2D left = center + FVector2D(sinf(angleToNorth + PI * 0.75f), -cosf(angleToNorth + PI * 0.75f)) * 6.f;
            FVector2D right = center + FVector2D(sinf(angleToNorth - PI * 0.75f), -cosf(angleToNorth - PI * 0.75f)) * 6.f;

            // Arrow triangle outline
            canvas->K2_DrawLine(tip, left, 1.5f, white);
            canvas->K2_DrawLine(left, right, 1.5f, white);
            canvas->K2_DrawLine(right, tip, 1.5f, white);
        }

        // Compass labels (basic draw, no shadow params here)
        canvas->K2_DrawText((L"N"), FVector2D(radarPosX + radarSize / 2 - 5, radarPosY + 5), FVector2D(1, 1), white, 1.0f, FLinearColor(), FVector2D(), false, false, false, FLinearColor());
        canvas->K2_DrawText((L"E"), FVector2D(radarPosX + radarSize - 15, radarPosY + radarSize / 2 - 5), FVector2D(1, 1), white, 1.0f, FLinearColor(), FVector2D(), false, false, false, FLinearColor());
        canvas->K2_DrawText((L"S"), FVector2D(radarPosX + radarSize / 2 - 5, radarPosY + radarSize - 15), FVector2D(1, 1), white, 1.0f, FLinearColor(), FVector2D(), false, false, false, FLinearColor());
        canvas->K2_DrawText((L"W"), FVector2D(radarPosX + 5, radarPosY + radarSize / 2 - 5), FVector2D(1, 1), white, 1.0f, FLinearColor(), FVector2D(), false, false, false, FLinearColor());

        // Enemy markers
        TArray<AShooterCharacter*> Actors = ShooterGameBlueprints::FindAllShooterCharactersWithAlliance(
            World, MyShooter, EAresAlliance::Alliance_Enemy, false, true);

        for (int i = 0; i < Actors.size(); ++i) {
            AShooterCharacter* Actor = Actors[i];
            if (!Actor || Actor == MyShooter || !Actor->IsAlive()) continue;

            FVector enemyPos = Actor->K2_GetActorLocation();
            float dx = enemyPos.X - localPos.X;
            float dy = enemyPos.Y - localPos.Y;

            // Rotate to player's yaw
            float rotatedX = dx * cosf(-yawRad) - dy * sinf(-yawRad);
            float rotatedY = dx * sinf(-yawRad) + dy * cosf(-yawRad);

            float scale = (radarSize / 2.0f) / radarRange;
            float radarX = radarPosX + radarSize / 2 + rotatedX * scale;
            float radarY = radarPosY + radarSize / 2 - rotatedY * scale;

            if (radarX < radarPosX || radarX > radarPosX + radarSize ||
                radarY < radarPosY || radarY > radarPosY + radarSize)
                continue;

            // ✅ Use proper visibility check
            bool bVisible = MyController->LineOfSightTo(Actor, FVector::ZeroVector, false);
            FLinearColor circleColor = bVisible ? FLinearColor(0, 1, 0, 1) : FLinearColor(1, 0, 0, 1);

            const float radius = 4.0f;
            const int segments = 12;

            for (int s = 0; s < segments; ++s) {
                float angle1 = 2.0f * PI * s / segments;
                float angle2 = 2.0f * PI * (s + 1) / segments;

                FVector2D p1 = FVector2D(radarX + cosf(angle1) * radius, radarY + sinf(angle1) * radius);
                FVector2D p2 = FVector2D(radarX + cosf(angle2) * radius, radarY + sinf(angle2) * radius);

                canvas->K2_DrawLine(FVector2D(radarX, radarY), p1, 1.0f, circleColor); // fill
                canvas->K2_DrawLine(p1, p2, 1.0f, circleColor);                        // edge
            }
        }
    }
    if (Globals::bullettracers)
    {

        auto inventory = MyShooter->GetInventory();
        if (!inventory) return;

        auto weapon = inventory->GetCurrentWeapon();
        if (!weapon) return;

        auto firing_state = weapon->get_firing_state();
  

        //// Validate memory before reading
        //if (!Memory::IsValidPtr((void*)((uintptr_t)firing_state + 0x44C), sizeof(float))) return;
        //if (!Memory::IsValidPtr((void*)((uintptr_t)firing_state + 0x4D0), sizeof(float))) return;

        float base = Memory::R<float>((uintptr_t)firing_state + 0x44C);
   
        float modifier = Memory::R<float>((uintptr_t)firing_state + 0x4D0);

        float effective = base * ((modifier > 0.01f && modifier < 10.0f) ? modifier : 1.0f);
        float cooldownSeconds = 1.0f / effective;

        static float lastTracerTick = 0.0f;

        float currentTime = GetTickCount64() / 1000.0f;
        bool isPressedNow = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool canSpawnTracer = isPressedNow && ((currentTime - lastTracerTick) >= cooldownSeconds);

        //// Extra safety: Don't trigger if we're holding a knife or invalid weapon
        //if (!weapon->IsGun()) return;

        // ✅ Get firing origin + view rotation
        FVector eyeLocation;
        FRotator viewRotation;

        MyShooter->get_firing_location_and_direction2(&eyeLocation, &viewRotation, true);
        FVector firingDir = RotatorToVector(viewRotation);
        // Apply spread using your existing system
        FVector spreadOffset = NoSpread::calc_spread(MyShooter, (uintptr_t)firing_state, weapon, firingDir);



        draw_enemy_tracers(canvas,  MyController, eyeLocation, viewRotation, spreadOffset, canSpawnTracer);

        if (canSpawnTracer)
            lastTracerTick = currentTime;
    }




    if (Globals::chatspam)
    {
        //ReadGrenadeThrowInfo();
        auto world = UWorld::GetWorld();
        if (!world)
        {
            //printf("[killsay]  UWorldSave is null\n");
            return;
        }

        UThreadedChatManager* chat_manager = UThreadedChatManager::GetThreadedChatManager(world);
        if (!chat_manager)
        {
            //printf("[killsay]  Failed to get UThreadedChatManager\n");
            return;
        }

        FString raw = FString(Globals::killsay_input);

        FText msg = kismentsystemlibrary::Conv_StringToText(raw);

        if (!msg.Data)
        {
            // printf("[killsay] FText conversion failed\n");
            return;
        }

        static bool wasPressed = false;

        if (GetAsyncKeyState(VK_F1) & 0x8000)
        {
            if (!wasPressed)
            {
                chat_manager->SendChatMessageV2(EChatRoomType::All, msg);
                wasPressed = true;
            }
        }
        else
        {
            wasPressed = false;
        }

        //chat_manager->SendChatMessageV2(EChatRoomType::All, msg);
    }

    if (Globals::ViewModelChanger || Globals::SpinBot || Globals::antiflash || Globals::skybox || Globals::finisher || Globals::self_fresnel || Globals::skyboxmat || Globals::antiflash)
    {
        static UPrimitiveComponent* cachedMesh1P = nullptr;
        static UObject* lastPawn = nullptr;

        // --- Validate MyPawn ---
        if (!Memory::IsValidPointer2(MyPawn)) {
            cachedMesh1P = nullptr;
            lastPawn = nullptr;
            return;
        }

        // --- Update Cache If Needed ---
        if (MyPawn != lastPawn || !cachedMesh1P || !Memory::IsValidPointer2(cachedMesh1P)) {
            lastPawn = MyPawn;
            cachedMesh1P = Memory::R<UPrimitiveComponent*>(uintptr_t(MyPawn) + Offsets::mesh1p);
            if (!Memory::IsValidPointer2(cachedMesh1P))
                return;
        }

        if (!cachedMesh1P)
        {
            return;
            antiaimenabled = false;
            antiflash = false;
        }
        else
        {
            antiflash = true;
            antiaimenabled = true;
            if (Globals::ViewModelChanger)
            {
                //UPrimitiveComponent* negro = Memory::R<UPrimitiveComponent*>(uintptr_t(MyPawn) + Offsets::mesh1p);

                if (auto handmesh = MyShooter->GetOverlayMesh1P())
                {
                    if (auto inventory = MyShooter->GetInventory())
                    {
                        if (auto weapon = inventory->GetCurrentWeapon())
                        {
                            if (auto mesh1p = weapon->GetMesh1P())
                            {
                                //USkeletalMeshComponent* Mesh1P = Memory::R<USkeletalMeshComponent*>(uintptr_t(MyPawn) + Offsets::mesh1p);
                                FVector scaleinspect;
                                FVector scaleknife;
                                FVector scalehand;
                                if (Globals::viewmodeltype == 0) //LongArms
                                    {
                                        scalehand = { 2.6, 1, 1 };
                                        scaleinspect = { 0.423117, 1, 1 };
                                        scaleknife = { 0.483117, 1, 1 };
                                    }
                                    else
                                        if (Globals::viewmodeltype == 1) //CSGO
                                        {
                                            scalehand = { 2.85714, 2.47619, 1 };
                                            scaleinspect = { 0.423117, 0.423117, 1 };
                                            scaleknife = { 0.493506, 0.532468, 1.44156 };

                                        }
                               /* scalehand = { Globals::handheldx, Globals::handheldy, Globals::handheldz };
                                scaleinspect = { Globals::scaleinspectx, Globals::scaleinspecty, Globals::scaleinspectz };
                                scaleknife = { Globals::knifex, Globals::knifey, Globals::knifez };*/
                                std::string weapon_name = kismentsystemlibrary::get_object_name((UObject*)G::MyWeapon).ToString();

                                handmesh->SetRelativeScale3D(scalehand);
                                uintptr_t RightHandWeaponAttachName = reinterpret_cast<uintptr_t>(MyShooter) + 0xFE8;
                                bool isInspecting = Memory::R<bool>((uintptr_t)weapon + 0x1461);
                                bool islefthand = weapon->IsZoomed();
                     
                                //printf("[+] isInspecting: %s\n", isInspecting ? "true" : "false");
                                if (!Memory::IsValidPointer(RightHandWeaponAttachName))
                                    return;
                                FVector scale = { 1, 1, 1 };

                                FRotator rotation = { 90, 90, 0 };
                                FRotator default2 = { 0, 0, 0 };
                                FName attach_point_name = Memory::R<FName>(RightHandWeaponAttachName);
                                if (weapon_name.find("AssaultRifle_AK_C") != std::string::npos) {
                                    if (islefthand)
                                    {
                                        
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                    

                                }
                                else if (weapon_name.find("AssaultRifle_ACR_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("BoltSniper_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("AssaultRifle_Burst_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("AutomaticPistol_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("DMR_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("RevolverPistol_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("LugerPistol_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("SubMachineGun_MP5_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("BasePistol_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("LeverSniperRifle_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("DS_Gun_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("TrainingBotBasePistol_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("SawedOffShotgun_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("Vector_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("PumpShotgun_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("AutomaticShotgun_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("LightMachineGun_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }
                                else if (weapon_name.find("HeavyMachineGun_C") != std::string::npos) {
                                    if (islefthand)
                                    {

                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, default2);
                                    }
                                    else
                                    {
                                        USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                        USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                        if (!isInspecting)
                                        {
                                            mesh1p->SetRelativeScale3D(scale);
                                        }
                                        else
                                        {
                                            mesh1p->SetRelativeScale3D(scaleinspect);
                                        }
                                    }
                                }

                                else if (weapon_name.find("Ability_Melee_Base_C") != std::string::npos) {
                                    USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                    USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                    mesh1p->SetRelativeScale3D(scaleknife);
                                }
                                else
                                {
                                    USceneComponentHelpers::AttachTo(mesh1p, handmesh, attach_point_name, 2, 2, 2, true);
                                    USceneComponentHelpers::SetRelativeRotation(mesh1p, rotation);
                                    mesh1p->SetRelativeScale3D({ 1, 1, 1 });
                                }
                            }
                        }
                    }
                }
            }



            // --- Fast Crouch ---
            if (Globals::FastCrouch && MyShooter) {
                MyShooter->SetCrouchTimeOverride(0.001);
                Globals::Mesh3PModifed = true;
            }
            else if (MyShooter && Globals::Mesh3PModifed) {
                MyShooter->SetCrouchTimeOverride(1.0);
                Globals::Mesh3PModifed = false;
            }

            if (Globals::antiflash && antiflash)
            {
                auto test1 = Memory::R<UBlindManagerComponent*>((uintptr_t)MyShooter + 0xa18); // BlindManagerComponent
                if (!test1)
                    return;
                use_blind_manager_component(test1);
            }
            static AGameObject* SkyDome = nullptr;

            if (Globals::skybox && antiflash)
            {
                AGameObject* SkyDome = nullptr;

                // First, check if we already cached one and it's still valid
                static AGameObject* CachedSkyDome = nullptr;
                if (CachedSkyDome && Memory::IsValidPointer((uintptr_t)CachedSkyDome))
                {
                    auto name = kismentsystemlibrary::get_object_name((UObject*)CachedSkyDome);
                    if (name.IsValid() && name.ToString() == "shared_SkyDomeB_0")
                    {
                        SkyDome = CachedSkyDome;
                    }
                }

                // Otherwise, re-find it
                if (!SkyDome)
                {
                    TArray<AGameObject*> Objects;
                    GameplayStatics::GetAllActorsOfClass2(UWorldSave, Class::Actors(), &Objects);

                    for (int i = 0; i < Objects.size(); ++i)
                    {
                        AGameObject* Object = Objects[i];
                        if (!Object || !Memory::IsValidPointer((uintptr_t)Object)) continue;

                        auto name = kismentsystemlibrary::get_object_name((UObject*)Object);
                        if (!name.IsValid()) continue;

                        if (name.ToString() == "shared_SkyDomeB_0")
                        {
                            SkyDome = Object;
                            CachedSkyDome = Object;  // cache it for future
                            break;
                        }
                    }
                }

                if (SkyDome && Memory::IsValidPointer((uintptr_t)SkyDome))
                {
                    auto Mesh = Memory::R<UPrimitiveComponent*>((uintptr_t)SkyDome + Offsets::skyboxmeshcomponent);




                    if (!Mesh || !Memory::IsValidPointer((uintptr_t)Mesh))
                        return;
                    FName first_name = kismentsystemlibrary::string_to_name(L"Horizon color");
                    FName second_name = kismentsystemlibrary::string_to_name(L"Zenith Color");
                    FName third_name = kismentsystemlibrary::string_to_name(L"Overall Color");
                    FName cloud_color = kismentsystemlibrary::string_to_name(L"Cloud Color");
                    FName cloud_speed = kismentsystemlibrary::string_to_name(L"Cloud Speed");
                    FName Stars_Brightness = kismentsystemlibrary::string_to_name(L"Stars Brightness");
                    FName cloud_op = kismentsystemlibrary::string_to_name(L"Cloud Opacity");
                    auto matPath = L"/Engine/EngineSky/M_Sky_Panning_Clouds2.M_Sky_Panning_Clouds2";
                    UObject* material = UObject::find_object(matPath);
                    if (!material)
                        UObject::StaticLoadObject(matPath);
                    material = UObject::find_object(matPath);

                    if (!material || !Memory::IsValidPointer((uintptr_t)material))
                        return;

                    static UObject* dynMat = nullptr;

                    if (!dynMat)
                    {
                        Mesh->SetMaterial(0, material);
                        dynMat = Mesh->create_and_set_material_instance_dynamic_from_material(0, material);
                    }

                    if (dynMat && Memory::IsValidPointer((uintptr_t)dynMat))
                    {

                        auto num_materials = Mesh->GetNumMaterials();
                        for (int i = 0; i < num_materials; i++) {
                            UObject* material_instance_dynamic = Mesh->create_and_set_material_instance_dynamic_from_material(i, material);
                            if (material_instance_dynamic) {

                                material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(first_name, { Globals::Overall.R * 1, Globals::Overall.G * 1, Globals::Overall.B * 1 });
                                material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(second_name, { Globals::Zenith.R * 1, Globals::Zenith.G * 1, Globals::Zenith.B * 1 });
                                material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(third_name, { Globals::Horizon.R * 1, Globals::Horizon.G * 1, Globals::Horizon.B * 1 });
                                material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(cloud_color, { Globals::Cloud.R * 1, Globals::Cloud.G * 1, Globals::Cloud.B * 1 });
                                material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_scalar_parameter_value(cloud_speed, Globals::CloudSpeed);
                                material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_scalar_parameter_value(Stars_Brightness, Globals::StarsBrightness);
                                material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_scalar_parameter_value(cloud_op, Globals::CloudOpacity);

                            }
                        }
                    }
                    if (Globals::skyboxRGB) {

                        rainbowTimeZskyboxRGB += 0.005f;
                        rainbowTimeZskyboxRGB2 += 0.010f;
                        rainbowTimeZskyboxRGB3 += 0.015f;
                        rainbowTimeZskyboxRGB4 += 0.020f;
                        FLinearColor rainbow = BOSSRAINBOMAW(rainbowTimeZskyboxRGB);
                        FLinearColor rainbow2 = BOSSRAINBOMAW(rainbowTimeZskyboxRGB2);
                        FLinearColor rainbow3 = BOSSRAINBOMAW(rainbowTimeZskyboxRGB3);
                        FLinearColor rainbow4 = BOSSRAINBOMAW(rainbowTimeZskyboxRGB4);

                        if (Mesh) {
                            auto num_materials = Mesh->GetNumMaterials();
                            for (int i = 0; i < num_materials; i++) {
                                UObject* material_instance_dynamic = Mesh->create_and_set_material_instance_dynamic_from_material(i, material);
                                if (material_instance_dynamic) {

                                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(first_name, { rainbow.R * 1, rainbow.G * 1, rainbow.B * 1 });
                                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(second_name, { rainbow2.R * 1, rainbow2.G * 1, rainbow2.B * 1 });
                                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(third_name, { rainbow3.R * 1, rainbow3.G * 1, rainbow3.B * 1 });
                                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value(cloud_color, { rainbow4.R * 1, rainbow4.G * 1, rainbow4.B * 1 });
                                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_scalar_parameter_value(cloud_speed, Globals::CloudSpeed);
                                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_scalar_parameter_value(Stars_Brightness, Globals::StarsBrightness);
                                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_scalar_parameter_value(cloud_op, Globals::CloudOpacity);
                                }
                            }
                        }
                    }

                }
            }
            if (Globals::skyboxmat)
            {
                AGameObject* SkyDome = nullptr;

                // First, check if we already cached one and it's still valid
                static AGameObject* CachedSkyDome = nullptr;
                if (CachedSkyDome && Memory::IsValidPointer((uintptr_t)CachedSkyDome))
                {
                    auto name = kismentsystemlibrary::get_object_name((UObject*)CachedSkyDome);
                    if (name.IsValid() && name.ToString() == "shared_SkyDomeB_0")
                    {
                        SkyDome = CachedSkyDome;
                    }
                }

                // Otherwise, re-find it
                if (!SkyDome)
                {
                    TArray<AGameObject*> Objects;
                    GameplayStatics::GetAllActorsOfClass2(UWorldSave, Class::Actors(), &Objects);

                    for (int i = 0; i < Objects.size(); ++i)
                    {
                        AGameObject* Object = Objects[i];
                        if (!Object || !Memory::IsValidPointer((uintptr_t)Object)) continue;

                        auto name = kismentsystemlibrary::get_object_name((UObject*)Object);
                        if (!name.IsValid()) continue;

                        if (name.ToString() == "shared_SkyDomeB_0")
                        {
                            SkyDome = Object;
                            CachedSkyDome = Object;  // cache it for future
                            break;
                        }
                    }
                }



                if (SkyDome && Memory::IsValidPointer((uintptr_t)SkyDome))
                {
                    auto Mesh = Memory::R<UPrimitiveComponent*>((uintptr_t)SkyDome + Offsets::skyboxmeshcomponent);
                    if (!Mesh || !Memory::IsValidPointer((uintptr_t)Mesh)) return;

                    FString customTexturePath = FString(L"C:/antivgcsky.jpg");
                    UObject* CustomTexture = USceneComponentHelpers::ImportFileAsTexture2D(UWorldSave, customTexturePath);
                    if (!CustomTexture || !Memory::IsValidPointer((uintptr_t)CustomTexture)) return;

                    auto matPath = L"/Game/Equippables/_Core/Materials/SpecialMaterials/Hellfire/1P_Hellfire_MI.1P_Hellfire_MI";
                    UObject* material = UObject::find_object(matPath);
                    if (!material) material = UObject::StaticLoadObject(matPath);
                    if (!material || !Memory::IsValidPointer((uintptr_t)material)) return;

                    Mesh->SetMaterial(0, material);
                    UObject* SkyDynamicMat = Mesh->create_and_set_material_instance_dynamic_from_material(0, material);
                    if (!SkyDynamicMat || !Memory::IsValidPointer((uintptr_t)SkyDynamicMat)) return;

                    auto* mat = SkyDynamicMat->cast<UMaterialInstanceDynamic>();
                    if (!mat) return;

                    std::vector<FString> textureParams = {
                        L"Lava DF", L"Albedo", L"Lava MRS", L"Lava Normal",
                        L"Lave AEM", L"AEM", L"MRS"
                    };

                    for (const auto& param : textureParams)
                        mat->SetTextureParameterValue(kismentsystemlibrary::string_to_name(param), CustomTexture);
                }

                Globals::skyboxmat = false;  // Reset the button flag
            }
        }
     


        // --- ViewModel Changer ---

    }




    //if (Globals::BuddyChanger)
    //{

    //    //FVector2D screenCenter(canvas->GetScreenSize().X * 0.5f, 50.0f);

    //    inventory_manager* manager = ares_instance::get_ares_client_game_instance(World)->get_inventory_manager();

    //    auto charm_instances = Memory::R<TArray<tmap<UObject*, UObject*>>>(std::uintptr_t(manager) + 0x430);
    //    buddy = charm_instances[index].Key;
    //    BuddyName = kismentsystemlibrary::get_object_name(buddy);
    //    BuddyNameYAY = BuddyName.wide();
    //    int shown = 0;

    //    for (int i = 0; i < charm_instances.size(); ++i)
    //    {
    //        UObject* current = charm_instances[i].Key;
    //        std::wstring name = kismentsystemlibrary::get_object_name(current).wide();

    //        // Helper function (if not already declared)
    //       

    //        // Lower both search input and buddy name
    //        std::wstring loweredName = ToLower(name);
    //        std::wstring loweredSearch = ToLower(Globals::BuddySearch);

    //        if (loweredSearch.empty() || loweredName.find(loweredSearch) != std::wstring::npos)

    //        {
    //            if (menu::Button(name.c_str(), FVector2D{ 250, 20 }))
    //            {
    //                buddy = current;
    //                index = i;
    //                BuddyNameYAY = name;
    //            }
    //            menu::offset_y += 22.f;
    //            if (++shown >= 10) break;
    //        }
    //    }

    //}



















    if (Globals::party_hat && Globals::Thirdperson) {
        USkeletalMeshComponent* myselfchams = Memory::R<USkeletalMeshComponent*>(uintptr_t(MyShooter) + Offsets::thirdpersoncosmetic);
        auto Head = GetBoneMatrix(myselfchams, Bones::head);
        SPOOF_CALL(partyhat)(MyController, myselfchams, canvas, Head);
    }


    if (Globals::customgun)
    {

        AAresEquippable* Equippable = MyShooter->GetInventory()->GetCurrentWeapon();
        UPrimitiveComponent* GunMesh = Memory::R<UPrimitiveComponent*>(uintptr_t(Equippable) + Offsets::mesh1pgun);
        if (!GunMesh || !Memory::IsValidPointer((uintptr_t)GunMesh)) return;

        FString customTexturePath = FString(L"C:/antivgcgun.jpg");
        UObject* CustomTexture = USceneComponentHelpers::ImportFileAsTexture2D(UWorldSave, customTexturePath);
        if (!CustomTexture || !Memory::IsValidPointer((uintptr_t)CustomTexture)) return;

        auto matPath = L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/Winter/Winter_MI.Winter_MI";
        UObject* material = UObject::find_object(matPath);
        if (!material) material = UObject::StaticLoadObject(matPath);
        if (!material || !Memory::IsValidPointer((uintptr_t)material)) return;

        GunMesh->SetMaterial(0, material);
        UObject* GunDynamicMat = GunMesh->create_and_set_material_instance_dynamic_from_material(0, material);
        if (!GunDynamicMat || !Memory::IsValidPointer((uintptr_t)GunDynamicMat)) return;

        auto* mat = GunDynamicMat->cast<UMaterialInstanceDynamic>();
        if (!mat) return;

        mat->SetTextureParameterValue(kismentsystemlibrary::string_to_name(L"Image 1"), CustomTexture);
        mat->SetTextureParameterValue(kismentsystemlibrary::string_to_name(L"Image 2"), CustomTexture);



        Globals::customgun = false;  // Reset toggle after attempt
    }

    if (Globals::galaxychams && MyShooter->IsAlive())
    {
        // const bool isVisible = MyController->LineOfSightTo(Actor);

         // === Static FName Param Handles ===
        static FName silohuetteColorParam = kismentsystemlibrary::string_to_name(L"SilohuetteColor");
        static FName centerEdgeParam = kismentsystemlibrary::string_to_name(L"CenterEdgeColor");
        static FName innerEdgeParam = kismentsystemlibrary::string_to_name(L"InnerEdgeColor");
        static FName outerEdgeParam = kismentsystemlibrary::string_to_name(L"OuterEdgeColor");
        static FName glowIntensityParam = kismentsystemlibrary::string_to_name(L"GlowIntensity");

        // === Static Material Loading ===
        static UObject* visibleMat = UObject::StaticLoadObject(L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI");
        static UObject* invisibleMat = UObject::StaticLoadObject(L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI");

        if (!visibleMat || !invisibleMat) return;

        // === Galaxy-style Fixed Colors ===
        FLinearColor centerColor, innerColor, outerColor;
        {
            // === Galaxy-style Fixed Colors ===
            Globals::sCenterEdgeR_Visible = 0.5f;
            Globals::sCenterEdgeG_Visible = 0.0f;
            Globals::sCenterEdgeB_Visible = 0.6f;

            Globals::sInnerEdgeR_Visible = 0.75f;
            Globals::sInnerEdgeG_Visible = 0.3f;
            Globals::sInnerEdgeB_Visible = 0.9f;

            Globals::sOuterEdgeR_Visible = 0.1f;
            Globals::sOuterEdgeG_Visible = 0.0f;
            Globals::sOuterEdgeB_Visible = 0.25f;

            centerColor = FLinearColor(
                Globals::sCenterEdgeR_Visible,
                Globals::sCenterEdgeG_Visible,
                Globals::sCenterEdgeB_Visible,
                Globals::intesityedger
            );
            innerColor = FLinearColor(
                Globals::sInnerEdgeR_Visible,
                Globals::sInnerEdgeG_Visible,
                Globals::sInnerEdgeB_Visible,
                Globals::intesityinner
            );
            outerColor = FLinearColor(
                Globals::sOuterEdgeR_Visible,
                Globals::sOuterEdgeG_Visible,
                Globals::sOuterEdgeB_Visible,
                Globals::intesityoutter
            );

        }


        // === Material Application Helper ===
        auto ApplyOutlineMaterial = [&](UPrimitiveComponent* mesh, UObject* mat)
            {
                if (!mesh) return;
                int count = mesh->GetNumMaterials();
                for (int i = 0; i < count; ++i)
                {
                    auto dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, mat);
                    auto matInst = dynMat ? dynMat->cast<UMaterialInstanceDynamic>() : nullptr;
                    if (!matInst) continue;
                    // matInst->set_vector_parameter_value(silohuetteColorParam, silColor);

                    matInst->set_vector_parameter_value(silohuetteColorParam, outerColor);
                    matInst->set_vector_parameter_value(centerEdgeParam, centerColor);
                    matInst->set_vector_parameter_value(innerEdgeParam, innerColor);
                    matInst->set_vector_parameter_value(outerEdgeParam, outerColor);

                    // Optional: Boost emissive intensity if supported
                    // matInst->set_scalar_parameter_value(glowIntensityParam, 2.0f);
                }
            };

        // === Apply to Mesh if Hidden or Outline Forced ===
        if (Globals::outlinetype == 0)
        {
            UPrimitiveComponent* mainMesh = Memory::R<UPrimitiveComponent*>(uintptr_t(MyShooter) + Offsets::mesh1p);
            UPrimitiveComponent* mainMesh2 = Memory::R<UPrimitiveComponent*>(uintptr_t(MyShooter) + Offsets::mesh1p_overlay);
            UObject* selectedMat = visibleMat;

            if (mainMesh || mainMesh2)
            {
                ApplyOutlineMaterial(mainMesh, selectedMat);
                ApplyOutlineMaterial(mainMesh2, selectedMat);
            }
        }
    }



    if (Globals::galaxychams2 && MyShooter->IsAlive())
    {
       // const bool isVisible = MyController->LineOfSightTo(Actor);

        // === Static FName Param Handles ===
        static FName silohuetteColorParam = kismentsystemlibrary::string_to_name(L"SilohuetteColor");
        static FName centerEdgeParam = kismentsystemlibrary::string_to_name(L"CenterEdgeColor");
        static FName innerEdgeParam = kismentsystemlibrary::string_to_name(L"InnerEdgeColor");
        static FName outerEdgeParam = kismentsystemlibrary::string_to_name(L"OuterEdgeColor");
        static FName glowIntensityParam = kismentsystemlibrary::string_to_name(L"GlowIntensity");

        // === Static Material Loading ===
        static UObject* visibleMat = UObject::StaticLoadObject(L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI");
        static UObject* invisibleMat = UObject::StaticLoadObject(L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI");

        if (!visibleMat || !invisibleMat) return;

        // === Galaxy-style Fixed Colors ===
        FLinearColor centerColor, innerColor, outerColor;
        {
            // === Galaxy-style Fixed Colors ===
            Globals::sCenterEdgeR_Visible = 0.5f;
            Globals::sCenterEdgeG_Visible = 0.0f;
            Globals::sCenterEdgeB_Visible = 0.6f;

            Globals::sInnerEdgeR_Visible = 0.75f;
            Globals::sInnerEdgeG_Visible = 0.3f;
            Globals::sInnerEdgeB_Visible = 0.9f;

            Globals::sOuterEdgeR_Visible = 0.1f;
            Globals::sOuterEdgeG_Visible = 0.0f;
            Globals::sOuterEdgeB_Visible = 0.25f;

            centerColor = FLinearColor(
                Globals::sCenterEdgeR_Visible,
                Globals::sCenterEdgeG_Visible,
                Globals::sCenterEdgeB_Visible,
                Globals::intesityedger
            );
            innerColor = FLinearColor(
                Globals::sInnerEdgeR_Visible,
                Globals::sInnerEdgeG_Visible,
                Globals::sInnerEdgeB_Visible,
                Globals::intesityinner
            );
            outerColor = FLinearColor(
                Globals::sOuterEdgeR_Visible,
                Globals::sOuterEdgeG_Visible,
                Globals::sOuterEdgeB_Visible,
                Globals::intesityoutter
            );

        }


        // === Material Application Helper ===
        auto ApplyOutlineMaterial = [&](UPrimitiveComponent* mesh, UObject* mat)
            {
                if (!mesh) return;
                int count = mesh->GetNumMaterials();
                for (int i = 0; i < count; ++i)
                {
                    auto dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, mat);
                    auto matInst = dynMat ? dynMat->cast<UMaterialInstanceDynamic>() : nullptr;
                    if (!matInst) continue;
                   // matInst->set_vector_parameter_value(silohuetteColorParam, silColor);

                    matInst->set_vector_parameter_value(silohuetteColorParam, outerColor);
                    matInst->set_vector_parameter_value(centerEdgeParam, centerColor);
                    matInst->set_vector_parameter_value(innerEdgeParam, innerColor);
                    matInst->set_vector_parameter_value(outerEdgeParam, outerColor);

                    // Optional: Boost emissive intensity if supported
                    // matInst->set_scalar_parameter_value(glowIntensityParam, 2.0f);
                }
            };

        // === Apply to Mesh if Hidden or Outline Forced ===
        if (Globals::outlinetype == 0)
        {
            UPrimitiveComponent* mainMesh = Memory::R<UPrimitiveComponent*>(uintptr_t(MyShooter) + Offsets::mesh_cosmetic_3p);
            UObject* selectedMat = visibleMat;

            if (mainMesh)
            {
                ApplyOutlineMaterial(mainMesh, selectedMat);
            }
        }
    }


    try {
        static DWORD spread_comp_ready_time = 0;
        static DWORD shoot_delay_time = 0;
        static bool delay_pending = false;
        static bool spread_locked = false;
        static bool stop_for_shot = false;
        static FVector cachedSpinRotation = FVector();
        if (target_id != -1) {
            if (Globals::EnableAim && !G::IsKnife && !Globals::Menu_Open) {

                if (!Actors.is_valid_index(target_id)) return;

                AShooterCharacter* Actor = Actors[target_id];
                if (!Actor || !MyShooter || !isActorValid(Actor, MyShooter)) return;

                USkeletalMeshComponent* Mesh = Actor->GetPawnMesh2();
                USkeletalMeshComponent* HandMesh = MyShooter->GetOverlayMesh1P();
                if (!Mesh || Actor->GetHealth() <= 0) return;

                FVector Target;

                Target = GetBoneMatrix(Mesh, Globals::AimBone == 0 ? 8 : (Globals::AimBone == 1 ? 6 : 3));

                FVector CameraPos = MyCamera->GetCameraLocation();
                FVector ControlRotation = MyController->GetControlRotation();
                FVector  CameraRot = MyCamera->GetCameraRotation();
                FVector BestAimPoint;
                FString BestBoneName;
                bool CanAutoWall = false;
                AutoWallSystem::WallPenetrationResult WallResult;

                if (Globals::auto_wall) {
                    CanAutoWall = AutoWallSystem::ShouldShootThroughWalls(MyShooter, Actor, BestAimPoint, BestBoneName, Globals::auto_wall_mode);
                    if (CanAutoWall) {
                        int BoneIndex = 2;

                        if (BestBoneName.contains(L"Head"))
                            BoneIndex = 0;
                        else if (BestBoneName.contains(L"Neck"))
                            BoneIndex = 1;

                        WallResult = AutoWallSystem::CalculateWallPenetration(
                            MyShooter, Actor, BestAimPoint,
                            BoneIndex
                        );

                       // AutoWallSystem::DrawAutoWallVisualization(canvas, Actor, WallResult);
                    }
                }
                FVector eyeLocation;
                FRotator eyeRotation;
                MyShooter->GetActorEyesViewPoint(&eyeLocation, &eyeRotation);

               /* bool bVisible;*/
                bool bVisible = !Globals::VisibleCheck || (
                    MyController->LineOfSightTo2(Actor)
                    );

                // Default predicted target is just the actual target
                FVector futurePosition = Target;
                bool willBeVisibleSoon = false;

                // ✅ Only predict if: prediction is enabled, actor is NOT visible, and player is in first person
                if (Globals::EnablePrediction && !bVisible && !Globals::Thirdperson)
                {
                    FVector velocity = Actor->GetVelocity();

                    if (velocity.IsFinite())
                    {
                        float peekTimeMS = Globals::PredictionPeekMS > 0 ? Globals::PredictionPeekMS : 90.f;
                        float peekTimeSec = peekTimeMS / 1000.f;

                        futurePosition = Target + velocity * peekTimeSec;

                        willBeVisibleSoon = MyController->LineOfSightTo(Actor, futurePosition, false);
                    }
                }
                else if (Globals::Thirdperson && !bVisible)
                {
                    // Optional: fallback LineOfSightTo check in third person, if needed
                    willBeVisibleSoon = MyController->LineOfSightTo2(Actor);
                }

                // Final aiming target: predicted or actual
                FVector AimTarget = (willBeVisibleSoon ? futurePosition : Target);

                bool ShouldAim = bVisible || CanAutoWall;

               
          


                auto IsInFovAndVisible = [&]() {
                    return in_rect(
                        Globals::ScreenCenterX,
                        Globals::ScreenCenterY,
                        Globals::LegitFOvValue,
                        head_screen.X,
                        head_screen.Y
                    ) && ShouldAim;
                    };

                auto ApplyAimbot = [&]() {
                    FVector recoil;
                    recoil.X = CameraRot.X - ControlRotation.X;
                    recoil.Y = CameraRot.Y - ControlRotation.Y;
                    recoil.Z = 0.f;
                    cachedSpinRotation = ControlRotation; // cache spin angle before aiming
                    Globals::RecoilControl ? aimbot_recoil(CameraRot, ControlRotation, AimTarget, CameraPos, recoil, MyController)
                        : aimbot(CameraRot, ControlRotation, AimTarget, CameraPos, MyController);
                    };

                if (MyController->ProjectWorldLocationToScreen(AimTarget, head_screen, 0) && head_screen.IsValid()) {
                    if (Globals::EnableAim &&
                        (GetAsyncKeyState(Globals::LegitBotKey) & 0x8000) &&
                        !G::pixel_time_bool &&
                        IsInFovAndVisible() &&
                      
                        !Globals::NoSpreadAimbot &&
                        !hasTarget)
                    {
                        ApplyAimbot();
                        hasTarget = true;
                    }
                    else if (Globals::Aimlock &&
                        !Globals::RecoilControl &&
                        !G::pixel_time_bool &&
                        IsInFovAndVisible() &&
                       Globals::EnableAim &&
                        !hasTarget)
                    {
                        ApplyAimbot();
                        hasTarget = true;
                    }
                    else if (Globals::EnableAim &&
                        Globals::Aimlock &&
                        Globals::RecoilControl &&
                        IsInFovAndVisible() &&
                       
                        !Globals::NoSpreadAimbot &&
                        !hasTarget)
                    {
                        ApplyAimbot();
                        hasTarget = true;
                    }

                }

                if (Globals::NoSpreadAimbot && !G::IsKnife) {
                    FVector Head = GetBoneMatrix(Actor->GetPawnMesh2(), 8);

                    auto ProcessWeapon = [&](FString obj_name) {
                        FVector CameraPos2 = FVector(0, 0, 0);
                        FVector firing_direction2 = FVector(0, 0, 0);
                        FVector spread_angle;
                        MyShooter->get_firing_location_and_direction(&CameraPos2, &firing_direction2, false);
                        FVector vector_pos = AimTarget - CameraPos2;
                        float distance = vector_pos.Size();

                        if (distance <= 0) return;

                        FVector target_rotation = FVector(
                            -((acosf(fminf(fmaxf(vector_pos.Z / distance, -1.f), 1.f)) * 180.f / PI) - 90.f),
                            atan2f(vector_pos.Y, vector_pos.X) * 180.f / PI,
                            0
                        );

                        FVector new_rotation = Math::SmoothAim(target_rotation, ControlRotation, 1.0f);
                        FVector direction = RotationToVector(new_rotation);

                        auto current_inv = MyShooter->GetInventory();
                        if (!current_inv) return;

                        SPOOF_FUNC;
                        auto current_equip = current_inv->GetCurrentWeapon();
                        if (!current_equip) return;

                        auto firing_state = current_equip->get_firing_state();
                        //printf("firing_state: 0x%llX\n", firing_state);
                        if (!firing_state) return;

                        std::wstring name = obj_name.wide();

                        bool allow_nospread =
                            name == L"Vandal" || name == L"Phantom" || name == L"Operator" ||
                            name == L"Marshal" || name == L"Sheriff" || name == L"Spectre" ||
                            name == L"Outlaw" || name == L"Classic" || name == L"Shorty" ||
                            name == L"Frenzy" || name == L"Ghost" || name == L"Stinger" ||
                            name == L"Bucky" || name == L"Judge" || name == L"Guardian" ||
                            name == L"Bulldog" || name == L"Ares" || name == L"Odin";

                        if (!allow_nospread) return;

                        spread_angle = NoSpread::calc_spread(MyShooter, (uintptr_t)firing_state, current_equip, direction);

                        if (spread_angle.Size() > 0.001f)
                            new_rotation = new_rotation - spread_angle;

                        FVector smoothed = Math::SmoothAim(new_rotation, ControlRotation, Globals::smooth);
                        MyController->SetControlRotation(smoothed);

                        hasTarget3 = true;
                        };

                    if (Globals::EnableAim &&
                        ((GetAsyncKeyState(Globals::LegitBotKey) & 0x8000)) &&
                        !hasTarget3 &&
                        MyShooter->GetInventory()->GetCurrentWeapon() &&
                        (ShouldAim)
                        ) {

                        FString obj_name = helper::convert_weapon_name(
                            kismentsystemlibrary::get_object_name(
                                MyShooter->GetInventory()->GetCurrentWeapon()));

                        std::wstring name = obj_name.wide();

                        bool allow_nospread =
                            name == L"Vandal" || name == L"Phantom" || name == L"Operator" ||
                            name == L"Marshal" || name == L"Sheriff" || name == L"Spectre" ||
                            name == L"Outlaw" || name == L"Classic" || name == L"Shorty" ||
                            name == L"Frenzy" || name == L"Ghost" || name == L"Stinger" ||
                            name == L"Bucky" || name == L"Judge" || name == L"Guardian" ||
                            name == L"Bulldog" || name == L"Ares" || name == L"Odin";

                        if (allow_nospread) {
                            ProcessWeapon(obj_name);
                        }
                        else {
                            if (MyController->ProjectWorldLocationToScreen(AimTarget, head_screen, 0) &&
                                head_screen.IsValid() &&
                                ((GetAsyncKeyState(Globals::LegitBotKey) & 0x8000))) {

                                FVector vector_pos = AimTarget - CameraPos;
                                float distance = vector_pos.Size();
                                if (distance <= 0) return;

                                FVector target_rotation = FVector(
                                    -((acosf(fminf(fmaxf(vector_pos.Z / distance, -1.f), 1.f)) * 180.f / PI) - 90.f),
                                    atan2f(vector_pos.Y, vector_pos.X) * 180.f / PI,
                                    0
                                );

                                FVector new_rotation = Math::SmoothAim(target_rotation, ControlRotation, 1.0f);
                                MyController->SetControlRotation(new_rotation);
                                hasTarget3 = true;
                            }
                        }
                    }
                }


                if (Globals::AutoShoot && (GetAsyncKeyState(Globals::LegitBotKey) & 0x8000)) {
                    DWORD now = GetTickCount();
                    static uintptr_t lastTargetFired = 0;
                    bool isNewTarget = reinterpret_cast<uintptr_t>(Actor) != lastTargetFired;
                    bool canFire = isNewTarget && (ShouldAim);


                    if (Globals::NoSpreadAimbot && !spread_locked) {
                        spread_comp_ready_time = now + 400;
                        spread_locked = true;
                    }


                    if (Globals::NoSpreadAimbot) {
                        if (spread_locked && now >= spread_comp_ready_time) {
                            if (canFire && !delay_pending) {
                                stop_for_shot = true;
                                shoot_delay_time = now + std::max<DWORD>(Globals::AutoshootFloat, 20);
                                delay_pending = true;
                            }
                            if (delay_pending && now >= shoot_delay_time) {
                                MyController->simulate_input_key(keys::left_mouse, true);
                                MyController->simulate_input_key(keys::left_mouse, false);
                                auto inventory = MyShooter->GetInventory();
                                if (!inventory) return;

                                auto weapon = inventory->GetCurrentWeapon();
                                if (!weapon) return;

                                auto firing_state = weapon->get_firing_state();


                                //// Validate memory before reading
                                //if (!Memory::IsValidPtr((void*)((uintptr_t)firing_state + 0x44C), sizeof(float))) return;
                                //if (!Memory::IsValidPtr((void*)((uintptr_t)firing_state + 0x4D0), sizeof(float))) return;

                                float base = Memory::R<float>((uintptr_t)firing_state + 0x44C);

                                float modifier = Memory::R<float>((uintptr_t)firing_state + 0x4D0);

                                float effective = base * ((modifier > 0.01f && modifier < 10.0f) ? modifier : 1.0f);
                                float cooldownSeconds = 1.0f / effective;

                                static float lastTracerTick = 0.0f;

                                float currentTime = GetTickCount64() / 1000.0f;
                                bool isPressedNow = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
                                bool canSpawnTracer = ((currentTime - lastTracerTick) >= cooldownSeconds);

                                //// Extra safety: Don't trigger if we're holding a knife or invalid weapon
                                //if (!weapon->IsGun()) return;

                                // ✅ Get firing origin + view rotation
                                FVector eyeLocation;
                                FRotator viewRotation;

                                MyShooter->get_firing_location_and_direction2(&eyeLocation, &viewRotation, true);
                                FVector firingDir = RotatorToVector(viewRotation);
                                // Apply spread using your existing system
                                FVector spreadOffset = NoSpread::calc_spread(MyShooter, (uintptr_t)firing_state, weapon, firingDir);


                                // ✅ Apply spread using your calc_spread


                                draw_enemy_tracers(canvas, MyController, eyeLocation, viewRotation, spreadOffset, canSpawnTracer);

                                if (canSpawnTracer)
                                    lastTracerTick = currentTime;
                                Globals::spin_paused = true;
                                Globals::waiting_for_kill = true;
                                Globals::resume_spin_time = GetTickCount() + 30;
                                delay_pending = false;
                            }
                        }
                    }
                    else {
                        spread_locked = false;
                        delay_pending = false;
                        if (canFire) {
                            MyController->simulate_input_key(keys::left_mouse, true);
                            MyController->simulate_input_key(keys::left_mouse, false);
                            auto inventory = MyShooter->GetInventory();
                            if (!inventory) return;

                            auto weapon = inventory->GetCurrentWeapon();
                            if (!weapon) return;

                            auto firing_state = weapon->get_firing_state();


                            //// Validate memory before reading
                            //if (!Memory::IsValidPtr((void*)((uintptr_t)firing_state + 0x44C), sizeof(float))) return;
                            //if (!Memory::IsValidPtr((void*)((uintptr_t)firing_state + 0x4D0), sizeof(float))) return;

                            float base = Memory::R<float>((uintptr_t)firing_state + 0x44C);

                            float modifier = Memory::R<float>((uintptr_t)firing_state + 0x4D0);

                            float effective = base * ((modifier > 0.01f && modifier < 10.0f) ? modifier : 1.0f);
                            float cooldownSeconds = 1.0f / effective;

                            static float lastTracerTick = 0.0f;

                            float currentTime = GetTickCount64() / 1000.0f;
                            bool isPressedNow = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
                            bool canSpawnTracer = ((currentTime - lastTracerTick) >= cooldownSeconds);

                            //// Extra safety: Don't trigger if we're holding a knife or invalid weapon
                            //if (!weapon->IsGun()) return;

                            // ✅ Get firing origin + view rotation
                            FVector eyeLocation;
                            FRotator viewRotation;

                            MyShooter->get_firing_location_and_direction2(&eyeLocation, &viewRotation, true);
                            FVector firingDir = RotatorToVector(viewRotation);
                            // Apply spread using your existing system
                            FVector spreadOffset = NoSpread::calc_spread(MyShooter, (uintptr_t)firing_state, weapon, firingDir);


           


                            draw_enemy_tracers(canvas, MyController, eyeLocation, viewRotation, spreadOffset, canSpawnTracer);

                            if (canSpawnTracer)
                                lastTracerTick = currentTime;
                            Globals::spin_paused = true;
                            Globals::waiting_for_kill = true;
                            Globals::resume_spin_time = GetTickCount() + 30;
                        }
                    }
                }





            }
        }
    }

    catch (const std::exception& e) {
    }
    catch (...) {
    }







    return pRender(_this, canvas, a3);
}
