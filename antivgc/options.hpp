#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <map>
#include <array>

#define A( s ) #s
#define OPTION(type, var, val) type var = val

template <typename T = bool>
class Var {
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) {
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T* () { return &*value; }
	operator T() const { return *value; }
};

class Options {
public:
	OPTION(bool, hitbox_aiming, true);
	OPTION(bool, NoSpreadmode, false);
	OPTION(bool, distancesp, false);
	OPTION(bool, drawfov, false);
	OPTION(bool, auto360, false);
	OPTION(bool, recoilcontrol, false);
	OPTION(bool, SpikeEsp, false);
	OPTION(bool, SpikeRadiusEsp, false);
	OPTION(bool, Team_Check, false);


	OPTION(bool, TeamCheck, false);
	OPTION(bool, ChamsESP, false);
	OPTION(bool, engine_color, true);
	OPTION(bool, Always, false);
	OPTION(bool, Behind, true);
	OPTION(bool, RecoilCrosshair, false);
	OPTION(bool, Bhop, false);
	OPTION(bool, fovchanger, false);
	OPTION(bool, skeletonesp, false);
	OPTION(bool, EnableESP, true);

	OPTION(bool, Silent, false);
	OPTION(bool, Aimbot, true);
	OPTION(bool, aim360, false);
	OPTION(bool, FakeRecoilCrosshair, false);
	OPTION(bool, NoSpreadSilent, false);
	OPTION(bool, skybox, false);

	OPTION(bool, CrossHide, true);
	OPTION(bool, Dormant, true);
	OPTION(bool, VisibleBool, false);
	OPTION(bool, SpinBot, false);
	OPTION(bool, AutoShoot, false);
	OPTION(bool, Thirdperson, false);
	OPTION(bool, Thirdperson_local_chams, false);
	OPTION(bool, tp_camera_bool, false);
	OPTION(bool, corneredbox, false);
	OPTION(bool, IsKnifeBool, false);
	OPTION(bool, FovChangerBool, false);
	OPTION(bool, autoknife, false);
	OPTION(bool, aimlock, false);
	OPTION(bool, HeadESP, false);

	OPTION(bool, WeaponEsp, false);
	OPTION(bool, AgentNameESP, false);
	OPTION(bool, VisibleCheck, true);
	OPTION(bool, WireFrameHand, false);
	OPTION(bool, SkipTutorial, false);

	OPTION(bool, menu_opened, true);
	OPTION(bool, healthbar, false);
	OPTION(bool, shieldbar, false);
	OPTION(bool, tsgamerdoc, false);
	OPTION(bool, HealthText, false);
	OPTION(bool, box3d, false);
	OPTION(bool, box2d, false);

	OPTION(bool, snapline, false);
	OPTION(bool, t, true);
	OPTION(bool, spinbot, false);
	OPTION(bool, gadgetm_mode, false);
	OPTION(bool, WireFrameWeapon, false);
	OPTION(bool, kekIsOff, false);
	OPTION(bool, EnableSkin, false);

	OPTION(int, tpkey, 0);
	OPTION(int, AimKey, 0);
	OPTION(int, AimKey2, 0);
	OPTION(int, SkinKnife, 0);
	OPTION(int, SkinVandal, 0);
	OPTION(int, SkinSheriff, 0);
	OPTION(int, SkinOperator, 0);
	OPTION(int, SkinMarshal, 0);
	OPTION(int, SkinSpectre, 0);
	OPTION(int, SkinPhantom, 0);
	OPTION(int, SkinBulldog, 0);
	OPTION(int, SkinFrenzy, 0);

	OPTION(int, SkinGuardian, 0);
	OPTION(int, SkinGhost, 0);
	OPTION(int, SkinClassic, 0);
	OPTION(int, SkinChangerKey, 0);

	OPTION(int, material, 0);

	OPTION(float, fovchangurvalue, 105.0f);
	OPTION(float, AimRadius, 20.0f);
	OPTION(float, Smooth, 2.0f);
	OPTION(float, PlayerDistange, 400.0f);

};

inline Options g_Options;
inline bool   g_Unload;
int material;
