#pragma once
#include "canva.hpp"
#include "config.h"
#include <ShlObj.h>

//
//void SaveVandalTextTransforms() {
//    std::string desktopPath = std::getenv("USERPROFILE");
//    desktopPath += "\\Desktop\\weapon_transforms.txt";
//    std::ofstream file(desktopPath);
//    if (!file.is_open()) { printf("[-] Failed to save\n"); return; }
//
//    file << "=== VANDAL TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::text_scale_z << std::endl;
//
//    file << "=== FRENZY TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::frenzy_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::frenzy_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::frenzy_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::frenzy_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::frenzy_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::frenzy_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::frenzy_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::frenzy_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::frenzy_text_scale_z << std::endl;
//
//    file << "=== GHOST TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::ghost_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::ghost_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::ghost_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::ghost_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::ghost_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::ghost_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::ghost_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::ghost_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::ghost_text_scale_z << std::endl;
//
//    file << "=== PHANTOM TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::phantom_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::phantom_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::phantom_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::phantom_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::phantom_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::phantom_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::phantom_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::phantom_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::phantom_text_scale_z << std::endl;
//
//    file << "=== VANDAL MESH ===" << std::endl;
//    file << "scale_x=" << globals::misc::vandal_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::vandal_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::vandal_scale_z << std::endl;
//
//    file << "=== FRENZY MESH ===" << std::endl;
//    file << "scale_x=" << globals::misc::frenzy_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::frenzy_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::frenzy_scale_z << std::endl;
//
//    file << "=== GHOST MESH ===" << std::endl;
//    file << "scale_x=" << globals::misc::ghost_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::ghost_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::ghost_scale_z << std::endl;
//
//    file << "=== SPECTRE TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::spectre_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::spectre_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::spectre_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::spectre_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::spectre_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::spectre_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::spectre_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::spectre_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::spectre_text_scale_z << std::endl;
//
//    file.close();
//    printf("[+] Saved weapon_transforms.txt\n");
//}
//
//
//namespace burat {
//
//    fvector2d pos = { 960 - (490 / 2), 540 - (420 / 2) };
//    bool menu_open = true;
//    static flinearcolor fovcolor = { 255.0f, 255.0f, 255.0f, 1.0f };
//
//    void Menu(ucanvas* canvas)
//    {
//        menu::SetupCanvas(canvas);
//        menu::input::handle();
//
//        if (GetAsyncKeyState(VK_DELETE) & 1) menu_open = !menu_open;
//
//        if (menu::Window(L"Magicbullet", &pos, fvector2d(490, 420), menu_open))
//        {
//            static int tab = 0;
//
//            if (menu::ButtonTab2(L"Aimbot", fvector2d(50, 23), tab == 0)) tab = 0;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Visuals", fvector2d(50, 23), tab == 1)) tab = 1;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Chams", fvector2d(45, 23), tab == 2)) tab = 2;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Exploit", fvector2d(50, 23), tab == 3)) tab = 3;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Misc", fvector2d(35, 23), tab == 4)) tab = 4;
//
//           
//            if (tab == 0)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 1.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Auto Shot", fvector2d(65, 23), sub_tab == 2)) sub_tab = 2;
//
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Aimbot", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Enabled", &globals::aimbot::a1mbot);
//                    menu::SameLine();
//                    menu::Hotkey("Key", fvector2d(9, 18), &globals::aimbot::a1m_k3y);
//                    menu::Checkbox(L"Silent Aim", &globals::aimbot::silent);
//                    menu::Checkbox(L"Nospread", &globals::aimbot::nospread);
//                    menu::Checkbox(L"Autowall", &globals::aimbot::auto_wall);
//                    menu::Checkbox(L"Visible Check", &globals::aimbot::v1sh_ch3ck);
//                    menu::Checkbox(L"Recoil Control", &globals::aimbot::reco1l_contr0l);
//                    menu::Checkbox(L"Draw FOV", &globals::aimbot::draw_f0v);
//                    menu::Checkbox(L"360 FOV", &globals::aimbot::enable_360_fov);
//                    menu::SliderFloat(L"FOV", &globals::aimbot::a1m_f0v, 5.0f, 2000.0f, "%.1f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Smooth", &globals::aimbot::a1m_sm00th, 1.0f, 20.0f, "%.1f");
//                    menu::Combobox(fvector2d(154, 23), &globals::aimbot::a1m_b0ne,
//                        L"Head", L"Neck", L"Chest", NULL);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Settings", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    if (globals::aimbot::nospread) {
//                        globals::aimbot::reco1l_contr0l = false;
//                    }
//                }
//                else if (sub_tab == 2)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Auto Shot", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Auto Shot", &globals::aimbot::auto_shot);
//                    menu::Checkbox(L"Hold Aim Key", &globals::aimbot::auto_shot_hold_key);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Options", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//                }
//            }
//
//            else if (tab == 1)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 1.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"ESP", fvector2d(35, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"World", fvector2d(45, 23), sub_tab == 2)) sub_tab = 2;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Materials", fvector2d(60, 23), sub_tab == 3)) sub_tab = 3;
//
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Player ESP", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"2D Box", &globals::visuals::box2d);
//                    menu::Checkbox(L"3D Box", &globals::visuals::box3d);
//                    menu::Checkbox(L"Corner Box", &globals::visuals::corner);
//                    menu::Checkbox(L"Vis Check", &globals::visuals::vischeck);
//                    menu::Checkbox(L"Head ESP", &globals::visuals::headb0x);
//                    menu::Checkbox(L"Health Bar", &globals::visuals::h3althbar);
//                    menu::Checkbox(L"Skeleton", &globals::visuals::sk3let0n);
//                    menu::Checkbox(L"Snapline", &globals::visuals::snapl1ne);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Info ESP", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Agent Icon", &globals::visuals::b00ms);
//                    menu::Checkbox(L"Weapon Info", &globals::visuals::b11ms);
//                    menu::Checkbox(L"Distance", &globals::visuals::dstc);
//                    menu::Checkbox(L"Spike Info", &globals::visuals::spike);
//                    menu::Checkbox(L"China Hat", &globals::visuals::chinahat);
//                    menu::Checkbox(L"China Hat Self", &globals::visuals::partyhat_self);
//
//                    if (globals::visuals::corner) {
//                        globals::visuals::box2d = false;
//                        globals::visuals::box3d = false;
//                    }
//                    if (globals::visuals::box3d) {
//                        globals::visuals::corner = false;
//                        globals::visuals::box2d = false;
//                    }
//                    if (globals::visuals::box2d) {
//                        globals::visuals::corner = false;
//                        globals::visuals::box3d = false;
//                    }
//                }
//                else if (sub_tab == 2)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"World", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"No Smoke", &globals::misc::no_smoke);
//                    menu::Checkbox(L"Skybox", &globals::misc::skybox);
//                    menu::Checkbox(L"Skybox RGB", &globals::misc::skyboxrgb);
//                    menu::Checkbox(L"Bullet Tracer", &globals::misc::bullet_tracers);
//
//                    menu::SliderFloat(L"Stars", &globals::misc::StarsBrightness, 0.f, 2500.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cloud Speed", &globals::misc::CloudSpeed, 0.f, 50.f, "%.1f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cloud Opacity", &globals::misc::CloudOpacity, 0.f, 10000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cloud op multi", &globals::misc::CloudOpacity1, 0.f, 100000.f, "%.0f");
//                    menu::offset_y -= 16;
//
//                    menu::SliderFloat(L"Sky R", &globals::misc::SkySharedR, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Sky G", &globals::misc::SkySharedG, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Sky B", &globals::misc::SkySharedB, 0.f, 1.f, "%.2f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Wireframe", fvector2d(228, 145));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Enemy Wire", &globals::misc::Wireframe);
//                    menu::Checkbox(L"Self Wire", &globals::misc::self_wireframe);
//                    menu::Checkbox(L"Hand Wire", &globals::misc::HandWire);
//                    menu::Checkbox(L"Gun Wire", &globals::misc::WireframeGun);
//                    menu::Checkbox(L"Gun 3P Wire", &globals::misc::gun_3p_wireframe);
//
//                    // Fog section
//                    menu::offset_x = 250;
//                    menu::offset_y = 215;
//                    menu::SectionWrapper(L"Fog", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Fog", &globals::misc::Fog);
//                    menu::Checkbox(L"Fog", &globals::misc::FogRGB);
//                    menu::Checkbox(L"Volumetric Fog", &globals::misc::bEnableVolumetricFog);
//                    menu::SliderFloat(L"Density", &globals::misc::FogDensity, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Falloff", &globals::misc::FogHeightFalloff, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Opacity", &globals::misc::FogMaxOpacity, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Start Dist", &globals::misc::FogStartDistance, 0.f, 50000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cutoff", &globals::misc::FogCutoffDistance, 0.f, 50000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Vol Dist", &globals::misc::VolumetricFogDistance, 0.f, 20000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    // Fog color sliders
//                    menu::SliderFloat(L"Fog R", &globals::misc::FogColor.r, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Fog G", &globals::misc::FogColor.g, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Fog B", &globals::misc::FogColor.b, 0.f, 1.f, "%.2f");
//                }
//                else if (sub_tab == 3)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Materials", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Material Hand", &globals::visuals::hand_with_material);
//                    menu::Checkbox(L"Material 1P Gun", &globals::visuals::gunmaterial1p);
//                    menu::Checkbox(L"Material 3P Gun", &globals::visuals::gunmaterial3p);
//                    menu::Checkbox(L"Material Skin 3P", &globals::misc::playerchamsself);
//                    menu::Combobox(fvector2d(154, 23), &globals::visuals::typehand,
//                        L"Black", L"Reyna", L"Green", L"Glass", L"Pink", L"Yellow", L"Red", L"Blue", NULL);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Gun Types", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Combobox(fvector2d(154, 23), &globals::visuals::typegun1p,
//                        L"Glass", L"Red", L"Blue", L"Yellow", NULL);
//                    menu::Combobox(fvector2d(154, 23), &globals::visuals::typegun3d,
//                        L"Glass", L"Red", L"Blue", L"Yellow", NULL);
//                    menu::Combobox(fvector2d(154, 23), &globals::misc::chams_material_index,
//                        L"Ninja", L"Cyber", L"Soviet", L"Sea", NULL);
//                }
//            }
//
//            // ==================== CHAMS TAB ====================
//            else if (tab == 2)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 1.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"Outline", fvector2d(50, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Glow", fvector2d(40, 23), sub_tab == 2)) sub_tab = 2;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Galaxy", fvector2d(50, 23), sub_tab == 3)) sub_tab = 3;
//
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Outline Chams", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Outline Chams", &globals::chams::outline_enabled);
//                    menu::Checkbox(L"Hand Outline", &globals::chams::hand_outline_enabled);
//                    menu::Checkbox(L"Self Chams", &globals::chams::self_chams);
//                    menu::Checkbox(L"Gun 1P Chams", &globals::chams::gun_outline1P_enabled);
//                    menu::Checkbox(L"Gun 3P Chams", &globals::chams::gun_outline3P_enabled);
//                    menu::SliderFloat(L"Vis Intensity", &globals::chams::intensityvisibleoutline, 10, 100, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Invis Intensity", &globals::chams::intensityinvisbleoutline, 10, 100, "%.0f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Presets", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Combobox(fvector2d(154, 23), &globals::chams::outlinetype,
//                        L"Always", L"Behind", NULL);
//                    menu::Combobox(fvector2d(154, 23), &globals::chams::visible_outline_preset,
//                        L"Galaxy", L"Blue", L"Green", L"Orange", L"Pink", L"White", NULL);
//                    menu::Combobox(fvector2d(154, 23), &globals::chams::invisible_outline_preset,
//                        L"Red", L"Orange", L"Yellow", L"Green", L"Pink", L"Gray", NULL);
//                }
//                else if (sub_tab == 2)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Visible Chams", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Visible Chams", &globals::chams::rchamsespall);
//                    menu::SliderFloat(L"Intensity", &globals::chams::Glow, 0.1f, 10.0f, "%.1f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Invisible Chams", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Invisible Chams", &globals::chams::rchamsesp);
//                    menu::SliderFloat(L"Intensity", &globals::chams::Glowvni, 0.1f, 10.0f, "%.1f");
//                }
//                else if (sub_tab == 3)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Galaxy Chams", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Hand Galaxy", &globals::chams::hand_galaxy_enabled);
//                    menu::Checkbox(L"Gun 1P Galaxy", &globals::chams::gun1p_galaxy_enabled);
//                    menu::Checkbox(L"Gun 3P Galaxy", &globals::chams::gun3p_galaxy_enabled);
//                    menu::Checkbox(L"Self Galaxy", &globals::chams::self_galaxy_enabled);
//                    menu::Checkbox(L"Enemy Galaxy", &globals::chams::enemy_galaxy_enabled);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Crystal Preset", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Combobox(fvector2d(154, 23), &globals::visuals::crystal_chams_preset,
//                        L"Custom", L"Red", L"Green", L"Blue", L"Orange", L"Pink", L"Purple", NULL);
//                }
//            }
//
//            
//            else if (tab == 3)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 1.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Skins", fvector2d(40, 23), sub_tab == 2)) sub_tab = 2;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"AA", fvector2d(25, 23), sub_tab == 3)) sub_tab = 3;
//
//               
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Movement", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Bunny Hop", &globals::misc::bhop);
//                    menu::Checkbox(L"Fast Crouch", &globals::misc::fastcrouch);
//                    menu::Checkbox(L"Anti Flash", &globals::misc::antiflash);
//                    menu::Checkbox(L"Third Person", &globals::misc::tperson);
//                    menu::SliderFloat(L"TP Distance", &globals::misc::PlayerDistance, 100, 1000, "%.0f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"View", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"FOV Changer", &globals::misc::fovchanger);
//                    menu::SliderFloat(L"FOV", &globals::misc::fovchangur, 0, 170, "%.0f");
//                    menu::Checkbox(L"Aspect Ratio", &globals::misc::aspectratio);
//                    menu::Checkbox(L"View Model", &globals::misc::ViewModelChanger);
//                    if (menu::Button(L"Skip Tutorial")) globals::misc::sk1ptut0rial = true;
//                    if (menu::Button(L"Unlock ALL"))    globals::misc::sk1n_chang3r = true;
//                }
//
//                else if (tab == 3)
//                {
//                    static int sub_tab = 1;
//
//                    menu::offset_x = 1.0f;
//                    menu::offset_y = 2.0f + 23.0f;
//
//                    if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
//                    menu::SameLine();
//                    if (menu::ButtonTab2(L"Skins", fvector2d(40, 23), sub_tab == 2)) sub_tab = 2;
//                    menu::SameLine();
//                    if (menu::ButtonTab2(L"AA", fvector2d(25, 23), sub_tab == 3)) sub_tab = 3;
//
//                    // ══ MAIN ══════════════════════════════════════════════════
//                    if (sub_tab == 1)
//                    {
//                        menu::offset_x = 12;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Movement", fvector2d(228, 312));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Bunny Hop", &globals::misc::bhop);
//                        menu::Checkbox(L"Fast Crouch", &globals::misc::fastcrouch);
//                        menu::Checkbox(L"Anti Flash", &globals::misc::antiflash);
//                        menu::Checkbox(L"Third Person", &globals::misc::tperson);
//                        menu::SliderFloat(L"TP Distance", &globals::misc::PlayerDistance, 100, 1000, "%.0f");
//
//                        menu::offset_x = 250;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"View", fvector2d(228, 312));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"FOV Changer", &globals::misc::fovchanger);
//                        menu::SliderFloat(L"FOV", &globals::misc::fovchangur, 0, 170, "%.0f");
//                        menu::Checkbox(L"Aspect Ratio", &globals::misc::aspectratio);
//                        menu::Checkbox(L"View Model", &globals::misc::ViewModelChanger);
//                        if (menu::Button(L"Skip Tutorial")) globals::misc::sk1ptut0rial = true;
//                        if (menu::Button(L"Unlock ALL"))    globals::misc::sk1n_chang3r = true;
//                    }
//
//                    else if (sub_tab == 2)
//                    {
//                        menu::offset_x = 12;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Skins", fvector2d(228, 312));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Custom Skin", &globals::misc::custom_vandal_enabled);
//                        globals::misc::custom_text_enabled = globals::misc::custom_vandal_enabled;
//                        menu::Checkbox(L"Custom Text", &globals::misc::custom_text_enabled);
//
//                        menu::offset_y -= 8;
//                        menu::SliderFloat(L"Sc X", &globals::misc::vandal_scale_x, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sc Y", &globals::misc::vandal_scale_y, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sc Z", &globals::misc::vandal_scale_z, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Fr Sc X", &globals::misc::frenzy_scale_x, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Fr Sc Y", &globals::misc::frenzy_scale_y, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Fr Sc Z", &globals::misc::frenzy_scale_z, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Gh Sc X", &globals::misc::ghost_scale_x, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Gh Sc Y", &globals::misc::ghost_scale_y, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Gh Sc Z", &globals::misc::ghost_scale_z, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Sp Rot P", &globals::misc::spectre_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot Y", &globals::misc::spectre_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot R", &globals::misc::spectre_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos X", &globals::misc::spectre_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Y", &globals::misc::spectre_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Z", &globals::misc::spectre_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//
//
//
//                        if (menu::Button(L"Save All")) SaveVandalTextTransforms();
//
//                        menu::offset_x = 250;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Text Transform", fvector2d(228, 312));
//                        menu::offset_y -= 11;
//
//                        menu::SliderFloat(L"V Pos X", &globals::misc::text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Pos Y", &globals::misc::text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Pos Z", &globals::misc::text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Rot P", &globals::misc::text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Rot Y", &globals::misc::text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Rot R", &globals::misc::text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Sc X", &globals::misc::text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Sc Y", &globals::misc::text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Sc Z", &globals::misc::text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"F Pos X", &globals::misc::frenzy_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Pos Y", &globals::misc::frenzy_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Pos Z", &globals::misc::frenzy_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Rot P", &globals::misc::frenzy_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Rot Y", &globals::misc::frenzy_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Rot R", &globals::misc::frenzy_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Sc X", &globals::misc::frenzy_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Sc Y", &globals::misc::frenzy_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Sc Z", &globals::misc::frenzy_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"G Pos X", &globals::misc::ghost_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Pos Y", &globals::misc::ghost_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Pos Z", &globals::misc::ghost_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Rot P", &globals::misc::ghost_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Rot Y", &globals::misc::ghost_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Rot R", &globals::misc::ghost_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Sc X", &globals::misc::ghost_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Sc Y", &globals::misc::ghost_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Sc Z", &globals::misc::ghost_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Ph Pos X", &globals::misc::phantom_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Pos Y", &globals::misc::phantom_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Pos Z", &globals::misc::phantom_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Rot P", &globals::misc::phantom_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Rot Y", &globals::misc::phantom_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Rot R", &globals::misc::phantom_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Sc X", &globals::misc::phantom_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Sc Y", &globals::misc::phantom_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Sc Z", &globals::misc::phantom_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//
//                        menu::SliderFloat(L"Sp Pos X", &globals::misc::spectre_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Y", &globals::misc::spectre_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Z", &globals::misc::spectre_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot P", &globals::misc::spectre_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot Y", &globals::misc::spectre_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot R", &globals::misc::spectre_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Sc X", &globals::misc::spectre_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Sc Y", &globals::misc::spectre_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Sc Z", &globals::misc::spectre_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                    }
//                    // ══ ANTI-AIM ══════════════════════════════════════════════
//                    else if (sub_tab == 3)
//                    {
//                        menu::offset_x = 12;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Anti-Aim", fvector2d(228, 312));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Enable (F1)", &globals::misc::SpinBot);
//                        menu::Combobox(fvector2d(154, 23), &globals::misc::pitch_mode,
//                            L"None", L"Up", L"Down", L"Custom", NULL);
//                        menu::Checkbox(L"Spin", &globals::misc::aa_spin);
//                        menu::Checkbox(L"Jitter", &globals::misc::aa_jitter);
//                        menu::Checkbox(L"Desync", &globals::misc::aa_desync);
//                        menu::Checkbox(L"Backwards", &globals::misc::aa_backwards);
//                        menu::Checkbox(L"Atomic AA", &globals::misc::aa_atomic);
//                        menu::SliderFloat(L"Spin Speed", &globals::misc::spinvalue, 1.0f, 50.0f, "%.1f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Yaw Offset", &globals::misc::yaw_add, -180.0f, 180.0f, "%.1f");
//
//                        menu::offset_x = 250;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Snap Keys", fvector2d(228, 312));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Three-Way", &globals::misc::aa_threeway);
//                        menu::Checkbox(L"Pred Breaker", &globals::misc::aa_prediction_breaker);
//                        menu::Hotkey("Left", fvector2d(9, 18), &globals::misc::snap_left_key);
//                        menu::Hotkey("Right", fvector2d(9, 18), &globals::misc::snap_right_key);
//                        menu::Hotkey("Back", fvector2d(9, 18), &globals::misc::snap_back_key);
//                    }
//                }
//
//
//                // ══ ANTI-AIM ════════════════════════════════════════════════
//                else if (sub_tab == 3)
//                {
//                    menu::offset_x = 12;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Anti-Aim", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Enable (F1)", &globals::misc::SpinBot);
//                    menu::Combobox(fvector2d(154, 23), &globals::misc::pitch_mode,
//                        L"None", L"Up", L"Down", L"Custom", NULL);
//                    menu::Checkbox(L"Spin", &globals::misc::aa_spin);
//                    menu::Checkbox(L"Jitter", &globals::misc::aa_jitter);
//                    menu::Checkbox(L"Desync", &globals::misc::aa_desync);
//                    menu::Checkbox(L"Backwards", &globals::misc::aa_backwards);
//                    menu::Checkbox(L"Atomic AA", &globals::misc::aa_atomic);
//                    menu::SliderFloat(L"Spin Speed", &globals::misc::spinvalue, 1.0f, 50.0f, "%.1f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Yaw Offset", &globals::misc::yaw_add, -180.0f, 180.0f, "%.1f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Snap Keys", fvector2d(228, 312));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Three-Way", &globals::misc::aa_threeway);
//                    menu::Checkbox(L"Pred Breaker", &globals::misc::aa_prediction_breaker);
//                    menu::Hotkey("Left", fvector2d(9, 18), &globals::misc::snap_left_key);
//                    menu::Hotkey("Right", fvector2d(9, 18), &globals::misc::snap_right_key);
//                    menu::Hotkey("Back", fvector2d(9, 18), &globals::misc::snap_back_key);
//                }
//            }
//
//           
//      else if (tab == 4)
//      {
//          static int sub_tab = 1;
//          menu::offset_x = 1.0f;
//          menu::offset_y = 2.0f + 23.0f;
//          if (menu::ButtonTab2(L"Config", fvector2d(45, 23), sub_tab == 1)) sub_tab = 1;
//          menu::SameLine();
//          if (menu::ButtonTab2(L"Sounds", fvector2d(50, 23), sub_tab == 2)) sub_tab = 2;
//          menu::SameLine();
//          if (menu::ButtonTab2(L"Misc", fvector2d(40, 23), sub_tab == 3)) sub_tab = 3;
//
//          // ── Config ────────────────────────────────────────────────────────
//          if (sub_tab == 1)
//          {
//              menu::offset_x = 12;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Config", fvector2d(228, 312));
//              menu::offset_y -= 11;
//              if (menu::Button(L"Save Config")) {
//                  std::filesystem::path configDir = "C:/MagicBullet";
//                  std::filesystem::path configPath = configDir / "config.json";
//                  if (!std::filesystem::exists(configDir))
//                      std::filesystem::create_directories(configDir);
//                  Config->SaveSettings(configPath.string());
//              }
//              if (menu::Button(L"Load Config"))
//                  Config->LoadSettings("C:/MagicBullet/config.json");
//              if (menu::Button(L"Discord"))
//                  system("start https://discord.gg/magicbullet");
//              menu::Checkbox(L"Watermark", &globals::Watermark);
//
//              menu::offset_x = 250;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Gun Buddy", fvector2d(228, 312));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Gun Buddy", &globals::buddy::enabled);
//              menu::SliderInt(L"Buddy ID", &globals::buddy::index, 0, 658, "%d");
//          }
//
//          // ── Sounds ────────────────────────────────────────────────────────
//          else if (sub_tab == 2)
//          {
//              menu::offset_x = 12;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Kill Sound", fvector2d(228, 312));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Kill Sound", &globals::misc::killsound);
//
//              menu::offset_x = 250;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Kill Say", fvector2d(228, 312));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Kill Say", &globals::misc::killsays);
//
//             
//              menu::InputField(L"Chat message", &globals::misc::chat_message, 256);
//          }
//
//         
//          else if (sub_tab == 3)
//          {
//              menu::offset_x = 12;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Finisher", fvector2d(228, 312));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Finisher", &globals::misc::finisher);
//              menu::Checkbox(L"Last Kill Only", &globals::misc::only_last_kill);
//  
//          }
//}
//            fvector2d cursorPos = menu::CursorPos();
//            menu::drawFilledRect(fvector2d(cursorPos.x - 2, cursorPos.y - 2), 4, 4, flinearcolor(1, 1, 1, 1));
//        }
//
//        menu::Render();
//    }
//}

namespace burat {

    fvector2d pos = { 960 - (490 / 2), 540 - (420 / 2) };
    bool menu_open = true;
    static flinearcolor fovcolor = { 255.0f, 255.0f, 255.0f, 1.0f };

    void Menu(ucanvas* canvas)
    {
        menu::SetupCanvas(canvas);
        menu::input::handle();

        if (GetAsyncKeyState(VK_DELETE) & 1) menu_open = !menu_open;

        if (menu::Window(L"Magicbullet", &pos, fvector2d(490, 420), menu_open))
        {
            static int tab = 0;

            if (menu::ButtonTab2(L"Aimbot", fvector2d(50, 23), tab == 0)) tab = 0;
            menu::SameLine();
            if (menu::ButtonTab2(L"Visuals", fvector2d(50, 23), tab == 1)) tab = 1;
            menu::SameLine();
            if (menu::ButtonTab2(L"Chams", fvector2d(45, 23), tab == 2)) tab = 2;
            menu::SameLine();
            if (menu::ButtonTab2(L"Misc", fvector2d(35, 23), tab == 3)) tab = 3;

            // ════════════════════════════════════════════════════
            // TAB 0 — AIMBOT
            // ════════════════════════════════════════════════════
            if (tab == 0)
            {
                static int sub_tab = 1;
                menu::offset_x = 1.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(L"Auto Shot", fvector2d(65, 23), sub_tab == 2)) sub_tab = 2;

                if (sub_tab == 1)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Aimbot", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Enabled", &globals::aimbot::a1mbot);
                    menu::SameLine();
                    menu::Hotkey("Key", fvector2d(9, 18), &globals::aimbot::a1m_k3y);
                    menu::Checkbox(L"Silent Aim", &globals::aimbot::silent);
                    menu::Checkbox(L"Nospread", &globals::aimbot::nospread);
                    menu::Checkbox(L"Autowall", &globals::aimbot::auto_wall);
                    menu::Checkbox(L"Visible Check", &globals::aimbot::v1sh_ch3ck);
                    menu::Checkbox(L"Recoil Control", &globals::aimbot::reco1l_contr0l);
                    menu::Checkbox(L"Draw FOV", &globals::aimbot::draw_f0v);
                    menu::Checkbox(L"360 FOV", &globals::aimbot::enable_360_fov);

                    if (globals::aimbot::nospread)
                        globals::aimbot::reco1l_contr0l = false;

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Settings", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::SliderFloat(L"FOV", &globals::aimbot::a1m_f0v, 5.0f, 2000.0f, "%.1f");
                    menu::offset_y -= 16;
                    menu::SliderFloat(L"Smooth", &globals::aimbot::a1m_sm00th, 1.0f, 20.0f, "%.1f");
                    menu::Combobox(fvector2d(154, 23), &globals::aimbot::a1m_b0ne,
                        L"Head", L"Neck", L"Chest", NULL);
                }
                else if (sub_tab == 2)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Auto Shot", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Auto Shot", &globals::aimbot::auto_shot);
                    menu::Checkbox(L"Hold Aim Key", &globals::aimbot::auto_shot_hold_key);

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Options", fvector2d(228, 312));
                    menu::offset_y -= 11;
                    
                    menu::Checkbox(L"Prediction", &globals::aimbot::prediction);
                }
            }

            // ════════════════════════════════════════════════════
            // TAB 1 — VISUALS
            // ════════════════════════════════════════════════════
            else if (tab == 1)
            {
                static int sub_tab = 1;
                menu::offset_x = 1.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(L"ESP", fvector2d(35, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(L"World", fvector2d(45, 23), sub_tab == 2)) sub_tab = 2;
                menu::SameLine();
                if (menu::ButtonTab2(L"Materials", fvector2d(60, 23), sub_tab == 3)) sub_tab = 3;

                if (sub_tab == 1)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Player ESP", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"2D Box", &globals::visuals::box2d);
                    menu::Checkbox(L"3D Box", &globals::visuals::box3d);
                    menu::Checkbox(L"Corner Box", &globals::visuals::corner);
                    menu::Checkbox(L"Vis Check", &globals::visuals::vischeck);
                    menu::Checkbox(L"Head ESP", &globals::visuals::headb0x);
                    menu::Checkbox(L"Health Bar", &globals::visuals::h3althbar);
                    menu::Checkbox(L"Skeleton", &globals::visuals::sk3let0n);
                    menu::Checkbox(L"Snapline", &globals::visuals::snapl1ne);

                    if (globals::visuals::corner) { globals::visuals::box2d = false; globals::visuals::box3d = false; }
                    if (globals::visuals::box3d) { globals::visuals::corner = false; globals::visuals::box2d = false; }
                    if (globals::visuals::box2d) { globals::visuals::corner = false; globals::visuals::box3d = false; }

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Info ESP", fvector2d(228, 145));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Agent Icon", &globals::visuals::b00ms);
                    menu::Checkbox(L"Weapon Info", &globals::visuals::b11ms);
                    menu::Checkbox(L"Distance", &globals::visuals::dstc);
                    menu::Checkbox(L"Spike Info", &globals::visuals::spike);
                    menu::Checkbox(L"China Hat", &globals::visuals::chinahat);
                    menu::Checkbox(L"China Hat Self", &globals::visuals::partyhat_self);

                    menu::offset_x = 250;
                    menu::offset_y = 215;
                    menu::SectionWrapper(L"Wireframe", fvector2d(228, 155));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Enemy Wire", &globals::misc::Wireframe);
                    menu::Checkbox(L"Self Wire", &globals::misc::self_wireframe);
                    menu::Checkbox(L"Hand Wire", &globals::misc::HandWire);
                    menu::Checkbox(L"Gun Wire", &globals::misc::WireframeGun);
                    menu::Checkbox(L"Gun 3P Wire", &globals::misc::gun_3p_wireframe);
                    menu::Checkbox(L"Bullet Tracer", &globals::misc::bullet_tracers);
                }
                else if (sub_tab == 2)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Skybox", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"No Smoke", &globals::misc::no_smoke);
                    menu::Checkbox(L"Skybox", &globals::misc::skybox);
                    menu::Checkbox(L"Skybox RGB", &globals::misc::skyboxrgb);

                    menu::SliderFloat(L"Stars", &globals::misc::StarsBrightness, 0.f, 2500.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Cloud Speed", &globals::misc::CloudSpeed, 0.f, 50.f, "%.1f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Cloud Opacity", &globals::misc::CloudOpacity, 0.f, 10000.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sky R", &globals::misc::SkySharedR, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sky G", &globals::misc::SkySharedG, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sky B", &globals::misc::SkySharedB, 0.f, 1.f, "%.2f");

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Fog", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Fog", &globals::misc::Fog);
                    menu::Checkbox(L"Fog RGB", &globals::misc::FogRGB);
                    menu::Checkbox(L"Volumetric Fog", &globals::misc::bEnableVolumetricFog);

                    menu::SliderFloat(L"Density", &globals::misc::FogDensity, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Falloff", &globals::misc::FogHeightFalloff, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Opacity", &globals::misc::FogMaxOpacity, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Start Dist", &globals::misc::FogStartDistance, 0.f, 50000.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Cutoff", &globals::misc::FogCutoffDistance, 0.f, 50000.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Vol Dist", &globals::misc::VolumetricFogDistance, 0.f, 20000.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Fog R", &globals::misc::FogColor.r, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Fog G", &globals::misc::FogColor.g, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Fog B", &globals::misc::FogColor.b, 0.f, 1.f, "%.2f");
                }
                else if (sub_tab == 3)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Materials", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Material Hand", &globals::visuals::hand_with_material);
                    menu::Checkbox(L"Material 1P Gun", &globals::visuals::gunmaterial1p);
                    menu::Checkbox(L"Material 3P Gun", &globals::visuals::gunmaterial3p);
                    menu::Checkbox(L"Material Skin 3P", &globals::misc::playerchamsself);
                    menu::Combobox(fvector2d(154, 23), &globals::visuals::typehand,
                        L"Black", L"Reyna", L"Green", L"Glass", L"Pink", L"Yellow", L"Red", L"Blue", NULL);

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Gun Types", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Combobox(fvector2d(154, 23), &globals::visuals::typegun1p,
                        L"Glass", L"Red", L"Blue", L"Yellow", NULL);
                    menu::Combobox(fvector2d(154, 23), &globals::visuals::typegun3d,
                        L"Glass", L"Red", L"Blue", L"Yellow", NULL);
                    menu::Combobox(fvector2d(154, 23), &globals::misc::chams_material_index,
                        L"Ninja", L"Cyber", L"Soviet", L"Sea", NULL);
                }
            }

            // ════════════════════════════════════════════════════
            // TAB 2 — CHAMS
            // ════════════════════════════════════════════════════
            else if (tab == 2)
            {
                static int sub_tab = 1;
                menu::offset_x = 1.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(L"Outline", fvector2d(50, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(L"Glow", fvector2d(40, 23), sub_tab == 2)) sub_tab = 2;
                menu::SameLine();
                if (menu::ButtonTab2(L"Galaxy", fvector2d(50, 23), sub_tab == 3)) sub_tab = 3;

                if (sub_tab == 1)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Outline Chams", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Outline Chams", &globals::chams::outline_enabled);
                    menu::Checkbox(L"Hand Outline", &globals::chams::hand_outline_enabled);
                    menu::Checkbox(L"Self Chams", &globals::chams::self_chams);
                    menu::Checkbox(L"Gun 1P Chams", &globals::chams::gun_outline1P_enabled);
                    menu::Checkbox(L"Gun 3P Chams", &globals::chams::gun_outline3P_enabled);
                    menu::SliderFloat(L"Vis Intensity", &globals::chams::intensityvisibleoutline, 10, 100, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Invis Intensity", &globals::chams::intensityinvisbleoutline, 10, 100, "%.0f");

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Presets", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Combobox(fvector2d(154, 23), &globals::chams::outlinetype,
                        L"Always", L"Behind", NULL);
                    menu::Combobox(fvector2d(154, 23), &globals::chams::visible_outline_preset,
                        L"Galaxy", L"Blue", L"Green", L"Orange", L"Pink", L"White", NULL);
                    menu::Combobox(fvector2d(154, 23), &globals::chams::invisible_outline_preset,
                        L"Red", L"Orange", L"Yellow", L"Green", L"Pink", L"Gray", NULL);
                }
                else if (sub_tab == 2)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Visible Chams", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Visible Chams", &globals::chams::rchamsespall);
                    menu::SliderFloat(L"Intensity", &globals::chams::Glow, 0.1f, 10.0f, "%.1f");

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Invisible Chams", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Invisible Chams", &globals::chams::rchamsesp);
                    menu::SliderFloat(L"Intensity", &globals::chams::Glowvni, 0.1f, 10.0f, "%.1f");
                }
                else if (sub_tab == 3)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Galaxy Chams", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Hand Galaxy", &globals::chams::hand_galaxy_enabled);
                    menu::Checkbox(L"Gun 1P Galaxy", &globals::chams::gun1p_galaxy_enabled);
                    menu::Checkbox(L"Gun 3P Galaxy", &globals::chams::gun3p_galaxy_enabled);
                    menu::Checkbox(L"Self Galaxy", &globals::chams::self_galaxy_enabled);
                    menu::Checkbox(L"Enemy Galaxy", &globals::chams::enemy_galaxy_enabled);

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Crystal Preset", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Combobox(fvector2d(154, 23), &globals::visuals::crystal_chams_preset,
                        L"Custom", L"Red", L"Green", L"Blue", L"Orange", L"Pink", L"Purple", NULL);
                }
            }

            // ════════════════════════════════════════════════════
            // TAB 3 — MISC
            // ════════════════════════════════════════════════════
            else if (tab == 3)
            {
                static int sub_tab = 1;
                menu::offset_x = 1.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(L"AA", fvector2d(25, 23), sub_tab == 2)) sub_tab = 2;
                menu::SameLine();
                if (menu::ButtonTab2(L"Chat", fvector2d(35, 23), sub_tab == 3)) sub_tab = 3;
                menu::SameLine();
                if (menu::ButtonTab2(L"Config", fvector2d(45, 23), sub_tab == 4)) sub_tab = 4;

                // ── Main ──────────────────────────────────────────────────
                if (sub_tab == 1)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Player", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Bunny Hop", &globals::misc::bhop);
                    menu::Checkbox(L"Fast Crouch", &globals::misc::fastcrouch);
                    menu::Checkbox(L"Anti Flash", &globals::misc::antiflash);
                    menu::Checkbox(L"Third Person", &globals::misc::tperson);
                    menu::SliderFloat(L"TP Distance", &globals::misc::PlayerDistance, 100, 1000, "%.0f");
                    menu::offset_y -= 8;
                    menu::Checkbox(L"FOV Changer", &globals::misc::fovchanger);
                    menu::SliderFloat(L"FOV", &globals::misc::fovchangur, 0, 170, "%.0f");
                    menu::Checkbox(L"Aspect Ratio", &globals::misc::aspectratio);
                    menu::Checkbox(L"View Model", &globals::misc::ViewModelChanger);
                    menu::Checkbox(L"Watermark", &globals::Watermark);
                    menu::offset_y += 5;
                    menu::Checkbox(L"Lineup Helper", &globals::lineup::enabled);
                    if (globals::lineup::enabled) {
                        menu::Checkbox(L"Show Guides", &globals::lineup::show_guides);
                        menu::Checkbox(L"Auto Aim", &globals::lineup::auto_aim);
                        menu::offset_y -= 8;
                        menu::SliderFloat(L"Velocity", &globals::lineup::projectile_velocity, 1000.0f, 5000.0f, "%.0f");
                        menu::offset_y -= 16;
                        menu::SliderFloat(L"Gravity", &globals::lineup::gravity_scale, 0.1f, 5.0f, "%.1f");
                        menu::offset_y -= 16;
                        menu::SliderFloat(L"Render Dist.", &globals::lineup::render_distance, 100.0f, 10000.0f, "%.0f");
                    }

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Extras", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Auto Peek", &globals::autopeek::enabled);
                    if (globals::autopeek::enabled) {
                        menu::Hotkey("Peek Key", fvector2d(9, 18), &globals::autopeek::peek_key);
                        menu::Checkbox(L"Draw Marker", &globals::autopeek::draw_position);
                        menu::offset_y += 5;
                    }

                    menu::Checkbox(L"Custom Skin", &globals::misc::custom_vandal_enabled);
                    if (globals::misc::custom_vandal_enabled) {
                        globals::misc::custom_text_enabled = true;
                        globals::misc::ViewModelChanger = true;
                    }

                    menu::Checkbox(L"Finisher", &globals::misc::finisher);
                    menu::Checkbox(L"Last Kill Only", &globals::misc::only_last_kill);
                    menu::Checkbox(L"Hellfire", &globals::misc::hellfiremode);   
                    menu::Checkbox(L"Lightning", &globals::misc::lightningmode);  
                    menu::Checkbox(L"Gun Buddy", &globals::buddy::enabled);
                    menu::SliderInt(L"Buddy ID", &globals::buddy::index, 0, 658, "%d");
                    menu::offset_y -= 8;
                    if (menu::Button(L"Skip Tutorial")) globals::misc::sk1ptut0rial = true;
                    if (menu::Button(L"Unlock ALL"))    globals::misc::sk1n_chang3r = true;
                }

                // ── AA ────────────────────────────────────────────────────
                else if (sub_tab == 2)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Anti-Aim", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Enable (F1)", &globals::misc::SpinBot);
                    menu::Combobox(fvector2d(154, 23), &globals::misc::pitch_mode,
                        L"None", L"Up", L"Down", L"Custom", NULL);
                    menu::Checkbox(L"Spin", &globals::misc::aa_spin);
                    menu::Checkbox(L"Jitter", &globals::misc::aa_jitter);
                    menu::Checkbox(L"Desync", &globals::misc::aa_desync);
                    menu::Checkbox(L"Backwards", &globals::misc::aa_backwards);
                    menu::Checkbox(L"Atomic AA", &globals::misc::aa_atomic);
                    menu::SliderFloat(L"Spin Speed", &globals::misc::spinvalue, 1.0f, 50.0f, "%.1f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Yaw Offset", &globals::misc::yaw_add, -180.0f, 180.0f, "%.1f");

                   
                   
                        menu::offset_x = 250;
                        menu::offset_y = 60;
                        menu::SectionWrapper(L"Snap Keys", fvector2d(228, 312));
                        menu::offset_y -= 11;

                        menu::Checkbox(L"Three-Way", &globals::misc::aa_threeway);
                        menu::Checkbox(L"Pred Breaker", &globals::misc::aa_prediction_breaker);
                        menu::HotkeyLabeled("Snap Left", &globals::misc::snap_left_key);
                        menu::HotkeyLabeled("Snap Right", &globals::misc::snap_right_key);
                        menu::HotkeyLabeled("Snap Back", &globals::misc::snap_back_key);
                }

                // ── Chat ──────────────────────────────────────────────────
                else if (sub_tab == 3)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Chat", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    menu::Checkbox(L"Kill Say", &globals::misc::killsays);
                    menu::Checkbox(L"Kill Sound", &globals::misc::killsound);
                    menu::Checkbox(L"Chat Spam (F4)", &globals::misc::chat_spammer);
                    menu::InputField(L"Message", &globals::misc::chat_message, 256);

                    menu::offset_x = 250;
                    menu::offset_y = 60;

                }

                // ── Config ────────────────────────────────────────────────
                else if (sub_tab == 4)
                {
                    menu::offset_x = 12;
                    menu::offset_y = 60;
                    menu::SectionWrapper(L"Config", fvector2d(228, 312));
                    menu::offset_y -= 11;

                    if (menu::Button(L"Save Config")) {
                        std::filesystem::path configDir = "C:/MagicBullet";
                        std::filesystem::path configPath = configDir / "config.json";
                        if (!std::filesystem::exists(configDir))
                            std::filesystem::create_directories(configDir);
                        Config->SaveSettings(configPath.string());
                    }
                    if (menu::Button(L"Load Config"))
                        Config->LoadSettings("C:/MagicBullet/config.json");
                    if (menu::Button(L"Discord"))
                        system("start https://discord.gg/magicbullet");

                    menu::offset_x = 250;
                    menu::offset_y = 60;
                }
            }

            fvector2d cursorPos = menu::CursorPos();
            menu::drawFilledRect(fvector2d(cursorPos.x - 2, cursorPos.y - 2), 4, 4, flinearcolor(1, 1, 1, 1));
        }

        menu::Render();
    }
}