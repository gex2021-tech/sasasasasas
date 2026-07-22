#pragma once
#include "memory.h"
#include "unrealengine.h"
#include "Decryptor.h"
#include "options.hpp"
#include "Hookign/ret_spoofing.h"
#include "Hookign/spoofer.h"
//#include "SDK/CoreUObject_classes.hpp"
//#include "SDK/CoreUObject_parameters.hpp"
//#include "SDK/CoreUObject_structs.hpp"
#include <algorithm>
#include <fstream> // Add this at the top for file I/O
#include <ctime> // needed for std::time, std::localtime

int index = 453;
int elements_count = 0;
FLinearColor BOSSRAINBOMAW(float time);
static FLinearColor fresnel = FLinearColor(0.f, 26.0f, 0.f, Globals::Glow1);
FLinearColor fresnel_team = FLinearColor(26.0f, 0.0f, 26.0f, 1.0f);
struct FColor
{
	uint8 R, G, B, Alpha;

	FColor()
		: R(0), G(0), B(0), Alpha(255) {
	}

	FColor(uint8 InR, uint8 InG, uint8 InB, uint8 InAlpha = 255)
		: R(InR), G(InG), B(InB), Alpha(InAlpha) {
	}
};
enum EBlendMode : uint8
{
	BLEND_Opaque = 0,
	BLEND_Masked = 1,
	BLEND_Translucent = 2,
	BLEND_Additive = 3,
	BLEND_Modulate = 4,
	BLEND_AlphaComposite = 5,
	BLEND_MAX = 6,
};
#define COLOR_BG RGBtoFLC(12, 12, 12)
#define COLOR_SIDEBAR RGBtoFLC(18, 18, 18)
#define COLOR_SUBTAB RGBtoFLC(20, 20, 20)
#define COLOR_TAB_ACTIVE RGBtoFLC(255, 255, 255)
#define COLOR_TAB_INACTIVE RGBtoFLC(110, 110, 110)
#define COLOR_TITLE_WHITE RGBtoFLC(225, 225, 225)
#define COLOR_TITLE_RED RGBtoFLC(200, 50, 50)
#define COLOR_RAINBOWBAR_HEIGHT 2.0f

#define MAX_COLORS 20


struct FAresDebugLineReplicated {
	FVector LineStart;         // 0x00
	FVector LineEnd;           // 0x18
	FColor Color;              // 0x30
	bool bPersistentLines;     // 0x34
	float LifeTime;            // 0x38
	uint8_t DepthPriority;     // 0x3C
	float Thickness;           // 0x40
};

FLinearColor colorPalette[MAX_COLORS];

FLinearColor HSVtoRGB2(float h, float s, float v)
{
	float R, G, B;

	int i = int(h * 6.0f);
	float f = h * 6.0f - i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch (i % 6) {
	case 0: R = v; G = t; B = p; break;
	case 1: R = q; G = v; B = p; break;
	case 2: R = p; G = v; B = t; break;
	case 3: R = p; G = q; B = v; break;
	case 4: R = t; G = p; B = v; break;
	case 5: R = v; G = p; B = q; break;
	}

	return FLinearColor(R, G, B, 1.0f);
}
FLinearColor HSVtoRGB22(float h, float s, float v)
{
	float r, g, b;

	int i = static_cast<int>(h * 6);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0: r = v, g = t, b = p; break;
	case 1: r = q, g = v, b = p; break;
	case 2: r = p, g = v, b = t; break;
	case 3: r = p, g = q, b = v; break;
	case 4: r = t, g = p, b = v; break;
	case 5: r = v, g = p, b = q; break;
	}

	return FLinearColor(r, g, b, 1.0f);
}
FLinearColor BOSSRAINBOMAW2(float time) {
	float speed = 0.5f;
	return FLinearColor(
		0.5f + 0.5f * sin(speed * time + 0.0f),
		0.5f + 0.5f * sin(speed * time + 2.0f),
		0.5f + 0.5f * sin(speed * time + 4.0f),
		1.0f
	);
}

//FLinearColor HSVToRGB(float H, float S, float V)
//{
//	float R = 0, G = 0, B = 0;
//
//	int i = FMath::FloorToInt(H * 6);
//	float f = H * 6 - i;
//	float p = V * (1 - S);
//	float q = V * (1 - f * S);
//	float t = V * (1 - (1 - f) * S);
//
//	switch (i % 6)
//	{
//	case 0: R = V; G = t; B = p; break;
//	case 1: R = q; G = V; B = p; break;
//	case 2: R = p; G = V; B = t; break;
//	case 3: R = p; G = q; B = V; break;
//	case 4: R = t; G = p; B = V; break;
//	case 5: R = V; G = p; B = q; break;
//	}
//
//	return FLinearColor(R, G, B, 1.0f);
//}

FLinearColor GetRainbowColor(float time)
{

	float red = (sin(time * 2.0f * 3.14159f / 3.0f) + 1.0f) / 2.0f;
	float green = (sin(time * 2.0f * 3.14159f / 3.0f + 2.0f * 3.14159f / 3.0f) + 1.0f) / 2.0f;
	float blue = (sin(time * 2.0f * 3.14159f / 3.0f + 4.0f * 3.14159f / 3.0f) + 1.0f) / 2.0f;
	return FLinearColor(red, green, blue, Globals::Glow1);
}
float rainbowTimeZChams = 0;
float rainbowTimeZHandChams = 0;
float rainbowTimeZHandFresnel = 0;
float rainbowTimeZskyboxRGB = 0;
float rainbowTimeZskyboxRGB2 = 0;
float rainbowTimeZskyboxRGB3 = 0;
float rainbowTimeZskyboxRGB4 = 0;
float rainbowTimeZCH = 0;
float rainbowTimeZFov = 0;
float rainbowTimeZ = 0;
float rainbowTimeZRGB = 0;
float rainbowTimeZRGB2 = 0;
float rainbowTimeZRGB3 = 0;
enum class e_blend_mode : uint8_t
{
	opaque = 0,
	masked = 1,
	translucent = 2,
	additive = 3,
	modulate = 4,
	alpha_composite = 5,
	max = 6,
};
struct FHitResult
{
	int32 FaceIndex; // 0x0
	float Time; // 0x4
	float Distance; // 0x8
	FVector Location; // 0x10
	FVector ImpactPoint; // 0x28
	FVector Normal; // 0x40
	FVector ImpactNormal; // 0x58
	FVector TraceStart; // 0x70
	FVector TraceEnd; // 0x88
	float PenetrationDepth; // 0xa0
	int32 MyItem; // 0xa4
	int32 Item; // 0xa8
	uint8 ElementIndex; // 0xac
	uint8 bBlockingHit : 1; // 0xad
	uint8 bStartPenetrating : 1; // 0xad
	uint8 PadFlags : 6; // padding bits
	uint8 Pad_AE[0x2]; // padding to align next member (0xb0)
	//TWeakObjectPtr<class UPhysicalMaterial> PhysMaterial; // 0xb0
	//FActorInstanceHandle HitObjectHandle; // 0xb8
	//TWeakObjectPtr<class UPrimitiveComponent> Component; // 0xd0
	FName BoneName; // 0xd8
	FName MyBoneName; // 0xe4
};


struct FWallSpanInfo {
	FHitResult Entrance;
	FHitResult Exit;
};

struct FWallSpanList {
	TArray<FWallSpanInfo> Spans;         // 0x00
	bool bLastPointInWall;               // 0x10
	uint8_t Pad_11[0x7];                 // 0x11
	FHitResult EntranceToLastPoint;      // 0x18
};

template<class type> class enum_as_byte {
public:
	enum_as_byte() {}
	enum_as_byte(type value) : value(static_cast<std::uint8_t>(value)) {}

	explicit enum_as_byte(std::int32_t value) : value(static_cast<std::uint8_t>(value)) {}
	explicit enum_as_byte(std::uint8_t value) : value(value) {}

	operator type() const { return type(value); }
	type get() const { return type(value); }
	type* cast() {
		return reinterpret_cast<type*>(this);
	}
private:
	std::uint8_t value;
};

static enum search_case : std::uint8_t {
	case_sensitive, ignore_case
};
struct SkinData {
	const wchar_t* SkinName;
	const wchar_t* skin;
	const wchar_t* chroma;
	int level;
};

namespace keys
{
	fkey_ left_mouse;
	fkey_ right_mouse;
	fkey_ insert;

	fkey_ w;
	fkey_ A;
	fkey_ s;
	fkey_ d;

	fkey_ space;
	fkey_ capslock;
}
template <typename T>
T Read1337(std::uintptr_t Address)
{
	if (!Memory::IsValidPointer(Address))
		return T{};


	return reinterpret_cast<T(*)(std::uintptr_t)>(VALORANT::Module + Offsets::Triggerveh)(Address - 0x8);
}
struct UTexture {
	uint8_t pad_0000[0xe0];        // Padding to LightingGuid
	int32_t LevelIndex;            // 0xf0
	int32_t LODBias;               // 0xf4
	uint8_t StreamingImportance;   // 0xf8
	// ...
};

class UObject
{
public:


	char padding_01[0x18];
	int32_t ComparisonIndex;

	static UObject* StaticFindObject(const wchar_t* Name)
	{

		static uintptr_t StaticFindObjectAddress = VALORANT::Module + Offsets::static_find_object;
		return reinterpret_cast<UObject * (__fastcall*)(UObject*, UObject*, const wchar_t*, bool, uintptr_t, void*)>(spoofcall_stub)(nullptr, reinterpret_cast<UObject*>(-1), Name, false, Offsets::MagicOffsets, (void*)StaticFindObjectAddress);
	}
	static UObject* StaticFindObject(UObject* Class, UObject* InOuter, const wchar_t* Name, bool ExactClass)
	{

		static uintptr_t StaticFindObjectAddress = VALORANT::Module + Offsets::static_find_object;
		return reinterpret_cast<UObject * (__fastcall*)(UObject*, UObject*, const wchar_t*, bool, uintptr_t, void*)>(spoofcall_stub)(Class, InOuter, Name, ExactClass, Offsets::MagicOffsets, (void*)StaticFindObjectAddress);
	}

	template<typename type = UObject> static inline type find_object2(const wchar_t* name, UObject* outer = nullptr, bool exact = false) {

		return reinterpret_cast<type>(UObject::StaticFindObject(nullptr, outer, name, exact));
	}
	template<typename type = UObject> static inline type* find_object1(const wchar_t* name, UObject* outer = reinterpret_cast<UObject*>(-1), bool exact = false) {

		return reinterpret_cast<type*>(UObject::StaticFindObject(nullptr, outer, name, exact));
	}
	template<typename type = UObject> static inline type* find_object(const wchar_t* name, UObject* outer = nullptr, bool exact = false) {

		return reinterpret_cast<type*>(UObject::StaticFindObject(nullptr, outer, name, exact));
	}

	static void ProcessEvent(void* class_, UObject* function, void* params)
	{

		static uintptr_t negritas = VALORANT::Module + Offsets::process_event;
		reinterpret_cast<void (*)(void*, UObject*, void*, uintptr_t, void*)>(spoofcall_stub)(class_, function, params, Offsets::MagicOffsets, (void*)negritas);
	}
	void ProcessEvent(void* class_, void* params)
	{

		static uintptr_t negritas = VALORANT::Module + Offsets::process_event;
		reinterpret_cast<void (*)(void*, UObject*, void*, uintptr_t, void*)>(spoofcall_stub)(class_, this, params, Offsets::MagicOffsets, (void*)negritas);
	}

	void ProcessEvent_(UObject* function, void* params) {

		static uintptr_t negritas = VALORANT::Module + Offsets::process_event;
		reinterpret_cast<void (*)(UObject*, UObject*, void*, uintptr_t, void*)>(spoofcall_stub)(this, function, params, Offsets::MagicOffsets, (void*)negritas);

	}


	void ProcessEventx(UObject* Function, void* args, void* out_args = nullptr, void* stack = nullptr) {

		if (!Function || !this)
			return;

		static void(__fastcall * process_event_func)(UObject*, UObject*, void*, void*, void*, uintptr_t, void*);
		if (!process_event_func) {
			process_event_func = reinterpret_cast<decltype(process_event_func)>(spoofcall_stub);
		}
		return process_event_func(this, Function, args, out_args, stack, Offsets::MagicOffsets, (void*)(VALORANT::Module + Offsets::process_event));
	}

	static UObject* StaticLoadObject(const wchar_t* ObjectPath) {

		if (!ObjectPath)
			return nullptr;


		static uintptr_t StaticLoadObjectAddress = VALORANT::Module + Offsets::static_load_object;
		return reinterpret_cast<UObject * (__fastcall*)(UObject*, UObject*, const wchar_t*, const wchar_t*, uint32_t, uint32_t, void*, uintptr_t, void*)>(spoofcall_stub)(nullptr, nullptr, ObjectPath, nullptr, 0, 0, nullptr, Offsets::MagicOffsets, (void*)StaticLoadObjectAddress);


	}


	template <typename type = UObject>
	type* cast() {
		return reinterpret_cast<type*>(this);
	}


};

struct UWorld
{
	static UWorld* GetWorld()
	{
		uintptr_t* UWorldRead_SECOND = Memory::R<uintptr_t*>(VALORANT::Module + Offsets::State); // wrong
		auto UWorld_RESULT_SECOND = Memory::R<UWorld*>(uintptr_t(UWorldRead_SECOND));
		return UWorld_RESULT_SECOND;
	}
};
UWorld* UWorldSave;
struct UFont : UObject
{

};
class fmemory {
public:
	static uint64_t malloc(int32_t size, uint32_t aligment);
};

uint64_t fmemory::malloc(int32_t size, uint32_t alignment) {
	if (size <= 0)
		return 0;

	using FMallocFn = uint64_t(__cdecl*)(uint64_t, int);

	static FMallocFn fn = reinterpret_cast<FMallocFn>(
		VALORANT::Module + Offsets::fmemory_malloc
		);

	if (!fn)
		return 0;

	// Call FMemory::Malloc via spoofed call
	return reinterpret_cast<uint64_t(__fastcall*)(uint64_t, int, uintptr_t, void*)>(spoofcall_stub)(
		size,
		alignment,
		Offsets::MagicOffsets,
		reinterpret_cast<void*>(fn)
		);
}
UObject* DefaultMediumFont;
struct UEngine
{
	UObject* GetMediumFont()
	{

		return Memory::R<UObject*>((uintptr_t)this + 0x98);
	}
};
// Use this if you're avoiding `enum class`
// Safe replacement using enum class
enum class EAttachmentRule

	: uint8

{


	KeepRelative

	= 0,

	KeepWorld

	= 1,

	SnapToTarget

	= 2,

	EAttachmentRule_MAX

	= 3

};
enum class EBoneSpaces

	: uint8

{


	WorldSpace

	= 0,

	ComponentSpace

	= 1,

	EBoneSpaces_MAX

	= 2

};
enum class EDetachmentRule

	: uint8

{


	KeepRelative

	= 0,

	KeepWorld

	= 1,

	EDetachmentRule_MAX

	= 2

};
uintptr_t FindSkyLightComponent()
{
	// Get UWorld from GEngine or known pointer
	uintptr_t GWorld = (uintptr_t)(UWorldSave);

	if (!GWorld) return 0;

	// Offset depends on engine version, usually +0x30
	uintptr_t PersistentLevel = *(uintptr_t*)(GWorld + 0x30);
	if (!PersistentLevel) return 0;

	int32_t ActorCount = *(int32_t*)(PersistentLevel + 0xA0);       // AActors.Num()
	uintptr_t* ActorArray = *(uintptr_t**)(PersistentLevel + 0x98); // AActors pointer

	for (int i = 0; i < ActorCount; i++) {
		uintptr_t Actor = ActorArray[i];
		if (!Actor) continue;

		// Optional: Check class name or vtable here
		// Example: Use reflection or known type ID, or call Actor->GetName()

		// Try casting to ASkyLight by checking for LightComponent at 0x3b8
		uintptr_t LightComponent = *(uintptr_t*)(Actor + 0x3b8);
		if (LightComponent) {
			return LightComponent; // Found valid SkyLightComponent
		}
	}

	return 0;
}




void SetSkyLowerHemisphereColor(uintptr_t LightComponent)
{
	if (!LightComponent) return;

	// Step 1: Set bLowerHemisphereIsBlack = false (offset 0x2e5)
	*(bool*)(LightComponent + 0x2e5) = false;

	// Step 2: Set new FLinearColor at 0x2e8
	FLinearColor newColor = { 1.0f, 0.0f, 1.0f, 1.0f }; // Light blue
	*(FLinearColor*)(LightComponent + 0x2e8) = newColor;
}

void ModifySkyColor()
{
	uintptr_t LightComponent = FindSkyLightComponent();
	if (LightComponent) {
		SetSkyLowerHemisphereColor(LightComponent);
	}
}
bool bOutline = 0;
bool InGame = 0;

struct FQuat
{
public:

	float X, Y, Z, W;

	FQuat() : X(0.f), Y(0.f), Z(0.f), W(0.f) {};

	FQuat(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}

	FQuat(struct FRotator& R);

	FVector RotateVector(const struct FVector& V) const;
};
struct FRotator
{
	double Pitch, Yaw, Roll;

	FRotator()
		: Pitch(0), Yaw(0), Roll(0)
	{
	}

	FRotator(double pitch, double yaw, double roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

	FRotator operator+ (const FRotator& other) const { return FRotator(Pitch + other.Pitch, Yaw + other.Yaw, Roll + other.Roll); }

	FRotator operator- (const FRotator& other) const { return FRotator(Pitch - other.Pitch, Yaw - other.Yaw, Roll - other.Roll); }

	FRotator operator* (double scalar) const { return FRotator(Pitch * scalar, Yaw * scalar, Roll * scalar); }

	FRotator& operator=  (const FRotator& other) { Pitch = other.Pitch; Yaw = other.Yaw; Roll = other.Roll; return *this; }

	FRotator& operator+= (const FRotator& other) { Pitch += other.Pitch; Yaw += other.Yaw; Roll += other.Roll; return *this; }

	FRotator& operator-= (const FRotator& other) { Pitch -= other.Pitch; Yaw -= other.Yaw; Roll -= other.Roll; return *this; }

	FRotator& operator*= (const double other) { Yaw *= other; Pitch *= other; Roll *= other; return *this; }

	struct FQuat Quaternion() const;
};
bool IsRotatorZero(const FRotator& rot)
{
	return rot.Pitch == 0.f && rot.Yaw == 0.f && rot.Roll == 0.f;
}
struct FDisplayName
{
	FString FullName;
};
struct UPlatformPlayer : public UObject
{

public:
	FDisplayName GetTrueDisplayName();  // ✅ NO CLASS NAME HERE!
};

enum class EChatRoomType : uint8_t
{
	Party = 0,
	Pregame = 1,
	All = 2,
	Team = 3,
	InGameSystem = 4,
	System = 5,
	Whisper = 6,
	Count = 7,
	EChatRoomType_MAX = 8,
};
class UThreadedChatManager : public UObject {
public:
	static UObject* GetDefaultObj() {
		static UObject* default_obj = UObject::find_object2<UObject*>(L"ShooterGame.Default__ThreadedChatManager");
		if (!default_obj) {
			//printf("[ThreadedChatManager] ❌ Failed to find Default__ThreadedChatManager\n");
		}
	
		return default_obj;
	}

	static UThreadedChatManager* GetThreadedChatManager(UWorld* world_context) {
		static UObject* fn = UObject::find_object<UObject>(L"ShooterGame.ThreadedChatManager.GetThreadedChatManager");

		if (!fn) {
			//printf("[ThreadedChatManager] ❌ Failed to find GetThreadedChatManager function\n");
			return nullptr;
		}

		struct {
			UWorld* world_context;
			UThreadedChatManager* return_value;
		} params{};

		params.world_context = world_context;

		UObject* obj = GetDefaultObj();
		if (!obj) {
			////printf("[ThreadedChatManager] ❌ DefaultObj is null\n");
			return nullptr;
		}

		//printf("[ThreadedChatManager] 📞 Calling GetThreadedChatManager (UWorld: %p)...\n", world_context);
		obj->ProcessEvent_(fn, &params);

		if (!params.return_value) {
			//printf("[ThreadedChatManager] ❌ Returned nullptr for ChatManager\n");
		}
	

		return params.return_value;
	}

	void SendChatMessageV2(EChatRoomType room_type, FText message) {
		if (!this) {
			//printf("[SendChatMessageV2] ❌ this == nullptr\n");
			return;
		}

		if (!message.Data) {
			//printf("[SendChatMessageV2] ❌ message.Data == nullptr\n");
			return;
		}

		static UObject* fn = UObject::find_object2<UObject*>(L"ShooterGame.ThreadedChatManager.SendChatMessageV2");
		if (!fn) {
			//printf("[SendChatMessageV2] ❌ Failed to find SendChatMessageV2 function\n");
			return;
		}

		struct {
			EChatRoomType room_type;
			FText message;
		} params{};

		params.room_type = room_type;
		params.message = message;

		//printf("[SendChatMessageV2]  Sending message to room type %d | FText: %p\n", static_cast<int>(room_type), message.Data);
		this->ProcessEvent_(fn, &params);
		//printf("[SendChatMessageV2] Message sent!\n");
	}
};

struct USkeletalMeshComponent : public UObject
{
	FVector get_bone_location(int32_t index) {
		FMatrix matrix;
		reinterpret_cast<FMatrix* (__fastcall*)(USkeletalMeshComponent*, FMatrix*, int)>(VALORANT::Module + Offsets::bone_matrix)(this, &matrix, index);
		return { matrix.WPlane.X, matrix.WPlane.Y, matrix.WPlane.Z };
	}
	int32_t GetNumMaterials()
	{

		static UObject* Function = 0;
		if (!Function) Function = this->StaticFindObject(ez(L"Engine.PrimitiveComponent.GetNumMaterials").d());

		struct {
			int32_t ReturnValue;
		} Params = { };

		this->ProcessEvent(this, Function, &Params);

		return Params.ReturnValue;
	}
	UObject* create_and_set_material_instance_dynamic_from_material(int32_t element_index, UObject* parent) {
		if (!this || !parent) return nullptr;

		static UObject* fn = nullptr;
		if (!fn)
			fn = UObject::find_object2<UObject*>(L"Engine.PrimitiveComponent.CreateAndSetMaterialInstanceDynamicFromMaterial");

		struct {
			int32_t element_index;
			UObject* parent;
			UObject* return_value;
		} params = { element_index, parent };

		this->ProcessEvent_(fn, &params);
		return params.return_value;
	}
	std::int32_t GetNumBones() {
		static UObject* Function = UObject::find_object2<UObject*>(ez(L"Engine.SkinnedMeshComponent.GetNumBones"));

		std::int32_t return_value = 0;
		Function->ProcessEvent_(Function, &return_value);

		return return_value;
	}

	void GetMesh3P()
	{

		UObject* function = UObject::find_object2<UObject*>(ez(L"ShooterGame.AresEquippable.GetMesh3P").d());

		struct
		{
			USkeletalMeshComponent* Out;
		}Parameters;
		function->ProcessEvent(this, function, &Parameters);

	}
	void GetCosmeticMesh3P()
	{

		UObject* function = UObject::find_object2<UObject*>(ez(L"ShooterGame.AresEquippable.GetCosmeticMesh3P").d());

		struct
		{
			USkeletalMeshComponent* Out;
		}Parameters;
		function->ProcessEvent(this, function, &Parameters);

	}

	bool K2_AttachToComponent(UObject* TargetComponent, UObject* Parent, FName SocketName,
		int LocationRule, int RotationRule, int ScaleRule, bool bWeldSimulatedBodies)
	{
		static UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.K2_AttachToComponent").d());

		struct
		{
			void* Parent;
			FName SocketName;
			int LocationRule;
			int RotationRule;
			int ScaleRule;
			bool bWeldSimulatedBodies;
			bool ReturnValue;
		} params;

		params.Parent = Parent;
		params.SocketName = SocketName;
		params.LocationRule = LocationRule;
		params.RotationRule = RotationRule;
		params.ScaleRule = ScaleRule;
		params.bWeldSimulatedBodies = bWeldSimulatedBodies;

		if (Function && TargetComponent)
			Function->ProcessEvent(TargetComponent, Function, &params);

		return params.ReturnValue;
	}


	void K2_DetachFromComponent(void* Target,
		int LocationRule, int RotationRule, int ScaleRule, bool bCallModify)
	{
		static UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.K2_DetachFromComponent").d());

		struct
		{
			int LocationRule;
			int RotationRule;
			int ScaleRule;
			bool bCallModify;
		} params;

		params.LocationRule = LocationRule;
		params.RotationRule = RotationRule;
		params.ScaleRule = ScaleRule;
		params.bCallModify = bCallModify;

		if (Function)
			Function->ProcessEvent(Target, Function, &params);
	}

	void SetRelativeLocation(const FVector& Location, bool bsweep, bool bteleport)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"Engine.SceneComponent.K2_SetRelativeLocation").d());

		if (function == nullptr)
			return;

		struct {
			FVector NewRotation;
			bool bsweep;
			uintptr_t FHitResult;
			bool bTeleport;
		} params = { Location, bsweep, 0, bteleport };

		this->ProcessEvent_(function, &params);
	}

	TArray<FName> GetAllSocketNames()
	{
		UObject* Function = UObject::find_object((L"Engine.SceneComponent.GetAllSocketNames"));

		struct
		{
			TArray<FName> ReturnValue;
		} params;

		Function->ProcessEvent(this, Function, &params);

		return params.ReturnValue;
	}

	void SetRelativeScale3D(const struct FVector& NewScale3D)
	{
		UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.SetRelativeScale3D").d());

		struct
		{
			struct FVector                                NewScale3D;
		}params;

		params.NewScale3D = NewScale3D;

		Function->ProcessEvent(this, Function, &params);
	}
	FVector GetSocketLocation(FName InSocketName)
	{
		UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.GetSocketLocation").d());

		struct
		{
			FName InSocketName;
			FVector ReturnValue;
		} params;

		params.InSocketName = InSocketName;

		Function->ProcessEvent(this, Function, &params);

		return params.ReturnValue;
	}

	void K2_SetRelativeRotation(const FRotator& NewRotation, bool bSweep, bool bTeleport)
	{
		UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.K2_SetRelativeRotation").d());

		struct
		{
			FRotator NewRotation;
		} params;

		params.NewRotation = NewRotation;

		Function->ProcessEvent(this, Function, &params);
	}
	void SetAspectRatio(float InAspectRatio)
	{
		UObject* Function = UObject::find_object(ez(L"Engine.CameraComponent.SetAspectRatio").d());

		struct
		{
			float InAspectRatio;
		} params;

		params.InAspectRatio = InAspectRatio;

		Function->ProcessEvent(this, Function, &params);
	}

	void set_world_rotation(FVector NewRotation, bool bsweep, bool bteleport)
	{

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"Engine.SceneComponent.K2_SetWorldRotation").d());

		if (function == nullptr)
			return;

		struct {
			FVector NewRotation;
			bool bsweep;
			uintptr_t FHitResult;
			bool bTeleport;
		} params = { NewRotation, bsweep, 0, bteleport };

		this->ProcessEvent_(function, &params);
	}
	void SetAresOutlineMode(EAresOutlineMode Mode, bool bPropagateToChildren)
	{

		auto function_name = ez(L"Engine.MeshComponent.SetAresOutlineMode").d();
		static UObject* Function;
		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
		{
			return;
		}
		struct
		{
			EAresOutlineMode Mode;
			bool bPropagateToChildren;
		} Parameters;
		Parameters.Mode = Mode;
		Parameters.bPropagateToChildren = bPropagateToChildren;
		Function->ProcessEvent(this, Function, &Parameters);
	}

	void SetMaterial(int32_t ElementIndex, UObject* Material) {

		UObject* function = UObject::find_object2<UObject*>(ez(L"Engine.PrimitiveComponent.SetMaterial").d());
		if (!function) {
			return;
		}

		struct {
			int32_t ElementIndex;
			UObject* Material;
		} Parameters;

		Parameters.ElementIndex = ElementIndex;
		Parameters.Material = Material;
		function->ProcessEvent_(function, &Parameters);
	}

	USkeletalMeshComponent* mesh3p() {
		static UObject* function;
		if (!function)
			function = UObject::find_object<UObject>(ez(L"ShooterGame.ShooterCharacter.GetCoreMesh3P").d());

		struct
		{
			USkeletalMeshComponent* output;
		} params;

		function->ProcessEvent_(function, &params);

		return params.output;
	}


	void SetOutlineMode(EAresOutlineMode Mode)
	{

		auto function_name = (ez(L"ShooterGame.AresOutlineComponent.SetOutlineMode").d());
		static UObject* Function;
		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
		{
			return;
		}
		struct
		{
			EAresOutlineMode Mode;
		} Parameters;
		Parameters.Mode = Mode;
		Function->ProcessEvent(this, Function, &Parameters);
	}


};


struct USceneComponentHelpers
{
	static void Detach(void* Target)
	{
		static UObject* fn = UObject::find_object(ez(L"Engine.SceneComponent.K2_DetachFromComponent").d());

		struct
		{
			int LocationRule;
			int RotationRule;
			int ScaleRule;
			bool bCallModify;
		} params;

		// 0 = KeepRelative
		params.LocationRule = 0;
		params.RotationRule = 0;
		params.ScaleRule = 0;
		params.bCallModify = true;

		if (Target && fn)
			fn->ProcessEvent(Target, fn, &params);
	}
	static void K2_DetachFromComponent(USkeletalMeshComponent* Target,
		int LocationRule, int RotationRule, int ScaleRule, bool bCallModify)
	{
		static UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.K2_DetachFromComponent").d());

		struct
		{
			int LocationRule;
			int RotationRule;
			int ScaleRule;
			bool bCallModify;
		} params;

		params.LocationRule = LocationRule;
		params.RotationRule = RotationRule;
		params.ScaleRule = ScaleRule;
		params.bCallModify = bCallModify;

		if (Function)
			Function->ProcessEvent(Target, Function, &params);
	}
	static FName GetAttachSocketName(USkeletalMeshComponent* TargetComponent)
	{
		static UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.GetAttachSocketName").d());

		struct
		{
			FName ReturnValue;
		} params;

		if (Function && TargetComponent)
			Function->ProcessEvent(TargetComponent, Function, &params);

		return params.ReturnValue;
	}

	static bool AttachTo(USkeletalMeshComponent* Target, USkeletalMeshComponent* Parent, FName SocketName,
		int LocationRule, int RotationRule, int ScaleRule, bool bWeldSimulatedBodies)
	{
		static UObject* Function = UObject::find_object(ez(L"Engine.SceneComponent.K2_AttachToComponent").d());

		struct
		{
			void* Parent;
			FName SocketName;
			int LocationRule;
			int RotationRule;
			int ScaleRule;
			bool bWeldSimulatedBodies;
			bool ReturnValue;
		} params;

		params.Parent = Parent;
		params.SocketName = SocketName;
		params.LocationRule = LocationRule;
		params.RotationRule = RotationRule;
		params.ScaleRule = ScaleRule;
		params.bWeldSimulatedBodies = bWeldSimulatedBodies;

		if (Function && Target)
			Function->ProcessEvent(Target, Function, &params);

		return params.ReturnValue;
	}
	static void SetRelativeLocation(void* Target, const FVector& Location, bool bSweep = false, bool bTeleport = true)
	{
		static UObject* fn = UObject::find_object(ez(L"Engine.SceneComponent.K2_SetRelativeLocation").d());

		struct
		{
			FVector NewLocation;
			bool bSweep;
			FHitResult SweepHitResult;  // output param
			bool bTeleport;
		} params;

		params.NewLocation = Location;
		params.bSweep = bSweep;
		params.bTeleport = bTeleport;
		memset(&params.SweepHitResult, 0, sizeof(FHitResult)); // safe zero init

		if (Target && fn)
			fn->ProcessEvent(Target, fn, &params);
	}
	static void SetRelativeRotation(void* Target, const FRotator& Location, bool bSweep = false, bool bTeleport = true)
	{
		static UObject* fn = UObject::find_object(ez(L"Engine.SceneComponent.K2_SetRelativeRotation").d());

		struct
		{
			FRotator NewLocation;
			bool bSweep;
			FHitResult SweepHitResult;  // output param
			bool bTeleport;
		} params;

		params.NewLocation = Location;
		params.bSweep = bSweep;
		params.bTeleport = bTeleport;
		memset(&params.SweepHitResult, 0, sizeof(FHitResult)); // safe zero init

		if (Target && fn)
			fn->ProcessEvent(Target, fn, &params);
	}
	static UObject* ImportFileAsTexture2D(UWorld* WorldContextObject, const FString& Filename)
	{
		static UObject* fn = UObject::find_object(L"Engine.KismetRenderingLibrary.ImportFileAsTexture2D");

		/*		if (!fn || !WorldContextObject || !Memory::IsValidPointer((uintptr_t)WorldContextObject))
					return nullptr;*/

		struct
		{
			UWorld* WorldContextObject;
			FString Filename;
			UObject* ReturnValue; // UTexture2D* technically, but UObject* works fine here
		} params = { WorldContextObject, Filename };

		fn->ProcessEvent_(fn, &params);
		return params.ReturnValue;
	}





};
inline UWorld* GWorld = nullptr;
struct UCanvas : public UObject
{
	void DrawBulletTracer(UObject* WorldContextObject, FVector Start, FVector End, FLinearColor Color, float Duration = 1.0f, float Thickness = 1.5f)
	{
		static UObject* fn = nullptr;

		if (!fn)
			fn = UObject::StaticFindObject(nullptr, nullptr, L"Engine.KismetSystemLibrary.DrawDebugLine", false);

		struct
		{
			UObject* WorldContextObject;
			FVector LineStart;
			FVector LineEnd;
			FLinearColor LineColor;
			float Duration;
			float Thickness;
		} Args;

		Args.WorldContextObject = WorldContextObject; // ✅
		Args.LineStart = Start;
		Args.LineEnd = End;
		Args.LineColor = Color;
		Args.Duration = Duration;
		Args.Thickness = Thickness;

		this->ProcessEventx(fn, &Args);
	}
	void K2_DrawBox(struct FVector2D ScreenPosition, FVector2D ScreenSize, float Thickness, FLinearColor RenderColor)
	{

		auto function_name = (L"Engine.Canvas.K2_DrawBox");
		static UObject* function;
		if (!function)
			function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			FVector2D ScreenPosition;
			FVector2D ScreenSize;
			float Thickness;
			FLinearColor RenderColor;
		} Args;

		Args.ScreenPosition = ScreenPosition;
		Args.ScreenSize = ScreenSize;
		Args.Thickness = Thickness;
		Args.RenderColor = RenderColor;
		this->ProcessEventx(function, &Args);
	}

	void K2_DrawPolygon(UObject* RenderTexture, FVector2D ScreenPosition, FVector2D Radius, int32 NumberOfSides, FLinearColor RenderColor)
	{
		auto function_name = (L"Engine.Canvas.K2_DrawPolygon");
		static UObject* function = nullptr;
		if (!function)
			function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			UObject* RenderTexture;
			FVector2D ScreenPosition;
			FVector2D Radius;
			int32 NumberOfSides;
			FLinearColor RenderColor;
		} Args;

		Args.RenderTexture = RenderTexture;
		Args.ScreenPosition = ScreenPosition;
		Args.Radius = Radius;
		Args.NumberOfSides = NumberOfSides;
		Args.RenderColor = RenderColor;

		this->ProcessEventx(function, &Args);
	}

	void K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, float Thickness, FLinearColor RenderColor)
	{
		UObject* Function = UObject::StaticFindObject(ez(L"Engine.Canvas.K2_DrawLine").d());
		if (!Function)
			return;

		struct
		{
			FVector2D ScreenPositionA;
			FVector2D ScreenPositionB;
			float Thickness;
			FLinearColor RenderColor;
		}Parameters;
		Parameters.ScreenPositionA = ScreenPositionA;
		Parameters.ScreenPositionB = ScreenPositionB;
		Parameters.Thickness = Thickness;
		Parameters.RenderColor = RenderColor;
		this->ProcessEventx(Function, &Parameters);
	}
	void K2_DrawTransparentLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, float Thickness, FLinearColor RenderColor)
	{
		UObject* Function = UObject::StaticFindObject(ez(L"Engine.Canvas.K2_DrawTransparentLine").d());
		if (!Function)
			return;

		struct
		{
			FVector2D ScreenPositionA;
			FVector2D ScreenPositionB;
			float Thickness;
			FLinearColor RenderColor;
		}Parameters;
		Parameters.ScreenPositionA = ScreenPositionA;
		Parameters.ScreenPositionB = ScreenPositionB;
		Parameters.Thickness = Thickness;
		Parameters.RenderColor = RenderColor;
		this->ProcessEventx(Function, &Parameters);
	}

	void K2_DrawTexture(UObject* texture, FVector2D screen_position, FVector2D screen_size, FVector2D coordinate_position, FVector2D coordinate_size, FLinearColor render_color, int blend_mode, float rotation, FVector2D pivot_point) {
		UObject* function = UObject::StaticFindObject(ez(L"Engine.Canvas.K2_DrawTexture").d());

		struct {
			UObject* texture;
			FVector2D screen_position;
			FVector2D screen_size;
			FVector2D coordinate_position;
			FVector2D coordinate_size;
			FLinearColor render_color;
			int blend_mode;
			float rotation;
			FVector2D pivot_point;
		} params = { texture, screen_position, screen_size, coordinate_position, coordinate_size, render_color, blend_mode, rotation, pivot_point };

		this->ProcessEventx(function, &params);
	}

	void K2_DrawTextx(const FString& text, const FVector2D& position, const FVector2D& scale, const FLinearColor& color, float kerning, const FLinearColor& shadow_color, const FVector2D& shadow_offset, bool centre_x, bool centre_y, bool outlined, const FLinearColor& outline_color) {
		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"Engine.Canvas.K2_DrawText", false);
		if (!Function)
			return;

		if (!DefaultMediumFont)
			DefaultMediumFont = UObject::StaticFindObject(nullptr, nullptr, L"/Engine/EngineFonts/Tahoma.Tahoma", false);

		struct {
			UObject* font;
			FString text;
			FVector2D position;
			FVector2D scale;
			FLinearColor color;
			float kerning;
			FLinearColor shadow_color;
			FVector2D shadow_offset;
			bool centre_x;
			bool centre_y;
			bool outlined;
			FLinearColor outline_color;
		} params = { DefaultMediumFont, text, position, scale, color, kerning, shadow_color, shadow_offset, centre_x, centre_y, outlined, outline_color };

		this->ProcessEventx(Function, &params);
	}


	void K2_DrawText(const FString& RenderText, const FVector2D& ScreenPosition,
		const FVector2D& Scale, const FLinearColor& RenderColor,
		float Kerning, const FLinearColor& ShadowColor,
		const FVector2D& ShadowOffset, bool bCentreX, bool bCentreY,
		bool bOutlined, const FLinearColor& OutlineColor)
	{
		if (!this) {
			//printf("[K2_DrawText] ERROR: 'this' is null\n");
			return;
		}

		static UObject* function = nullptr;
		if (!function) {
			function = UObject::StaticFindObject(
				nullptr, nullptr,
				L"Engine.Canvas.K2_DrawText", // Correct path
				false
			);
		}
		if (!function) {
			//printf("[K2_DrawText] ERROR: Could not find UFunction\n");
			return;
		}

		if (!DefaultMediumFont) {
			//printf("[K2_DrawText] ERROR: DefaultMediumFont is null\n");
			return;
		}

		struct
		{
			UObject* RenderFont;
			FString RenderText;
			FVector2D ScreenPosition;
			FVector2D Scale;
			FLinearColor RenderColor;
			float Kerning;
			FLinearColor ShadowColor;
			FVector2D ShadowOffset;
			bool bCentreX;
			bool bCentreY;
			bool bOutlined;
			FLinearColor OutlineColor;
		} Args;

		Args.RenderFont = DefaultMediumFont;
		Args.RenderText = RenderText;
		Args.ScreenPosition = ScreenPosition;
		Args.Scale = Scale;
		Args.RenderColor = RenderColor;
		Args.Kerning = Kerning;
		Args.ShadowColor = ShadowColor;
		Args.ShadowOffset = ShadowOffset;
		Args.bCentreX = bCentreX;
		Args.bCentreY = bCentreY;
		Args.bOutlined = bOutlined;
		Args.OutlineColor = OutlineColor;

		this->ProcessEventx(function, &Args);
	}

	FVector2D GetScreenSize() { //int32_t
		return FVector2D(Memory::R<float>(std::uintptr_t(this) + Offsets::ClipX), Memory::R<float>(std::uintptr_t(this) + Offsets::ClipY));
	}
};
FDisplayName UPlatformPlayer::GetTrueDisplayName()
{
	static UObject* function = nullptr;
	if (!function)
		function = UObject::find_object<UObject>(ez(L"ShooterGame.UPlatformPlayer.GetTrueDisplayName").d());

	struct {
		FDisplayName ReturnValue;
	} params;

	this->ProcessEvent_(function, &params);
	return params.ReturnValue;
}

struct UGameInstance
{
	struct ULocalPlayer* LocalPlayer()
	{

		const auto localplayers = Memory::R<ULocalPlayer*>((uintptr_t)this + 0x40);
		return Memory::R<ULocalPlayer*>((uintptr_t)localplayers);
	}
	UEngine* GetUEngine()
	{

		return Memory::R<UEngine*>((uintptr_t)this + 0x28);
	}
};

struct ULocalPlayer
{
	struct UGameViewportClient* ViewportClient()
	{

		return Memory::R<UGameViewportClient*>((uintptr_t)this + 0x80);
	}
	struct APlayerController* GetController()
	{

		return Memory::R<APlayerController*>((uintptr_t)this + 0x38);
	};
};
struct UGameViewportClient
{
	UWorld* GetUWorld()
	{

		return Memory::R<UWorld*>((uintptr_t)this + 0x80);
	}
	UGameInstance* GetGameInstance()
	{

		return Memory::R<UGameInstance*>((uintptr_t)this + 0x88);
	}
};

struct FBoxSphereBounds {
	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
};

namespace AresOutlineComponent
{
	UObject* Static_Class()
	{

		auto class_name = ez(L"ShooterGame.Default__AresOutlineComponent").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	void SetOutlineMode(EAresOutlineMode OutlineMode)
	{

		auto function_name = ez(L"ShooterGame.AresOutlineComponent.SetOutlineMode").d();
		static UObject* Function;
		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
		{
			return;
		}

		struct
		{
			EAresOutlineMode OutlineMode;
		} Parameters;

		Parameters.OutlineMode = OutlineMode;
		Function->ProcessEvent(Static_Class(), Function, &Parameters);
	}
}

struct UAresOutlineComponent
{
	UObject* Static_Class()
	{

		auto class_name = ez(L"ShooterGame.Default__AresOutlineComponent").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	void SetOutlineMode(EAresOutlineMode OutlineMode)
	{

		auto function_name = ez(L"ShooterGame.AresOutlineComponent.SetOutlineMode").d();
		static UObject* Function;
		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
		{
			return;
		}

		struct
		{
			EAresOutlineMode OutlineMode;
		} Parameters;

		Parameters.OutlineMode = OutlineMode;
		Function->ProcessEvent(Static_Class(), Function, &Parameters);
	}
};

DWORD64 ProjectWorldToScreen;

struct APlayerState : public UObject
{
	FString GetPlayerName()
	{
		auto function_name = ez(L"Engine.PlayerState.GetPlayerName").d();
		static UObject* function;
		if (!function)
			function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!function)
			return ez(L"AAresPlayerCharacter_PC_C").d();

		struct
		{
			FString Return;
		} Args;

		function->ProcessEvent(this, function, &Args);
		return Args.Return;
	}

};






namespace classes {
	namespace defines {
		static UObject* system = nullptr;
		static UObject* game_statics = nullptr;
		static UObject* blueprint = nullptr;
		static UObject* content_library = nullptr;
		static UObject* string_library_name = nullptr;
		static UObject* text_library = nullptr;
		static UObject* math_system = nullptr;
		static UObject* material_library = nullptr;
	}

	static inline void init()
	{
		auto math_system = (L"Engine.Default__KismetMathLibrary");

		auto game_statics_name = (L"Engine.Default__GameplayStatics");
		auto blueprint_name = (L"ShooterGame.Default__ShooterBlueprintLibrary");
		auto content_library_name = (L"ShooterGame.Default__ContentLibrary");
		auto system_name = (L"Engine.Default__KismetSystemLibrary");
		auto string_library_name = (L"Engine.Default__KismetStringLibrary");
		auto text_library_name = (L"Engine.Default__KismetTextLibrary"); // ✅ CORRECTED
		defines::math_system = UObject::StaticFindObject(nullptr, nullptr, math_system, false);
		defines::game_statics = UObject::StaticFindObject(nullptr, nullptr, game_statics_name, false);
		defines::blueprint = UObject::StaticFindObject(nullptr, nullptr, blueprint_name, false);
		defines::content_library = UObject::StaticFindObject(nullptr, nullptr, content_library_name, false);
		defines::system = UObject::StaticFindObject(nullptr, nullptr, system_name, false);
		defines::string_library_name = UObject::StaticFindObject(nullptr, nullptr, string_library_name, false);
		defines::text_library = UObject::StaticFindObject(nullptr, nullptr, text_library_name, false);
	}

}
class MAWGAGO {
public:
	UObject* gameobject = UObject::find_object2<UObject*>(L"ShooterGame.GameObject");
	UObject* pawn = UObject::find_object2<UObject*>(L"ShooterGame.Pawn");
	UObject* patch = UObject::find_object2<UObject*>(L"ShooterGame.PatchManager");
	UObject* projectile = UObject::find_object2<UObject*>(L"ShooterGame.Projectile");


};
std::wstring ToLower(const std::wstring& str) {
	std::wstring result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::towlower);
	return result;
}
enum class ETraceTypeQuery : uint8_t
{
	TraceTypeQuery1 = 0,
	TraceTypeQuery2 = 1,
	TraceTypeQuery3 = 2,
	TraceTypeQuery4 = 3,
	TraceTypeQuery5 = 4,
	TraceTypeQuery6 = 5,
	TraceTypeQuery7 = 6,
	TraceTypeQuery8 = 7,
	TraceTypeQuery9 = 8,
	TraceTypeQuery10 = 9,
	TraceTypeQuery11 = 10,
	TraceTypeQuery12 = 11,
	TraceTypeQuery13 = 12,
	TraceTypeQuery14 = 13,
	TraceTypeQuery15 = 14,
	TraceTypeQuery16 = 15,
	TraceTypeQuery17 = 16,
	TraceTypeQuery18 = 17,
	TraceTypeQuery19 = 18,
	TraceTypeQuery20 = 19,
	TraceTypeQuery21 = 20,
	TraceTypeQuery22 = 21,
	TraceTypeQuery23 = 22,
	TraceTypeQuery24 = 23,
	TraceTypeQuery25 = 24,
	TraceTypeQuery26 = 25,
	TraceTypeQuery27 = 26,
	TraceTypeQuery28 = 27,
	TraceTypeQuery_MAX = 28
};
// Unreal Engine's EDrawDebugTrace (simplified)
enum EDrawDebugTrace : uint8_t
{
	None = 0,
	ForOneFrame = 1,
	ForDuration = 2,
	Persistent = 3,
	EDrawDebugTrace_MAX = 4
};
class AActor : public UObject {
public:

};
namespace kismentsystemlibrary
{
	UObject* static_class()
	{

		auto class_name = ez(L"Engine.Default__KismetSystemLibrary").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}
	void DrawBulletTracer(UWorld* WorldContextObject, FVector Start, FVector End, FLinearColor Color, float Duration = 1.0f, float Thickness = 1.5f)
	{
		static UObject* fn = nullptr;

		if (!fn)
			fn = UObject::StaticFindObject(nullptr, nullptr, L"Engine.KismetSystemLibrary.DrawDebugLine", false);

		struct
		{
			UWorld* WorldContextObject;
			FVector LineStart;
			FVector LineEnd;
			FLinearColor LineColor;
			float Duration;
			float Thickness;
		} Args;

		Args.WorldContextObject = WorldContextObject; // ✅
		Args.LineStart = Start;
		Args.LineEnd = End;
		Args.LineColor = Color;
		Args.Duration = Duration;
		Args.Thickness = Thickness;

		fn->ProcessEvent(static_class(), fn, &Args);
	}
	FString get_object_name(UObject* obj)
	{

		auto function_name = (L"Engine.KismetSystemLibrary.GetObjectName");
		static UObject* functions;
		if (!functions)
			functions = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			UObject* obj;
			FString out;
		} Args;

		Args.obj = obj;
		functions->ProcessEvent(static_class(), functions, &Args);
		return Args.out;
	}





	static inline UObject* get_outer_object(UObject* object)
	{

		auto function_name = ez(L"Engine.KismetSystemLibrary.GetOuterObject").d();
		static UObject* functions;
		if (!functions)
			functions = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			UObject* object;
			UObject* return_value;
		} params = { object };

		functions->ProcessEvent(static_class(), &params);
		return params.return_value;
	}

	UObject* static_class_string()
	{

		auto class_name = ez(L"Engine.Default__KismetStringLibrary").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	FName_ Conv_StringToName(FString InString)
	{

		auto function_name = ez(L"Engine.KismetStringLibrary.Conv_StringToName").d();
		static UObject* Function = nullptr;
		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			FString InString;
			FName_ ReturnValue;
		} Params = { InString };

		classes::defines::string_library_name->ProcessEvent(static_class_string(), Function, &Params);

		return Params.ReturnValue;
	}
	FName string_to_name(FString string) {
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>((L"Engine.KismetStringLibrary.Conv_StringToName"));

		struct
		{
			FString string;
			FName output;
		} params;

		params.string = string;

		classes::defines::string_library_name->ProcessEvent_(function, &params);

		return params.output;
	}
	FText Conv_StringToText(FString InString)
	{
		// Cached pointer to the Kismet function
		static UObject* function = nullptr;
		if (!function)
		{
			function = UObject::find_object2<UObject*>(L"Engine.KismetTextLibrary.Conv_StringToText");

			if (!function)
			{
				
				return {};
			}

			
		}

		// Validate text library object
		if (!classes::defines::text_library)
		{
			
			return {};
		}

		// Set up parameters
		struct
		{
			FString string;
			FText output;
		} params{};

		params.string = InString;

		

		// Process the event
		classes::defines::text_library->ProcessEvent_(function, &params);

	

		return params.output;
	}

	FString TextToString(FText str)
	{

		auto function_name = ez(L"Engine.KismetTextLibrary.Conv_TextToString").d();
		static UObject* Function;
		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			FText string;
			FString out;
		} Args;

		Args.string = str;
		Function->ProcessEvent(static_class(), Function, &Args);
		return Args.out;
	}
	FText string_to_text(const FString& InString)
	{
		static UObject* function = nullptr;
		if (!function) {
			function = UObject::find_object2<UObject*>(ez(L"Engine.KismetTextLibrary.Conv_StringToText"));
			if (!function)
				printf("[TextLib] Failed to find StringToText function.\n");
			else
				printf("[TextLib] Found StringToText function: %p\n", function);
		}

		struct
		{
			FString InString;
			FText ReturnValue;
		} params;

		params.InString = InString;

		if (!classes::defines::text_library) {
			printf("[TextLib] text_library pointer is null! Aborting conversion.\n");
			return FText();
		}

		printf("[TextLib] Calling ProcessEvent for StringToText...\n");
		//printf("[TextLib] Input FString length: %llu\n", InString.count());
		printf("[TextLib] Input FString pointer: %p\n", InString.c_str());

		function->ProcessEvent(static_class(), function, &params);

		printf("[TextLib] Conversion complete. Returning FText.\n");
		return params.ReturnValue;
	}



	int32_t random_int(int32_t min, int32_t max) {
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"Engine.KismetMathLibrary.RandomIntegerInRange");

		struct
		{
			int32_t min;
			int32_t max;
			int32_t output;
		} params = { min, max };


		classes::defines::math_system->ProcessEvent_(function, &params);
		return params.output;
	}
	void DrawDebugLineOnAllClients(
		UWorld* WorldContextObject,
		const FVector& LineStart,
		const FVector& LineEnd,
		const FLinearColor& LineColor,
		bool bPersistentLines = false,
		float LifeTime = 1.0f,
		uint8 DepthPriority = 0,
		float Thickness = 1.0f
	)
	{
		static UObject* Function = nullptr;
		if (!Function)
			Function = UObject::find_object2<UObject*>(L"ShooterGame.ShooterBlueprintLibrary.DrawDebugLineOnAllClients");

		struct
		{
			UWorld* WorldContextObject;
			FVector LineStart;
			FVector LineEnd;
			FLinearColor LineColor;
			bool bPersistentLines;
			float LifeTime;
			uint8 DepthPriority;
			float Thickness;
		} Params;

		Params.WorldContextObject = WorldContextObject;
		Params.LineStart = LineStart;
		Params.LineEnd = LineEnd;
		Params.LineColor = LineColor;
		Params.bPersistentLines = bPersistentLines;
		Params.LifeTime = LifeTime;
		Params.DepthPriority = DepthPriority;
		Params.Thickness = Thickness;

		classes::defines::system->ProcessEvent_(Function, &Params);
	}



	bool LineTraceSingle(
		UWorld* WorldContextObject,
		FVector Start,
		FVector End,
		ETraceTypeQuery TraceChannel,
		bool bTraceComplex,
		TArray<AActor*> ActorsToIgnore,
		EDrawDebugTrace DrawDebugType,
		FHitResult& OutHit,
		bool bIgnoreSelf,
		FLinearColor TraceColor,
		FLinearColor TraceHitColor,
		float DrawTime
	)
	{
		static UObject* function = nullptr;
		if (!function)
			function = UObject::find_object2<UObject*>(L"Engine.KismetSystemLibrary.LineTraceSingle");

		struct
		{
			UWorld* WorldContextObject;
			FVector Start;
			FVector End;
			ETraceTypeQuery TraceChannel;
			bool bTraceComplex;
			TArray<AActor*> ActorsToIgnore;
			char  DrawDebugType;
			FHitResult OutHit;
			bool bIgnoreSelf;
			FLinearColor TraceColor;
			FLinearColor TraceHitColor;
			float DrawTime;
			bool ReturnValue; // must match UE function return
		} params;

		params.WorldContextObject = WorldContextObject;
		params.Start = Start;
		params.End = End;
		params.TraceChannel = TraceChannel;
		params.bTraceComplex = bTraceComplex;
		params.ActorsToIgnore = ActorsToIgnore;
		params.DrawDebugType = DrawDebugType;
		params.bIgnoreSelf = bIgnoreSelf;
		params.TraceColor = TraceColor;
		params.TraceHitColor = TraceHitColor;
		params.DrawTime = DrawTime;
	/*	if (!WorldContextObject)
		{
			printf("[ERROR] WorldContextObject is NULL!\n");
		}
		else
		{
			printf("[DEBUG] World is valid: %p\n", WorldContextObject);
		}*/

		classes::defines::system->ProcessEvent_(function, &params);

		OutHit = params.OutHit;
		//// ✅ Debug prints
		//printf("=== LineTraceSingle ===\n");
		//printf("Start: [%.2f, %.2f, %.2f]\n", Start.X, Start.Y, Start.Z);
		//printf("End:   [%.2f, %.2f, %.2f]\n", End.X, End.Y, End.Z);
		//printf("Hit:   %s\n", params.OutHit.bBlockingHit ? "true" : "false");
		//printf("ImpactPoint: [%.2f, %.2f, %.2f]\n", params.OutHit.ImpactPoint.X, params.OutHit.ImpactPoint.Y, params.OutHit.ImpactPoint.Z);
		////printf("HitActor: %p\n", params.OutHit.Actor);
		//printf("ReturnValue: %s\n", params.ReturnValue ? "true" : "false");
		return params.ReturnValue;
	}

	static inline bool contains(const wchar_t* search_in, const wchar_t* sub_string, bool use_case = false, bool search_from_end = false)
	{

		auto function_name = ez(L"Engine.KismetStringLibrary.Contains").d();
		struct
		{
			FString search_in;
			FString sub_string;
			bool use_case;
			bool search_from_end;
			bool return_value;
		} params = { search_in, sub_string, use_case, search_from_end };

		classes::defines::text_library->ProcessEvent_(UObject::find_object(function_name), &params), function_name;

		return params.return_value;
	}
	static inline bool contains2(FString search_in, FString sub_string, bool use_case = false, bool search_from_end = false)
	{

		auto function_name = ez(L"Engine.KismetStringLibrary.Contains").d();
		struct
		{
			FString search_in;
			FString sub_string;
			bool use_case;
			bool search_from_end;
			bool return_value;
		} params = { search_in, sub_string, use_case, search_from_end };

		classes::defines::text_library->ProcessEvent_(UObject::find_object(function_name), &params), function_name;

		return params.return_value;
	}
}



struct FMath {
	static __forceinline void SinCos(float* ScalarSin, float* ScalarCos, float  Value);
	static __forceinline float Fmod(float X, float Y);
	static __forceinline float DegreesToRadians(float deg);
	template<class T>
	static __forceinline T Clamp(const T X, const T Min, const T Max) { return X < Min ? Min : X < Max ? X : Max; }
	template<class T>
	static __forceinline T Min(const T A, const T B) { return A < B ? A : B; }
	template<class T>
	static __forceinline T Max(const T A, const T B) { return A > B ? A : B; }
	template<class T>
	static __forceinline T Lerp(const T A, const T B, const float Alpha) { return A + Alpha * (B - A); }
	static __forceinline FLinearColor LerpColor(const FLinearColor& A, const FLinearColor& B, float Alpha)
	{
		return FLinearColor(
			A.R + (B.R - A.R) * Alpha,
			A.G + (B.G - A.G) * Alpha,
			A.B + (B.B - A.B) * Alpha,
			A.A + (B.A - A.A) * Alpha
		);
	}

};
struct FTransform
{
	FQuat Rotation;
	FVector Translation;
	char UnknownData00[0x4];
	FVector Scale3D;
	char UnknownData01[0x4];

	/** Default constructor. */
	FTransform() : Rotation(0.f, 0.f, 0.f, 1.f), Translation(0.f), Scale3D(FVector::OneVector) {};

	FVector TransformPosition(FVector& V) const;

	FTransform(const FRotator& InRotation) : Rotation(InRotation.Quaternion()), Translation(FVector::ZeroVector), Scale3D(FVector::OneVector) {};
};

class USkyLightComponent : public UObject
{
public:

	void set_light_color(FLinearColor new_light_color, bool bSRGB)
	{
		static UObject* function = nullptr;
		if (!function)
			function = UObject::find_object2<UObject*>(L"LightComponent.SetLightColor");

		if (!function)
		{
			printf("[Error] Failed to find UFunction: LightComponent.SetLightColor");
			return;
		}

		struct {
			FLinearColor new_light_color;
			bool bSRGB;
		} params = { new_light_color, bSRGB };

		this->ProcessEvent_(function, &params);
	}
	void set_lower_hemisphere_color(FLinearColor in_lower_hemisphere_color)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"SkyLightComponent.SetLowerHemisphereColor");

		struct {
			FLinearColor in_lower_hemisphere_color;
		} params = { in_lower_hemisphere_color };

		this->ProcessEvent_(function, &params);
	}
};

struct UMaterialInstance : UObject
{
public:
	e_blend_mode GetBlendMode();

};
class UTexture2D; // Forward declaration
struct UMaterialInstanceDynamic : UMaterialInstance {
	e_blend_mode GetBlendMode() {

		UObject* function = UObject::find_object2<UObject*>(ez(L"MaterialInterface.GetBlendMode").d());

		struct {
			e_blend_mode ReturnValue;
		} Params = {  };

		this->ProcessEvent_(function, &Params);

		return Params.ReturnValue;
	}
	void set_scalar_parameter_value(FName parameter_name, float value)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"MaterialInstanceDynamic.SetScalarParameterValue");

		struct {
			FName parameter_name;
			float value;
		} params = { parameter_name, value };

		this->ProcessEvent_(function, &params);
	}
	void set_scalar_parameter_value2(FName_ parameter_name, float value)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"MaterialInstanceDynamic.SetScalarParameterValue");

		struct {
			FName_ parameter_name;
			float value;
		} params = { parameter_name, value };

		this->ProcessEvent_(function, &params);
	}


	void set_vector_parameter_value(FName parameter_name, FLinearColor value)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"MaterialInstanceDynamic.SetVectorParameterValue");

		struct {
			FName parameter_name;
			FLinearColor value;
		} params = { parameter_name, value };

		this->ProcessEvent_(function, &params);
	}
	void set_vector_parameter_value2(FName_ parameter_name, FLinearColor value)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"MaterialInstanceDynamic.SetVectorParameterValue");

		struct {
			FName_ parameter_name;
			FLinearColor value;
		} params = { parameter_name, value };

		this->ProcessEvent_(function, &params);
	}
	void SetVectorParameterValue(FName_ ParameterName, FLinearColor Value)
	{

		static UObject* Function;

		if (!Function) Function = UObject::StaticFindObject(ez(L"MaterialInstanceDynamic.SetVectorParameterValue").d());

		struct {
			FName_ ParameterName;
			int shift;
			FLinearColor Value;
		} Params = { ParameterName, 0, Value };

		this->ProcessEvent(this, Function, &Params);
	}

	void SetScalarParameterValue(FName_ ParameterName, float Value) {

		static UObject* Function = nullptr;
		if (!Function) {
			Function = UObject::StaticFindObject(ez(L"MaterialInstanceDynamic.SetScalarParameterValue").d());
		}

		struct {
			FName_ ParameterName;
			int shift;
			float Value;
		} Params = { ParameterName,0, Value };

		this->ProcessEvent(this, Function, &Params);
	}

	void SetTextureParameterValue(struct FName ParameterName, UObject* Texture) {

		static UObject* Function = 0;
		if (!Function) Function = UObject::StaticFindObject(ez(L"MaterialInstanceDynamic.SetTextureParameterValue").d());

		struct {
			FName ParameterName;
			int shift;
			UObject* Value;
		}Params = { ParameterName, 0, Texture };

		this->ProcessEvent(this, Function, &Params);
	}
};
struct firing_state : public UObject {
public:
	float cooldown() {
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"ShooterGame.FiringStateComponent.GetCooldownTimeRemaining");

		if (!function || this == nullptr) {
			return false;
		}

		struct
		{
			float ret;
		} params;


		this->ProcessEvent_(function, &params);
		return params.ret;
	}
};

struct UBlindManagerComponent : UObject {
	bool is_blinded() {
		UObject* function = UObject::find_object(L"ShooterGame.BlindManagerComponent.IsBlinded");

		if (function == nullptr) {
			// Debug output if the function is not found
			return false;
		}

		struct {
			bool return_value;
		} params;

		this->ProcessEvent_(function, &params);

		return params.return_value;
	}

	void set_blinded(bool value) {
		UObject* function = UObject::find_object(L"ShooterGame.BlindManagerComponent.SetBlinded");

		if (function == nullptr) {
			// Debug output if the function is not found
			return;
		}

		struct {
			bool blinded_value;
		} params = { value };

		this->ProcessEvent_(function, &params);
	}
	UBlindManagerComponent* client_cleanse_blinds() {

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"ShooterGame.BlindManagerComponent.ClientCleanseBlinds");

		struct
		{
			UBlindManagerComponent* output;
		} params;

		this->ProcessEvent_(function, &params);

		return params.output;
	}
};
void use_blind_manager_component(UObject* target_object) {

	//memory::read<blind_manager_component*>(target_object + 0x7e8);

	auto* blind_manager = static_cast<UBlindManagerComponent*>(target_object);

	if (blind_manager == nullptr) {
		return;
	}
	bool blinded = blind_manager->is_blinded();

	if (blinded)
	{
		blind_manager->set_blinded(false);

		blind_manager->client_cleanse_blinds();
	}

}
struct UPrimitiveComponent : UObject {
	void SetCustomDepthStencilValue_Manual(int32 Value)
	{

		static UObject* Function = nullptr;
		if (!Function)
		{
			auto function_name = ez(L"Engine.PrimitiveComponent.SetCustomDepthStencilValue").d();
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);
		}

		if (!Function)
			return;

		struct
		{
			int32 StencilValue;
		} Parameters;

		Parameters.StencilValue = Value;

		Function->ProcessEvent_(Function, &Parameters);
	}
	void SetRenderCustomDepth_Manual(bool bValue)
	{


		static UObject* Function = nullptr;
		if (!Function)
		{
			auto function_name = ez(L"Engine.PrimitiveComponent.SetRenderCustomDepth").d();
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);
		}

		if (!Function)
			return;

		struct
		{
			bool bValue;
		} Parameters;

		Parameters.bValue = bValue;

		Function->ProcessEvent_(Function, &Parameters);
	}

	UObject* create_and_set_material_instance_dynamic_from_material(int32_t element_index, UObject* parent) {
		if (!this || !parent) return nullptr;

		static UObject* fn = nullptr;
		if (!fn)
			fn = UObject::find_object2<UObject*>(L"Engine.PrimitiveComponent.CreateAndSetMaterialInstanceDynamicFromMaterial");

		struct {
			int32_t element_index;
			UObject* parent;
			UObject* return_value;
		} params = { element_index, parent };

		this->ProcessEvent_(fn, &params);
		return params.return_value;
	}
	void SetSkeletalMesh(UObject* NewMesh)
	{
		UObject* function = UObject::find_object2<UObject*>(ez(L"Engine.SkeletalMeshComponent.SetSkeletalMesh").d());
		if (!function || !NewMesh) return;

		struct {
			UObject* NewMesh;
			bool bReinitPose;
		} Parameters;

		Parameters.NewMesh = NewMesh;
		Parameters.bReinitPose = true;

		this->ProcessEvent_(function, &Parameters);
	}
	void SetMaterial(int32_t ElementIndex, UObject* Material)
	{

		static UObject* Function = 0;
		if (!Function) Function = this->StaticFindObject(ez(L"Engine.PrimitiveComponent.SetMaterial").d());

		struct {
			int32_t ElementIndex;
			UObject* Material;
		} Params = { ElementIndex, Material };

		this->ProcessEvent(this, Function, &Params);
	}

	UMaterialInstanceDynamic* GetMaterial(int32_t ElementIndex)
	{
		static UObject* Function = 0;
		if (!Function) Function = this->StaticFindObject(ez(L"Engine.PrimitiveComponent.GetMaterial").d());

		struct {
			int32_t ElementIndex;
			UMaterialInstanceDynamic* ReturnValue;
		} Params = { ElementIndex };

		this->ProcessEvent(this, Function, &Params);

		return Params.ReturnValue;
	}

	int32_t GetNumMaterials()
	{

		static UObject* Function = 0;
		if (!Function) Function = this->StaticFindObject(ez(L"Engine.PrimitiveComponent.GetNumMaterials").d());

		struct {
			int32_t ReturnValue;
		} Params = { };

		this->ProcessEvent(this, Function, &Params);

		return Params.ReturnValue;
	}

};


bool starts_with(const wchar_t* source, const wchar_t* prefix, const enum_as_byte<search_case>& search_case = ignore_case) {

	UObject* niggersmeow = UObject::find_object2<UObject*>(ez(L"Engine.Default__KismetStringLibrary").d());
	static UObject* function;
	if (!function)
		function = UObject::find_object2<UObject*>(ez(L"Engine.KismetStringLibrary.StartsWith").d());

	struct {
		FString source;
		FString prefix;
		std::uint8_t search_case;
		bool return_value;
	} params = { source, prefix, search_case.get() };

	niggersmeow->ProcessEvent_(function, &params);

	return params.return_value;
}
class stability_component : public UObject {
public:
};
struct AAresEquippable : UObject
{
	struct type {

		const wchar_t* search = nullptr;
		const char* config = nullptr;
		std::int32_t index = 0;

		const bool is_valid() const noexcept {
			return this->search != nullptr || this->config != nullptr;
		}
	};
	static inline AAresEquippable::type types[] = {
		{ L"default__basepistol", "classic", 0 },
		{ L"default__sawedoffshotgun", "shorty", 0 },
		{ L"default__autopistol", "frenzy", 0 },
		{ L"default__luger", "ghost", 0 },
		{ L"default__revolver", "sheriff", 0 },

		{ L"default__vector", "stinger", 0 },
		{ L"default__mp5", "spectre", 0 }, // 6

		{ L"default__pumpshotgun", "bucky", 0 },
		{ L"default__automaticshotgun", "judge", 0 },

		{ L"default__burst", "bulldog", 0 },
		{ L"default__dmr", "guardian", 0 },
		{ L"default__assaultrifle_acr", "phantom", 0 }, // 11
		{ L"default__ak", "vandal", 0 },

		{ L"default__leversniperrifle", "marshal", 0 },
		{ L"default__boltsniper", "operator", 0 },

		{ L"default__lmg", "ares", 0 },
		{ L"default__heavymachinegun", "odin", 0 }, // 16

		{ L"default__melee", "knife", 0 },

		{ L"default__carbine", "phantom", 11 },
		{ L"default__automaticpistol", "frenzy", 2 },
		{ L"default__subMachinegun_mp5", "spectre", 6 },
		{ L"Default__Slim", "shorty", 1 },
		{ L"Default__hmg", "odin", 16 },
		{ L"Default__lightmachinegun", "ares", 15 },
		{ L"Default__assaultrifle_ak", "vandal", 12 },
		{ L"Default__AssaultRifle_Burst", "bulldog", 9 },
		{ L"Default__AutoShotgun", "judge", 8 },
		{ L"Default__LeverSniper", "marshal", 13 },
	};
	FVector GetMuzzleLocation(uint64_t weaponAddress)
	{
		return Memory::R<FVector>(weaponAddress + 0x1350);
	}

	AAresEquippable::type GetType() {
		const FString& name = kismentsystemlibrary::get_object_name(this);

		for (std::int32_t index = 0; index < sizeof(AAresEquippable::types) / sizeof(AAresEquippable::type); index++) {
			AAresEquippable::type type = AAresEquippable::types[index];

			if (type.index == 0)
				type.index = index;

			if (kismentsystemlibrary::contains(name.c_str(), type.search))
				return { type.search, type.config, type.index };
		}

		return {};
	}

	TArray<int> get_skin_levels() {
		return Memory::R<TArray<int>>(std::uintptr_t(this) + 0x88);
	}
	struct USkeletalMeshComponent* GetEquippableMesh()
	{

		auto function_name = ez(L"ShooterGame.AresEquippable.GetEquippableMesh").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			struct USkeletalMeshComponent* Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}
	USkeletalMeshComponent* GetMesh3P()
	{

		UObject* function = UObject::find_object2<UObject*>(ez(L"ShooterGame.AresEquippable.GetMesh3P").d());

		struct
		{
			USkeletalMeshComponent* Out;
		}Parameters;
		function->ProcessEvent(this, function, &Parameters);
		return Parameters.Out;
	}

	bool IsZoomed()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.AresEquippable.IsZoomed"));

		struct {
			bool ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

		

	stability_component* get_stability_component2()
	{

		auto sceneviscomp_temp = reinterpret_cast<__int64(*)(AAresEquippable*)>(VALORANT::Module + 0x3BAF190);
		if (!sceneviscomp_temp) return nullptr;

		uintptr_t sceneviscomp = reinterpret_cast<__int64(*)(AAresEquippable*, uint64_t, void*)>(spoofcall_stub)(this, Offsets::MagicOffsets, (void*)sceneviscomp_temp);
		if (!sceneviscomp) return nullptr;

		return Memory::R<stability_component*>(sceneviscomp + 0x4A0);
	}
	firing_state* get_firing_state()
	{
		return Memory::R<firing_state*>(uintptr_t(this) + Offsets::firing_state_component);
	}
	USkeletalMeshComponent* GetMesh1P()
	{

		UObject* function = UObject::find_object2<UObject*>(ez(L"ShooterGame.AresEquippable.GetMesh1P").d());

		struct
		{
			USkeletalMeshComponent* Out;
		}Parameters;
		function->ProcessEvent(this, function, &Parameters);
		return Parameters.Out;
	}
	struct skin_data_asset* GetSkinDataAsset()
	{

		auto function_name = ez(L"ShooterGame.AresEquippable.GetEquippableSkinDataAsset").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			struct skin_data_asset* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}


};

struct UAresInventory : UObject
{
	AAresEquippable* GetCurrentWeapon()
	{

		auto function_name = ez(L"ShooterGame.AresInventory.GetCurrentEquippable").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			AAresEquippable* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
};

class UDamageResponse : public UObject {

public:
	struct APawn* GetDamageCauser() {
		UObject* function = (UObject*)UObject::find_object<UObject*>(L"ShooterGame.DamageResponse.GetEventInstigatorPawn");


		struct
		{
			APawn* output;
		} params;



		this->ProcessEvent_(function, &params);


		return params.output;
	}
	struct APawn* GetEquippableUsed() {
		UObject* function = (UObject*)UObject::find_object<UObject*>(L"ShooterGame.DamageResponse.GetEquippableUsed");


		struct
		{
			APawn* output;
		} params;



		this->ProcessEvent_(function, &params);


		return params.output;
	}
	bool DamageWasBlocked()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.DamageWasBlocked"));

		struct {
			bool ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
	UObject* GetDamagedActor()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.GetDamagedActor"));

		struct {
			UObject* ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	void GetDamagedComponentsCopy(TArray<UObject*>& Components)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.GetDamagedComponentsCopy"));

		struct {
			TArray<UObject*> Components;
		} params;

		this->ProcessEvent_(function, &params);
		Components = params.Components;
	}

	float GetDamageDealt()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.GetDamageDealt"));

		struct {
			float ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	FVector GetDamageOrigin()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.GetDamageOrigin"));

		struct {
			FVector ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	UObject* GetDamagerPlayerState()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.GetDamagerPlayerState"));

		struct {
			UObject* ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	float GetDamageTaken()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.GetDamageTaken"));

		struct {
			float ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	UObject* GetDamageTypeClass()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DamageResponse.GetDamageTypeClass"));

		struct {
			UObject* ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
};
struct AShooterCamera : UObject
{

};
struct AShooterCharacter : UObject
{
	bool dormant;
	UObject* get_character_icon()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetCharacterIcon");
		UObject* Function = UObject::StaticFindObject(function_name.d());
		if (!Function) { return nullptr; }
		struct
		{
			UObject* Out;
		} Args;
		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}
	class UObject* GetDeathReactionComponent()
	{

		UObject* Function = UObject::find_object((L"ShooterGame.ShooterCharacter.GetDeathReactionComponent"));
		if (!Function)
			return nullptr;

		struct
		{
			class UObject* ReturnValue;

		}params;

		Function->ProcessEvent(this, Function, &params);
		return params.ReturnValue;
	}
	void k2_drawtexture(UObject* texture, FVector2D screen_position, FVector2D screen_size, FVector2D coordinate_position, FVector2D coordinate_size, FLinearColor render_color, e_blend_mode blend_mode, float rotation, FVector2D pivot_point)
	{

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"Engine.Canvas.K2_DrawTexture").d());

		struct {
			UObject* texture;
			FVector2D screen_position;
			FVector2D screen_size;
			FVector2D coordinate_position;
			FVector2D coordinate_size;
			FLinearColor render_color;
			e_blend_mode blend_mode;
			float rotation;
			FVector2D pivot_point;
		} params;
		params.texture = texture;
		params.screen_position = screen_position;
		params.screen_size = screen_size;
		params.coordinate_position = coordinate_position;
		params.coordinate_size = coordinate_size;
		params.render_color = render_color;
		params.blend_mode = blend_mode;
		params.rotation = rotation;
		params.pivot_point = pivot_point;

		this->ProcessEvent_(function, &params);
	}
	void relative_rotation(FVector NewRelativeRotation, bool bSweep, bool bTeleport)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"Engine.SceneComponent.K2_SetRelativeRotation");



		struct
		{
			FVector NewRelativeRotation;
			bool bSweep;
			char* gay;
			bool bTeleport;

		} params;



		params.NewRelativeRotation = NewRelativeRotation;
		params.bSweep = bSweep;
		char buf[500];
		params.gay = buf;
		params.bTeleport = bTeleport;

		this->ProcessEvent_(function, &params);
	}
	FRotator K2_SetActorRotation(FRotator NewRotation, bool bTeleportPhysics)
	{

		auto function_name = ez(L"Engine.Actor.K2_SetActorRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FRotator NewRotation;
			bool bTeleportPhysics;
			FRotator Out;
		} Parameters;

		Parameters.NewRotation = NewRotation;
		Parameters.bTeleportPhysics = bTeleportPhysics;
		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	void K2_SetActorRelativeRotationnigger(struct FRotator NewRelativeRotation, bool bSweep, bool bTeleport)
	{

		static UObject* Function;
		if (!Function)
			Function = UObject::find_object2<UObject*>(ez(L"Engine.Actor.K2_SetActorRelativeRotation").d());

		struct
		{
			struct FRotator NewRelativeRotation;
			bool bSweep;
			bool bTeleport;
		}Parameters;
		Parameters.NewRelativeRotation = NewRelativeRotation;
		Parameters.bSweep = bSweep;
		Parameters.bTeleport = bTeleport;
		this->ProcessEvent_(Function, &Parameters);
	}
	void K2_SetActorRelativeRotationFVector(FVector NewRelativeRotation, bool bSweep, bool bTeleport)
	{

		static UObject* Function;
		if (!Function)
			Function = UObject::find_object2<UObject*>(ez(L"Engine.Actor.K2_SetActorRelativeRotation").d());

		struct
		{
			FVector NewRelativeRotation;
			bool bSweep;
			bool bTeleport;
		}Parameters;
		Parameters.NewRelativeRotation = NewRelativeRotation;
		Parameters.bSweep = bSweep;
		Parameters.bTeleport = bTeleport;
		this->ProcessEvent_(Function, &Parameters);
	}
	bool IsDormant()
	{

		return this->dormant;
	}

	bool IsDeadAndSettled()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.IsDeadAndSettled").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return false;

		struct {
			bool Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}

	bool WasRecentlyRendered(float Tolerance)
	{

		auto function_name = ez(L"Engine.Actor.WasRecentlyRendered").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return false;

		struct {
			float Tolerance;
			bool Out;
		} Args;

		Args.Tolerance = Tolerance;
		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}

	UAresInventory* GetInventory()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetInventory").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			UAresInventory* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	FDisplayName GetTrueDisplayName()
	{
		auto function_name = ez(L"ShooterGame.PlatformPlayer.GetTrueDisplayName").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return FDisplayName(); // Return default if not found

		struct {
			FDisplayName ReturnValue;
		} Parameters;

		this->ProcessEvent(Function, &Parameters);
		return Parameters.ReturnValue;
	}

	UObject* FindClass()
	{

		auto class_name = ez(L"ShooterGame.Default__ContentLibrary").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	bool CanJump()
	{

		static UObject* function;
		if (!function)
			function = this->StaticFindObject(0, 0, L"Character.CanJump", false);

		struct { bool ret; } params;

		this->ProcessEvent_(function, &params);
		return params.ret;
	}
	void SetCrouchTimeOverride(float Override)
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.SetCrouchTimeOverride").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			float Override;
		} Parameters;

		Parameters.Override = Override;
		Function->ProcessEvent(this, Function, &Parameters);
	}

	void Set3pMeshVisible(bool value)
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.Set3PMeshVisible").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = this->StaticFindObject(nullptr, nullptr, function_name, false);

		struct { bool a1; } params;
		params.a1 = value;

		this->ProcessEvent_(Function, &params);
	}

	bool ClearWeaponComponents(UObject* equippable)
	{

		auto function_name = ez(L"ShooterGame.ContentLibrary.ClearWeaponComponents").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return false;

		struct {
			UObject* equippable;
			UObject* skindata;
			UObject* chromadata;
			int skinlevel;
			UObject* charmdata;
			int charm_level;
		} Parameters = { equippable };

		UObject::ProcessEvent(FindClass(), Function, &Parameters);
	}

	bool ApplySkin(UObject* equippable, UObject* skindata, UObject* chromadata, int skinlevel, UObject* charmdata, int charm_level)
	{

		auto function_name = ez(L"ShooterGame.ContentLibrary.ApplySkin").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return false;

		struct {
			UObject* equippable;
			UObject* skindata;
			UObject* chromadata;
			int skinlevel;
			UObject* charmdata;
			int charm_level;
		} Parameters = { equippable, skindata, chromadata, skinlevel, charmdata, charm_level };

		UObject::ProcessEvent(FindClass(), Function, &Parameters);
	}

	void SetControlRotation(FVector NewRot)
	{

		auto function_name = ez(L"Engine.Controller.SetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FVector Rot;
		} Parameters;

		Parameters.Rot = NewRot;
		Function->ProcessEvent(this, Function, &Parameters);
	}
	void get_firing_location_and_direction(FVector* Loc, FVector* Rot, bool bApplyRecoil) {
		static UObject* function;
		if (!function)
			function = this->StaticFindObject(0, 0, L"ShooterGame.ShooterCharacter.GetFiringLocationAndDirection", false);
		struct
		{
			FVector location;
			FVector rotation;
			bool bapplyrecoil;
		} params;

		params.bapplyrecoil = bApplyRecoil;

		this->ProcessEvent(this, function, &params);

		*Loc = params.location;
		*Rot = params.rotation;
	}
	void get_firing_location_and_direction2(FVector* Loc, FRotator* Rot, bool bApplyRecoil) {
		static UObject* function;
		if (!function)
			function = this->StaticFindObject(0, 0, L"ShooterGame.ShooterCharacter.GetFiringLocationAndDirection", false);
		struct
		{
			FVector location;
			FRotator rotation;
			bool bapplyrecoil;
		} params;

		params.bapplyrecoil = bApplyRecoil;

		this->ProcessEvent(this, function, &params);

		*Loc = params.location;
		*Rot = params.rotation;
	}
	APlayerState* GetPlayerState()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetPlayerState").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			APlayerState* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	int32_t GetMaxHealth()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetMaxHealth").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return 0;

		struct {
			int32_t Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	float GetMaxShield()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetMaxShield").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return 0;

		struct {
			float Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	float GetHealth()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetHealth").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return 0;

		struct {
			float Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	float GetShield()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetShield").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return 0;

		struct {
			float Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	FVector K2_GetActorLocation()
	{

		auto function_name = ez(L"Engine.Actor.K2_GetActorLocation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FVector Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	FVector GetVelocity()
	{

		auto function_name = ez(L"Engine.Actor.GetVelocity").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FVector Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	FVector GetPawnViewLocation()
	{

		auto function_name = ez(L"Engine.ShooterCharacter.GetPawnViewLocation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FVector Out;
		} Parameters;

		this->ProcessEvent_(Function, &Parameters);
		return Parameters.Out;
	}
	FVector GetActorForwardVector()
	{

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"Engine.Actor.GetActorForwardVector"));

		struct
		{
			FVector ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);

		return params.ReturnValue;
	}
	FVector GetActorUpVector()
	{

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"Engine.Actor.GetActorUpVector"));

		struct
		{
			FVector ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);

		return params.ReturnValue;
	}
	void GetActorEyesViewPoint(FVector* OutLocation, FRotator* OutRotation)

	{

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"Engine.Actor.GetActorEyesViewPoint").d());

		struct
		{
			FVector OutLocation;
			FRotator OutRotation;
		} params;

		this->ProcessEvent_(function, &params);

		if (OutLocation)
			*OutLocation = params.OutLocation;
		if (OutRotation)
			*OutRotation = params.OutRotation;

	}
	USkeletalMeshComponent* GetCosmeticMesh3P()
	{

		UObject* function = UObject::find_object2<UObject*>(ez(L"ShooterGame.ShooterCharacter.GetCosmeticMesh3P").d());

		struct
		{
			USkeletalMeshComponent* Out;
		}Parameters;
		function->ProcessEvent(this, function, &Parameters);
		return Parameters.Out;
	}

	USkeletalMeshComponent* GetPawnMesh()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetPawnMesh").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			USkeletalMeshComponent* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	/*	void get_mesh() {

			static UObject* function;
			if (!function)
				function = UObject::find_object2<UObject*>((L"ShooterGame.ShooterCharacter.GetPawnMesh"));
			static UObject* function = nullptr;
			struct
			{
				USkeletalMeshComponent* output;
			} params;

			this->ProcessEvent_(function, &params);

			return params.output;
		}*/
	void call_set_aspect_ratio(UObject* cameraObject, float NewAspectRatio)
	{


		static UObject* fnSetAspect = nullptr;
		if (!fnSetAspect)
		{
			fnSetAspect = UObject::find_object2<UObject*>(ez(L"Engine.CameraComponent.SetAspectRatio").d());
			if (!fnSetAspect)
				return; // Function not found — don't crash
		}

		if (!cameraObject)
			return;

		// Validate vtable
		uintptr_t vftable = *(uintptr_t*)cameraObject;
		if (vftable < 0x10000 || vftable > 0x7FFFFFFFFFFF)
			return;

		struct {
			float NewAspectRatio;
		} params = { NewAspectRatio };

		cameraObject->ProcessEvent_(fnSetAspect, &params);
	}
	USkeletalMeshComponent* get_mesh() {

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(L"ShooterGame.ShooterCharacter.GetPawnMesh");

		struct
		{
			USkeletalMeshComponent* output;
		} params;

		this->ProcessEvent_(function, &params);

		return params.output;
	}

	USkeletalMeshComponent* GetPawnMesh2()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetPawnMesh").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			USkeletalMeshComponent* Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}
	void ResetCharacterMaterials_Internal(USkeletalMeshComponent* MeshToReset)
	{
		// Find the function once and cache it
		static UObject* Function = nullptr;

		if (!Function)
		{
			// Replace with actual object and function path from the SDK
			auto function_name = ez(L"ShooterGame.ShooterCharacter.ResetCharacterMaterials").d();
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);
		}

		if (!Function)
			return;

		// Construct the parameter struct exactly as expected by the function
		struct
		{
			USkeletalMeshComponent* MeshToReset;
		} Args;

		Args.MeshToReset = MeshToReset;

		// `this` must be a pointer to the object that owns the function (likely a MyShooter or pawn)
		Function->ProcessEvent(this, Function, &Args);
	}

	USkeletalMeshComponent* GetOverlayMesh1P()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetOverlayMesh1P").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			USkeletalMeshComponent* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	USkeletalMeshComponent* GetMesh1P()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetMesh1P").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			USkeletalMeshComponent* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	APlayerState* GetplayerState()
	{
		return Memory::R<APlayerState*>(uintptr_t(this) + Offsets::PlayerState);
	}
	APlayerState* GetState()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetPlayerState").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			APlayerState* KekState;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.KekState;
	}


	APlayerState* K2_GetRootComponent()
	{

		auto function_name = ez(L"ShooterGame.USceneComponent.K2_GetRootComponent").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			APlayerState* RootComponent;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.RootComponent;
	}
	bool IsAlive()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.IsAlive").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return false;

		struct {
			bool Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}


};
enum class EAresItemSlot : unsigned char
{
	Primary = 0,
	Secondary = 1,
	Melee = 2,
	GrenadeAbility = 3,
	Ability1 = 4,
	Ability2 = 5,
	Passive = 6,
	Level = 7,
	Invisible = 8,
	Ultimate = 9,
	Unarmed = 10,
	Armor = 11,
	Backpack = 12,
	Totem = 13,
	PrimaryStorage = 14,
	SecondaryStorage = 15,
	Count = 16,
	Any = 253,
	Invalid = 254,
	EAresItemSlot_MAX = 255,
};
struct USpringArmComponent
{
	char pad[0x130]; // adjust this if needed
	float TargetArmLength;     // likely at offset X
	FVector SocketOffset;      // likely follows after that
};

struct APlayerCameraManager
{

	FVector GetCameraLocation()
	{

		auto function_name = ez(L"Engine.PlayerCameraManager.GetCameraLocation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FVector Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	FRotator GetCameraRotationRotate()
	{

		auto function_name = ez(L"Engine.PlayerCameraManager.GetCameraRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FRotator Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}
	FVector GetCameraRotation()
	{

		auto function_name = ez(L"Engine.PlayerCameraManager.GetCameraRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FVector Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}
};



struct APawn : UObject
{
	EAresItemSlot GetItemSlot() {
		UObject* function = UObject::find_object2<UObject*>(ez(L"ShooterGame.AresItem.GetItemSlot"));

		if (!function || !Memory::IsValidPointer((uintptr_t)function) || !Memory::IsValidPointer((uintptr_t)function))
			return EAresItemSlot::EAresItemSlot_MAX;

		struct
		{
			EAresItemSlot output;
		} params;


		this->ProcessEvent_(function, &params);


		return params.output;
	}
	FVector GetControlRotation()
	{

		auto function_name = ez(L"Engine.Pawn.GetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FVector Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}

	void k2_set_actor_relative_rotation(struct FRotator NewRotation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport)
	{

		auto function_name = ez(L"SceneComponent.K2_SetRelativeRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FRotator a1;
			bool a2;
			int a3;
			bool a4;
		} params;

		params.a1 = NewRotation;
		params.a2 = bSweep;
		params.a4 = bTeleport;

		Function->ProcessEvent_(Function, &params);
	}

	void K2_SetRelativeRotation(FRotator NewRotation, bool bSweep, FHitResult& SweepHitResult, bool bTeleport)
	{
		UObject* function = UObject::StaticFindObject(nullptr, nullptr, ez(L"Engine.SceneComponent.K2_SetRelativeRotation").d(), false);

		struct {
			FRotator NewRotation;
			bool bSweep;
			FHitResult SweepHitResult;
			bool bTeleport;
		}Args;
		Args.NewRotation = NewRotation;
		Args.bSweep = bSweep;
		Args.SweepHitResult = SweepHitResult;
		Args.bTeleport = bTeleport;
		function->ProcessEvent(this, function, &Args);
	}
	bool set_actor_rotation(FRotator new_rotation, bool teleport_physics) {

		static UObject* function;
		if (!function) function = UObject::StaticFindObject(nullptr, nullptr, ez(L"Engine.Actor.K2_SetActorRelativeRotation").d(), false);
		struct {
			FRotator new_rotation;
			bool teleport_physics;
			bool return_value;
		} params = { new_rotation, teleport_physics };


		function->ProcessEvent(this, function, &params);
		return params.return_value;
	}
	USkeletalMeshComponent* GetOverlayMesh1P()
	{

		auto function_name = ez(L"ShooterGame.ShooterCharacter.GetOverlayMesh1P").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			USkeletalMeshComponent* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
};
struct USceneComponent
{
	uintptr_t VTable; // just a placeholder
};


struct UCameraComponent : UObject
{
	void SetViewmodelFOV(float NewFOV)
	{
		auto function_name = ez(L"Engine.CameraComponent.SetFieldOfView").d();
		UObject* function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!function) return;

		struct { float InFieldOfView; } Args;
		Args.InFieldOfView = NewFOV;

		this->ProcessEvent_(function, &Args);  // ✅ Correct call
	}

	void SetViewmodelAspect(float Aspect)
	{
		auto function_name = ez(L"Engine.CameraComponent.SetAspectRatio").d();
		UObject* function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!function) return;

		struct { float InAspectRatio; } Args;
		Args.InAspectRatio = Aspect;

		this->ProcessEvent_(function, &Args);  // ✅ Correct call
	}
};

struct APlayerController : UObject
{
	void ClientForceSetControlRotation(FRotator NewRotation)
	{
		auto function_name = ez(L"ShooterGame.AresPlayerController.ClientForceSetControlRotation").d();
		UObject* function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FRotator NewRotation;
		} Args;
		Args.NewRotation = NewRotation;
		function->ProcessEvent(this, function, &Args);
	}
	FVector2D ProjectWorldToScreen(FVector WorldLocation)
	{

		auto function_name = ez(L"Engine.PlayerController.ProjectWorldLocationToScreen").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return FVector2D();

		struct {
			FVector WorldLocation;
			FVector2D OutLocation;
			bool bRelativeForViewPort;
		} Args;

		Args.WorldLocation = WorldLocation;
		Args.bRelativeForViewPort = true;
		Function->ProcessEvent(this, Function, &Args);
		return Args.OutLocation;
	}

	APlayerCameraManager* GetPlayerCameraManager()
	{

		auto function_name = ez(L"ShooterGame.AresPlayerController.GetPlayerCameraManager").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			APlayerCameraManager* Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}

	AShooterCharacter* K2_GetPawn_in()
	{

		auto function_name = ez(L"Engine.Controller.K2_GetPawn").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			AShooterCharacter* Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}

	APawn* K2_GetPawn()
	{

		auto function_name = ez(L"Engine.Controller.K2_GetPawn").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			APawn* Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}
	APawn* get_acknowledged_pawn() {

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"Engine.Controller.K2_GetPawn").d());

		struct
		{
			APawn* output;
		} params;

		this->ProcessEvent_(function, &params);
		return params.output;
	}
	APawn* K2_GetWeapon()
	{

		auto function_name = ez(L"ShooterGame.AresEquippable.GetMesh1P").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			APawn* Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}

	bool LineOfSightTo(AShooterCharacter* Actor, FVector ViewPoint, bool bAlternateChecks)
	{

		auto function_name = ez(L"Engine.Controller.LineOfSightTo").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return false;

		struct {
			AShooterCharacter* Actor;
			FVector ViewPoint;
			bool bAlternateChecks;
			bool Out;
		} Parameters;

		Parameters.Actor = Actor;
		Parameters.ViewPoint = ViewPoint;
		Parameters.bAlternateChecks = bAlternateChecks;
		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	bool LineOfSightTo2(AShooterCharacter* Enemy)
	{

		auto function_name = (L"Engine.Controller.LineOfSightTo");
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			AShooterCharacter* Enemy;
			FVector ViewPoint;
			bool bAlternateCheck;
			bool ReturnValue;
		} Args;

		Args.Enemy = Enemy;
		Args.ViewPoint = FVector(0, 0, 0);
		Args.bAlternateCheck = false;
		Function->ProcessEvent(this, Function, &Args);
		return Args.ReturnValue;
	}

	void simulate_input_key(fkey_ key, bool bpressed) {

		static UObject* function;
		if (!function)
			function = this->StaticFindObject(0, 0, L"AresPlayerController.SimulateInputKey", false);

		struct { fkey_ a1; bool a2; } params;
		params.a1 = key;
		params.a2 = bpressed;

		this->ProcessEvent_(function, &params);
		return;
	}
	void ServerSay(FString Msg, int32 ChatMode) {

		static UObject* function;
		if (!function)
			function = this->StaticFindObject(0, 0, L"AresPlayerController.ServerSay", false);

		struct { FString a1; int32 a2; } params;
		params.a1 = Msg;
		params.a2 = ChatMode;

		this->ProcessEvent_(function, &params);
		return;
	}
	float GetMouseSensitivity()
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, ez(L"ShooterGame.ShooterPlayerController.GetMouseSensitivity").d(), false);

		if (!Function)
		{
			return 0;
		}
		struct
		{
			float Out;
		}Parameters;
		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	void GetInputMouseDelta(float& DeltaX, float& DeltaY)
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, ez(L"Engine.PlayerController.GetInputMouseDelta").d(), false);

		if (!Function)
		{
			return;
		}
		struct
		{
			float DeltaX;
			float DeltaY;
		}Parameters;
		Function->ProcessEvent(this, Function, &Parameters);
		DeltaX = Parameters.DeltaX;
		DeltaY = Parameters.DeltaY;
	}
	void ClientDrawDebugLines(const TArray<FAresDebugLineReplicated>& Lines)
	{
		static UObject* fn = UObject::StaticFindObject(nullptr, nullptr, ez(L"Engine.PlayerController.ClientDrawDebugLines").d(), false);
		if (!fn) return;

		struct
		{
			TArray<FAresDebugLineReplicated> DebugLines;
		} Params;

		Params.DebugLines = Lines;

		this->ProcessEvent_(fn, &Params);
	}

	FVector GetControlRotation()
	{

		auto function_name = ez(L"Engine.Controller.GetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FVector Rot;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Rot;
	}
	FRotator GetControlRotationFRotator()
	{

		auto function_name = ez(L"Engine.Controller.GetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return { 0, 0, 0 };

		struct {
			FRotator Rot;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Rot;
	}
	void SetControlRotationRage(FRotator NewRot)
	{

		auto function_name = ez(L"Engine.Controller.SetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FRotator Rot;
		} Parameters;

		Parameters.Rot = NewRot;
		Function->ProcessEvent(this, Function, &Parameters);
	}

	void ClientForceSetControlRotation2(FRotator NewRotation)
	{

		auto function_name = ez(L"ShooterGame.AresPlayerController.ClientForceSetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FRotator NewRotation;
		} Args;

		Args.NewRotation = NewRotation;
		Function->ProcessEvent(this, Function, &Args);
	}
	void SetControlRotationFrotator(FRotator NewRot)
	{

		auto function_name = ez(L"Engine.Controller.SetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FRotator Rot;
		} Parameters;

		Parameters.Rot = NewRot;
		Function->ProcessEvent(this, Function, &Parameters);
	}
	void SetControlRotation(FVector NewRot)
	{

		auto function_name = ez(L"Engine.Controller.SetControlRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FVector Rot;
		} Parameters;

		Parameters.Rot = NewRot;
		Function->ProcessEvent(this, Function, &Parameters);
	}



	bool ProjectWorldLocationToScreen(struct FVector WorldLocation, struct FVector2D& ScreenLocation, bool bPlayerViewportRelative)
	{

		auto function_name = ez(L"Engine.PlayerController.ProjectWorldLocationToScreen").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FVector WorldLocationn;
			FVector2D ScreenLocationn;
			bool bPlayerViewportRelativee;
			bool return_value;
		} Args;

		Args.WorldLocationn = WorldLocation;
		Args.bPlayerViewportRelativee = bPlayerViewportRelative;

		Function->ProcessEvent(this, Function, &Args);

		ScreenLocation = Args.ScreenLocationn;

		return Args.return_value;
	}

	bool IsInputKeyDown(fkey_ key)
	{

		static UObject* Function;
		if (!Function) Function = this->StaticFindObject(0, 0, ez(L"PlayerController.IsInputKeyDown").d(), false);

		struct
		{
			fkey_ key;
			bool ReturnValue;
		} params = { key };

		this->ProcessEvent_(Function, &params);

		return params.ReturnValue;
	}



	void set_fov(float fov)
	{

		auto function_name = ez(L"ShooterGame.AresPlayerController.SetFOV").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			float fov;
		} Args;

		Args.fov = fov;
		Function->ProcessEvent(this, Function, &Args);
	}
	AShooterCamera* GetShooterCamera()
	{
			
		auto function_name = ez(L"ShooterGame.AresPlayerController.GetShooterCamera").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			AShooterCamera* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}
	AShooterCharacter* GetShooterCharacter()
	{

		auto function_name = ez(L"ShooterGame.AresPlayerController.GetShooterCharacter").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return nullptr;

		struct {
			AShooterCharacter* Out;
		} Parameters;

		Function->ProcessEvent(this, Function, &Parameters);
		return Parameters.Out;
	}

	void DisconnectFromServer()
	{

		auto function_name = ez(L"ShooterGame.AresPlayerController.DisconnectFromServer").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return;

		Function->ProcessEvent(this, Function, nullptr);
	}
};
enum class ECollisionChannel : uint8 {
	ECC_WorldStatic = 0,
	ECC_WorldDynamic = 1,
	ECC_Pawn = 2,
	ECC_Visibility = 3,
	ECC_Camera = 4,
	ECC_PhysicsBody = 5,
	ECC_Vehicle = 6,
	ECC_Destructible = 7,
	ECC_EngineTraceChannel1 = 8,
	ECC_EngineTraceChannel2 = 9,
	ECC_EngineTraceChannel3 = 10,
	ECC_EngineTraceChannel4 = 11,
	ECC_EngineTraceChannel5 = 12,
	ECC_EngineTraceChannel6 = 13,
	ECC_GameTraceChannel1 = 14,
	ECC_GameTraceChannel2 = 15,
	ECC_GameTraceChannel3 = 16,
	ECC_GameTraceChannel4 = 17,
	ECC_GameTraceChannel5 = 18,
	ECC_GameTraceChannel6 = 19,
	ECC_GameTraceChannel7 = 20,
	ECC_GameTraceChannel8 = 21,
	ECC_GameTraceChannel9 = 22,
	ECC_GameTraceChannel10 = 23,
	ECC_GameTraceChannel11 = 24,
	ECC_GameTraceChannel12 = 25,
	ECC_GameTraceChannel13 = 26,
	ECC_GameTraceChannel14 = 27,
	ECC_GameTraceChannel15 = 28,
	ECC_GameTraceChannel16 = 29,
	ECC_GameTraceChannel17 = 30,
	ECC_GameTraceChannel18 = 31,
	ECC_OverlapAll_Deprecated = 32,
	ECC_MAX = 33
};

namespace ShooterGameBlueprints
{
	UObject* Static_Class()
	{

		auto class_name = ez(L"ShooterGame.Default__ShooterBlueprintLibrary").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	void DrawDebugLineOnAllClients(
		UObject* WorldContextObject,
		const FVector& LineStart,
		const FVector& LineEnd,
		const FColor& LineColor,
		bool bPersistentLines = false,
		float LifeTime = 1.0f,
		uint8 DepthPriority = 0,
		float Thickness = 1.0f
	)
	{
		static UObject* Function = nullptr;
		if (!Function)
			Function = UObject::find_object2<UObject*>(L"ShooterGame.ShooterBlueprintLibrary.DrawDebugLineOnAllClients");

		struct
		{
			UObject* WorldContextObject;
			FVector LineStart;
			FVector LineEnd;
			FColor LineColor;
			bool bPersistentLines;
			float LifeTime;
			uint8 DepthPriority;
			float Thickness;
		} Params;

		Params.WorldContextObject = WorldContextObject;
		Params.LineStart = LineStart;
		Params.LineEnd = LineEnd;
		Params.LineColor = LineColor;
		Params.bPersistentLines = bPersistentLines;
		Params.LifeTime = LifeTime;
		Params.DepthPriority = DepthPriority;
		Params.Thickness = Thickness;

		Function->ProcessEvent(Static_Class(), Function, &Params);
	}

	static inline FString GetValVersion() {
		auto function = UObject::find_object(L"ShooterGame.ShooterBlueprintLibrary.GetFullBuildVersionString");

		if (function == nullptr)
			return nullptr;

		FString return_value;

		function->ProcessEvent(Static_Class(), function, &return_value);

		return return_value;
	}
	bool ares_line_trace_single_by_channel(
		UWorld* world_context,
		const FVector& start,
		const FVector& end,
		const TArray<UObject*>& actors_to_ignore,
		FHitResult* out_hit,
		ECollisionChannel trace_channel,
		bool trace_complex,
		bool include_moving_objects
	)
	{
		static UObject* function = nullptr;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.ShooterBlueprintLibrary.AresLineTraceSingleByChannel"));

		struct
		{
			UWorld* world_context_object;
			FVector start;
			FVector end;
			TArray<UObject*> actors_to_ignore;
			FHitResult out_hit;
			ECollisionChannel trace_channel;
			bool trace_complex;
			bool include_moving_objects;
			bool return_value;
		} params{};

		params.world_context_object = world_context;
		params.start = start;
		params.end = end;
		params.actors_to_ignore = actors_to_ignore;
		params.trace_channel = trace_channel;
		params.trace_complex = trace_complex;
		params.include_moving_objects = include_moving_objects;

		// Debug logging
		//printf("[Trace] Start: [%.1f, %.1f, %.1f] End: [%.1f, %.1f, %.1f]\n",
			//start.X, start.Y, start.Z, end.X, end.Y, end.Z);

		//printf("[Trace] Ignoring %d actors | Complex: %d | MovingObjs: %d\n",
			//actors_to_ignore.Num(), trace_complex, include_moving_objects);

		// Call safely
		if (function)
			classes::defines::blueprint->ProcessEvent_(function, &params);

		// Copy result
		if (out_hit)
			*out_hit = params.out_hit;

		return params.return_value;
	}


	APlayerController* GetFirstLocalPlayerController(UWorld* WorldContextObject)
	{

		auto function_name = ez(L"ShooterGame.ShooterBlueprintLibrary.GetFirstLocalPlayerController").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
		{
			return nullptr;
		}

		struct
		{
			UWorld* WorldContextObject;
			APlayerController* Out;
		} Parameters;

		Parameters.WorldContextObject = WorldContextObject;
		Function->ProcessEvent(Static_Class(), Function, &Parameters);
		return Parameters.Out;
	}
	static void GetWallPenetrationSpans(UWorld* WorldContextObject, FVector StartLocation, FVector EndLocation, TArray<UObject*> IgnoreActors, ECollisionChannel Channel, float MinimumPlayableSpan, FWallSpanList& Spans)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.ShooterBlueprintLibrary.GetWallPenetrationSpans"));

		struct {
			UWorld* WorldContextObject;
			FVector StartLocation;
			FVector EndLocation;
			TArray<UObject*> IgnoreActors;
			ECollisionChannel Channel;
			float MinimumPlayableSpan;
			FWallSpanList Spans;
		} params;

		// Assign input
		params.WorldContextObject = WorldContextObject;
		params.StartLocation = StartLocation;
		params.EndLocation = EndLocation;
		params.IgnoreActors = IgnoreActors;
		params.Channel = Channel;
		params.MinimumPlayableSpan = MinimumPlayableSpan;
		//if (!WorldContextObject)
		//{
		//	printf("[ERROR] WorldContextObject is NULL!\n");
		//}
		//else
		//{
		//	printf("[DEBUG] World is valid: %p\n", WorldContextObject);
		//}
		// 🔍 Debug input
		//printf("[WallPenetration] Start: X: %.2f Y: %.2f Z: %.2f\n", StartLocation.X, StartLocation.Y, StartLocation.Z);
		//printf("[WallPenetration] End:   X: %.2f Y: %.2f Z: %.2f\n", EndLocation.X, EndLocation.Y, EndLocation.Z);
		//printf("[WallPenetration] IgnoredActors count: %d\n", IgnoreActors.Num());
		//printf("[WallPenetration] Channel: %d | MinPlayableSpan: %.2f\n", Channel, MinimumPlayableSpan);

		// Call the function
		classes::defines::blueprint->ProcessEvent_(function, &params);

		// Return result
		Spans = params.Spans;

		// 🔍 Debug output
		/*int wallCount = Spans.Spans.size();
		printf("[WallPenetration] Spans returned: %d\n", wallCount);*/

		/*for (int i = 0; i < wallCount; i++) {
			const auto& Span = Spans.Spans[i];
			const FVector& entry = Span.Entrance.Location;
			const FVector& exit = Span.Exit.Location;
			printf("  [Span %d] Entry: X: %.2f Y: %.2f Z: %.2f | Exit: X: %.2f Y: %.2f Z: %.2f\n",
				i, entry.X, entry.Y, entry.Z, exit.X, exit.Y, exit.Z);
		}*/
	}



	static UObject* GetDamageSourceFromDamageResponse(UObject* DamageResponse)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.ShooterBlueprintLibrary.GetDamageSourceFromDamageResponse"));

		struct {
			UObject* DamageResponse;
			UObject* ReturnValue;
		} params;
		params.DamageResponse = DamageResponse;

		classes::defines::blueprint->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
	TArray<AShooterCharacter*> FindAllShooterCharactersWithAlliance(UWorld* WorldContextObject, AShooterCharacter* LocalViewer, EAresAlliance Alliance, bool OnlyPlayerControlled, bool OnlyAlivePlayers)
	{

		auto function_name = ez(L"ShooterGame.ShooterBlueprintLibrary.FindAllShooterCharactersWithAlliance").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			UWorld* WorldContextObject;
			AShooterCharacter* LocalViewer;
			EAresAlliance Alliance;
			bool OnlyPlayerControlled;
			bool OnlyAlivePlayers;
			TArray<AShooterCharacter*> Out;
		} Parameters;

		Parameters.WorldContextObject = WorldContextObject;
		Parameters.LocalViewer = LocalViewer;
		Parameters.Alliance = Alliance;
		Parameters.OnlyPlayerControlled = OnlyPlayerControlled;
		Parameters.OnlyAlivePlayers = OnlyAlivePlayers;

		Function->ProcessEvent(Static_Class(), Function, &Parameters);
		return Parameters.Out;
	}
}

namespace AresOutlineRendering
{
	UObject* Static_Class()
	{

		auto class_name = ez(L"Renderer.Default__AresOutlineRendering").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	void SetOutlineColorsForRender(UWorld* WorldContextObject, FLinearColor AllyColor, FLinearColor EnemyColor)
	{

		auto function_name = ez(L"Renderer.AresOutlineRendering.SetOutlineColorsForRender").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
		{
			return;
		}

		struct
		{
			UWorld* WorldContextObject;
			FLinearColor AllyColor;
			FLinearColor EnemyColor;
		} Parameters;

		Parameters.WorldContextObject = WorldContextObject;
		Parameters.AllyColor = AllyColor;
		Parameters.EnemyColor = EnemyColor;

		Function->ProcessEvent(Static_Class(), Function, &Parameters);
	}
}

namespace BaseTeamComponent
{
	UObject* Static_Class()
	{

		auto class_name = ez(L"ShooterGame.Default__BaseTeamComponent").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	bool IsAlly(AShooterCharacter* Enemy, AShooterCharacter* Self)
	{

		auto function_name = ez(L"ShooterGame.BaseTeamComponent.IsAlly").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct
		{
			AShooterCharacter* Enemy;
			AShooterCharacter* Self;
			bool ReturnValue;
		} Parameters;

		Parameters.Enemy = Enemy;
		Parameters.Self = Self;

		Function->ProcessEvent(Static_Class(), Function, &Parameters);
		return Parameters.ReturnValue;
	}
}

class UCustomWallPenetrationResponseComponent : public UObject {
public:
	bool ProjectileShouldIgnoreWallPenetration(UObject* Projectile, FHitResult HitResult, bool bExit)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.CustomWallPenetrationResponseComponent.ProjectileShouldIgnoreWallPenetration"));

		struct {
			UObject* Projectile;
			FHitResult HitResult;
			bool bExit;
			bool ReturnValue;
		} params;
		params.Projectile = Projectile;
		params.HitResult = HitResult;
		params.bExit = bExit;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
};
class IDirectionalDamageQuery : public UObject {
public:
	FName GetDamagedBone()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DirectionalDamageQuery.GetDamagedBone"));

		struct {
			FName ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	UObject* GetDamagedComponent()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DirectionalDamageQuery.GetDamagedComponent"));

		struct {
			UObject* ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	FVector GetDamageDirection()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DirectionalDamageQuery.GetDamageDirection"));

		struct {
			FVector ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	FVector GetDamageImpactLocation()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DirectionalDamageQuery.GetDamageImpactLocation"));

		struct {
			FVector ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	FVector GetDamageImpactNormal()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.DirectionalDamageQuery.GetDamageImpactNormal"));

		struct {
			FVector ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
};

class UWallPenetrationTargetingStateComponent : public UObject {
public:
	FHitResult GetWallExitResult()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallPenetrationTargetingStateComponent.GetWallExitResult"));

		struct {
			FHitResult ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
};
class IWallPenetrationDamageQuery : public UObject {
public:
	bool IsWallPenetration()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallPenetrationDamageQuery.IsWallPenetration"));

		struct {
			bool ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
};
class UWallPenetrationComponent : public UObject {
public:
	float StoppingDistanceMultiplier;
	float PenetrationPowerMultiplier;
	bool bApplyPostPenetrationAirDropoff;
	uint8_t Pad_101[0x3];
	float PostPenetrationAirDropoffDistance;
	uint8_t Pad_158[0x2];
	bool bImpactEffectsEnabled;
	uint8_t Pad_15B[0x25];

public:
	void OnMaximumRangeReached()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallPenetrationComponent.OnMaximumRangeReached"));

		this->ProcessEvent_(function, nullptr);
	}

	void OnStop()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallPenetrationComponent.OnStop"));

		this->ProcessEvent_(function, nullptr);
	}

	void SetPostPenetrationAirDropoff(bool bEnablePostPenetrationAirDropoff)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallPenetrationComponent.SetPostPenetrationAirDropoff"));

		struct {
			bool bEnablePostPenetrationAirDropoff;
		} params;
		params.bEnablePostPenetrationAirDropoff = bEnablePostPenetrationAirDropoff;

		this->ProcessEvent_(function, &params);
	}

	bool GetPostPenetrationAirDropoff()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallPenetrationComponent.GetPostPenetrationAirDropoff"));

		struct {
			bool ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
};
class UWallMesh : public UObject {
public:
	float WallHeight;
	float MaxWallHeightOffset;
	float MaxAnchorSeparation;
	float WallEndPadding;
	float WallBottomOffset;
	float ClientTargetEdgeLength;
	int32_t ClientNumSubdivisions;
	bool bClientGenerateCollision;
	bool bServerGenerateCollision;
	uint8_t Pad_62E[0x2];
	float MaskedSideOffset;
	float MaskedTopOffset;
	bool bUseSimpleBoxCollision;
	bool bBoxesStartRaised;
	uint8_t Pad_63A[0x2];
	float BoxEdgeMaskCoveragePercent;
	float BoxTopAdjust;
	uint8_t Pad_644[0xA4];
	uint8_t Pad_6F8[0x8];

public:
	void AddWallAnchor(FVector AnchorLocation, FVector AnchorTangent)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.AddWallAnchor"));

		struct {
			FVector AnchorLocation;
			FVector AnchorTangent;
		} params;
		params.AnchorLocation = AnchorLocation;
		params.AnchorTangent = AnchorTangent;

		this->ProcessEvent_(function, &params);
	}

	void BeginBuildingMesh()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.BeginBuildingMesh"));

		this->ProcessEvent_(function, nullptr);
	}

	FVector2D DistanceToWall(FVector Position, float OpenStartTime, float OpenDistancePerSecond, float PointDuration, const FVector2D& MaxThresholds, float AdditionalHalfHeight)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.DistanceToWall"));

		struct {
			FVector Position;
			float OpenStartTime;
			float OpenDistancePerSecond;
			float PointDuration;
			FVector2D MaxThresholds;
			float AdditionalHalfHeight;
			FVector2D ReturnValue;
		} params;
		params.Position = Position;
		params.OpenStartTime = OpenStartTime;
		params.OpenDistancePerSecond = OpenDistancePerSecond;
		params.PointDuration = PointDuration;
		params.MaxThresholds = MaxThresholds;
		params.AdditionalHalfHeight = AdditionalHalfHeight;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	float GetWallEndDistance()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.GetWallEndDistance"));

		struct {
			float ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	float GetWallStartDistance()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.GetWallStartDistance"));

		struct {
			float ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	void SetSimpleCollisionRaiseUniform(float RaisedAmount)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.SetSimpleCollisionRaiseUniform"));

		struct {
			float RaisedAmount;
		} params;
		params.RaisedAmount = RaisedAmount;

		this->ProcessEvent_(function, &params);
	}

	void SetSimpleCollisionRaiseValues(float RaiseStartScale, float RaiseStartDist, float RaiseEndScale, float RaiseEndDist)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.SetSimpleCollisionRaiseValues"));

		struct {
			float RaiseStartScale;
			float RaiseStartDist;
			float RaiseEndScale;
			float RaiseEndDist;
		} params;
		params.RaiseStartScale = RaiseStartScale;
		params.RaiseStartDist = RaiseStartDist;
		params.RaiseEndScale = RaiseEndScale;
		params.RaiseEndDist = RaiseEndDist;

		this->ProcessEvent_(function, &params);
	}

	void SetSimpleCollisionRaiseWave(float WaveStart, float WaveLength)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.SetSimpleCollisionRaiseWave"));

		struct {
			float WaveStart;
			float WaveLength;
		} params;
		params.WaveStart = WaveStart;
		params.WaveLength = WaveLength;

		this->ProcessEvent_(function, &params);
	}

	FVector WallSectionEndLocation(int32_t Section)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.WallSectionEndLocation"));

		struct {
			int32_t Section;
			FVector ReturnValue;
		} params;
		params.Section = Section;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	FVector WallSectionStartLocation(int32_t Section)
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.WallSectionStartLocation"));

		struct {
			int32_t Section;
			FVector ReturnValue;
		} params;
		params.Section = Section;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}

	bool MeshBuilt()
	{
		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.WallMesh.MeshBuilt"));

		struct {
			bool ReturnValue;
		} params;

		this->ProcessEvent_(function, &params);
		return params.ReturnValue;
	}
};
namespace menu
{
	namespace input
	{
		bool mouseDown[5];
		bool mouseDownAlready[256];

		bool keysDown[256];
		bool keysDownAlready[256];

		bool is_any_mouse_down()
		{
			if (mouseDown[0]) return true;
			if (mouseDown[1]) return true;
			if (mouseDown[2]) return true;
			if (mouseDown[3]) return true;
			if (mouseDown[4]) return true;

			return false;
		}

		bool is_mouse_clicked(int button, int element_id, bool repeat)
		{
			if (mouseDown[button])
			{
				if (!mouseDownAlready[element_id])
				{
					mouseDownAlready[element_id] = true;
					return true;
				}
				if (repeat)
					return true;
			}
			else
			{
				mouseDownAlready[element_id] = false;
			}
			return false;
		}
		bool is_key_pressed(int key, bool repeat)
		{
			if (keysDown[key])
			{
				if (!keysDownAlready[key])
				{
					keysDownAlready[key] = true;
					return true;
				}
				if (repeat)
					return true;
			}
			else
			{
				keysDownAlready[key] = false;
			}
			return false;
		}

		void handle()
		{
			if (GetAsyncKeyState(0x01))
				mouseDown[0] = true;
			else
				mouseDown[0] = false;
		}
	}
}
wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

FLinearColor RGBtoFLC(float R, float G, float B)
{
	return { R / 255, G / 255, B / 255, 1 };
}

namespace menu
{
	UObject* font;

	FLinearColor RGBtoFLC(float R, float G, float B)
	{
		return { R / 255, G / 255, B / 255, 1 };
	}

	FLinearColor HSVtoRGB(float h, float s, float v) {
		float R, G, B;

		int i = static_cast<int>(h * 6);
		float f = h * 6 - i;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1 - (1 - f) * s);

		switch (i % 6) {
		case 0: R = v; G = t; B = p; break;
		case 1: R = q; G = v; B = p; break;
		case 2: R = p; G = v; B = t; break;
		case 3: R = p; G = q; B = v; break;
		case 4: R = t; G = p; B = v; break;
		case 5: R = v; G = p; B = q; break;
		default: R = G = B = 0.0f;
		}

		return FLinearColor{ R, G, B, 1.0f };

	}

	// x9 ware
	/*namespace Colors
	{
		FLinearColor Text{ 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 255.0f / 255.0f }; //
		FLinearColor Text_Shadow{ 0.0f, 0.0f, 0.0f, 1.0f };
		FLinearColor Text_Outline{ 0.0f, 0.0f, 0.0f, 1.0f };
		FLinearColor Text_Active = RGBtoFLC(184, 51, 255);  // Updated purple

		FLinearColor Button_Idle{ 10.0 / 255.0f, 10.0 / 255.0f, 10.0 / 255.0f, 0.8f }; //
		FLinearColor Button_Hovered = RGBtoFLC(7, 7, 7);
		FLinearColor Button_Active = RGBtoFLC(184, 51, 255);  // Updated purple

		FLinearColor Checkbox_Idle = RGBtoFLC(15, 15, 15);
		FLinearColor Checkbox_Hovered = RGBtoFLC(20, 20, 20);
		FLinearColor Checkbox_Enabled = RGBtoFLC(184, 51, 255); // Updated purple

		FLinearColor Combobox_Idle{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		FLinearColor Combobox_Hovered{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		FLinearColor Combobox_Elements{ 0.239f, 0.42f, 0.0f, 0.5f };

		FLinearColor Slider_Idle = RGBtoFLC(8, 8, 8);
		FLinearColor Slider_Hovered = RGBtoFLC(14, 14, 14);
		FLinearColor Slider_Progress = RGBtoFLC(184, 51, 255);  // Updated purple
		FLinearColor Slider_Button = RGBtoFLC(184, 51, 255);  // Updated purple

		FLinearColor ColorPicker_Background{ 0.006f, 0.006f, 0.006f, 0.4f };
	}*/


	namespace colors
	{
		FLinearColor Text{ 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 255.0f / 255.0f }; //
		FLinearColor Text_Shadow{ 0.0f, 0.0f, 0.0f, 0.0f };
		FLinearColor Text_Outline{ 0.0f, 0.0f, 0.0f, 0.0f };
		FLinearColor Text_Active{ 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 0.8f };

		FLinearColor Button_Idle{ 10.0 / 255.0f, 10.0 / 255.0f, 10.0 / 255.0f, 0.8f / 255.0f }; //
		FLinearColor Button_Hovered{ 15.0f / 255.0f, 15.0f / 255.0f, 15.0f / 255.0f, 1.f };
		FLinearColor Button_Active{ 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 0.8f };

		FLinearColor Checkbox_Idle = RGBtoFLC(15, 15, 15);
		FLinearColor Checkbox_Hovered = RGBtoFLC(20, 20, 20);
		FLinearColor Checkbox_Enabled = RGBtoFLC(128, 128, 128); //

		FLinearColor Combobox_Idle{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		FLinearColor Combobox_Hovered{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		FLinearColor Combobox_Elements{ 0.239f, 0.42f, 0.0f, 0.5f };

		FLinearColor Slider_Idle = RGBtoFLC(8, 8, 8);
		FLinearColor Slider_Hovered = RGBtoFLC(14, 14, 14);
		FLinearColor Slider_Progress = RGBtoFLC(128, 128, 128);
		FLinearColor Slider_Button = RGBtoFLC(128, 128, 128);

		FLinearColor ColorPicker_Background{ 0.006f, 0.006f, 0.006f, 0.4f };
	}

	namespace PostRenderer
	{
		struct DrawList
		{
			int type = -1; //1 = FilledRect, 2 = TextLeft, 3 = TextCenter, 4 = Draw_Line
			FVector2D pos;
			FVector2D size;
			FLinearColor color;
			const wchar_t* name;
			bool outline;

			FVector2D from;
			FVector2D to;
			int thickness;
		};
		DrawList drawlist[128];

		void drawFilledRect(FVector2D pos, float w, float h, FLinearColor color)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 1;
					drawlist[i].pos = pos;
					drawlist[i].size = FVector2D{ w, h };
					drawlist[i].color = color;
					return;
				}
			}
		}
		void TextLeft(const wchar_t* name, FVector2D pos, FLinearColor color, bool outline)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 2;
					drawlist[i].name = name;
					drawlist[i].pos = pos;
					drawlist[i].outline = outline;
					drawlist[i].color = color;
					return;
				}
			}
		}
		void TextCenter(const wchar_t* name, FVector2D pos, FLinearColor color, bool outline)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 3;
					drawlist[i].name = name;
					drawlist[i].pos = pos;
					drawlist[i].outline = outline;
					drawlist[i].color = color;
					return;
				}
			}
		}
		void Draw_Line(FVector2D from, FVector2D to, int thickness, FLinearColor color)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 4;
					drawlist[i].from = from;
					drawlist[i].to = to;
					drawlist[i].thickness = thickness;
					drawlist[i].color = color;
					return;
				}
			}
		}
	}

	UCanvas* canvas;


	bool hover_element = false;
	FVector2D menu_pos = FVector2D{ 0, 0 };
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	FVector2D first_element_pos = FVector2D{ 0, 0 };

	FVector2D last_element_pos = FVector2D{ 0, 0 };
	FVector2D last_element_size = FVector2D{ 0, 0 };

	int current_element = -1;
	FVector2D current_element_pos = FVector2D{ 0, 0 };
	FVector2D current_element_size = FVector2D{ 0, 0 };


	bool sameLine = false;

	bool pushY = false;
	float pushYvalue = 0.0f;

	void SetupCanvas(UCanvas* _canvas)
	{
		canvas = _canvas;
	}

	void Draw_Line(FVector2D from, FVector2D to, int thickness, FLinearColor color)
	{
		canvas->K2_DrawLine(FVector2D{ from.X, from.Y }, FVector2D{ to.X, to.Y }, thickness, color);
	}
	void drawFilledRect(FVector2D initial_pos, float w, float h, FLinearColor color)
	{
		for (float i = 0.0f; i < h; i += 1.0f)
			canvas->K2_DrawLine(FVector2D{ initial_pos.X, initial_pos.Y + i }, FVector2D{ initial_pos.X + w, initial_pos.Y + i }, 1.0f, color);
	}
	void drawGradientRect(FVector2D pos, float width, float height, FLinearColor startColor, FLinearColor endColor)
	{
		for (int i = 0; i < static_cast<int>(width); ++i)
		{
			float t = i / width;
			FLinearColor color = {
				startColor.R + (endColor.R - startColor.R) * t,
				startColor.G + (endColor.G - startColor.G) * t,
				startColor.B + (endColor.B - startColor.B) * t,
				startColor.A + (endColor.A - startColor.A) * t
			};

			drawFilledRect(FVector2D{ pos.X + i, pos.Y }, 1, height, color);
		}
	}
	void DrawGradientLine(FVector2D screenpos_a, FVector2D screenpos_b, FLinearColor color_a, FLinearColor color_c, FLinearColor color_b, float thickness, int num_segments)
	{
		for (int i = 0; i < num_segments; i++)
		{
			float t1 = static_cast<float>(i) / num_segments;
			float t2 = static_cast<float>(i + 1) / num_segments;

			FVector2D start = screenpos_a + (screenpos_b - screenpos_a) * t1;
			FVector2D end = screenpos_a + (screenpos_b - screenpos_a) * t2;

			float t_mid = (t1 + t2) / 2.0f;

			FLinearColor color;

			if (t_mid <= 0.5f)
			{
				float u = 2.0f * t_mid;  // Maps t_mid [0, 0.5] to u [0, 1]
				color.R = color_a.R * (1.0f - u) + color_c.R * u;
				color.G = color_a.G * (1.0f - u) + color_c.G * u;
				color.B = color_a.B * (1.0f - u) + color_c.B * u;
				color.A = color_a.A * (1.0f - u) + color_c.A * u;
			}
			else
			{
				float v = 2.0f * (t_mid - 0.5f);  // Maps t_mid [0.5, 1] to v [0, 1]
				color.R = color_c.R * (1.0f - v) + color_b.R * v;
				color.G = color_c.G * (1.0f - v) + color_b.G * v;
				color.B = color_c.B * (1.0f - v) + color_b.B * v;
				color.A = color_c.A * (1.0f - v) + color_b.A * v;
			}

			canvas->K2_DrawLine(start, end, thickness, color);
		}
	}
	void DrawFilledCircle(FVector2D pos, float R, FLinearColor color)
	{
		float smooth = 0.07f;

		double PI = 3.14159265359;
		int size = (int)(2.0f * PI / smooth) + 1;

		float angle = 0.0f;
		int i = 0;

		for (; angle < 2 * PI; angle += smooth, i++)
		{
			Draw_Line(FVector2D{ pos.X, pos.Y }, FVector2D{ pos.X + cosf(angle) * R, pos.Y + sinf(angle) * R }, 1.0f, color);
		}
	}
	// DrawGradientLine(startPos, endPos, red, green, blue, 2.0f, 20);
	void drawGradientFilledRect(FVector2D initial_pos, float w, float h, FLinearColor color_a, FLinearColor color_c, FLinearColor color_b, int num_segments)
	{
		for (float i = 0.0f; i < h; i += 1.0f)
			DrawGradientLine(FVector2D{ initial_pos.X, initial_pos.Y + i }, FVector2D{ initial_pos.X + w, initial_pos.Y + i }, color_a, color_c, color_b, 1.0f, num_segments);
	}

	void drawGradientFilledRectVertical(FVector2D initial_pos, float w, float h, FLinearColor color_a, FLinearColor color_c, FLinearColor color_b, int num_segments)
	{
		for (float i = 0.0f; i < w; i += 1.0f)
			DrawGradientLine(FVector2D{ initial_pos.X + i, initial_pos.Y }, FVector2D{ initial_pos.X + i, initial_pos.Y + h }, color_a, color_c, color_b, 1.0f, num_segments);
	}


	void draw_filled_rect(UCanvas* canvas, float X, float Y, float width, float height, FLinearColor color) {
		for (float i = 0; i < height; i++) {
			canvas->K2_DrawLine(
				{ X, Y + i },
				{ X + width, Y + i },
				1.0f,
				color
			);
		}
	}

	void draw_rect(UCanvas* canvas, float X, float Y, float width, float height, FLinearColor color) {
		canvas->K2_DrawLine({ X, Y }, { X + width, Y }, 1.0f, color);
		canvas->K2_DrawLine({ X + width, Y }, { X + width, Y + height }, 1.0f, color);
		canvas->K2_DrawLine({ X + width, Y + height }, { X, Y + height }, 1.0f, color);
		canvas->K2_DrawLine({ X, Y + height }, { X, Y }, 1.0f, color);
	}

	FVector2D CursorPos()
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		return FVector2D{ (float)cursorPos.x, (float)cursorPos.y };
	}
	bool MouseInZone(FVector2D pos, FVector2D size)
	{
		FVector2D cursor_pos = CursorPos();

		if (cursor_pos.X > pos.X && cursor_pos.Y > pos.Y)
			if (cursor_pos.X < pos.X + size.X && cursor_pos.Y < pos.Y + size.Y)
				return true;

		return false;
	}

	void Draw_Cursor(bool toggle)
	{
		if (toggle)
		{
			FVector2D cursorPos = CursorPos();
			float len = 6.0f;
			float thickness = 1.8f;

			// Horizontal line
			drawFilledRect(FVector2D(cursorPos.X - len, cursorPos.Y - thickness / 2), len * 2, thickness, FLinearColor(255, 255, 255, 230));
			// Vertical line
			drawFilledRect(FVector2D(cursorPos.X - thickness / 2, cursorPos.Y - len), thickness, len * 2, FLinearColor(255, 255, 255, 230));
			// Center dot (optional)

		}
	}




	void SameLine()
	{
		sameLine = true;
	}
	void PushNextElementY(float Y, bool from_last_element = true)
	{
		pushY = true;
		if (from_last_element)
			pushYvalue = last_element_pos.Y + last_element_size.Y + Y;
		else
			pushYvalue = Y;
	}
	void NextColumn(float X)
	{
		offset_x = X;
		PushNextElementY(first_element_pos.Y, false);
	}
	void ClearFirstPos()
	{
		first_element_pos = FVector2D{ 0, 0 };
	}

	void xDrawTextRGB(const wchar_t* text, float X, float Y, FLinearColor color)
	{
		canvas->K2_DrawText(text, { X, Y }, { 1.1, 1.1 }, { 1.f,1.f,1.f,1.0f }, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, 0, 0, 0, { 0, 0, 0, 1 });
	}
	void yDrawTextRGB(const wchar_t* text, float X, float Y, FLinearColor color)
	{
		canvas->K2_DrawText(text, { X, Y }, { 1.1, 1.1 }, { 1.f,1.f,1.f,1.0f }, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, 1, 0, 0, { 0, 0, 0, 1 });
	}
	void TextLeft(const wchar_t* name, FVector2D pos, FLinearColor color, bool outline)
	{
		xDrawTextRGB(name, pos.X, pos.Y, { 1,1,1,1 });
	}
	void TextCenter(const wchar_t* name, FVector2D pos, FLinearColor color, bool outline, bool kekw = 0)
	{
		if (kekw)
			yDrawTextRGB(name, pos.X, pos.Y - 10, color);
		else
			yDrawTextRGB(name, pos.X, pos.Y - 10, { 1.f,1.f,1.f,1.0f });
	}

	void GetColor(FLinearColor* color, float* R, float* G, float* B, float* A)
	{
		*R = color->R;
		*G = color->G;
		*B = color->B;
		*A = color->A;
	}
	UINT32 GetColorUINT(int R, int G, int B, int A)
	{
		UINT32 result = (BYTE(A) << 24) + (BYTE(R) << 16) + (BYTE(G) << 8) + BYTE(B);
		return result;
	}

	void DrawCircleGradient(FVector2D center, float radius, int segments, float timeOffset)
	{
		const float brightness = 0.6f; // 🔧 Control darkness here (0.5 = darker, 1.0 = original)

		for (int i = 0; i < segments; ++i)
		{
			float t = (float)i / segments;
			float angle1 = t * 2.0f * PI;
			float angle2 = ((float)(i + 1) / segments) * 2.0f * PI;

			FVector2D p1 = center + FVector2D(std::cos(angle1), std::sin(angle1)) * radius;
			FVector2D p2 = center + FVector2D(std::cos(angle2), std::sin(angle2)) * radius;

			float deg = angle1 * (180.0f / PI);
			if (deg < 0.0f) deg += 360.0f;

			FLinearColor color;

			if (deg < 90.0f)
				color = FMath::LerpColor(
					FLinearColor(1.f, 0.95f, 0.9f, 1.f),
					FLinearColor(1.f, 0.6f, 0.85f, 1.f),
					deg / 90.f);
			else if (deg < 180.0f)
				color = FMath::LerpColor(
					FLinearColor(1.f, 0.6f, 0.85f, 1.f),
					FLinearColor(0.65f, 0.5f, 1.f, 1.f),
					(deg - 90.f) / 90.f);
			else if (deg < 270.0f)
				color = FMath::LerpColor(
					FLinearColor(0.65f, 0.5f, 1.f, 1.f),
					FLinearColor(0.3f, 0.55f, 1.f, 1.f),
					(deg - 180.f) / 90.f);
			else
				color = FMath::LerpColor(
					FLinearColor(0.3f, 0.55f, 1.f, 1.f),
					FLinearColor(1.f, 0.95f, 0.9f, 1.f),
					(deg - 270.f) / 90.f);

			// 🕶️ Darken color
			color.R *= brightness;
			color.G *= brightness;
			color.B *= brightness;

			menu::Draw_Line(p1, p2, 2.0f, color);
		}
	}



	void DrawCircle(FVector2D pos, int radius, int numSides, FLinearColor Color)
	{
		float PI = 3.1415927f;

		float Step = PI * 2.0 / numSides;
		int Count = 0;
		FVector2D V[128];
		for (float A = 0; A < PI * 2.0; A += Step) {
			float X1 = radius * cos(A) + pos.X;
			float Y1 = radius * sin(A) + pos.Y;
			float X2 = radius * cos(A + Step) + pos.X;
			float Y2 = radius * sin(A + Step) + pos.Y;
			V[Count].X = X1;
			V[Count].Y = Y1;
			V[Count + 1].X = X2;
			V[Count + 1].Y = Y2;
			//Draw_Line(FVector2D{ pos.X, pos.Y }, FVector2D{ X2, Y2 }, 1.0f, Color); // Points from Centre to ends of circle
			Draw_Line(FVector2D{ V[Count].X, V[Count].Y }, FVector2D{ X2, Y2 }, 1.0f, Color);// Circle Around
		}
	}

	FVector2D dragPos;
	//bool Window(FString title, FVector2D* pos, FVector2D size, bool isOpen)
	//{
	//	rainbowTimeZ += 0.005f;
	//	FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZ);
	//	elements_count = 0;

	//	if (!isOpen)
	//		return false;

	//	bool isHovered = MouseInZone(FVector2D{ pos->X, pos->Y }, size);

	//	//Drop last element
	//	if (current_element != -1 && !GetAsyncKeyState(0x1))
	//	{
	//		current_element = -1;
	//	}

	//	//Drag
	//	if (hover_element && GetAsyncKeyState(0x1))
	//	{

	//	}
	//	else if ((isHovered || dragPos.X != 0) && !hover_element)
	//	{
	//		if (input::is_mouse_clicked(0, elements_count, true))
	//		{
	//			FVector2D cursorPos = CursorPos();

	//			cursorPos.X -= size.X;
	//			cursorPos.Y -= size.Y;

	//			if (dragPos.X == 0)
	//			{
	//				dragPos.X = (cursorPos.X - pos->X);
	//				dragPos.Y = (cursorPos.Y - pos->Y);
	//			}
	//			pos->X = cursorPos.X - dragPos.X;
	//			pos->Y = cursorPos.Y - dragPos.Y;
	//		}
	//		else
	//		{
	//			dragPos = FVector2D{ 0, 0 };
	//		}
	//	}
	//	else
	//	{
	//		hover_element = false;
	//	}


	//	offset_x = 15.0f; offset_y = 63.0f;
	//	menu_pos = FVector2D{ pos->X, pos->Y };
	//	first_element_pos = FVector2D{ 0, 0 };
	//	current_element_pos = FVector2D{ 0, 0 };
	//	current_element_size = FVector2D{ 0, 0 };

	//		drawFilledRect(FVector2D(pos->X, pos->Y), size.X, size.Y, RGBtoFLC(1.2, 1.2, 1.2));
	//		float rectX = pos->X;
	//		float rectY = pos->Y + 45.0f;
	//		float rectWidth = size.X;
	//		float rectHeight = 2.0f;

	//		int steps = 100; // Increase for smoother gradient
	//		float stepWidth = rectWidth / steps;

	//		for (int i = 0; i < steps; ++i)
	//		{
	//			float t = (float)i / (float)steps;
	//			float hue = fmodf(rainbowTimeZ + t, 1.0f); // Moving gradient effect
	//			FLinearColor color = HSVtoRGB(hue, 1.0f, 1.0f);

	//			float X = rectX + i * stepWidth;
	//			drawFilledRect(FVector2D(X, rectY), stepWidth + 1, rectHeight, color);
	//		}


	//		canvas->K2_DrawText( title, FVector2D(pos->X + (size.X / 2), pos->Y + (47 / 2)), FVector2D(1.15, 1.15), RGBtoFLC(225, 225, 225), 0.0f, RGBtoFLC(0, 0, 0), FVector2D(0, 0), true, true, false, RGBtoFLC(0, 0, 0));

	//	return true;
	//}


	void DrawSeparatorLine(FVector2D pos, float width, bool horizontal = true)
	{
		FLinearColor col = RGBtoFLC(35, 35, 35);
		if (horizontal)
			drawFilledRect(pos, width, 1, col);
		else
			drawFilledRect(pos, 1, width, col);
	}
	void drawOutlineRect(FVector2D pos, float w, float h, FLinearColor col)
	{
		drawFilledRect(FVector2D(pos.X, pos.Y), w, 1, col);
		drawFilledRect(FVector2D(pos.X, pos.Y + h - 1), w, 1, col);
		drawFilledRect(FVector2D(pos.X, pos.Y), 1, h, col);
		drawFilledRect(FVector2D(pos.X + w - 1, pos.Y), 1, h, col);
	}
	bool ButtonTab2(FString label, FVector2D size, bool selected)
	{
		elements_count++;
		const float paddingX = 4.f;
		FVector2D pos = FVector2D(menu::menu_pos.X + paddingX, menu::menu_pos.Y + menu::offset_y);

		bool hovered = MouseInZone(pos, size);

		// Only draw background if selected (NO background for hover or unselected)
		if (selected)
			drawFilledRect(pos, size.X, size.Y, RGBtoFLC(15, 15, 15));

		// Red selection bar
		if (selected)
			drawFilledRect(FVector2D(pos.X, pos.Y), 2, size.Y, RGBtoFLC(0, 120, 255));

		// Label
		canvas->K2_DrawText(label, FVector2D(pos.X + 14, pos.Y + 6),
			FVector2D(0.95f, 0.95f),
			selected ? RGBtoFLC(255, 255, 255) : hovered ? RGBtoFLC(170, 170, 170) : RGBtoFLC(130, 130, 130),
			0.f, RGBtoFLC(0, 0, 0), FVector2D(0, 0), false, false, false, RGBtoFLC(0, 0, 0));

		menu::offset_y += size.Y + 3;  // Slightly more padding

		return hovered && input::is_mouse_clicked(0, elements_count, false);
	}




	void BeginSubTab(FString label, FVector2D pos, FVector2D size)
	{
		// Background and border
		drawFilledRect(pos, size.X, size.Y, RGBtoFLC(2, 2, 2));                          // Slightly lighter than background
		drawOutlineRect(pos, size.X, size.Y, RGBtoFLC(15, 15, 15));                     // Thin outer border

		// Title text
		canvas->K2_DrawText(label, FVector2D(pos.X + 12, pos.Y + 9), FVector2D(0.9f, 0.9f),
			RGBtoFLC(110, 110, 110), 0.f,
			RGBtoFLC(0, 0, 0), FVector2D(0, 0), false, false, false, RGBtoFLC(0, 0, 0));

		// Horizontal separator (dark grey, spans width - 24 with margin)
		float separatorY = pos.Y + 28;                         // Slightly below text
		float separatorX = pos.X + 10;
		float separatorW = size.X - 20;                        // 10px padding left/right
		drawFilledRect(FVector2D(separatorX, separatorY), separatorW, 1, RGBtoFLC(25, 25, 25));
	}





	void SidebarCategory(FString label, FVector2D pos, float width)
	{
		canvas->K2_DrawText(label, FVector2D(pos.X + 12, pos.Y + 4),  // Slightly lower
			FVector2D(0.8f, 0.8f), RGBtoFLC(130, 130, 130), 0.f,
			RGBtoFLC(0, 0, 0), FVector2D(0, 0),
			false, false, false, RGBtoFLC(0, 0, 0));

		float labelWidth = label.Len() * 6.6f;
		float lineX = pos.X + 12 + labelWidth + 6;
		float lineY = pos.Y + 11; // Align with text bottom
		float lineW = width - (lineX - pos.X) - 10;

		drawFilledRect(FVector2D(lineX, lineY), lineW, 1, RGBtoFLC(35, 35, 35));
	}




	void DrawTitle(FVector2D pos)
	{
		// Title text
		canvas->K2_DrawText(
			L"BYT3.",
			FVector2D(pos.X + 130, pos.Y + 15),
			FVector2D(1.1f, 1.1f),
			RGBtoFLC(255, 255, 255), 0.f, RGBtoFLC(0, 0, 0),
			FVector2D(0, 0), false, false, false, RGBtoFLC(0, 0, 0)
		);
		canvas->K2_DrawText(

			L" AC",
			FVector2D(pos.X + 178, pos.Y + 15),
			FVector2D(1.1f, 1.1f),
			RGBtoFLC(255, 255, 255), 0.f, RGBtoFLC(0, 0, 0),
			FVector2D(0, 0), false, false, false, RGBtoFLC(0, 0, 0)
		);

		// subtle underline, exactly 90px wide
		DrawSeparatorLine(FVector2D(pos.X + 130, pos.Y + 32), 90, true);

	}


	bool Window(FVector2D* pos, FVector2D size, bool isOpen)
	{
		rainbowTimeZ += 0.005f;
		elements_count = 0;

		if (!isOpen)
			return false;

		// Main background
		drawFilledRect(*pos, size.X, size.Y, RGBtoFLC(1.2, 1.2, 1.2));

		// Sidebar background
		drawFilledRect(FVector2D(pos->X, pos->Y), 110, size.Y, RGBtoFLC(1.5, 1.5, 1.5));

		// Sidebar right separator
		drawFilledRect(FVector2D(pos->X + 110, pos->Y), 1, size.Y, RGBtoFLC(5, 5, 5));

		// Draw Title (integrated directly)
		canvas->K2_DrawText(
			L"BYTE",
			FVector2D(pos->X + 130, pos->Y + 15),
			FVector2D(1.1f, 1.1f),
			RGBtoFLC(255, 255, 255), 0.f, RGBtoFLC(0, 0, 0),
			FVector2D(0, 0), false, false, false, RGBtoFLC(0, 0, 0)
		);

		canvas->K2_DrawText(
			L".VIP",
			FVector2D(pos->X + 172, pos->Y + 15),
			FVector2D(1.1f, 1.1f),
			RGBtoFLC(0, 120, 255), 0.f, RGBtoFLC(0, 0, 0),
			FVector2D(0, 0), false, false, false, RGBtoFLC(0, 0, 0)
		);


		// --- Draw top header outline around title and subtabs ---
		float sidebarWidth = 110; // whatever your actual value is
		float outlineX = pos->X + sidebarWidth; // start after sidebar
		float outlineY = pos->Y;
		float outlineW = size.X - sidebarWidth; // not full window, subtract sidebar width
		float outlineH = 48; // or whatever your header height is

		drawOutlineRect(
			FVector2D(outlineX, outlineY),
			outlineW,
			outlineH,
			RGBtoFLC(15, 15, 15) // outline color
		);





		// Dragging logic
		bool hovered = MouseInZone(*pos, size);
		if (current_element != -1 && !GetAsyncKeyState(VK_LBUTTON))
			current_element = -1;

		if (hover_element && GetAsyncKeyState(VK_LBUTTON)) {}
		else if ((hovered || dragPos.X != 0) && !hover_element) {
			if (input::is_mouse_clicked(0, elements_count, true)) {
				FVector2D cur = CursorPos();
				cur.X -= size.X;
				cur.Y -= size.Y;

				if (dragPos.X == 0) {
					dragPos.X = cur.X - pos->X;
					dragPos.Y = cur.Y - pos->Y;
				}
				pos->X = cur.X - dragPos.X;
				pos->Y = cur.Y - dragPos.Y;
			}
			else dragPos = FVector2D{ 0, 0 };
		}
		else hover_element = false;

		// Layout base register
		menu::menu_pos = *pos;
		menu::offset_y = 40;  // Raised to better match the reference

		return true;
	}
	// --- InputText that doesn't move with the menu drag ---
// --- InputText that doesn't move with the menu drag ---
	void InputText(const wchar_t* label, wchar_t* buffer, int bufferSize)
	{
		elements_count++;

		const float inputWidth = 180.f;
		const float inputHeight = 20.f;
		const float padY = 7.f;
		const float labelSpacing = 2.f;
		const float textScale = 0.96f;

		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);

		// Draw label
		canvas->K2_DrawText(
			label,
			FVector2D(pos.X, pos.Y),
			FVector2D(textScale, textScale),
			RGBtoFLC(255, 255, 255),
			0.0f, colors::Text_Shadow,
			FVector2D(0, 0), false, false, false, colors::Text_Outline
		);

		// Position below label
		FVector2D inputPos = FVector2D(pos.X, pos.Y + inputHeight + labelSpacing);

		// Draw input background
		drawFilledRect(inputPos, inputWidth, inputHeight, RGBtoFLC(6, 6, 6));
		drawOutlineRect(inputPos, inputWidth, inputHeight, RGBtoFLC(30, 30, 30));

		// Render input text
		canvas->K2_DrawText(
			buffer,
			FVector2D(inputPos.X + 4, inputPos.Y + 2),
			FVector2D(0.9f, 0.9f),
			RGBtoFLC(255, 255, 255),
			0.0f, colors::Text_Shadow,
			FVector2D(0, 0), false, false, false, colors::Text_Outline
		);

		// Optional: Make it editable with key input (basic impl)
		if (MouseInZone(inputPos, FVector2D(inputWidth, inputHeight))) {
			static bool editing = false;
			if (input::is_mouse_clicked(0, elements_count, false)) {
				editing = true;
			}

			if (editing) {
				for (int vk = 0x20; vk <= 0x5A; ++vk) {
					if (GetAsyncKeyState(vk) & 1) {
						wchar_t ch = 0;

						// Check if character is A-Z
						if (vk >= 'A' && vk <= 'Z') {
							bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
							bool caps = (GetKeyState(VK_CAPITAL) & 0x0001);
							bool upper = shift ^ caps;

							ch = upper ? (wchar_t)vk : (wchar_t)(vk + 32); // to lowercase
						}
						else {
							ch = (wchar_t)vk; // leave numbers/symbols as-is
						}

						size_t len = wcslen(buffer);
						if (len < bufferSize - 1) {
							buffer[len] = ch;
							buffer[len + 1] = L'\0';
						}
					}
				}

				if (GetAsyncKeyState(VK_BACK) & 1) {
					size_t len = wcslen(buffer);
					if (len > 0) buffer[len - 1] = L'\0';
				}

				if (GetAsyncKeyState(VK_RETURN) & 1) {
					editing = false;
				}
			}
		}

		// Move down for next control
		menu::offset_y += inputHeight + labelSpacing + padY;
	}









#include <wchar.h> // for swprintf


	bool Watermark(FVector2D pos, FVector2D size, bool isOpen, int mode = 1) // 1 = box, 0 = base
	{
		if (!isOpen)
			return false;

		rainbowTimeZ += 0.00001f;

		// === BASE MODE ===
		if (mode == 0)
		{
			FString baseText = L"BYTE.VGC";
			float baseX = pos.X;
			float baseY = pos.Y;

			for (int i = 0; i < baseText.size(); ++i)
			{
				float hue = fmodf(rainbowTimeZ + i * 0.05f, 1.0f);
				FLinearColor letterColor = HSVtoRGB(hue, 1.0f, 1.0f);
				wchar_t ch[2] = { baseText[i], '\0' }; // Null-terminated single MyShooter
				FString singleChar = ch;                // Implicit constructor from wchar_t*
				canvas->K2_DrawTextx(
					singleChar,
					FVector2D(baseX, baseY),
					FVector2D(0.9f, 0.9f),
					letterColor,
					0.0f,
					RGBtoFLC(0, 0, 0),
					FVector2D(1, 1),
					true,
					true,
					true,
					RGBtoFLC(0, 0, 0)
				);

				baseX += 11.0f;
			}

			return true;
		}
		rainbowTimeZ += 0.005f;
		FLinearColor RainbowColor = BOSSRAINBOMAW(rainbowTimeZ);
		// Get current time
		std::time_t now = std::time(nullptr);
		std::tm* localTime = std::localtime(&now);

		// Format time into wchar_t buffer
		wchar_t timeBuffer[16];
		swprintf(timeBuffer, sizeof(timeBuffer) / sizeof(wchar_t), L"%02d:%02d:%02d", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
		FString nigga;
		// Format full final text
		wchar_t finalBuffer[128];
		 nigga = ShooterGameBlueprints::GetValVersion();

		swprintf(finalBuffer, sizeof(finalBuffer) / sizeof(wchar_t), L"%s | %s", L"BYTE - VGC" , nigga.c_str());

		// Now create a FString properly (not touching Data/Count manually!)
		FString finalText(finalBuffer); // <-- correct constructor usage

		drawFilledRect(pos, size.X, size.Y, RGBtoFLC(1.2, 1.2, 1.2));
		drawFilledRect(FVector2D(pos.X, pos.Y + size.Y - 2), size.X, 2, RainbowColor);

		canvas->K2_DrawText(
			finalText,
			FVector2D(pos.X + size.X / 2, pos.Y + size.Y / 2),
			FVector2D(0.9f, 0.9f),
			RGBtoFLC(255, 255, 255),
			0.0f,
			RGBtoFLC(0, 0, 0),
			FVector2D(1, 1),
			true,
			true,
			false,
			RGBtoFLC(0, 0, 0)
		);

		return true;

	}

	enum AnimationState { MOVING_RIGHT, WAITING, MOVING_LEFT };
	AnimationState current_state = MOVING_RIGHT;
	//static float pushYvalue = 0.0f;
	static float rect_speed = 5.0f;
	static float line_shrink_speed = 0.6f;
	static FVector2D rect_position = FVector2D{ 0.0f, 0.0f };
	static float rect_width = 150.0f;
	static float rect_height = 45.0f;
	static float line_width = 150.0f;
	static bool moving_right = true;
	float screen_center_x = canvas->GetScreenSize().X + 150.0f;
	float screen_center_y = canvas->GetScreenSize().Y;
	void UpdateAnimation()
	{
		// Constants
		const int text_padding = 4;

		switch (current_state)
		{
		case MOVING_RIGHT:
			if (rect_position.X < screen_center_x)
			{
				rect_position.X += 5;
			}

			rect_position.Y = 10;

			if (rect_position.X + rect_width / 2 >= screen_center_x)
			{
				current_state = WAITING;
			}
			break;

		case WAITING:
			line_width -= line_shrink_speed;
			if (line_width <= 0)
			{
				line_width = 0;
				current_state = MOVING_LEFT;
			}
			break;

		case MOVING_LEFT:
			rect_position.X -= rect_speed;
			if (rect_position.X <= 0)
			{
				rect_position.X = 0;
				rect_width = 0;
				rect_height = 0;
				line_width = 0;
				moving_right = false;
			}
			break;
		}

		// Draw background box
		FLinearColor boxColor = FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f };  // white
		//drawFilledRect(rect_position, rect_width, rect_height, boxColor);

		// Draw progress line
		FVector2D line_start = FVector2D{ rect_position.X + 1.f, rect_position.Y + rect_height };
		FVector2D line_end = FVector2D{ rect_position.X + line_width, rect_position.Y + rect_height };
		FLinearColor lineColor = FLinearColor{ 0.f, 0.1f, 0.6f, 1.0f };  // blue
		//Draw_Line(line_start, line_end, 5, lineColor);

		// Determine contrast text color (black on white background)
		FLinearColor textColor = FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f };  // black

		// Center text within the box
		float text_width = 100.0f;  // adjust if you can measure actual text width
		float text_height = 25.0f;
		FVector2D textPos = FVector2D{
			rect_position.X + (rect_width - text_width) / 2.0f,
			rect_position.Y + (rect_height - text_height) / 2.0f
		};


	}


	bool ButtonTab(const wchar_t* name, FVector2D size, bool active)
	{
		elements_count++;

		FVector2D padding = FVector2D{ 0, 0 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + offset_x;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, size);

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		FVector2D textPos = FVector2D{ pos.X + size.X / 2, pos.Y + size.Y / 2 };

		//Bg
		if (active)
		{
			drawFilledRect(FVector2D(pos.X, pos.Y), size.X, size.Y, RGBtoFLC(3, 3, 3));
			canvas->K2_DrawText(name, textPos, FVector2D(1.05, 1.05), RGBtoFLC(255, 255, 255), 0.0f, RGBtoFLC(0, 0, 0), FVector2D(0, 0), true, true, false, RGBtoFLC(0, 0, 0));
		}
		else if (isHovered)
		{
			canvas->K2_DrawText(name, textPos, FVector2D(1.05, 1.05), RGBtoFLC(120, 120, 120), 0.0f, RGBtoFLC(0, 0, 0), FVector2D(0, 0), true, true, false, RGBtoFLC(0, 0, 0));
			hover_element = true;
		}
		else
		{
			canvas->K2_DrawText(name, textPos, FVector2D(1.05, 1.05), RGBtoFLC(60, 60, 60), 0.0f, RGBtoFLC(0, 0, 0), FVector2D(0, 0), true, true, false, RGBtoFLC(0, 0, 0));
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			return true;

		return false;
	}

	void DrawCheckMark(FVector2D pos, float sz, FLinearColor color)
	{
		float thickness = fmaxf(sz / 5.0f, 1.0f);
		sz -= thickness * 0.5f;
		pos.X += thickness * 0.25f;
		pos.Y += thickness * 0.25f;

		float third = sz / 3.0f;
		float bx = pos.X + third;
		float by = pos.Y + sz - third * 0.5f;

		FVector2D p1 = FVector2D{ bx - third, by - third };
		FVector2D p2 = FVector2D{ bx, by };
		FVector2D p3 = FVector2D{ bx + third * 2.0f, by - third * 2.0f };

		// Assuming A simple function to draw lines with thickness
		canvas->K2_DrawLine(p1, p2, thickness, color);
		canvas->K2_DrawLine(p2, p3, thickness, color);
	}

	void SectionWrapper(FString name, FVector2D size)
	{
		FVector2D padding = FVector2D{ 0, 0 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
		if (sameLine)
		{
			pos.X = menu_pos.X + padding.X + offset_x;
			pos.Y = menu_pos.Y + padding.Y + offset_y;
		}

		drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, RGBtoFLC(4, 4, 4));
		drawFilledRect(FVector2D{ pos.X + 1, pos.Y + 1 }, size.X - 2, size.Y - 2, RGBtoFLC(2, 2, 2));

		canvas->K2_DrawText(name, FVector2D(pos.X + (size.X / 2), pos.Y + (24 / 2)), FVector2D(1.05, 1.05), RGBtoFLC(255, 255, 255), 0.0f, RGBtoFLC(0, 0, 0), FVector2D(0, 0), true, true, false, RGBtoFLC(0, 0, 0));

		drawFilledRect(FVector2D{ pos.X + 1 + 13, pos.Y + 26 }, size.X - 28, 2, RGBtoFLC(3, 3, 3));

		menu::offset_y += 26 + 4;

		sameLine = false;
	}
	void Checkbox(const wchar_t* label, bool* value) {
		elements_count++;

		// --- Smaller style constants ---
		const float boxSize = 13.f;
		const float boxPadX = 214.f;        // tweak if you want closer/further from text
		const float padY = 7.f;
		const float textScale = 0.96f;      // slightly smaller text for compact look

		// Calculate absolute position
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);

		// Draw checkbox background
		drawFilledRect(FVector2D(pos.X + boxPadX, pos.Y), boxSize, boxSize, RGBtoFLC(7, 7, 7));
		drawFilledRect(FVector2D(pos.X + boxPadX + 1, pos.Y + 1), boxSize - 2, boxSize - 2, RGBtoFLC(2.5, 2.5, 2.5));

		// Gradient fill if checked
		if (*value) {
			drawGradientRect(
				FVector2D(pos.X + boxPadX + 2, pos.Y + 2),
				boxSize - 4,
				boxSize - 4,
				RGBtoFLC(0, 120, 255),
				RGBtoFLC(150, 0, 255)
			);
			// Optional: Draw a checkmark overlay if you want!
		}

		// Text: vertically center relative to checkbox
		float textOffsetY = (boxSize - 11.f) / 2.0f; // tweak 11.f if your font height differs
		FVector2D textPos = FVector2D(pos.X, pos.Y + textOffsetY);

		if (*value) {
			canvas->K2_DrawText(label, textPos, FVector2D(textScale, textScale), RGBtoFLC(255, 255, 255),
				0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, false, colors::Text_Outline);
		}
		else {
			canvas->K2_DrawText(label, textPos, FVector2D(textScale, textScale), RGBtoFLC(90, 90, 90),
				0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, false, colors::Text_Outline);
		}

		// --- Mouse handling: Click to toggle ---
		bool isHovered = MouseInZone(FVector2D(pos.X + boxPadX, pos.Y), FVector2D(boxSize, boxSize));
		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			*value = !*value;

		// --- Move offset_y down for next control ---
		menu::offset_y += boxSize + padY;
	}



	//void Checkbox(const wchar_t* name, bool* value)
	//{
	//	elements_count++;

	//	float sizex = 16.5;
	//	float xsize = 2;
	//	FVector2D padding = FVector2D{ 15,10 };
	//	FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
	//	if (sameLine)
	//	{
	//		pos.X = last_element_pos.X + last_element_size.X + padding.X;
	//		pos.Y = last_element_pos.Y;
	//	}
	//	if (pushY)
	//	{
	//		pos.Y = pushYvalue;
	//		pushY = false;
	//		pushYvalue = 0.0f;
	//		offset_y = pos.Y - menu_pos.Y;
	//	}
	//	bool isHovered = MouseInZone(FVector2D{ pos.X + 215, pos.Y }, FVector2D{ sizex, sizex });

	//	FVector2D textPos = FVector2D{ pos.X, pos.Y - xsize };

	//	drawFilledRect(FVector2D{ pos.X + 215, pos.Y }, sizex, sizex, RGBtoFLC(7, 7, 7));
	//	drawFilledRect(FVector2D{ pos.X + 215 + 1, pos.Y + 1 }, sizex - 2, sizex - 2, RGBtoFLC(2.5, 2.5, 2.5));

	//	if (!sameLine)
	//		offset_y += sizex + padding.Y;

	//	if (*value)
	//	{
	//		drawFilledRect(FVector2D{ pos.X + 215 + 2, pos.Y + 2 }, sizex - 4, sizex - 4, RGBtoFLC(0, 120, 255));
	//		DrawCheckMark(FVector2D(pos.X + 215 + 4, pos.Y + 4), sizex - 8, RGBtoFLC(3, 3, 3));
	//		canvas->K2_DrawText( name, textPos, FVector2D(1, 1), RGBtoFLC(255, 255, 255), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, false, colors::Text_Outline);
	//	}
	//	else {
	//		canvas->K2_DrawText( name, textPos, FVector2D(1, 1), RGBtoFLC(90, 90, 90), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, false, colors::Text_Outline);
	//	}

	//	sameLine = false;
	//	last_element_pos = pos;
	//	//last_element_size = size;
	//	if (first_element_pos.X == 0.0f)
	//		first_element_pos = pos;

	//	if (isHovered && input::is_mouse_clicked(0, elements_count, false))
	//		*value = !*value;
	//}

	bool checkbox_enabled[256];
	void Combobox(const wchar_t* name, FVector2D size, int* value, const wchar_t* arg, ...)
	{
		elements_count++;

		FVector2D padding = FVector2D{ 15, 30 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 20.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, size);

		//Bg
		if (isHovered || checkbox_enabled[elements_count])
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, RGBtoFLC(5, 5, 5));
			drawFilledRect(FVector2D{ pos.X + 1, pos.Y + 1 }, size.X - 2, size.Y - 2, RGBtoFLC(4, 4, 4));

			hover_element = true;
		}
		else
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, RGBtoFLC(5, 5, 5));
			drawFilledRect(FVector2D{ pos.X + 1, pos.Y + 1 }, size.X - 2, size.Y - 2, RGBtoFLC(3, 3, 3));
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Text
		FVector2D textPos = FVector2D{ pos.X - 2.0f, pos.Y - size.Y + 2.0f };
		canvas->K2_DrawText(name, textPos, FVector2D(0.99, 0.99), RGBtoFLC(255, 255, 255), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, true, colors::Text_Outline);

		canvas->K2_DrawText(s2wc("v"), FVector2D{ pos.X + size.X - 18, pos.Y - 2 + size.Y / 2 }, FVector2D(1.2, 0.96), RGBtoFLC(255, 255, 255), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, true, false, colors::Text_Outline);

		//Elements
		bool isHovered2 = false;
		FVector2D element_pos = pos;
		int num = 0;

		if (checkbox_enabled[elements_count])
		{
			current_element_size.X = element_pos.X - 5.0f;
			current_element_size.Y = element_pos.Y - 5.0f;
		}
		va_list arguments;
		for (va_start(arguments, arg); arg != NULL; arg = va_arg(arguments, const wchar_t*))
		{
			//Selected Element
			if (num == *value)
			{
				FVector2D _textPos = FVector2D{ pos.X + 5, pos.Y + size.Y / 2 };
				canvas->K2_DrawText(arg, _textPos, FVector2D(0.98, 0.98), RGBtoFLC(130, 130, 130), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, true, true, colors::Text_Outline);
			}

			if (checkbox_enabled[elements_count])
			{
				element_pos.Y += 25.0f;

				isHovered2 = MouseInZone(FVector2D{ element_pos.X, element_pos.Y }, FVector2D{ size.X, 25.0f });
				if (isHovered2)
				{
					hover_element = true;
					PostRenderer::drawFilledRect(FVector2D{ element_pos.X, element_pos.Y }, size.X, 25.0f, RGBtoFLC(4, 4, 4));

					//Click
					if (input::is_mouse_clicked(0, elements_count, false))
					{
						*value = num;
						checkbox_enabled[elements_count] = false;
					}
				}
				else
				{
					PostRenderer::drawFilledRect(FVector2D{ element_pos.X, element_pos.Y }, size.X, 25.0f, RGBtoFLC(3, 3, 3));
				}

				PostRenderer::TextLeft(arg, FVector2D{ element_pos.X + 10.0f, element_pos.Y + 5.0f }, RGBtoFLC(110, 110, 110), true);
			}
			num++;
		}
		va_end(arguments);
		if (checkbox_enabled[elements_count])
		{
			current_element_size.X = element_pos.X + 5.0f;
			current_element_size.Y = element_pos.Y + 5.0f;
		}


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = !checkbox_enabled[elements_count];
		}
		if (!isHovered && !isHovered2 && input::is_mouse_clicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = false;
		}
	}
	void Combobox2(const wchar_t* name, FVector2D size, int* value, const std::vector<const wchar_t*>& items)
	{
		elements_count++;

		FVector2D padding = FVector2D{ 15, 30 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 20.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, size);

		if (isHovered || checkbox_enabled[elements_count])
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, RGBtoFLC(5, 5, 5));
			drawFilledRect(FVector2D{ pos.X + 1, pos.Y + 1 }, size.X - 2, size.Y - 2, RGBtoFLC(4, 4, 4));
			hover_element = true;
		}
		else
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, RGBtoFLC(5, 5, 5));
			drawFilledRect(FVector2D{ pos.X + 1, pos.Y + 1 }, size.X - 2, size.Y - 2, RGBtoFLC(3, 3, 3));
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		FVector2D textPos = FVector2D{ pos.X - 2.0f, pos.Y - size.Y + 2.0f };
		canvas->K2_DrawText(name, textPos, FVector2D(0.99, 0.99), RGBtoFLC(255, 255, 255), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, true, colors::Text_Outline);
		canvas->K2_DrawText(s2wc("v"), FVector2D{ pos.X + size.X - 18, pos.Y - 2 + size.Y / 2 }, FVector2D(1.2, 0.96), RGBtoFLC(255, 255, 255), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, true, false, colors::Text_Outline);

		bool isHovered2 = false;
		FVector2D element_pos = pos;

		if (checkbox_enabled[elements_count])
		{
			current_element_size.X = element_pos.X - 5.0f;
			current_element_size.Y = element_pos.Y - 5.0f;
		}

		for (int num = 0; num < items.size(); num++)
		{
			const wchar_t* arg = items[num];
			if (num == *value)
			{
				FVector2D _textPos = FVector2D{ pos.X + 5, pos.Y + size.Y / 2 };
				canvas->K2_DrawText(arg, _textPos, FVector2D(0.98, 0.98), RGBtoFLC(130, 130, 130), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, true, true, colors::Text_Outline);
			}

			if (checkbox_enabled[elements_count])
			{
				element_pos.Y += 25.0f;
				isHovered2 = MouseInZone(FVector2D{ element_pos.X, element_pos.Y }, FVector2D{ size.X, 25.0f });

				if (isHovered2)
				{
					hover_element = true;
					PostRenderer::drawFilledRect(FVector2D{ element_pos.X, element_pos.Y }, size.X, 25.0f, RGBtoFLC(4, 4, 4));

					if (input::is_mouse_clicked(0, elements_count, false))
					{
						*value = num;
						checkbox_enabled[elements_count] = false;
					}
				}
				else
				{
					PostRenderer::drawFilledRect(FVector2D{ element_pos.X, element_pos.Y }, size.X, 25.0f, RGBtoFLC(3, 3, 3));
				}

				PostRenderer::TextLeft(arg, FVector2D{ element_pos.X + 10.0f, element_pos.Y + 5.0f }, RGBtoFLC(110, 110, 110), true);
			}
		}

		if (checkbox_enabled[elements_count])
		{
			current_element_size.X = element_pos.X + 5.0f;
			current_element_size.Y = element_pos.Y + 5.0f;
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = !checkbox_enabled[elements_count];
		}
		if (!isHovered && !isHovered2 && input::is_mouse_clicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = false;
		}
	}

	void ShowSkinCombobox(const wchar_t* name, FVector2D size, int* selectedSkin, const std::vector<SkinData>& skinData)
	{
		// Collect the skin display names in a vector of const wchar_t*
		std::vector<const wchar_t*> skinNames;
		for (const auto& skin : skinData)
		{
			skinNames.push_back(skin.SkinName);
		}

		// Pass the skin names to your Combobox function
		Combobox2(name, size, selectedSkin, skinNames);
	}

	void drawGradientRect(FVector2D pos, float width, float height, FLinearColor startColor, FLinearColor endColor);
	void SliderFloat(const wchar_t* name, float* value, float min, float max, const char* format = "%.0f")
	{
		elements_count++;
		FVector2D size = FVector2D{ 231, 10 };
		FVector2D slider_size = FVector2D{ 231, 10 };
		FVector2D adjust_zone = FVector2D{ 0, 20 };
		FVector2D padding = FVector2D{ 15, 10 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);

		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}

		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		bool isHovered = MouseInZone(
			FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y - adjust_zone.Y },
			FVector2D{ slider_size.X, slider_size.Y + adjust_zone.Y * 1.5f });

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		// Background
		if (isHovered || current_element == elements_count)
		{
			if (input::is_mouse_clicked(0, elements_count, true))
			{
				current_element = elements_count;
				FVector2D cursorPos = CursorPos();
				*value = ((cursorPos.X - pos.X) * ((max - min) / slider_size.X)) + min;
				if (*value < min) *value = min;
				if (*value > max) *value = max;
			}

			drawFilledRect(FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, RGBtoFLC(10, 10, 9));
			drawGradientRect(FVector2D{ pos.X + 1, pos.Y + slider_size.Y + padding.Y + 1 },
				slider_size.X - 2, slider_size.Y - 2,
				RGBtoFLC(30, 30, 30), RGBtoFLC(10, 10, 10));

			hover_element = true;
		}
		else
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, RGBtoFLC(10, 10, 9));
			drawGradientRect(FVector2D{ pos.X + 1, pos.Y + slider_size.Y + padding.Y + 1 },
				slider_size.X - 2, slider_size.Y - 2,
				RGBtoFLC(20, 20, 20), RGBtoFLC(4, 4, 4));
		}

		// Text Label
		FVector2D textPos = FVector2D{ pos.X, pos.Y - 2.0f };
		canvas->K2_DrawText(name, textPos, FVector2D(0.98, 0.98), RGBtoFLC(211, 211, 211), 0.0f,
			colors::Text_Shadow, FVector2D(0, 0), false, false, false, colors::Text_Outline);

		// Gradient Slider Fill
		float oneP = slider_size.X / (max - min);
		float fillWidth = oneP * (*value - min) - 2;
		if (fillWidth > 0)
		{
			drawGradientRect(
				FVector2D{ pos.X + 1, pos.Y + slider_size.Y + padding.Y + 1 },
				fillWidth,
				slider_size.Y - 2,
				RGBtoFLC(0, 120, 255), // start - vibrant rose pink
				RGBtoFLC(150, 0, 255)   // end   - deeper magenta pink

			);
		}

		// Value Display
		char buffer[32];
		sprintf_s(buffer, format, *value);
		canvas->K2_DrawText(
			s2wc(buffer),
			FVector2D(pos.X + slider_size.X - 14.0f, pos.Y - 2.0f),
			FVector2D(0.98, 0.98),
			RGBtoFLC(78, 78, 78),
			0.0f,
			colors::Text_Shadow,
			FVector2D(0, 0),
			true,
			false,
			false,
			colors::Text_Outline
		);

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
		menu::offset_y += 28.f;
	}

	int active_hotkey = -1;
	bool already_pressed = false;
	int active_hotkeySilent = -1;
	bool already_pressedSilent = false;
	std::string VirtualKeyCodeToString(UCHAR virtualKey)
	{
		UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
		if (virtualKey == VK_LBUTTON)
		{
			return ("MOUSE0");
		}
		if (virtualKey == VK_RBUTTON)
		{
			return ("MOUSE1");
		}
		if (virtualKey == VK_MBUTTON)
		{
			return ("MBUTTON");
		}
		if (virtualKey == VK_XBUTTON1)
		{
			return ("XBUTTON1");
		}
		if (virtualKey == VK_XBUTTON2)
		{
			return ("XBUTTON2");
		}


		CHAR szName[128];
		int result = 0;
		switch (virtualKey)
		{
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_RCONTROL: case VK_RMENU:
		case VK_LWIN: case VK_RWIN: case VK_APPS:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		default:
			result = GetKeyNameTextA(scanCode << 16, szName, 128);
		}

		return szName;
	}
	void HotkeySilent(const char* name, FVector2D size, int* key)
	{
		elements_count++;

		FVector2D padding = FVector2D{ 15, 10 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);

		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y + (last_element_size.Y / 2) - size.Y / 2;
		}

		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, size);

		// Background
		if (isHovered)
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, colors::Button_Hovered);
			hover_element = true;
		}
		else
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, colors::Button_Idle);
		}

		// Add "KEYBIND AIM" text
		FVector2D keybindTextPos = FVector2D{ pos.X + size.X + 10, pos.Y + (size.Y / 2) - 8 };
		TextLeft(L"KEYBIND SILENT", keybindTextPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		if (active_hotkeySilent == elements_count)
		{
			FVector2D textPos = FVector2D{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
			TextCenter(L"PressKey", textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

			if (!input::is_any_mouse_down())
			{
				already_pressedSilent = false;
			}

			if (!already_pressedSilent)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code))
					{
						*key = code; // FIXED: assign to *key, not key
						already_pressedSilent = -1;
					}
				}
			}
		}
		else
		{
			FVector2D textPos = FVector2D{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
			TextCenter(s2wc(VirtualKeyCodeToString(*key).c_str()), textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false); // FIXED: use *key

			if (isHovered)
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					already_pressedSilent = true;
					active_hotkeySilent = elements_count;

					// Queue Fix
					for (int code = 0; code < 255; code++)
						if (GetAsyncKeyState(code)) {}
				}
			}
			else
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					active_hotkeySilent = -1;
				}
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}

	void HotkeyAim(const char* name, FVector2D size, int* key)
	{
		elements_count++;

		FVector2D padding = FVector2D{ 15, 10 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);

		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y + (last_element_size.Y / 2) - size.Y / 2;
		}

		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, size);

		// Background
		if (isHovered)
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, colors::Button_Hovered);
			hover_element = true;
		}
		else
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, colors::Button_Idle);
		}

		// Add "KEYBIND AIM" text
		FVector2D keybindTextPos = FVector2D{ pos.X + size.X + 10, pos.Y + (size.Y / 2) - 8 };
		TextLeft(L"KEYBIND AIM", keybindTextPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		if (active_hotkey == elements_count)
		{
			FVector2D textPos = FVector2D{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
			TextCenter(L"PressKey", textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

			if (!input::is_any_mouse_down())
			{
				already_pressed = false;
			}

			if (!already_pressed)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code))
					{
						*key = code; // FIXED: assign to *key, not key
						already_pressed = -1;
					}
				}
			}
		}
		else
		{
			FVector2D textPos = FVector2D{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
			TextCenter(s2wc(VirtualKeyCodeToString(*key).c_str()), textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false); // FIXED: use *key

			if (isHovered)
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					already_pressed = true;
					active_hotkey = elements_count;

					// Queue Fix
					for (int code = 0; code < 255; code++)
						if (GetAsyncKeyState(code)) {}
				}
			}
			else
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					active_hotkey = -1;
				}
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}

	void Hotkey(const char* name, FVector2D size, int* key)
	{
		elements_count++;

		const FVector2D padding = FVector2D{ 80, 4 };
		FVector2D pos(menu::offset_x, menu::offset_y);

		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y - padding.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		// Position box on the right side
		float labelWidth = 80.f;
		FVector2D boxPos = FVector2D(pos.X + labelWidth + 10.f, pos.Y);
		FVector2D labelPos = FVector2D(pos.X, pos.Y + size.Y / 2.f);

		bool isHovered = MouseInZone(boxPos, size);

		// Draw label on left
		canvas->K2_DrawText(
			s2wc(name),
			labelPos,
			FVector2D(0.95f, 0.95f),
			RGBtoFLC(255, 255, 255),
			0.f, colors::Text_Shadow,
			FVector2D(0, 0),
			false, true, true,
			colors::Text_Outline
		);

		// Draw input box
		drawFilledRect(boxPos, size.X, size.Y, RGBtoFLC(10, 10, 10));
		drawFilledRect(FVector2D(boxPos.X + 1, boxPos.Y + 1), size.X - 2, size.Y - 2, RGBtoFLC(2, 2, 2));

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		// Draw key text or capture input
		FVector2D textPos = FVector2D(boxPos.X + size.X / 2.f, boxPos.Y + size.Y / 2.f);

		if (active_hotkey == elements_count)
		{
			canvas->K2_DrawText(L"...", textPos, FVector2D(0.99f, 0.99f),
				RGBtoFLC(60, 60, 60), 0.f, colors::Text_Shadow, FVector2D(0, 0),
				true, true, true, colors::Text_Outline);

			if (!menu::input::is_any_mouse_down())
				already_pressed = false;

			if (!already_pressed)
			{
				for (int code = 1; code < 255; ++code)
				{
					if (GetAsyncKeyState(code) & 0x1)
					{
						*key = code;
						active_hotkey = -1;
						break;
					}
				}
			}
		}
		else
		{
			std::wstring keyName = s2wc(VirtualKeyCodeToString(*key).c_str());
			canvas->K2_DrawText(keyName.c_str(), textPos, FVector2D(0.99f, 0.99f),
				RGBtoFLC(60, 60, 60), 0.f, colors::Text_Shadow, FVector2D(0, 0),
				true, true, true, colors::Text_Outline);

			if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			{
				already_pressed = true;
				active_hotkey = elements_count;

				for (int i = 1; i < 255; i++) GetAsyncKeyState(i);
			}
			else if (input::is_mouse_clicked(0, elements_count, false))
			{
				active_hotkey = -1;
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = FVector2D(labelWidth + size.X + 10.f, size.Y);

		if (first_element_pos.X == 0.f)
			first_element_pos = pos;
	}





	bool draw_color_picker(UCanvas* canvas, UObject* font, const wchar_t* label, FLinearColor& color, FVector2D pos, FVector2D mouse_pos, bool mouse_clicked) {
		const float hue_slider_width = 100.0f;
		const float hue_slider_height = 7.0f;
		const float sb_picker_size = 75.0f;
		const float alpha_slider_width = hue_slider_width;
		const float alpha_slider_height = 7.0f;
		const float preview_box_size = 15.0f;
		const float spacing = 5.0f;

		static bool hue_dragging = false;
		static bool sb_dragging = false;
		static bool alpha_dragging = false;

		static float hue = 0.0f;
		static float saturation = 1.0f;
		static float brightness = 1.0f;

		float thickness = 2.0f; // grosor del texto
		float width_scale = 1.0f; // escala de anchura
		float height_scale = 0.8f; // escala de altura
		float text_scale = 1.1f; // escala del tamaÃ±o de la fuente

		bool value_changed = false;

		for (int i = 0; i < hue_slider_width; i++) {
			float h = (float)i / hue_slider_width;
			FLinearColor color_at_hue = HSVtoRGB(h, 1.0f, 1.0f);
			draw_filled_rect(canvas, pos.X + i, pos.Y, 1.0f, hue_slider_height, color_at_hue); // raya arriba
		}
		draw_rect(canvas, pos.X, pos.Y, hue_slider_width, hue_slider_height, RGBtoFLC(5, 5, 5));

		if (mouse_clicked && mouse_pos.X >= pos.X && mouse_pos.X <= pos.X + hue_slider_width &&
			mouse_pos.Y >= pos.Y && mouse_pos.Y <= pos.Y + hue_slider_height) {
			hue_dragging = true;
		}
		if (hue_dragging) {
			if (mouse_clicked) {
				hue = (mouse_pos.X - pos.X) / hue_slider_width;
				hue = std::clamp(hue, 0.0f, 1.0f);
				value_changed = true;
			}
			else {
				hue_dragging = false;
			}
		}

		FVector2D sb_pos = { pos.X, pos.Y + hue_slider_height + spacing };

		for (int Y = 0; Y < sb_picker_size; Y++) {
			for (int X = 0; X < sb_picker_size; X++) {
				float s = (float)X / sb_picker_size;
				float B = 1.0f - (float)Y / sb_picker_size;
				FLinearColor sb_color = HSVtoRGB(hue, s, B);
				draw_filled_rect(canvas, sb_pos.X + X, sb_pos.Y + Y, 1.0f, 1.0f, sb_color);
			}
		}
		draw_rect(canvas, sb_pos.X, sb_pos.Y, sb_picker_size, sb_picker_size, RGBtoFLC(5, 5, 5));

		if (mouse_clicked && mouse_pos.X >= sb_pos.X && mouse_pos.X <= sb_pos.X + sb_picker_size &&
			mouse_pos.Y >= sb_pos.Y && mouse_pos.Y <= sb_pos.Y + sb_picker_size) {
			sb_dragging = true;
		}
		if (sb_dragging) {
			if (mouse_clicked) {
				saturation = (mouse_pos.X - sb_pos.X) / sb_picker_size;
				brightness = 1.0f - (mouse_pos.Y - sb_pos.Y) / sb_picker_size;
				saturation = std::clamp(saturation, 0.0f, 1.0f);
				brightness = std::clamp(brightness, 0.0f, 1.0f);
				value_changed = true;
			}
			else {
				sb_dragging = false;
			}
		}

		FVector2D alpha_pos = { pos.X, sb_pos.Y + sb_picker_size + spacing };

		for (int i = 0; i < alpha_slider_width; i++) {
			float alpha = (float)i / alpha_slider_width;
			FLinearColor alpha_color = HSVtoRGB(hue, saturation, brightness);
			alpha_color.A = alpha;
			draw_filled_rect(canvas, alpha_pos.X + i, alpha_pos.Y, 1.0f, alpha_slider_height, alpha_color);
		}
		draw_rect(canvas, alpha_pos.X, alpha_pos.Y, alpha_slider_width, alpha_slider_height, RGBtoFLC(5, 5, 5));

		if (mouse_clicked && mouse_pos.X >= alpha_pos.X && mouse_pos.X <= alpha_pos.X + alpha_slider_width &&
			mouse_pos.Y >= alpha_pos.Y && mouse_pos.Y <= alpha_pos.Y + alpha_slider_height) {
			alpha_dragging = true;
		}
		if (alpha_dragging) {
			if (mouse_clicked) {
				color.A = (mouse_pos.X - alpha_pos.X) / alpha_slider_width;
				color.A = std::clamp(color.A, 0.0f, 1.0f);
				value_changed = true;
			}
			else {
				alpha_dragging = false;
			}
		}

		FLinearColor final_color = HSVtoRGB(hue, saturation, brightness);
		color.R = final_color.R;
		color.G = final_color.G;
		color.B = final_color.B;

		FVector2D preview_pos = { pos.X + hue_slider_width + spacing, pos.Y };
		draw_filled_rect(canvas, preview_pos.X, preview_pos.Y, preview_box_size, preview_box_size, { color.R, color.G, color.B, color.A });
		draw_rect(canvas, preview_pos.X, preview_pos.Y, preview_box_size, preview_box_size, RGBtoFLC(5, 5, 5));

		return value_changed;
	}

	// END
	void Text(const wchar_t* text, bool center = false, bool outline = true, bool active = true)
	{
		elements_count++;

		float size = 24;
		float textScale = 1.0f;
		FVector2D padding = FVector2D{ 1, 3 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);

		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}

		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		if (!sameLine)
			offset_y += size + padding.Y;

		FVector2D textPos = FVector2D{ pos.X + 5.0f, pos.Y };

		// Choose color based on "active"
		FLinearColor renderColor = active
			? RGBtoFLC(255, 255, 255)   // bright white
			: RGBtoFLC(90, 90, 90);     // inactive gray

		FLinearColor shadowColor = colors::Text_Shadow;
		FVector2D shadowOffset = FVector2D(0, 0);
		FLinearColor outlineColor = colors::Text_Outline;

		// Draw using your canvas
		canvas->K2_DrawText(
			text,                      // label
			textPos,                   // position
			FVector2D(textScale, textScale), // scale
			renderColor,               // main text color
			0.0f,                      // kerning
			shadowColor,               // shadow color
			shadowOffset,              // shadow offset
			center,                    // center X
			false,                     // center Y (usually false)
			outline,                   // outline toggle
			outlineColor               // outline color
		);

		sameLine = false;
		last_element_pos = pos;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}

	bool Button(const wchar_t* name, FVector2D size)
	{
		elements_count++;

		FVector2D padding = FVector2D{ 15, 15 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, size);

		drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, RGBtoFLC(7, 7, 7));

		//Bg
		if (isHovered)
		{
			drawFilledRect(FVector2D{ pos.X + 1, pos.Y + 1 }, size.X - 2, size.Y - 2, RGBtoFLC(3, 3, 3));
			hover_element = true;
		}
		else
		{
			drawFilledRect(FVector2D{ pos.X + 1, pos.Y + 1 }, size.X - 2, size.Y - 2, RGBtoFLC(2, 2, 2));
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Text
		FVector2D textPos = FVector2D{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
		canvas->K2_DrawText(name, textPos, FVector2D(1.05, 0.99), RGBtoFLC(160, 160, 160), 0.0f, colors::Text_Shadow, FVector2D(0, 0), true, true, false, colors::Text_Outline);


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			return true;

		return false;
	}

	void SliderInt(const wchar_t* name, int* value, int min, int max, const char* format = "%d")
	{
		elements_count++;
		FVector2D size = FVector2D{ 231, 10 };
		FVector2D slider_size = FVector2D{ 231, 10 };
		FVector2D adjust_zone = FVector2D{ 0, 20 };
		FVector2D padding = FVector2D{ 15, 10 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);

		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}

		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		bool isHovered = MouseInZone(
			FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y - adjust_zone.Y },
			FVector2D{ slider_size.X, slider_size.Y + adjust_zone.Y * 1.5f });

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		if (isHovered || current_element == elements_count)
		{
			if (input::is_mouse_clicked(0, elements_count, true))
			{
				current_element = elements_count;

				FVector2D cursorPos = CursorPos();
				float ratio = (cursorPos.X - pos.X) / slider_size.X;
				int newValue = static_cast<int>(min + (max - min) * ratio + 0.5f);
				if (newValue < min) newValue = min;
				if (newValue > max) newValue = max;
				*value = newValue;
			}

			drawFilledRect(FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, RGBtoFLC(10, 10, 9));
			drawGradientRect(FVector2D{ pos.X + 1, pos.Y + slider_size.Y + padding.Y + 1 }, slider_size.X - 2, slider_size.Y - 2,
				RGBtoFLC(30, 30, 30), RGBtoFLC(10, 10, 10));
			hover_element = true;
		}
		else
		{
			drawFilledRect(FVector2D{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, RGBtoFLC(10, 10, 9));
			drawGradientRect(FVector2D{ pos.X + 1, pos.Y + slider_size.Y + padding.Y + 1 }, slider_size.X - 2, slider_size.Y - 2,
				RGBtoFLC(20, 20, 20), RGBtoFLC(4, 4, 4));
		}

		FVector2D textPos = FVector2D{ pos.X, pos.Y - 2.0f };
		canvas->K2_DrawText(name, textPos, FVector2D(0.98, 0.98), RGBtoFLC(211, 211, 211), 0.0f,
			colors::Text_Shadow, FVector2D(0, 0), false, false, false, colors::Text_Outline);

		float oneP = slider_size.X / float(max - min);
		float fillWidth = oneP * (*value - min) - 2;
		if (fillWidth > 0)
		{
			drawGradientRect(
				FVector2D{ pos.X + 1, pos.Y + slider_size.Y + padding.Y + 1 },
				fillWidth,
				slider_size.Y - 2,
				RGBtoFLC(0, 120, 255), // start color
				RGBtoFLC(150, 0, 255)   // end color
			);
		}

		char buffer[32];
		sprintf_s(buffer, format, *value);
		canvas->K2_DrawText(
			s2wc(buffer),
			FVector2D(pos.X + slider_size.X - 14.0f, pos.Y - 2.0f),
			FVector2D(0.98, 0.98),
			RGBtoFLC(78, 78, 78),
			0.0f,
			colors::Text_Shadow,
			FVector2D(0, 0),
			true,
			false,
			false,
			colors::Text_Outline
		);

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}


	int active_picker = -1;
	FLinearColor saved_color;
	bool ColorPixel(FVector2D pos, FVector2D size, FLinearColor* original, FLinearColor color)
	{
		PostRenderer::drawFilledRect(FVector2D{ pos.X, pos.Y }, size.X, size.Y, color);

		//Выбранный цвет
		if (original->R == color.R && original->G == color.G && original->B == color.B)
		{
			PostRenderer::Draw_Line(FVector2D{ pos.X, pos.Y }, FVector2D{ pos.X + size.X - 1, pos.Y }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
			PostRenderer::Draw_Line(FVector2D{ pos.X, pos.Y + size.Y - 1 }, FVector2D{ pos.X + size.X - 1, pos.Y + size.Y - 1 }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
			PostRenderer::Draw_Line(FVector2D{ pos.X, pos.Y }, FVector2D{ pos.X, pos.Y + size.Y - 1 }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
			PostRenderer::Draw_Line(FVector2D{ pos.X + size.X - 1, pos.Y }, FVector2D{ pos.X + size.X - 1, pos.Y + size.Y - 1 }, 1.0f, FLinearColor{ 0.0f, 0.0f, 0.0f, 1.0f });
		}

		//Смена цвета
		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, size);
		if (isHovered)
		{
			if (input::is_mouse_clicked(0, elements_count, false))
				*original = color;
		}

		return true;
	}
	void ColorPicker_Fresnel(const wchar_t* name, FLinearColor* color)
	{
		elements_count++;

		float size = 15;
		float xsize = 5;
		FVector2D padding = FVector2D{ 15, 10 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, FVector2D{ size, size });

		if (!sameLine)
			offset_y += size + padding.Y;

		if (active_picker == elements_count)
		{
			hover_element = true;

			float sizePickerX = 260;
			float sizePickerY = 260;
			bool isHoveredPicker = MouseInZone(FVector2D{ pos.X, pos.Y }, FVector2D{ sizePickerX, sizePickerY - 60 });

			//Background
			PostRenderer::drawFilledRect(FVector2D{ pos.X, pos.Y }, sizePickerX - 120.0f, sizePickerY - 210, menu::colors::ColorPicker_Background);

			FVector2D pixelSize = FVector2D{ sizePickerX / 12, sizePickerY / 12 };

			//0
			{
				ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.f, 24.0f, 0.0f, 1.0f });

			}
			//1
			{
				ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 24.0f, 0.0f, 24.0f , 1.0f });

			}
			//2
			{
				ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 24.0f, 0.0f, 0.0f, 1.0f });
			}
			//2
			{
				ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 24.0f,24.0f, 0.0f, 1.0f });
			}
			//1
			{
				ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.0f, 25.0f, 25.0f , 1.0f });

			}

			if (isHoveredPicker)
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{

				}
			}
			else
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					active_picker = -1;
					//hover_element = false;
				}
			}
		}
		else
		{
			//Bg
			if (isHovered)
			{
				drawFilledRect(FVector2D{ pos.X, pos.Y }, size, size, menu::colors::Checkbox_Hovered);
				hover_element = true;
			}
			else
			{
				drawFilledRect(FVector2D{ pos.X, pos.Y }, size, size, menu::colors::Checkbox_Idle);
			}

			//Color
			drawFilledRect(FVector2D{ pos.X + 4, pos.Y + 4 }, size - 8, size - 8, *color);

			//Text
			FVector2D textPos = FVector2D{ pos.X + size + 5.0f, pos.Y - xsize + 4.0f };
			canvas->K2_DrawText(name, textPos, FVector2D(0.98f, 0.98f), RGBtoFLC(192, 192, 192), 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, true, colors::Text_Outline);

			if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			{
				saved_color = *color;
				active_picker = elements_count;
			}
		}


		sameLine = false;
		last_element_pos = pos;
		//last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}

	void ColorPicker(const wchar_t* name, FLinearColor* color)
	{
		elements_count++;

		float size = 20;
		float xsize = 5;
		FVector2D padding = FVector2D{ 15, 10 };
		FVector2D pos = FVector2D(menu::offset_x, menu::offset_y);
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(FVector2D{ pos.X, pos.Y }, FVector2D{ size, size });

		if (!sameLine)
			offset_y += size + padding.Y;

		if (active_picker == elements_count)
		{
			hover_element = true;
			float sizePickerX = 250;
			float sizePickerY = 250;
			bool isHoveredPicker = MouseInZone(FVector2D{ pos.X, pos.Y }, FVector2D{ sizePickerX, sizePickerY - 60 });
			PostRenderer::drawFilledRect(FVector2D{ pos.X, pos.Y }, sizePickerX, sizePickerY - 65, colors::ColorPicker_Background);
			FVector2D pixelSize = FVector2D{ sizePickerX / 12, sizePickerY / 12 };

			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 1.000f, 0.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 1.000f, 0.500f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 1.000f, 1.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.500f, 1.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.000f, 1.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.000f, 1.000f, 0.500f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.000f, 1.000f, 1.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.000f, 0.500f, 1.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.000f, 0.000f, 1.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 0.500f, 0.000f, 1.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 1.000f, 0.000f, 1.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 0 }, pixelSize, color, FLinearColor{ 1.000f, 0.000f, 0.500f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.925f, 0.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.925f, 0.463f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.925f, 0.925f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.463f, 0.925f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.000f, 0.925f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.000f, 0.925f, 0.463f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.000f, 0.925f, 0.925f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.000f, 0.463f, 0.925f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.000f, 0.000f, 0.925f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.463f, 0.000f, 0.925f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.925f, 0.000f, 0.925f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 1 }, pixelSize, color, FLinearColor{ 0.925f, 0.000f, 0.463f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.850f, 0.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.850f, 0.425f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.850f, 0.850f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.425f, 0.850f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.000f, 0.850f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.000f, 0.850f, 0.425f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.000f, 0.850f, 0.850f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.000f, 0.425f, 0.850f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.000f, 0.000f, 0.850f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.425f, 0.000f, 0.850f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.850f, 0.000f, 0.850f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 2 }, pixelSize, color, FLinearColor{ 0.850f, 0.000f, 0.425f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.775f, 0.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.775f, 0.388f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.775f, 0.775f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.388f, 0.775f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.000f, 0.775f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.000f, 0.775f, 0.388f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.000f, 0.775f, 0.775f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.000f, 0.388f, 0.775f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.000f, 0.000f, 0.775f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.388f, 0.000f, 0.775f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.775f, 0.000f, 0.775f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 3 }, pixelSize, color, FLinearColor{ 0.775f, 0.000f, 0.388f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.700f, 0.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.700f, 0.350f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.700f, 0.700f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.350f, 0.700f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.000f, 0.700f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.000f, 0.700f, 0.350f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.000f, 0.700f, 0.700f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.000f, 0.350f, 0.700f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.000f, 0.000f, 0.700f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.350f, 0.000f, 0.700f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.700f, 0.000f, 0.700f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 4 }, pixelSize, color, FLinearColor{ 0.700f, 0.000f, 0.350f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.625f, 0.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.625f, 0.312f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.625f, 0.625f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.312f, 0.625f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.000f, 0.625f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.000f, 0.625f, 0.312f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.000f, 0.625f, 0.625f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.000f, 0.312f, 0.625f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.000f, 0.000f, 0.625f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.312f, 0.000f, 0.625f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.625f, 0.000f, 0.625f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 5 }, pixelSize, color, FLinearColor{ 0.625f, 0.000f, 0.312f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.550f, 0.000f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.550f, 0.275f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.550f, 0.550f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.275f, 0.550f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.000f, 0.550f, 0.000f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.000f, 0.550f, 0.275f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.000f, 0.550f, 0.550f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.000f, 0.275f, 0.550f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.000f, 0.000f, 0.550f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.275f, 0.000f, 0.550f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.550f, 0.000f, 0.550f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 6 }, pixelSize, color, FLinearColor{ 0.550f, 0.000f, 0.275f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.475f, 0.190f, 0.190f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.475f, 0.332f, 0.190f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.475f, 0.475f, 0.190f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.332f, 0.475f, 0.190f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.190f, 0.475f, 0.190f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.190f, 0.475f, 0.332f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.190f, 0.475f, 0.475f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.190f, 0.332f, 0.475f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.190f, 0.190f, 0.475f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.332f, 0.190f, 0.475f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.475f, 0.190f, 0.475f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 7 }, pixelSize, color, FLinearColor{ 0.475f, 0.190f, 0.332f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.400f, 0.160f, 0.160f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.400f, 0.280f, 0.160f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.400f, 0.400f, 0.160f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.280f, 0.400f, 0.160f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.160f, 0.400f, 0.160f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.160f, 0.400f, 0.280f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.160f, 0.400f, 0.400f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.160f, 0.280f, 0.400f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.160f, 0.160f, 0.400f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.280f, 0.160f, 0.400f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.400f, 0.160f, 0.400f, 1.0f });
			ColorPixel(FVector2D{ pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 8 }, pixelSize, color, FLinearColor{ 0.400f, 0.160f, 0.280f, 1.0f });

			if (isHoveredPicker)
			{
				if (input::is_mouse_clicked(0, elements_count, false)) {}
			}
			else
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					active_picker = -1;
				}
			}
		}
		else
		{
			//Bg
			if (isHovered)
			{
				drawFilledRect(FVector2D{ pos.X, pos.Y }, size, size, colors::Checkbox_Hovered);
				hover_element = true;
			}
			else
			{
				drawFilledRect(FVector2D{ pos.X, pos.Y }, size, size, colors::Checkbox_Idle);
			}

			//Color
			drawFilledRect(FVector2D{ pos.X + 4, pos.Y + 4 }, size - 8, size - 8, *color);

			//Text
			FVector2D textPos = FVector2D{ pos.X + size + 5.0f, pos.Y - xsize + 6.0f };
			TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

			if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			{
				saved_color = *color;
				active_picker = elements_count;
			}
		}


		sameLine = false;
		last_element_pos = pos;
		//last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}
	void Render()
	{
		for (int i = 0; i < 128; i++)
		{
			if (PostRenderer::drawlist[i].type != -1)
			{
				//Filled Rect
				if (PostRenderer::drawlist[i].type == 1)
				{
					menu::drawFilledRect(PostRenderer::drawlist[i].pos, PostRenderer::drawlist[i].size.X, PostRenderer::drawlist[i].size.Y, PostRenderer::drawlist[i].color);
				}
				//TextLeft
				else if (PostRenderer::drawlist[i].type == 2)
				{
					canvas->K2_DrawText((const wchar_t*)PostRenderer::drawlist[i].name, PostRenderer::drawlist[i].pos, FVector2D(0.98, 0.98), PostRenderer::drawlist[i].color, 0.0f, colors::Text_Shadow, FVector2D(0, 0), false, false, PostRenderer::drawlist[i].outline, colors::Text_Outline);
				}
				//TextCenter
				else if (PostRenderer::drawlist[i].type == 3)
				{
					menu::TextCenter((const wchar_t*)PostRenderer::drawlist[i].name, PostRenderer::drawlist[i].pos, PostRenderer::drawlist[i].color, PostRenderer::drawlist[i].outline);
				}
				//Draw_Line
				else if (PostRenderer::drawlist[i].type == 4)
				{
					Draw_Line(PostRenderer::drawlist[i].from, PostRenderer::drawlist[i].to, PostRenderer::drawlist[i].thickness, PostRenderer::drawlist[i].color);
				}

				PostRenderer::drawlist[i].type = -1;
			}
		}
	}

}




struct AGameObject
{
	char pad_0[0x4a0];
	FText DisplayName;

	FVector GetObjectLocation()
	{

		auto function_name = (L"Engine.Actor.K2_GetActorLocation");
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return FVector();

		struct {
			FVector Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}

	FText GetDisplayName()
	{

		return this->DisplayName;
	}

	float GetObjectHealth()
	{

		auto function_name = (L"ShooterGame.GameObject.GetHealth");
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!Function)
			return 0;

		struct {
			float Out;
		} Args;

		Function->ProcessEvent(this, Function, &Args);
		return Args.Out;
	}
};
namespace Filters
{
	namespace Names
	{
		FString CharacterName(FString in)
		{
			if (in.ToString().find("Training") != std::string::npos)
				return FString(L"Bot Lobby");
			if (in.ToString().find("BountyHunter_PC_C") != std::string::npos)
				return FString(L"Fade");
			if (in.ToString().find("Stealth_PC_C") != std::string::npos)
				return FString(L"Yoru");
			if (in.ToString().find("Pandemic_PC_C") != std::string::npos)
				return FString(L"Viper");
			if (in.ToString().find("Hunter_PC_C") != std::string::npos)
				return FString(L"Sova");
			if (in.ToString().find("Guide_PC_C") != std::string::npos)
				return FString(L"Skye");
			if (in.ToString().find("Thorne_PC_C") != std::string::npos)
				return FString(L"Sage");
			if (in.ToString().find("Vampire_PC_C") != std::string::npos)
				return FString(L"Reyna");
			if (in.ToString().find("Clay_PC_C") != std::string::npos)
				return FString(L"Raze");
			if (in.ToString().find("Phoenix_PC_C") != std::string::npos)
				return FString(L"Phoenix");
			if (in.ToString().find("Wraith_PC_C") != std::string::npos)
				return FString(L"Omen");
			if (in.ToString().find("Sprinter_PC_C") != std::string::npos)
				return FString(L"Neon");
			if (in.ToString().find("Killjoy_PC_C") != std::string::npos)
				return FString(L"Killjoy");
			if (in.ToString().find("Grenadier_PC_C") != std::string::npos)
				return FString(L"Kayo");
			if (in.ToString().find("Terra_PC_C") != std::string::npos)
				return FString(L"Waylay");
			if (in.ToString().find("Cashew_PC_C") != std::string::npos)
				return FString(L"Tejo");
			if (in.ToString().find("Wushu_PC_C") != std::string::npos)
				return FString(L"Jett");
			if (in.ToString().find("Gumshoe_PC_C") != std::string::npos)
				return FString(L"Cypher");
			if (in.ToString().find("Deadeye_PC_C") != std::string::npos)
				return FString(L"Chamber");
			if (in.ToString().find("Sarge_PC_C") != std::string::npos)
				return FString(L"Brimstone");
			if (in.ToString().find("Breach_PC_C") != std::string::npos)
				return FString(L"Breach");
			if (in.ToString().find("Rift_TargetingForm_PC_C") != std::string::npos)
				return FString(L"Astra");
			if (in.ToString().find("Rift_PC_C") != std::string::npos)
				return FString(L"Astra");
			if (in.ToString().find("Mage_PC_C") != std::string::npos)
				return FString(L"Harbor");
			if (in.ToString().find("AggroBot_PC_C") != std::string::npos)
				return FString(L"Gekko");
			if (in.ToString().find("Cable_PC_C") != std::string::npos)
				return FString(L"DeadLock");
			if (in.ToString().find("Pawn_TrainingBot_DanceHall_Easy_C") != std::string::npos)
				return FString(L"Bot Lobby");
			if (in.ToString().find("Pawn_TrainingBot_Defuse_Ultimate_C") != std::string::npos)
				return FString(L"Bot Lobby");
			if (in.ToString().find("Sequoia_PC_C") != std::string::npos)
				return FString(L"Iso");
			if (in.ToString().find("Smonk_PC_C") != std::string::npos)
				return FString(L"Clove");
			if (in.ToString().find("Nox_PC_C") != std::string::npos)
				return FString(L"Vyse");
			else
				return FString(L"");
		}
		FString WeaponName(FString in)
		{
			if (in.ToString().find("Ability_Melee_Base_C") != std::string::npos)
				return FString(L"Knife");
			if (in.ToString().find("BasePistol_C") != std::string::npos)
				return FString(L"Classic");
			if (in.ToString().find("TrainingBotBasePistol_C") != std::string::npos)
				return FString(L"Classic");
			if (in.ToString().find("SawedOffShotgun_C") != std::string::npos)
				return FString(L"Shorty");
			if (in.ToString().find("AutomaticPistol_C") != std::string::npos)
				return FString(L"Frenzy");
			if (in.ToString().find("LugerPistol_C") != std::string::npos)
				return FString(L"Ghost");
			if (in.ToString().find("RevolverPistol_C") != std::string::npos)
				return FString(L"Sheriff");
			if (in.ToString().find("Vector_C") != std::string::npos)
				return FString(L"Stinger");
			if (in.ToString().find("SubMachineGun_MP5_C") != std::string::npos)
				return FString(L"Spectre");
			if (in.ToString().find("PumpShotgun_C") != std::string::npos)
				return FString(L"Bucky");
			if (in.ToString().find("AutomaticShotgun_C") != std::string::npos)
				return FString(L"Judge");
			if (in.ToString().find("AssaultRifle_Burst_C") != std::string::npos)
				return FString(L"Bulldog");
			if (in.ToString().find("DMR_C") != std::string::npos)
				return FString(L"Guardian");
			if (in.ToString().find("AssaultRifle_ACR_C") != std::string::npos)
				return FString(L"Phantom");
			if (in.ToString().find("AssaultRifle_AK_C") != std::string::npos)
				return FString(L"Vandal");
			if (in.ToString().find("LeverSniperRifle_C") != std::string::npos)
				return FString(L"Marshall");
			if (in.ToString().find("BoltSniper_C") != std::string::npos)
				return FString(L"Operator");
			if (in.ToString().find("LightMachineGun_C") != std::string::npos)
				return FString(L"Ares");
			if (in.ToString().find("HeavyMachineGun_C") != std::string::npos)
				return FString(L"Odin");
			if (in.ToString().find("DS_Gun_C") != std::string::npos)
				return FString(L"Outlaw");
			else
				return FString(L"");
		}

		FString ItsGamerDoc(FString in)
		{
			if (in.ToString().find("Ability_Melee_Base_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("BasePistol_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("TrainingBotBasePistol_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("SawedOffShotgun_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("AutomaticPistol_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("LugerPistol_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("RevolverPistol_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("Vector_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("SubMachineGun_MP5_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("PumpShotgun_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("AutomaticShotgun_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("AssaultRifle_Burst_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("DMR_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("AssaultRifle_ACR_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("AssaultRifle_AK_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("LeverSniperRifle_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("BoltSniper_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("LightMachineGun_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			if (in.ToString().find("HeavyMachineGun_C") != std::string::npos)
				return FString(L"ItsGamerDoc");
			else
				return FString(L"ItsGamerDoc");
		}
	}

}
namespace Class
{
	static UObject* Actors()
	{
		return UObject::find_object(ez(L"Engine.Actor"));
	}
	UObject* ShooterGameObject()
	{

		return UObject::StaticFindObject(nullptr, nullptr, (L"ShooterGame.GameObject"), false);
	}
}

namespace GameplayStatics
{
	UObject* GameplayStatics()
	{

		auto class_name = ez(L"Engine.Default__GameplayStatics").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}
	void GetAllActorsOfClass2(UWorld* WorldContextObject, UObject* ActorClass, TArray<AGameObject*>* OutActors)
	{

		UObject* Function = UObject::find_object(ez(L"GameplayStatics.GetAllActorsOfClass"));

		struct
		{
			const class UWorld* WorldContextObject;
			UObject* ActorClass;
			TArray<AGameObject*>                         OutActors;

		}params;

		params.WorldContextObject = WorldContextObject;
		params.ActorClass = ActorClass;

		Function->ProcessEvent(GameplayStatics(), Function, &params);

		if (OutActors != nullptr)
			*OutActors = params.OutActors;
	}
	TArray<AGameObject*> GetAllActorsOfClass(UWorld* WorldContext, UObject* Class)
	{
		auto function_name = (L"Engine.GameplayStatics.GetAllActorsOfClass");
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			UWorld* World;
			UObject* Class;
			TArray<AGameObject*> Out;
		} Args;

		Args.World = WorldContext;
		Args.Class = Class;
		Function->ProcessEvent(GameplayStatics(), Function, &Args);
		return Args.Out;
	}

	static inline UObject* get_game_instance(UWorld* world_context)
	{

		auto function_name = ez(L"Engine.GameplayStatics.GetGameInstance").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			UWorld* world_context;
			UObject* return_value;
		} params = { world_context };

		Function->ProcessEvent(GameplayStatics(), &params);
		return params.return_value;
	}
}

namespace KismetMathLibrary
{
	UObject* KismetMathLibrary()
	{

		auto class_name = ez(L"Engine.Default__KismetMathLibrary").d();
		return UObject::StaticFindObject(nullptr, nullptr, class_name, false);
	}

	FVector FindLookAtRotation(FVector start, FVector target)
	{

		auto function_name = ez(L"Engine.KismetMathLibrary.FindLookAtRotation").d();
		static UObject* Function = nullptr;

		if (!Function)
			Function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		struct {
			FVector start;
			FVector target;
			FVector out;
		} Args;

		Args.start = start;
		Args.target = target;
		Function->ProcessEvent(KismetMathLibrary(), Function, &Args);
		return Args.out;
	}
	FVector GetForwardVector(FRotator inRot)
	{
		static UObject* function = nullptr;

		if (!function)
		{
			// You had this right in your image
			function = UObject::StaticFindObject(nullptr, nullptr, L"Engine.KismetMathLibrary.GetForwardVector", false);
		}

		struct
		{
			FRotator InRot;
			FVector ReturnValue;
		} params;

		params.InRot = inRot;

		
		function->ProcessEvent(KismetMathLibrary(), function, &params);
			return params.ReturnValue;
		

		return FVector(0, 0, 0);
	}

	double distance_2d(FVector2D v1, FVector2D v2)
	{

		auto function_name = ez(L"Engine.KismetMathLibrary.Distance2D").d();
		UObject* function = UObject::StaticFindObject(nullptr, nullptr, function_name, false);

		if (!function)
			return 0;

		struct
		{
			FVector2D v1;
			FVector2D v2;
			double output;
		} params;

		params.v1 = v1;
		params.v2 = v2;

		function->ProcessEvent(KismetMathLibrary(), function, &params);

		return params.output;
	}
}
struct inventory_manager
{

};
/*class currentequippable : public UObject
{
public:
	struct type {
		const wchar_t* search = nullptr;
		const char* config = nullptr;
		std::int32_t index = 0;

		const bool is_valid() const noexcept {
			return this->search != nullptr || this->config != nullptr;
		}
	};





	static inline currentequippable::type types[] = {
			{ L"basepistol", "classic", 0 },
			{ L"sawedoffshotgun", "shorty", 0 },
			{ L"automaticpistol", "frenzy", 0 },
			{ L"lugerpistol", "ghost", 0 },
			{ L"revolverpistol", "sheriff", 0 },

			{ L"vector", "stinger", 0 },
			{ L"submachinegun_mp5", "spectre", 0 },

			{ L"pumpshotgun", "bucky", 0 },
			{ L"automaticshotgun", "judge", 0 },

			{ L"assaultrifle_burst", "bulldog", 0 },
			{ L"dmr", "guardian", 0 },
			{ L"assaultrifle_acr", "phantom", 0 },
			{ L"assaultrifle_ak", "vandal", 0 },

			{ L"leversniperrifle", "marshal", 0 },
			{ L"boltsniper", "operator", 0 },

			{ L"lightmachinegun", "ares", 0 },
			{ L"heavymachinegun", "odin", 0 },

			{ L"ability_melee_base", "knife", 0 },

			{ L"gun_sprinter_x_heavylightninggun_production", "neon_x", 0 },
			{ L"ability_wushu_x_dagger_production", "jett_dagger", 0 }
	};

	currentequippable::type get_type() {
		const FString& name = system::get_object_name(this);

		for (std::int32_t index = 0; index < sizeof(currentequippable::types) / sizeof(currentequippable::type); index++) {
			currentequippable::type type = currentequippable::types[index];

			if (type.index == 0)
				type.index = index;

			if (system::starts_with(name.c_str(), type.search))
				return { type.search, type.config, type.index };
		}

		return {};
	}




	equippable_skin_data_asset* get_skin_data_asset();
};*/
class UEquippableSkinDataAsset : public UObject
{
public:

	struct type {
		const wchar_t* search = nullptr;
		const char* config = nullptr;

		std::int32_t index = 0;

		const bool is_valid() const noexcept {
			return this->search != nullptr || this->config != nullptr;
		}
	};

	static inline UEquippableSkinDataAsset::type types[] = {
		{ L"default__basepistol", "classic", 0 },
		{ L"default__sawedoffshotgun", "shorty", 0 },
		{ L"default__autopistol", "frenzy", 0 },
		{ L"default__luger", "ghost", 0 },
		{ L"default__revolver", "sheriff", 0 },

		{ L"default__vector", "stinger", 0 },
		{ L"default__mp5", "spectre", 0 }, // 6

		{ L"default__pumpshotgun", "bucky", 0 },
		{ L"default__automaticshotgun", "judge", 0 },

		{ L"default__burst", "bulldog", 0 },
		{ L"default__dmr", "guardian", 0 },
		{ L"default__assaultrifle_acr", "phantom", 0 }, // 11
		{ L"default__ak", "vandal", 0 },

		{ L"default__leversniperrifle", "marshal", 0 },
		{ L"default__boltsniper", "operator", 0 },

		{ L"default__lmg", "ares", 0 },
		{ L"default__heavymachinegun", "odin", 0 }, // 16

		{ L"default__melee", "knife", 0 },

		{ L"default__carbine", "phantom", 11 },
		{ L"default__automaticpistol", "frenzy", 2 },
		{ L"default__subMachinegun_mp5", "spectre", 6 },
		{ L"Default__Slim", "shorty", 1 },
		{ L"Default__hmg", "odin", 16 },
		{ L"Default__lightmachinegun", "ares", 15 },
		{ L"Default__assaultrifle_ak", "vandal", 12 },
		{ L"Default__AssaultRifle_Burst", "bulldog", 9 },
		{ L"Default__AutoShotgun", "judge", 8 },
		{ L"Default__LeverSniper", "marshal", 13 },
	};

	UEquippableSkinDataAsset::type GetType() {
		const FString& name = kismentsystemlibrary::get_object_name(this);

		for (std::int32_t index = 0; index < sizeof(UEquippableSkinDataAsset::types) / sizeof(UEquippableSkinDataAsset::type); index++) {
			UEquippableSkinDataAsset::type type = UEquippableSkinDataAsset::types[index];

			if (type.index == 0)
				type.index = index;

			if (kismentsystemlibrary::contains(name.c_str(), type.search))
				return { type.search, type.config, type.index };
		}

		return {};
	}

	TArray<int> get_skin_levels() {
		return Memory::R<TArray<int>>(std::uintptr_t(this) + 0x88);
	}
};
struct skin_data_asset : UObject
{
	struct type {

		const wchar_t* search = nullptr;
		const char* config = nullptr;
		std::int32_t index = 0;

		const bool is_valid() const noexcept {
			return this->search != nullptr || this->config != nullptr;
		}
	};
	static inline skin_data_asset::type types[] = {
		{ L"default__basepistol", "classic", 0 },
		{ L"default__sawedoffshotgun", "shorty", 0 },
		{ L"default__autopistol", "frenzy", 0 },
		{ L"default__luger", "ghost", 0 },
		{ L"default__revolver", "sheriff", 0 },

		{ L"default__vector", "stinger", 0 },
		{ L"default__mp5", "spectre", 0 }, // 6

		{ L"default__pumpshotgun", "bucky", 0 },
		{ L"default__automaticshotgun", "judge", 0 },

		{ L"default__burst", "bulldog", 0 },
		{ L"default__dmr", "guardian", 0 },
		{ L"default__assaultrifle_acr", "phantom", 0 }, // 11
		{ L"default__ak", "vandal", 0 },

		{ L"default__leversniperrifle", "marshal", 0 },
		{ L"default__boltsniper", "operator", 0 },

		{ L"default__lmg", "ares", 0 },
		{ L"default__heavymachinegun", "odin", 0 }, // 16

		{ L"default__melee", "knife", 0 },

		{ L"default__carbine", "phantom", 11 },
		{ L"default__automaticpistol", "frenzy", 2 },
		{ L"default__subMachinegun_mp5", "spectre", 6 },
		{ L"Default__Slim", "shorty", 1 },
		{ L"Default__hmg", "odin", 16 },
		{ L"Default__lightmachinegun", "ares", 15 },
		{ L"Default__assaultrifle_ak", "vandal", 12 },
		{ L"Default__AssaultRifle_Burst", "bulldog", 9 },
		{ L"Default__AutoShotgun", "judge", 8 },
		{ L"Default__LeverSniper", "marshal", 13 },
	};

	skin_data_asset::type GetType() {
		const FString& name = kismentsystemlibrary::get_object_name(this);

		for (std::int32_t index = 0; index < sizeof(skin_data_asset::types) / sizeof(skin_data_asset::type); index++) {
			skin_data_asset::type type = skin_data_asset::types[index];

			if (type.index == 0)
				type.index = index;

			if (kismentsystemlibrary::contains(name.c_str(), type.search))
				return { type.search, type.config, type.index };
		}

		return {};
	}
	int32_t get_type() {
		FString name = kismentsystemlibrary::get_object_name(this);

		if (kismentsystemlibrary::contains2(name, L"Default__BasePistol", true, false)) {
			return 1;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__SawedOffShotgun", true, false) || kismentsystemlibrary::contains2(name, L"Default__Slim", true, false)) {
			return 2;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AutoPistol", true, false) || kismentsystemlibrary::contains2(name, L"Default__AutomaticPistol", true, false)) {
			return 3;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Luger", true, false)) {
			return 4;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Revolver", true, false)) {
			return 5;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Vector", true, false)) {
			return 6;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__MP5", true, false) || kismentsystemlibrary::contains2(name, L"Default__SubMachineGun_MP5", true, false)) {
			return 7;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__PumpShotgun", true, false)) {
			return 8;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AutomaticShotgun", true, false) || kismentsystemlibrary::contains2(name, L"Default__AutoShotgun", true, false)) {
			return 9;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Burst", true, false) || kismentsystemlibrary::contains2(name, L"Default__AssaultRifle_Burst", true, false)) {
			return 10;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__DMR", true, false)) {
			return 11;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AssaultRifle_ACR", true, false) || kismentsystemlibrary::contains2(name, L"Default__Carbine", true, false)) {
			return 12;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AK", true, false) || kismentsystemlibrary::contains2(name, L"Default__AssaultRifle_AK", true, false)) {
			return 13;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__LeverSniperRifle", true, false) || kismentsystemlibrary::contains2(name, L"Default__LeverSniper", true, false)) {
			return 14;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__BoltSniper", true, false)) {
			return 15;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__LMG", true, false) || kismentsystemlibrary::contains2(name, L"Default__LightMachineGun", true, false)) {
			return 16;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__HMG", true, false) || kismentsystemlibrary::contains2(name, L"Default__HeavyMachineGun", true, false)) {
			return 17;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Melee", true, false)) {
			return 18;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__DS_Gun", true, false)) {
			return 19;
		}
		return 0;
	}

	TArray<int> get_levels()
	{

		return Memory::R<TArray<int>>(std::uintptr_t(this) + 0x88);
	}
};

struct invetory_chroma_asset
{
	UObject* get_skin_chroma_asset()
	{

		return Memory::R<UObject*>(std::uintptr_t(this) + 0x108);
	}
};

struct skin_inventory_model
{
	skin_data_asset* get_skin_data_asset()
	{

		return Memory::R<skin_data_asset*>(std::uintptr_t(this) + 0x108);
	}
	invetory_chroma_asset* get_skin_invetory_chroma_asset()
	{

		static UObject* function;
		if (!function)
			function = UObject::find_object2<UObject*>(ez(L"ShooterGame.EquippableSkinInventoryModel.GetEquippedChroma").d());

		struct {
			invetory_chroma_asset* return_value;
		} params;

		function->ProcessEvent(this, function, &params);
		return params.return_value;
	}
};

struct invetory_model
{
	skin_inventory_model* get_equipped_skin()
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.EquippableInventoryModel.GetEquippedSkin", false);
		struct {
			skin_inventory_model* out;
		}Args;
		Function->ProcessEvent(this, Function, &Args);
		return Args.out;
	}
};

struct arsenal_view_model
{
	TArray<invetory_model*> get_gun_models()
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.ArsenalViewModel.GetGunModels", false);
		struct {
			TArray<invetory_model*> out;
		}Args;
		Function->ProcessEvent(this, Function, &Args);
		return Args.out;
	}
};

struct arsenal_view_controller
{
	arsenal_view_model* get_arsenal_view_model()
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.ArsenalViewController.GetViewModel", false);
		struct {
			arsenal_view_model* out;
		}Args;
		Function->ProcessEvent(this, Function, &Args);
		return Args.out;
	}
};

struct client_gameinstance
{
	inventory_manager* get_inventory_manager()
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.AresClientGameInstance.GetInventoryManager", false);
		struct {
			inventory_manager* out;
		}Args;
		Function->ProcessEvent(this, Function, &Args);
		return Args.out;
	}
	arsenal_view_controller* get_arsenal_view_controller()
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.AresClientGameInstance.GetArsenalViewController", false);
		struct {
			arsenal_view_controller* out;
		}Args;
		Function->ProcessEvent(this, Function, &Args);
		return Args.out;
	}
	TArray<UObject*> get_all_spray_models()
	{

		static UObject* function = nullptr;
		if (!function)
			function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.InventoryManager.GetAllSprayModels", false);

		struct {
			TArray<UObject*> return_value;
		} params = { };

		function->ProcessEvent(this, function, &params);

		return params.return_value;
	}
};


namespace ares_instance
{
	UObject* AresIntanceStatic()
	{

		return UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.Default__AresClientGameInstance", false);
	}

	client_gameinstance* get_ares_client_game_instance(UWorld* WORLD)
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.AresClientGameInstance.GetAresClientGameInstance", false);
		struct {
			UWorld* World;
			client_gameinstance* Class;
		}Args;
		Args.World = WORLD;
		Function->ProcessEvent(AresIntanceStatic(), Function, &Args);
		return Args.Class;
	}
}
namespace ContentLibrary
{
	UObject* ContentLibrary()
	{

		return UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.Default__ContentLibrary", false);
	}

	void ApplySkin(AAresEquippable* Equippable, UObject* SkinAsset, UObject* ChromaAsset, std::int32_t Level, UObject* CharmData, std::int32_t BuddieLevel)
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.ContentLibrary.ApplySkin", false);
		struct {
			AAresEquippable* eqp;
			UObject* skinasset;
			UObject* chromaasset;
			std::int32_t level;
			UObject* charmdata;
			std::int32_t buddielevel;
		}Args;
		Args.eqp = Equippable;
		Args.skinasset = SkinAsset;
		Args.chromaasset = ChromaAsset;
		Args.level = Level;
		Args.charmdata = CharmData;
		Args.buddielevel = BuddieLevel;
		Function->ProcessEvent(ContentLibrary(), Function, &Args);
	}
	void ClearWeaponComponents(AAresEquippable* Equippable)
	{

		UObject* Function = UObject::StaticFindObject(nullptr, nullptr, L"ShooterGame.ContentLibrary.ClearWeaponComponents", false);
		struct {
			AAresEquippable* Equippable;
		}Args;
		Args.Equippable = Equippable;
		Function->ProcessEvent(ContentLibrary(), Function, &Args);
	}
}
class system : public UObject
{
public:
	static FString get_object_name(UObject* object);
	static bool contains_string(const wchar_t* search_in, const wchar_t* sub_string, bool use_case = false, bool search_from_end = false);
	static bool starts_with(const wchar_t* source, const wchar_t* prefix, const enum_as_byte<search_case>& search_case = ignore_case);
	UObject* get_outer_object(UObject* object);

	/*static inline __int64 decrypt_parent(UObject* parent) {

		static __int64(__fastcall * decrypt_function)(__int64);
		if (!decrypt_function) {
			decrypt_function = reinterpret_cast<decltype(decrypt_function)>(Offsets::skin_decrypt + VALORANT::Module);
		}

		if (!decrypt_function) return 0x0;

		return decrypt_function((__int64)parent);
	}*/
};

class equippable_skin_data_asset : public UObject
{
public:
	/*int32_t get_type() {
		FString name = kismentsystemlibrary::get_object_name(this);

		if (kismentsystemlibrary::contains2(name, L"Default__BasePistol", true, false)) {
			return 1;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__SawedOffShotgun", true, false) || kismentsystemlibrary::contains2(name, L"Default__Slim", true, false)) {
			return 2;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AutoPistol", true, false) || kismentsystemlibrary::contains2(name, L"Default__AutomaticPistol", true, false)) {
			return 3;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Luger", true, false)) {
			return 4;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Revolver", true, false)) {
			return 5;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Vector", true, false)) {
			return 6;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__MP5", true, false) || kismentsystemlibrary::contains2(name, L"Default__SubMachineGun_MP5", true, false)) {
			return 7;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__PumpShotgun", true, false)) {
			return 8;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AutomaticShotgun", true, false) || kismentsystemlibrary::contains2(name, L"Default__AutoShotgun", true, false)) {
			return 9;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Burst", true, false) || kismentsystemlibrary::contains2(name, L"Default__AssaultRifle_Burst", true, false)) {
			return 10;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__DMR", true, false)) {
			return 11;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AssaultRifle_ACR", true, false) || kismentsystemlibrary::contains2(name, L"Default__Carbine", true, false)) {
			return 12;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__AK", true, false) || kismentsystemlibrary::contains2(name, L"Default__AssaultRifle_AK", true, false)) {
			return 13;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__LeverSniperRifle", true, false) || kismentsystemlibrary::contains2(name, L"Default__LeverSniper", true, false)) {
			return 14;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__BoltSniper", true, false)) {
			return 15;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__LMG", true, false) || kismentsystemlibrary::contains2(name, L"Default__LightMachineGun", true, false)) {
			return 16;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__HMG", true, false) || kismentsystemlibrary::contains2(name, L"Default__HeavyMachineGun", true, false)) {
			return 17;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__Melee", true, false)) {
			return 18;
		}
		else if (kismentsystemlibrary::contains2(name, L"Default__DS_Gun", true, false)) {
			return 19;
		}
		return 0;
	}*/

	TArray<int> get_skin_levels();
};

namespace arsenal_view
{
	//invetory_model* get_aresnal_gun_models(int type_index)
	//{
	//	
	//	if (APlayerController* controller = ShooterGameBlueprints::GetFirstLocalPlayerController(UWorldSave))
	//		if (client_gameinstance* ares = ares_instance::get_ares_client_game_instance(UWorldSave))
	//			if (arsenal_view_controller* arsenal_view_controller = ares->get_arsenal_view_controller())
	//				if (arsenal_view_model* arsenal_view_model = arsenal_view_controller->get_arsenal_view_model())
	//				{
	//					auto models = arsenal_view_model->get_gun_models();
	//					for (int i = 0; i < models.Num(); i++)
	//					{
	//						auto model = models[i];
	//						if (!model)
	//							continue;
	//						if (skin_data_asset* skin_data = model->get_equipped_skin()->get_skin_data_asset())
	//						{
	//							if (int type = skin_data->get_type())
	//							{
	//								if (type == type_index)
	//								{
	//									return model;
	//								}
	//							}
	//						}
	//					}
	//				}
	//	return nullptr;
	//}
}


//void unlock_all_apply2(AAresEquippable* Equippable, UObject* skin_data_asset, UObject* skin_chroma_asset, int max_level,UWorld* world) {
//	inventory_manager* manager = ares_instance::get_ares_client_game_instance(world)->get_inventory_manager();

//	auto charm_instances = Memory::R<TArray<tmap<UObject*, UObject*>>>(std::uintptr_t(manager) + 0x430);
//	buddy = charm_instances[index].Key;
//	auto ares_client_game_instance = ares_instance::get_ares_client_game_instance(world);
//	auto v1 = *(uintptr_t*)(ares_client_game_instance + 0x398);
//	auto v2 = *(uintptr_t*)(v1 + 0xB0);
//	*(BYTE*)(v2 + 0x80) = 2;
//	ContentLibrary::ClearWeaponComponents(Equippable);
//	ContentLibrary::ApplySkin(Equippable, skin_data_asset, skin_chroma_asset, max_level, nullptr, -1);
//	*(BYTE*)(v2 + 0x80) = 6;
//}

//namespace skin_changer_utils
//{

//	invetory_model* get_aresnal_gun_models(UWorld* World, int type_index)
//	{
//		
//		if (client_gameinstance* ares = ares_instance::get_ares_client_game_instance(World))
//		{
//			
//			if (arsenal_view_controller* arsenal_view_controller = ares->get_arsenal_view_controller())
//			{
//				
//				if (arsenal_view_model* arsenal_view_model = arsenal_view_controller->get_arsenal_view_model())
//				{
//					
//					auto models = arsenal_view_model->get_gun_models();
//					for (int i = 0; i < models.size(); i++)
//					{

//						auto model = models[i];

//						if (skin_data_asset* skin_data = model->get_equipped_skin()->get_skin_data_asset())
//						{
//							if (skin_data_asset::type type = skin_data->GetType(); type.is_valid())
//							{
//								if (type.index == type_index)
//								{
//									return model;
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//		return nullptr;
//	}
//}

//void skin_changer(UWorld* World, APlayerController* controller)
//{
//	
//	if (AShooterCharacter* pawn = controller->GetShooterCharacter())
//	{
//		
//		if (UAresInventory* inventory = pawn->GetInventory())
//		{
//			
//			if (AAresEquippable* equippable = inventory->GetCurrentWeapon())
//			{
//				
//				if (AAresEquippable::type type = equippable->GetType(); type.is_valid())
//				{
//					
//					if (skin_data_asset* current_real_skin = equippable->GetSkinDataAsset())
//					{
//						
//						if (invetory_model* model = skin_changer_utils::get_aresnal_gun_models(World, type.index))
//						{

//							
//							int max_level = model->get_equipped_skin()->get_skin_data_asset()->get_levels().size();
//							UObject* skin_data_asset = model->get_equipped_skin()->get_skin_data_asset();
//							UObject* skin_chroma_asset = model->get_equipped_skin()->get_skin_invetory_chroma_asset()->get_skin_chroma_asset();

//							auto re_ = Memory::R<UObject*>(std::uintptr_t(equippable) + 0xEB8);
//							auto name = kismentsystemlibrary::get_object_name(re_);
//							if (!skin_data_asset || !skin_chroma_asset || (UObject*)skin_data_asset == current_real_skin) {
//								return;
//							}

//							Memory::W(std::uintptr_t(equippable) + (0xEB8), skin_data_asset);
//							Memory::W(std::uintptr_t(equippable) + (0xEE0), skin_chroma_asset);

//							auto ares_client_game_instance = ares_instance::get_ares_client_game_instance(World);
//							auto v1 = *(uintptr_t*)(ares_client_game_instance + 0x398);
//							auto v2 = *(uintptr_t*)(v1 + 0xB0);
//							*(BYTE*)(v2 + 0x80) = 2;
//							ContentLibrary::ClearWeaponComponents(equippable);
//							ContentLibrary::ApplySkin(equippable, skin_data_asset, skin_chroma_asset, max_level, nullptr, -1);
//							*(BYTE*)(v2 + 0x80) = 6;
//						}
//					}
//				}
//			}
//		}
//	}
//}

//void unlock_all_skins(UWorld* World) {
//	if (client_gameinstance* ares = ares_instance::get_ares_client_game_instance(World))
//	{
//		
//		if (inventory_manager* manager = ares->get_inventory_manager()) {
//			
//			TArray<UObject*> equippable_models = Memory::R<TArray<UObject*>>(std::uintptr_t(manager) + 0xe0);

//			for (int i = 0; i < equippable_models.size(); i++) {
//				
//				if (auto model = equippable_models[i]) {
//					
//					TArray<UObject*> skins = Memory::R<TArray<UObject*>>(std::uintptr_t(model) + (0x150));

//					if (skins.size() > 0) {
//						
//						for (int a = 0; a < skins.size(); a++) {
//							
//							if (auto skin = skins[a]) {
//								
//								TArray<UObject*> possible_levels = Memory::R<TArray<UObject*>>(std::uintptr_t(skin) + (0x138));
//								for (int a = 0; a < possible_levels.size(); a++)
//									if (UObject* xyz = possible_levels[a])
//										Memory::W<bool>(std::uintptr_t(xyz) + (0xfe), true),
//										Memory::W<bool>(std::uintptr_t(xyz) + (0xf2), true);


//								TArray<UObject*> possible_chromas = Memory::R<TArray<UObject*>>(std::uintptr_t(skin) + (0x128)); // ?
//								for (int a = 0; a < possible_chromas.size(); a++)
//									if (UObject* xyz = possible_chromas[a])
//										Memory::W<bool>(std::uintptr_t(xyz) + (0xfe), true),
//										Memory::W<bool>(std::uintptr_t(xyz) + (0xf2), true);


//								Memory::W<bool>(std::uintptr_t(skin) + (0xfe), true);
//								Memory::W<bool>(std::uintptr_t(skin) + (0xf3), true);
//								Memory::W<bool>(std::uintptr_t(skin) + (0xf2), true);
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//}
template<class k, class e>
class tmap
{
public:
	k Key;
	e Element;
	char __pad0x[0x8];
};
FString BuddyName;
UObject* buddy;
namespace Skin
{
	void ChangeSkin(AAresEquippable* Equippable, UObject* SkinAsset, UObject* ChromaAsset, std::int32_t Level, UWorld* world)
	{




		inventory_manager* manager = ares_instance::get_ares_client_game_instance(world)->get_inventory_manager();

		auto charm_instances = Memory::R<TArray<tmap<UObject*, UObject*>>>(std::uintptr_t(manager) + 0x430);
		buddy = charm_instances[index].Key;
		auto ares_client_game_instance = ares_instance::get_ares_client_game_instance(world);
		auto v1 = *(uintptr_t*)(ares_client_game_instance + 0x3a8);
		auto v2 = *(uintptr_t*)(v1 + 0xB0);
		*(BYTE*)(v2 + 0x80) = 2;




		ContentLibrary::ClearWeaponComponents(Equippable);
		ContentLibrary::ApplySkin(Equippable, SkinAsset, ChromaAsset, Level, nullptr, -1);
		*(BYTE*)(v2 + 0x80) = 6;





	}
	void ChangeSkinbuddy(AAresEquippable* Equippable, UObject* SkinAsset, UObject* ChromaAsset, std::int32_t Level, UWorld* world)
	{




		inventory_manager* manager = ares_instance::get_ares_client_game_instance(world)->get_inventory_manager();

		auto charm_instances = Memory::R<TArray<tmap<UObject*, UObject*>>>(std::uintptr_t(manager) + 0x430);
		buddy = charm_instances[index].Key;
		auto ares_client_game_instance = ares_instance::get_ares_client_game_instance(world);
		auto v1 = *(uintptr_t*)(ares_client_game_instance + 0x3a8);
		auto v2 = *(uintptr_t*)(v1 + 0xB0);
		*(BYTE*)(v2 + 0x80) = 2;


		ContentLibrary::ClearWeaponComponents(Equippable);
		ContentLibrary::ApplySkin(Equippable, SkinAsset, ChromaAsset, Level, buddy, 1);
		*(BYTE*)(v2 + 0x80) = 6;






	}
}
float DegreeToRadian(float degrees) {
	return degrees * (PI / 180);
}
void DrawTextRGB(UCanvas* canvas, const wchar_t* text, float X, float Y, FLinearColor color, bool CenterX = 0)
{
	canvas->K2_DrawText(text, { X, Y }, { 1.1, 1.1 }, color, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, CenterX, 0, bOutline, { 0, 0, 0, 1 });
}
void DrawTextSimple(UCanvas* canvas, const std::wstring& text, float X, float Y, FLinearColor color, bool CenterX = 0)
{
	canvas->K2_DrawText(text.c_str(), { X, Y }, { 1.1, 1.1 }, color, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, CenterX, 0, bOutline, { 0, 0, 0, 1 });
}
void DrawTextRGBWithFString(UCanvas* canvas, FString text, float X, float Y, FLinearColor color, bool CenterX = 0)
{
	canvas->K2_DrawText(text, { X, Y }, { 1.1, 1.1 }, color, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, CenterX, 0, bOutline, { 0, 0, 0, 1 });
}
void xxDrawTextRGB(UCanvas* canvas, FString text, float X, float Y, FLinearColor color)
{
	canvas->K2_DrawText(text, { X, Y }, { 1.1, 1.1 }, color, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, 0, 0, false, { 0, 0, 0, 1 });
}

__forceinline void Clamp2(FVector& Ang) {
	if (Ang.X < 0.f)
		Ang.X += 360.f;

	if (Ang.X > 360.f)
		Ang.X -= 360.f;

	if (Ang.Y < 0.f) Ang.Y += 360.f;
	if (Ang.Y > 360.f) Ang.Y -= 360.f;
	Ang.Z = 0.f;
}
void normalize2(FVector& in) {
	if (in.X > 89.f) in.X -= 360.f;
	else if (in.X < -89.f) in.X += 360.f;

	while (in.Y > 180)in.Y -= 360;
	while (in.Y < -180)in.Y += 360;
	in.Z = 0;
}

FVector smooth_aim2(FVector target, FVector delta_rotation, float smooth) {
	FVector diff = target - delta_rotation;
	normalize2(diff);
	return delta_rotation + diff / smooth;
}
struct FPostProcessSettings
{
	// SceneColorTint
	bool bOverride_SceneColorTint = false;
	FLinearColor SceneColorTint = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Auto Exposure
	bool bOverride_AutoExposureMinBrightness = false;
	float AutoExposureMinBrightness = 1.0f;

	bool bOverride_AutoExposureMaxBrightness = false;
	float AutoExposureMaxBrightness = 1.0f;

	// Film
	bool bOverride_FilmContrast = false;
	float FilmContrast = 1.0f;

	bool bOverride_FilmSaturation = false;
	float FilmSaturation = 1.0f;

	// Bloom
	bool bOverride_BloomIntensity = false;
	float BloomIntensity = 1.0f;

	// Grain
	bool bOverride_GrainIntensity = false;
	float GrainIntensity = 0.0f;

	// Vignette
	bool bOverride_VignetteIntensity = false;
	float VignetteIntensity = 0.4f;

	// Chromatic Aberration (Fringe)
	bool bOverride_SceneFringeIntensity = false;
	float SceneFringeIntensity = 0.0f;

	// Ambient Occlusion
	bool bOverride_AmbientOcclusionIntensity = false;
	float AmbientOcclusionIntensity = 1.0f;

	bool bOverride_AmbientOcclusionDistance = false;
	float AmbientOcclusionDistance = 150.0f;

	bool bOverride_AmbientOcclusionFadeDistance = false;
	float AmbientOcclusionFadeDistance = 500.0f;

	// Padding to roughly align with Unreal's original size (if needed)
	uint8 Padding[1440]; // adjust based on actual struct size (~1552 total)
};

struct FMinimalViewInfo {
	FVector Location;
	FVector Rotation;
	float FOV;                        // 0x30
	float DesiredFOV;                // 0x34
	float OrthoWidth;                // 0x38
	float OrthoNearClipPlane;       // 0x3C
	float OrthoFarClipPlane;        // 0x40
	float PerspectiveNearClipPlane; // 0x44
	float AspectRatio;              // 0x48
	uint8_t pad[0x54 - 0x4C];       // Padding to reach 0x54
	uint8_t bConstrainAspectRatio;  // 0x54 : 0
};


struct Bone {
	BYTE thing[0xCC];
	float X;
	BYTE thing2[0xC];
	float Y;
	BYTE thing3[0xC];
	float z;
};
boolean in_rect(float centerX, float centerY, float radius, float X, float Y) {
	return X >= centerX - radius && X <= centerX + radius &&
		Y >= centerY - radius && Y <= centerY + radius;
}
uintptr_t OffsetGetActorBounds;
FVector GetBoneMatrix(void* Mesh, int BoneIndex) {
	if (!Mesh || BoneIndex < 0)
		return FVector(0.f, 0.f, 0.f);

	FMatrix BoneMatrix{};

	// Define the original bone matrix function type
	using BoneMatrixFn = FMatrix * (__fastcall*)(void*, FMatrix*, int);

	static BoneMatrixFn fn = reinterpret_cast<BoneMatrixFn>(
		VALORANT::Module + Offsets::bone_matrix
		);

	if (!fn)
		return FVector(0.f, 0.f, 0.f);

	// Call the bone matrix function using spoofcall_stub
	reinterpret_cast<void(__fastcall*)(void*, FMatrix*, int, uintptr_t, void*)>(spoofcall_stub)(
		Mesh,
		&BoneMatrix,
		BoneIndex,
		Offsets::MagicOffsets,
		reinterpret_cast<void*>(fn)
		);

	// Return the position extracted from the matrix
	return FVector(BoneMatrix.WPlane.X, BoneMatrix.WPlane.Y, BoneMatrix.WPlane.Z);
}



FVector FindSkeleton(APlayerController* controller, void* mesh, int i)
{

	FVector2D drawtextat;
	return controller->ProjectWorldLocationToScreen(GetBoneMatrix(mesh, i), drawtextat, 0);
}

static FVector2D scr[8];
void FMath::SinCos(float* ScalarSin, float* ScalarCos, float Value)
{
	float quotient = (INV_PI * 0.5f) * Value;
	if (Value >= 0.0f)
	{
		quotient = (float)((int)(quotient + 0.5f));
	}
	else
	{
		quotient = (float)((int)(quotient - 0.5f));
	}
	float Y = Value - (2.0f * PI) * quotient;
	float sign;
	if (Y > HALF_PI)
	{
		Y = PI - Y;
		sign = -1.0f;
	}
	else if (Y < -HALF_PI)
	{
		Y = -PI - Y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = Y * Y;

	*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * Y;

	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*ScalarCos = sign * p;
}
float FMath::Fmod(float X, float Y)
{
	const float AbsY = fabsf(Y);
	if (AbsY <= 1.e-8f) { return 0.f; }
	const float Div = (X / Y);

	const float Quotient = fabsf(Div) < FLOAT_NON_FRACTIONAL ? truncf(Div) : Div;
	float IntPortion = Y * Quotient;


	if (fabsf(IntPortion) > fabsf(X)) { IntPortion = X; }

	const float Result = X - IntPortion;

	return FMath::Clamp(Result, -AbsY, AbsY);
}
float FMath::DegreesToRadians(float deg)
{
	return deg * (3.14159265358979323846f / 180.0f);
}


FQuat FRotator::Quaternion() const
{
	float SP, SY, SR;
	float CP, CY, CR;
	const float PitchNoWinding = FMath::Fmod(Pitch, 360.0f);
	const float YawNoWinding = FMath::Fmod(Yaw, 360.0f);
	const float RollNoWinding = FMath::Fmod(Roll, 360.0f);
	FMath::SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
	FMath::SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
	FMath::SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);
	FQuat RotationQuat;
	RotationQuat.X = CR * SP * SY - SR * CP * CY;
	RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
	RotationQuat.Z = CR * CP * SY - SR * SP * CY;
	RotationQuat.W = CR * CP * CY + SR * SP * SY;
	return RotationQuat;
}
const FVector FVector::ZeroVector(0.0f, 0.0f, 0.0f);
const FVector FVector::OneVector(1.0f, 1.0f, 1.0f);
FVector FTransform::TransformPosition(FVector& V) const
{
	return Rotation.RotateVector(Scale3D * V) + Translation;
}
FVector FQuat::RotateVector(const FVector& V) const
{
	const FVector Q(X, Y, Z);
	const FVector T = (Q ^ V) * 2.f;
	const FVector Result = V + (T * W) + (Q ^ T);
	return Result;
}
// HSV to RGB conversion in pure C++
FLinearColor HSVToRGB(float H, float S, float V)
{
	float R = 0.0f, G = 0.0f, B = 0.0f;

	int i = (int)(H * 6.0f);
	float f = H * 6.0f - i;
	float p = V * (1.0f - S);
	float q = V * (1.0f - f * S);
	float t = V * (1.0f - (1.0f - f) * S);

	switch (i % 6)
	{
	case 0: R = V; G = t; B = p; break;
	case 1: R = q; G = V; B = p; break;
	case 2: R = p; G = V; B = t; break;
	case 3: R = p; G = q; B = V; break;
	case 4: R = t; G = p; B = V; break;
	case 5: R = V; G = p; B = q; break;
	}

	return FLinearColor(R, G, B, 1.0f);
}
#include <chrono>

float GetStableTime()
{
	using namespace std::chrono;
	static auto start = steady_clock::now(); // doesn't change with system time
	auto now = steady_clock::now();
	duration<float> elapsed = now - start;
	return elapsed.count();
}

FLinearColor BOSSRAINBOMAW(float offset = 0.f)
{
	const float PI = 3.14159265359f;

	// 🐢 Slow it down by a factor (e.g., 0.5 = half speed, 0.1 = very slow)
	const float speed = 0.2f; // ⏱️ You can tweak this from 0.1 to 1.0
	float t = (GetStableTime() * speed) + offset;

	float R = 0.5f + 0.5f * sinf(t);
	float G = 0.5f + 0.5f * sinf(t + 2.0f * PI / 3.0f);
	float B = 0.5f + 0.5f * sinf(t + 4.0f * PI / 3.0f);
	return FLinearColor(R, G, B, 1.0f);
}


namespace NoSpread
{
	
	

			static FVector calc_spread(AShooterCharacter* actor, uint64_t firing_state_component, AAresEquippable* weapon, FVector direction)
			{



				if (!actor || !firing_state_component || !weapon)
					return FVector(0, 0, 0);

				// Vérification du composant de stabilité
				uint64_t stability_component = Memory::R<uint64_t>(firing_state_component + Offsets::stability_component);
				if (!stability_component)
					return FVector(0, 0, 0);

				static uint8_t error_values[4096];
				static uint8_t seed_data_snapshot[4096];
				static uint8_t spread_angles[4096];
				static uint8_t out_spread_angles[4096];

				auto func1_fn = (float* (__fastcall*)(uint64_t, float*))(VALORANT::Module + Offsets::get_spread_values);
				auto func2_fn = (void(__fastcall*)(uint64_t, FVector*, float, float, int, int, uint64_t))(VALORANT::Module + Offsets::get_spread_angles);
				auto func3_fn = (FVector * (__fastcall*)(FVector*, FVector*))(VALORANT::Module + Offsets::tovector_and_normalize);
				auto func4_fn = (FVector * (__fastcall*)(FVector*, FVector*))(VALORANT::Module + Offsets::toangle_and_normalize);
				auto func5_fn = (uintptr_t(__fastcall*)(__int64, float*))(VALORANT::Module + Offsets::get_spread_values);

				// Configuration des angles de sortie
				*(uint64_t*)(&out_spread_angles[0]) = (uint64_t)&spread_angles[0];
				*(int*)(&out_spread_angles[0] + 8) = 1;
				*(int*)(&out_spread_angles[0] + 12) = 1;

				// Copie des données de seed depuis la mémoire du jeu
				uint64_t seed_data = Memory::R<uint64_t>(firing_state_component + Offsets::seed_data);
				if (!seed_data)
					return FVector(0, 0, 0);

				memcpy((void*)seed_data_snapshot, (void*)seed_data, sizeof(seed_data_snapshot));

				// Obtenir le vecteur d'erreur avec spoofcall
				reinterpret_cast<float* (__cdecl*)(uint64_t, float*, uintptr_t, void*)>(
					spoofcall_stub)(stability_component, (float*)&error_values[0], Offsets::MagicOffsets, (void*)func1_fn);

				FVector temp1, temp2 = FVector(0, 0, 0);
				FVector previous_firing_direction, firing_direction = FVector(0, 0, 0);

				// Utiliser ta méthode existante
				actor->get_firing_location_and_direction(&temp1, &previous_firing_direction, false);

				// Normalisation et mathématiques vectorielles
				reinterpret_cast<FVector* (__cdecl*)(FVector*, FVector*, uintptr_t, void*)>(
					spoofcall_stub)(&previous_firing_direction, &temp2, Offsets::MagicOffsets, (void*)func3_fn);
				reinterpret_cast<FVector* (__cdecl*)(FVector*, FVector*, uintptr_t, void*)>(
					spoofcall_stub)(&temp2, &temp1, Offsets::MagicOffsets, (void*)func4_fn);

				previous_firing_direction = temp1;

				// Appliquer les Offsets horizontaux/verticaux depuis les données d'erreur
				temp1.X += *(float*)(&error_values[0] + 12);
				temp1.Y += *(float*)(&error_values[0] + 16);

				// Finaliser la direction de tir
				reinterpret_cast<FVector* (__cdecl*)(FVector*, FVector*, uintptr_t, void*)>(
					spoofcall_stub)(&temp1, &firing_direction, Offsets::MagicOffsets, (void*)func3_fn);

				// Facteurs de spread supplémentaires
				float test[20];
				uintptr_t v38 = reinterpret_cast<uintptr_t(__cdecl*)(__int64, float*, uintptr_t, void*)>(
					spoofcall_stub)(stability_component, test, Offsets::MagicOffsets, (void*)func5_fn);

				float v46 = Memory::R<float>(v38 + 0x14);
				float v48 = *(float*)(&error_values[0] + 8) + *(float*)(&error_values[0] + 4);
				int error_retries = Memory::R<int>(firing_state_component + Offsets::error_retries);

				// Appel final du calcul de spread
				reinterpret_cast<void(__cdecl*)(
					uint64_t, FVector*, float, float, int, int, uint64_t, uintptr_t, void*)>(
						spoofcall_stub)(
							((uint64_t)&seed_data_snapshot[0]) + Offsets::seed_addsomeshit, &firing_direction,
							v48, v46, error_retries, 1,
							(uint64_t)&out_spread_angles[0], Offsets::MagicOffsets, (void*)func2_fn);

				// Récupérer le vecteur de spread calculé
				FVector spread_vector = *(FVector*)(&spread_angles[0]);

				reinterpret_cast<FVector* (__cdecl*)(FVector*, FVector*, uintptr_t, void*)>(
					spoofcall_stub)(&spread_vector, &firing_direction, Offsets::MagicOffsets, (void*)func4_fn);

				return firing_direction - previous_firing_direction;
			}
	FVector calc_spread2(uint64_t actor, uint64_t firing_state_component) {
		auto get_spread_values_fn =
			(float* (__fastcall*)(uint64_t, float*))(VALORANT::Module + Offsets::get_spread_values);
		auto get_spread_angles_fn =
			(void(__fastcall*)(uint64_t, FVector*, float, float, int, int, uint64_t))(VALORANT::Module + Offsets::get_spread_values);
		auto get_firing_location_and_direction_fn =
			(void(__fastcall*)(uint64_t, FVector*, FVector*))(VALORANT::Module + Offsets::get_firing_location_direction);
		auto to_vector_and_normalize_fn =
			(FVector * (__fastcall*)(FVector*, FVector*))(VALORANT::Module + Offsets::tovector_and_normalize);
		auto to_angle_and_normalize_fn =
			(FVector * (__fastcall*)(FVector*, FVector*))(VALORANT::Module + Offsets::tovector_and_normalize);

		static uint8_t error_values[4096];
		static uint8_t seed_data_snapshot[4096];
		static uint8_t spread_angles[4096];
		static uint8_t out_spread_angles[4096];

		if (!actor || !firing_state_component)
			return FVector(0, 0, 0);

		memset(error_values, 0, sizeof(error_values));
		memset(seed_data_snapshot, 0, sizeof(seed_data_snapshot));
		memset(spread_angles, 0, sizeof(spread_angles));
		memset(out_spread_angles, 0, sizeof(out_spread_angles));

		*(uint64_t*)(&out_spread_angles[0]) = (uint64_t)&spread_angles[0];
		*(int*)(&out_spread_angles[0] + 8) = 1;
		*(int*)(&out_spread_angles[0] + 12) = 1;

		uint64_t seed_data = *(uint64_t*)(firing_state_component + Offsets::seed_data);
		memcpy((void*)seed_data_snapshot, (void*)seed_data, sizeof(seed_data_snapshot)); //Make our own copy since we don't want to desync our own seed component

		uint64_t stability_component = *(uint64_t*)(firing_state_component + Offsets::stability_component);
		if (stability_component)
			get_spread_values_fn(stability_component, (float*)&error_values[0]);

		FVector temp1, temp2 = FVector(0, 0, 0);
		FVector previous_firing_direction, firing_direction = FVector(0, 0, 0);
		get_firing_location_and_direction_fn(actor, &temp1, &previous_firing_direction);
		to_vector_and_normalize_fn(&previous_firing_direction, &temp2);
		to_angle_and_normalize_fn(&temp2, &temp1);
		previous_firing_direction = temp1;
		temp1.X += *(float*)(&error_values[0] + 12); //Your recoil angle
		temp1.Y += *(float*)(&error_values[0] + 16);
		to_vector_and_normalize_fn(&temp1, &firing_direction);

		float error_degrees = *(float*)(&error_values[0] + 8) + *(float*)(&error_values[0] + 4);
		float error_power = *(float*)(firing_state_component + Offsets::error_power); // OK
		int error_retries = *(int*)(firing_state_component + Offsets::error_retries); // OK
		get_spread_angles_fn(((uint64_t)&seed_data_snapshot[0]) + 0xE8, &firing_direction, error_degrees, error_power, error_retries, 1, (uint64_t)&out_spread_angles[0]);

		FVector spread_vector = *(FVector*)(&spread_angles[0]);
		to_angle_and_normalize_fn(&spread_vector, &firing_direction);

		return firing_direction - previous_firing_direction;
	}













	inline FVector GetErrorAngle(uint64_t actor, uint64_t firing_state_component) {
		static auto get_spread_values_fn = (float* (__fastcall*)(uint64_t, float*))(VALORANT::Module + Offsets::get_spread_values);
		static auto get_spread_angles_fn = (void(__fastcall*)(uint64_t, FVector*, float, float, int, int, uint64_t))(VALORANT::Module + Offsets::get_spread_angles);
		//static autog
		static auto get_firing_location_and_direction_fn = (void(__fastcall*)(uint64_t, FVector*, FVector*))(VALORANT::Module + Offsets::get_firing_location_direction);
		static auto to_vector_and_normalize_fn = (FVector * (__fastcall*)(FVector*, FVector*))(VALORANT::Module + Offsets::tovector_and_normalize);
		static auto to_angle_and_normalize_fn = (FVector * (__fastcall*)(FVector*, FVector*))(VALORANT::Module + Offsets::toangle_and_normalize);
		if (get_spread_values_fn && get_spread_angles_fn && get_firing_location_and_direction_fn && to_vector_and_normalize_fn && to_angle_and_normalize_fn)
		{

			static uint8_t error_values[4096] = { 0 };
			static uint8_t seed_data_snapshot[4096] = { 0 };
			static uint8_t spread_angles[4096] = { 0 };
			static uint8_t out_spread_angles[4096] = { 0 };

			if (!actor || !firing_state_component)
				return NULL;

			memset(error_values, 0, sizeof(error_values));
			memset(seed_data_snapshot, 0, sizeof(seed_data_snapshot));
			memset(spread_angles, 0, sizeof(spread_angles));
			memset(out_spread_angles, 0, sizeof(out_spread_angles));

			*(uint64_t*)(&out_spread_angles[0]) = (uint64_t)&spread_angles[0];
			*(int*)(&out_spread_angles[0] + 8) = 1;
			*(int*)(&out_spread_angles[0] + 12) = 1;

			uint64_t seed_data = Memory::R<uint64_t>(firing_state_component + Offsets::seed_data);
			if (!seed_data || seed_data > 9999999999999 || seed_data < 1000000000000) {
				return NULL;
			}

			memcpy((void*)seed_data_snapshot, (void*)seed_data, sizeof(seed_data_snapshot));

			uint64_t stability_component = Memory::R<uint64_t>(firing_state_component + Offsets::stability_component);
			if (stability_component)
				get_spread_values_fn(stability_component, (float*)&error_values[0]);

			FVector temp1, temp2 = FVector(0, 0, 0);
			FVector previous_firing_direction, firing_direction = FVector(0, 0, 0);
			get_firing_location_and_direction_fn(actor, &temp1, &previous_firing_direction);
			to_vector_and_normalize_fn(&previous_firing_direction, &temp2);
			to_angle_and_normalize_fn(&temp2, &temp1);
			previous_firing_direction = temp1;
			temp1.X += *(float*)(&error_values[0] + 12);
			temp1.Y += *(float*)(&error_values[0] + 16);
			to_vector_and_normalize_fn(&temp1, &firing_direction);

			float error_degrees = *(float*)(&error_values[0] + 8) + *(float*)(&error_values[0] + 4);
			if (firing_state_component != -1008 && firing_state_component != -1012)
			{
				float error_power = *(float*)(firing_state_component + Offsets::error_power);
				int error_retries = *(int*)(firing_state_component + Offsets::error_retries);
				if (error_degrees == 0) {
					return NULL;
				}
				get_spread_angles_fn(((uint64_t)&seed_data_snapshot[0]) + 0xE8, &firing_direction, error_degrees, error_power, error_retries, 1, (uint64_t)&out_spread_angles[0]);

				FVector spread_vector = *(FVector*)(&spread_angles[0]);
				to_angle_and_normalize_fn(&spread_vector, &firing_direction);

				FVector spread_out = firing_direction - previous_firing_direction;
				if (spread_out.X > 10000 || spread_out.Y > 10000 || spread_out.Z > 10000)
					return NULL;
				return spread_out;
			}
			else
			{
				return NULL;
			}

		}
		return NULL;
	}



}
enum class ECompetitiveTier : uint8_t
{
	Iron1 = 3,
	Iron2 = 4,
	Iron3 = 5,
	Bronze1 = 6,
	Bronze2 = 7,
	Bronze3 = 8,
	Silver1 = 9,
	Silver2 = 10,
	Silver3 = 11,
	Gold1 = 12,
	Gold2 = 13,
	Gold3 = 14,
	Platinum1 = 15,
	Platinum2 = 16,
	Platinum3 = 17,
	Diamond1 = 18,
	Diamond2 = 19,
	Diamond3 = 20,
	Immortal1 = 21,
	Immortal2 = 22,
	Immortal3 = 23,
	Radiant = 24,
	ECompetitiveTier_MAX = 25,
};
std::wstring GetRankName(int tier) {
	switch ((ECompetitiveTier)tier) {
	case ECompetitiveTier::Iron1: return L"Iron 1";
	case ECompetitiveTier::Iron2: return L"Iron 2";
	case ECompetitiveTier::Iron3: return L"Iron 3";
	case ECompetitiveTier::Bronze1: return L"Bronze 1";
	case ECompetitiveTier::Bronze2: return L"Bronze 2";
	case ECompetitiveTier::Bronze3: return L"Bronze 3";
	case ECompetitiveTier::Silver1: return L"Silver 1";
	case ECompetitiveTier::Silver2: return L"Silver 2";
	case ECompetitiveTier::Silver3: return L"Silver 3";
	case ECompetitiveTier::Gold1: return L"Gold 1";
	case ECompetitiveTier::Gold2: return L"Gold 2";
	case ECompetitiveTier::Gold3: return L"Gold 3";
	case ECompetitiveTier::Platinum1: return L"Platinum 1";
	case ECompetitiveTier::Platinum2: return L"Platinum 2";
	case ECompetitiveTier::Platinum3: return L"Platinum 3";
	case ECompetitiveTier::Diamond1: return L"Diamond 1";
	case ECompetitiveTier::Diamond2: return L"Diamond 2";
	case ECompetitiveTier::Diamond3: return L"Diamond 3";
	case ECompetitiveTier::Immortal1: return L"Immortal 1";
	case ECompetitiveTier::Immortal2: return L"Immortal 2";
	case ECompetitiveTier::Immortal3: return L"Immortal 3";
	case ECompetitiveTier::Radiant: return L"Radiant";
	default: return L"Unranked";
	}
}

FLinearColor GetTierColor(int tier) {
	if (tier >= 3 && tier <= 5) return RGBtoFLC(110, 110, 110); // Iron
	if (tier >= 6 && tier <= 8) return RGBtoFLC(150, 111, 51);  // Bronze
	if (tier >= 9 && tier <= 11) return RGBtoFLC(192, 192, 192); // Silver
	if (tier >= 12 && tier <= 14) return RGBtoFLC(255, 215, 0); // Gold
	if (tier >= 15 && tier <= 17) return RGBtoFLC(120, 255, 255); // Plat
	if (tier >= 18 && tier <= 20) return RGBtoFLC(170, 85, 255); // Diamond
	if (tier >= 21 && tier <= 23) return RGBtoFLC(255, 85, 255); // Immortal
	if (tier == 24) return RGBtoFLC(255, 50, 50);               // Radiant
	return RGBtoFLC(255, 255, 255); // default white
}

void angle_rotation(const FVector& angles, FVector* forward)
{
	float	sp, sy, cp, cy;

	sy = sin(DegreeToRadian(angles.Y));
	cy = cos(DegreeToRadian(angles.Y));

	sp = sin(DegreeToRadian(angles.X));
	cp = cos(DegreeToRadian(angles.X));

	forward->X = cp * cy;
	forward->Y = cp * sy;
	forward->Z = -sp;
}

void AntiAfk() {

	if (!GetAsyncKeyState(0x57) || !GetAsyncKeyState(0x41) || GetAsyncKeyState(0x44) || GetAsyncKeyState(0x53)) {
		INPUT space = { 0 };
		space.ki.wVk = VK_SPACE;
		space.type = INPUT_KEYBOARD;
		SendInput(1, &space, sizeof(INPUT));
		Sleep(rand() % 5 + 1);
		space.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &space, sizeof(INPUT));
	}

}

void mouse_shoot(BYTE key, bool set) {

	keybd_event(key,
		0x45,
		set ? (KEYEVENTF_EXTENDEDKEY | 0) : (KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP),
		0);
}
void FovChanger(APlayerController* MyController) {

	MyController->set_fov(Globals::FovChangerValue);
}
enum Bones : int {
	head = 8,
	base = 0,
	CestBone = 6,
	Leg = 4
};

void DrawLineSimple(UCanvas* can, FVector2D first_bone_position, FVector2D second_bone_position, FLinearColor Color)
{

	can->K2_DrawLine({ first_bone_position.X, first_bone_position.Y }, { second_bone_position.X, second_bone_position.Y }, Globals::ESPThickness, Color);
}
void DrawBox(UCanvas* canvas, const FVector2D& topLeft, const FVector2D& bottomRight, const FLinearColor& color, bool outline = false) {
	const float thickness = Globals::ESPThickness;

	const FVector2D topRight = { bottomRight.X, topLeft.Y };
	const FVector2D bottomLeft = { topLeft.X, bottomRight.Y };

	if (outline) {
		const float outlineThickness = thickness + 2.0f;
		const FLinearColor outlineColor = { 0.0f,0.0f,0.f,1.0f };

		canvas->K2_DrawLine(topLeft, topRight, outlineThickness, outlineColor);
		canvas->K2_DrawLine(topLeft, bottomLeft, outlineThickness, outlineColor);
		canvas->K2_DrawLine(bottomRight, topRight, outlineThickness, outlineColor);
		canvas->K2_DrawLine(bottomRight, bottomLeft, outlineThickness, outlineColor);
	}

	// Main box lines
	canvas->K2_DrawLine(topLeft, topRight, thickness, color);
	canvas->K2_DrawLine(topLeft, bottomLeft, thickness, color);
	canvas->K2_DrawLine(bottomRight, topRight, thickness, color);
	canvas->K2_DrawLine(bottomRight, bottomLeft, thickness, color);
}
namespace helper {
	FString convert_weapon_name(FString weapon_name)
	{
		std::wstring weapon_name_str = weapon_name.wide();

		if (weapon_name_str.find(L"Ability_Melee_Base_C") != std::wstring::npos)
			return L"Melee";
		else if (weapon_name_str.find(L"BasePistol_C") != std::wstring::npos)
			return L"Classic";
		else if (weapon_name_str.find(L"SawedOffShotgun_C") != std::wstring::npos)
			return L"Shorty";
		else if (weapon_name_str.find(L"AutomaticPistol_C") != std::wstring::npos)
			return L"Frenzy";
		else if (weapon_name_str.find(L"LugerPistol_C") != std::wstring::npos)
			return L"Ghost";
		else if (weapon_name_str.find(L"RevolverPistol_C") != std::wstring::npos)
			return L"Sheriff";
		else if (weapon_name_str.find(L"Vector_C") != std::wstring::npos)
			return L"Stinger";
		else if (weapon_name_str.find(L"SubMachineGun_MP5") != std::wstring::npos)
			return L"Spectre";
		else if (weapon_name_str.find(L"PumpShotgun_C") != std::wstring::npos)
			return L"Bucky";
		else if (weapon_name_str.find(L"AutomaticShotgun_C") != std::wstring::npos)
			return L"Judge";
		else if (weapon_name_str.find(L"AssaultRifle_Burst_C") != std::wstring::npos)
			return L"Bulldog";
		else if (weapon_name_str.find(L"DMR_C") != std::wstring::npos)
			return L"Guardian";
		else if (weapon_name_str.find(L"AssaultRifle_ACR_C") != std::wstring::npos)
			return L"Phantom";
		else if (weapon_name_str.find(L"AssaultRifle_AK_C") != std::wstring::npos)
			return L"Vandal";
		else if (weapon_name_str.find(L"LeverSniperRifle_C") != std::wstring::npos)
			return L"Marshal";
		else if (weapon_name_str.find(L"BoltSniper_C") != std::wstring::npos)
			return L"Operator";
		else if (weapon_name_str.find(L"LightMachineGun_C") != std::wstring::npos)
			return L"Ares";
		else if (weapon_name_str.find(L"HeavyMachineGun_C") != std::wstring::npos)
			return L"Odin";
		else if (weapon_name_str.find(L"Gun_Deadeye_Q_Pistol_C") != std::wstring::npos)
			return L"Headhunter";
		else if (weapon_name_str.find(L"Ability_Wushu_X_Dagger_Production_C") != std::wstring::npos)
			return L"Blade storm";
		else if (weapon_name_str.find(L"Gun_Sprinter_X_HeavyLightningGun_Production_C") != std::wstring::npos)
			return L"Overdrive";
		else if (weapon_name_str.find(L"DS_Gun_C") != std::wstring::npos)
			return L"Outlaw";
		else if (weapon_name_str.find(L"Gun_Deadeye_X_Giantslayer_Prototype_C") != std::wstring::npos)
			return L"Tour de force";
		return L"Invalid";
	}
}

namespace pos_decrpt {
	FVector2D pos = {
	((float)GetSystemMetrics(SM_CXSCREEN) / 2) - 500,
	((float)GetSystemMetrics(SM_CYSCREEN) / 2) - 475
	};
}
void DrawLineSimple_3D(UCanvas* can, FVector2D first_bone_position, FVector2D second_bone_position, FLinearColor Color) {
	can->K2_DrawLine({ first_bone_position.X, first_bone_position.Y }, { second_bone_position.X, second_bone_position.Y }, 1.15f, Color);
}
void DrawLineSimple_Spike(UCanvas* can, FVector2D first_bone_position, FVector2D second_bone_position, FLinearColor Color) {
	can->K2_DrawLine({ first_bone_position.X, first_bone_position.Y }, { second_bone_position.X, second_bone_position.Y }, 0.9f, Color);
}
void Draw3DDiamond(UCanvas* Canvas, APlayerController* MYController, FVector ObjectLocation3D, float BaseLength, float Height, FLinearColor Color)
{

	FVector Pos0, Pos1, Pos2, Pos3, Pos4, Pos5;
	Pos0 = ObjectLocation3D;
	Pos0.Z += 20;
	Pos1 = Pos0 + FVector(-BaseLength / 2, 0, 0);
	Pos2 = Pos0 + FVector(BaseLength / 2, 0, 0);
	Pos3 = Pos0 + FVector(0, -BaseLength / 2, 0);
	Pos4 = Pos0 + FVector(0, BaseLength / 2, 0);
	Pos5 = Pos0 + FVector(0, 0, Height);

	FVector2D ScreenPos0, ScreenPos1, ScreenPos2, ScreenPos3, ScreenPos4, ScreenPos5;
	if (MYController->ProjectWorldLocationToScreen(Pos0, ScreenPos0, 0) && ScreenPos0.IsValid() &&
		MYController->ProjectWorldLocationToScreen(Pos1, ScreenPos1, 0) && ScreenPos1.IsValid() &&
		MYController->ProjectWorldLocationToScreen(Pos2, ScreenPos2, 0) && ScreenPos2.IsValid() &&
		MYController->ProjectWorldLocationToScreen(Pos3, ScreenPos3, 0) && ScreenPos3.IsValid() &&
		MYController->ProjectWorldLocationToScreen(Pos4, ScreenPos4, 0) && ScreenPos4.IsValid() &&
		MYController->ProjectWorldLocationToScreen(Pos5, ScreenPos5, 0) && ScreenPos5.IsValid())
	{
		// Draw lines between points to create the 3D triangle
		DrawLineSimple_Spike(Canvas, ScreenPos1, ScreenPos2, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos2, ScreenPos3, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos3, ScreenPos1, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos1, ScreenPos4, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos2, ScreenPos4, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos3, ScreenPos4, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos0, ScreenPos5, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos1, ScreenPos5, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos2, ScreenPos5, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos3, ScreenPos5, Color);
		DrawLineSimple_Spike(Canvas, ScreenPos4, ScreenPos5, Color);
	}

}

void Draw3D(UCanvas* Canvas, APlayerController* MYController, USkeletalMeshComponent* Mesh, FLinearColor Color)
{
	FVector2D bottom1;
	FVector2D bottom2;
	FVector2D bottom3;
	FVector2D bottom4;
	FVector2D top1;
	FVector2D top2;
	FVector2D top3;
	FVector2D top4;
	FVector vBaseBone = GetBoneMatrix(Mesh, 0);
	FVector vHeadBone = GetBoneMatrix(Mesh, 8);
	if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X + 53, vBaseBone.Y - 55, vBaseBone.Z), bottom1, 0) && bottom1.IsValid())
	{
		if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X - 53, vBaseBone.Y - 55, vBaseBone.Z), bottom2, 0) && bottom2.IsValid())
		{
			if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X - 53, vBaseBone.Y + 55, vBaseBone.Z), bottom3, 0) && bottom3.IsValid())
			{
				if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X + 53, vBaseBone.Y + 55, vBaseBone.Z), bottom4, 0) && bottom4.IsValid())
				{
					if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X + 53, vHeadBone.Y - 55, vHeadBone.Z + 26), top1, 0) && top1.IsValid())
					{
						if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X - 53, vHeadBone.Y - 55, vHeadBone.Z + 26), top2, 0) && top2.IsValid())
						{
							if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X - 53, vHeadBone.Y + 55, vHeadBone.Z + 26), top3, 0) && top3.IsValid())
							{
								if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X + 53, vHeadBone.Y + 55, vHeadBone.Z + 26), top4, 0) && top4.IsValid())
								{

									DrawLineSimple_3D(Canvas, bottom1, top1, Color);
									DrawLineSimple_3D(Canvas, bottom1, top1, Color);
									DrawLineSimple_3D(Canvas, bottom2, top2, Color);
									DrawLineSimple_3D(Canvas, bottom3, top3, Color);
									DrawLineSimple_3D(Canvas, bottom4, top4, Color);


									DrawLineSimple_3D(Canvas, bottom1, bottom2, Color);
									DrawLineSimple_3D(Canvas, bottom2, bottom3, Color);
									DrawLineSimple_3D(Canvas, bottom3, bottom4, Color);
									DrawLineSimple_3D(Canvas, bottom4, bottom1, Color);

									DrawLineSimple_3D(Canvas, top1, top2, Color);
									DrawLineSimple_3D(Canvas, top2, top3, Color);
									DrawLineSimple_3D(Canvas, top3, top4, Color);
									DrawLineSimple_3D(Canvas, top4, top1, Color);
								}
							}
						}
					}
				}
			}
		}

	}

}
namespace health {
	FLinearColor HealthColor;
	static FLinearColor HighHealth = { 0.0f,255.0f,0.0f,1.0f };
	static FLinearColor NormalHealth = { 255, 151, 0 ,1.0f };
	static FLinearColor LowHealth = { 255.0f,0.0f,0.f,1.0f };
}
void DrawLineBottomLocation(UCanvas* canvas, APlayerController* MyController, USkeletalMeshComponent* Mesh) {
	FVector2D drawtextat;
	if (MyController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 0), drawtextat, 0) && drawtextat.IsValid()) {
		canvas->K2_DrawLine({ (float)canvas->GetScreenSize().X / 2, (float)canvas->GetScreenSize().Y / 1 }, drawtextat, Globals::ESPThickness, Colors::LineColor);
	}
}

void DrawLineTopLocation(UCanvas* canvas, APlayerController* MyController, USkeletalMeshComponent* Mesh) {
	FVector2D drawtextat;
	if (MyController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 8), drawtextat, 0) && drawtextat.IsValid()) {
		canvas->K2_DrawLine({ (float)canvas->GetScreenSize().X / 2, (float)canvas->GetScreenSize().Y / 800.0f }, drawtextat, Globals::ESPThickness, Colors::LineColor);
	}
}

void DrawEspText(UCanvas* canvas, const FString& text, const FVector2D& position, const FVector2D& scale, const FLinearColor& color, float shadowOpacity = 0.75f)
{
	FLinearColor shadowColor = menu::colors::Text_Shadow;
	FLinearColor outlineColor = FLinearColor(0, 0, 0, shadowOpacity);  // black w/ variable alpha
	FVector2D shadowOffset = FVector2D(1.0f, 1.0f); // slight offset for soft shadow

	canvas->K2_DrawText(
		text,                 // The text to render
		position,             // Screen position
		scale,                // Scale (usually FVector2D(1.0f, 1.0f))
		color,                // Main text color
		0.0f,                 // Kerning
		shadowColor,          // Shadow color
		shadowOffset,         // Shadow offset
		true,                 // Center X
		true,                 // Center Y
		true,                 // Enable outline
		outlineColor          // Outline color
	);
}


void DrawAgentNameEsp_noneHealth(UCanvas* canvas, UObject* actor, const FVector2D& baseOut, USkeletalMeshComponent* Mesh, APlayerController* myController, float boxWidth, float distance) {
	if (Globals::AgentName && distance >= 1.2f) {
		FString objectName = kismentsystemlibrary::get_object_name((UObject*)actor);
		float X = baseOut.X - (boxWidth / 2), Y = baseOut.Y + (boxWidth / 10.0f);
		FVector2D vHeadBoneOut;
		if (myController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 8), vHeadBoneOut, 0) && vHeadBoneOut.IsValid()) {
			DrawEspText(canvas, Filters::Names::CharacterName(objectName), FVector2D(baseOut.X, baseOut.Y + 15.0f), FVector2D{ 0.8f ,0.8f }, Colors::AgentNameColor);
		}
	}
}
void EnemyIcon(UCanvas* canvas, AShooterCharacter* Actor, FVector2D BaseOut2, float distance) {

	UObject* icon = Actor->get_character_icon();
	if (!icon) return;

	FVector2D baseSize = FVector2D(2.5f, 2.5f);
	float scaleFactor = 90.0f / distance;
	FVector2D newSize = baseSize * scaleFactor;

	canvas->K2_DrawTexture(icon, BaseOut2, newSize,
		FVector2D(0.0f, 0.0f),
		FVector2D{ 1.0f, 1.0f },
		FLinearColor(1, 1, 1, 1), EBlendMode::BLEND_Translucent, 0.0f, FVector2D(0.5f, 0.5f));
}


void TargetLine(UCanvas* canvas, APlayerController* MyController, USkeletalMeshComponent* Mesh, FVector Bone) {
	FVector2D drawtextat;
	if (MyController->ProjectWorldLocationToScreen(Bone, drawtextat, 0) && drawtextat.IsValid()) {
		canvas->K2_DrawLine({ (float)canvas->GetScreenSize().X / 2, (float)canvas->GetScreenSize().Y / 2 }, drawtextat, 1.1f, Colors::LineColor);
	}
}

void DrawAgentNameEsp(UCanvas* canvas, UObject* actor, const FVector2D& baseOut, USkeletalMeshComponent* Mesh, APlayerController* myController, float boxWidth, float distance) {
	if (Globals::AgentName && distance >= 1.2f) {
		FString objectName = kismentsystemlibrary::get_object_name((UObject*)actor);
		float X = baseOut.X - (boxWidth / 2), Y = baseOut.Y + (boxWidth / 10.0f);
		FVector2D vHeadBoneOut;
		if (myController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 8), vHeadBoneOut, 0) && vHeadBoneOut.IsValid()) {
			DrawEspText(
				canvas,
				Filters::Names::CharacterName(objectName),          // Agent name
				FVector2D(baseOut.X, baseOut.Y + 25.0f),            // Slight vertical offset below head
				FVector2D(0.8f, 0.8f),                              // Slightly smaller scale
				Colors::AgentNameColor,                             // Your defined agent name color
				0.75f                                               // Shadow opacity
			);

		}
	}
}

void DrawAgentNameEsp_dist8(UCanvas* canvas, UObject* actor, const FVector2D& baseOut, USkeletalMeshComponent* Mesh, APlayerController* myController, float boxWidth, float distance) {
	if (Globals::AgentName && distance >= 1.2f) {
		FString objectName = kismentsystemlibrary::get_object_name((UObject*)actor);
		float X = baseOut.X - (boxWidth / 2), Y = baseOut.Y + (boxWidth / 10.0f);
		FVector2D vHeadBoneOut;

		FVector2D newSize;
		if (distance >= 16) {
			FVector2D baseSize = FVector2D(0.20f, 0.20f);
			float scaleFactor = 100.0f / distance;
			newSize = baseSize * scaleFactor;
		}
		else {
			newSize = FVector2D(1.2f, 1.2f);
		}

		if (myController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, 8), vHeadBoneOut, 0) && vHeadBoneOut.IsValid()) {
			DrawEspText(canvas, Filters::Names::CharacterName(objectName), FVector2D(baseOut.X, baseOut.Y + 40.0f), FVector2D{ 0.8f, 0.8f }, Colors::AgentNameColor);
		}
	}
}
void Draw2DBox(UCanvas* canvas, APlayerController* MyController, USkeletalMeshComponent* Mesh, const FLinearColor& color, bool outline = false)
{
	// Get top of head with Z offset
	FVector headWorld = GetBoneMatrix(Mesh, 8);
	headWorld.Z += 15.0f; // <- ensures box touches top of head

	FVector footWorld = GetBoneMatrix(Mesh, 0);

	FVector2D headPos, footPos;

	if (!MyController->ProjectWorldLocationToScreen(headWorld, headPos, 0) || !headPos.IsValid())
		return;

	if (!MyController->ProjectWorldLocationToScreen(footWorld, footPos, 0) || !footPos.IsValid())
		return;

	// Calculate box dimensions
	const float height = (footPos.Y > headPos.Y) ? (footPos.Y - headPos.Y) : (headPos.Y - footPos.Y);
	const float width = height * 0.65f;

	const FVector2D topLeft = { headPos.X - width * 0.5f, headPos.Y };
	const FVector2D bottomRight = { headPos.X + width * 0.5f, footPos.Y };

	// Draw the box
	DrawBox(canvas, topLeft, bottomRight, color, outline);
}

void DrawCorner(UCanvas* Canvas, APlayerController* MYController, USkeletalMeshComponent* Mesh, FLinearColor Color, float thick) {

	FVector Head = GetBoneMatrix(Mesh, 8);
	FVector2D HeadLongOut = MYController->ProjectWorldToScreen(FVector(Head.X, Head.Y, Head.Z + 15));
	FVector Base = GetBoneMatrix(Mesh, 0);
	FVector2D BaseOut = MYController->ProjectWorldToScreen(Base);

	float BoxHeight = abs(HeadLongOut.Y - BaseOut.Y);
	float BoxWidth = BoxHeight * 0.55;

	float X = HeadLongOut.X - (BoxWidth / 2), Y = HeadLongOut.Y;
	float lineW = (BoxWidth / 3);
	float lineH = (BoxHeight / 3);
	Canvas->K2_DrawLine(FVector2D(X, Y), FVector2D(X, Y + lineH), thick, Colors::BoxColor);
	Canvas->K2_DrawLine(FVector2D(X, Y), FVector2D(X + lineW, Y), thick, Colors::BoxColor);
	Canvas->K2_DrawLine(FVector2D(X + BoxWidth - lineW, Y), FVector2D(X + BoxWidth, Y), thick, Colors::BoxColor);
	Canvas->K2_DrawLine(FVector2D(X + BoxWidth, Y), FVector2D(X + BoxWidth, Y + lineH), thick, Colors::BoxColor);
	Canvas->K2_DrawLine(FVector2D(X, Y + BoxHeight - lineH), FVector2D(X, Y + BoxHeight), thick, Colors::BoxColor);
	Canvas->K2_DrawLine(FVector2D(X, Y + BoxHeight), FVector2D(X + lineW, Y + BoxHeight), thick, Colors::BoxColor);
	Canvas->K2_DrawLine(FVector2D(X + BoxWidth - lineW, Y + BoxHeight), FVector2D(X + BoxWidth, Y + BoxHeight), thick, Colors::BoxColor);
	Canvas->K2_DrawLine(FVector2D(X + BoxWidth, Y + BoxHeight - lineH), FVector2D(X + BoxWidth, Y + BoxHeight), thick, Colors::BoxColor);
}
void DrawSnapline(UCanvas* canvas, AShooterCharacter* shooter, FVector2D headOut, FLinearColor color, float tick) {
	if (shooter->IsAlive()) {
		float radius = 54;
		float A = headOut.Y - (float)canvas->GetScreenSize().Y / 2;
		float B = headOut.X - (float)canvas->GetScreenSize().X / 2;
		float x3, y3;
		float angle = atan(A / B);

		if (B >= 0) {
			x3 = radius * cos(angle) + (float)canvas->GetScreenSize().X / 2;
			y3 = radius * sin(angle) + (float)canvas->GetScreenSize().Y / 2;
		}
		else {
			x3 = radius * (-cos(angle)) + (float)canvas->GetScreenSize().X / 2;
			y3 = radius * (-sin(angle)) + (float)canvas->GetScreenSize().Y / 2;
		}

		if (sqrt(A * A + B * B) >= radius) {
			canvas->K2_DrawLine(FVector2D(headOut.X, headOut.Y), FVector2D(x3, y3), tick, color);
		}
	}
}
void DrawHealthAndShieldBars(UCanvas* canvas, APlayerController* controller, USkeletalMeshComponent* mesh, float health, float shield)
{
	// Get projected head and foot
	FVector headWorld = GetBoneMatrix(mesh, 8);
	headWorld.Z += 15.0f; // Align with box top

	FVector footWorld = GetBoneMatrix(mesh, 0);

	FVector2D headPos, footPos;
	if (!controller->ProjectWorldLocationToScreen(headWorld, headPos, true) || !headPos.IsValid()) return;
	if (!controller->ProjectWorldLocationToScreen(footWorld, footPos, true) || !footPos.IsValid()) return;

	// Continue as before
	const float height = (footPos.Y > headPos.Y) ? (footPos.Y - headPos.Y) : (headPos.Y - footPos.Y);
	const float boxWidth = height * 0.65f;

	const float barThickness = 3.0f;
	const float barSpacing = 1.0f;
	const float barOffset = 5.0f;

	const float barBaseX = headPos.X - (boxWidth / 2.0f) - barOffset - (barThickness * 2.0f) - barSpacing;

	const float topY = headPos.Y;
	const float bottomY = footPos.Y;

	const float healthRatio = FMath::Clamp(health / 100.0f, 0.0f, 1.0f);
	const float shieldRatio = FMath::Clamp(shield / 50.0f, 0.0f, 1.0f);

	const float healthFillHeight = height * healthRatio;
	const float shieldFillHeight = height * shieldRatio;

	FVector2D healthTopLeft(barBaseX, bottomY - healthFillHeight);
	FVector2D healthBottomRight(barBaseX + barThickness, bottomY);

	FVector2D shieldTopLeft(barBaseX + barThickness + barSpacing, bottomY - shieldFillHeight);
	FVector2D shieldBottomRight(barBaseX + (2 * barThickness) + barSpacing, bottomY);

	FVector2D healthOutlineTopLeft(barBaseX, topY);
	FVector2D healthOutlineBottomRight(barBaseX + barThickness, bottomY);

	FVector2D shieldOutlineTopLeft(barBaseX + barThickness + barSpacing, topY);
	FVector2D shieldOutlineBottomRight(barBaseX + (2 * barThickness) + barSpacing, bottomY);

	const FLinearColor healthColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
	const FLinearColor shieldColor = FLinearColor(0.25f, 0.6f, 1.0f, 1.0f);
	const FLinearColor outlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);;
	const float thickness = 1.0f;

	// Fill bar
	auto DrawBar = [&](FVector2D topLeft, FVector2D bottomRight, const FLinearColor& color) {
		for (float y = bottomRight.Y - 1; y >= topLeft.Y; --y) {
			canvas->K2_DrawLine(FVector2D(topLeft.X, y), FVector2D(bottomRight.X, y), thickness, color);
		}
		};

	// Outline
	auto DrawOutline = [&](FVector2D topLeft, FVector2D bottomRight) {
		canvas->K2_DrawLine(topLeft, FVector2D(bottomRight.X, topLeft.Y), thickness, outlineColor); // Top
		canvas->K2_DrawLine(topLeft, FVector2D(topLeft.X, bottomRight.Y), thickness, outlineColor); // Left
		canvas->K2_DrawLine(FVector2D(bottomRight.X, topLeft.Y), bottomRight, thickness, outlineColor); // Right
		canvas->K2_DrawLine(FVector2D(topLeft.X, bottomRight.Y), bottomRight, thickness, outlineColor); // Bottom
		};

	DrawBar(healthTopLeft, healthBottomRight, healthColor);
	DrawBar(shieldTopLeft, shieldBottomRight, shieldColor);
	DrawOutline(healthOutlineTopLeft, healthOutlineBottomRight);
	DrawOutline(shieldOutlineTopLeft, shieldOutlineBottomRight);
}
















void DrawWeaponEsp(UCanvas* canvas, UObject* equippable, const FVector2D& headLongOut, float distance)
{
	if (Globals::WeaponEsp && distance >= 1.2f) {
		FString weaponName = kismentsystemlibrary::get_object_name((UObject*)equippable);
		DrawEspText(canvas, Filters::Names::WeaponName(weaponName), FVector2D(headLongOut.X, headLongOut.Y - 10), FVector2D{ 1.f, 1.f }, Colors::WeaponNameColor);
	}
}
void DrawBoxLines(UCanvas* canvas, FVector2D position, float width, float height, FLinearColor color)
{
	float X = position.X - (width / 2);
	float Y = position.Y;
	float lineW = (width / 3);
	float lineH = (height / 3);

	canvas->K2_DrawLine(FVector2D(X, Y), FVector2D(X, Y + lineH), 1.8f, color);
	canvas->K2_DrawLine(FVector2D(X, Y), FVector2D(X + lineW, Y), 1.8f, color);
	canvas->K2_DrawLine(FVector2D(X + width - lineW, Y), FVector2D(X + width, Y), 1.8f, color);
	canvas->K2_DrawLine(FVector2D(X + width, Y), FVector2D(X + width, Y + lineH), 1.8f, color);
	canvas->K2_DrawLine(FVector2D(X, Y + height - lineH), FVector2D(X, Y + height), 1.8f, color);
	canvas->K2_DrawLine(FVector2D(X, Y + height), FVector2D(X + lineW, Y + height), 1.8f, color);
	canvas->K2_DrawLine(FVector2D(X + width - lineW, Y + height), FVector2D(X + width, Y + height), 1.8f, color);
	canvas->K2_DrawLine(FVector2D(X + width, Y + height - lineH), FVector2D(X + width, Y + height), 1.8f, color);
}
void DrawCircle(UCanvas* canvas, FVector2D pos, int radius, int numSides, FLinearColor Color)
{
	float PI = 3.1415927f;

	float Step = PI * 2.0 / numSides;
	int Count = 0;
	FVector2D V[128];
	for (float A = 0; A < PI * 2.0; A += Step) {
		float X1 = radius * cos(A) + pos.X;
		float Y1 = radius * sin(A) + pos.Y;
		float X2 = radius * cos(A + Step) + pos.X;
		float Y2 = radius * sin(A + Step) + pos.Y;
		V[Count].X = X1;
		V[Count].Y = Y1;
		V[Count + 1].X = X2;
		V[Count + 1].Y = Y2;
		//Draw_Line(FVector2D{ pos.X, pos.Y }, FVector2D{ X2, Y2 }, 1.0f, Color); // Points from Centre to ends of circle
		canvas->K2_DrawLine(FVector2D{ V[Count].X, V[Count].Y }, FVector2D{ X2, Y2 }, 0.5f, Color);// Circle Around
	}
}
void DrawFilledCircle(UCanvas* canvas, FVector2D pos, int radius, int numSides, FLinearColor Color)
{
	if (!canvas) return;

	float PI = 3.1415927f;
	float Step = PI * 2.0f / numSides;

	for (int i = 0; i < numSides; ++i)
	{
		float angle1 = Step * i;
		float angle2 = Step * (i + 1);

		FVector2D point1 = FVector2D(pos.X + radius * cosf(angle1), pos.Y + radius * sinf(angle1));
		FVector2D point2 = FVector2D(pos.X + radius * cosf(angle2), pos.Y + radius * sinf(angle2));

		// Draw triangle "fill" as two lines from center to the two points,
		// then the line between those two points to close the segment
		canvas->K2_DrawLine(pos, point1, 1.0f, Color);
		canvas->K2_DrawLine(point1, point2, 1.0f, Color);
		canvas->K2_DrawLine(pos, point2, 1.0f, Color);
	}
}

void DrawProgressBarFill(UCanvas* canvas, const FVector2D& TopLeft, const FVector2D& BottomRight, float Percentage) {
	float X = TopLeft.X, Y = TopLeft.Y;
	const FLinearColor ForegroundColor = { 0, 0, 1, 1 };

	for (int i = 0; i < 5; i++) {
		if (i > 0 && i < 5 - 1) {
			// Draw foreground bar with percentage
			float ForegroundWidth = 70 * Percentage / 100;
			canvas->K2_DrawLine(FVector2D(X, Y + i), FVector2D(X + ForegroundWidth, Y + i), 2.9, ForegroundColor);
		}
	}
}
void DrawBoxOutline(UCanvas* canvas, const FVector2D& TopLeft, const FVector2D& BottomRight) {
	canvas->K2_DrawLine(TopLeft, FVector2D(BottomRight.X, TopLeft.Y), 2, { 170, 170, 170, 0.0002f });
	canvas->K2_DrawLine(FVector2D(TopLeft.X, BottomRight.Y), BottomRight, 2, { 170, 170, 170, 0.0002f });
	canvas->K2_DrawLine(TopLeft, FVector2D(TopLeft.X, BottomRight.Y), 2, { 170, 170, 170, 0.0002f });
	canvas->K2_DrawLine(FVector2D(BottomRight.X, TopLeft.Y), BottomRight, 2, { 170, 170, 170, 0.0002f });
}

void ModifySkyLight_Raw()
{
	if (!GWorld)
	{
		printf("[Error] UWorldSave is null!\n");
		return;
	}

	uintptr_t world = *reinterpret_cast<uintptr_t*>(GWorld);
	if (!world || world == 0xFFFFFFFFFFFFFFFF)
	{
		printf("[Error] UWorld pointer is invalid!\n");
		return;
	}

	uintptr_t persistentLevel = *reinterpret_cast<uintptr_t*>(world + 0x30);
	if (!persistentLevel)
	{
		printf("[Error] PersistentLevel is null!\n");
		return;
	}

	constexpr uintptr_t actor_array = 0x98;
	constexpr uintptr_t actors_count = 0xA0;

	uintptr_t actorArray = *reinterpret_cast<uintptr_t*>(persistentLevel + actor_array);
	int32_t actorCount = *reinterpret_cast<int32_t*>(persistentLevel + actors_count);
	printf("[Debug] actorArray = 0x%llX\n", actorArray);
	printf("[Debug] actorCount = %d\n", actorCount);
	if (!actorArray || actorCount <= 0 || actorCount > 10000)
	{
		printf("[Error] Actor array invalid or suspicious actor count: %d\n", actorCount);
		return;
	}

	for (int i = 0; i < actorCount; i++)
	{
		uintptr_t actor = *reinterpret_cast<uintptr_t*>(actorArray + i * sizeof(uintptr_t));
		if (!actor || actor == 0xFFFFFFFFFFFFFFFF)
			continue;

		UObject* uobj = reinterpret_cast<UObject*>(actor);
		if (!uobj)
			continue;

		FString name = kismentsystemlibrary::get_object_name(uobj);
		if (!name.IsValid())
			continue;

		std::wstring wide_name(name.c_str());
		if (wide_name.find(L"SkyLight") == std::wstring::npos)
			continue;

		printf("[Debug] Found potential SkyLight actor: %ws\n", wide_name.c_str());

		uintptr_t lightComponent = *reinterpret_cast<uintptr_t*>(actor + 0x3b8);
		if (!lightComponent || lightComponent == 0xFFFFFFFFFFFFFFFF)
		{
			printf("[Warning] LightComponent is null or invalid.\n");
			continue;
		}

		static UObject* fn = UObject::find_object2<UObject*>(L"SkyLightComponent.SetLowerHemisphereColor");
		if (!fn)
		{
			printf("[Error] Failed to find UFunction: SetLowerHemisphereColor\n");
			return;
		}

		struct {
			FLinearColor in_lower_hemisphere_color;
		} params;

		params.in_lower_hemisphere_color = { 1.f, 0.40f, 0.0f, 1.0f };

		// ✅ Use ProcessEvent directly!
		UObject::ProcessEvent(reinterpret_cast<void*>(lightComponent), fn, &params);

		printf("[Success] SkyLight color applied to actor: %ws\n", wide_name.c_str());
		break;
	}
}



void Draw3DWeapon(UCanvas* Canvas, APlayerController* MYController, FVector ObjectLocation3D, FLinearColor Color)
{
	int xz = 70;
	int uzunluk = 20;
	FVector Pos0, Pos1, Pos2, Pos3, Pos4, Pos5, Pos6, Pos7, Pos8;
	Pos0 = ObjectLocation3D;
	Pos0.Z += 10;
	Pos1 = Pos0 + FVector(-xz, -xz, uzunluk);
	Pos2 = Pos0 + FVector(-xz, -xz, -uzunluk);
	Pos3 = Pos0 + FVector(xz, -xz, -uzunluk);
	Pos4 = Pos0 + FVector(xz, -xz, uzunluk);
	Pos5 = Pos0 + FVector(-xz, xz, uzunluk);
	Pos6 = Pos0 + FVector(-xz, xz, -uzunluk);
	Pos7 = Pos0 + FVector(xz, xz, -uzunluk);
	Pos8 = Pos0 + FVector(xz, xz, uzunluk);

	FVector2D ScreenPos0, ScreenPos1, ScreenPos2, ScreenPos3, ScreenPos4, ScreenPos5, ScreenPos6, ScreenPos7, ScreenPos8;
	if (MYController->ProjectWorldLocationToScreen(Pos0, ScreenPos0, 0) && ScreenPos0.IsValid())
	{
		if (MYController->ProjectWorldLocationToScreen(Pos1, ScreenPos1, 0) && ScreenPos1.IsValid())
		{
			if (MYController->ProjectWorldLocationToScreen(Pos2, ScreenPos2, 0) && ScreenPos2.IsValid())
			{
				if (MYController->ProjectWorldLocationToScreen(Pos3, ScreenPos3, 0) && ScreenPos3.IsValid())
				{
					if (MYController->ProjectWorldLocationToScreen(Pos4, ScreenPos4, 0) && ScreenPos4.IsValid())
					{
						if (MYController->ProjectWorldLocationToScreen(Pos5, ScreenPos5, 0) && ScreenPos5.IsValid())
						{
							if (MYController->ProjectWorldLocationToScreen(Pos6, ScreenPos6, 0) && ScreenPos6.IsValid())
							{
								if (MYController->ProjectWorldLocationToScreen(Pos7, ScreenPos7, 0) && ScreenPos7.IsValid())
								{
									if (MYController->ProjectWorldLocationToScreen(Pos8, ScreenPos8, 0) && ScreenPos8.IsValid())
									{
										// Draw lines between points to create the 3D box
										DrawLineSimple(Canvas, ScreenPos1, ScreenPos2, Color);
										DrawLineSimple(Canvas, ScreenPos2, ScreenPos3, Color);
										DrawLineSimple(Canvas, ScreenPos3, ScreenPos4, Color);
										DrawLineSimple(Canvas, ScreenPos4, ScreenPos1, Color);

										DrawLineSimple(Canvas, ScreenPos5, ScreenPos6, Color);
										DrawLineSimple(Canvas, ScreenPos6, ScreenPos7, Color);
										DrawLineSimple(Canvas, ScreenPos7, ScreenPos8, Color);
										DrawLineSimple(Canvas, ScreenPos8, ScreenPos5, Color);

										DrawLineSimple(Canvas, ScreenPos1, ScreenPos5, Color);
										DrawLineSimple(Canvas, ScreenPos2, ScreenPos6, Color);
										DrawLineSimple(Canvas, ScreenPos3, ScreenPos7, Color);
										DrawLineSimple(Canvas, ScreenPos4, ScreenPos8, Color);
									}
								}
							}
						}
					}
				}
			}
		}
	}


}
void DrawProgressBar(UCanvas* canvas, const FVector2D& Location, float Percentage) {
	FVector2D BoxTopLeft = FVector2D(Location.X - 35, Location.Y + 15);
	FVector2D BoxBottomRight = FVector2D(BoxTopLeft.X + 70, BoxTopLeft.Y + 5);

	DrawBoxOutline(canvas, BoxTopLeft, BoxBottomRight);
	DrawProgressBarFill(canvas, BoxTopLeft, BoxBottomRight, Percentage);
}
void DrawBombInfo(UCanvas* canvas, const FVector2D& ObjectLocation, float DefusePercentage, float bomb_time_remaining) {
	std::string timeRemainingStr = std::to_string((int)bomb_time_remaining);
	std::wstring timeRemainingWStr(timeRemainingStr.begin(), timeRemainingStr.end());

	canvas->K2_DrawText(timeRemainingWStr.c_str(), ObjectLocation, FVector2D{ 1.15f, 1.15f }, { 1,1, 1 ,2.5f }, false, menu::colors::Text_Shadow, FVector2D{ pos_decrpt::pos.X + 1, pos_decrpt::pos.Y + 1 }, true, true, true, { 0,0,0,0.60f });

	if (DefusePercentage > 0) {
		DrawProgressBar(canvas, ObjectLocation, DefusePercentage);
	}

}

void DrawDroppedSpikeInfo(UCanvas* canvas, const FVector2D& ObjectLocation) {
	canvas->K2_DrawText(FString{ L"Dropped Spike" }, ObjectLocation, FVector2D{ 1.06f, 1.06f }, { 1, 1, 1 ,2.5f }, false, menu::colors::Text_Shadow, FVector2D{ pos_decrpt::pos.X + 1, pos_decrpt::pos.Y + 1 }, true, true, true, { 0,0,0.65f });
}


void config_value(int value) {
	switch (value)
	{
	case 1:
		Globals::LegitFOvValue = 60.0f;

		Globals::EnableAim = true;
		Globals::DrawFov = true;
		Globals::LegitBotKey = VK_XBUTTON1;
		Globals::smooth = 4.0f;

		Globals::RecoilControl = true;
		Globals::RecoilCrosshair = true;
		Globals::NoSpreadAimbot = false;
		Globals::VisibleCheck = true;
		Globals::Silent = false;


		Globals::EnableESP = true;
		Globals::CorneredBox = true;
		Globals::HeadESP = true;
		Globals::Snapline = true;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = false;
		Globals::HealthText = false;
		Globals::Shieldbar = false;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = true;
		Globals::AntiAFK = false;
		break;
	case 2:
		Globals::LegitFOvValue = 1000.0f;
		Globals::EnableAim = true;
		Globals::DrawFov = false;
		Globals::LegitBotKey = VK_LBUTTON;
		Globals::smooth = 1.0f;
		Globals::RecoilControl = true;
		Globals::RecoilCrosshair = true;
		Globals::VisibleCheck = true;
		Globals::Silent = false;

		Globals::EnableESP = true;
		Globals::CorneredBox = true;
		Globals::HeadESP = true;
		Globals::Snapline = true;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = false;
		Globals::HealthText = false;
		Globals::Shieldbar = false;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = true;
		Globals::AntiAFK = false;
		break;
	case 3:
		Globals::LegitFOvValue = 1000.0f;
		Globals::EnableAim = true;
		Globals::DrawFov = false;
		Globals::LegitBotKey = VK_LBUTTON;
		Globals::smooth = 1.0f;
		Globals::RecoilControl = true;
		Globals::RecoilCrosshair = true;
		Globals::VisibleCheck = true;
		Globals::Silent = false;

		Globals::EnableESP = true;
		Globals::CorneredBox = true;
		Globals::HeadESP = true;
		Globals::Snapline = true;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = false;
		Globals::HealthText = false;
		Globals::Shieldbar = false;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = true;
		Globals::AntiAFK = false;
		break;
	case 4:
		Globals::LegitFOvValue = 17.0f;
		Globals::EnableAim = true;
		Globals::DrawFov = true;
		Globals::LegitBotKey;
		Globals::smooth = 15.0f;
		Globals::RecoilControl = false;
		Globals::RecoilCrosshair = false;
		Globals::AutoShoot = false;
		Globals::VisibleCheck = true;
		Globals::Silent = false;
		Globals::EnableSkin = false;

		Globals::EnableESP = true;
		Globals::CorneredBox = false;
		Globals::HeadESP = true;
		Globals::Snapline = false;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = true;
		Globals::HealthText = false;
		Globals::Shieldbar = true;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = true;
		Globals::AntiAFK = false;
		break;
	case 5:
		Globals::LegitFOvValue = 21.0f;
		Globals::EnableAim = true;
		Globals::DrawFov = false;
		Globals::LegitBotKey = VK_LBUTTON;
		Globals::smooth = 21.0f;
		Globals::RecoilControl = true;
		Globals::RecoilCrosshair = false;
		Globals::VisibleCheck = true;
		Globals::Silent = false;

		Globals::EnableESP = true;
		Globals::CorneredBox = false;
		Globals::HeadESP = true;
		Globals::Snapline = false;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = false;
		Globals::HealthText = false;
		Globals::Shieldbar = false;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = false;
		Globals::AntiAFK = false;
		break;
	case 6:
		Globals::LegitFOvValue = 6.0f;
		Globals::EnableAim = true;
		Globals::DrawFov = false;
		Globals::LegitBotKey = VK_LBUTTON;
		Globals::smooth = 25.0f;
		Globals::RecoilControl = true;
		Globals::RecoilCrosshair = false;
		Globals::VisibleCheck = true;
		Globals::Silent = false;

		Globals::EnableESP = true;
		Globals::CorneredBox = false;
		Globals::HeadESP = false;
		Globals::Snapline = false;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = false;
		Globals::HealthText = false;
		Globals::Shieldbar = false;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = false;
		Globals::AntiAFK = false;
		break;

	case 7:
		Globals::LegitFOvValue = 14.0f;
		Globals::EnableAim = true;
		Globals::DrawFov = false;
		Globals::LegitBotKey = VK_LBUTTON;
		Globals::smooth = 6.0f;
		Globals::RecoilControl = true;
		Globals::RecoilCrosshair = false;
		Globals::VisibleCheck = true;
		Globals::Silent = false;

		Globals::EnableESP = true;
		Globals::CorneredBox = false;
		Globals::HeadESP = true;
		Globals::Snapline = false;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = false;
		Globals::HealthText = false;
		Globals::Shieldbar = false;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = false;
		Globals::AntiAFK = false;
		break;
	case 8:
		Globals::LegitFOvValue = 16.0f;
		Globals::EnableAim = true;
		Globals::DrawFov = true;
		Globals::LegitBotKey = VK_LBUTTON;
		Globals::smooth = 4.7f;
		Globals::RecoilControl = false;
		Globals::RecoilCrosshair = false;
		Globals::VisibleCheck = true;
		Globals::Silent = false;

		Globals::EnableESP = true;
		Globals::CorneredBox = true;
		Globals::HeadESP = true;
		Globals::Snapline = true;
		Globals::SelectSnapline = 1;
		Globals::Healthbar = true;
		Globals::SkeletonEsp = true;

		Globals::Box2D = false;
		Globals::Box3D = false;
		Globals::AgentName = false;
		Globals::ChamsESP = false;
		Globals::DistangeEsp = false;
		Globals::HealthText = false;
		Globals::Shieldbar = false;

		Globals::SpikeEsp = false;
		Globals::weaponesp = false;
		Globals::WeaponEsp = false;
		Globals::EnableSkin = false;
		Globals::BunnyHop = false;
		Globals::AntiAFK = false;
		break;
	}



}
void aimbot_recoil(const FVector& CameraRot, const FVector& ControlRotation, const FVector& Target, const FVector& CameraPos, const FVector& recoil, APlayerController* MyController) {



	FVector vector_pos = Target - CameraPos;
	float distance = sqrtf(vector_pos.X * vector_pos.X + vector_pos.Y * vector_pos.Y + vector_pos.Z * vector_pos.Z);
	float X, Y, z;
	X = -((acosf(vector_pos.Z / distance) * (float)(180.0f / PI)) - 90.f);
	Y = atan2f(vector_pos.Y, vector_pos.X) * (float)(180.0f / PI);
	z = 0;

	FVector target_rotation = FVector(X, Y, z);
	FVector new_aim_rotation;
	new_aim_rotation.X = target_rotation.X - recoil.X - recoil.X;
	new_aim_rotation.Y = target_rotation.Y - recoil.Y - recoil.Y;
	new_aim_rotation.Z = 0;

	FVector new_rotation = Math::SmoothAim(new_aim_rotation, ControlRotation, Globals::smooth);

	if (new_rotation.X < 0) {
		new_rotation.X += 360.f;
	}

	if (new_rotation.Y < 0) {
		new_rotation.Y += 360.f;
	}

	new_rotation.Z = 0;

	MyController->SetControlRotation(new_rotation);
}
void aimbot(const FVector& CameraRot, const FVector& ControlRotation, const FVector& Target, const FVector& CameraPos, APlayerController* MyController) {



	FVector vector_pos = Target - CameraPos;
	float distance = sqrtf(vector_pos.X * vector_pos.X + vector_pos.Y * vector_pos.Y + vector_pos.Z * vector_pos.Z);
	float X, Y, z;
	X = -((acosf(vector_pos.Z / distance) * (float)(180.0f / PI)) - 90.f);
	Y = atan2f(vector_pos.Y, vector_pos.X) * (float)(180.0f / PI);
	z = 0;

	FVector target_rotation = FVector(X, Y, z);
	FVector new_aim_rotation;
	new_aim_rotation.X = target_rotation.X;
	new_aim_rotation.Y = target_rotation.Y;
	new_aim_rotation.Z = 0;

	FVector new_rotation = Math::SmoothAim(new_aim_rotation, ControlRotation, Globals::smooth);

	if (new_rotation.X < 0) {
		new_rotation.X += 360.f;
	}

	if (new_rotation.Y < 0) {
		new_rotation.Y += 360.f;
	}

	new_rotation.Z = 0;

	MyController->SetControlRotation(new_rotation);
}
void Draw3DSkeleton(UCanvas* Canvas, FVector start, FVector last, APlayerController* MYController, USkeletalMeshComponent* Mesh, FLinearColor Color)
{
	FVector2D bottom1;
	FVector2D bottom2;
	FVector2D bottom3;
	FVector2D bottom4;
	FVector2D top1;
	FVector2D top2;
	FVector2D top3;
	FVector2D top4;
	FVector vBaseBone = start;
	FVector vHeadBone = last;
	if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X + 4, vBaseBone.Y - 4, vBaseBone.Z), bottom1, 0) && bottom1.IsValid())
	{
		if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X - 4, vBaseBone.Y - 4, vBaseBone.Z), bottom2, 0) && bottom2.IsValid())
		{
			if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X - 4, vBaseBone.Y + 4, vBaseBone.Z), bottom3, 0) && bottom3.IsValid())
			{
				if (MYController->ProjectWorldLocationToScreen(FVector(vBaseBone.X + 4, vBaseBone.Y + 4, vBaseBone.Z), bottom4, 0) && bottom4.IsValid())
				{
					if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X + 4, vHeadBone.Y - 4, vHeadBone.Z + 8), top1, 0) && top1.IsValid())
					{
						if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X - 4, vHeadBone.Y - 4, vHeadBone.Z + 8), top2, 0) && top2.IsValid())
						{
							if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X - 4, vHeadBone.Y + 4, vHeadBone.Z + 8), top3, 0) && top3.IsValid())
							{
								if (MYController->ProjectWorldLocationToScreen(FVector(vHeadBone.X + 4, vHeadBone.Y + 4, vHeadBone.Z + 8), top4, 0) && top4.IsValid())
								{

									DrawLineSimple_3D(Canvas, bottom1, top1, Color);
									DrawLineSimple_3D(Canvas, bottom1, top1, Color);
									DrawLineSimple_3D(Canvas, bottom2, top2, Color);
									DrawLineSimple_3D(Canvas, bottom3, top3, Color);
									DrawLineSimple_3D(Canvas, bottom4, top4, Color);


									DrawLineSimple_3D(Canvas, bottom1, bottom2, Color);
									DrawLineSimple_3D(Canvas, bottom2, bottom3, Color);
									DrawLineSimple_3D(Canvas, bottom3, bottom4, Color);
									DrawLineSimple_3D(Canvas, bottom4, bottom1, Color);

									DrawLineSimple_3D(Canvas, top1, top2, Color);
									DrawLineSimple_3D(Canvas, top2, top3, Color);
									DrawLineSimple_3D(Canvas, top3, top4, Color);
									DrawLineSimple_3D(Canvas, top4, top1, Color);
								}
							}
						}
					}
				}
			}
		}

	}

}
void DrawSkeleton(APlayerController* MyController, USkeletalMeshComponent* Mesh, int bone_count, UCanvas* canvas) {
	struct BoneIDs {
		int hip, neck, upperArmLeft, leftHand, leftHand1, upperArmRight, rightHand, rightHand1;
		int leftThigh, leftCalf, leftFoot, rightThigh, rightCalf, rightFoot;
		int head;
	};

	BoneIDs boneIDs;

	switch (bone_count) {
	case 101:
		boneIDs = { 3, 21, 23, 24, 25, 49, 50, 51, 75, 76, 78, 82, 83, 85, 20 };
		break;
	case 103:
		boneIDs = { 3, 9, 33, 30, 32, 58, 55, 57, 63, 65, 69, 77, 79, 83, 8 };
		break;
	case 104:
		boneIDs = { 3, 21, 23, 24, 25, 49, 50, 51, 77, 78, 80, 84, 85, 87, 20 };
		break;
	default:
		return;
	}

	auto ProjectAndCheck = [&](int boneIndex, FVector2D& out) {
		return MyController->ProjectWorldLocationToScreen(GetBoneMatrix(Mesh, boneIndex), out, 0) && out.IsValid();
		};

	FVector2D vHipOut, vNeckOut, vUpperArmLeftOut, vLeftHandOut, vLeftHandOut1;
	FVector2D vUpperArmRightOut, vRightHandOut, vRightHandOut1;
	FVector2D vLeftThighOut, vLeftCalfOut, vLeftFootOut;
	FVector2D vRightThighOut, vRightCalfOut, vRightFootOut;
	FVector2D vHeadOut;

	if (ProjectAndCheck(boneIDs.hip, vHipOut) &&
		ProjectAndCheck(boneIDs.neck, vNeckOut) &&
		ProjectAndCheck(boneIDs.upperArmLeft, vUpperArmLeftOut) &&
		ProjectAndCheck(boneIDs.leftHand, vLeftHandOut) &&
		ProjectAndCheck(boneIDs.leftHand1, vLeftHandOut1) &&
		ProjectAndCheck(boneIDs.upperArmRight, vUpperArmRightOut) &&
		ProjectAndCheck(boneIDs.rightHand, vRightHandOut) &&
		ProjectAndCheck(boneIDs.rightHand1, vRightHandOut1) &&
		ProjectAndCheck(boneIDs.leftThigh, vLeftThighOut) &&
		ProjectAndCheck(boneIDs.leftCalf, vLeftCalfOut) &&
		ProjectAndCheck(boneIDs.leftFoot, vLeftFootOut) &&
		ProjectAndCheck(boneIDs.rightThigh, vRightThighOut) &&
		ProjectAndCheck(boneIDs.rightCalf, vRightCalfOut) &&
		ProjectAndCheck(boneIDs.rightFoot, vRightFootOut)) {

		canvas->K2_DrawLine(vHipOut, vNeckOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vUpperArmLeftOut, vNeckOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vUpperArmRightOut, vNeckOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vLeftHandOut, vUpperArmLeftOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vRightHandOut, vUpperArmRightOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vLeftHandOut, vLeftHandOut1, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vRightHandOut, vRightHandOut1, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vLeftThighOut, vHipOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vRightThighOut, vHipOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vLeftCalfOut, vLeftThighOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vRightCalfOut, vRightThighOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vLeftFootOut, vLeftCalfOut, 1, Colors::Skeleton);
		canvas->K2_DrawLine(vRightFootOut, vRightCalfOut, 1, Colors::Skeleton);
	}
	if (Globals::HeadESP)

	{
		// ✅ Draw Head Circle
		if (ProjectAndCheck(boneIDs.head, vHeadOut)) {
			const float radius = 4.0f;
			const int segments = 12;

			for (int i = 0; i < segments; ++i) {
				float theta1 = (2 * PI * i) / segments;
				float theta2 = (2 * PI * (i + 1)) / segments;

				FVector2D p1 = {
					vHeadOut.X + radius * cosf(theta1),
					vHeadOut.Y + radius * sinf(theta1)
				};

				FVector2D p2 = {
					vHeadOut.X + radius * cosf(theta2),
					vHeadOut.Y + radius * sinf(theta2)
				};

				canvas->K2_DrawLine(p1, p2, 1.5f, Colors::Skeleton);
			}
		}
	}

}



namespace G {
	UWorld* UWorldClass;
	UWorld* UWorldClass_2;
	AShooterCharacter* actor;
	APlayerCameraManager* MyCamera;
	APlayerController* controller;
	AAresEquippable* LastWeapon;
	AAresEquippable* MyWeapon;
	UObject* GameInstance;
	ULocalPlayer* LocalPlayers;
	ULocalPlayer* LocalPlayer;
	FVector2D pos = { ((float)GetSystemMetrics(SM_CXSCREEN) / 2) - 500, ((float)GetSystemMetrics(SM_CYSCREEN) / 2) - 475 };
	static bool recoil_hide_bool = true;
	static bool IsKnife = false;
	static bool alive = true;

	bool Mesh3PModifed = true;
	static bool pixel_time_bool;
	float spin_speed = 10.f;

}
void skills_names(std::string ObjectName, AShooterCharacter* MyShooter, UCanvas* canvas, FVector2D ObjectLocation) {

	static float font_size = 0.80f;

	if (ObjectName.find(("SmokeZone")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("Zone_Wraith_4_Smoke_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("Zone_Mage_Q_SphereShield_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sarge_4_SmokeManager_Production_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Pandemic_E_SmokeScreenManager_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Viper Wall Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Phoenix_Q_FlameWallManager_Production_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Flame Wall Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sprinter_4_Tunnel_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Neon Wall Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sequoia_4_MovingCover_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Iso Wall" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Rift_GlobalWall_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Astra Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Breach_4_FusionBlast_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Fusion Line" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Breach_4_SweetSpotFissure_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Fault Line" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Breach_X_Shockwave_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Breach Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sarge_E_SpeedStim_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Brim SpeedKit" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Deadeye_E_Trap_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Chamber Trap" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Deadeye_E_Teleporter_Tether_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Chamber Teleport" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Gumshoe_E_TripWire_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Cyper Trap" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Gumshoe_4_Cage_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Cyper Cage" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_CableJamRoot_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Deadlock JamRoot" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_StealthingTrap_SoundSensor_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Deadlock Trap" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_FishingHook_BouncingTrajectoryWarning_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Deadlock Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_BountyHunter_E_LoSReveal_Source_Reactivate_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Fade Hount" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Q_BountyHunter_Tether_SphereExpansion_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Fade Seize" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_BountyHunter_X_WaveForm_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Fade Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_BountyHunter_X_WaveForm_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Fade Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Mage_4_Wave_Chunk_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Harbor Wall" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Mage_E_WallManager_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Harbor Wall" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sequoia_Q_FragileMissile_TrajectoryWarning_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Iso UnderCut" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Grenadier_E_SuppressionPulse_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Kayo Point" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Killjoy_X_Bomb_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Killjoy Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sprinter_Q_ElectricSphere_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Neon RelayBlot" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Wraith_Q_NearsightMissile_TrajectoryWarning_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Omen Paranora" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Wraith_X_GlobalTeleport_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Omen Teleport" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Phoenix_X_ResTarget_Production_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Phoenix Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Phoenix_X_ResTarget_Production_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Phoenix Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Vampire_4_NearsightAOE_Source_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Reyna Blind" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Hunter_Q_SonarBolt_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Sova Arrow" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Hunter_4_ExplosiveBolt_Explosion_PrototypeBalance_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Sova Shock" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Gumshoe_E_TripWire_SecondWire_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Cyper Trap" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Gumshoe_E_TripWire_SecondWire_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Cyper Trap" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sarge_4_Smoke_ProductionNEW_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Smoke" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Sarge_4_OrbitalStrike_Production_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Brim Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Killjoy_4_BeeSwarm_Damage_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Killjoy Molly" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Aggrobot_Reclaim_Orb_SkederNade_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Iso Orb" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}

	// Clove
	if (ObjectName.find(("GameObject_Smonk_NewSmoke_C")) != std::string::npos) {
		if (!MyShooter->IsAlive())
			return;
		DrawEspText(canvas, FString{ L"Clove Ruse" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}

	if (ObjectName.find(("GameObject_Smonk_Q_DecayExplosion_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Clove Meddle" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}


	if (ObjectName.find(("GameObject_Gumshoe_Q_PossessableCamera_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Cyper Camera" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Killjoy_E_Turret_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Killjoy Turret" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Killjoy_Q_StealthAlarmbot_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Killjoy Alarmbot" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Clay_E_Boomba_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"RazeBot" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Aggrobot_SeekerNade_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Wingman" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("Ability_Guide_Q_PossessableScout_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Sky Kurd" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Stealth_4_Decoy_V2_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Yoru Klon" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Stealth_E_TeleporterMoving_FakeTP_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Yoru Teleport" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("Ability_Stealth_X_Cloak_Equip_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Yoru Ulti" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_BountyHunter_4_WolfHound_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Fade Prowler" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Hunter_E_Drone_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Sova Drone" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
	if (ObjectName.find(("GameObject_Rift_Q_FlashBurst_C")) != std::string::npos)
	{
		if (!MyShooter->IsAlive())
			return;

		DrawEspText(canvas, FString{ L"Astra FlashBurst" }, ObjectLocation, FVector2D{ font_size, font_size }, RGBtoFLC(255, 255, 255), 0.75f);
	}
}
void galaxychams(APawn* pawn, AShooterCharacter* shooter_character)
{
	// Cache material
	static UObject* material = nullptr;
	if (!material)
		material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Hellfire/1P_Hellfire_MI.1P_Hellfire_MI");

	if (!material || !Memory::IsValidPointer((uintptr_t)material))
		return;

	// Cache texture
	static UObject* CustomTexture = nullptr;
	if (!CustomTexture)
	{
		FString customTexturePath = FString(L"C:/galaxychams.jpg");
		CustomTexture = USceneComponentHelpers::ImportFileAsTexture2D(UWorldSave, customTexturePath);
	}

	if (!CustomTexture || !Memory::IsValidPointer((uintptr_t)CustomTexture))
		return;

	// Get meshes
	UPrimitiveComponent* mesh1p = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p);


	UPrimitiveComponent* meshOverlay = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p_overlay);


	// Apply material lambda
	auto applyMaterial = [&](UPrimitiveComponent* mesh)
		{
			if (!mesh) return;

			int numMaterials = mesh->GetNumMaterials();
			for (int i = 0; i < numMaterials; i++)
			{
				UObject* currentMat = mesh->GetMaterial(i);
				if (currentMat == material)
					continue; // Already applied

				UObject* dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, material);
				if (!dynMat) continue;

				auto mid = dynMat->cast<UMaterialInstanceDynamic>();
				if (!mid) continue;

				FLinearColor brightWhite = { 1.0f, 1.0f, 1.0f, 0.0f };
				std::vector<FString> textureParams = {
					L"Lava DF", L"Albedo", L"Lava MRS", L"Lava Normal",
					L"Lave AEM", L"AEM", L"MRS"
				};

				for (const auto& param : textureParams)
					mid->SetTextureParameterValue(kismentsystemlibrary::string_to_name(param), CustomTexture);

				FLinearColor glowColor = { 1.0f, 0.25f, 1.0f, 1.0f };

				mid->set_vector_parameter_value(kismentsystemlibrary::string_to_name(L"Emissive Color"), glowColor);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Emissive Intensity"), 30.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Lava Emissive Intensity"), 30.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"UV Scale"), 0.5f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Pan Speed"), 0.2f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"BP Lava Alpha"), 1.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"BP Emissive Lerp"), 1.0f);
			}
		};

	applyMaterial(mesh1p);
	applyMaterial(meshOverlay);
}
void sixseven(APawn* pawn, AShooterCharacter* shooter_character)
{
	// Cache material (use Ashen_Crystall_MI instead of v3)
	static UObject* material = nullptr;
	if (!material)
		material = UObject::StaticLoadObject(
			L"/Game/Equippables/_Core/Materials/SpecialMaterials/Ashen/Ashen_Crystall_MI.Ashen_Crystall_MI"
		);

	if (!material || !Memory::IsValidPointer((uintptr_t)material))
		return;

	// Cache custom texture (galaxy)
	static UObject* CustomTexture = nullptr;
	if (!CustomTexture)
	{
		FString customTexturePath = FString(L"C:/galaxychams.jpg");
		CustomTexture = USceneComponentHelpers::ImportFileAsTexture2D(UWorldSave, customTexturePath);
	}

	if (!CustomTexture || !Memory::IsValidPointer((uintptr_t)CustomTexture))
		return;

	// Get meshes
	UPrimitiveComponent* mesh1p = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p);
	UPrimitiveComponent* meshOverlay = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p_overlay);

	// Apply material lambda
	auto applyMaterial = [&](UPrimitiveComponent* mesh)
		{
			if (!mesh) return;

			int numMaterials = mesh->GetNumMaterials();
			for (int i = 0; i < numMaterials; i++)
			{
				UObject* dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, material);
				if (!dynMat) continue;

				auto mid = dynMat->cast<UMaterialInstanceDynamic>();
				if (!mid) continue;

				// === Scalars from Ashen_Crystall_MI JSON ===
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Diffuse Overlay Intensity"), 0.25f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Inner (B) Emissive Intenstiy"), 0.75f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Inner (G) Emissive Intenstiy"), 1.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Bump Offset Height"), -4.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Texture Tiling"), 3.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Reflection Vector Intensity"), 0.2f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Flow Map Speed Y"), 0.02f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Panner Y"), 0.03f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Diffuse Multiply"), 1.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Diffuse Power"), 1.45f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"RefractionDepthBias"), 0.0f);

				// === Vectors from JSON ===
				FLinearColor emissiveSurface = { 0.075f, 0.0f, 0.0f, 1.0f };
				FLinearColor diffuseTint = { 1.0f, 1.0f, 1.0f, 1.0f };

				mid->set_vector_parameter_value(kismentsystemlibrary::string_to_name(L"Emissive Surface Color"), emissiveSurface);
				mid->set_vector_parameter_value(kismentsystemlibrary::string_to_name(L"Diffuse Tint"), diffuseTint);

				// === Textures ===
				// Replace "Albedo" and "Emissive Gradient" with custom galaxy texture
				mid->SetTextureParameterValue(kismentsystemlibrary::string_to_name(L"Albedo"), CustomTexture);
				mid->SetTextureParameterValue(kismentsystemlibrary::string_to_name(L"Emissive Gradient"), CustomTexture);

				// Keep placeholders for other slots but point them to CustomTexture if you want solid galaxy look:
				std::vector<FString> otherSlots = { L"MRS", L"Normal", L"AEM", L"Emissive Gradient Reflection Vector" };
				for (const auto& param : otherSlots)
					mid->SetTextureParameterValue(kismentsystemlibrary::string_to_name(param), CustomTexture);
			}
		};

	applyMaterial(mesh1p);
	applyMaterial(meshOverlay);
}


void galaxychamsplayer(APawn* pawn, UPrimitiveComponent* Mesh)
{
	// Cache material
	static UObject* material = nullptr;
	if (!material)
		material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Hellfire/1P_Hellfire_MI.1P_Hellfire_MI");

	if (!material || !Memory::IsValidPointer((uintptr_t)material))
		return;

	// Cache texture
	static UObject* CustomTexture = nullptr;
	if (!CustomTexture)
	{
		FString customTexturePath = FString(L"C:/galaxychams.jpg");
		CustomTexture = USceneComponentHelpers::ImportFileAsTexture2D(UWorldSave, customTexturePath);
	}

	if (!CustomTexture || !Memory::IsValidPointer((uintptr_t)CustomTexture))
		return;


	// Apply material lambda
	auto applyMaterial = [&](UPrimitiveComponent* mesh)
		{
			if (!mesh) return;

			int numMaterials = mesh->GetNumMaterials();
			for (int i = 0; i < numMaterials; i++)
			{
				UObject* currentMat = mesh->GetMaterial(i);
				if (currentMat == material)
					continue; // Already applied

				UObject* dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, material);
				if (!dynMat) continue;

				auto mid = dynMat->cast<UMaterialInstanceDynamic>();
				if (!mid) continue;

				FLinearColor brightWhite = { 1.0f, 1.0f, 1.0f, 0.0f };
				std::vector<FString> textureParams = {
					L"Lava DF", L"Albedo", L"Lava MRS", L"Lava Normal",
					L"Lave AEM", L"AEM", L"MRS"
				};

				for (const auto& param : textureParams)
					mid->SetTextureParameterValue(kismentsystemlibrary::string_to_name(param), CustomTexture);

				FLinearColor glowColor = { 1.0f, 0.25f, 1.0f, 1.0f };

				mid->set_vector_parameter_value(kismentsystemlibrary::string_to_name(L"Emissive Color"), glowColor);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Emissive Intensity"), 30.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Lava Emissive Intensity"), 30.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"UV Scale"), 0.5f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"Pan Speed"), 0.2f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"BP Lava Alpha"), 1.0f);
				mid->set_scalar_parameter_value(kismentsystemlibrary::string_to_name(L"BP Emissive Lerp"), 1.0f);
			}
		};

	applyMaterial(Mesh);
}
void fresnel_handchams(APawn* pawn, AShooterCharacter* shooter_character)
{
	// Load material
	static UObject* material = UObject::StaticLoadObject(L"/Game/Characters/_Core/1P/1P_Material/SpecialMaterials/Iridescence/1P_Character_Mat_Iridescence.1P_Character_Mat_Iridescence");
	static FName baseColorName = kismentsystemlibrary::string_to_name(L"Fresnel Ally Tint");
	static FName emissiveColorName = kismentsystemlibrary::string_to_name(L"Fresnel Enemy Tint");
	static FName FresnelIntensity = kismentsystemlibrary::string_to_name(L"Fresnel Intensity");
	UPrimitiveComponent* mesh1p = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p);

	UPrimitiveComponent* meshOverlay = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p_overlay);


	// Determine color to apply
	FLinearColor colorToApply;
	if (Globals::rainbow_fresnel) {
		rainbowTimeZHandFresnel += 0.008f;
		colorToApply = BOSSRAINBOMAW(rainbowTimeZHandFresnel);
	}
	else {
		colorToApply = FLinearColor{
			fresnel.R,
			fresnel.G,
			fresnel.B
		};
	}	
	colorToApply = FLinearColor{
colorToApply.R * Globals::intesity,
colorToApply.G * Globals::intesity,
colorToApply.B * Globals::intesity,
colorToApply.A // leave alpha untouched, or adjust if needed
	};


	// Reusable lambda to apply dynamic materials
	auto applyMaterial = [&](UPrimitiveComponent* mesh) {
		if (!mesh) return;
		int numMaterials = mesh->GetNumMaterials();
		for (int i = 0; i < numMaterials; i++) {
			UObject* dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, material);
			if (!dynMat) continue;

			auto mid = dynMat->cast<UMaterialInstanceDynamic>();
			if (!mid) continue;

			mid->set_vector_parameter_value(baseColorName, colorToApply);
			mid->set_vector_parameter_value(emissiveColorName, colorToApply);
			mid->set_scalar_parameter_value(FresnelIntensity, Globals::Glow1);
		}
		};

	// Apply to both meshes
	applyMaterial(mesh1p);
	applyMaterial(meshOverlay);
}
void meshp1_material1(APawn* pawn, AShooterCharacter* shooter_character)
{
	// Load material
	static UObject* material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI");
	static FName baseColorName = kismentsystemlibrary::string_to_name(L"Base Color");
	static FName emissiveColorName = kismentsystemlibrary::string_to_name(L"Emissive Color");

	UPrimitiveComponent* mesh1p = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p);

	UPrimitiveComponent* meshOverlay = Memory::R<UPrimitiveComponent*>((uintptr_t)pawn + Offsets::mesh1p_overlay);


	// Determine color to apply
	FLinearColor colorToApply;
	if (Globals::handChamsRGB) {
		rainbowTimeZHandChams += 0.008f;
		colorToApply = BOSSRAINBOMAW(rainbowTimeZHandChams);
	}
	else {
		colorToApply = FLinearColor{
			Globals::handcolor.R,
			Globals::handcolor.G,
			Globals::handcolor.B
		};
	}
	colorToApply = FLinearColor{
colorToApply.R * 8,
colorToApply.G * 8,
colorToApply.B * 8,
colorToApply.A // leave alpha untouched, or adjust if needed
	};


	// Reusable lambda to apply dynamic materials
	auto applyMaterial = [&](UPrimitiveComponent* mesh) {
		if (!mesh) return;
		int numMaterials = mesh->GetNumMaterials();
		for (int i = 0; i < numMaterials; i++) {
			UObject* dynMat = mesh->create_and_set_material_instance_dynamic_from_material(i, material);
			if (!dynMat) continue;

			auto mid = dynMat->cast<UMaterialInstanceDynamic>();
			if (!mid) continue;

			mid->set_vector_parameter_value(baseColorName, colorToApply);
			mid->set_vector_parameter_value(emissiveColorName, colorToApply);
		}
		};

	// Apply to both meshes
	applyMaterial(mesh1p);
	applyMaterial(meshOverlay);
}
#include <vector>
#include <sstream>
#include <thread>










std::atomic<bool> g_TestOffsetsRunning(false);


static void partyhat(APlayerController* MyController, USkeletalMeshComponent* Mesh, UCanvas* canvas, FVector Bone)
{
	constexpr int segments = 24;              // More segments = smoother
	constexpr float baseRadius = 40.0f;       // Bigger cone
	constexpr float coneHeight = 12.0f;
	constexpr float headOffset = 10.0f;       // Sits nicely above head

	FVector baseCenter = Bone + FVector(0, 0, headOffset);
	FVector coneTip = baseCenter + FVector(0, 0, coneHeight);

	FVector2D screenTip;
	if (!MyController->ProjectWorldLocationToScreen(coneTip, screenTip, 0) || !screenTip.IsValid())
		return;

	static FLinearColor colors[segments] = {
		// 24-color smooth rainbow
		FLinearColor(1, 0, 0, 1), FLinearColor(1, 0.5, 0, 1), FLinearColor(1, 1, 0, 1),
		FLinearColor(0.5, 1, 0, 1), FLinearColor(0, 1, 0, 1), FLinearColor(0, 1, 0.5, 1),
		FLinearColor(0, 1, 1, 1), FLinearColor(0, 0.5, 1, 1), FLinearColor(0, 0, 1, 1),
		FLinearColor(0.5, 0, 1, 1), FLinearColor(1, 0, 1, 1), FLinearColor(1, 0, 0.5, 1),
		FLinearColor(0.9, 0.1, 0.2, 1), FLinearColor(0.8, 0.2, 0.4, 1), FLinearColor(0.6, 0.3, 0.7, 1),
		FLinearColor(0.3, 0.6, 0.9, 1), FLinearColor(0.2, 0.7, 0.8, 1), FLinearColor(0.1, 0.8, 0.5, 1),
		FLinearColor(0.1, 0.9, 0.3, 1), FLinearColor(0.2, 1.0, 0.1, 1), FLinearColor(0.6, 1.0, 0.0, 1),
		FLinearColor(1.0, 0.8, 0.0, 1), FLinearColor(1.0, 0.4, 0.0, 1), FLinearColor(1.0, 0.0, 0.0, 1),
	};

	const float angleStep = 2 * PI / segments;

	for (int i = 0; i < segments; ++i)
	{
		float angle1 = angleStep * i;
		float angle2 = angle1 + angleStep;

		FVector base1 = baseCenter + FVector(baseRadius * cos(angle1), baseRadius * sin(angle1), 0);
		FVector base2 = baseCenter + FVector(baseRadius * cos(angle2), baseRadius * sin(angle2), 0);

		FVector2D screen1, screen2;
		bool valid1 = MyController->ProjectWorldLocationToScreen(base1, screen1, 0) && screen1.IsValid();
		bool valid2 = MyController->ProjectWorldLocationToScreen(base2, screen2, 0) && screen2.IsValid();

		if (valid1 && valid2)
		{
			// Draw cone slice
			canvas->K2_DrawLine(screenTip, screen1, 1.5f, colors[i]);
			canvas->K2_DrawLine(screenTip, screen2, 1.5f, colors[i]);
			canvas->K2_DrawLine(screen1, screen2, 1.5f, colors[i]);

			// Optional: inner fill line
			FVector innerBase = baseCenter + FVector(baseRadius * cos((angle1 + angle2) * 0.5f), baseRadius * sin((angle1 + angle2) * 0.5f), 0);
			FVector2D screenMid;
			if (MyController->ProjectWorldLocationToScreen(innerBase, screenMid, 0) && screenMid.IsValid()) {
				canvas->K2_DrawLine(screenTip, screenMid, 1.2f, colors[i]);
			}
		}
	}
}








void meshp1_material(APawn* MyPawn, AShooterCharacter* characterz)
{
	UPrimitiveComponent* mesh = Memory::R<UPrimitiveComponent*>(uintptr_t(MyPawn) + Offsets::mesh1p);
	UPrimitiveComponent* meshOverlay = Memory::R<UPrimitiveComponent*>(uintptr_t(MyPawn) + Offsets::mesh1p_overlay);

	FString customTexturePath = FString(L"C:/antivgchandchams.jpg");
	UObject* CustomTexture = USceneComponentHelpers::ImportFileAsTexture2D(UWorldSave, customTexturePath);
	if (!CustomTexture || !Memory::IsValidPointer((uintptr_t)CustomTexture))
		return;

	static UObject* material = UObject::find_object(L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/Winter/Winter_MI.Winter_MI");
	if (!material) material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/Winter/Winter_MI.Winter_MI");
	if (!material || !Memory::IsValidPointer((uintptr_t)material))
		return;

	static UObject* SkyDynamicMat = nullptr;
	static UObject* SkyDynamicMat2 = nullptr;

	UObject* Material = nullptr;

	switch (g_Options.material)
	{
	case 0:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/3P_Weapon_Translucent_Mat");
		break;
	case 1:
		Material = UObject::StaticLoadObject(L"/Game/Characters/Vampire/S0/VFX/Ability_X/1P_Vampire_Tattoo_X_S0_MI_VFX.1P_Vampire_Tattoo_X_S0_MI_VFX");
		break;
	case 2:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Gems/1P_Gem_MAT");
		break;
	case 3:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat");
		break;
	case 4:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Sakura/Tritium_Sakura_3P_MI");
		break;
	case 5:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Yellow_MI.Arcade_Emissive_Yellow_MI");
		break;
	case 6:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Red_MI.Arcade_Emissive_Red_MI");
		break;
	case 7:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI");
		break;
	case 8:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Afterglow3/BakedLight/Afterglow3_BakedLight_MI.Afterglow3_BakedLight_MI");
		break;
	case 9:
		Material = UObject::StaticLoadObject(L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI");
		break;
	case 10:
		Material = UObject::StaticLoadObject(L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI");
		break;
	case 11:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Warring/1P_Melee_Warring_Emissive_Gradient_LUT_MI.1P_Melee_Warring_Emissive_Gradient_LUT_MI");
		break;
	case 12:
	{
		std::vector<FString> textureParams = {
			L"Image 1", L"Image 2"
		};

		mesh->SetMaterial(0, material);
		meshOverlay->SetMaterial(0, material);

		SkyDynamicMat = mesh->create_and_set_material_instance_dynamic_from_material(0, material);
		SkyDynamicMat2 = meshOverlay->create_and_set_material_instance_dynamic_from_material(0, material);

		if (!SkyDynamicMat || !SkyDynamicMat2 ||
			!Memory::IsValidPointer((uintptr_t)SkyDynamicMat) ||
			!Memory::IsValidPointer((uintptr_t)SkyDynamicMat2))
			return;

		auto* dynMat1 = SkyDynamicMat->cast<UMaterialInstanceDynamic>();
		auto* dynMat2 = SkyDynamicMat2->cast<UMaterialInstanceDynamic>();
		if (!dynMat1 || !dynMat2) return;

		for (const auto& param : textureParams)
		{
			FName texParam = kismentsystemlibrary::string_to_name(param);
			dynMat1->SetTextureParameterValue(texParam, CustomTexture);
			dynMat2->SetTextureParameterValue(texParam, CustomTexture);
		}

		return; // Don't override with other materials below
	}
	}

	// Only apply non-case-12 materials
	if (Material)
	{
		if (mesh && mesh->GetMaterial(0) != Material)
			mesh->SetMaterial(0, Material);

		if (meshOverlay && meshOverlay->GetMaterial(0) != Material)
			meshOverlay->SetMaterial(0, Material);
	}
}





void Gunp1_material(APawn* MyPawn, AShooterCharacter* characterz)
{
	UPrimitiveComponent* mesh = Memory::R<UPrimitiveComponent*>(uintptr_t(MyPawn) + Offsets::mesh1p);
	UPrimitiveComponent* meshOverlay = Memory::R<UPrimitiveComponent*>(uintptr_t(MyPawn) + Offsets::mesh1p_overlay);
	UPrimitiveComponent* gun = Memory::R<UPrimitiveComponent*>(uintptr_t(MyPawn) + Offsets::mesh1pgun);
	//  this two offset is for hands

	UObject* Material = nullptr;

	// Spoof only once before loading
   // yea so what u need exactly, HAND FRESNEL
	switch (g_Options.material) {
	case 0:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/3P_Weapon_Translucent_Mat");
		break;
	case 1:
		Material = UObject::StaticLoadObject(L"/Game/Characters/Vampire/S0/VFX/Ability_X/1P_Vampire_Tattoo_X_S0_MI_VFX.1P_Vampire_Tattoo_X_S0_MI_VFX");
		break;
	case 2:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Gems/1P_Gem_MAT");
		break;
	case 3:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat");
		break;
	case 4:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Sakura/Tritium_Sakura_3P_MI");
		break;
	case 5:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Yellow_MI.Arcade_Emissive_Yellow_MI");
		break;
	case 6:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Red_MI.Arcade_Emissive_Red_MI");
		break;
	case 7:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI");
		break;
	case 8:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Afterglow3/BakedLight/Afterglow3_BakedLight_MI.Afterglow3_BakedLight_MI");
		break;
	case 9:
		Material = UObject::StaticLoadObject(L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI");
		break;
	case 10:
		Material = UObject::StaticLoadObject(L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI");
		break;
	case 11:
		Material = UObject::StaticLoadObject(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Warring/1P_Melee_Warring_Emissive_Gradient_LUT_MI.1P_Melee_Warring_Emissive_Gradient_LUT_MI");
		break;

	}

	// Only apply if we loaded a valid material
	if (Material)
	{
		if (gun && gun->GetMaterial(0) != Material)
			gun->SetMaterial(0, Material);

		/*	if (meshOverlay && meshOverlay->GetMaterial(0) != Material)
				meshOverlay->SetMaterial(0, Material);*/
	}
}

// auto wall
#include <map>
#include <algorithm>
template<typename T>
T MaxSafe(T a, T b) {
	return (a > b) ? a : b;
}
template<typename T>
T MinSafe(T a, T b) {
	return (a < b) ? a : b;
}
float SafeThickness(float thickness) {
	if (!std::isfinite(thickness) || thickness < 0.0f || thickness > 100000.0f) {
		//printf("[AutoWall] !! Skipped invalid thickness: %f\n", thickness);
		return 0.0f;
	}
	return thickness;
}
namespace AutoWallSystem {
	struct WallPenetrationResult {
		bool CanPenetrate;
		bool IsLethal;
		float EstimatedDamage;
		int WallCount;
		float TotalWallThickness;
		FVector OptimalAimPoint;
		FWallSpanList WallSpans;
		FString HitBoneName;
	};

	struct WeaponPenetrationData {
		float HeadDamage;
		float BodyDamage;
		float LegDamage;
		float PenetrationPower;
		float DamageReductionPerWall;
		bool CanPenetrateWalls;
	};

	std::map<FString, WeaponPenetrationData> WeaponDatabase = {
		{ L"Vandal",     { 160.0f, 160.0f, 134.0f, 0.75f, 0.35f, true } },
		{ L"Phantom",    { 156.0f, 156.0f, 130.0f, 0.75f, 0.35f, true } },
		{ L"Operator",   { 255.0f, 150.0f, 120.0f, 0.90f, 0.25f, true } },
		{ L"Marshal",    { 202.0f, 101.0f,  85.0f, 0.85f, 0.30f, true } },
		{ L"Sheriff",    { 159.0f,  55.0f,  46.0f, 0.70f, 0.40f, true } },
		{ L"Guardian",   { 195.0f,  65.0f,  49.0f, 0.80f, 0.30f, true } },
		{ L"Outlaw",     { 140.0f, 140.0f, 119.0f, 0.78f, 0.32f, true } },
		{ L"Ghost",      { 105.0f,  30.0f,  25.0f, 0.65f, 0.45f, true } },
		{ L"Classic",    {  78.0f,  26.0f,  22.0f, 0.50f, 0.60f, false } },
		{ L"Shorty",     {  20.0f,  12.0f,  10.0f, 0.20f, 0.80f, false } },
		{ L"Frenzy",     {  78.0f,  26.0f,  21.0f, 0.55f, 0.55f, false } },
		{ L"Spectre",    {  78.0f,  26.0f,  22.0f, 0.60f, 0.50f, true } },
		{ L"Stinger",    {  67.0f,  27.0f,  22.0f, 0.58f, 0.52f, true } },
		{ L"Bucky",      {  40.0f,  20.0f,  17.0f, 0.30f, 0.70f, false } },
		{ L"Judge",      {  34.0f,  17.0f,  14.0f, 0.35f, 0.65f, false } },
		{ L"Bulldog",    { 115.0f,  35.0f,  29.0f, 0.68f, 0.38f, true } },
		{ L"Ares",       {  72.0f,  30.0f,  25.0f, 0.62f, 0.48f, true } },
		{ L"Odin",       {  95.0f,  38.0f,  32.0f, 0.72f, 0.36f, true } }
	};

	WeaponPenetrationData GetWeaponData(AShooterCharacter* Player, const FString& BoneName) {
		auto current_wep = Player->GetInventory()->GetCurrentWeapon();
		FString obj_name = helper::convert_weapon_name(kismentsystemlibrary::get_object_name(current_wep));

		auto it = WeaponDatabase.find(obj_name);
		if (it != WeaponDatabase.end()) {
			return it->second;
		}

		return { 100.0f, 35.0f, 25.0f, 0.60f, 0.40f, true };
	}

	float GetBoneDamageMultiplier(const FString& BoneName, const WeaponPenetrationData& WeaponData) {
		if (BoneName.contains(L"Head") || BoneName.contains(L"head"))
			return WeaponData.HeadDamage;
		else if (BoneName.contains(L"Spine") || BoneName.contains(L"Chest") || BoneName.contains(L"Pelvis"))
			return WeaponData.BodyDamage;
		else
			return WeaponData.LegDamage;
	}

	float CalculateWallThickness(const FWallSpanInfo& Span) {
		FVector entrance = Span.Entrance.Location;
		FVector exit = Span.Exit.Location;
		float distance = (exit - entrance).Size();

		FVector hit_normal = Span.Entrance.Normal;
		float angle_factor = std::abs(hit_normal.dot((exit - entrance).GetSafeNormal()));

		return distance * (1.0f + (1.0f - angle_factor) * 0.5f);
	}

	float CalculatePenetrationDamage(const WeaponPenetrationData& WeaponData, const FString& BoneName, float WallThickness, int WallCount) {
		if (!WeaponData.CanPenetrateWalls) return 0.0f;

		float base_damage = GetBoneDamageMultiplier(BoneName, WeaponData);

		float thickness_factor = MinSafe(WallThickness / 80.0f, 1.5f);
		float penetration_efficiency = WeaponData.PenetrationPower * (1.0f - thickness_factor * 0.3f);
		float damage_reduction = WeaponData.DamageReductionPerWall * (1.0f - penetration_efficiency);

		float final_damage = base_damage;
		for (int i = 0; i < WallCount; i++) {
			final_damage *= (1.0f - damage_reduction);

			if (WallCount >= 2) {
				final_damage *= 0.7f;
			}
		}

		float min_damage = base_damage * 0.15f;
		return MaxSafe(final_damage, min_damage);
	}

	FString GetBoneNameFromIndex(int bone_index) {
		switch (bone_index) {
		case 0: return L"Head";
		case 1: return L"Neck";
		case 2: return L"Body";
		default: return L"Body";
		}
	}

	WallPenetrationResult CalculateWallPenetration(AShooterCharacter* LocalPlayer, AShooterCharacter* Target, FVector TargetBone, int BoneIndex) {
		WallPenetrationResult Result = { false, false, 0.0f, 0, 0.0f, TargetBone };
		Result.HitBoneName = GetBoneNameFromIndex(BoneIndex);

		if (!LocalPlayer || !Target || !Target->IsAlive())
			return Result;

		FVector EyeLocation;
		FRotator EyeRotation;
		LocalPlayer->GetActorEyesViewPoint(&EyeLocation, &EyeRotation);
		FVector StartPos = EyeLocation;
		auto WeaponData = GetWeaponData(LocalPlayer, Result.HitBoneName);

		if (!WeaponData.CanPenetrateWalls) {
			return Result;
		}

		FWallSpanList WallSpans;
		TArray<UObject*> IgnoreActors = { LocalPlayer, Target };

		ShooterGameBlueprints::GetWallPenetrationSpans(
			UWorld::GetWorld(),
			StartPos,
			TargetBone,
			IgnoreActors,
			ECollisionChannel::ECC_WorldStatic,
			0	,
			WallSpans
		);

		Result.WallSpans = WallSpans;
		Result.WallCount = WallSpans.Spans.size();

		if (Result.WallCount > 0) {
			float TotalThickness = 0.0f;
			for (const auto& Span : WallSpans.Spans) {
				TotalThickness += CalculateWallThickness(Span);
			}
			Result.TotalWallThickness = TotalThickness;

			Result.EstimatedDamage = CalculatePenetrationDamage(
				WeaponData,
				Result.HitBoneName,
				TotalThickness,
				Result.WallCount
			);

			float TargetHealth = Target->GetHealth();
			Result.IsLethal = (Result.EstimatedDamage >= TargetHealth);

			float min_effective_damage = GetBoneDamageMultiplier(Result.HitBoneName, WeaponData) * 0.20f;
			Result.CanPenetrate = (Result.EstimatedDamage >= min_effective_damage);

			if (Result.WallCount > 0 && WallSpans.Spans.size() > 0) {
				FVector LastExit = WallSpans.Spans.back().Exit.Location;
				FVector WallNormal = WallSpans.Spans.back().Exit.Normal;
				FVector to_target = (TargetBone - LastExit).GetSafeNormal();
				Result.OptimalAimPoint = LastExit + (to_target * 5.0f);
			}
		}
		else {
			Result.CanPenetrate = true;
			Result.EstimatedDamage = GetBoneDamageMultiplier(Result.HitBoneName, WeaponData);
			Result.IsLethal = (Result.EstimatedDamage >= Target->GetHealth());
		}

		return Result;
	}

	bool ShouldShootThroughWalls(AShooterCharacter* LocalPlayer, AShooterCharacter* Target, FVector& BestAimPoint, FString& BestBoneName, int Mode) {
		if (!Target || !Target->IsAlive())
			return false;

		struct BonePenetration {
			FVector Bone;
			int BoneIndex;
			FString BoneName;
			WallPenetrationResult Result;
		};

		std::vector<BonePenetration> BoneChecks;

		USkeletalMeshComponent* Mesh = Target->get_mesh();
		if (!Mesh) return false;

		BoneChecks.push_back({ GetBoneMatrix(Mesh, 0), 0, L"Head", {} });
		BoneChecks.push_back({ GetBoneMatrix(Mesh, 1), 1, L"Neck", {} });
		BoneChecks.push_back({ GetBoneMatrix(Mesh, 2), 2, L"Body", {} });

		for (auto& BoneCheck : BoneChecks) {
			if (BoneCheck.Bone.IsValid()) {
				BoneCheck.Result = CalculateWallPenetration(LocalPlayer, Target, BoneCheck.Bone, BoneCheck.BoneIndex);
			}
		}

		WallPenetrationResult* BestResult = nullptr;

		if (Mode == 0) {
			for (auto& BoneCheck : BoneChecks) {
				if (BoneCheck.Result.CanPenetrate && BoneCheck.Result.IsLethal) {
					if (!BestResult || BoneCheck.Result.EstimatedDamage > BestResult->EstimatedDamage) {
						BestResult = &BoneCheck.Result;
						BestAimPoint = BoneCheck.Bone;
						BestBoneName = BoneCheck.BoneName;
					}
				}
			}

			if (!BestResult) {
				for (auto& BoneCheck : BoneChecks) {
					if (BoneCheck.Result.CanPenetrate && BoneCheck.Result.EstimatedDamage >= Globals::auto_wall_min_damage) {
						if (!BestResult || BoneCheck.Result.EstimatedDamage > BestResult->EstimatedDamage) {
							BestResult = &BoneCheck.Result;
							BestAimPoint = BoneCheck.Bone;
							BestBoneName = BoneCheck.BoneName;
						}
					}
				}
			}
		}
		else if (Mode == 1) {
			for (auto& BoneCheck : BoneChecks) {
				if (BoneCheck.Result.CanPenetrate) {
					if (!BestResult || BoneCheck.Result.EstimatedDamage > BestResult->EstimatedDamage) {
						BestResult = &BoneCheck.Result;
						BestAimPoint = BoneCheck.Bone;
						BestBoneName = BoneCheck.BoneName;
					}
				}
			}
		}

		return (BestResult != nullptr);
	}

}


