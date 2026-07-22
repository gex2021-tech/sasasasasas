
// MY BASE HERE
#include "hooks.hpp"
#include "Menu/menu.hpp"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "ret_spoof.h"
#include "spoof.h"
#include "mutex"
#pragma comment(lib, "winmm.lib")
//#include "skin.hpp"

uintptr_t camera_engine;
bool should_hook_gay;

int screen_width = GetSystemMetrics(SM_CXSCREEN);
int screen_height = GetSystemMetrics(SM_CYSCREEN);

bool InGame = 0;
uworld* UWorldSave;
float spin_value;

static auto OldAimAngles = fvector();

// visibility is now computed locally inside the actor loop - no more stale globals

namespace keys
{
    fkey left_mouse;
    fkey right_mouse;
    fkey insert;

    fkey w;
    fkey a;
    fkey s;
    fkey d;
    fkey space;

    fkey capslock;
}





void* m_memset(void* dest, char c, unsigned int len)
{
    unsigned int i;
    unsigned int fill;
    unsigned int chunks = len / sizeof(fill);
    char* char_dest = (char*)dest;
    unsigned int* uint_dest = (unsigned int*)dest;
    fill = (c << 24) + (c << 16) + (c << 8) + c;

    for (i = len; i > chunks * sizeof(fill); i--) {
        char_dest[i - 1] = c;
    }

    for (i = chunks; i > 0; i--) {
        uint_dest[i - 1] = fill;
    }

    return dest;
}

void* m_memcpy(void* dest, void* src, unsigned int len)
{
    unsigned int i;
    char* char_src = (char*)src;
    char* char_dest = (char*)dest;
    for (i = 0; i < len; i++) {
        char_dest[i] = char_src[i];
    }
    return dest;
}

static bool previous_viewmodel_state = false;


float brillpg = 1.0f;
flinearcolor hlpclrg{ 1.0f, 0.5f, 0.0f, 0.9f };

static flinearcolor tlrclr = { 1.0f, 1.0f, 1.0f, 0.7f };
static flinearcolor basee_color = { 0.835f, 0.576f, 0.584f, 1.0f };



namespace hooks
{
    static int TargetX = 0;
    static int TargetY = 0;
    float ESPThickness = 1;
    bool enemiesarround = true;
    int enemyID = 0;
    int enemyIDvis = 0;
    int CloseRangeDistanceID = 0;
    float CloseRangeDistance = 50.f;

    static bool auto_shot_active = false;
    static ashootercharacter* safe_target_actor = nullptr;
    static std::mutex target_mutex; // Mutex pour synchroniser l'accÃ¨s


    aplayercontroller* controllers;

    acknowledgedpawn* pawn;

    aplayercameramanager* camera_cache;

    ashootercharacter* character;

    ashootercharacter* target_actor;


    inline static float  FOVChangorSprite = 5.0f;

    inline static float  Glow1 = 10.0f;
    inline static flinearcolor fresnel(2.093f, 0.019f, 20.0f, Glow1);

    inline float fresnelBaseR = 2.093f;
    inline float fresnelBaseG = 0.019f;
    inline float fresnelBaseB = 20.0f;

    //inline flinearcolor handcolor(fresnelBaseR, fresnelBaseG, fresnelBaseB, Glow1);

    /*  void Recoil_Control() {
          Sleep(3);
          mouse.mouse_event(TargetX, 5, 0);
      }*/

    flinearcolor get_color(bool condition) {
        return condition ? flinearcolor{ 0.1f, 1.0f, 0.1f, 1 } : flinearcolor{ 1.0f, 0.0f, 0.0f, 1 };
    }

    static flinearcolor Invisible{ 255.0f, 0.0f, 0.0f, 1.0f };  // Couleur rouge pour les Ã©lÃ©ments invisibles

    inline flinearcolor ChamsColor = Invisible;

    float rainbowTime = 0;  // Le temps qui augmentera pour gÃ©nÃ©rer un arc-en-ciel dynamique

    //flinearcolor Invisible = flinearcolor(0.0f, 0.0f, 0.0f, 0.0f);  // Invisible est une couleur de base (transparent ou noir)

    // Fonction pour gÃ©nÃ©rer une couleur arc-en-ciel en fonction du temps
    flinearcolor GetRainbowColor(float time)
    {
        const float PI = 3.14159265359f;
        float r = 0.5f + 0.5f * sin(time);
        float g = 0.5f + 0.5f * sin(time + 2.0f * PI / 3.0f);
        float b = 0.5f + 0.5f * sin(time + 4.0f * PI / 3.0f);
        return flinearcolor(r, g, b, 1.0f); // Alpha Ã  1.0 pour la pleine opacitÃ©
    }

    fvector2d posss = { ((float)GetSystemMetrics(SM_CXSCREEN) / 2) - 500, ((float)GetSystemMetrics(SM_CYSCREEN) / 2) - 475 };

    auto calculate_box_dimensions = [](fvector2d head_long_out, fvector2d base_out) -> std::pair<float, float> {
        float box_height = abs(head_long_out.y - base_out.y);
        float box_width = box_height * 0.55f;
        return { box_width, box_height };
        };

    auto get_target_bone_matrix = [](uskeletalmeshcomponent* mesh, int bone) -> fvector {
        switch (bone) {
        case 0: return mesh->get_bone_location(8); break;
        case 1: return mesh->get_bone_location(6); break;
        case 2: return mesh->get_bone_location(4); break;
        default: return fvector();
        }
        };

    void draw_head(ucanvas* canvas, uobject* Font, const wchar_t* text, flinearcolor color, fvector2d pos) {
        canvas->k2_drawtext(menu::font, fstring(text), pos, { 1.50f, 1.50f }, color, 0.f, { 0, 0, 0, 0.30f }, { 0, 0 }, true, true, true, { 0, 0, 0, 0.90f });
    }


    float DegreeToRadian(float degrees) {
        return degrees * (3.1415926535897932f / 180);
    }



    void draw_head2(ucanvas* canvas, uobject* Font, const wchar_t* text, flinearcolor color, fvector2d pos) {
        flinearcolor white_color = flinearcolor(1.0f, 1.0f, 1.0f, 1.0f); // Blanc (totalement opaque)

        // DÃ©finir la couleur de l'ombre : une version grise avec une opacitÃ© rÃ©duite (50 %)
        flinearcolor shadow_color = flinearcolor(0.5f, 0.5f, 0.5f, 0.5f); // Gris semi-transparent

        // DÃ©finir le dÃ©calage de l'ombre pour le texte (lÃ©gÃ¨rement dÃ©calÃ© en x et y)
        fvector2d shadow_offset = fvector2d{ 2.0f, 2.0f };

        // Draw the shadow first (slightly offset from the original position)
        canvas->k2_drawtext(menu::font, fstring(text), pos + shadow_offset, { 1.0f, 1.0f }, shadow_color, 0.f, shadow_color, shadow_offset, true, true, true, { 0, 0, 0, 0.90f });

        // Draw the main text with the purple color (overlaid on top of the shadow)
        canvas->k2_drawtext(menu::font, fstring(text), pos, { 1.0f, 1.0f }, white_color, 0.f, { 0, 0, 0, 0.30f }, { 0, 0 }, true, true, true, { 0, 0, 0, 0.90f });
    }





    boolean in_rect(double centerX, double centerY, double radius, double x, double y) {
        return x >= centerX - radius && x <= centerX + radius &&
            y >= centerY - radius && y <= centerY + radius;
    }





    inline bool is_target_in_fov(double screen_center_x, double screen_center_y, fvector2d target_pos) {

        return in_rect(screen_center_x, screen_center_y, globals::aimbot::a1m_f0v, target_pos.x, target_pos.y);
    }

    __forceinline fvector GetBoneMatrix(void* Mesh, int BoneIndex) {

        if (!Mesh) [[unlikely]]
            return fvector(0.f, 0.f, 0.f);


        if (BoneIndex < 0) [[unlikely]]
            return fvector(0.f, 0.f, 0.f);


        using BoneMatrixFn = FMatrix * (__fastcall*)(void*, FMatrix*, int);
        static const BoneMatrixFn fn = reinterpret_cast<BoneMatrixFn>(memory::module_base + offsets::bone_matrix);


        FMatrix BoneMatrix;


        fn(Mesh, &BoneMatrix, BoneIndex);


        return fvector(BoneMatrix.WPlane.X, BoneMatrix.WPlane.Y, BoneMatrix.WPlane.Z);
    }

    //fvector GetBoneMatrix(void* Mesh, int Idx) {

    //    FMatrix Matrix;
    //    reinterpret_cast<FMatrix* (__cdecl*)(void*, FMatrix*, int, uintptr_t, void*)>(spoofcall_stub)(Mesh, &Matrix, Idx, 0x46C4660, (void*)(memory::module_base + offsets::bone_matrix));

    //    
    //    return fvector(Matrix.WPlane.X, Matrix.WPlane.Y, Matrix.WPlane.Z);
    //}



    void DrawBox(ucanvas* can, fvector2d topleft, fvector2d downright, flinearcolor color)
    {

        if (!can) {
            return;
        }

        if (!topleft.is_valid() || !downright.is_valid()) {
            return;
        }


        if (topleft.x > downright.x || topleft.y > downright.y) {
            if (topleft.x > downright.x) {
                double temp = topleft.x;
                topleft.x = downright.x;
                downright.x = temp;
            }
            if (topleft.y > downright.y) {
                double temp = topleft.y;
                topleft.y = downright.y;
                downright.y = temp;
            }
        }

        double h = downright.y - topleft.y;
        double w = downright.x - topleft.x;

        if (h <= 0.0 || w <= 0.0) {
            return;
        }


        double thicc = (ESPThickness > 0.0) ? ESPThickness : 1.0;

        fvector2d topright = fvector2d(downright.x, topleft.y);
        fvector2d bottomleft = fvector2d(topleft.x, downright.y);

        can->k2_drawline(topleft, topright, thicc, color);
        can->k2_drawline(topright, downright, thicc, color);
        can->k2_drawline(downright, bottomleft, thicc, color);
        can->k2_drawline(bottomleft, topleft, thicc, color);
    }



    void Draw3DBox(ucanvas* canvas, aplayercontroller* controllers, fvector origin, fvector extent, const flinearcolor& color)
    {



        fvector vertex[8] = {
            origin + fvector(-extent.x, -extent.y, -extent.z),
            origin + fvector(extent.x, -extent.y, -extent.z),
            origin + fvector(extent.x, extent.y, -extent.z),
            origin + fvector(-extent.x, extent.y, -extent.z),
            origin + fvector(-extent.x, -extent.y, extent.z),
            origin + fvector(extent.x, -extent.y, extent.z),
            origin + fvector(extent.x, extent.y, extent.z),
            origin + fvector(-extent.x, extent.y, extent.z)
        };

        fvector2d screenVertex[8];
        bool canProject = true;


        for (int i = 0; i < 8; i++)
        {

            if (!controllers->project_world_location_to_screen(vertex[i], screenVertex[i], false))
            {
                canProject = false;
                break;
            }
        }

        if (!canProject)
            return;


        canvas->k2_drawline(screenVertex[0], screenVertex[1], ESPThickness, color);
        canvas->k2_drawline(screenVertex[1], screenVertex[2], ESPThickness, color);
        canvas->k2_drawline(screenVertex[2], screenVertex[3], ESPThickness, color);
        canvas->k2_drawline(screenVertex[3], screenVertex[0], ESPThickness, color);


        canvas->k2_drawline(screenVertex[4], screenVertex[5], ESPThickness, color);
        canvas->k2_drawline(screenVertex[5], screenVertex[6], ESPThickness, color);
        canvas->k2_drawline(screenVertex[6], screenVertex[7], ESPThickness, color);
        canvas->k2_drawline(screenVertex[7], screenVertex[4], ESPThickness, color);


        canvas->k2_drawline(screenVertex[0], screenVertex[4], ESPThickness, color);
        canvas->k2_drawline(screenVertex[1], screenVertex[5], ESPThickness, color);
        canvas->k2_drawline(screenVertex[2], screenVertex[6], ESPThickness, color);
        canvas->k2_drawline(screenVertex[3], screenVertex[7], ESPThickness, color);
    }
    void DrawCornerBox(ucanvas* canvas, int x, int y, int W, int H, flinearcolor color, int thickness)
    {

        float lineW = W / 3.0f;
        float lineH = H / 3.0f;

        // Convert flinearcolor to a color format compatible with k2_drawline
        flinearcolor clr = menu::RGBtoFLC(color.r, color.g, color.b);  // Assuming RGBtoFLC converts correctly

        // Top-left corner
        canvas->k2_drawline(fvector2d(x, y), fvector2d(x, y + lineH), thickness, color); // Left vertical line
        canvas->k2_drawline(fvector2d(x, y), fvector2d(x + lineW, y), thickness, color); // Top horizontal line

        // Top-right corner
        canvas->k2_drawline(fvector2d(x + W - lineW, y), fvector2d(x + W, y), thickness, color); // Top horizontal line
        canvas->k2_drawline(fvector2d(x + W, y), fvector2d(x + W, y + lineH), thickness, color); // Right vertical line

        // Bottom-left corner
        canvas->k2_drawline(fvector2d(x, y + H - lineH), fvector2d(x, y + H), thickness, color); // Bottom-left vertical line
        canvas->k2_drawline(fvector2d(x, y + H), fvector2d(x + lineW, y + H), thickness, color); // Bottom horizontal line

        // Bottom-right corner
        canvas->k2_drawline(fvector2d(x + W - lineW, y + H), fvector2d(x + W, y + H), thickness, color); // Bottom-right horizontal line
        canvas->k2_drawline(fvector2d(x + W, y + H - lineH), fvector2d(x + W, y + H), thickness, color); // Right vertical line
    }

    bool bOutline = 1;

    void draw_text(ucanvas* canvas, uobject* Font, const wchar_t* text, flinearcolor color, fvector2d pos) {
        canvas->k2_drawtext(menu::font, fstring(text), pos, { 1.00f, 1.00f }, color, 0.f, { 0, 0, 0, 0.30f }, { 0, 0 }, true, true, true, { 0, 0, 0, 0.45f });
    }

    static void draw_text_rgb_string(ucanvas* canvas, uobject* Font, fstring text, float x, float y, flinearcolor color, bool CenterX = false) {
        canvas->k2_drawtext(menu::font, text, { x, y }, { 1.1f, 1.1f }, color, 0.f, { 0, 0, 0, 1.0f }, { 0, 0 }, CenterX, false, true, { 0, 0, 0, 1.0f });
    }

    int current_selection = 3;
    const int max_selection = 26;
    static bool open_canvas = true;

    currentequippable* myweapon;
    currentequippable* lastweapon;

    static flinearcolor maincolor{ 1.0f,1.0f,1.0f,1.0f };
    float bowv4l = 1;

    void draw_triangle(ucanvas* canvas, int current_selection, float x, float y, float size, flinearcolor color) {
        static float time = 0.0f;

        float animationSpeed = 0.05f;
        float maxMovementRange = 4.0f;

        float animatedX = x + (sin(time) * maxMovementRange);

        time += animationSpeed;

        if (time > 6.2832f) {
            time -= 6.2832f;
        }

        fvector2d point1, point2, point3;

        switch (current_selection) {
        case 3:
            point1 = { animatedX, y };
            point2 = { animatedX + size, y + size / 2 };
            point3 = { animatedX, y + size };
            break;
        case 4:
            point1 = { animatedX, y };
            point2 = { animatedX + size / 2, y + size };
            point3 = { animatedX + size, y };
            break;
        case 5:
            point1 = { animatedX + size, y };
            point2 = { animatedX, y + size / 2 };
            point3 = { animatedX + size, y + size };
            break;
        case 6:
            point1 = { animatedX, y + size };
            point2 = { animatedX + size / 2, y };
            point3 = { animatedX + size, y + size };
            break;
        default:
            point1 = { animatedX, y };
            point2 = { animatedX + size, y + size / 2 };
            point3 = { animatedX, y + size };
            break;
        }

        canvas->k2_drawline(point1, point2, 2.0f, color);
        canvas->k2_drawline(point2, point3, 2.0f, color);
        canvas->k2_drawline(point3, point1, 2.0f, color);
    }

    void DrawAdaptiveBoundingBox(ucanvas* canvas, aplayercontroller* my_controller, uskeletalmeshcomponent* mesh, flinearcolor color)
    {
        if (!canvas || !my_controller || !mesh) return;

        fvector vHeadBone = mesh->get_bone_location(8);
        fvector vBaseBone = mesh->get_bone_location(0);

        if (!vHeadBone.is_valid() || !vBaseBone.is_valid()) return;

        fvector2d bottom1, bottom2, bottom3, bottom4;
        fvector2d top1, top2, top3, top4;

        bool valid_projection =
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x + 53, vBaseBone.y - 55, vBaseBone.z), bottom1, 0) && bottom1.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x - 53, vBaseBone.y - 55, vBaseBone.z), bottom2, 0) && bottom2.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x - 53, vBaseBone.y + 55, vBaseBone.z), bottom3, 0) && bottom3.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x + 53, vBaseBone.y + 55, vBaseBone.z), bottom4, 0) && bottom4.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x + 53, vHeadBone.y - 55, vHeadBone.z + 26), top1, 0) && top1.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x - 53, vHeadBone.y - 55, vHeadBone.z + 26), top2, 0) && top2.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x - 53, vHeadBone.y + 55, vHeadBone.z + 26), top3, 0) && top3.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x + 53, vHeadBone.y + 55, vHeadBone.z + 26), top4, 0) && top4.is_valid();

        if (!valid_projection) return;

        float left_most = fmin(fmin(bottom1.x, bottom2.x), fmin(bottom3.x, bottom4.x)) - 1.0f;
        float right_most = fmax(fmax(top1.x, top2.x), fmax(top3.x, top4.x)) + 1.0f;
        float top_most = fmin(fmin(top1.y, top2.y), fmin(top3.y, top4.y)) - 5.0f;
        float bottom_most = fmax(fmax(bottom1.y, bottom2.y), fmax(bottom3.y, bottom4.y)) + 5.0f;

        fvector2d top_left = { left_most, top_most };
        fvector2d bottom_right = { right_most, bottom_most };

        if (globals::visuals::box2d) {
            canvas->k2_drawline(top_left, { bottom_right.x, top_left.y }, 1.5f, color);
            canvas->k2_drawline(top_left, { top_left.x, bottom_right.y }, 1.5f, color);
            canvas->k2_drawline(bottom_right, { bottom_right.x, top_left.y }, 1.5f, color);
            canvas->k2_drawline(bottom_right, { top_left.x, bottom_right.y }, 1.5f, color);
        }

    }

    void DrawAdaptiveCornerBox(ucanvas* canvas, aplayercontroller* my_controller, uskeletalmeshcomponent* mesh, flinearcolor color, double thickness = 1.5f)
    {
        if (!canvas || !my_controller || !mesh) return;

        fvector vHeadBone = mesh->get_bone_location(8);
        fvector vBaseBone = mesh->get_bone_location(0);
        if (!vHeadBone.is_valid() || !vBaseBone.is_valid()) return;

        fvector2d bottom1, bottom2, bottom3, bottom4;
        fvector2d top1, top2, top3, top4;

        bool valid_projection =
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x + 53, vBaseBone.y - 55, vBaseBone.z), bottom1, 0) && bottom1.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x - 53, vBaseBone.y - 55, vBaseBone.z), bottom2, 0) && bottom2.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x - 53, vBaseBone.y + 55, vBaseBone.z), bottom3, 0) && bottom3.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vBaseBone.x + 53, vBaseBone.y + 55, vBaseBone.z), bottom4, 0) && bottom4.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x + 53, vHeadBone.y - 55, vHeadBone.z + 26), top1, 0) && top1.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x - 53, vHeadBone.y - 55, vHeadBone.z + 26), top2, 0) && top2.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x - 53, vHeadBone.y + 55, vHeadBone.z + 26), top3, 0) && top3.is_valid() &&
            my_controller->project_world_location_to_screen(fvector(vHeadBone.x + 53, vHeadBone.y + 55, vHeadBone.z + 26), top4, 0) && top4.is_valid();

        if (!valid_projection) return;

        // Calculer les limites de la box
        float left_most = fmin(fmin(bottom1.x, bottom2.x), fmin(bottom3.x, bottom4.x)) - 1.0f;
        float right_most = fmax(fmax(top1.x, top2.x), fmax(top3.x, top4.x)) + 1.0f;
        float top_most = fmin(fmin(top1.y, top2.y), fmin(top3.y, top4.y)) - 5.0f;
        float bottom_most = fmax(fmax(bottom1.y, bottom2.y), fmax(bottom3.y, bottom4.y)) + 5.0f;

        // Calculer dimensions et positions
        float width = right_most - left_most;
        float height = bottom_most - top_most;

        // Longueur des coins (25% de chaque dimension)
        float corner_length_x = width * 0.25f;
        float corner_length_y = height * 0.25f;

        // Dessiner les 4 coins directement avec k2_drawline

        // Coin TOP-LEFT
        canvas->k2_drawline({ left_most, top_most }, { left_most + corner_length_x, top_most }, thickness, color); // Horizontal
        canvas->k2_drawline({ left_most, top_most }, { left_most, top_most + corner_length_y }, thickness, color); // Vertical

        // Coin TOP-RIGHT  
        canvas->k2_drawline({ right_most, top_most }, { right_most - corner_length_x, top_most }, thickness, color); // Horizontal
        canvas->k2_drawline({ right_most, top_most }, { right_most, top_most + corner_length_y }, thickness, color); // Vertical

        // Coin BOTTOM-LEFT
        canvas->k2_drawline({ left_most, bottom_most }, { left_most + corner_length_x, bottom_most }, thickness, color); // Horizontal
        canvas->k2_drawline({ left_most, bottom_most }, { left_most, bottom_most - corner_length_y }, thickness, color); // Vertical

        // Coin BOTTOM-RIGHT
        canvas->k2_drawline({ right_most, bottom_most }, { right_most - corner_length_x, bottom_most }, thickness, color); // Horizontal
        canvas->k2_drawline({ right_most, bottom_most }, { right_most, bottom_most - corner_length_y }, thickness, color); // Vertical
    }

    // Nouvelle fonction pour calculer la distance 3D rÃ©elle sans projection Ã©cran
    inline bool is_target_in_fov_360(fvector player_pos, fvector target_pos, double fov_radius) {
        // Calculer la distance 3D directe entre le joueur et la cible
        fvector delta = target_pos - player_pos;
        double distance_3d = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

        // Le FOV devient une sphÃ¨re autour du joueur plutÃ´t qu'un cÃ´ne de vision
        return distance_3d <= fov_radius;
    }

    // Version alternative utilisant distance horizontale seulement (plus rÃ©aliste)
    inline bool is_target_in_fov_360_horizontal(fvector player_pos, fvector target_pos, double fov_radius) {
        // Calculer seulement la distance horizontale (ignorer Z pour la hauteur)
        fvector delta = target_pos - player_pos;
        double distance_2d = sqrt(delta.x * delta.x + delta.y * delta.y);

        return distance_2d <= fov_radius;
    }

    // Fonction hybride : FOV normal OU 360Â° selon le mode
    inline bool is_target_in_fov_adaptive(double screen_center_x, double screen_center_y,
        fvector2d target_screen_pos,
        fvector player_world_pos,
        fvector target_world_pos,
        double fov_value,
        bool use_360_fov) {
        if (use_360_fov) {
            // Mode 360Â° : utiliser la distance monde directe
            return is_target_in_fov_360_horizontal(player_world_pos, target_world_pos, fov_value);
        }
        else {
            // Mode normal : utiliser la projection Ã©cran classique
            return in_rect(screen_center_x, screen_center_y, fov_value, target_screen_pos.x, target_screen_pos.y);
        }
    }



    enum class ValorantBones : int {
        CHEST = 6,
        RIBS = 5,
        HEAD = 8,
        NECK = 9,
        LEFT_SHOULDER = 36,
        RIGHT_SHOULDER = 33,
        LEFT_ARM = 37,
        RIGHT_ARM = 30,
        LEFT_HAND = 38,
        RIGHT_HAND = 32,
        PELVIS = 60,
        LEFT_HIPS = 77,
        RIGHT_HIPS = 63,
        LEFT_KNEE = 79,
        RIGHT_KNEE = 65,
        LEFT_ANKLE = 80,
        RIGHT_ANKLE = 66
    };

    // Structure pour stocker toutes les positions des bones
    struct ValorantSkeletonData {
        fvector2d head;
        fvector2d neck;
        fvector2d chest;
        fvector2d ribs;
        fvector2d left_shoulder;
        fvector2d right_shoulder;
        fvector2d left_arm;
        fvector2d right_arm;
        fvector2d left_hand;
        fvector2d right_hand;
        fvector2d pelvis;
        fvector2d left_hips;
        fvector2d right_hips;
        fvector2d left_knee;
        fvector2d right_knee;
        fvector2d left_ankle;
        fvector2d right_ankle;

        bool valid = false;
    };


    ValorantSkeletonData extract_skeleton_bones(aplayercontroller* controller, uskeletalmeshcomponent* mesh) {
        ValorantSkeletonData skeleton_data;

        if (!controller || !mesh) {
            skeleton_data.valid = false;
            return skeleton_data;
        }

        auto get_bone_screen_pos = [&](ValorantBones bone_id, fvector2d& out_pos) -> bool {
            fvector world_pos = mesh->get_bone_location(static_cast<int>(bone_id));
            if (!world_pos.is_valid()) return false;

            return controller->project_world_location_to_screen(world_pos, out_pos, false) && out_pos.is_valid();
            };

        // Extraire toutes les positions
        bool all_bones_valid =
            get_bone_screen_pos(ValorantBones::HEAD, skeleton_data.head) &&
            get_bone_screen_pos(ValorantBones::NECK, skeleton_data.neck) &&
            get_bone_screen_pos(ValorantBones::CHEST, skeleton_data.chest) &&
            get_bone_screen_pos(ValorantBones::RIBS, skeleton_data.ribs) &&
            get_bone_screen_pos(ValorantBones::LEFT_SHOULDER, skeleton_data.left_shoulder) &&
            get_bone_screen_pos(ValorantBones::RIGHT_SHOULDER, skeleton_data.right_shoulder) &&
            get_bone_screen_pos(ValorantBones::LEFT_ARM, skeleton_data.left_arm) &&
            get_bone_screen_pos(ValorantBones::RIGHT_ARM, skeleton_data.right_arm) &&
            get_bone_screen_pos(ValorantBones::LEFT_HAND, skeleton_data.left_hand) &&
            get_bone_screen_pos(ValorantBones::RIGHT_HAND, skeleton_data.right_hand) &&
            get_bone_screen_pos(ValorantBones::PELVIS, skeleton_data.pelvis) &&
            get_bone_screen_pos(ValorantBones::LEFT_HIPS, skeleton_data.left_hips) &&
            get_bone_screen_pos(ValorantBones::RIGHT_HIPS, skeleton_data.right_hips) &&
            get_bone_screen_pos(ValorantBones::LEFT_KNEE, skeleton_data.left_knee) &&
            get_bone_screen_pos(ValorantBones::RIGHT_KNEE, skeleton_data.right_knee) &&
            get_bone_screen_pos(ValorantBones::LEFT_ANKLE, skeleton_data.left_ankle) &&
            get_bone_screen_pos(ValorantBones::RIGHT_ANKLE, skeleton_data.right_ankle);

        skeleton_data.valid = all_bones_valid;
        return skeleton_data;
    }

    // Fonction pour dessiner le skeleton complet
    void draw_valorant_skeleton(ucanvas* canvas, const ValorantSkeletonData& skeleton, flinearcolor color, double thickness = 1.5f) {
        if (!canvas || !skeleton.valid) return;

        // Dessiner la colonne vertÃ©brale (tÃªte -> cou -> chest -> ribs -> pelvis)
        canvas->k2_drawline(skeleton.head, skeleton.neck, thickness, color);
        canvas->k2_drawline(skeleton.neck, skeleton.chest, thickness, color);
        canvas->k2_drawline(skeleton.chest, skeleton.ribs, thickness, color);
        canvas->k2_drawline(skeleton.ribs, skeleton.pelvis, thickness, color);

        // Dessiner les Ã©paules et bras complets (neck -> shoulders -> arms -> hands)
        canvas->k2_drawline(skeleton.neck, skeleton.left_shoulder, thickness, color);
        canvas->k2_drawline(skeleton.neck, skeleton.right_shoulder, thickness, color);
        canvas->k2_drawline(skeleton.left_shoulder, skeleton.left_arm, thickness, color);
        canvas->k2_drawline(skeleton.right_shoulder, skeleton.right_arm, thickness, color);
        canvas->k2_drawline(skeleton.left_arm, skeleton.left_hand, thickness, color);
        canvas->k2_drawline(skeleton.right_arm, skeleton.right_hand, thickness, color);

        // Dessiner les hanches (left hips <- pelvis -> right hips)
        canvas->k2_drawline(skeleton.pelvis, skeleton.left_hips, thickness, color);
        canvas->k2_drawline(skeleton.pelvis, skeleton.right_hips, thickness, color);

        // Dessiner les jambes gauches (left hips -> left knee -> left ankle)
        canvas->k2_drawline(skeleton.left_hips, skeleton.left_knee, thickness, color);
        canvas->k2_drawline(skeleton.left_knee, skeleton.left_ankle, thickness, color);

        // Dessiner les jambes droites (right hips -> right knee -> right ankle)
        canvas->k2_drawline(skeleton.right_hips, skeleton.right_knee, thickness, color);
        canvas->k2_drawline(skeleton.right_knee, skeleton.right_ankle, thickness, color);
    }

    // Fonction principale Ã  appeler dans ton ESP
    void draw_valorant_skeleton_esp(aplayercontroller* controller, uskeletalmeshcomponent* mesh, ucanvas* canvas, flinearcolor color) {
        if (!controller || !mesh || !canvas) return;

        // Extraire les positions des bones
        ValorantSkeletonData skeleton_data = extract_skeleton_bones(controller, mesh);

        // Dessiner le skeleton si valide
        if (skeleton_data.valid) {
            draw_valorant_skeleton(canvas, skeleton_data, color);
        }
    }



    namespace G
    {
        currentequippable* MyWeapon = nullptr;
        currentequippable* LastWeapon = nullptr;
    }




    template<class k, class e>
    class tmap
    {
    public:
        k Key;
        e Element;
        char __pad0x[0x8];
    };
    inline fstring BuddyName;
    inline uobject* buddy;



    static flinearcolor Name_Color{ 1.f,1.f,1.f,1.f };
    float RainbowTime = 0.0f;
    const float RainbowSpeed = 1.0f;
    const float PI = 3.14159265359f;

    flinearcolor convert_to_flinearcolor(int r, int g, int b, int a) {
        return flinearcolor(
            (float)r / 255.0f,
            (float)g / 255.0f,
            (float)b / 255.0f,
            (float)a / 255.0f
        );
    }

    int index = 453;


    std::wstring to_wide_string(const std::string& str) {
        return std::wstring(str.begin(), str.end());
    }

    static bool bFlickSilent = true;
    static bool bLockedCameraRotation = false;

    fvector2d head_scren;

    double screen_center_x;
    double screen_center_y;

    fvector	LocalCameraLocation;
    float LocalCameraFOV;
    fvector	LocalCameraRotation;

    static bool first_location = true;
    static bool aim_check = false;
    static bool second_locked_camera = false;
    static bool finished_hook = false;
    static fvector first_camera_location;
    static fvector first_camera_rotation;

    static fvector saved_client_view;
    static bool anti_aim_is_active = false;



    void(*SetCameraCachePOVOriginal)(uintptr_t, FMinimalViewInfo*) = nullptr;

    float pitch = -90.f;
    float yaw = 0.f;




    static bool aimbot_key_pressed_last_frame = false;





    namespace helper {
        fstring convert_weapon_name(fstring weapon_name)
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





    static fvector calc_spread(ashootercharacter* actor, uint64_t firing_state_component, currentequippable* weapon, fvector direction)
    {
        if (!actor || !firing_state_component || !weapon)
            return fvector(0, 0, 0);


        uint64_t stability_component = memory::read<uint64_t>(firing_state_component + offsets::stability_component);
        if (!stability_component)
            return fvector(0, 0, 0);


        alignas(16) static uint8_t error_values[4096];
        alignas(16) static uint8_t seed_data_snapshot[4096];
        alignas(16) static uint8_t spread_angles[4096];
        alignas(16) static uint8_t out_spread_angles[4096];


        static auto func1_fn = (float* (__fastcall*)(uint64_t, float*))(memory::module_base + offsets::get_spread_values);
        static auto func2_fn = (void(__fastcall*)(uint64_t, fvector*, float, float, int, int, uint64_t))(memory::module_base + offsets::get_spread_angles);
        static auto func3_fn = (fvector * (__fastcall*)(fvector*, fvector*))(memory::module_base + offsets::to_vector_and_normalize);
        static auto func4_fn = (fvector * (__fastcall*)(fvector*, fvector*))(memory::module_base + offsets::to_angle_and_normalize);
        static auto func5_fn = (uintptr_t(__fastcall*)(__int64, float*))(memory::module_base + offsets::get_spread_values);


        *(uint64_t*)(&out_spread_angles[0]) = (uint64_t)&spread_angles[0];
        *(int*)(&out_spread_angles[0] + 8) = 1;
        *(int*)(&out_spread_angles[0] + 12) = 1;


        uint64_t seed_data = memory::read<uint64_t>(firing_state_component + offsets::seed_data);
        if (!seed_data)
            return fvector(0, 0, 0);


        memcpy((void*)seed_data_snapshot, (void*)seed_data, sizeof(seed_data_snapshot));


        reinterpret_cast<float* (__cdecl*)(uint64_t, float*, uintptr_t, void*)>(
            spoofcall_stub)(stability_component, (float*)&error_values[0], 0x46C4660, (void*)func1_fn);

        fvector temp1, temp2 = fvector(0, 0, 0);
        fvector previous_firing_direction, firing_direction = fvector(0, 0, 0);


        actor->get_firing_location_and_direction(&temp1, &previous_firing_direction, false);


        reinterpret_cast<fvector* (__cdecl*)(fvector*, fvector*, uintptr_t, void*)>(
            spoofcall_stub)(&previous_firing_direction, &temp2, 0x46C4660, (void*)func3_fn);
        reinterpret_cast<fvector* (__cdecl*)(fvector*, fvector*, uintptr_t, void*)>(
            spoofcall_stub)(&temp2, &temp1, 0x46C4660, (void*)func4_fn);

        previous_firing_direction = temp1;


        temp1.x += *(float*)(&error_values[0] + 12);
        temp1.y += *(float*)(&error_values[0] + 16);


        reinterpret_cast<fvector* (__cdecl*)(fvector*, fvector*, uintptr_t, void*)>(
            spoofcall_stub)(&temp1, &firing_direction, 0x46C4660, (void*)func3_fn);


        float test[20];
        uintptr_t v38 = reinterpret_cast<uintptr_t(__cdecl*)(__int64, float*, uintptr_t, void*)>(
            spoofcall_stub)(stability_component, test, 0x46C4660, (void*)func5_fn);

        float v46 = memory::read<float>(v38 + 0x14);
        float v48 = *(float*)(&error_values[0] + 8) + *(float*)(&error_values[0] + 4);
        int error_retries = memory::read<int>(firing_state_component + offsets::error_retries);


        reinterpret_cast<void(__cdecl*)(
            uint64_t, fvector*, float, float, int, int, uint64_t, uintptr_t, void*)>(
                spoofcall_stub)(
                    ((uint64_t)&seed_data_snapshot[0]) + 0xD8, &firing_direction,
                    v48, v46, error_retries, 1,
                    (uint64_t)&out_spread_angles[0], 0x46C4660, (void*)func2_fn);


        fvector spread_vector = *(fvector*)(&spread_angles[0]);

        reinterpret_cast<fvector* (__cdecl*)(fvector*, fvector*, uintptr_t, void*)>(
            spoofcall_stub)(&spread_vector, &firing_direction, 0x46C4660, (void*)func4_fn);

        return firing_direction - previous_firing_direction;
    }

    void angle_rotation(const fvector& angles, fvector* forward)
    {
        float	sp, sy, cp, cy;

        sy = sin(DegreeToRadian(angles.y));
        cy = cos(DegreeToRadian(angles.y));

        sp = sin(DegreeToRadian(angles.x));
        cp = cos(DegreeToRadian(angles.x));

        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    fvector RotationToVector(const fvector& rotation) {
        const double DEG_TO_RAD = 0.017453292519943295f;

        double sy = sinf(rotation.y * DEG_TO_RAD);
        double cy = cosf(rotation.y * DEG_TO_RAD);
        double sp = sinf(rotation.x * DEG_TO_RAD);
        double cp = cosf(rotation.x * DEG_TO_RAD);

        return fvector(cp * cy, cp * sy, -sp);
    }

    inline float clamp_value(float value, float min_val, float max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }









    void apply_outline_chams(acknowledgedpawn* pawn, ashootercharacter* actor, aplayercontroller* controllers)
    {
        if (!pawn || !actor || !controllers) return;

        auto is_visible = controllers->line_of_sight(actor);

        float glowIntensity;
        flinearcolor centerEdgeColor, innerEdgeColor, outerEdgeColor;

        if (is_visible) {
            centerEdgeColor = flinearcolor(globals::chams::CenterEdgeR_Visible, globals::chams::CenterEdgeG_Visible, globals::chams::CenterEdgeB_Visible, globals::chams::intensityvisibleoutline);
            innerEdgeColor = flinearcolor(globals::chams::InnerEdgeR_Visible, globals::chams::InnerEdgeG_Visible, globals::chams::InnerEdgeB_Visible, globals::chams::intensityvisibleoutline);
            outerEdgeColor = flinearcolor(globals::chams::OuterEdgeR_Visible, globals::chams::OuterEdgeG_Visible, globals::chams::OuterEdgeB_Visible, globals::chams::intensityvisibleoutline);
            glowIntensity = globals::chams::GlowVisible;
        }
        else {
            centerEdgeColor = flinearcolor(globals::chams::CenterEdgeR_Invisible, globals::chams::CenterEdgeG_Invisible, globals::chams::CenterEdgeB_Invisible, globals::chams::intensityinvisbleoutline);
            innerEdgeColor = flinearcolor(globals::chams::InnerEdgeR_Invisible, globals::chams::InnerEdgeG_Invisible, globals::chams::InnerEdgeB_Invisible, globals::chams::intensityinvisbleoutline);
            outerEdgeColor = flinearcolor(globals::chams::OuterEdgeR_Invisible, globals::chams::OuterEdgeG_Invisible, globals::chams::OuterEdgeB_Invisible, globals::chams::intensityinvisbleoutline);
            glowIntensity = globals::chams::GlowInvisible;
        }

        static fname silohuette_color_name, center_edge_color_name, inner_edge_color_name, outer_edge_color_name, glow_intensity_param;
        if (!silohuette_color_name.comparison_index) {
            silohuette_color_name = string::string_to_name(L"SilohuetteColor");
            center_edge_color_name = string::string_to_name(L"CenterEdgeColor");
            inner_edge_color_name = string::string_to_name(L"InnerEdgeColor");
            outer_edge_color_name = string::string_to_name(L"OuterEdgeColor");
            glow_intensity_param = string::string_to_name(L"GlowIntensity");
        }

        uobject* visible_material = uobject::static_load_object(
            nullptr,
            nullptr,
            L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI"
        );

        uobject* invisible_material = uobject::static_load_object(
            nullptr,
            nullptr,
            L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI"
        );

        if (!visible_material || !invisible_material) return;

        if (globals::chams::outlinetype == 0 || !is_visible)
        {
            auto main_mesh = actor->get_mesh();
            if (main_mesh) {
                auto num_materials = main_mesh->get_num_materials();
                for (int i = 0; i < num_materials; i++) {
                    auto material_instance_dynamic = main_mesh->create_and_set_material_instance_dynamic_from_material(
                        i,
                        is_visible ? visible_material : invisible_material
                    );

                    auto dynCast = material_instance_dynamic->cast<UMaterialInstanceDynamic>();
                    if (!dynCast) continue;

                    dynCast->set_vector_parameter_value1(silohuette_color_name, outerEdgeColor);
                    dynCast->set_vector_parameter_value1(center_edge_color_name, centerEdgeColor);
                    dynCast->set_vector_parameter_value1(inner_edge_color_name, innerEdgeColor);
                    dynCast->set_vector_parameter_value1(outer_edge_color_name, outerEdgeColor);
                    dynCast->set_scalar_parameter_value(glow_intensity_param, glowIntensity);
                }
            }


            uskeletalmeshcomponent* mesh_cosmetic_3p = actor->GetCosmeticMesh3P();
            if (mesh_cosmetic_3p) {
                auto num_materials = mesh_cosmetic_3p->get_num_materials();
                for (int i = 0; i < num_materials; i++) {
                    auto material_instance_dynamic = mesh_cosmetic_3p->create_and_set_material_instance_dynamic_from_material(
                        i,
                        is_visible ? visible_material : invisible_material
                    );

                    auto dynCast = material_instance_dynamic->cast<UMaterialInstanceDynamic>();
                    if (!dynCast) continue;

                    dynCast->set_vector_parameter_value1(silohuette_color_name, outerEdgeColor);
                    dynCast->set_vector_parameter_value1(center_edge_color_name, centerEdgeColor);
                    dynCast->set_vector_parameter_value1(inner_edge_color_name, innerEdgeColor);
                    dynCast->set_vector_parameter_value1(outer_edge_color_name, outerEdgeColor);
                    dynCast->set_scalar_parameter_value(glow_intensity_param, glowIntensity);
                }
            }
        }
        else if (globals::chams::outlinetype == 1 && is_visible)
        {
            auto main_mesh = actor->get_mesh();
            if (main_mesh) {
                actor->reset_character_materials_internal(main_mesh);
            }


            uskeletalmeshcomponent* mesh_cosmetic_3p = actor->GetCosmeticMesh3P();
            if (mesh_cosmetic_3p) {
                actor->reset_character_materials_internal(mesh_cosmetic_3p);
            }
        }
    }

    void ApplyCrystalChamsPreset(int preset) {
        switch (preset) {
        case 0: // Custom - ne rien faire
            break;
        case 1: // Red Dark
            globals::visuals::Self_CenterEdgeR = 0.613636f;
            globals::visuals::Self_CenterEdgeG = 0.0f;
            globals::visuals::Self_CenterEdgeB = 0.170455f;
            globals::visuals::Self_InnerEdgeR = 1.32955f;
            globals::visuals::Self_InnerEdgeG = 0.0f;
            globals::visuals::Self_InnerEdgeB = 0.89f;
            globals::visuals::Self_OuterEdgeR = 9.64773f;
            globals::visuals::Self_OuterEdgeG = 11.64f;
            globals::visuals::Self_OuterEdgeB = 0.0f;

            globals::visuals::GlowVisible = 1.5f;
            globals::visuals::AlphaBasePower = 2.0f;
            globals::visuals::AlphaColorMult = 1.2f;
            globals::visuals::DepthBias = 0.1f;
            globals::visuals::AlphaDissolveOpacity = 0.8f;
            globals::visuals::BoundingBox = 1.0f;
            globals::visuals::InnerEdgeThickness = 0.3f;
            globals::visuals::OuterEdgeThickness = 0.2f;
            globals::visuals::RimFresnel = 2.5f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 12.2727f;
            globals::visuals::OcclusionDepth = 0.5f;
            globals::visuals::OcclusionBehindWall = 0.3f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.246591f;
            break;

        case 2: // Dark Green
            globals::visuals::Self_CenterEdgeR = 0.0f;
            globals::visuals::Self_CenterEdgeG = 0.545455f;
            globals::visuals::Self_CenterEdgeB = 0.170455f;
            globals::visuals::Self_InnerEdgeR = 1.32955f;
            globals::visuals::Self_InnerEdgeG = 0.0f;
            globals::visuals::Self_InnerEdgeB = 0.89f;
            globals::visuals::Self_OuterEdgeR = 9.64773f;
            globals::visuals::Self_OuterEdgeG = 9.95455f;
            globals::visuals::Self_OuterEdgeB = 0.0f;

            globals::visuals::GlowVisible = 1.5f;
            globals::visuals::AlphaBasePower = 2.0f;
            globals::visuals::AlphaColorMult = 1.2f;
            globals::visuals::DepthBias = 0.1f;
            globals::visuals::AlphaDissolveOpacity = 0.8f;
            globals::visuals::BoundingBox = 1.0f;
            globals::visuals::InnerEdgeThickness = 0.3f;
            globals::visuals::OuterEdgeThickness = 0.2f;
            globals::visuals::RimFresnel = 2.5f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 12.2727f;
            globals::visuals::OcclusionDepth = 0.5f;
            globals::visuals::OcclusionBehindWall = 0.3f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.246591f;
            break;

        case 3: // Dark Blue
            globals::visuals::Self_CenterEdgeR = 0.0f;
            globals::visuals::Self_CenterEdgeG = 0.0f;
            globals::visuals::Self_CenterEdgeB = 0.477273f;
            globals::visuals::Self_InnerEdgeR = 0.0340909f;
            globals::visuals::Self_InnerEdgeG = 0.0f;
            globals::visuals::Self_InnerEdgeB = 0.0f;
            globals::visuals::Self_OuterEdgeR = 0.0f;
            globals::visuals::Self_OuterEdgeG = 0.0f;
            globals::visuals::Self_OuterEdgeB = 0.0f;

            globals::visuals::GlowVisible = 1.5f;
            globals::visuals::AlphaBasePower = 2.0f;
            globals::visuals::AlphaColorMult = 1.2f;
            globals::visuals::DepthBias = 0.1f;
            globals::visuals::AlphaDissolveOpacity = 0.8f;
            globals::visuals::BoundingBox = 1.0f;
            globals::visuals::InnerEdgeThickness = 0.3f;
            globals::visuals::OuterEdgeThickness = 0.2f;
            globals::visuals::RimFresnel = 2.5f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 12.2727f;
            globals::visuals::OcclusionDepth = 0.5f;
            globals::visuals::OcclusionBehindWall = 0.3f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.246591f;
            break;

        case 4: // Dark Orange
            globals::visuals::Self_CenterEdgeR = 0.647727f;
            globals::visuals::Self_CenterEdgeG = 0.579545f;
            globals::visuals::Self_CenterEdgeB = 0.0f;
            globals::visuals::Self_InnerEdgeR = 0.511364f;
            globals::visuals::Self_InnerEdgeG = 0.27f;
            globals::visuals::Self_InnerEdgeB = 1.0f;
            globals::visuals::Self_OuterEdgeR = 0.04f;
            globals::visuals::Self_OuterEdgeG = 0.23f;
            globals::visuals::Self_OuterEdgeB = 0.21f;

            globals::visuals::GlowVisible = 200.0f;
            globals::visuals::AlphaBasePower = 0.806818f;
            globals::visuals::AlphaColorMult = 0.515227f;
            globals::visuals::DepthBias = 0.106818f;
            globals::visuals::AlphaDissolveOpacity = 0.207412f;
            globals::visuals::BoundingBox = -50.0f;
            globals::visuals::InnerEdgeThickness = 0.1f;
            globals::visuals::OuterEdgeThickness = 0.37f;
            globals::visuals::RimFresnel = 1.0f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 20.0f;
            globals::visuals::OcclusionDepth = 0.0f;
            globals::visuals::OcclusionBehindWall = 1.24545f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.0f;
            break;

        case 5: // Orange
            globals::visuals::Self_CenterEdgeR = 0.681818f;
            globals::visuals::Self_CenterEdgeG = 0.579545f;
            globals::visuals::Self_CenterEdgeB = 0.0f;
            globals::visuals::Self_InnerEdgeR = 0.511364f;
            globals::visuals::Self_InnerEdgeG = 0.27f;
            globals::visuals::Self_InnerEdgeB = 1.0f;
            globals::visuals::Self_OuterEdgeR = 0.04f;
            globals::visuals::Self_OuterEdgeG = 0.23f;
            globals::visuals::Self_OuterEdgeB = 0.21f;

            globals::visuals::GlowVisible = 200.0f;
            globals::visuals::AlphaBasePower = 0.806818f;
            globals::visuals::AlphaColorMult = 0.515227f;
            globals::visuals::DepthBias = 0.106818f;
            globals::visuals::AlphaDissolveOpacity = 0.207412f;
            globals::visuals::BoundingBox = -50.0f;
            globals::visuals::InnerEdgeThickness = 0.1f;
            globals::visuals::OuterEdgeThickness = 0.37f;
            globals::visuals::RimFresnel = 1.0f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 20.0f;
            globals::visuals::OcclusionDepth = 0.0f;
            globals::visuals::OcclusionBehindWall = 1.24545f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.0f;
            break;

        case 6: // Pink
            globals::visuals::Self_CenterEdgeR = 0.647727f;
            globals::visuals::Self_CenterEdgeG = 0.545455f;
            globals::visuals::Self_CenterEdgeB = 0.511364f;
            globals::visuals::Self_InnerEdgeR = 0.511364f;
            globals::visuals::Self_InnerEdgeG = 0.27f;
            globals::visuals::Self_InnerEdgeB = 1.0f;
            globals::visuals::Self_OuterEdgeR = 0.04f;
            globals::visuals::Self_OuterEdgeG = 0.23f;
            globals::visuals::Self_OuterEdgeB = 0.21f;

            globals::visuals::GlowVisible = 200.0f;
            globals::visuals::AlphaBasePower = 0.806818f;
            globals::visuals::AlphaColorMult = 0.515227f;
            globals::visuals::DepthBias = 0.106818f;
            globals::visuals::AlphaDissolveOpacity = 0.207412f;
            globals::visuals::BoundingBox = -50.0f;
            globals::visuals::InnerEdgeThickness = 0.1f;
            globals::visuals::OuterEdgeThickness = 0.37f;
            globals::visuals::RimFresnel = 1.0f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 20.0f;
            globals::visuals::OcclusionDepth = 0.0f;
            globals::visuals::OcclusionBehindWall = 1.24545f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.0f;
            break;

        case 7: // Black Galaxy
            globals::visuals::Self_CenterEdgeR = 0.647727f;
            globals::visuals::Self_CenterEdgeG = 0.579545f;
            globals::visuals::Self_CenterEdgeB = 0.545455f;
            globals::visuals::Self_InnerEdgeR = 0.511364f;
            globals::visuals::Self_InnerEdgeG = 0.27f;
            globals::visuals::Self_InnerEdgeB = 1.0f;
            globals::visuals::Self_OuterEdgeR = 0.04f;
            globals::visuals::Self_OuterEdgeG = 0.23f;
            globals::visuals::Self_OuterEdgeB = 0.21f;

            globals::visuals::GlowVisible = 200.0f;
            globals::visuals::AlphaBasePower = 0.806818f;
            globals::visuals::AlphaColorMult = 0.515227f;
            globals::visuals::DepthBias = 0.106818f;
            globals::visuals::AlphaDissolveOpacity = 0.207412f;
            globals::visuals::BoundingBox = -50.0f;
            globals::visuals::InnerEdgeThickness = 0.1f;
            globals::visuals::OuterEdgeThickness = 0.37f;
            globals::visuals::RimFresnel = 1.0f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 20.0f;
            globals::visuals::OcclusionDepth = 0.0f;
            globals::visuals::OcclusionBehindWall = 1.24545f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.0f;
            break;

        case 8: // Purple
            globals::visuals::Self_CenterEdgeR = 0.613636f;
            globals::visuals::Self_CenterEdgeG = 0.511364f;
            globals::visuals::Self_CenterEdgeB = 0.545455f;
            globals::visuals::Self_InnerEdgeR = 0.511364f;
            globals::visuals::Self_InnerEdgeG = 0.27f;
            globals::visuals::Self_InnerEdgeB = 1.0f;
            globals::visuals::Self_OuterEdgeR = 0.04f;
            globals::visuals::Self_OuterEdgeG = 0.23f;
            globals::visuals::Self_OuterEdgeB = 0.21f;

            globals::visuals::GlowVisible = 200.0f;
            globals::visuals::AlphaBasePower = 0.806818f;
            globals::visuals::AlphaColorMult = 0.515227f;
            globals::visuals::DepthBias = 0.106818f;
            globals::visuals::AlphaDissolveOpacity = 0.207412f;
            globals::visuals::BoundingBox = -50.0f;
            globals::visuals::InnerEdgeThickness = 0.1f;
            globals::visuals::OuterEdgeThickness = 0.37f;
            globals::visuals::RimFresnel = 1.0f;
            globals::visuals::RimMultiply = 1.0f;
            globals::visuals::RimPower = 20.0f;
            globals::visuals::OcclusionDepth = 0.0f;
            globals::visuals::OcclusionBehindWall = 1.24545f;
            globals::visuals::OcclusionState = 1.0f;
            globals::visuals::RefractionDepthBias = 0.0f;
            break;
        }
    }

    void apply_galaxy_chams(acknowledgedpawn* pawn, ashootercharacter* actor, aplayercontroller* controllers)
    {
        if (!pawn || !actor || !controllers) return;
        if (!globals::chams::enemy_galaxy_enabled) return;

        static int last_preset = -1;

        if (last_preset != globals::visuals::crystal_chams_preset) {
            ApplyCrystalChamsPreset(globals::visuals::crystal_chams_preset);
            last_preset = globals::visuals::crystal_chams_preset;
        }

        uobject* galaxy_material = uobject::static_load_object(
            nullptr,
            nullptr,
            L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI"
        );
        if (!galaxy_material) return;

        fname silohuette_color_name = string::string_to_name(L"SilohuetteColor");
        fname center_edge_color_name = string::string_to_name(L"CenterEdgeColor");
        fname inner_edge_color_name = string::string_to_name(L"InnerEdgeColor");
        fname outer_edge_color_name = string::string_to_name(L"OuterEdgeColor");
        fname glow_intensity_param = string::string_to_name(L"GlowIntensity");

        fname alpha_base_power_name = string::string_to_name(L"Alpha_Base_Power");
        fname depth_bias_name = string::string_to_name(L"DepthBias");
        fname alpha_dissolve_opacity_name = string::string_to_name(L"Alpha_Dissolve_Opacity");
        fname bounding_box_name = string::string_to_name(L"BoundingBox");
        fname inner_edge_thickness_name = string::string_to_name(L"InnerEdgeThickness");
        fname outer_edge_thickness_name = string::string_to_name(L"OuterEdgeThickness");
        fname rim_fresnel_name = string::string_to_name(L"Rim_Fresnel");
        fname rim_multiply_name = string::string_to_name(L"Rim_Multiply");
        fname rim_power_name = string::string_to_name(L"Rim_Power");
        fname occlusion_behind_wall_name = string::string_to_name(L"OcclusionDepth_BehindWall");
        fname occlusion_state_name = string::string_to_name(L"OcclusionState");
        fname refraction_depth_bias_name = string::string_to_name(L"RefractionDepthBias");

        float enemy_glowIntensity = globals::visuals::GlowVisible;
        float alpha_base_power = globals::visuals::AlphaBasePower;
        float alpha_colormult = globals::visuals::AlphaColorMult;
        float depth_bias = globals::visuals::DepthBias;
        float alpha_dissolve_opacity = globals::visuals::AlphaDissolveOpacity;
        float bounding_box = globals::visuals::BoundingBox;
        float inner_edge_thickness = globals::visuals::InnerEdgeThickness;
        float outer_edge_thickness = globals::visuals::OuterEdgeThickness;
        float rim_fresnel = globals::visuals::RimFresnel;
        float rim_multiply = globals::visuals::RimMultiply;
        float rim_power = globals::visuals::RimPower;
        float occlusion_behind_wall = globals::visuals::OcclusionBehindWall;
        float occlusion_state = globals::visuals::OcclusionState;
        float refraction_depth_bias = globals::visuals::RefractionDepthBias;

        flinearcolor enemy_centerEdgeColor = flinearcolor(
            globals::visuals::Self_CenterEdgeR,
            globals::visuals::Self_CenterEdgeG,
            globals::visuals::Self_CenterEdgeB,
            18.20f
        );
        flinearcolor enemy_innerEdgeColor = flinearcolor(
            globals::visuals::Self_InnerEdgeR,
            globals::visuals::Self_InnerEdgeG,
            globals::visuals::Self_InnerEdgeB,
            18.20f
        );
        flinearcolor enemy_outerEdgeColor = flinearcolor(
            globals::visuals::Self_OuterEdgeR,
            globals::visuals::Self_OuterEdgeG,
            globals::visuals::Self_OuterEdgeB,
            18.20f
        );

        auto main_mesh = actor->get_mesh();
        if (main_mesh) {
            auto num_materials = main_mesh->get_num_materials();
            for (int i = 0; i < num_materials; i++) {
                auto material_instance_dynamic = main_mesh->create_and_set_material_instance_dynamic_from_material(
                    i,
                    galaxy_material
                );
                auto dynCast = material_instance_dynamic->cast<UMaterialInstanceDynamic>();
                if (!dynCast) continue;

                dynCast->set_vector_parameter_value1(silohuette_color_name, enemy_outerEdgeColor);
                dynCast->set_vector_parameter_value1(center_edge_color_name, enemy_centerEdgeColor);
                dynCast->set_vector_parameter_value1(inner_edge_color_name, enemy_innerEdgeColor);
                dynCast->set_vector_parameter_value1(outer_edge_color_name, enemy_outerEdgeColor);
                dynCast->set_scalar_parameter_value(glow_intensity_param, enemy_glowIntensity);
                dynCast->set_scalar_parameter_value(alpha_base_power_name, alpha_base_power);
                dynCast->set_scalar_parameter_value(depth_bias_name, depth_bias);
                dynCast->set_scalar_parameter_value(alpha_dissolve_opacity_name, alpha_dissolve_opacity);
                dynCast->set_scalar_parameter_value(bounding_box_name, bounding_box);
                dynCast->set_scalar_parameter_value(inner_edge_thickness_name, inner_edge_thickness);
                dynCast->set_scalar_parameter_value(outer_edge_thickness_name, outer_edge_thickness);
                dynCast->set_scalar_parameter_value(rim_fresnel_name, rim_fresnel);
                dynCast->set_scalar_parameter_value(rim_multiply_name, rim_multiply);
                dynCast->set_scalar_parameter_value(rim_power_name, rim_power);
                dynCast->set_scalar_parameter_value(occlusion_behind_wall_name, occlusion_behind_wall);
                dynCast->set_scalar_parameter_value(occlusion_state_name, occlusion_state);
                dynCast->set_scalar_parameter_value(refraction_depth_bias_name, refraction_depth_bias);
            }
        }

        uskeletalmeshcomponent* mesh_cosmetic_3p = actor->GetCosmeticMesh3P();
        if (mesh_cosmetic_3p) {
            auto num_materials = mesh_cosmetic_3p->get_num_materials();
            for (int i = 0; i < num_materials; i++) {
                auto material_instance_dynamic = mesh_cosmetic_3p->create_and_set_material_instance_dynamic_from_material(
                    i,
                    galaxy_material
                );
                auto dynCast = material_instance_dynamic->cast<UMaterialInstanceDynamic>();
                if (!dynCast) continue;

                dynCast->set_vector_parameter_value1(silohuette_color_name, enemy_outerEdgeColor);
                dynCast->set_vector_parameter_value1(center_edge_color_name, enemy_centerEdgeColor);
                dynCast->set_vector_parameter_value1(inner_edge_color_name, enemy_innerEdgeColor);
                dynCast->set_vector_parameter_value1(outer_edge_color_name, enemy_outerEdgeColor);
                dynCast->set_scalar_parameter_value(glow_intensity_param, enemy_glowIntensity);
                dynCast->set_scalar_parameter_value(alpha_base_power_name, alpha_base_power);
                dynCast->set_scalar_parameter_value(depth_bias_name, depth_bias);
                dynCast->set_scalar_parameter_value(alpha_dissolve_opacity_name, alpha_dissolve_opacity);
                dynCast->set_scalar_parameter_value(bounding_box_name, bounding_box);
                dynCast->set_scalar_parameter_value(inner_edge_thickness_name, inner_edge_thickness);
                dynCast->set_scalar_parameter_value(outer_edge_thickness_name, outer_edge_thickness);
                dynCast->set_scalar_parameter_value(rim_fresnel_name, rim_fresnel);
                dynCast->set_scalar_parameter_value(rim_multiply_name, rim_multiply);
                dynCast->set_scalar_parameter_value(rim_power_name, rim_power);
                dynCast->set_scalar_parameter_value(occlusion_behind_wall_name, occlusion_behind_wall);
                dynCast->set_scalar_parameter_value(occlusion_state_name, occlusion_state);
                dynCast->set_scalar_parameter_value(refraction_depth_bias_name, refraction_depth_bias);
            }
        }
    }





#include <string>

#pragma pack(push, 1)
    union fp_flag_store {
        unsigned char raw;
        struct {
            unsigned char f0 : 1;
            unsigned char f1 : 1;
            unsigned char f2 : 6;
        } bits;
    };
#pragma pack(pop)

    struct ViewModelCache {
        uskeletalmeshcomponent* mesh1p = nullptr;
        uskeletalmeshcomponent* overlayMesh = nullptr;
        uskeletalmeshcomponent* weaponMesh1P = nullptr;
        uskeletalmeshcomponent* cosmeticMesh1P = nullptr;
        uskeletalmeshcomponent* meleeMesh1P = nullptr;
        uskeletalmeshcomponent* offHandMesh = nullptr;
        currentequippable* lastWeapon = nullptr;
        currentequippable* lastMelee = nullptr;
        ULONGLONG lastCacheTime = 0;

        void Clear() {
            mesh1p = overlayMesh = weaponMesh1P = cosmeticMesh1P = nullptr;
            meleeMesh1P = offHandMesh = nullptr;
            lastWeapon = lastMelee = nullptr;
        }

        bool IsValid() const {
            return mesh1p != nullptr && weaponMesh1P != nullptr;
        }
    };

    inline bool IsValidViewModelPointer(uintptr_t ptr) {
        if (ptr == 0 || ptr == (uintptr_t)-1 || ptr < 0x10000 || ptr > 0x7FFFFFFFFFFF) {
            return false;
        }

        return memory::IsValidPointer(ptr);
    }

    inline bool IsValidViewModelObject(void* obj) {
        if (!obj) return false;
        return IsValidViewModelPointer((uintptr_t)obj);
    }

    inline tarray<USceneComponent*> GetChildrenComponents(USceneComponent* component, bool bIncludeAllDescendants) {
        tarray<USceneComponent*> result;
        if (!component || !IsValidViewModelObject(component)) return result;

        static uobject* Function = nullptr;
        if (!Function) {
            auto function_name = (L"Engine.SceneComponent.GetChildrenComponents");
            Function = uobject::find_object<uobject*>(function_name);
        }

        if (!Function || !IsValidViewModelObject(Function)) return result;

        struct {
            bool bIncludeAllDescendants;
            tarray<USceneComponent*> Children;
        } Args;

        Args.bIncludeAllDescendants = bIncludeAllDescendants;
        Args.Children.data = nullptr;
        Args.Children.count = 0;
        Args.Children.maxCount = 0;

        if (component && Function) { component->process_event(Function, &Args); } return Args.Children;
    }

    currentequippable* GetLocalMeleeWeapon() {
        static currentequippable* CachedMelee = nullptr;
        static ULONGLONG lastCacheTime = 0;

        ULONGLONG currentTime = GetTickCount64();
        if (CachedMelee && (currentTime - lastCacheTime) < 2000) {
            return CachedMelee;
        }

        CachedMelee = nullptr;
        if (!UWorldSave) return nullptr;

        tarray<AGameObject*> Objects;
        GameplayStatics::GetAllActorsOfClass2(UWorldSave, Class::Actors(), &Objects);

        for (int i = 0; i < Objects.size(); ++i) {
            AGameObject* Object = Objects[i];
            if (!Object) continue;

            auto name = system::get_object_name(Object);
            if (!name.is_valid()) continue;

            std::string name_str = name.ToString();
            if (name_str.find("Ability_Melee_Base_C") != std::string::npos) {
                CachedMelee = (currentequippable*)Object;
                lastCacheTime = currentTime;
                return CachedMelee;
            }
        }

        return nullptr;
    }

    template<typename T>
    inline bool SafeRead(uintptr_t address, T& value) {
        if (!IsValidViewModelPointer(address)) return false;

        value = *reinterpret_cast<T*>(address); return true;
    }

    template<typename T>
    inline bool SafeWrite(uintptr_t address, const T& value) {
        if (!IsValidViewModelPointer(address)) return false;

        *reinterpret_cast<T*>(address) = value; return true;
    }

    inline bool SafeProcessFlag(uskeletalmeshcomponent* mesh, uintptr_t flag_offset, bool clear_flag) {
        if (!mesh || !IsValidViewModelObject(mesh)) return false;

        uintptr_t flag_addr = (uintptr_t)mesh + flag_offset;
        if (!IsValidViewModelPointer(flag_addr)) return false;

        fp_flag_store state;
        if (!SafeRead(flag_addr, state.raw)) return false;

        if (clear_flag) {
            state.bits.f0 = 0;
            return SafeWrite(flag_addr, state.raw);
        }

        return true;
    }

    inline void SafeLockDescendants(USceneComponent* component) {
        if (!component || !IsValidViewModelObject(component)) return;

        tarray<USceneComponent*> allChildren;
        allChildren = GetChildrenComponents(component, true);

        for (int i = 0; i < allChildren.Num(); i++) {
            USceneComponent* child = allChildren[i];
            if (child && IsValidViewModelObject(child)) {
                uintptr_t child_flag_addr = (uintptr_t)child + 0x364;
                SafeWrite(child_flag_addr, (unsigned char)0x00);
            }
        }
    }

    inline void process_fp_mode(ashootercharacter* shooter) {
        if (!shooter) return;

        uintptr_t shooter_ptr = (uintptr_t)shooter;
        if (shooter_ptr == 0 || shooter_ptr == (uintptr_t)-1 || shooter_ptr < 0x10000 || shooter_ptr > 0x7FFFFFFFFFFF) {
            return;
        }

        

        bool isAlive = false;
        isAlive = shooter ? shooter->is_alive() : false;

        static ViewModelCache cache;

        if (!isAlive) {
            cache.Clear();
            return;
        }

        static bool last_force_key_state = false;
        ULONGLONG current_time = GetTickCount64();
        bool force_key_pressed = GetAsyncKeyState(VK_F8) & 0x8000;
        bool force_reapply = force_key_pressed && !last_force_key_state;
        last_force_key_state = force_key_pressed;

        uinventory* inventory = nullptr;
        __try {
            inventory = shooter->get_inventory();
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            cache.Clear();
            return;
        }

        if (!inventory || !IsValidViewModelObject(inventory)) {
            cache.Clear();
            return;
        }

        currentequippable* weapon = nullptr;
        __try {
            weapon = inventory->get_current_equippable();
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            cache.Clear();
            return;
        }

        if (!weapon || !IsValidViewModelObject(weapon)) {
            cache.Clear();
            return;
        }

        bool needs_refresh = false;
        if (!cache.IsValid() ||
            weapon != cache.lastWeapon ||
            (current_time - cache.lastCacheTime) >= 2000 ||
            force_reapply) {
            needs_refresh = true;
        }

        if (needs_refresh) {
            cache.Clear();
            cache.lastCacheTime = current_time;
            cache.lastWeapon = weapon;

            __try {
                cache.mesh1p = shooter->getmesh1p();
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                cache.Clear();
                return;
            }

            if (!cache.mesh1p || !IsValidViewModelObject(cache.mesh1p)) {
                cache.Clear();
                return;
            }

            __try {
                cache.overlayMesh = shooter->GetOverlayMesh1P();
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {}

            __try {
                cache.weaponMesh1P = weapon->GetMesh1P();
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                cache.Clear();
                return;
            }

            if (!cache.weaponMesh1P || !IsValidViewModelObject(cache.weaponMesh1P)) {
                cache.Clear();
                return;
            }

            uintptr_t cosmetic_ptr = (uintptr_t)weapon + 0x1188;
            if (IsValidViewModelPointer(cosmetic_ptr)) {
                __try {
                    cache.cosmeticMesh1P = memory::read<uskeletalmeshcomponent*>(cosmetic_ptr);
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {}
            }

            auto meleeWeapon = GetLocalMeleeWeapon();
            if (meleeWeapon && IsValidViewModelObject(meleeWeapon)) {
                cache.lastMelee = meleeWeapon;

                __try {
                    cache.meleeMesh1P = meleeWeapon->GetMesh1P();
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {
                    cache.meleeMesh1P = nullptr;
                }

                if (cache.meleeMesh1P) {
                    uintptr_t offhand_ptr = (uintptr_t)meleeWeapon + 0x1220;
                    if (IsValidViewModelPointer(offhand_ptr)) {
                        __try {
                            cache.offHandMesh = memory::read<uskeletalmeshcomponent*>(offhand_ptr);
                        }
                        __except (EXCEPTION_EXECUTE_HANDLER) {
                            cache.offHandMesh = nullptr;
                        }
                    }
                }
            }
        }

        if (!cache.IsValid() || !cache.mesh1p || !cache.weaponMesh1P) {
            return;
        }

        const uintptr_t flag_offset = 0x364;

        bool should_process_flags = false;
        fp_flag_store main_state;

        uintptr_t mesh_flag_addr = (uintptr_t)cache.mesh1p + flag_offset;
        if (SafeRead(mesh_flag_addr, main_state.raw)) {
            should_process_flags = (main_state.bits.f0 != 0) || force_reapply;
        }

        if (should_process_flags) {
            SafeProcessFlag(cache.mesh1p, flag_offset, true);

            if (cache.overlayMesh && IsValidViewModelObject(cache.overlayMesh)) {
                SafeProcessFlag(cache.overlayMesh, flag_offset, true);
            }

            if (cache.weaponMesh1P && IsValidViewModelObject(cache.weaponMesh1P)) {
                SafeProcessFlag(cache.weaponMesh1P, flag_offset, true);
            }

            if (cache.cosmeticMesh1P && IsValidViewModelObject(cache.cosmeticMesh1P)) {
                SafeProcessFlag(cache.cosmeticMesh1P, flag_offset, true);
            }

            if (cache.meleeMesh1P && IsValidViewModelObject(cache.meleeMesh1P)) {
                SafeProcessFlag(cache.meleeMesh1P, flag_offset, true);
            }

            if (cache.offHandMesh && IsValidViewModelObject(cache.offHandMesh)) {
                SafeProcessFlag(cache.offHandMesh, flag_offset, true);
            }
        }

        if (cache.mesh1p && IsValidViewModelObject(cache.mesh1p)) {
            SafeLockDescendants((USceneComponent*)cache.mesh1p);
        }

        if (cache.weaponMesh1P && IsValidViewModelObject(cache.weaponMesh1P)) {
            SafeLockDescendants((USceneComponent*)cache.weaponMesh1P);
        }

        if (cache.meleeMesh1P && IsValidViewModelObject(cache.meleeMesh1P)) {
            SafeLockDescendants((USceneComponent*)cache.meleeMesh1P);
        }
    }

    bool IsValidUObject(uobject* obj)
    {
        if (!obj) return false;
        if (!memory::IsValidPointer((uintptr_t)obj)) return false;


        void* vtable = *(void**)obj;
        return memory::IsValidPointer((uintptr_t)vtable);
    }

    struct USceneComponentHelpers
    {
        static void Detach(void* Target)
        {
            static uobject* fn = nullptr;
            if (!fn)
                fn = uobject::find_object<uobject*>(L"Engine.SceneComponent.K2_DetachFromComponent");

            if (!fn || !Target) return;

            struct
            {
                int LocationRule;
                int RotationRule;
                int ScaleRule;
                bool bCallModify;
            } params;

            params.LocationRule = 0;
            params.RotationRule = 0;
            params.ScaleRule = 0;
            params.bCallModify = true;

            ((uobject*)Target)->process_event(fn, &params);
        }

        static void K2_DetachFromComponent(void* Target, int LocationRule, int RotationRule, int ScaleRule, bool bCallModify)
        {
            static uobject* Function = nullptr;
            if (!Function)
                Function = uobject::find_object<uobject*>(L"Engine.SceneComponent.K2_DetachFromComponent");

            if (!Function || !Target) return;

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

            ((uobject*)Target)->process_event(Function, &params);
        }

        static fname GetAttachSocketName(uskeletalmeshcomponent* TargetComponent)
        {
            static uobject* Function = nullptr;
            if (!Function)
                Function = uobject::find_object<uobject*>(L"Engine.SceneComponent.GetAttachSocketName");

            if (!Function || !TargetComponent) return fname();

            struct
            {
                fname ReturnValue;
            } params;

            TargetComponent->process_event(Function, &params);
            return params.ReturnValue;
        }

        static bool AttachTo(uskeletalmeshcomponent* Target, uskeletalmeshcomponent* Parent, fname SocketName,
            int LocationRule, int RotationRule, int ScaleRule, bool bWeldSimulatedBodies)
        {
            static uobject* Function = nullptr;
            if (!Function)
                Function = uobject::find_object<uobject*>(L"Engine.SceneComponent.K2_AttachToComponent");

            if (!Function || !Target) return false;

            struct
            {
                void* Parent;
                fname SocketName;
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

            Target->process_event(Function, &params);
            return params.ReturnValue;
        }

        static void SetRelativeLocation(void* Target, const fvector& Location, bool bSweep = false, bool bTeleport = true)
        {
            static uobject* fn = nullptr;
            if (!fn)
                fn = uobject::find_object<uobject*>(L"Engine.SceneComponent.K2_SetRelativeLocation");

            if (!fn || !Target) return;

            struct
            {
                fvector NewLocation;
                bool bSweep;
                FHitResult SweepHitResult;
                bool bTeleport;
            } params;

            params.NewLocation = Location;
            params.bSweep = bSweep;
            params.bTeleport = bTeleport;
            memset(&params.SweepHitResult, 0, sizeof(FHitResult));

            ((uobject*)Target)->process_event(fn, &params);
        }

        static void SetRelativeRotation(void* Target, const FRotator& Rotation, bool bSweep = false, bool bTeleport = true)
        {
            static uobject* fn = nullptr;
            if (!fn)
                fn = uobject::find_object<uobject*>(L"Engine.SceneComponent.K2_SetRelativeRotation");

            if (!fn || !Target) return;

            struct
            {
                FRotator NewRotation;
                bool bSweep;
                FHitResult SweepHitResult;
                bool bTeleport;
            } params;

            params.NewRotation = Rotation;
            params.bSweep = bSweep;
            params.bTeleport = bTeleport;
            memset(&params.SweepHitResult, 0, sizeof(FHitResult));

            ((uobject*)Target)->process_event(fn, &params);
        }

    };


    struct WeaponTransform {
        fvector position;
        frotator rotation;
        fvector scale;
    };
    enum class EAttachmentRule : uint8 {
        KeepRelative = 0,
        KeepWorld = 1,
        SnapToTarget = 2,
        EAttachmentRule_MAX = 3
    };

    WeaponTransform GetTextTransform(const std::wstring& weaponName, int skinIndex) {
        WeaponTransform transform;

        if (weaponName == L"Melee") {
            transform.position = fvector(globals::misc::melee_text_pos_x, globals::misc::melee_text_pos_y, globals::misc::melee_text_pos_z);
            transform.rotation = frotator(globals::misc::melee_text_rot_pitch, globals::misc::melee_text_rot_yaw, globals::misc::melee_text_rot_roll);
            transform.scale = fvector(globals::misc::melee_text_scale_x, globals::misc::melee_text_scale_y, globals::misc::melee_text_scale_z);
        }
        else if (weaponName == L"Vandal") {
            transform.position = fvector(globals::misc::text_pos_x, globals::misc::text_pos_y, globals::misc::text_pos_z);
            transform.rotation = frotator(globals::misc::text_rot_pitch, globals::misc::text_rot_yaw, globals::misc::text_rot_roll);
            transform.scale = fvector(globals::misc::text_scale_x, globals::misc::text_scale_y, globals::misc::text_scale_z);
        }
        else if (weaponName == L"Phantom") {
            transform.position = fvector(globals::misc::phantom_text_pos_x, globals::misc::phantom_text_pos_y, globals::misc::phantom_text_pos_z);
            transform.rotation = frotator(globals::misc::phantom_text_rot_pitch, globals::misc::phantom_text_rot_yaw, globals::misc::phantom_text_rot_roll);
            transform.scale = fvector(globals::misc::phantom_text_scale_x, globals::misc::phantom_text_scale_y, globals::misc::phantom_text_scale_z);
        }
        else if (weaponName == L"Ghost") {
            transform.position = fvector(globals::misc::ghost_text_pos_x, globals::misc::ghost_text_pos_y, globals::misc::ghost_text_pos_z);
            transform.rotation = frotator(globals::misc::ghost_text_rot_pitch, globals::misc::ghost_text_rot_yaw, globals::misc::ghost_text_rot_roll);
            transform.scale = fvector(globals::misc::ghost_text_scale_x, globals::misc::ghost_text_scale_y, globals::misc::ghost_text_scale_z);
        }
        else if (weaponName == L"Frenzy") {
            transform.position = fvector(globals::misc::frenzy_text_pos_x, globals::misc::frenzy_text_pos_y, globals::misc::frenzy_text_pos_z);
            transform.rotation = frotator(globals::misc::frenzy_text_rot_pitch, globals::misc::frenzy_text_rot_yaw, globals::misc::frenzy_text_rot_roll);
            transform.scale = fvector(globals::misc::frenzy_text_scale_x, globals::misc::frenzy_text_scale_y, globals::misc::frenzy_text_scale_z);
        }
        else if (weaponName == L"Spectre") {
            transform.position = fvector(globals::misc::spectre_text_pos_x, globals::misc::spectre_text_pos_y, globals::misc::spectre_text_pos_z);
            transform.rotation = frotator(globals::misc::spectre_text_rot_pitch, globals::misc::spectre_text_rot_yaw, globals::misc::spectre_text_rot_roll);
            transform.scale = fvector(globals::misc::spectre_text_scale_x, globals::misc::spectre_text_scale_y, globals::misc::spectre_text_scale_z);
        }
        else {
            transform.position = fvector(globals::misc::text_pos_x, globals::misc::text_pos_y, globals::misc::text_pos_z);
            transform.rotation = frotator(globals::misc::text_rot_pitch, globals::misc::text_rot_yaw, globals::misc::text_rot_roll);
            transform.scale = fvector(globals::misc::text_scale_x, globals::misc::text_scale_y, globals::misc::text_scale_z);
        }

        return transform;
    }




    static std::map<uintptr_t, bool> WeaponHasCustomMesh;
    static std::map<uintptr_t, UProceduralMeshComponent*> WeaponTextMeshMap;

    static bool text_meshcreated = false;
    static UProceduralMeshComponent* TextMesh = nullptr;

    static int last_vandal_sel = -1;
    static int last_frenzy_sel = -1;
    static int last_ghost_sel = -1;

    struct MeshData {
        tarray<fvector> Vertices;
        tarray<int32_t> Triangles;
        tarray<fvector> Normals;
        tarray<fvector2d> UV0;
        tarray<FColor> VertexColors;
        tarray<FProcMeshTangent> Tangents;
    };

    static std::map<std::string, MeshData> ModelCache;
    static std::map<std::string, uobject*> TextureCache;
    static bool ModelsLoadedToRAM = false;
    static uobject* LastWorldPtr = nullptr;
    static uintptr_t LastWeaponProcessed = 0;


    inline void SetComponentVisibility(USceneComponent* component, bool bNewVisibility, bool bPropagateToChildren) {
        if (!component || !memory::IsValidPointer((uintptr_t)component)) return;
        static uobject* Function = uobject::find_object<uobject*>(L"Engine.SceneComponent.SetVisibility");
        if (!Function) return;
        struct { bool bNewVisibility; bool bPropagateToChildren; } Args = { bNewVisibility, bPropagateToChildren };
        ((uobject*)component)->process_event(Function, &Args);
    }

    inline uskeletalmeshcomponent* FindSightComponent(currentequippable* weapon, uskeletalmeshcomponent* GunMesh1P) {
        if (!weapon || !memory::IsValidPointer((uintptr_t)weapon)) return nullptr;
        if (!GunMesh1P || !memory::IsValidPointer((uintptr_t)GunMesh1P)) return nullptr;
        USceneComponent* sceneComp = reinterpret_cast<USceneComponent*>(GunMesh1P);
        tarray<USceneComponent*> children = GetChildrenComponents(sceneComp, true);
        for (int i = 0; i < children.Num(); i++) {
            if (!children[i]) continue;
            fstring childName = system::get_object_name((uobject*)children[i]);
            if (childName.to_str().find("SkeletalMeshComponent_") != std::string::npos) {
                uskeletalmeshcomponent* skelMesh = reinterpret_cast<uskeletalmeshcomponent*>(children[i]);
                UPrimitiveComponent* primComp = reinterpret_cast<UPrimitiveComponent*>(skelMesh);
                if (primComp && memory::IsValidPointer((uintptr_t)primComp)) {
                    if (primComp->get_num_materials() > 0) return skelMesh;
                }
            }
        }
        return nullptr;
    }


#include <wininet.h>
#include <thread>
#include <atomic>
#pragma comment(lib, "wininet.lib")

    static std::string GetPublicPath() {
        char* sharedProfile = getenv("ALLUSERSPROFILE");
        if (sharedProfile != nullptr)
            return std::string(sharedProfile) + "\\cs2_models\\";
        return "C:\\ProgramData\\cs2_models\\";
    }

    bool DownloadFromServer(std::string fileName, std::string url) {
        std::string path = GetPublicPath();
        std::string fullPath = path + fileName;
        system(("mkdir \"" + path + "\" > nul 2>&1").c_str());

        HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) return false;

        HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hUrl) { InternetCloseHandle(hInternet); return false; }

        std::vector<uint8_t> buffer;
        char temp[4096]; DWORD bytesRead;
        while (InternetReadFile(hUrl, temp, sizeof(temp), &bytesRead) && bytesRead > 0)
            buffer.insert(buffer.end(), temp, temp + bytesRead);

        InternetCloseHandle(hUrl); InternetCloseHandle(hInternet);
        if (buffer.empty()) return false;

        HANDLE hFile = CreateFileA(fullPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return false;
        DWORD bytesWritten;
        WriteFile(hFile, buffer.data(), (DWORD)buffer.size(), &bytesWritten, NULL);
        CloseHandle(hFile);
        return true;
    }

    void DownloadMissingAssets() {
        std::string baseDir = GetPublicPath();
        system(("mkdir \"" + baseDir + "\" > nul 2>&1").c_str());

        std::map<std::string, std::string> assetList = {
            // Assets download removed for clean release.
        };

        for (auto const& [fileName, url] : assetList) {
            std::string localPath = baseDir + fileName;
            if (GetFileAttributesA(localPath.c_str()) == INVALID_FILE_ATTRIBUTES)
                DownloadFromServer(fileName, url);
        }
    }

    const MeshData& ParseOBJFile(const char* filepath) {
        static MeshData empty;
        std::string pathKey(filepath);
        if (ModelCache.count(pathKey)) return ModelCache[pathKey];

        MeshData data;
        std::ifstream file(filepath);
        if (!file.is_open()) return empty;

        std::vector<fvector> temp_vertices;
        std::vector<fvector2d> temp_uvs;
        std::vector<fvector> temp_normals;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;
            if (type == "v") {
                float x, y, z; iss >> x >> y >> z;
                temp_vertices.push_back(fvector(x * 100.0f, y * 100.0f, z * 100.0f));
            }
            else if (type == "vt") {
                float u, v; iss >> u >> v;
                temp_uvs.push_back(fvector2d(u, 1.0f - v));
            }
            else if (type == "vn") {
                float x, y, z; iss >> x >> y >> z;
                temp_normals.push_back(fvector(x, y, z));
            }
            else if (type == "f") {
                std::string v[3];
                if (!(iss >> v[0] >> v[1] >> v[2])) continue;
                for (int i = 0; i < 3; i++) {
                    std::istringstream vss(v[i]);
                    std::string part;
                    int v_idx = 0, vt_idx = -1, vn_idx = -1;
                    if (std::getline(vss, part, '/')) v_idx = std::stoi(part) - 1;
                    if (std::getline(vss, part, '/')) { if (!part.empty()) vt_idx = std::stoi(part) - 1; }
                    if (std::getline(vss, part, '/')) { if (!part.empty()) vn_idx = std::stoi(part) - 1; }
                    if (v_idx < 0 || v_idx >= (int)temp_vertices.size()) continue;
                    int current_index = data.Vertices.Num();
                    data.Vertices.Add(temp_vertices[v_idx]);
                    data.UV0.Add((vt_idx >= 0 && vt_idx < (int)temp_uvs.size()) ? temp_uvs[vt_idx] : fvector2d(0, 0));
                    data.Normals.Add((vn_idx >= 0 && vn_idx < (int)temp_normals.size()) ? temp_normals[vn_idx] : fvector(0, 0, 1));
                    data.VertexColors.Add(FColor(255, 255, 255, 255));
                    data.Triangles.Add(current_index);
                }
            }
        }
        file.close();

        std::string p = pathKey;
        for (auto& c : p) c = tolower(c);
        if (p.find("sheriff") == std::string::npos) {
            for (int i = 0; i + 2 < data.Triangles.Num(); i += 3) {
                int32_t temp = data.Triangles[i + 1];
                data.Triangles[i + 1] = data.Triangles[i + 2];
                data.Triangles[i + 2] = temp;
            }
        }
        if (data.Vertices.Num() > 0) ModelCache[pathKey] = std::move(data);
        if (ModelCache.count(pathKey)) return ModelCache[pathKey];
        return empty;
    }

    static std::atomic<bool> g_CacheThreadRunning = false;

    void HardCacheModelsFromDisk() {
        if (ModelsLoadedToRAM || !UWorldSave) return;
        if (g_CacheThreadRunning) return;

        g_CacheThreadRunning = true;
        ModelsLoadedToRAM = true;

        std::thread([]() {
            DownloadMissingAssets();

            std::string baseDir = GetPublicPath();

            const char* weaponBases[] = { "vandal", "phantom" };
            const char* otherWeapons[] = {
            "bulldog", "guardian", "sheriff", "ghost",
            "bucky", "judge", "frenzy", "bicak",
            "spectre", "stinger", "marshal", "operator", "ares",
            "odin", "classic", "shorty"
            };

            for (auto base : weaponBases) {
                ParseOBJFile((baseDir + base + ".obj").c_str());
                for (int i = 1; i <= 6; i++) {
                    std::string skinPath = baseDir + base + "_skin" + std::to_string(i) + ".obj";
                    if (GetFileAttributesA(skinPath.c_str()) != INVALID_FILE_ATTRIBUTES)
                        ParseOBJFile(skinPath.c_str());
                }
            }

            for (auto name : otherWeapons) {
                std::string objPath = baseDir + name + ".obj";
                if (GetFileAttributesA(objPath.c_str()) != INVALID_FILE_ATTRIBUTES)
                    ParseOBJFile(objPath.c_str());
            }

            std::string textPath = baseDir + "text.obj";
            if (GetFileAttributesA(textPath.c_str()) != INVALID_FILE_ATTRIBUTES)
                ParseOBJFile(textPath.c_str());

            g_CacheThreadRunning = false;
            }).detach();
    }

    void PreCacheAllVisuals() {
        if (UWorldSave != LastWorldPtr) {
            TextureCache.clear();
            LastWorldPtr = UWorldSave;
            ModelsLoadedToRAM = false;
            LastWeaponProcessed = 0;
            g_CacheThreadRunning = false;
        }
        HardCacheModelsFromDisk();
    }

    void ReplaceTextMeshWith3DModel(currentequippable* Weapon, const char* objFilePath) {
        if (!Weapon || !memory::IsValidPointer((uintptr_t)Weapon)) return;
        if (!UWorldSave || !memory::IsValidPointer((uintptr_t)UWorldSave)) return;

        auto* OriginalMesh = Weapon->GetMesh1P();
        if (!OriginalMesh || !memory::IsValidPointer((uintptr_t)OriginalMesh)) return;

        static uobject* ProcMeshClass = (uobject*)uobject::find_object<uclass*>(L"ProceduralMeshComponent.ProceduralMeshComponent");
        static uobject* AddComponentFunc = (uobject*)uobject::find_object<uclass*>(L"ShooterGame.ShooterBlueprintLibrary.AddComponentByClass");
        static uobject* CreateMeshFunc = (uobject*)uobject::find_object<uclass*>(L"ProceduralMeshComponent.ProceduralMeshComponent.CreateMeshSection");

        if (!ProcMeshClass || !AddComponentFunc || !CreateMeshFunc) return;

        struct { AActor* Actor; UActorComponent* ComponentClass; UActorComponent* ReturnValue; }
        AddParams{ (AActor*)Weapon, (UActorComponent*)ProcMeshClass, nullptr };
        variables.blueprints->process_event(AddComponentFunc, &AddParams);

        auto* ProcMesh = (uskeletalmeshcomponent*)AddParams.ReturnValue;
        if (!ProcMesh || !memory::IsValidPointer((uintptr_t)ProcMesh)) return;

        const MeshData& mesh = ParseOBJFile(objFilePath);
        if (mesh.Vertices.Num() == 0) return;

        struct {
            int32_t SectionIndex; tarray<fvector> Vertices; tarray<int32_t> Triangles;
            tarray<fvector> Normals; tarray<fvector2d> UV0; tarray<FColor> VertexColors;
            tarray<FProcMeshTangent> Tangents; bool bCreateCollision;
        } CreateParams = { 0, mesh.Vertices, mesh.Triangles, mesh.Normals, mesh.UV0, mesh.VertexColors, mesh.Tangents, false };

        ((uobject*)ProcMesh)->process_event(CreateMeshFunc, &CreateParams);

        uobject* GlowMaterial = uobject::static_load_object(nullptr, nullptr,
            L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI");

        if (GlowMaterial && memory::IsValidPointer((uintptr_t)GlowMaterial)) {
            uobject* DynMat = ProcMesh->create_and_set_material_instance_dynamic_from_material(0, GlowMaterial);
            if (DynMat && memory::IsValidPointer((uintptr_t)DynMat)) {
                auto* matInstance = (UMaterialInstanceDynamic*)DynMat;
                flinearcolor white = flinearcolor(1.0f, 1.0f, 1.0f, 1.0f);
                matInstance->set_vector_parameter_value(string::string_to_name(L"Base Color"), white);
                matInstance->set_vector_parameter_value(string::string_to_name(L"Emissive Color"), white);
                matInstance->set_scalar_parameter_value(string::string_to_name(L"EmissiveIntensity"), 5.0f);
            }
        }

        USceneComponentHelpers::AttachTo(ProcMesh, (uskeletalmeshcomponent*)OriginalMesh,
            string::string_to_name(L"R_WeaponPoint"), 1, 1, 1, false);
        SetComponentVisibility((USceneComponent*)ProcMesh, true, true);

        ProcMesh->SetRelativeScale3D(fvector(
            globals::misc::text_scale_x,
            globals::misc::text_scale_y,
            globals::misc::text_scale_z
        ));
        USceneComponentHelpers::SetRelativeRotation(ProcMesh,
            FRotator{ globals::misc::text_rot_pitch,
                      globals::misc::text_rot_yaw,
                      globals::misc::text_rot_roll });
        USceneComponentHelpers::SetRelativeLocation(ProcMesh,
            fvector(globals::misc::text_pos_x,
                globals::misc::text_pos_y,
                globals::misc::text_pos_z));

        TextMesh = (UProceduralMeshComponent*)ProcMesh;
        text_meshcreated = true;
        WeaponTextMeshMap[(uintptr_t)Weapon] = TextMesh;
    }

    void ReplaceWeaponMeshWith3DModel(currentequippable* Weapon, const char* objFilePath, const wchar_t* texFilePath) {
        if (!Weapon || !memory::IsValidPointer((uintptr_t)Weapon)) return;
        if (!UWorldSave || !memory::IsValidPointer((uintptr_t)UWorldSave)) return;

        auto* OriginalMesh = Weapon->GetMesh1P();
        if (!OriginalMesh || !memory::IsValidPointer((uintptr_t)OriginalMesh)) return;

        {
            USceneComponent* sceneComp = reinterpret_cast<USceneComponent*>(OriginalMesh);
            tarray<USceneComponent*> children = GetChildrenComponents(sceneComp, true);
            static uobject* DestroyComponentFunc = uobject::find_object<uobject*>(L"Engine.Actor.DestroyComponent");
            for (int i = 0; i < children.Num(); i++) {
                if (!children[i] || !memory::IsValidPointer((uintptr_t)children[i])) continue;
                fstring childName = system::get_object_name((uobject*)children[i]);
                if (childName.to_str().find("ProceduralMesh") != std::string::npos) {
                    if (DestroyComponentFunc) {
                        struct { UActorComponent* Component; } Args = { (UActorComponent*)children[i] };
                        ((uobject*)Weapon)->process_event(DestroyComponentFunc, &Args);
                    }
                    break;
                }
            }
        }

        std::wstring wTexPath(texFilePath);
        std::string  texKey(wTexPath.begin(), wTexPath.end());
        uobject* WeaponTexture = nullptr;

        if (TextureCache.size() > 50) TextureCache.clear();

        if (TextureCache.count(texKey) && IsValidUObject(TextureCache[texKey])) {
            WeaponTexture = TextureCache[texKey];
        }
        else {
            WeaponTexture = system::import_file_as_texture2d(UWorldSave, fstring(texFilePath));
            if (WeaponTexture) TextureCache[texKey] = WeaponTexture;
        }

        static uobject* ProcMeshClass = (uobject*)uobject::find_object<uclass*>(L"ProceduralMeshComponent.ProceduralMeshComponent");
        static uobject* AddComponentFunc = (uobject*)uobject::find_object<uclass*>(L"ShooterGame.ShooterBlueprintLibrary.AddComponentByClass");
        static uobject* CreateMeshFunc = (uobject*)uobject::find_object<uclass*>(L"ProceduralMeshComponent.ProceduralMeshComponent.CreateMeshSection");

        if (!ProcMeshClass || !AddComponentFunc || !CreateMeshFunc) return;

        struct { AActor* Actor; UActorComponent* ComponentClass; UActorComponent* ReturnValue; }
        AddParams{ (AActor*)Weapon, (UActorComponent*)ProcMeshClass, nullptr };
        variables.blueprints->process_event(AddComponentFunc, &AddParams);

        auto* ProcMesh = (uskeletalmeshcomponent*)AddParams.ReturnValue;
        if (!ProcMesh || !memory::IsValidPointer((uintptr_t)ProcMesh)) return;

        const MeshData& mesh = ParseOBJFile(objFilePath);
        if (mesh.Vertices.Num() == 0) return;

        struct {
            int32_t SectionIndex; tarray<fvector> Vertices; tarray<int32_t> Triangles;
            tarray<fvector> Normals; tarray<fvector2d> UV0; tarray<FColor> VertexColors;
            tarray<FProcMeshTangent> Tangents; bool bCreateCollision;
        } CreateParams = { 0, mesh.Vertices, mesh.Triangles, mesh.Normals, mesh.UV0, mesh.VertexColors, mesh.Tangents, false };

        ((uobject*)ProcMesh)->process_event(CreateMeshFunc, &CreateParams);

        if (WeaponTexture) {
            uobject* MasterMat = OriginalMesh->GetMaterial(0);
            if (MasterMat) {
                uobject* DynMat = ProcMesh->create_and_set_material_instance_dynamic_from_material(0, MasterMat);
                if (DynMat) {
                    auto* matInstance = (UMaterialInstanceDynamic*)DynMat;
                    const wchar_t* pNames[] = { L"BaseColor", L"Diffuse", L"Albedo", L"Texture" };
                    for (auto p : pNames)
                        matInstance->set_texture_parameter_value(string::string_to_name(p), WeaponTexture);
                    matInstance->set_scalar_parameter_value2(string::string_to_name(L"TwoSided"), 1.0f);
                }
            }
        }

        USceneComponentHelpers::AttachTo(ProcMesh, (uskeletalmeshcomponent*)OriginalMesh,
            string::string_to_name(L"R_WeaponPoint"), 1, 1, 1, false);
        SetComponentVisibility((USceneComponent*)OriginalMesh, false, true);
        SetComponentVisibility((USceneComponent*)ProcMesh, true, true);

        fstring converted_name = helper::convert_weapon_name(system::get_object_name((uobject*)Weapon));
        std::wstring wNameMesh = converted_name.wide();

        if (wNameMesh == L"Melee") {
            ProcMesh->SetRelativeScale3D(fvector(1.35f, 1.35f, 1.35f));
            USceneComponentHelpers::SetRelativeRotation(ProcMesh, FRotator{ -46.6f, -103.2f, 93.1f });
            USceneComponentHelpers::SetRelativeLocation(ProcMesh, fvector(-19.93f, -1.05f, -0.70f));
        }
        else if (wNameMesh == L"Spectre") {
            ProcMesh->SetRelativeScale3D(fvector(
                globals::misc::spectre_scale_x,
                globals::misc::spectre_scale_y,
                globals::misc::spectre_scale_z
            ));
            USceneComponentHelpers::SetRelativeRotation(ProcMesh, FRotator{
                globals::misc::spectre_rot_pitch,
                globals::misc::spectre_rot_yaw,
                globals::misc::spectre_rot_roll
                });
            USceneComponentHelpers::SetRelativeLocation(ProcMesh, fvector(
                globals::misc::spectre_pos_x,
                globals::misc::spectre_pos_y,
                globals::misc::spectre_pos_z
            ));
        }
        else {
            ProcMesh->SetRelativeScale3D(fvector(1.5f, 1.5f, 1.5f));
            USceneComponentHelpers::SetRelativeRotation(ProcMesh, FRotator{ 0.f, 90.f, -90.f });
            USceneComponentHelpers::SetRelativeLocation(ProcMesh, fvector(-0.9434f, 0.943392f, -2.83019f));
        }

        LastWeaponProcessed = (uintptr_t)Weapon;
    }











    static float normalize_angle(float angle) {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    }





    auto SetCameraCachePOVHook(uintptr_t PlayerCameraManager, FMinimalViewInfo* ViewInfo)
    {
        bool aimbot_key_current = GetAsyncKeyState(globals::aimbot::a1m_k3y) != 0;

        // === SILENT AIM ===
        if (globals::aimbot::silent && !globals::misc::tperson && !aim_check) {
            if (GetAsyncKeyState(globals::aimbot::a1m_k3y)) {
                if (first_location) {
                    aim_check = true;
                    first_camera_location = controllers->get_control_rotation();
                    first_camera_rotation = ViewInfo->Rotation;
                    aim_check = false;
                    first_location = false;
                }
                second_locked_camera = true;
                finished_hook = false;
            }
            else if (!first_location) {
                aim_check = true;
                controllers->set_control_rotation(first_camera_location);
                if (controllers->get_control_rotation() == first_camera_location &&
                    ViewInfo->Rotation == first_camera_rotation) {
                    finished_hook = true;
                    second_locked_camera = false;
                }
                first_location = true;
            }
        }
        else {
            second_locked_camera = false;
            first_camera_location = fvector();
            aim_check = false;
            finished_hook = false;
            first_location = true;
        }

        if (globals::aimbot::silent && second_locked_camera && ViewInfo != nullptr && first_camera_rotation != fvector()) {
            ViewInfo->Rotation = first_camera_rotation;
        }


        static float spinAngle = 0.0f;
        static bool jitterFlip = false;
        static int threewayStep = 0;
        static bool aaEnabled = false;


        if (!globals::misc::SpinBot) {
            aaEnabled = false;

            if (ViewInfo) {
                LocalCameraRotation = ViewInfo->Rotation;
            }

            if (globals::misc::tperson) {
                float radPitch = ViewInfo->Rotation.x * (M_PI / 180.0f);
                float radYaw = ViewInfo->Rotation.y * (M_PI / 180.0f);

                fvector forward;
                forward.x = cosf(radPitch) * cosf(radYaw);
                forward.y = cosf(radPitch) * sinf(radYaw);
                forward.z = sinf(radPitch);

                ViewInfo->Location.x -= forward.x * globals::misc::PlayerDistance;
                ViewInfo->Location.y -= forward.y * globals::misc::PlayerDistance;
                ViewInfo->Location.z -= forward.z * globals::misc::PlayerDistance;
            }

            if (globals::misc::aspectratio && ViewInfo) {
                ViewInfo->bConstrainAspectRatio = true;
                ViewInfo->AspectRatio = globals::misc::aspectfloat;
            }

            aimbot_key_pressed_last_frame = aimbot_key_current;
            SetCameraCachePOVOriginal(PlayerCameraManager, ViewInfo);
            return;
        }


        float deltaX = 0.f, deltaY = 0.f;
        controllers->GetInputMouseDelta(deltaX, deltaY);
        float sensitivity = controllers->GetMouseSensitivity();

        LocalCameraRotation.x += deltaY * sensitivity;
        LocalCameraRotation.y += deltaX * sensitivity;

        ViewInfo->Rotation = LocalCameraRotation;
        character->K2_SetActorRelativeRotation(fvector{ 0, LocalCameraRotation.y, 0 }, false, false);


        static bool lastTState = false;
        bool currentTState = (GetAsyncKeyState(0x54) & 1);

        if (currentTState && !lastTState) {
            aaEnabled = !aaEnabled;
            if (!aaEnabled) {
                spinAngle = LocalCameraRotation.y;
                threewayStep = 0;
                jitterFlip = false;
            }
        }
        lastTState = currentTState;


        if (GetAsyncKeyState('J') & 0x8000) {
            globals::misc::yaw_add = -90.0f;
        }
        else if (GetAsyncKeyState('L') & 0x8000) {
            globals::misc::yaw_add = 90.0f;
        }


        if (aaEnabled) {
            float fakeYaw = LocalCameraRotation.y;
            float fakePitch = LocalCameraRotation.x;


            switch (globals::misc::pitch_mode) {
            case 0: break;
            case 1: fakePitch = 89.0f; break;
            case 2: fakePitch = -89.0f; break;
            case 3: fakePitch = globals::misc::pitch_value; break;
            }


            fakeYaw += globals::misc::yaw_add;


            bool backKeyPressed = false;
            float direction = 0.0f;

            if (GetAsyncKeyState(globals::misc::snap_left_key) & 0x8000) {
                backKeyPressed = true;
                direction = -90.f;
            }
            else if (GetAsyncKeyState(globals::misc::snap_right_key) & 0x8000) {
                backKeyPressed = true;
                direction = 90.f;
            }
            else if (GetAsyncKeyState(globals::misc::snap_back_key) & 0x8000) {
                backKeyPressed = true;
                direction = 180.f;
            }

            if (backKeyPressed) {
                if (globals::misc::jitter_enabled && globals::misc::jitter_on_back) {
                    jitterFlip = !jitterFlip;
                    fakeYaw += direction + (jitterFlip ? globals::misc::jitter_range : -globals::misc::jitter_range);
                }
                else {
                    fakeYaw += direction;
                }
            }
            else {



                if (globals::misc::aa_backwards) {
                    fakeYaw += 180.f;
                }


                if (globals::misc::aa_spin) {
                    spinAngle = fmodf(spinAngle + globals::misc::spinvalue, 360.f);
                    fakeYaw += spinAngle;
                }


                if (globals::misc::aa_jitter) {
                    jitterFlip = !jitterFlip;
                    fakeYaw += jitterFlip ? globals::misc::jitter_range : -globals::misc::jitter_range;
                }


                if (globals::misc::aa_threeway) {
                    threewayStep = (threewayStep + 1) % 3;
                    fakeYaw += (threewayStep == 0) ? 90.f : (threewayStep == 1) ? -90.f : 180.f;
                }


                if (globals::misc::aa_desync) {
                    static float desync_timer = 0.0f;
                    desync_timer += 0.02f;

                    float wave_desync = sinf(desync_timer) * globals::misc::desync_range;

                    if (globals::misc::jitter_enabled) {
                        static int desync_jitter_counter = 0;
                        desync_jitter_counter++;
                        if (desync_jitter_counter % 8 == 0) {
                            wave_desync += (rand() % 60 - 30);
                        }
                    }
                    fakeYaw += wave_desync;
                }


                if (globals::misc::aa_atomic) {
                    static float atomic_timer = 0.0f;
                    atomic_timer += 0.01f * globals::misc::atomic_speed;

                    switch (globals::misc::atomic_mode) {
                    case 0:
                        fakeYaw += sinf(atomic_timer * 5.0f) * 120.0f + cosf(atomic_timer * 3.0f) * 60.0f;
                        break;
                    case 1:
                        fakeYaw += (sinf(atomic_timer * 8.0f) > 0 ? 180.0f : -180.0f);
                        break;
                    case 2: {
                        static bool flicker_state = false;
                        static int flicker_counter = 0;
                        flicker_counter++;
                        if (flicker_counter % 5 == 0) {
                            flicker_state = !flicker_state;
                            fakeYaw += flicker_state ? 135.0f : -135.0f;
                        }
                        break;
                    }
                    }

                    if (globals::misc::jitter_enabled) {
                        fakeYaw += jitterFlip ? globals::misc::jitter_range * 0.5f : -globals::misc::jitter_range * 0.5f;
                        jitterFlip = !jitterFlip;
                    }
                }


                if (globals::misc::aa_prediction_breaker) {
                    static float breaker_timer = 0.0f;
                    static float flick_timer = 0.0f;

                    breaker_timer += 0.01f * globals::misc::breaker_intensity;

                    float breaker_yaw = sinf(breaker_timer * 3.0f) * 90.0f +
                        cosf(breaker_timer * 7.0f) * 45.0f +
                        sinf(breaker_timer * 13.0f) * 30.0f;

                    fakeYaw += breaker_yaw;

                    flick_timer += 0.01f;
                    if (flick_timer > 2.0f) {
                        flick_timer = 0.0f;
                        fakeYaw += 180.0f;
                    }
                }
            }


            controllers->set_control_rotation(fvector(fakePitch, fakeYaw, 0));

            if (auto mesh3p = character->get_mesh()) {
                mesh3p->set_world_rotation(fvector(0, fakeYaw, 0), 0, 0);
            }
        }
        else {
            controllers->set_control_rotation(fvector(LocalCameraRotation.x, LocalCameraRotation.y, 0));

            if (auto mesh3p = character->get_mesh()) {
                mesh3p->set_world_rotation(fvector(0, LocalCameraRotation.y, 0), 0, 0);
            }
        }


        if (globals::misc::tperson) {
            float radPitch = ViewInfo->Rotation.x * (M_PI / 180.0f);
            float radYaw = ViewInfo->Rotation.y * (M_PI / 180.0f);

            fvector forward;
            forward.x = cosf(radPitch) * cosf(radYaw);
            forward.y = cosf(radPitch) * sinf(radYaw);
            forward.z = sinf(radPitch);

            ViewInfo->Location.x -= forward.x * globals::misc::PlayerDistance;
            ViewInfo->Location.y -= forward.y * globals::misc::PlayerDistance;
            ViewInfo->Location.z -= forward.z * globals::misc::PlayerDistance;
        }


        if (globals::misc::aspectratio && ViewInfo) {
            ViewInfo->bConstrainAspectRatio = true;
            ViewInfo->AspectRatio = globals::misc::aspectfloat;
        }

        aimbot_key_pressed_last_frame = aimbot_key_current;
        SetCameraCachePOVOriginal(PlayerCameraManager, ViewInfo);
    }





    static flinearcolor accent_color = { 0.6f, 0.6f, 0.6f, 1.0f }; // OUR COLOR (VIBRANT GREY)
    const flinearcolor background_color = { 0.01f, 0.01f, 0.01f, 1.0f };
    const flinearcolor border_color = { 0.10f, 0.10f, 0.10f, 1.0f };
    const flinearcolor panel_color = { 0.015f, 0.015f, 0.015f, 1.0f };
    const flinearcolor text_color = { 0.9f, 0.9f, 0.9f, 1.0f };
    const flinearcolor hover_color = { 0.2f, 0.2f, 0.2f, 1.0f };
    const flinearcolor disabled_color = { 0.5f, 0.5f, 0.5f, 1.0f };
    const flinearcolor dark_grey_color = { 0.15f, 0.15f, 0.15f, 1.0f };  // Dark grey for unfilled part  
    const flinearcolor shadow_color = { 0.1f, 0.1f, 0.1f, 0.6f };        // Subtle shadow effect  
    const flinearcolor dark_border_color = { 0.15f, 0.15f, 0.15f, 1.0f };

    void DrawSmoothFilledCircle(fvector2d center, float radius, flinearcolor color, ucanvas* canvas)
    {
        constexpr float PI = 3.14159265359f;

        fvector2d p0 = { center.x + radius * cosf(0.0f), center.y + radius * sinf(0.0f) };
        fvector2d p1 = { center.x + radius * cosf(PI / 3.0f), center.y + radius * sinf(PI / 3.0f) };
        fvector2d p2 = { center.x + radius * cosf(2.0f * PI / 3.0f), center.y + radius * sinf(2.0f * PI / 3.0f) };
        fvector2d p3 = { center.x + radius * cosf(PI), center.y + radius * sinf(PI) };
        fvector2d p4 = { center.x + radius * cosf(4.0f * PI / 3.0f), center.y + radius * sinf(4.0f * PI / 3.0f) };
        fvector2d p5 = { center.x + radius * cosf(5.0f * PI / 3.0f), center.y + radius * sinf(5.0f * PI / 3.0f) };

        canvas->k2_drawline(center, p0, 1.0f, color);
        canvas->k2_drawline(center, p1, 1.0f, color);
        canvas->k2_drawline(p0, p1, 1.0f, color);

        canvas->k2_drawline(center, p1, 1.0f, color);
        canvas->k2_drawline(center, p2, 1.0f, color);
        canvas->k2_drawline(p1, p2, 1.0f, color);

        canvas->k2_drawline(center, p2, 1.0f, color);
        canvas->k2_drawline(center, p3, 1.0f, color);
        canvas->k2_drawline(p2, p3, 1.0f, color);

        canvas->k2_drawline(center, p3, 1.0f, color);
        canvas->k2_drawline(center, p4, 1.0f, color);
        canvas->k2_drawline(p3, p4, 1.0f, color);

        canvas->k2_drawline(center, p4, 1.0f, color);
        canvas->k2_drawline(center, p5, 1.0f, color);
        canvas->k2_drawline(p4, p5, 1.0f, color);

        canvas->k2_drawline(center, p5, 1.0f, color);
        canvas->k2_drawline(center, p0, 1.0f, color);
        canvas->k2_drawline(p5, p0, 1.0f, color);
    }


    void DrawSimpleCircle(fvector2d center, float radius, flinearcolor color, ucanvas* canvas)
    {
        constexpr float PI = 3.14159265359f;

        float angle0 = 0.0f;
        float angle1 = PI / 3.0f;
        float angle2 = 2.0f * PI / 3.0f;
        float angle3 = PI;
        float angle4 = 4.0f * PI / 3.0f;
        float angle5 = 5.0f * PI / 3.0f;

        fvector2d p0 = { center.x + cosf(angle0) * radius, center.y + sinf(angle0) * radius };
        fvector2d p1 = { center.x + cosf(angle1) * radius, center.y + sinf(angle1) * radius };
        fvector2d p2 = { center.x + cosf(angle2) * radius, center.y + sinf(angle2) * radius };
        fvector2d p3 = { center.x + cosf(angle3) * radius, center.y + sinf(angle3) * radius };
        fvector2d p4 = { center.x + cosf(angle4) * radius, center.y + sinf(angle4) * radius };
        fvector2d p5 = { center.x + cosf(angle5) * radius, center.y + sinf(angle5) * radius };

        canvas->k2_drawline(p0, p1, 1.0f, color);
        canvas->k2_drawline(p1, p2, 1.0f, color);
        canvas->k2_drawline(p2, p3, 1.0f, color);
        canvas->k2_drawline(p3, p4, 1.0f, color);
        canvas->k2_drawline(p4, p5, 1.0f, color);
        canvas->k2_drawline(p5, p0, 1.0f, color);
    }

    namespace radar
    {
        static fvector pRadar;
        /*void DrawCircleRadar(int x, int y, int radius, flinearcolor color, ucanvas* cvs)
        {
            DrawFilledCircle(fvector2d(x, y), radius, color, cvs);
        }*/

        void DrawCircleRadar(int x, int y, int radius, flinearcolor color, ucanvas* canvas)
        {
            // Outer glow
            flinearcolor glowColor = color;
            glowColor.a *= 0.25f;
            DrawSmoothFilledCircle(fvector2d(x, y), radius + 2, glowColor, canvas);

            // Solid core
            DrawSmoothFilledCircle(fvector2d(x, y), radius, color, canvas);
        }


        fvector WorldRadar(fvector srcPos, fvector distPos, float yaw, float radarX, float radarY, float size)
        {
            auto cosYaw = cos(DegreeToRadian(yaw));
            auto sinYaw = sin(DegreeToRadian(yaw));

            auto deltaX = srcPos.x - distPos.x;
            auto deltaY = srcPos.y - distPos.y;

            auto locationX = (float)(deltaY * cosYaw - deltaX * sinYaw) / 45.f;
            auto locationY = (float)(deltaX * cosYaw + deltaY * sinYaw) / 45.f;

            if (locationX > (size - 2.f))
                locationX = (size - 2.f);
            else if (locationX < -(size - 2.f))
                locationX = -(size - 2.f);

            if (locationY > (size - 6.f))
                locationY = (size - 6.f);
            else if (locationY < -(size - 6.f))
                locationY = -(size - 6.f);

            return fvector((int)(-locationX + radarX), (int)(locationY + radarY), 0);
        }

        static float closestDistance = FLT_MAX;
        static ashootercharacter* pulsingActor = nullptr;

        inline float GetPulseScale()
        {
            float time = GetTickCount64() / 1000.f; // system time in seconds
            return 1.0f + 0.25f * sinf(time * 5.0f); // pulsing between 1.0x - 1.25x
        }


        void DrawRadar(fvector EntityPos, acknowledgedpawn* MyPawns, ucanvas* cvs, ashootercharacter* actor)
        {
            if (!actor || !actor->is_alive())
                return; // skip dead enemies

            aplayercameramanager* camerap = controllers->get_camera_manager();
            int radar_posX = pRadar.x + 135;
            int radar_posY = pRadar.y + 135;

            uint64_t LocalRootComp = memory::read<uint64_t>((uint64_t)MyPawns + offsets::Rootcomponent);
            fvector LocalPos = memory::read<fvector>(LocalRootComp + offsets::root_position);
            FMinimalViewInfo camerae = memory::read<FMinimalViewInfo>((uint64_t)camerap + offsets::CameraRadar);

            fvector Radar2D = WorldRadar(LocalPos, EntityPos, camerae.Rotation.y, radar_posX, radar_posY, 135.f);

            float distance = (EntityPos - LocalPos).length();
            bool Visible = controllers->line_of_sight(actor);

            // Determine if this is the closest actor this frame
            if (distance < closestDistance)
            {
                closestDistance = distance;
                pulsingActor = actor;
            }

            float baseSize = 5.0f;
            float pulseSize = (actor == pulsingActor && distance < 1000.f) ? baseSize * GetPulseScale() : baseSize;

            // Shadow effect
            flinearcolor shadowColor = { 0.f, 0.f, 0.f, 0.3f };
            DrawCircleRadar(Radar2D.x + 1, Radar2D.y + 1, pulseSize, shadowColor, cvs);

            // Main radar blip
            if (Visible) {
                static flinearcolor greenColor{ 0.0f, 1.0f, 0.0f, 1.0f };
                DrawCircleRadar(Radar2D.x, Radar2D.y, pulseSize, greenColor, cvs);
            }
            else {
                static flinearcolor redColor{ 1.0f, 0.0f, 0.0f, 1.0f };
                DrawCircleRadar(Radar2D.x, Radar2D.y, pulseSize, redColor, cvs);
            }
        }

        // Call this at the beginning of each frame, before drawing enemies
        void ResetRadarPulse()
        {
            closestDistance = DBL_MAX;
            pulsingActor = nullptr;
        }
    }













    void meshp1_material1(acknowledgedpawn* pawn, ashootercharacter* shooter_character)
    {

        uobject* material = uobject::static_load_object(
            nullptr,
            nullptr,
            L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI"
        );

        fname first_name = string::string_to_name(L"Base Color");
        fname second_name = string::string_to_name(L"Emissive Color");

        UPrimitiveComponent* mesh1p = memory::read<UPrimitiveComponent*>(uintptr_t(pawn) + offsets::mesh1p);
        UPrimitiveComponent* meshOverlay = memory::read<UPrimitiveComponent*>(uintptr_t(pawn) + offsets::mesh1p_overlay);

        /*   uskeletalmeshcomponent* mesh1p = pawn->GetMesh11P();
           uskeletalmeshcomponent* meshOverlay = pawn->GetOverlayMesh11P();*/



        if (mesh1p) {
            auto num_materials = mesh1p->get_num_materials();
            for (int i = 0; i < num_materials; i++) {
                uobject* material_instance_dynamic = mesh1p->create_and_set_material_instance_dynamic_from_material(i, material);
                if (material_instance_dynamic) {

                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(first_name, { globals::misc::handcolor.r * globals::misc::handbright, globals::misc::handcolor.g * globals::misc::handbright, globals::misc::handcolor.b * globals::misc::handbright });
                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(second_name, { globals::misc::handcolor.r * globals::misc::handbright, globals::misc::handcolor.g * globals::misc::handbright, globals::misc::handcolor.b * globals::misc::handbright });
                }
            }
        }

        if (meshOverlay) {
            auto num_materials = meshOverlay->get_num_materials();
            for (int i = 0; i < num_materials; i++) {
                auto material_instance_dynamic = meshOverlay->create_and_set_material_instance_dynamic_from_material(i, material);
                if (material_instance_dynamic) {

                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(first_name, { globals::misc::handcolor.r * globals::misc::handbright, globals::misc::handcolor.g * globals::misc::handbright, globals::misc::handcolor.b * globals::misc::handbright });
                    material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(second_name, { globals::misc::handcolor.r * globals::misc::handbright, globals::misc::handcolor.g * globals::misc::handbright,globals::misc::handcolor.b * globals::misc::handbright });
                }
            }
        }


        if (globals::misc::HandChamsRbg) {

            rainbowTime += 0.005f;
            flinearcolor rainbow = GetRainbowColor(rainbowTime);

            if (mesh1p) {
                auto num_materials = mesh1p->get_num_materials();
                for (int i = 0; i < num_materials; i++) {
                    uobject* material_instance_dynamic = mesh1p->create_and_set_material_instance_dynamic_from_material(i, material);
                    if (material_instance_dynamic) {

                        material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(first_name, { rainbow.r * globals::misc::handbright, rainbow.g * globals::misc::handbright, rainbow.b * globals::misc::handbright });
                        material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(second_name, { rainbow.r * globals::misc::handbright, rainbow.g * globals::misc::handbright, rainbow.b * globals::misc::handbright });
                    }
                }
            }

            if (meshOverlay) {
                auto num_materials = meshOverlay->get_num_materials();
                for (int i = 0; i < num_materials; i++) {
                    auto material_instance_dynamic = meshOverlay->create_and_set_material_instance_dynamic_from_material(i, material);
                    if (material_instance_dynamic) {

                        material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(first_name, { rainbow.r * globals::misc::handbright, rainbow.g * globals::misc::handbright, rainbow.b * globals::misc::handbright });
                        material_instance_dynamic->cast<UMaterialInstanceDynamic>()->set_vector_parameter_value1(second_name, { rainbow.r * globals::misc::handbright, rainbow.g * globals::misc::handbright, rainbow.b * globals::misc::handbright });
                    }
                }
            }
        }
    }



    void meshp1_material(acknowledgedpawn* MyPawn, ashootercharacter* characterz)
    {
        UPrimitiveComponent* mesh = memory::read<UPrimitiveComponent*>(uintptr_t(MyPawn) + offsets::mesh1p);
        UPrimitiveComponent* meshOverlay = memory::read<UPrimitiveComponent*>(uintptr_t(MyPawn) + offsets::mesh1p_overlay);

        uobject* material = nullptr;

        switch (globals::misc::materials) {
        case 0:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/3P_Weapon_Translucent_Mat"
            );
            break;
        case 1:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Characters/Vampire/S0/VFX/Ability_X/1P_Vampire_Tattoo_X_S0_MI_VFX.1P_Vampire_Tattoo_X_S0_MI_VFX"
            );
            break;
        case 2:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Gems/1P_Gem_MAT"
            );
            break;
        case 3:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat"
            );
            break;
        case 4:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Sakura/Tritium_Sakura_3P_MI"
            );
            break;
        case 5:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Yellow_MI.Arcade_Emissive_Yellow_MI"
            );
            break;
        case 6:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Red_MI.Arcade_Emissive_Red_MI"
            );
            break;
        case 7:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI"
            );
            break;
        case 8:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Afterglow3/BakedLight/Afterglow3_BakedLight_MI.Afterglow3_BakedLight_MI"
            );
            break;
        case 9:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI"
            );
            break;
        case 10:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI"
            );
            break;
        case 11:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Warring/1P_Melee_Warring_Emissive_Gradient_LUT_MI.1P_Melee_Warring_Emissive_Gradient_LUT_MI"
            );
            break;
        }

        if (material)
        {
            if (mesh && mesh->get_material(0) != material)
                mesh->set_material(0, material);

            if (meshOverlay && meshOverlay->get_material(0) != material)
                meshOverlay->set_material(0, material);
        }
    }
    void Gunp1_material(acknowledgedpawn* MyPawn, ashootercharacter* characterz)
    {
        UPrimitiveComponent* mesh = memory::read<UPrimitiveComponent*>(uintptr_t(MyPawn) + offsets::mesh1p);
        UPrimitiveComponent* meshOverlay = memory::read<UPrimitiveComponent*>(uintptr_t(MyPawn) + offsets::mesh1p_overlay);
        UPrimitiveComponent* gun = memory::read<UPrimitiveComponent*>(uintptr_t(MyPawn) + offsets::mesh1pgun);

        uobject* material = nullptr;

        switch (globals::misc::materials) {
        case 0:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/3P_Weapon_Translucent_Mat"
            );
            break;
        case 1:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Characters/Vampire/S0/VFX/Ability_X/1P_Vampire_Tattoo_X_S0_MI_VFX.1P_Vampire_Tattoo_X_S0_MI_VFX"
            );
            break;
        case 2:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Gems/1P_Gem_MAT"
            );
            break;
        case 3:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat"
            );
            break;
        case 4:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Sakura/Tritium_Sakura_3P_MI"
            );
            break;
        case 5:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Yellow_MI.Arcade_Emissive_Yellow_MI"
            );
            break;
        case 6:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Red_MI.Arcade_Emissive_Red_MI"
            );
            break;
        case 7:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI"
            );
            break;
        case 8:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Afterglow3/BakedLight/Afterglow3_BakedLight_MI.Afterglow3_BakedLight_MI"
            );
            break;
        case 9:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI"
            );
            break;
        case 10:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/VFX/Materials/HunterReveal_MI.HunterReveal_MI"
            );
            break;
        case 11:
            material = uobject::static_load_object(
                nullptr,
                nullptr,
                L"/Game/Equippables/_Core/Materials/SpecialMaterials/Warring/1P_Melee_Warring_Emissive_Gradient_LUT_MI.1P_Melee_Warring_Emissive_Gradient_LUT_MI"
            );
            break;

        }

        if (material)
        {
            if (gun && gun->get_material(0) != material)
                gun->set_material(0, material);
        }
    }



    void use_blind_manager_component(uobject* target_object)
    {
        auto blind_manager = static_cast<UBlindManagerComponent*>(target_object);

        if (blind_manager == nullptr)
            return;

        bool blinded = blind_manager->IsBlinded();

        if (blinded)
        {
            blind_manager->SetBlinded(false);

            blind_manager->ClientCleanseBlinds();
        }
    }



    static bool saved_anti_aim = false;
    static bool saved_spinbot = false;
    static bool saved_tperson = false;
    static bool saved_auto_shot = false;
    static bool states_saved = false;
    static uint64_t no_enemies_start_time = 0;
    static uint64_t characters_found_time = 0;
    static bool waiting_for_reactivation = false;


    static const uint64_t DISABLE_DELAY = 2000;
    static const uint64_t ENABLE_DELAY = 500;





    ucanvas* canvas;

    void draw_rect21(ucanvas* canvas, float x, float y, float width, float height, flinearcolor color) {
        canvas->k2_drawline({ x, y }, { x + width, y }, 1.0f, color);
        canvas->k2_drawline({ x + width, y }, { x + width, y + height }, 1.0f, color);
        canvas->k2_drawline({ x + width, y + height }, { x, y + height }, 1.0f, color);
        canvas->k2_drawline({ x, y + height }, { x, y }, 1.0f, color);
    }

    inline auto DrawBorder(ucanvas* canva, float x, float y, float w, float h, float px, flinearcolor BorderColor) -> void
    {
        draw_rect21(canva, x, (y + h - px), w, px, BorderColor);
        draw_rect21(canva, x, y, px, h, BorderColor);
        draw_rect21(canva, x, y, w, px, BorderColor);
        draw_rect21(canva, (x + w - px), y, px, h, BorderColor);
    }

    void DrawLineCanvas(ucanvas* canvas, int x1, int y1, int x2, int y2, flinearcolor color, int thickness)
    {
        canvas->k2_drawline(fvector2d(x1, y1), fvector2d(x2, y2), thickness, color);
    }


    void draw_line_2(ucanvas* canvas, fvector2d from, fvector2d to, int thickness, flinearcolor color)
    {
        canvas->k2_drawline(from, to, static_cast<float>(thickness), color);
    }









    void meshp3_material12(acknowledgedpawn* pawn, ashootercharacter* shooter_character)
    {
        // All your materials as a selectable array
        static const wchar_t* material_paths[] =
        {
            L"/Game/Equippables/_Core/Materials/SpecialMaterials/Ninja/AK_Ninja_Shuriken_MI.AK_Ninja_Shuriken_MI",
            L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/CyberCity/CyberCity_MI.CyberCity_MI",
            L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/SovWorld/3p_SovWorld_MI.3p_SovWorld_MI",
            L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/UnderTheSea/UnderTheSea_MI.UnderTheSea_MI",
        };

        int material_count = sizeof(material_paths) / sizeof(material_paths[0]);


        int selected_material = globals::misc::chams_material_index;

        if (selected_material < 0 || selected_material >= material_count)
            selected_material = 0;

        // Load selected material dynamically
        uobject* material = uobject::static_load_object(
            nullptr,
            nullptr,
            material_paths[selected_material]
        );

        if (!material) return;

        fname first_name = string::string_to_name(L"Base Color");
        fname second_name = string::string_to_name(L"Emissive Color");

        uskeletalmeshcomponent* myselfchams = shooter_character->GetCosmeticMesh3P();

        if (globals::misc::playerchamsself && myselfchams)
        {
            auto num_materials = myselfchams->get_num_materials();
            for (int i = 0; i < num_materials; i++) {
                uobject* material_instance_dynamic = myselfchams->create_and_set_material_instance_dynamic_from_material(i, material);
                if (material_instance_dynamic)
                {
                    static float t = 0.0f;
                    t += 0.0004f;
                    if (t > 1.0f) t = 0.0f;

                    float r = fabsf(sinf(t * 6.2831f));
                    float g = fabsf(sinf((t + 0.33f) * 6.2831f));
                    float b = fabsf(sinf((t + 0.66f) * 6.2831f));

                    flinearcolor rainbow_color = { r, g, b };

                    auto dynamic_mat = material_instance_dynamic->cast<UMaterialInstanceDynamic>();
                    dynamic_mat->set_vector_parameter_value1(first_name, rainbow_color);
                    dynamic_mat->set_vector_parameter_value1(second_name, rainbow_color);
                }
            }
        }
    }

    //void ashen_crystal_material(acknowledgedpawn* pawn, ashootercharacter* shooter_character)
    //{
    //    if (!shooter_character) return;

    //    UPrimitiveComponent* myselfchams = memory::read<UPrimitiveComponent*>(uintptr_t(pawn) + offsets::mesh_cosmetic_3p);

    //    if (globals::misc::ashen_crystal_enabled && myselfchams)
    //    {
    //        // Load Ashen Crystal material
    //        uobject* material = uobject::static_load_object(nullptr, nullptr,
    //            L"/Game/Equippables/_Core/Materials/SpecialMaterials/Ashen/Ashen_Crystal_v3_MI.Ashen_Crystal_v3_MI");

    //        if (!material) return;

    //        auto num_materials = myselfchams->get_num_materials();

    //        for (int i = 0; i < num_materials; i++) {
    //            auto material_instance_dynamic = myselfchams->create_and_set_material_instance_dynamic_from_material(i, material);

    //            if (material_instance_dynamic)
    //            {
    //                auto dynamic_mat = material_instance_dynamic->cast<UMaterialInstanceDynamic>();
    //                if (!dynamic_mat) continue;

    //                // ParamÃ¨tres Ashen Crystal
    //                fname diffuse_overlay = string::string_to_name(L"Diffuse Overlay Intensity");
    //                fname inner_b_emissive = string::string_to_name(L"Inner (B) Emissive Intenstiy");
    //                fname inner_g_emissive = string::string_to_name(L"Inner (G) Emissive Intenstiy");
    //                fname bump_offset = string::string_to_name(L"Bump Offset Height");
    //                fname texture_tiling = string::string_to_name(L"Texture Tiling");
    //                fname reflection_vector = string::string_to_name(L"Reflection Vector Intensity");
    //                fname flow_map_speed = string::string_to_name(L"Flow Map Speed Y");
    //                fname panner_y = string::string_to_name(L"Panner Y");
    //                fname diffuse_multiply = string::string_to_name(L"Diffuse Multiply");
    //                fname diffuse_power = string::string_to_name(L"Diffuse Power");
    //                fname refraction_bias = string::string_to_name(L"RefractionDepthBias");

    //                // Animation
    //                static float animation_time = 0.0f;
    //                animation_time += 0.08f;
    //                if (animation_time > 6.283f) animation_time = 0.0f;

    //                float animated_flow = 0.1f + sinf(animation_time * 2.0f) * 1.5f;
    //                float animated_panner = 0.05f + cosf(animation_time * 1.5f) * 1.2f;

    //                // ParamÃ¨tres galaxy
    //                dynamic_mat->set_scalar_parameter_value(diffuse_overlay, 12.0f);
    //                dynamic_mat->set_scalar_parameter_value(inner_b_emissive, 8.0f);
    //                dynamic_mat->set_scalar_parameter_value(inner_g_emissive, 1.0f);
    //                dynamic_mat->set_scalar_parameter_value(bump_offset, -5.0f);
    //                dynamic_mat->set_scalar_parameter_value(texture_tiling, 250.0f);
    //                dynamic_mat->set_scalar_parameter_value(reflection_vector, 6.0f);
    //                dynamic_mat->set_scalar_parameter_value(flow_map_speed, animated_flow);
    //                dynamic_mat->set_scalar_parameter_value(panner_y, animated_panner);
    //                dynamic_mat->set_scalar_parameter_value(diffuse_multiply, 5.0f);
    //                dynamic_mat->set_scalar_parameter_value(diffuse_power, 2.0f);
    //                dynamic_mat->set_scalar_parameter_value(refraction_bias, 2.5f);

    //                // Couleurs par dÃ©faut
    //                fname emissive_surface = string::string_to_name(L"Emissive Surface Color");
    //                fname diffuse_tint = string::string_to_name(L"Diffuse Tint");

    //                flinearcolor default_color = { 1.0f, 1.0f, 1.0f, 1.0f };

    //                dynamic_mat->set_vector_parameter_value1(emissive_surface, default_color);
    //                dynamic_mat->set_vector_parameter_value1(diffuse_tint, default_color);
    //            }
    //        }
    //    }
    //}















    bool HasVisibleEnemy(tarray<ashootercharacter*>& actors, aplayercontroller* controllers, ashootercharacter* character) {
        for (int32_t idx = 0; idx < actors.count; ++idx) {
            ashootercharacter* actor = actors[idx];
            if (!actor || actor == character) continue;
            if (!actor->is_alive()) continue;

            if (controllers->line_of_sight(actor)) {
                return true;
            }
        }
        return false;
    }


    bool GetAllActorsSafely(uworld* world, uobject* actor_class, tarray<AGameObject*>* Objects) {
        if (!world || !memory::IsValidPointer((uintptr_t)world)) {
            return false;
        }
        if (!actor_class || !memory::IsValidPointer((uintptr_t)actor_class)) {
            return false;
        }

        memset(Objects, 0, sizeof(*Objects));
        GameplayStatics::GetAllActorsOfClass2(world, actor_class, Objects);

        if (!memory::IsValidPointer((uintptr_t)Objects->data) && Objects->Num() > 0) {
            return false;
        }

        return true;
    }

    void apply_custom_hand_texture(acknowledgedpawn* pawn, ashootercharacter* character, uworld* world)
    {
        if (!pawn || !character || !world) return;
        if (!character->is_alive()) return;

        UPrimitiveComponent* mesh1p = memory::read<UPrimitiveComponent*>(uintptr_t(pawn) + offsets::mesh1p);
        UPrimitiveComponent* meshOverlay = memory::read<UPrimitiveComponent*>(uintptr_t(pawn) + offsets::mesh1p_overlay);

        if (!mesh1p || !meshOverlay) return;
        if (!memory::IsValidPointer((uintptr_t)mesh1p) || !memory::IsValidPointer((uintptr_t)meshOverlay)) return;

        fstring customHandTexturePath = fstring(L"C:/hand.jpg");
        uobject* CustomHandTexture = system::import_file_as_texture2d(world, customHandTexturePath);

        if (!CustomHandTexture || !memory::IsValidPointer((uintptr_t)CustomHandTexture)) return;

        auto handMatPath = L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/Winter/Winter_MI.Winter_MI";
        uobject* handMaterial = uobject::find_object<uobject*>(handMatPath);

        if (!handMaterial) {
            handMaterial = uobject::static_load_object(nullptr, nullptr, handMatPath);
        }

        if (!handMaterial || !memory::IsValidPointer((uintptr_t)handMaterial)) return;

        if (mesh1p) {
            mesh1p->set_material(0, handMaterial);
            uobject* HandDynamicMat = mesh1p->create_and_set_material_instance_dynamic_from_material(0, handMaterial);

            if (HandDynamicMat && memory::IsValidPointer((uintptr_t)HandDynamicMat)) {
                auto* handMat = HandDynamicMat->cast<UMaterialInstanceDynamic>();
                if (handMat) {
                    fname handParam1 = string::string_to_name(crypt(L"Image 1").decrypt());
                    fname handParam2 = string::string_to_name(crypt(L"Image 2").decrypt());

                    handMat->set_texture_parameter_value(handParam1, CustomHandTexture);
                    handMat->set_texture_parameter_value(handParam2, CustomHandTexture);

                    fname baseColorParam = string::string_to_name(crypt(L"Base Color").decrypt());
                    fname diffuseParam = string::string_to_name(crypt(L"Diffuse").decrypt());
                    fname textureParam = string::string_to_name(crypt(L"Texture").decrypt());

                    handMat->set_texture_parameter_value(baseColorParam, CustomHandTexture);
                    handMat->set_texture_parameter_value(diffuseParam, CustomHandTexture);
                    handMat->set_texture_parameter_value(textureParam, CustomHandTexture);
                }
            }
        }

        if (meshOverlay) {
            meshOverlay->set_material(0, handMaterial);
            uobject* OverlayDynamicMat = meshOverlay->create_and_set_material_instance_dynamic_from_material(0, handMaterial);

            if (OverlayDynamicMat && memory::IsValidPointer((uintptr_t)OverlayDynamicMat)) {
                auto* overlayMat = OverlayDynamicMat->cast<UMaterialInstanceDynamic>();
                if (overlayMat) {
                    fname overlayParam1 = string::string_to_name(crypt(L"Image 1").decrypt());
                    fname overlayParam2 = string::string_to_name(crypt(L"Image 2").decrypt());

                    overlayMat->set_texture_parameter_value(overlayParam1, CustomHandTexture);
                    overlayMat->set_texture_parameter_value(overlayParam2, CustomHandTexture);

                    fname baseColorParam = string::string_to_name(crypt(L"Base Color").decrypt());
                    fname diffuseParam = string::string_to_name(crypt(L"Diffuse").decrypt());
                    fname textureParam = string::string_to_name(crypt(L"Texture").decrypt());

                    overlayMat->set_texture_parameter_value(baseColorParam, CustomHandTexture);
                    overlayMat->set_texture_parameter_value(diffuseParam, CustomHandTexture);
                    overlayMat->set_texture_parameter_value(textureParam, CustomHandTexture);
                }
            }
        }
    }





    //using FinisherFn = void* (__fastcall*)(uintptr_t);

    //inline void* PlayFinisherEffect(uintptr_t effect)
    //{
    //    SPOOF_FUNC;
    //    static void* (__fastcall * fn)(uintptr_t) = nullptr;
    //    if (!fn)
    //        fn = reinterpret_cast<FinisherFn>(memory::module_base + offsets::player_finisher);

    //    return fn(effect);
    //}

    //void hk_death(ashootercharacter* shooter_character, UDamageResponse* a2) {
    //    try {
    //        if (!shooter_character || !memory::IsValidPointer((uintptr_t)shooter_character)) {
    //            return oHkDeath(shooter_character, a2);
    //        }

    //        // Get local player context
    //        acknowledgedpawn* pawn = controllers->get_acknowledged_pawn();
    //        ashootercharacter* character_context = character;
    //        acknowledgedpawn* local_pawn_context = pawn;
    //        auto damage_response = a2;

    //        if (!character_context || !local_pawn_context || !damage_response) {
    //            return oHkDeath(shooter_character, a2);
    //        }

    //        // Validate pointers
    //        if (!memory::IsValidPointer((uintptr_t)character_context) ||
    //            !memory::IsValidPointer((uintptr_t)local_pawn_context) ||
    //            !memory::IsValidPointer((uintptr_t)damage_response)) {
    //            return oHkDeath(shooter_character, a2);
    //        }

    //        // Get death reaction component
    //        auto component = (uintptr_t)memory::read<uobject*>((uintptr_t)shooter_character + offsets::death_reaction_component_offset);

    //        if (!component || !memory::IsValidPointer(component)) {
    //            return oHkDeath(shooter_character, a2);
    //        }

    //        // Check death reaction flags
    //        BYTE b1 = memory::read<BYTE>(component + 0x15A);
    //        BYTE b2 = memory::read<BYTE>(component + 0x168);

    //        if (!(b1 == 0 || b2 == 1)) {
    //            return oHkDeath(shooter_character, a2);
    //        }

    //        // Get killer and weapon info
    //        auto killer = damage_response->get_damage_causer();
    //        auto equippable = damage_response->get_equippable_used();

    //        if (!killer || !equippable ||
    //            !memory::IsValidPointer((uintptr_t)killer) ||
    //            !memory::IsValidPointer((uintptr_t)equippable)) {
    //            return oHkDeath(shooter_character, a2);
    //        }

    //        // Get world context
    //        uworld* world_save = nullptr;
    //        uintptr_t* uworld_state_ptr = *(uintptr_t**)(memory::module_base + offsets::State);
    //        if (uworld_state_ptr) {
    //            world_save = *(uworld**)uworld_state_ptr;
    //        }

    //        if (!world_save || !memory::IsValidPointer((uintptr_t)world_save)) {
    //            return oHkDeath(shooter_character, a2);
    //        }

    //        // Find enemies
    //        tarray<ashootercharacter*> enemies = blueprints::find_all_shooters_with_alliance(
    //            world_save, character, earesalliance::enemy, false, true);

    //        currentequippable* my_weapon = character->get_inventory()->get_current_equippable();

    //        // Check if finisher should be processed
    //        bool process_finisher = globals::misc::finisher &&
    //            character->is_alive() &&
    //            character->health() > 0 &&
    //            character &&
    //            memory::IsValidPointer((uintptr_t)character);

    //        if (process_finisher) {
    //            if (killer == local_pawn_context) {
    //                int num_enemies = enemies.count;

    //                // Remove the killed enemy from count
    //                for (int i = 0; i < enemies.count; ++i) {
    //                    if (enemies[i] == shooter_character) {
    //                        num_enemies -= 1;
    //                        break;
    //                    }
    //                }

    //                // Check finisher conditions
    //                bool should_play_finisher = globals::misc::only_last_kill ? (num_enemies == 0) : true;

    //                if (globals::misc::finisher && should_play_finisher) {
    //                    std::string weapon_name = system::get_object_name(my_weapon).to_str();

    //                    auto apply_finisher = [&]() {
    //                       /* std::wstring skin = get_chosen_skin(weapon_name);
    //                        printf("[DEBUG] get_chosen_skin weapon_name: %s\n", weapon_name.c_str());
    //                        wprintf(L"[DEBUG] Chosen skin: %ls\n", skin.empty() ? L"(none)" : skin.c_str());
    //                        uobject* finisher = get_finisher_from_skin(skin.c_str());*/

    //                        /*    if (!finisher) {
    //                                return;
    //                            }*/

    //                            // Set up finisher override
    //                        static uobject* dummy_finisher = uobject::find_object<uobject*>(L"FXC_Finisher_Champions_Victim_C", (uobject*)-1);

    //                        // Clear existing overrides
    //               /*         memory::write<uobject*>(component + offsets::montage_effect_override_offset, dummy_finisher);
    //                        memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, nullptr);*/

    //                        memory::write<uobject*>(component + offsets::montage_effect_override_offset, nullptr);
    //                        memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, nullptr);

    //                        // Set new finisher
    //                        memory::write<uobject*>(component + offsets::montage_effect_override_offset, dummy_finisher);
    //                        memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, local_pawn_context);


    //                        PlayFinisherEffect(component); // btw it's not crashing just freezing so it's a func
    //                        };

    //                    // Check weapon type and apply finisher
    //                    if (weapon_name.find("AssaultRifle_AK_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("AssaultRifle_ACR_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("BoltSniper_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("AssaultRifle_Burst_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("AutomaticPistol_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("DMR_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("RevolverPistol_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("LugerPistol_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("SubMachineGun_MP5_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("BasePistol_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("LeverSniperRifle_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("DS_Gun_C") != std::string::npos) {
    //                        apply_finisher();
    //                    }
    //                    else if (weapon_name.find("Ability_Melee_Base_C") != std::string::npos) {
    //                        memory::write<uobject*>(component + offsets::montage_effect_override_offset, nullptr);
    //                        memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, nullptr);
    //                    }
    //                }
    //            }
    //        }
    //    }
    //    catch (...) {
    //        // Silent catch
    //    }

    //    return oHkDeath(shooter_character, a2);
    //}



    static inline uobject* GetExponentialHeightFogClass()
    {
        static uobject* cls = nullptr;
        if (!cls)
            cls = uobject::find_object<uobject*>(crypt(L"Engine.ExponentialHeightFog").decrypt());
        return cls;
    }

    void apply_crystal_chams_to_self(ashootercharacter* local_player, ugameinstance* gameinstance)
    {
        if (!local_player || !gameinstance) return;

        static int self_frame_counter = 0;
        static int last_preset = -1;

        self_frame_counter++;
        if (self_frame_counter % 6 != 0) return;

        if (last_preset != globals::visuals::crystal_chams_preset) {
            switch (globals::visuals::crystal_chams_preset) {
            case 0:
                globals::visuals::Self_CenterEdgeR = 0.53f;
                globals::visuals::Self_CenterEdgeG = 0.58f;
                globals::visuals::Self_CenterEdgeB = 0.52f;
                globals::visuals::Self_InnerEdgeR = 12.64f;
                globals::visuals::Self_InnerEdgeG = 15.00f;
                globals::visuals::Self_InnerEdgeB = 0.89f;
                globals::visuals::Self_OuterEdgeR = 2.47f;
                globals::visuals::Self_OuterEdgeG = 11.64f;
                globals::visuals::Self_OuterEdgeB = 0.00f;

                globals::visuals::GlowVisible = 1.5f;
                globals::visuals::AlphaBasePower = 2.0f;
                globals::visuals::AlphaColorMult = 1.2f;
                globals::visuals::DepthBias = 0.1f;
                globals::visuals::AlphaDissolveOpacity = 0.8f;
                globals::visuals::BoundingBox = 1.0f;
                globals::visuals::InnerEdgeThickness = 0.3f;
                globals::visuals::OuterEdgeThickness = 0.2f;
                globals::visuals::RimFresnel = 2.5f;
                globals::visuals::RimMultiply = 1.0f;
                globals::visuals::RimPower = 3.0f;
                globals::visuals::OcclusionDepth = 0.5f;
                globals::visuals::OcclusionBehindWall = 0.3f;
                globals::visuals::OcclusionState = 1.0f;
                globals::visuals::RefractionDepthBias = 0.05f;
                break;

            case 1:
                globals::visuals::Self_CenterEdgeR = 0.58f;
                globals::visuals::Self_CenterEdgeG = 0.63f;
                globals::visuals::Self_CenterEdgeB = 0.52f;
                globals::visuals::Self_InnerEdgeR = 0.00f;
                globals::visuals::Self_InnerEdgeG = 0.00f;
                globals::visuals::Self_InnerEdgeB = 0.00f;
                globals::visuals::Self_OuterEdgeR = 2.67f;
                globals::visuals::Self_OuterEdgeG = 0.00f;
                globals::visuals::Self_OuterEdgeB = 0.00f;

                globals::visuals::GlowVisible = 1.8f;
                globals::visuals::AlphaBasePower = 2.2f;
                globals::visuals::AlphaColorMult = 1.0f;
                globals::visuals::DepthBias = 0.15f;
                globals::visuals::AlphaDissolveOpacity = 0.9f;
                globals::visuals::BoundingBox = 1.0f;
                globals::visuals::InnerEdgeThickness = 0.4f;
                globals::visuals::OuterEdgeThickness = 0.25f;
                globals::visuals::RimFresnel = 3.0f;
                globals::visuals::RimMultiply = 1.2f;
                globals::visuals::RimPower = 2.5f;
                globals::visuals::OcclusionDepth = 0.6f;
                globals::visuals::OcclusionBehindWall = 0.4f;
                globals::visuals::OcclusionState = 1.0f;
                globals::visuals::RefractionDepthBias = 0.08f;
                break;

            case 2:
                globals::visuals::Self_CenterEdgeR = 0.63f;
                globals::visuals::Self_CenterEdgeG = 0.63f;
                globals::visuals::Self_CenterEdgeB = 0.58f;
                globals::visuals::Self_InnerEdgeR = 0.00f;
                globals::visuals::Self_InnerEdgeG = 12.74f;
                globals::visuals::Self_InnerEdgeB = 13.64f;
                globals::visuals::Self_OuterEdgeR = 3.36f;
                globals::visuals::Self_OuterEdgeG = 0.00f;
                globals::visuals::Self_OuterEdgeB = 0.00f;

                globals::visuals::GlowVisible = 2.0f;
                globals::visuals::AlphaBasePower = 1.8f;
                globals::visuals::AlphaColorMult = 1.5f;
                globals::visuals::DepthBias = 0.12f;
                globals::visuals::AlphaDissolveOpacity = 0.7f;
                globals::visuals::BoundingBox = 1.0f;
                globals::visuals::InnerEdgeThickness = 0.35f;
                globals::visuals::OuterEdgeThickness = 0.18f;
                globals::visuals::RimFresnel = 2.8f;
                globals::visuals::RimMultiply = 1.1f;
                globals::visuals::RimPower = 3.2f;
                globals::visuals::OcclusionDepth = 0.4f;
                globals::visuals::OcclusionBehindWall = 0.2f;
                globals::visuals::OcclusionState = 1.0f;
                globals::visuals::RefractionDepthBias = 0.06f;
                break;

            case 3:
                globals::visuals::Self_CenterEdgeR = 0.68f;
                globals::visuals::Self_CenterEdgeG = 0.63f;
                globals::visuals::Self_CenterEdgeB = 0.58f;
                globals::visuals::Self_InnerEdgeR = 0.00f;
                globals::visuals::Self_InnerEdgeG = 12.74f;
                globals::visuals::Self_InnerEdgeB = 13.64f;
                globals::visuals::Self_OuterEdgeR = 3.36f;
                globals::visuals::Self_OuterEdgeG = 0.00f;
                globals::visuals::Self_OuterEdgeB = 0.00f;

                globals::visuals::GlowVisible = 1.6f;
                globals::visuals::AlphaBasePower = 2.5f;
                globals::visuals::AlphaColorMult = 0.9f;
                globals::visuals::DepthBias = 0.08f;
                globals::visuals::AlphaDissolveOpacity = 0.85f;
                globals::visuals::BoundingBox = 1.0f;
                globals::visuals::InnerEdgeThickness = 0.25f;
                globals::visuals::OuterEdgeThickness = 0.15f;
                globals::visuals::RimFresnel = 2.2f;
                globals::visuals::RimMultiply = 0.8f;
                globals::visuals::RimPower = 2.8f;
                globals::visuals::OcclusionDepth = 0.7f;
                globals::visuals::OcclusionBehindWall = 0.5f;
                globals::visuals::OcclusionState = 1.0f;
                globals::visuals::RefractionDepthBias = 0.04f;
                break;

            case 4:
                globals::visuals::Self_CenterEdgeR = 0.00f;
                globals::visuals::Self_CenterEdgeG = 0.63f;
                globals::visuals::Self_CenterEdgeB = 0.47f;
                globals::visuals::Self_InnerEdgeR = 0.00f;
                globals::visuals::Self_InnerEdgeG = 12.74f;
                globals::visuals::Self_InnerEdgeB = 13.64f;
                globals::visuals::Self_OuterEdgeR = 3.25f;
                globals::visuals::Self_OuterEdgeG = 0.00f;
                globals::visuals::Self_OuterEdgeB = 0.00f;

                globals::visuals::GlowVisible = 2.2f;
                globals::visuals::AlphaBasePower = 1.9f;
                globals::visuals::AlphaColorMult = 1.3f;
                globals::visuals::DepthBias = 0.18f;
                globals::visuals::AlphaDissolveOpacity = 0.75f;
                globals::visuals::BoundingBox = 1.0f;
                globals::visuals::InnerEdgeThickness = 0.45f;
                globals::visuals::OuterEdgeThickness = 0.28f;
                globals::visuals::RimFresnel = 3.5f;
                globals::visuals::RimMultiply = 1.4f;
                globals::visuals::RimPower = 2.6f;
                globals::visuals::OcclusionDepth = 0.35f;
                globals::visuals::OcclusionBehindWall = 0.25f;
                globals::visuals::OcclusionState = 1.0f;
                globals::visuals::RefractionDepthBias = 0.09f;
                break;
            }
            last_preset = globals::visuals::crystal_chams_preset;
        }

        if (!globals::visuals::crystal_chams_enabled) {
            auto self_main_mesh = local_player->get_mesh();
            if (self_main_mesh) {
                local_player->reset_character_materials_internal(self_main_mesh);
            }
            uskeletalmeshcomponent* self_cosmetic_mesh = local_player->GetCosmeticMesh3P();
            if (self_cosmetic_mesh) {
                local_player->reset_character_materials_internal(self_cosmetic_mesh);
            }

            auto mesh1p = memory::read<uskeletalmeshcomponent*>((uintptr_t)local_player + 0x0F10);
            auto meshOverlay = memory::read<uskeletalmeshcomponent*>((uintptr_t)local_player + 0x0F18);
            if (mesh1p) local_player->reset_character_materials_internal(mesh1p);
            if (meshOverlay) local_player->reset_character_materials_internal(meshOverlay);
            return;
        }

        uobject* crystal_material = uobject::static_load_object(nullptr, nullptr,
            L"/Game/Characters/BountyHunter/S0/VFX/Materials/BountyHunterReveal_MI.BountyHunterReveal_MI");

        if (!crystal_material) return;


        fname silohuette_color_name = string::string_to_name(L"SilohuetteColor");
        fname center_edge_color_name = string::string_to_name(L"CenterEdgeColor");
        fname inner_edge_color_name = string::string_to_name(L"InnerEdgeColor");
        fname outer_edge_color_name = string::string_to_name(L"OuterEdgeColor");
        fname glow_intensity_param = string::string_to_name(L"GlowIntensity");


        fname alpha_base_power_name = string::string_to_name(L"Alpha_Base_Power");
        fname depth_bias_name = string::string_to_name(L"DepthBias");
        fname alpha_dissolve_opacity_name = string::string_to_name(L"Alpha_Dissolve_Opacity");
        fname bounding_box_name = string::string_to_name(L"BoundingBox");
        fname inner_edge_thickness_name = string::string_to_name(L"InnerEdgeThickness");
        fname outer_edge_thickness_name = string::string_to_name(L"OuterEdgeThickness");
        fname rim_fresnel_name = string::string_to_name(L"Rim_Fresnel");
        fname rim_multiply_name = string::string_to_name(L"Rim_Multiply");
        fname rim_power_name = string::string_to_name(L"Rim_Power");
        fname occlusion_behind_wall_name = string::string_to_name(L"OcclusionDepth_BehindWall");
        fname occlusion_state_name = string::string_to_name(L"OcclusionState");
        fname refraction_depth_bias_name = string::string_to_name(L"RefractionDepthBias");


        float self_glowIntensity = globals::visuals::GlowVisible;
        float alpha_base_power = globals::visuals::AlphaBasePower;
        float alpha_colormult = globals::visuals::AlphaColorMult;
        float depth_bias = globals::visuals::DepthBias;
        float alpha_dissolve_opacity = globals::visuals::AlphaDissolveOpacity;
        float bounding_box = globals::visuals::BoundingBox;
        float inner_edge_thickness = globals::visuals::InnerEdgeThickness;
        float outer_edge_thickness = globals::visuals::OuterEdgeThickness;
        float rim_fresnel = globals::visuals::RimFresnel;
        float rim_multiply = globals::visuals::RimMultiply;
        float rim_power = globals::visuals::RimPower;
        float occlusion_behind_wall = globals::visuals::OcclusionBehindWall;
        float occlusion_state = globals::visuals::OcclusionState;
        float refraction_depth_bias = globals::visuals::RefractionDepthBias;

        flinearcolor self_centerEdgeColor = flinearcolor(
            globals::visuals::Self_CenterEdgeR,
            globals::visuals::Self_CenterEdgeG,
            globals::visuals::Self_CenterEdgeB,
            globals::visuals::intensityvisibleoutline
        );
        flinearcolor self_innerEdgeColor = flinearcolor(
            globals::visuals::Self_InnerEdgeR,
            globals::visuals::Self_InnerEdgeG,
            globals::visuals::Self_InnerEdgeB,
            globals::visuals::intensityvisibleoutline
        );
        flinearcolor self_outerEdgeColor = flinearcolor(
            globals::visuals::Self_OuterEdgeR,
            globals::visuals::Self_OuterEdgeG,
            globals::visuals::Self_OuterEdgeB,
            globals::visuals::intensityvisibleoutline
        );


        auto apply_material = [&](uskeletalmeshcomponent* mesh) {
            if (!mesh) return;

            int num_materials = mesh->get_num_materials();
            for (int i = 0; i < num_materials; i++) {
                auto material_instance_dynamic = mesh->create_and_set_material_instance_dynamic_from_material(i, crystal_material);
                auto dynCast = material_instance_dynamic->cast<UMaterialInstanceDynamic>();
                if (!dynCast) continue;

                dynCast->set_vector_parameter_value1(silohuette_color_name, self_outerEdgeColor);
                dynCast->set_vector_parameter_value1(center_edge_color_name, self_centerEdgeColor);
                dynCast->set_vector_parameter_value1(inner_edge_color_name, self_innerEdgeColor);
                dynCast->set_vector_parameter_value1(outer_edge_color_name, self_outerEdgeColor);
                dynCast->set_scalar_parameter_value(glow_intensity_param, self_glowIntensity);
                dynCast->set_scalar_parameter_value(alpha_base_power_name, alpha_base_power);
                dynCast->set_scalar_parameter_value(depth_bias_name, depth_bias);
                dynCast->set_scalar_parameter_value(alpha_dissolve_opacity_name, alpha_dissolve_opacity);
                dynCast->set_scalar_parameter_value(bounding_box_name, bounding_box);
                dynCast->set_scalar_parameter_value(inner_edge_thickness_name, inner_edge_thickness);
                dynCast->set_scalar_parameter_value(outer_edge_thickness_name, outer_edge_thickness);
                dynCast->set_scalar_parameter_value(rim_fresnel_name, rim_fresnel);
                dynCast->set_scalar_parameter_value(rim_multiply_name, rim_multiply);
                dynCast->set_scalar_parameter_value(rim_power_name, rim_power);
                dynCast->set_scalar_parameter_value(occlusion_behind_wall_name, occlusion_behind_wall);
                dynCast->set_scalar_parameter_value(occlusion_state_name, occlusion_state);
                dynCast->set_scalar_parameter_value(refraction_depth_bias_name, refraction_depth_bias);
            }
            };

        apply_material(local_player->get_mesh());
        apply_material(local_player->GetCosmeticMesh3P());

        auto mesh1p = memory::read<uskeletalmeshcomponent*>((uintptr_t)local_player + offsets::mesh1p);
        auto meshOverlay = memory::read<uskeletalmeshcomponent*>((uintptr_t)local_player + offsets::mesh1p_overlay);

        apply_material(mesh1p);
        apply_material(meshOverlay);
    }


    static std::unordered_map<uobject*, std::string> objectNameCache;
    inline std::string get_cached_name(uobject* obj) {
        auto it = objectNameCache.find(obj);
        if (it != objectNameCache.end()) return it->second;
        std::string name = system::get_object_name(obj).ToString();
        objectNameCache[obj] = name;
        return name;
    }
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
    struct SkinItem {
        std::wstring name;
    };
    static std::unordered_map<std::string, std::vector<SkinItem>> g_byFamily;
    static std::unordered_map<std::string, int> g_selectedIndexForFamily;
    std::wstring get_chosen_skin(const std::string& weapon_name) {


        std::string family;
        for (const auto& [key, fam] : kWeaponToFamily) {
            if (weapon_name == key) {
                family = fam;
                break;
            }
        }

        if (family.empty()) {

            return L"";
        }



        auto it = g_byFamily.find(family);
        if (it == g_byFamily.end()) {
            return L"";
        }

        int index = g_selectedIndexForFamily[family];
        auto& skins = it->second;

        if (index < 0 || index >= static_cast<int>(skins.size())) {

            return L"";
        }

        return skins[index].name;
    }
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
    static void store_skin_by_name(const std::wstring& fullName) {
        std::string fam = family_from_logged_name(fullName);
        auto& vec = g_byFamily[fam];

        if (std::none_of(vec.begin(), vec.end(), [&](const SkinItem& it) { return it.name == fullName; })) {
            vec.push_back(SkinItem{ fullName });

            if (vec.size() == 1) {
                g_selectedIndexForFamily[fam] = 0;
            }
        }
    }
    static std::unordered_map<equippable_skin_data_asset*, std::wstring> g_skinNameCache;
    static const std::wstring& get_skin_name_cached(equippable_skin_data_asset* p, bool refresh = false) {
        static const std::wstring kEmpty;
        if (!p) return kEmpty;

        if (!refresh) {
            auto it = g_skinNameCache.find(p);
            if (it != g_skinNameCache.end()) return it->second;
        }

        // Query once, then copy into a stable std::wstring
        fstring f = system::get_object_name(p);
        auto [it, _] = g_skinNameCache.emplace(p, std::wstring(f.c_str()));
        if (!_) it->second.assign(f.c_str()); // if already existed & refresh==true
        return it->second;
    }
    std::string normalize_weapon_class(const std::string& weapon) {
        size_t pos = weapon.find_last_of('_');
        if (pos != std::string::npos && pos + 1 < weapon.size() &&
            std::all_of(weapon.begin() + pos + 1, weapon.end(), ::isdigit)) {
            return weapon.substr(0, pos);
        }
        return weapon;
    }
    uobject* get_finisher_from_skin(std::wstring skinData) {
        std::wstring assetName = skinData;
        const std::wstring defaultPrefix = L"Default__";
        if (assetName.find(defaultPrefix) == 0) {
            assetName = assetName.substr(defaultPrefix.length());
        }

        size_t firstUnderscore = assetName.find(L'_');
        size_t lastUnderscore = assetName.rfind(L"_PrimaryAsset_C");

        if (firstUnderscore == std::wstring::npos || lastUnderscore == std::wstring::npos || lastUnderscore <= firstUnderscore) {
            return nullptr;
        }

        std::wstring skinNameW = assetName.substr(firstUnderscore + 1, lastUnderscore - firstUnderscore - 1);
        std::string skinName(skinNameW.begin(), skinNameW.end());

        std::string obj = "FXC_Finisher_" + skinName + "_Victim_C";
        std::wstring wobj(obj.begin(), obj.end());
        uobject* effect = uobject::find_object<uobject*>(wobj.c_str(), reinterpret_cast<uobject*>(-1));

        if (!effect && !skinName.empty()) {
            while (!skinName.empty() && std::isdigit(skinName.back())) {
                skinName.pop_back();
            }

            obj = "FXC_Finisher_" + skinName + "_Victim_C";
            wobj = std::wstring(obj.begin(), obj.end());
            effect = uobject::find_object<uobject*>(wobj.c_str(), reinterpret_cast<uobject*>(-1));
        }

        return effect;
    }




    using FinisherFn = void* (__fastcall*)(uintptr_t);

    inline void* PlayFinisherEffect(uintptr_t effect)
    {
        SPOOF_FUNC;
        static void* (__fastcall * fn)(uintptr_t) = nullptr;
        if (!fn)
            fn = reinterpret_cast<FinisherFn>(memory::module_base + offsets::play_finisher_effect);
        return fn(effect);
    }

    void hk_death(ashootercharacter* shooter_character, UDamageResponse* a2) {
        try {
            printf("[DEATH] hk_death called! shooter=0x%p a2=0x%p\n", (void*)shooter_character, (void*)a2);

            if (!shooter_character || !memory::IsValidPointer((uintptr_t)shooter_character)) {
                printf("[DEATH] shooter_character invalid, passthrough\n");
                return oHkDeath(shooter_character, a2);
            }

            acknowledgedpawn* pawn = controllers->get_acknowledged_pawn();
            ashootercharacter* character_context = (ashootercharacter*)character;
            acknowledgedpawn* local_pawn_context = (acknowledgedpawn*)pawn;
            auto damage_response = a2;

            printf("[DEATH] character_context=0x%p local_pawn=0x%p damage_response=0x%p\n",
                (void*)character_context, (void*)local_pawn_context, (void*)damage_response);

            if (!character_context || !local_pawn_context || !damage_response) {
                printf("[DEATH] FAIL: one of context/pawn/dmg is null\n");
                return oHkDeath(shooter_character, a2);
            }

            if (!memory::IsValidPointer((uintptr_t)character_context) ||
                !memory::IsValidPointer((uintptr_t)local_pawn_context) ||
                !memory::IsValidPointer((uintptr_t)damage_response)) {
                printf("[DEATH] FAIL: pointer validation failed\n");
                return oHkDeath(shooter_character, a2);
            }

            auto component = (uintptr_t)memory::read<uobject*>((uintptr_t)shooter_character + offsets::death_reaction_component_offset);
            printf("[DEATH] component=0x%p (offset=0x%X)\n", (void*)component, (int)offsets::death_reaction_component_offset);

            if (!component || !memory::IsValidPointer(component)) {
                printf("[DEATH] FAIL: component null/invalid\n");
                return oHkDeath(shooter_character, a2);
            }

            BYTE b1 = memory::read<BYTE>(component + 0x15A);
            BYTE b2 = memory::read<BYTE>(component + 0x168);
            printf("[DEATH] b1=0x%X b2=0x%X | condition=(b1==0 || b2==1) = %d\n", b1, b2, (int)(b1 == 0 || b2 == 1));

            if (!(b1 == 0 || b2 == 1)) {
                printf("[DEATH] FAIL: b1/b2 condition not met\n");
                return oHkDeath(shooter_character, a2);
            }

            auto killer = damage_response->get_damage_causer();
            auto equippable = damage_response->get_equippable_used();
            printf("[DEATH] killer=0x%p local_pawn=0x%p equippable=0x%p\n",
                (void*)killer, (void*)local_pawn_context, (void*)equippable);
            printf("[DEATH] killer==local_pawn? %d\n", (int)(killer == local_pawn_context));

            if (!killer || !equippable ||
                !memory::IsValidPointer((uintptr_t)killer) ||
                !memory::IsValidPointer((uintptr_t)equippable)) {
                printf("[DEATH] FAIL: killer or equippable null/invalid\n");
                return oHkDeath(shooter_character, a2);
            }

            if (!UWorldSave || !memory::IsValidPointer((uintptr_t)UWorldSave)) {
                printf("[DEATH] FAIL: UWorldSave null/invalid\n");
                return oHkDeath(shooter_character, a2);
            }

            tarray<ashootercharacter*> enemies = blueprints::find_all_shooters_with_alliance(
                UWorldSave, character, earesalliance::enemy, false, true);
            printf("[DEATH] enemies.count=%d\n", enemies.count);

            myweapon = character->get_inventory()->get_current_equippable();
            printf("[DEATH] myweapon=0x%p\n", (void*)myweapon);

            if (globals::misc::finisher &&
                character_context->is_alive() &&
                character_context->health() > 0 &&
                memory::IsValidPointer((uintptr_t)character_context))
            {
                printf("[DEATH] finisher enabled, checking killer match\n");

                if (killer == local_pawn_context)
                {
                    printf("[DEATH] killer matches local pawn -> applying finisher\n");

                    int num_enemies = enemies.count;
                    for (int i = 0; i < enemies.size(); ++i) {
                        if (enemies[i] == shooter_character) {
                            num_enemies -= 1;
                            break;
                        }
                    }
                    printf("[DEATH] num_enemies after exclude=%d only_last_kill=%d\n",
                        num_enemies, (int)globals::misc::only_last_kill);

                    bool should_play = globals::misc::only_last_kill ? (num_enemies == 0) : true;
                    printf("[DEATH] should_play_finisher=%d\n", (int)should_play);

                    if (should_play)
                    {
                        std::string raw_weapon = get_cached_name(myweapon);
                        std::string weapon_name = normalize_weapon_class(raw_weapon.c_str());
                        printf("[DEATH] raw_weapon=%s normalized=%s\n", raw_weapon.c_str(), weapon_name.c_str());

                        auto apply_finisher = [&]() {
                            std::wstring skin = get_chosen_skin(weapon_name);
                            printf("[DEATH] chosen skin=%ws\n", skin.empty() ? L"(empty)" : skin.c_str());

                            uobject* finisher = get_finisher_from_skin(skin.c_str());
                            printf("[DEATH] finisher object=0x%p\n", (void*)finisher);

                            static uobject* dummy_finisher = uobject::find_object<uobject*>(
                                L"FXC_Finisher_Invalid_Victim_C", reinterpret_cast<uobject*>(-1));
                            printf("[DEATH] dummy_finisher=0x%p\n", (void*)dummy_finisher);

                            printf("[DEATH] writing to component=0x%p offset_override=0x%X offset_ctx=0x%X\n",
                                (void*)component,
                                (int)offsets::montage_effect_override_offset,
                                (int)offsets::montage_effect_override_context_offset);

                            memory::write<uobject*>(component + offsets::montage_effect_override_offset, dummy_finisher);
                            memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, nullptr);
                            memory::write<uobject*>(component + offsets::montage_effect_override_offset, nullptr);
                            memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, nullptr);
                            memory::write<uobject*>(component + offsets::montage_effect_override_offset, finisher);
                            memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, local_pawn_context);

                            printf("[DEATH] calling PlayFinisherEffect(0x%p)\n", (void*)component);
                            PlayFinisherEffect(component);
                            printf("[DEATH] PlayFinisherEffect done\n");
                            };

                        bool weapon_matched = false;
                        if (weapon_name.find("AssaultRifle_AK_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("AssaultRifle_ACR_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("BoltSniper_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("AssaultRifle_Burst_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("AutomaticPistol_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("DMR_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("RevolverPistol_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("LugerPistol_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("SubMachineGun_MP5_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("BasePistol_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("LeverSniperRifle_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("DS_Gun_C") != std::string::npos) { weapon_matched = true; apply_finisher(); }
                        else if (weapon_name.find("Ability_Melee_Base_C") != std::string::npos) {
                            weapon_matched = true;
                            memory::write<uobject*>(component + offsets::montage_effect_override_offset, nullptr);
                            memory::write<uobject*>(component + offsets::montage_effect_override_context_offset, nullptr);
                        }

                        if (!weapon_matched)
                            printf("[DEATH] FAIL: weapon_name '%s' matched NO weapon case!\n", weapon_name.c_str());
                    }
                }
                else {
                    printf("[DEATH] killer != local_pawn, not our kill\n");
                }
            }
            else {
                printf("[DEATH] finisher check failed: enabled=%d alive=%d health=%d\n",
                    (int)globals::misc::finisher,
                    (int)character_context->is_alive(),
                    (int)character_context->health());
            }
        }
        catch (...) {
            printf("[DEATH] EXCEPTION caught\n");
        }

        return oHkDeath(shooter_character, a2);
    }



    namespace kismentsystemlibrary {
        static bool line_trace_single(
            uworld* world_context_object,
            fvector start,
            fvector end,
            ETraceTypeQuery trace_channel,
            bool b_trace_complex,
            tarray<AActor*> actors_to_ignore,
            EDrawDebugTrace draw_debug_type,
            FHitResult& out_hit,
            bool b_ignore_self,
            flinearcolor trace_color,
            flinearcolor trace_hit_color,
            float draw_time)
        {
            static uobject* function = nullptr;
            if (!function)
                function = uobject::find_object<uobject*>(L"Engine.KismetSystemLibrary.LineTraceSingle");
            if (!function) return false;

            struct {
                uworld* world_context_object;
                fvector start;
                fvector end;
                ETraceTypeQuery trace_channel;
                bool b_trace_complex;
                tarray<AActor*> actors_to_ignore;
                char draw_debug_type;
                FHitResult out_hit;
                bool b_ignore_self;
                flinearcolor trace_color;
                flinearcolor trace_hit_color;
                float draw_time;
                bool return_value;
            } params;

            params.world_context_object = world_context_object;
            params.start = start;
            params.end = end;
            params.trace_channel = trace_channel;
            params.b_trace_complex = b_trace_complex;
            params.actors_to_ignore = actors_to_ignore;
            params.draw_debug_type = draw_debug_type;
            params.b_ignore_self = b_ignore_self;
            params.trace_color = trace_color;
            params.trace_hit_color = trace_hit_color;
            params.draw_time = draw_time;

            world_context_object->process_event(function, &params);
            out_hit = params.out_hit;

            return params.return_value;
        }
    }

    inline uworld* GetWorldSafe() {
        uintptr_t* uworld_state_ptr = *(uintptr_t**)(memory::module_base + offsets::State);
        if (uworld_state_ptr) {
            return *(uworld**)uworld_state_ptr;
        }
        return nullptr;
    }

    namespace ShooterGameBlueprints {
        static uintptr_t GetAresGlobals() {
            static uobject* Function = nullptr;
            if (!Function) {
                Function = uobject::find_object<uobject*>(L"ShooterGame.ShooterBlueprintLibrary.GetAresGlobals");
            }
            if (!Function) return 0;

            static uobject* DefaultObj = nullptr;
            if (!DefaultObj) {
                DefaultObj = uobject::find_object<uobject*>(L"ShooterGame.Default__ShooterBlueprintLibrary");
            }
            if (!DefaultObj) return 0;

            struct {
                uintptr_t ReturnValue;
            } params = { 0 };

            DefaultObj->process_event(Function, &params);
            return params.ReturnValue;
        }

        static void GetWallPenetrationSpans(
            uworld* World,
            const fvector& Start,
            const fvector& End,
            const tarray<AActor*>& IgnoreActors,
            ECollisionChannel TraceChannel,
            float IgnoreGapTolerance,
            FWallSpanList& OutWallSpans)
        {
            static uobject* Function = nullptr;
            if (!Function) {
                Function = uobject::find_object<uobject*>(L"ShooterGame.ShooterBlueprintLibrary.GetWallPenetrationSpans");
            }

            if (!Function || !World) {
                return;
            }

            struct {
                uworld* World;
                fvector Start;
                fvector End;
                tarray<AActor*> IgnoreActors;
                ECollisionChannel TraceChannel;
                float IgnoreGapTolerance;
                FWallSpanList OutWallSpans;
            } params;

            params.World = World;
            params.Start = Start;
            params.End = End;
            params.IgnoreActors = IgnoreActors;
            params.TraceChannel = TraceChannel;
            params.IgnoreGapTolerance = IgnoreGapTolerance;

            World->process_event(Function, &params);
            OutWallSpans = params.OutWallSpans;
        }

        static uint8_t ConvertToAresSurfaceType(uint8_t SurfaceType) {
            static uobject* Function = nullptr;
            if (!Function) {
                Function = uobject::find_object<uobject*>(L"ShooterGame.ShooterBlueprintLibrary.ConvertToAresSurfaceType");
            }
            if (!Function) return 0;

            static uobject* DefaultObj = nullptr;
            if (!DefaultObj) {
                DefaultObj = uobject::find_object<uobject*>(L"ShooterGame.Default__ShooterBlueprintLibrary");
            }
            if (!DefaultObj) return 0;

            struct {
                uint8_t SurfaceType;
                uint8_t ReturnValue;
            } params;

            params.SurfaceType = SurfaceType;
            DefaultObj->process_event(Function, &params);
            return params.ReturnValue;
        }
    }


#include <string>
#include <map>







    class BoneHelper {
    public:
        static int32_t GetBonePriorityByIndex(int32_t bi, int32_t bc)
        {
            switch (bc) {
            case 101:
                if (bi == 20) return 100; if (bi == 21) return 90;
                if (bi >= 17 && bi <= 19) return 80; if (bi >= 15 && bi <= 16) return 70;
                if (bi >= 13 && bi <= 14) return 60; if (bi == 3)  return 50;
                if (bi >= 23 && bi <= 25) return 30; if (bi >= 49 && bi <= 51) return 30;
                if (bi >= 75 && bi <= 78) return 25; if (bi >= 82 && bi <= 85) return 25;
                return 10;
            case 103:
                if (bi == 8)  return 100; if (bi == 9)  return 90;
                if (bi >= 5 && bi <= 7)   return 80; if (bi == 3)  return 50;
                if (bi >= 30 && bi <= 33) return 30; if (bi >= 55 && bi <= 58) return 30;
                if (bi >= 63 && bi <= 69) return 25; if (bi >= 77 && bi <= 83) return 25;
                return 10;
            case 104:
                if (bi == 20) return 100; if (bi == 21) return 90;
                if (bi >= 17 && bi <= 19) return 80; if (bi >= 15 && bi <= 16) return 70;
                if (bi >= 13 && bi <= 14) return 60; if (bi == 3)  return 50;
                if (bi >= 23 && bi <= 25) return 30; if (bi >= 49 && bi <= 51) return 30;
                if (bi >= 77 && bi <= 80) return 25; if (bi >= 84 && bi <= 87) return 25;
                return 10;
            default:
                if (bi <= 10) return 80; if (bi <= 20) return 60; if (bi <= 30) return 40; return 20;
            }
        }

        static inline void GetCriticalBones(int32_t bc, int32_t* out, int32_t& cnt)
        {
            if (bc == 101 || bc == 104) { static const int32_t b[] = { 20,21,19,18,17,3 }; memcpy(out, b, sizeof(b)); cnt = 6; }
            else if (bc == 103) { static const int32_t b[] = { 8,9,7,6,5,3 };     memcpy(out, b, sizeof(b)); cnt = 6; }
            else cnt = 0;
        }
    };




    // ======================== AUTOWALL SYSTEM v2 ========================
    // Uses proper FWallSpanList struct parsing + exit location validation
    // No bBlockingHit check (was causing false negatives on boxes)
    // No PhysMaterial/GObject dependency

    struct WeaponPenData {
        float HeadDamage;
        float PenetrationPower;       // 0.0 - 1.2 scale
        float DamageReductionPerWall;  // fraction lost per wall
        bool  CanPenetrateWalls;
    };

    static std::map<std::wstring, WeaponPenData> WeaponPenMap = {
        { L"Vandal",     { 160.0f, 1.00f, 0.30f, true  } },
        { L"Phantom",    { 140.0f, 0.95f, 0.30f, true  } },
        { L"Operator",   { 255.0f, 1.20f, 0.15f, true  } },
        { L"Marshal",    { 202.0f, 1.10f, 0.20f, true  } },
        { L"Outlaw",     { 238.0f, 1.15f, 0.18f, true  } },
        { L"Guardian",   { 195.0f, 1.05f, 0.25f, true  } },
        { L"Bulldog",    { 115.0f, 0.85f, 0.35f, true  } },
        { L"Odin",       {  95.0f, 0.80f, 0.40f, true  } },
        { L"Ares",       {  72.0f, 0.75f, 0.40f, true  } },
        { L"Ghost",      { 105.0f, 0.65f, 0.45f, true  } },
        { L"Spectre",    {  78.0f, 0.60f, 0.45f, true  } },
        { L"Stinger",    {  67.0f, 0.55f, 0.50f, true  } },
        { L"Frenzy",     {  78.0f, 0.50f, 0.55f, false } },
        { L"Classic",    {  78.0f, 0.45f, 0.55f, false } },
        { L"Shorty",     {  22.0f, 0.20f, 0.80f, false } },
        { L"Bucky",      {  44.0f, 0.25f, 0.80f, false } },
        { L"Judge",      {  34.0f, 0.30f, 0.70f, false } }
    };

    static WeaponPenData GetCurrentWeaponPenData(ashootercharacter* shooter) {
        WeaponPenData default_data = { 150.0f, 1.00f, 0.30f, true };
        if (!shooter) return default_data;

        auto inv = shooter->get_inventory();
        if (!inv) return default_data;

        auto eq = inv->get_current_equippable();
        if (!eq) return default_data;

        fstring weapon_name = system::get_object_name(eq);
        if (!weapon_name.c_str()) return default_data;

        std::wstring wname = weapon_name.c_str();
        for (const auto& pair : WeaponPenMap) {
            if (wname.find(pair.first) != std::wstring::npos) {
                return pair.second;
            }
        }
        return default_data;
    }

    static float AresMaterialMultipliers[40];
    static bool initialized_materials = false;

    static void CacheNativeMaterialMultipliers() {
        if (initialized_materials) return;

        for (int j = 0; j < 40; j++) AresMaterialMultipliers[j] = 1.0f;

        uintptr_t aresGlobals = ShooterGameBlueprints::GetAresGlobals();
        if (!aresGlobals || !memory::IsValidPointer(aresGlobals)) return;

        uintptr_t wallPenGlobals = *(uintptr_t*)(aresGlobals + 0x820); // CachedWallPenetrationGlobals
        if (!wallPenGlobals || !memory::IsValidPointer(wallPenGlobals)) return;

        printf("[AutoWall] Caching native material multipliers...\n");
        for (int i = 0; i < 39; i++) {
            uintptr_t cachedPenTypeCDO = *(uintptr_t*)(wallPenGlobals + 0x788 + (i * 8));
            if (!cachedPenTypeCDO || !memory::IsValidPointer(cachedPenTypeCDO)) continue;

            float energyReductionMulti = *(float*)(cachedPenTypeCDO + 0x30);

            // Read DebugWallPenetrationTypeName (offset 0x38, FString)
            fstring* name_str = (fstring*)(cachedPenTypeCDO + 0x38);
            if (name_str && name_str->is_valid() && memory::IsValidPointer((uintptr_t)name_str->data)) {
                printf("[AutoWall] Material %d (%ls): Energy Reduction Multiplier = %.3f\n",
                    i, name_str->c_str(), energyReductionMulti);
            }
            else {
                printf("[AutoWall] Material %d (Unknown): Energy Reduction Multiplier = %.3f\n",
                    i, energyReductionMulti);
            }

            if (energyReductionMulti > 0.001f) {
                // If game calculates EnergyLost = Thickness * EnergyReductionMultiplier,
                // then MaxThickness = BaseEnergy / EnergyReductionMultiplier.
                // Our old logic did: MaxThickness = BaseEnergy * MaterialMultiplier.
                // We set our MaterialMultiplier = 1.0f / EnergyReductionMultiplier.
                AresMaterialMultipliers[i] = 1.0f / energyReductionMulti;
            }
            else {
                AresMaterialMultipliers[i] = 100.0f; // Extremely penetrable
            }
        }
        initialized_materials = true;
    }

    // ======================== MAIN AUTOWALL CLASS v2 ========================

    class TraceHelper {
    public:
        static inline bool IsValidActorPtr(uintptr_t ptr) {
            return ptr != 0 && ptr != 0xFFFFFFFF && ptr != 0xFFFFFFFFFFFFFFFF && ptr > 0x10000;
        }

        static bool CanShootThrough(aplayercontroller* controller, ashootercharacter* shooter_char,
            ashootercharacter* target_enemy, int aim_bone = 8)
        {
            if (!controller || !shooter_char || !target_enemy) return false;
            if (!memory::IsValidPointer((uintptr_t)controller) ||
                !memory::IsValidPointer((uintptr_t)shooter_char) ||
                !memory::IsValidPointer((uintptr_t)target_enemy)) return false;
            if (!UWorldSave || !memory::IsValidPointer((uintptr_t)UWorldSave)) return false;

            uskeletalmeshcomponent* enemy_mesh = target_enemy->get_mesh();
            if (!enemy_mesh || !memory::IsValidPointer((uintptr_t)enemy_mesh)) return false;

            fvector camera_loc = controller->get_camera_manager()->get_camera_location();

            // If the aimbot is targeting feet (0), the trace will clip into the floor 
            // and generate many false walls. Force the trace to test the Head (8) instead.
            int trace_bone = (aim_bone == 0) ? 8 : aim_bone;
            fvector target_bone = GetBoneMatrix(enemy_mesh, trace_bone);
            tarray<AActor*> ignore_actors;
            ignore_actors.Add((AActor*)shooter_char);

            // Get weapon penetration data
            auto wpn = GetCurrentWeaponPenData(shooter_char);
            if (!wpn.CanPenetrateWalls) return false;

            // Max thickness per wall based on weapon pen power
            // Rifles ~180u, Operator ~216u, SMGs ~100u
            float max_per_wall = wpn.PenetrationPower * 180.0f;
            // Total thickness budget across all walls
            float max_total_thickness = wpn.PenetrationPower * 300.0f;

            // Call engine's GetWallPenetrationSpans with proper typed struct
            FWallSpanList wall_spans;
            memset(&wall_spans, 0, sizeof(FWallSpanList));

            ShooterGameBlueprints::GetWallPenetrationSpans(
                UWorldSave, camera_loc, target_bone,
                ignore_actors, ECollisionChannel::ECC_Visibility,
                0.0f, wall_spans
            );

            // If the target point is embedded inside a wall, can't penetrate
            if (wall_spans.bLastPointInWall) {
                // printf("[AutoWall] Target inside wall.\n");
                return false;
            }

            // No walls between us = clear line of fire (shouldn't happen since we're in autowall path)
            if (wall_spans.Spans.count <= 0 || !wall_spans.Spans.data) return true;

            // Validate span data pointer
            if (!memory::IsValidPointer((uintptr_t)wall_spans.Spans.data)) return false;

            float remaining_damage = wpn.HeadDamage;
            float total_thickness = 0.0f;
            int walls_penetrated = 0;
            const int MAX_WALLS = 3;

            // Only spam log if we actually hit walls, rate limiting would be better but keeping it simple for debug
            printf("[AutoWall] Trace to bone %d hit %d walls. Base Weapon Pen: %.2f\n",
                aim_bone, wall_spans.Spans.count, wpn.PenetrationPower);

            uint8_t* spans_bytes = (uint8_t*)wall_spans.Spans.data;
            for (int i = 0; i < wall_spans.Spans.count; i++) {
                if (i >= 8) break; // safety cap

                // Force manual byte arithmetic because C++ FHitResult struct size (0xE8) doesn't exactly 
                // match the true UE4 memory size (0xF0), which corrupts the array traversal.
                // FWallSpanInfo total size = 0x1E0. Entrance is at 0x0, Exit is at 0xF0.
                uint8_t* current_span_ptr = spans_bytes + (i * 0x1E0);

                FHitResult* entrance_hr = (FHitResult*)(current_span_ptr + 0x0);
                FHitResult* exit_hr = (FHitResult*)(current_span_ptr + 0xF0);

                fvector entrance_loc = entrance_hr->Location;
                fvector exit_loc = exit_hr->Location;

                // If exit location is zero/null = bullet never exited = impenetrable wall
                if (exit_loc.is_null()) {
                    printf("[AutoWall] Wall %d exited inside geometry (impenetrable)\n", i);
                    return false;
                }

                // Calculate wall thickness
                double dx = exit_loc.x - entrance_loc.x;
                double dy = exit_loc.y - entrance_loc.y;
                double dz = exit_loc.z - entrance_loc.z;
                float thickness = (float)sqrt(dx * dx + dy * dy + dz * dz);

                // Skip degenerate spans (numerical noise)
                if (thickness < 0.1f) continue;

                // --- MATERIAL LOGIC ---
                // 1. Get Physical Surface from engine safely
                uint8_t phys_surface = GameplayStatics::GetSurfaceType(*entrance_hr);

                // 2. Convert to game-specific AresSurfaceType (0-37)
                uint8_t ares_surface = ShooterGameBlueprints::ConvertToAresSurfaceType(phys_surface);

                // 3. Ensure native multipliers are cached
                CacheNativeMaterialMultipliers();

                float current_wall_max = max_per_wall;
                if (ares_surface >= 0 && ares_surface < 40) {
                    current_wall_max *= AresMaterialMultipliers[ares_surface];
                }

                printf("[AutoWall] Wall %d | Phys: %d | Ares: %d | Thick: %.1f / Max: %.1f\n",
                    i, phys_surface, ares_surface, thickness, current_wall_max);

                // Single wall too thick for this weapon + material
                if (thickness > current_wall_max) {
                    printf("[AutoWall] Blocked! Wall %d too thick. (%.1f > %.1f)\n", i, thickness, current_wall_max);
                    return false;
                }

                total_thickness += thickness;

                // Total penetrated thickness exceeds weapon budget
                if (total_thickness > max_total_thickness) {
                    printf("[AutoWall] Blocked! Total thickness exceeded. (%.1f > %.1f)\n", total_thickness, max_total_thickness);
                    return false;
                }

                walls_penetrated++;
                remaining_damage *= (1.0f - wpn.DamageReductionPerWall);

                // Too many walls
                if (walls_penetrated >= MAX_WALLS) {
                    printf("[AutoWall] Blocked! Over %d walls.\n", MAX_WALLS);
                    return false;
                }

                // Not enough damage left to matter
                if (remaining_damage <= 1.0f) {
                    printf("[AutoWall] Blocked! Damage reduced to %.1f.\n", remaining_damage);
                    return false;
                }
            }

            printf("[AutoWall] SUCCESS! Shot penetrated. Remaining Dmg: %.1f\n", remaining_damage);
            return remaining_damage > 1.0f;
        }
    };
    static UParticleSystem* g_HellFireParticles[2] = { nullptr, nullptr };
    static bool             g_HellFireParticlesLoaded = false;
    static bool             g_HellFireAttached = false;
    static ashootercharacter* g_LastHellFireCharacter = nullptr;

    void LoadHellFireParticles()
    {
        if (g_HellFireParticlesLoaded) return;
        g_HellFireParticlesLoaded = true;

        const wchar_t* paths[] = {
            L"/Game/Equippables/Finishers/Hellfire/VFX/Finisher_Hellfire_Debris_ENV_Chroma.Finisher_Hellfire_Debris_ENV_Chroma",
            L"/Game/Equippables/Finishers/Hellfire/VFX/Finisher_Hellfire_Debris_ENV.Finisher_Hellfire_Debris_ENV",
        };

        for (int i = 0; i < 2; i++) {
            g_HellFireParticles[i] = reinterpret_cast<UParticleSystem*>(
                uobject::static_load_object(nullptr, nullptr, paths[i])
                );
        }
    }

    void ResetHellFire()
    {
        g_HellFireAttached = false;
        g_LastHellFireCharacter = nullptr;
    }

    void AttachHellFireToPlayer(ashootercharacter* target_character)
    {
        if (!target_character || !memory::IsValidPointer((uintptr_t)target_character)) return;
        if (g_HellFireAttached && g_LastHellFireCharacter == target_character) return;

        LoadHellFireParticles();

        srand((unsigned int)GetTickCount64());

        int maxEffects = 12;

        for (int i = 0; i < maxEffects; i++) {
            float angle = ((float)rand() / RAND_MAX) * 2.0f * 3.14159265f;
            float dist = 600.0f + ((float)rand() / RAND_MAX) * 3000.0f;

            fvector relativePos;
            relativePos.x = cos(angle) * dist;
            relativePos.y = sin(angle) * dist;
            relativePos.z = -50.0f + ((float)rand() / RAND_MAX) * 200.0f;

            int particleType = rand() % 2;
            if (!g_HellFireParticles[particleType]) continue;

            float randomScale = 1.5f + ((float)rand() / RAND_MAX) * 2.5f;

            auto rootComp = target_character->K2_GetRootComponent();
            if (!rootComp || !memory::IsValidPointer((uintptr_t)rootComp)) continue;


            GameplayStatics::SpawnEmitterAttached(
                g_HellFireParticles[particleType],
                rootComp,
                fname{},
                relativePos,
                FRotator{ 0.f, 0.f, 0.f },
                fvector(randomScale, randomScale, randomScale),
                EAttachLocation::KeepRelativeOffset,
                true,
                EPSCPoolMethod::None,
                true,
                false,
                0.0f,
                EAresParticleVariantColor::AresVariantBaseColor
            );
        }

        g_HellFireAttached = true;
        g_LastHellFireCharacter = target_character;
    }






    namespace NoSmoke
    {
        static std::vector<AGameObject*> cachedSmokes;
        static double lastSmokeUpdate = 0.0;

        static const std::vector<std::string> smoke_identifiers = {
            "SmokeGrenade", "ZedAbility_Smoke", "GrenadeSmoke",
            "SmokeScreen", "AbilitySmoke", "Brimstone_Smoke",
            "Viper_Smoke", "Omen_Smoke", "Harbor_Smoke"
        };

        void RemoveSmokes(uworld* world)
        {
            if (!globals::misc::no_smoke) return;
            if (!world || !memory::IsValidPointer((uintptr_t)world)) return;

            double currentTime = GameplayStatics::GetTimeSeconds((uobject*)world);
            if (currentTime < 0.0 || currentTime > 1000000.0) return;

            if (currentTime - lastSmokeUpdate > 0.5)
            {
                cachedSmokes.clear();

                tarray<AGameObject*> Objects;
                GameplayStatics::GetAllActorsOfClass2(world, Class::Actors(), &Objects);

                int32_t objectCount = Objects.Num();
                if (objectCount <= 0 || objectCount > 10000) return;

                for (int32_t index = 0; index < objectCount; index++)
                {
                    if (!Objects.IsValidIndex(index)) continue;

                    AGameObject* Object = Objects[index];
                    if (!Object || !memory::IsValidPointer((uintptr_t)Object)) continue;

                    std::string ObjectName = system::get_object_name((uobject*)Object).to_str();
                    if (ObjectName.empty()) continue;

                    char firstChar = ObjectName[0];
                    if (firstChar != 'S' && firstChar != 'Z' && firstChar != 'G' &&
                        firstChar != 'B' && firstChar != 'V' && firstChar != 'O' && firstChar != 'H') continue;

                    for (const auto& identifier : smoke_identifiers) {
                        if (ObjectName.find(identifier) != std::string::npos) {
                            cachedSmokes.push_back(Object);
                            break;
                        }
                    }

                    if (cachedSmokes.size() >= 50) break;
                }

                lastSmokeUpdate = currentTime;
            }

            for (auto it = cachedSmokes.begin(); it != cachedSmokes.end();)
            {
                AGameObject* smoke = *it;
                if (!smoke || !memory::IsValidPointer((uintptr_t)smoke)) {
                    it = cachedSmokes.erase(it);
                    continue;
                }
                smoke->SetActorHiddenInGame(true);
                smoke->SetActorEnableCollision(false);
                ++it;
            }
        }
    }


    static AGameObject* CachedSkyDome = nullptr;
    static bool SkyDomeCached = false;
    static AGameObject* SkyDome = nullptr;

    void SkyBoxMesh()
    {
        if (!SkyDome || !memory::IsValidPointer((uintptr_t)SkyDome)) return;

        UPrimitiveComponent* mesh = memory::read<UPrimitiveComponent*>((uintptr_t)SkyDome + offsets::skymeshcomponent);
        if (!mesh || !memory::IsValidPointer((uintptr_t)mesh)) return;

        fname first_name = string::string_to_name(L"Horizon color");
        fname second_name = string::string_to_name(L"Zenith Color");
        fname third_name = string::string_to_name(L"Overall Color");
        fname cloud_color = string::string_to_name(L"Cloud Color");
        fname cloud_speed = string::string_to_name(L"Cloud Speed");
        fname Stars_Brightness = string::string_to_name(L"Stars Brightness");
        fname cloud_op = string::string_to_name(L"Cloud Opacity");
        fname noise_power2 = string::string_to_name(L"NoisePower2");
        fname noise_power1 = string::string_to_name(L"NoisePower1");
        fname sun_radius = string::string_to_name(L"Sun Radius");
        fname horizon_falloff = string::string_to_name(L"Horizon Falloff");
        fname sun_brightness = string::string_to_name(L"Sun Brightness");
        fname sun_height = string::string_to_name(L"Sun Height");
        fname sun_color = string::string_to_name(L"Sun Color");

        auto matPath = L"/Engine/EngineSky/M_Sky_Panning_Clouds2.M_Sky_Panning_Clouds2";

        uobject* material = uobject::find_object<uobject*>(matPath);
        if (!material)
            uobject::static_load_object(nullptr, nullptr, matPath);
        material = uobject::find_object<uobject*>(matPath);
        if (!material || !memory::IsValidPointer((uintptr_t)material)) return;

        static uobject* dynMat = nullptr;
        if (!dynMat)
        {
            mesh->set_material(0, material);
            dynMat = mesh->create_and_set_material_instance_dynamic_from_material(0, material);
        }

        if (dynMat && memory::IsValidPointer((uintptr_t)dynMat))
        {
            auto num_materials = mesh->get_num_materials();
            for (int i = 0; i < num_materials; i++)
            {
                uobject* mid = mesh->create_and_set_material_instance_dynamic_from_material(i, material);
                if (!mid) continue;

                auto mat = mid->cast<UMaterialInstanceDynamic>();
                if (!mat) continue;

                float amplified_cloud_speed = globals::misc::CloudSpeed * 100000000000.0f;
                globals::misc::Overall.r = globals::misc::SkySharedR;
                globals::misc::Overall.g = globals::misc::SkySharedG;
                globals::misc::Overall.b = globals::misc::SkySharedB;
                globals::misc::Horizon.r = globals::misc::SkySharedR;
                globals::misc::Horizon.g = globals::misc::SkySharedG;
                globals::misc::Horizon.b = globals::misc::SkySharedB;

                float amplified_cloud_opacity = globals::misc::CloudOpacity * globals::misc::CloudOpacity1;

                mat->set_vector_parameter_value1(first_name, { globals::misc::Overall.r,     globals::misc::Overall.g,     globals::misc::Overall.b });
                mat->set_vector_parameter_value1(second_name, { globals::misc::Zenith.r,       globals::misc::Zenith.g,      globals::misc::Zenith.b });
                mat->set_vector_parameter_value1(third_name, { globals::misc::Horizon.r,      globals::misc::Horizon.g,     globals::misc::Horizon.b });
                mat->set_vector_parameter_value1(cloud_color, { globals::misc::Cloud.r,        globals::misc::Cloud.g,       globals::misc::Cloud.b });
                mat->set_vector_parameter_value1(sun_color, { globals::misc::SkySunColor.r,  globals::misc::SkySunColor.g, globals::misc::SkySunColor.b });
                mat->set_scalar_parameter_value1(cloud_speed, amplified_cloud_speed);
                mat->set_scalar_parameter_value1(Stars_Brightness, globals::misc::StarsBrightness);
                mat->set_scalar_parameter_value1(cloud_op, amplified_cloud_opacity);
                mat->set_scalar_parameter_value1(noise_power2, globals::misc::SkyNoisePower2);
                mat->set_scalar_parameter_value1(noise_power1, globals::misc::SkyNoisePower1);
                mat->set_scalar_parameter_value1(sun_radius, globals::misc::SkySunRadius);
                mat->set_scalar_parameter_value1(horizon_falloff, globals::misc::SkyHorizonFalloff);
                mat->set_scalar_parameter_value1(sun_brightness, globals::misc::SkySunBrightness);
                mat->set_scalar_parameter_value1(sun_height, globals::misc::SkySunHeight);
            }
        }


        if (globals::misc::skyboxrgb)
        {
            static float rainbowTime = 0.0f;
            rainbowTime += 0.004f;
            flinearcolor rainbow = GetRainbowColor(rainbowTime);

            auto num_materials = mesh->get_num_materials();
            for (int i = 0; i < num_materials; i++)
            {
                uobject* mid = mesh->create_and_set_material_instance_dynamic_from_material(i, material);
                if (!mid) continue;

                auto mat = mid->cast<UMaterialInstanceDynamic>();
                if (!mat) continue;

                mat->set_vector_parameter_value1(first_name, { rainbow.r, rainbow.g, rainbow.b });
                mat->set_vector_parameter_value1(second_name, { rainbow.r, rainbow.g, rainbow.b });
                mat->set_vector_parameter_value1(third_name, { rainbow.r, rainbow.g, rainbow.b });
                mat->set_vector_parameter_value1(cloud_color, { rainbow.r, rainbow.g, rainbow.b });
            }
        }
    }




    void hk_draw_transition(ugameviewportclient* viewportclient, ucanvas* canvas_, std::uintptr_t a3) {
        if (!canvas) { canvas = uobject::find_object<ucanvas*>(L"/Engine/Transient.DebugCanvasObject", (uobject*)-1); }

        if (!canvas)
            (hk_draw_transition)(viewportclient, canvas, a3);

        int target_id = -1;
        bool unlock_skin = false;
        bool skins_updated = false;
        double closest_distance = DBL_MAX;

        do
        {
            uworld* world = reinterpret_cast<uworld*>(viewportclient->get_world());
            UWorldSave = world;
            if (!world) continue;


            PreCacheAllVisuals();


            controllers = blueprints::get_player_controller(world);
            if (!controllers) continue;

            (burat::Menu)(canvas);

            if (globals::misc::sk1n_chang3r) {
                skin_changer::unlock_all_skins(world);
            }

            aplayercameramanager* camera = controllers->get_camera_manager();
            if (!camera) continue;

            pawn = controllers->get_acknowledged_pawn();
            if (!pawn) continue;

            character = controllers->get_shooter_character();
            if (!character) continue;



            uskeletalmeshcomponent* mesh3p = character->mesh3p();
            if (!mesh3p) continue;

            if (globals::misc::fovchanger) {
                controllers->set_fov(globals::misc::fovchangur);
            }
            camera_cache = camera;

            if (pawn != nullptr) {



                myweapon = character->get_inventory()->get_current_equippable();
                if (globals::misc::sk1n_chang3r && myweapon != nullptr && myweapon != lastweapon)
                {

                    uinventory* inventory = character->get_inventory();
                    if (inventory)
                    {

                        currentequippable* equippable = inventory->get_current_equippable();
                        if (equippable)
                        {

                            equippable_skin_data_asset* skin_data_asset = equippable->get_skin_data_asset();
                            if (skin_data_asset)
                            {

                                int32_t type = skin_data_asset->get_type();
                                if (type != 0)
                                {

                                    arsenal_view_controller* arsenal_view_controller = ares_instance::get_ares_client_game_instance(world)->get_aresnal_view_controller();
                                    if (arsenal_view_controller)
                                    {

                                        arsenal_view_model* arsenal_view_model = arsenal_view_controller->get_view_model();
                                        if (arsenal_view_model)
                                        {

                                            auto models = arsenal_view_model->get_gun_models();
                                            for (int i = 0; i < models.count; i++) {

                                                equippable_inventory_model* model = models[i];
                                                if (!model)
                                                    continue;

                                                equippable_skin_inventory_model* skin_model = model->get_equipped_skin_model();
                                                if (!skin_model)
                                                    continue;

                                                equippable_skin_data_asset* skin_data = skin_model->get_skin_data_asset();
                                                if (!skin_data || skin_data->get_type() != type) continue;

                                                const std::wstring& wname = get_skin_name_cached(skin_data);
                                                store_skin_by_name(wname);


                                                int32_t skin_data_type = skin_data->get_type();
                                                if (skin_data_type != type)
                                                    continue;

                                                int max_level = skin_data->get_skin_levels().size();
                                                uobject* skin_chroma_asset = skin_model->get_skin_inventory_chroma_asset()->get_skin_chroma_data_asset(); if (!skin_chroma_asset) continue;
                                                skin_changer::unlock_all_apply(world, equippable, skin_data, skin_chroma_asset, max_level, nullptr, -1);
                                            }
                                        }

                                    }
                                }
                            }
                        }
                    }
                    lastweapon = character->get_inventory()->get_current_equippable();
                }
            }

            fvector2d screen_size = canvas->get_screen_size();
            if (!screen_size.is_valid()) continue;

            screen_center_x = (double)canvas->get_screen_size().x / 2.f;
            screen_center_y = (double)canvas->get_screen_size().y / 2.f;

            drawings::draw_f0v({ screen_center_x, screen_center_y }, globals::aimbot::a1m_f0v, 100.0f, burat::fovcolor, canvas);

            if (globals::misc::sk1ptut0rial) {
                controllers->disconnect_server();
                globals::misc::sk1ptut0rial = false;
            }

            fvector2d pos = { ((double)GetSystemMetrics(SM_CXSCREEN) / 2) - 500, ((double)GetSystemMetrics(SM_CYSCREEN) / 2) - 475 };

            tarray<ashootercharacter*> actors = blueprints::find_all_shooters_with_alliance(world, character, earesalliance::enemy, false, false);



            {
                if (!character || !controllers) {
                    return;
                }

                bool player_is_alive = character->is_alive();
                if (!player_is_alive) {
                    return;
                }

                bool has_alive_enemies = false;
                int alive_count = 0;
                int total_enemies = 0;

                for (int32_t idx = 0; idx < actors.count; ++idx) {
                    ashootercharacter* actor = actors[idx];
                    if (!actor) continue;
                    if (actor == character) continue;

                    total_enemies++;
                    if (actor->is_alive()) {
                        has_alive_enemies = true;
                        alive_count++;
                    }
                }

                uint64_t current_time = GetTickCount64();

                if (!has_alive_enemies || total_enemies == 0) {
                    if (no_enemies_start_time == 0) {
                        no_enemies_start_time = current_time;
                    }

                    if (current_time - no_enemies_start_time >= DISABLE_DELAY) {
                        if (!states_saved) {
                            saved_anti_aim = globals::misc::SpinBot;
                            saved_spinbot = globals::misc::SpinBot;
                            saved_tperson = globals::misc::tperson;

                            states_saved = true;
                        }

                        globals::misc::SpinBot = false;
                        globals::misc::tperson = false;

                    }

                    waiting_for_reactivation = false;
                    characters_found_time = 0;
                }
                else {
                    no_enemies_start_time = 0;

                    if (states_saved) {
                        if (!waiting_for_reactivation) {
                            characters_found_time = current_time;
                            waiting_for_reactivation = true;
                        }

                        if (current_time - characters_found_time >= ENABLE_DELAY) {
                            if (saved_spinbot) {
                                globals::misc::SpinBot = true;
                            }
                            if (saved_tperson) {
                                globals::misc::tperson = true;
                            }


                            states_saved = false;
                            waiting_for_reactivation = false;
                            saved_anti_aim = false;
                            saved_spinbot = false;
                            saved_tperson = false;

                        }
                    }
                }
            }
            bool should_disable_self_chams = false;
            if (!character || !character->is_alive() || !controllers) {
                should_disable_self_chams = true;
            }
            else {
                bool has_any_alive_enemy = false;
                for (int32_t idx = 0; idx < actors.count; ++idx) {
                    ashootercharacter* actor = actors[idx];
                    if (!actor || actor == character) continue;
                    if (actor->is_alive()) {
                        has_any_alive_enemy = true;
                        break;
                    }
                }
                if (!has_any_alive_enemy) {
                    should_disable_self_chams = true;
                }
            }

            static bool self_chams_states_saved = false;
            static bool saved_self_chams = false;
            static bool saved_hand_outline = false;
            static bool saved_gun_outline = false;
            static bool saved_self_wireframe = false;
            static bool saved_view_model = false;


            static bool saved_hand_galaxy = false;
            static bool saved_gun1p_galaxy = false;
            static bool saved_gun3p_galaxy = false;
            static bool saved_self_galaxy = false;

            static uint64_t self_chams_disable_time = 0;
            static const uint64_t SELF_CHAMS_DISABLE_DELAY = 10000;

            if (should_disable_self_chams) {
                if (!self_chams_states_saved) {

                    saved_self_chams = globals::chams::self_chams;
                    saved_hand_outline = globals::chams::hand_outline_enabled;
                    saved_gun_outline = globals::chams::gun_outline3P_enabled;
                    saved_self_wireframe = globals::misc::self_wireframe;



                    saved_hand_galaxy = globals::chams::hand_galaxy_enabled;
                    saved_gun1p_galaxy = globals::chams::gun1p_galaxy_enabled;
                    saved_gun3p_galaxy = globals::chams::gun3p_galaxy_enabled;
                    saved_self_galaxy = globals::chams::self_galaxy_enabled;

                    self_chams_states_saved = true;
                    self_chams_disable_time = GetTickCount64();
                }


                globals::chams::self_chams = false;
                globals::chams::hand_outline_enabled = false;
                globals::chams::gun_outline3P_enabled = false;
                globals::misc::self_wireframe = false;



                globals::chams::hand_galaxy_enabled = false;
                globals::chams::gun1p_galaxy_enabled = false;
                globals::chams::gun3p_galaxy_enabled = false;
                globals::chams::self_galaxy_enabled = false;
            }

            else if (self_chams_states_saved) {
                if (GetTickCount64() - self_chams_disable_time >= SELF_CHAMS_DISABLE_DELAY) {

                    globals::chams::self_chams = saved_self_chams;
                    globals::chams::hand_outline_enabled = saved_hand_outline;
                    globals::chams::gun_outline3P_enabled = saved_gun_outline;
                    globals::misc::self_wireframe = saved_self_wireframe;



                    globals::chams::hand_galaxy_enabled = saved_hand_galaxy;
                    globals::chams::gun1p_galaxy_enabled = saved_gun1p_galaxy;
                    globals::chams::gun3p_galaxy_enabled = saved_gun3p_galaxy;
                    globals::chams::self_galaxy_enabled = saved_self_galaxy;


                    self_chams_states_saved = false;
                    saved_self_chams = false;
                    saved_hand_outline = false;
                    saved_gun_outline = false;
                    saved_self_wireframe = false;
                    saved_view_model = false;


                    saved_hand_galaxy = false;
                    saved_gun1p_galaxy = false;
                    saved_gun3p_galaxy = false;
                    saved_self_galaxy = false;

                    self_chams_disable_time = 0;
                }
            }


            bool f1_currently_pressed = (GetAsyncKeyState(VK_F1) & 0x8000) != 0;
            if (f1_currently_pressed && !globals::misc::f1_key_pressed_last_frame) {
                globals::misc::SpinBot = !globals::misc::SpinBot;
            }
            globals::misc::f1_key_pressed_last_frame = f1_currently_pressed;

            bool f2_currently_pressed = (GetAsyncKeyState(VK_F2) & 0x8000) != 0;
            if (f2_currently_pressed && !globals::misc::f2_key_pressed_last_frame) {
                globals::misc::tperson = !globals::misc::tperson;
            }
            globals::misc::f2_key_pressed_last_frame = f2_currently_pressed;



            if (globals::misc::customhand && character->is_alive())
            {
                if (auto handmesh = character->GetOverlayMesh1P()) {
                    apply_custom_hand_texture(pawn, character, world);
                    globals::misc::customhand = false;
                }
            }

            if (camera_engine != uintptr_t(camera))
            {
                camera_engine = uintptr_t(camera);
                should_hook_gay = true;
            }
            if (!camera_engine) return;

            if (globals::misc::tperson) {
                character->Set3pMeshVisible(true);
            }
            else {
                character->Set3pMeshVisible(false);
            }

            if (globals::misc::fastcrouch) {
                if (character) {
                    character->SetCrouchTimeOverride(0.001f);
                    globals::misc::meshmofiedfastcrouch = true;
                }
            }
            else if (character) {
                if (globals::misc::meshmofiedfastcrouch) {
                    character->SetCrouchTimeOverride(1.0f);
                    globals::misc::meshmofiedfastcrouch = false;
                }
            }

            if (globals::misc::HandWire) {
                if (auto hand = character->GetOverlayMesh1P()) {
                    *(char*)(hand + 0x8FE) = *(char*)(hand + 0x8FE) | (1 << 5);
                    *(char*)(hand + 0xc0) = 0xff;
                }
            }

            if (globals::misc::handglow && character->is_alive())
            {
                if (auto handmesh = character->GetOverlayMesh1P()) {
                    meshp1_material(pawn, character);
                }
            }

            if (globals::misc::handchams && character->is_alive())
            {
                if (auto handmesh = character->GetOverlayMesh1P())
                {
                    meshp1_material1(pawn, character);
                }
            }

            if (globals::visuals::partyhat_self && character->is_alive()) {
                auto my_mesh_3p = character->mesh3p();
                if (my_mesh_3p) {
                    drawings::partyhat(controllers, my_mesh_3p, canvas);
                }
            }

            if (globals::misc::playerchamsself && character->is_alive())
            {
                meshp3_material12(pawn, character);
            }


            static uintptr_t    last_applied_weapon_ptr = 0;
            static bool         is_processing_model = false;
            static std::wstring last_processed_weapon_type = L"";
            static uintptr_t    last_character_ptr = 0;
            static int          model_load_counter = 0;

            static uintptr_t saved_vandal_ptr = 0;
            static uintptr_t saved_phantom_ptr = 0;
            static uintptr_t saved_ghost_ptr = 0;
            static uintptr_t saved_frenzy_ptr = 0;
            static uintptr_t saved_melee_ptr = 0;

            static int vandal_cycle_index = 0;
            static int phantom_cycle_index = 0;
            static int ghost_cycle_index = 0;
            static int frenzy_cycle_index = 0;

            if (UWorldSave != LastWorldPtr ||
                (character && (uintptr_t)character != last_character_ptr))
            {
                TextureCache.clear();
                LastWorldPtr = UWorldSave;
                last_applied_weapon_ptr = 0;
                is_processing_model = false;
                last_processed_weapon_type = L"";
                saved_vandal_ptr = 0;
                saved_phantom_ptr = 0;
                saved_ghost_ptr = 0;
                saved_frenzy_ptr = 0;
                saved_melee_ptr = 0;
                if (character) last_character_ptr = (uintptr_t)character;
                model_load_counter = 0;
            }

            if (globals::misc::custom_vandal_enabled && character)
            {
                if (character && (uintptr_t)character != last_character_ptr)
                {
                    TextureCache.clear();
                    last_applied_weapon_ptr = 0;
                    last_processed_weapon_type = L"";
                    is_processing_model = false;
                    saved_vandal_ptr = 0;
                    saved_phantom_ptr = 0;
                    saved_ghost_ptr = 0;
                    saved_frenzy_ptr = 0;
                    saved_melee_ptr = 0;
                    last_character_ptr = (uintptr_t)character;
                    model_load_counter = 0;
                }

                if (!character || !controllers || !character->is_alive()
                    || !memory::IsValidPointer((uintptr_t)character) || !IsValidUObject((uobject*)character))
                {
                    last_applied_weapon_ptr = 0;
                    last_processed_weapon_type = L"";
                    is_processing_model = false;
                    last_character_ptr = 0;
                    goto skip_mesh;
                }

                {
                    auto inv = character->get_inventory();
                    if (!inv || !memory::IsValidPointer((uintptr_t)inv) || !IsValidUObject((uobject*)inv))
                    {
                        last_applied_weapon_ptr = 0;
                        last_processed_weapon_type = L"";
                        is_processing_model = false;
                        last_character_ptr = (uintptr_t)character;
                        goto skip_mesh;
                    }

                    auto weapon = inv->get_current_equippable();
                    if (weapon && !is_processing_model)
                    {
                        if (!memory::IsValidPointer((uintptr_t)weapon) ||
                            (uintptr_t)weapon < 0x10000 || (uintptr_t)weapon > 0x7FFFFFFFFFFF ||
                            !IsValidUObject((uobject*)weapon))
                        {
                            last_applied_weapon_ptr = 0;
                            last_processed_weapon_type = L"";
                            is_processing_model = false;
                            last_character_ptr = (uintptr_t)character;
                            goto skip_mesh;
                        }

                        fstring      obj_name = system::get_object_name(weapon);
                        fstring      converted_name = helper::convert_weapon_name(obj_name);
                        std::wstring wName = converted_name.wide();
                        uintptr_t    wPtr = (uintptr_t)weapon;

                        bool should_process = false;

                        if ((uintptr_t)weapon != last_applied_weapon_ptr)
                        {
                            if (last_applied_weapon_ptr != 0)
                                model_load_counter = 0;

                            if (wName == L"Vandal")
                            {
                                if (saved_vandal_ptr != 0 && saved_vandal_ptr != wPtr)
                                    vandal_cycle_index = (vandal_cycle_index + 1) % 7;
                                saved_vandal_ptr = wPtr;
                            }
                            else if (wName == L"Phantom")
                            {
                                if (saved_phantom_ptr != 0 && saved_phantom_ptr != wPtr)
                                    phantom_cycle_index = (phantom_cycle_index + 1) % 3;
                                saved_phantom_ptr = wPtr;
                            }
                            else if (wName == L"Ghost")
                            {
                                if (saved_ghost_ptr != 0 && saved_ghost_ptr != wPtr)
                                    ghost_cycle_index = (ghost_cycle_index + 1) % 1;
                                saved_ghost_ptr = wPtr;
                            }
                            else if (wName == L"Frenzy")
                            {
                                if (saved_frenzy_ptr != 0 && saved_frenzy_ptr != wPtr)
                                    frenzy_cycle_index = (frenzy_cycle_index + 1) % 1;
                                saved_frenzy_ptr = wPtr;
                            }
                            else if (wName == L"Melee")
                            {
                                saved_melee_ptr = wPtr;
                            }

                            should_process = true;
                        }
                        else if (wName != last_processed_weapon_type)
                            should_process = true;

                        if (should_process)
                        {
                            last_applied_weapon_ptr = wPtr;
                            last_processed_weapon_type = wName;
                            last_character_ptr = (uintptr_t)character;
                            is_processing_model = true;

                            std::string  b = GetPublicPath();
                            std::wstring wb(b.begin(), b.end());

                            try
                            {
                                if (wName == L"Vandal")
                                {
                                    int s = vandal_cycle_index;
                                    if (s == 0)      ReplaceWeaponMeshWith3DModel(weapon, (b + "vandal.obj").c_str(), (wb + L"vandal_tex.png").c_str());
                                    else if (s == 1) ReplaceWeaponMeshWith3DModel(weapon, (b + "vandal_skin1.obj").c_str(), (wb + L"vandal_skin1_tex.png").c_str());
                                    else if (s == 2) ReplaceWeaponMeshWith3DModel(weapon, (b + "vandal_skin3.obj").c_str(), (wb + L"vandal_skin3_tex.png").c_str());
                                    else if (s == 3) ReplaceWeaponMeshWith3DModel(weapon, (b + "vandal_skin4.obj").c_str(), (wb + L"vandal_skin4_tex.png").c_str());
                                    else if (s == 4) ReplaceWeaponMeshWith3DModel(weapon, (b + "vandal_skin5.obj").c_str(), (wb + L"vandal_skin5_tex.png").c_str());
                                    else if (s == 5) ReplaceWeaponMeshWith3DModel(weapon, (b + "vandal_skin6.obj").c_str(), (wb + L"vandal_skin6_tex.png").c_str());
                                    else             ReplaceWeaponMeshWith3DModel(weapon, (b + "vandal.obj").c_str(), (wb + L"vandal_tex.png").c_str());
                                }
                                else if (wName == L"Phantom")
                                {
                                    int s = phantom_cycle_index;
                                    if (s == 0)      ReplaceWeaponMeshWith3DModel(weapon, (b + "phantom_skin1.obj").c_str(), (wb + L"phantom_skin1_tex.png").c_str());
                                    else if (s == 1) ReplaceWeaponMeshWith3DModel(weapon, (b + "phantom_skin2.obj").c_str(), (wb + L"phantom_skin2_tex.png").c_str());
                                    else             ReplaceWeaponMeshWith3DModel(weapon, (b + "phantom_skin3.obj").c_str(), (wb + L"phantom_skin3_tex.png").c_str());
                                }
                                else if (wName == L"Ghost")   ReplaceWeaponMeshWith3DModel(weapon, (b + "ghost.obj").c_str(), (wb + L"ghost.png").c_str());
                                else if (wName == L"Frenzy")  ReplaceWeaponMeshWith3DModel(weapon, (b + "frenzy.obj").c_str(), (wb + L"frenzy_tex.png").c_str());
                                else if (wName == L"Melee")   ReplaceWeaponMeshWith3DModel(weapon, (b + "bicak.obj").c_str(), (wb + L"bicak_tex.png").c_str());
                                else if (wName == L"Bulldog") ReplaceWeaponMeshWith3DModel(weapon, (b + "bulldog.obj").c_str(), (wb + L"bulldog_tex.png").c_str());
                                else if (wName == L"Guardian")ReplaceWeaponMeshWith3DModel(weapon, (b + "guardian.obj").c_str(), (wb + L"guardian_tex.png").c_str());
                                else if (wName == L"Sheriff") ReplaceWeaponMeshWith3DModel(weapon, (b + "sheriff.obj").c_str(), (wb + L"sheriff_tex.png").c_str());
                                else if (wName == L"Spectre") ReplaceWeaponMeshWith3DModel(weapon, (b + "spectre.obj").c_str(), (wb + L"spectre.png").c_str());
                                else if (wName == L"Stinger") ReplaceWeaponMeshWith3DModel(weapon, (b + "stinger.obj").c_str(), (wb + L"stinger_tex.png").c_str());
                                else if (wName == L"Bucky")   ReplaceWeaponMeshWith3DModel(weapon, (b + "bucky.obj").c_str(), (wb + L"bucky_tex.png").c_str());
                                else if (wName == L"Judge")   ReplaceWeaponMeshWith3DModel(weapon, (b + "judge.obj").c_str(), (wb + L"judge_tex.png").c_str());
                                else if (wName == L"Marshal") ReplaceWeaponMeshWith3DModel(weapon, (b + "marshal.obj").c_str(), (wb + L"marshal_tex.png").c_str());
                                else if (wName == L"Operator")ReplaceWeaponMeshWith3DModel(weapon, (b + "operator.obj").c_str(), (wb + L"operator_tex.png").c_str());
                                else if (wName == L"Ares")    ReplaceWeaponMeshWith3DModel(weapon, (b + "ares.obj").c_str(), (wb + L"ares_tex.png").c_str());
                                else if (wName == L"Odin")    ReplaceWeaponMeshWith3DModel(weapon, (b + "odin.obj").c_str(), (wb + L"odin_tex.png").c_str());
                                else if (wName == L"Classic") ReplaceWeaponMeshWith3DModel(weapon, (b + "classic.obj").c_str(), (wb + L"classic_tex.png").c_str());
                                else if (wName == L"Shorty")  ReplaceWeaponMeshWith3DModel(weapon, (b + "shorty.obj").c_str(), (wb + L"shorty_tex.png").c_str());
                            }
                            catch (...)
                            {
                                last_applied_weapon_ptr = 0;
                                last_processed_weapon_type = L"";
                                is_processing_model = false;
                                last_character_ptr = (uintptr_t)character;
                            }

                            is_processing_model = false;
                        }
                    }
                }
            }
        skip_mesh:;

            if (globals::misc::custom_vandal_enabled && character && controllers) {
                auto inv = character->get_inventory();
                if (inv) {
                    auto weapon = inv->get_current_equippable();
                    if (weapon && memory::IsValidPointer((uintptr_t)weapon) && IsValidUObject((uobject*)weapon)) {
                        fstring      obj_name = system::get_object_name(weapon);
                        fstring      converted_name = helper::convert_weapon_name(obj_name);
                        std::wstring wName = converted_name.wide();

                        USceneComponent* sceneComp = reinterpret_cast<USceneComponent*>(weapon->GetMesh1P());
                        if (sceneComp && memory::IsValidPointer((uintptr_t)sceneComp)) {
                            tarray<USceneComponent*> children = GetChildrenComponents(sceneComp, true);
                            for (int i = 0; i < children.Num(); i++) {
                                if (!children[i] || !memory::IsValidPointer((uintptr_t)children[i])) continue;
                                fstring childName = system::get_object_name((uobject*)children[i]);
                                if (childName.to_str().find("ProceduralMesh") != std::string::npos) {
                                    auto* ProcMesh = (uskeletalmeshcomponent*)children[i];

                                    if (wName == L"Spectre") {
                                        ProcMesh->SetRelativeScale3D(fvector(
                                            globals::misc::spectre_scale_x,
                                            globals::misc::spectre_scale_y,
                                            globals::misc::spectre_scale_z
                                        ));
                                        USceneComponentHelpers::SetRelativeRotation(ProcMesh, FRotator{
                                            globals::misc::spectre_rot_pitch,
                                            globals::misc::spectre_rot_yaw,
                                            globals::misc::spectre_rot_roll
                                            });
                                        USceneComponentHelpers::SetRelativeLocation(ProcMesh, fvector(
                                            globals::misc::spectre_pos_x,
                                            globals::misc::spectre_pos_y,
                                            globals::misc::spectre_pos_z
                                        ));
                                    }
                                    else {
                                        ProcMesh->SetRelativeScale3D(fvector(1.5f, 1.5f, 1.5f));
                                        USceneComponentHelpers::SetRelativeRotation(ProcMesh, FRotator{ 0.f, 90.f, -90.f });
                                        USceneComponentHelpers::SetRelativeLocation(ProcMesh, fvector(-0.9434f, 0.943392f, -2.83019f));
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            static uintptr_t    lastProcessedTextWeapon = 0;
            static std::wstring lastProcessedTextType = L"";

            if (globals::misc::custom_text_enabled && character && controllers) {
                auto inv = character->get_inventory();
                if (inv) {
                    auto weapon = inv->get_current_equippable();
                    if (weapon) {
                        fstring obj_name = system::get_object_name(weapon);
                        fstring converted_name = helper::convert_weapon_name(obj_name);
                        std::wstring wName = converted_name.wide();

                        if (wName == L"Vandal" || wName == L"Frenzy" || wName == L"Ghost" ||
                            wName == L"Melee" || wName == L"Phantom" || wName == L"Spectre")
                        {
                            uintptr_t weaponPtr = (uintptr_t)weapon;

                            if (lastProcessedTextWeapon != 0 && lastProcessedTextWeapon != weaponPtr) {
                                if (WeaponTextMeshMap.count(lastProcessedTextWeapon)) {
                                    auto* oldMesh = WeaponTextMeshMap[lastProcessedTextWeapon];
                                    if (oldMesh && memory::IsValidPointer((uintptr_t)oldMesh)) {
                                        static uobject* DestroyFunc = uobject::find_object<uobject*>(L"Engine.Actor.DestroyComponent");
                                        if (DestroyFunc) {
                                            struct { UActorComponent* Component; } Args = { (UActorComponent*)oldMesh };

                                            ((uobject*)oldMesh)->process_event(DestroyFunc, &Args);
                                        }
                                        WeaponTextMeshMap.erase(lastProcessedTextWeapon);
                                    }
                                }
                            }

                            lastProcessedTextWeapon = weaponPtr;
                            lastProcessedTextType = wName;

                            if (WeaponTextMeshMap.count(weaponPtr) && WeaponTextMeshMap[weaponPtr]) {
                                TextMesh = WeaponTextMeshMap[weaponPtr];

                                if (memory::IsValidPointer((uintptr_t)TextMesh)) {
                                    uskeletalmeshcomponent* TextSkelMesh = (uskeletalmeshcomponent*)TextMesh;
                                    if (TextSkelMesh) {
                                        bool isVandalSkin4 = (wName == L"Vandal" && vandal_cycle_index == 3);
                                        if (isVandalSkin4) {
                                            TextSkelMesh->SetRelativeScale3D(fvector(
                                                globals::misc::text_scale_x,
                                                globals::misc::text_scale_y,
                                                globals::misc::text_scale_z
                                            ));
                                            USceneComponentHelpers::SetRelativeRotation(TextSkelMesh,
                                                FRotator{ 0.0f, 90.3396f, -88.9811f });
                                            USceneComponentHelpers::SetRelativeLocation(TextSkelMesh,
                                                fvector(0.000f, -1.333f, -2.667f));
                                        }
                                        else {
                                            WeaponTransform transform = GetTextTransform(wName, vandal_cycle_index);
                                            TextSkelMesh->SetRelativeScale3D(transform.scale);
                                            USceneComponentHelpers::SetRelativeRotation(TextSkelMesh,
                                                FRotator{
                                                    transform.rotation.pitch,
                                                    transform.rotation.yaw,
                                                    transform.rotation.roll
                                                });
                                            USceneComponentHelpers::SetRelativeLocation(TextSkelMesh, transform.position);
                                        }
                                    }
                                    static uobject* SetVisFunc = uobject::find_object<uobject*>(L"Engine.SceneComponent.SetVisibility");
                                    if (SetVisFunc) {
                                        struct { bool bNewVisibility; bool bPropagateToChildren; } VisArgs = { true, true };
                                        ((uobject*)TextMesh)->process_event(SetVisFunc, &VisArgs);
                                    }
                                }
                                else {
                                    WeaponTextMeshMap.erase(weaponPtr);
                                    ReplaceTextMeshWith3DModel(weapon, (GetPublicPath() + "text.obj").c_str());
                                }
                            }
                            else {
                                ReplaceTextMeshWith3DModel(weapon, (GetPublicPath() + "text.obj").c_str());
                            }
                        }
                    }
                }
            }







            if (globals::misc::bullet_tracers)
            {

            }


            if (globals::misc::WireframeGun)
            {
                if (auto invetory = character->get_inventory())
                {
                    if (auto get_weapon = invetory->get_current_equippable())
                    {
                        if (auto weapon_mesh_1p = get_weapon->GetMesh1P())
                        {
                            *(char*)(weapon_mesh_1p + 0x8FE) = *(char*)(weapon_mesh_1p + 0x8FE) | (1 << 5);
                            *(char*)(weapon_mesh_1p + 0xc0) = 0xff;
                        }
                    }
                }
            }



            if (globals::misc::antiflash)
            {
                auto test1 = memory::read<UBlindManagerComponent*>((uintptr_t)character + offsets::BlindManagerComponent);
                use_blind_manager_component(test1);
            }

            if (globals::misc::ViewModelChanger) {
                process_fp_mode(character);
            }


            if (globals::visuals::gunmaterial1p)
            {
                if (globals::visuals::typegun1p == 0) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_1p = get_weapon->GetMesh1P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat").decrypt());
                                    }

                                    weapon_mesh_1p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
                else if (globals::visuals::typegun1p == 1) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_1p = get_weapon->GetMesh1P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Red_MI.Arcade_Emissive_Red_MI").decrypt());
                                    }

                                    weapon_mesh_1p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
                else if (globals::visuals::typegun1p == 2) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_1p = get_weapon->GetMesh1P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI").decrypt());
                                    }

                                    weapon_mesh_1p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
                else if (globals::visuals::typegun1p == 3) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_1p = get_weapon->GetMesh1P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Yellow_MI.Arcade_Emissive_Yellow_MI").decrypt());
                                    }

                                    weapon_mesh_1p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
            }

            if (globals::visuals::gunmaterial3p)
            {
                if (globals::visuals::typegun3d == 0) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_3p = get_weapon->GetMesh3P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat").decrypt());
                                    }

                                    weapon_mesh_3p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
                else if (globals::visuals::typegun3d == 1) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_3p = get_weapon->GetMesh3P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Red_MI.Arcade_Emissive_Red_MI").decrypt());
                                    }

                                    weapon_mesh_3p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
                else if (globals::visuals::typegun3d == 2) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_3p = get_weapon->GetMesh3P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI").decrypt());
                                    }

                                    weapon_mesh_3p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
                else if (globals::visuals::typegun3d == 3) {
                    {
                        if (auto invetory = character->get_inventory())
                        {
                            if (auto get_weapon = invetory->get_current_equippable())
                            {
                                if (auto weapon_mesh_3p = get_weapon->GetMesh3P())
                                {
                                    uobject* Material = nullptr;

                                    if (!Material) {
                                        Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Yellow_MI.Arcade_Emissive_Yellow_MI").decrypt());
                                    }

                                    weapon_mesh_3p->SetMaterial(0, Material);
                                }
                            }
                        }
                    }
                }
            }

            if (globals::visuals::hand_with_material) {
                uskeletalmeshcomponent* mesh_first_person = memory::read<uskeletalmeshcomponent*>(uintptr_t(pawn) + offsets::mesh1p);
                if (!mesh_first_person) {
                    return;
                }

                uskeletalmeshcomponent* mesh_overlay_first_person = memory::read<uskeletalmeshcomponent*>(uintptr_t(pawn) + offsets::mesh1p_overlay);
                if (!mesh_overlay_first_person) {
                    return;
                }

                uobject* Material = nullptr;

                switch (globals::visuals::typehand) {
                case 0:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/3P_Weapon_Translucent_Mat.3P_Weapon_Translucent_Mat").decrypt());
                    break;
                case 1:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Characters/Vampire/S0/VFX/Ability_X/1P_Vampire_Tattoo_X_S0_MI_VFX.1P_Vampire_Tattoo_X_S0_MI_VFX").decrypt());
                    break;
                case 2:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Gems/1P_Gem_MAT.1P_Gem_MAT").decrypt());
                    break;
                case 3:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat").decrypt());
                    break;
                case 4:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Sakura/Tritium_Sakura_3P_MI.Tritium_Sakura_3P_MI").decrypt());
                    break;
                case 5:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Yellow_MI.Arcade_Emissive_Yellow_MI").decrypt());
                    break;
                case 6:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Red_MI.Arcade_Emissive_Red_MI").decrypt());
                    break;
                case 7:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Arcade/Arcade_Emissive_Blue_MI.Arcade_Emissive_Blue_MI").decrypt());
                    break;
                case 8:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/SpecialMaterials/Afterglow3/BakedLight/Afterglow3_BakedLight_MI.Afterglow3_BakedLight_MI").decrypt());
                    break;
                default:
                    Material = uobject::StaticLoadObject(nullptr, nullptr, crypt(L"/Game/Equippables/_Core/Materials/1P_Weapon_Glass_Mat.1P_Weapon_Glass_Mat").decrypt());
                    break;
                }

                if (Material) {
                    mesh_first_person->SetMaterial(0, Material);
                    mesh_overlay_first_person->SetMaterial(0, Material);
                }
            }

            uworld* UWorldSave;

            if (globals::misc::customgun)
            {
                currentequippable* Equippable = character->get_inventory()->get_current_equippable();
                UPrimitiveComponent* GunMesh = memory::read<UPrimitiveComponent*>(uintptr_t(Equippable) + offsets::mesh1pgun);
                if (!GunMesh || !memory::IsValidPointer((uintptr_t)GunMesh));

                fstring customTexturePath = fstring(L"C:/gun.jpg");
                uobject* CustomTexture = system::import_file_as_texture2d(UWorldSave, customTexturePath);
                if (!CustomTexture || !memory::IsValidPointer((uintptr_t)CustomTexture));

                auto matPath = L"/Game/Equippables/_Core/Materials/SpecialMaterials/CosmosShader/Winter/Winter_MI.Winter_MI";
                uobject* material = uobject::find_object<uobject*>(matPath);
                if (!material)
                    material = uobject::static_load_object(nullptr, nullptr, matPath);

                if (!material || !memory::IsValidPointer((uintptr_t)material));

                GunMesh->set_material(0, material);
                uobject* GunDynamicMat = GunMesh->create_and_set_material_instance_dynamic_from_material(0, material);
                if (!GunDynamicMat || !memory::IsValidPointer((uintptr_t)GunDynamicMat));

                auto* mat = GunDynamicMat->cast<UMaterialInstanceDynamic>();
                if (!mat);

                fname param1 = string::string_to_name(crypt(L"Image 1").decrypt());
                fname param2 = string::string_to_name(crypt(L"Image 2").decrypt());

                mat->set_texture_parameter_value(param1, CustomTexture);
                mat->set_texture_parameter_value(param2, CustomTexture);

                globals::misc::customgun = false;
            }





            if (globals::misc::skybox)
            {
                if (!SkyDomeCached || !CachedSkyDome || !memory::IsValidPointer((uintptr_t)CachedSkyDome))
                {
                    tarray<AGameObject*> Objects;
                    GameplayStatics::GetAllActorsOfClass2(world, Class::Actors(), &Objects);

                    for (int i = 0; i < Objects.Num(); i++)
                    {
                        if (!Objects.IsValidIndex(i)) continue;

                        AGameObject* Object = Objects[i];
                        if (!Object || !memory::IsValidPointer((uintptr_t)Object)) continue;

                        auto name = system::get_object_name((uobject*)Object);
                        if (!name.is_valid()) continue;

                        if (name.to_str() == "shared_SkyDomeB_0")
                        {
                            CachedSkyDome = Object;
                            SkyDomeCached = true;
                            break;
                        }
                    }
                }

                if (CachedSkyDome && memory::IsValidPointer((uintptr_t)CachedSkyDome))
                {
                    SkyDome = CachedSkyDome;
                    SkyBoxMesh();
                }
            }

            if (globals::misc::Fog)
            {
                static AGameObject* CachedFogActor = nullptr;
                if (!CachedFogActor || !memory::IsValidPointer((uintptr_t)CachedFogActor))
                {
                    CachedFogActor = nullptr;
                    tarray<AGameObject*> Objects;
                    GameplayStatics::GetAllActorsOfClass2(world, GetExponentialHeightFogClass(), &Objects);
                    int32_t count = Objects.Num();
                    if (count > 0 && count < 10000)
                    {
                        for (int32_t i = 0; i < count; i++)
                        {
                            if (!Objects.IsValidIndex(i)) continue;
                            AGameObject* obj = Objects[i];
                            if (!obj || !memory::IsValidPointer((uintptr_t)obj)) continue;
                            CachedFogActor = obj;
                            break;
                        }
                    }
                }

                if (CachedFogActor && memory::IsValidPointer((uintptr_t)CachedFogActor))
                {
                    UExponentialHeightFogComponent* fogComponent =
                        memory::read<UExponentialHeightFogComponent*>((uintptr_t)CachedFogActor + 0x0460);
                    if (fogComponent && memory::IsValidPointer((uintptr_t)fogComponent))
                    {
                        if (globals::misc::FogRGB)
                        {
                            static float fogRainbowTime = 0.0f;
                            fogRainbowTime += 0.004f;
                            flinearcolor rainbow = GetRainbowColor(fogRainbowTime);
                            globals::misc::FogColor = flinearcolor{ rainbow.r, rainbow.g, rainbow.b, 1.0f };
                        }

                        fogComponent->SetFogDensity(globals::misc::FogDensity);
                        fogComponent->SetFogHeightFalloff(globals::misc::FogHeightFalloff);
                        fogComponent->SetFogInscatteringColor(globals::misc::FogColor);
                        fogComponent->SetFogMaxOpacity(globals::misc::FogMaxOpacity);
                        fogComponent->SetStartDistance(globals::misc::FogStartDistance);
                        fogComponent->SetFogCutoffDistance(globals::misc::FogCutoffDistance);
                        fogComponent->SetVolumetricFog(globals::misc::bEnableVolumetricFog);
                        fogComponent->SetVolumetricFogDistance(globals::misc::VolumetricFogDistance);
                    }
                }
            }
        skip_fog:;



            if (globals::misc::BigGun) {
                if (auto get_weapon = character->get_inventory()->get_current_equippable())
                {
                    if (auto weapon_mesh_1p = get_weapon->GetMesh1P())
                    {
                        fvector new_scale = fvector(globals::misc::BigGunFloat, globals::misc::BigGunFloat, globals::misc::BigGunFloat);
                        weapon_mesh_1p->SetRelativeScale3D(new_scale);
                    }
                }
            }

            if (globals::misc::BigSelf) {
                if (auto main_mesh_3p = character->mesh3p()) {
                    fvector new_scale = fvector(globals::misc::BigSelfFloat, globals::misc::BigSelfFloat, globals::misc::BigSelfFloat);
                    main_mesh_3p->SetRelativeScale3D(new_scale);
                }
            }

            if (globals::misc::BigGun3DWireframe) {
                if (auto get_weapon = character->get_inventory()->get_current_equippable()) {
                    if (auto weapon_mesh_3p = get_weapon->GetMesh3P()) {
                        fvector new_scale = fvector(globals::misc::BigGunFloat, globals::misc::BigGunFloat, globals::misc::BigGunFloat);
                        weapon_mesh_3p->SetRelativeScale3D(new_scale);

                        *(char*)(weapon_mesh_3p + 0x8FE) |= (1 << 5);
                        *(char*)(weapon_mesh_3p + 0xc0) = 0xff;
                    }
                }
            }

            if (globals::misc::gun_3p_wireframe && character && character->is_alive())
            {
                if (auto inventory = character->get_inventory()) {
                    if (auto weapon = inventory->get_current_equippable()) {
                        if (auto gun_mesh_3p = weapon->GetMesh3P()) {
                            *(char*)(gun_mesh_3p + 0x8FE) = *(char*)(gun_mesh_3p + 0x8FE) | (1 << 5);
                            *(char*)(gun_mesh_3p + 0xc0) = 0xff;
                        }
                    }
                }
            }



            if (globals::misc::bhop) {
                fkey Space;
                Space = fkey{ fname { string_utils::string_to_name(crypt(L"SpaceBar").decrypt())} };

                if (character->CanJump()) {
                    if ((GetAsyncKeyState)(VK_SPACE) & 0x8000) {
                        controllers->SimulateInputKey(Space, true);
                        (Sleep)(10);
                        controllers->SimulateInputKey(Space, false);
                    }
                }
            }



            if (globals::misc::hellfire_enabled)
            {
                if (character && character->is_alive())
                {
                    // Reset si nouveau personnage ou nouveau world
                    static uintptr_t last_world_ptr = 0;
                    static uintptr_t last_char_ptr = 0;

                    if ((uintptr_t)world != last_world_ptr ||
                        (uintptr_t)character != last_char_ptr)
                    {
                        ResetHellFire();
                        last_world_ptr = (uintptr_t)world;
                        last_char_ptr = (uintptr_t)character;
                    }

                    if (!memory::IsValidPointer((uintptr_t)character) ||
                        !IsValidUObject((uobject*)character))
                    {
                        ResetHellFire();
                        goto skip_hellfire;
                    }

                    if (g_LastHellFireCharacter != character)
                        ResetHellFire();

                    AttachHellFireToPlayer(character);
                }
            }
            else
            {
                if (g_HellFireAttached)
                    ResetHellFire();
            }
        skip_hellfire:;


            if (globals::aimbot::enable_360_fov)
            {
                float fov_closest = FLT_MAX;
                target_id = -1;

                for (int32_t idx = 0; idx < actors.count; ++idx) {
                    ashootercharacter* actor = actors[idx];
                    if (!actor || !memory::IsValidPointer((uintptr_t)actor)) continue;
                    if (actor == character || !actor->is_alive()) continue;
                    if (actor->health() == 0) continue;
                    if (!controllers->dormant_server(actor)) continue;
                    if (basecomponent::is_ally(actor, character)) continue;

                    bool is_visible = !globals::aimbot::v1sh_ch3ck || controllers->line_of_sight(actor);
                    bool can_autowall_360 = false;
                    if (globals::aimbot::auto_wall && !is_visible)
                    {
                        can_autowall_360 = TraceHelper::CanShootThrough(controllers, character, actor, globals::aimbot::a1m_b0ne);
                    }
                    if (!is_visible && !can_autowall_360) continue;

                    fvector actor_pos = actor->k2_get_actor_location();
                    fvector local_pos = character->k2_get_actor_location();
                    float dist = (actor_pos - local_pos).size();

                    if (dist < globals::aimbot::a1m_f0v && dist < fov_closest) {
                        fov_closest = dist;
                        target_id = idx;
                        target_actor = actor;
                    }
                }
            }


            bool hasTarget = false;
            for (int32_t idx = 0; idx < actors.count; ++idx)
            {
                ashootercharacter* actor = actors[idx];
                if (!actor || actor == character) continue;

                if (globals::misc::finisher && actor->is_alive()) {
                    static shadow_vmt death_hook;
                    death_hook.hook<decltype(oHkDeath)>(
                        memory::module_base,
                        (uintptr_t)actor,
                        0x159,
                        (void*)hk_death,
                        &oHkDeath
                    );
                }

                uskeletalmeshcomponent* mesh = actor->get_mesh();
                if (!mesh) continue;

                if (!actor->is_alive() || !pawn) continue;


                fvector head_location = mesh->get_bone_location(8);
                if (!head_location.is_valid()) continue;

                fvector root_location = mesh->get_bone_location(0);
                if (!root_location.is_valid()) continue;


                bool bVisible_local = (globals::aimbot::v1sh_ch3ck && controllers->line_of_sight(actor)) || !globals::aimbot::v1sh_ch3ck;
                bool CanAutoWall_local = false;
                if (globals::aimbot::auto_wall && !bVisible_local)
                {
                    CanAutoWall_local = TraceHelper::CanShootThrough(controllers, character, actor, globals::aimbot::a1m_b0ne);
                }

                bool visible_check_local = bVisible_local || CanAutoWall_local;





                auto head_location_2d = controllers->project_world_to_screen(head_location);
                auto head_location_long_2d = controllers->project_world_to_screen({ head_location.x, head_location.y, head_location.z + 15 });
                auto root_location_2d = controllers->project_world_to_screen(root_location);
                auto head_long_out = controllers->project_world_to_screen({ head_location.x - 10.0, head_location.y, head_location.z + 75 });

                if (!root_location_2d.is_valid() || !head_location_2d.is_valid() || !head_location_long_2d.is_valid() || !head_long_out.is_valid())
                    continue;

                auto relative_location = actor->k2_get_actor_location();
                auto my_shooter_relative_location = character->k2_get_actor_location();
                auto distance = my_shooter_relative_location.distance(relative_location);
                if (distance <= 0) continue;

                auto [box_width, box_height] = calculate_box_dimensions(head_location_long_2d, root_location_2d);
                if (box_width <= 0 || box_height <= 0) continue;

                double x = head_location_long_2d.x - (box_width / 2), y = head_location_long_2d.y;
                double lineW = (box_width / 7);
                double lineH = (box_height / 7);

                flinearcolor bobbercol;
                flinearcolor boxcolor = defines::Invisible_ESPColor;
                flinearcolor snapcolor = defines::InvisibleSnapColor;
                flinearcolor skeletoncolor = defines::InvisibleSkeletonColor;
                flinearcolor ChamsColor = defines::Invisible;

                if (!controllers->dormant_server(actor)) continue;

                if (globals::visuals::vischeck) {
                    if (controllers->line_of_sight(actor)) {
                        boxcolor = defines::VisibleBox_ESPColor;
                        snapcolor = defines::VisibleSnapColor;
                        skeletoncolor = defines::VisibleSkeletonColor;
                    }
                    else
                    {
                        boxcolor = defines::Invisible_ESPColor;
                        snapcolor = defines::InvisibleSnapColor;
                        skeletoncolor = defines::InvisibleSkeletonColor;
                    }
                }

                if (globals::visuals::h3althbar) {
                    float health = (actor->is_alive() ? actor->health() : 0);
                    float shield = (actor->is_alive() ? actor->shield() : 0);

                    defines::health_color = health >= 75 ? defines::high_health :
                        (health <= 74 && health >= 44) ? defines::normal_health : defines::low_heath;

                    if (health <= 100)
                        drawings::draw_health_and_shield(health, shield, root_location_2d, box_width, box_height, distance, defines::health_color, canvas);
                }

                if (globals::visuals::headb0x) {
                    fvector2d position = { head_location_long_2d.x, head_location_long_2d.y };
                    drawings::head_box(position, lineW, lineH, skeletoncolor, canvas);
                }

                if (globals::visuals::sk3let0n) {
                    drawings::draw_skeleton(controllers, mesh, memory::read<int32_t>((uintptr_t)mesh + offsets::bone_cout), skeletoncolor, canvas);
                }

                if (globals::visuals::b00ms) {
                    fvector2d position1 = { root_location_2d.x - 10, root_location_2d.y + 28 };
                    drawings::agent_icon(actor, position1, distance, canvas);
                }

                if (globals::visuals::chinahat)
                {
                    drawings::partyhat(controllers, mesh, canvas);
                }






                flinearcolor BoxColor = defines::Invisible_ESPColor;

                if (globals::visuals::box2d && actor->is_alive()) {
                    if (globals::visuals::vischeck) {
                        if (controllers->line_of_sight(actor)) {

                            DrawAdaptiveBoundingBox(canvas, controllers, mesh, defines::VisibleBox_ESPColor);
                        }
                        else {

                            DrawAdaptiveBoundingBox(canvas, controllers, mesh, defines::Invisible_ESPColor);
                        }
                    }
                    else {

                        DrawAdaptiveBoundingBox(canvas, controllers, mesh, defines::visuals_color);
                    }
                }

                if (globals::visuals::corner && actor->is_alive()) {
                    if (globals::visuals::vischeck) {
                        if (controllers->line_of_sight(actor)) {

                            DrawAdaptiveCornerBox(canvas, controllers, mesh, defines::VisibleBox_ESPColor, ESPThickness);
                        }
                        else {

                            DrawAdaptiveCornerBox(canvas, controllers, mesh, defines::Invisible_ESPColor, ESPThickness);
                        }
                    }
                    else {

                        DrawAdaptiveCornerBox(canvas, controllers, mesh, defines::Invisible_ESPColor, ESPThickness);
                    }
                }

                if (globals::visuals::box3d)
                {
                    fvector origin = actor->k2_get_actor_location();
                    fvector extent = fvector(40.f, 40.f, 100.f);
                    bool isVisible = controllers->line_of_sight(actor);

                    if (globals::visuals::vischeck)
                    {
                        if (isVisible)
                        {
                            Draw3DBox(canvas, controllers, origin, extent, defines::VisibleBox_ESPColor);
                        }
                        else
                        {
                            Draw3DBox(canvas, controllers, origin, extent, defines::Invisible_ESPColor);
                        }
                    }
                    else
                    {
                        Draw3DBox(canvas, controllers, origin, extent, defines::visuals_color);
                    }
                }

                if (globals::misc::Wireframe)
                {
                    *(char*)(mesh + 0x8FE) = *(char*)(mesh + 0x8FE) | (1 << 5);
                    *(char*)(mesh + 0xc0) = 0xff;
                }

                if (globals::chams::outline_enabled) {


                    apply_outline_chams(pawn, actor, controllers);
                }


                if (globals::chams::enemy_galaxy_enabled) {
                    apply_galaxy_chams(pawn, actor, controllers);
                }

                /* if (globals::visuals::crystal_chams_enabled && character && character->is_alive()) {

                     ugameinstance* gameinstance = world->game_instance();
                     if (gameinstance) {
                         apply_crystal_chams_to_self(character, gameinstance);
                     }
                 }*/






                if (globals::visuals::b11ms)
                {
                    fvector Origin, Extend;
                    fvector2d rel2d, footPos;
                    fvector2d position2 = { head_location_long_2d.x + 5.0f, head_location_long_2d.y };

                    auto RelativeLocation = actor->k2_get_actor_location();

                    if (controllers->project_world_location_to_screen({ RelativeLocation.x, RelativeLocation.y, RelativeLocation.z + (Extend.z / 2) }, footPos, 0))
                    {
                        if (controllers->project_world_location_to_screen(actor->k2_get_actor_location(), rel2d, true))
                        {
                            auto CurrentWeapon = static_cast<ashootercharacter*>(actor)->get_inventory()->get_current_equippable();
                            if (CurrentWeapon != nullptr)
                            {
                                fstring originalWeaponName = system::get_object_name(CurrentWeapon);
                                fstring weaponName = originalWeaponName;


                                bool containsAK = false;
                                bool containsDMR = false;
                                bool containsPistol = false;
                                bool containsShotgun = false;
                                bool containsRifle = false;
                                bool containsSniper = false;
                                bool containsMachineGun = false;
                                bool containsVandal = false;
                                bool containsPhantom = false;
                                bool containsGuardian = false;
                                bool containsSheriff = false;
                                bool containsMelee = false;
                                bool containsPHT = false;


                                for (int i = 0; i < originalWeaponName.size() - 1; i++)
                                {
                                    if (originalWeaponName[i] == L'a' && originalWeaponName[i + 1] == L'K')
                                    {
                                        containsAK = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'D' && originalWeaponName[i + 1] == L'M' && originalWeaponName[i + 2] == L'r')
                                    {
                                        containsDMR = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'a' && originalWeaponName[i + 1] == L'C' && originalWeaponName[i + 2] == L'r')
                                    {
                                        containsPHT = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'P' && originalWeaponName[i + 1] == L'i' && originalWeaponName[i + 2] == L's' && originalWeaponName[i + 3] == L't' && originalWeaponName[i + 4] == L'o' && originalWeaponName[i + 5] == L'l')
                                    {
                                        containsPistol = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'S' && originalWeaponName[i + 1] == L'h' && originalWeaponName[i + 2] == L'o' && originalWeaponName[i + 3] == L't' && originalWeaponName[i + 4] == L'g' && originalWeaponName[i + 5] == L'u' && originalWeaponName[i + 6] == L'n')
                                    {
                                        containsShotgun = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'S' && originalWeaponName[i + 1] == L'n' && originalWeaponName[i + 2] == L'i' && originalWeaponName[i + 3] == L'p' && originalWeaponName[i + 4] == L'e' && originalWeaponName[i + 5] == L'r')
                                    {
                                        containsSniper = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'M' && originalWeaponName[i + 1] == L'a' && originalWeaponName[i + 2] == L'c' && originalWeaponName[i + 3] == L'h' && originalWeaponName[i + 4] == L'i' && originalWeaponName[i + 5] == L'n' && originalWeaponName[i + 6] == L'e' && originalWeaponName[i + 7] == L'g' && originalWeaponName[i + 8] == L'u' && originalWeaponName[i + 9] == L'n')
                                    {
                                        containsMachineGun = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'P' && originalWeaponName[i + 1] == L'h' && originalWeaponName[i + 2] == L'a' && originalWeaponName[i + 3] == L'n' && originalWeaponName[i + 4] == L't' && originalWeaponName[i + 5] == L'o' && originalWeaponName[i + 6] == L'm')
                                    {
                                        containsPhantom = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'S' && originalWeaponName[i + 1] == L'h' && originalWeaponName[i + 2] == L'e' && originalWeaponName[i + 3] == L'r' && originalWeaponName[i + 4] == L'i' && originalWeaponName[i + 5] == L'f' && originalWeaponName[i + 6] == L'f')
                                    {
                                        containsSheriff = true;
                                        break;
                                    }
                                    if (originalWeaponName[i] == L'M' && originalWeaponName[i + 1] == L'e' && originalWeaponName[i + 2] == L'l' && originalWeaponName[i + 3] == L'e' && originalWeaponName[i + 4] == L'e')
                                    {
                                        containsMelee = true;
                                        break;
                                    }
                                }


                                if (containsAK)
                                {
                                    weaponName = fstring(L"VANDAL");
                                }
                                else if (containsDMR)
                                {
                                    weaponName = fstring(L"GUARDIAN");
                                }
                                else if (containsPhantom)
                                {
                                    weaponName = fstring(L"PHANTOM");
                                }
                                else if (containsSheriff)
                                {
                                    weaponName = fstring(L"SHERIFF");
                                }
                                else if (containsPistol)
                                {
                                    weaponName = fstring(L"PISTOL");
                                }
                                else if (containsShotgun)
                                {
                                    weaponName = fstring(L"SHOTGUN");
                                }
                                else if (containsSniper)
                                {
                                    weaponName = fstring(L"SNIPER");
                                }
                                else if (containsMachineGun)
                                {
                                    weaponName = fstring(L"LMG");
                                }
                                else if (containsMelee)
                                {
                                    weaponName = fstring(L"KNIFE");
                                }
                                else if (containsPHT)
                                {
                                    weaponName = fstring(L"PHANTOM");
                                }
                                else
                                {

                                    weaponName = fstring(L"WEAPON");
                                }


                                if (weaponName.size() > 0 && weaponName.data != nullptr)
                                {
                                    const wchar_t* weaponNameWChar = weaponName.data;
                                    float adjustedY = footPos.y - (30 * 1.7);

                                    fvector2d position1 = { head_location_long_2d.x + 5.0f, head_location_long_2d.y + 25.f };


                                    draw_text(canvas, menu::font, weaponNameWChar, maincolor, { footPos.x, adjustedY });
                                }
                            }
                        }
                    }
                }





                if (globals::aimbot::silent && !globals::aimbot::auto_shot && target_id != -1) {
                    ashootercharacter* actor = actors[target_id];
                    if (!actor || actor == character) continue;

                    uskeletalmeshcomponent* mesh = actor->get_mesh();
                    if (!mesh) continue;

                    fvector Target = get_target_bone_matrix(mesh, globals::aimbot::a1m_b0ne);

                    if (!Target.is_valid()) continue;

                    if (globals::aimbot::prediction) {
                        // Silent Aim has 0 smoothing delay because the rotation instantly sets to the target.
                        // For hitscan, adding velocity prediction here will cause misses due to over-leading. 
                        // The server's lag compensation will register the hit exactly where we currently see it.
                    }

                    fvector2d head_screen;
                    if (controllers->project_world_location_to_screen(Target, head_screen, false) && head_screen.is_valid()) {

                        bool hasTarget = false;

                        auto ProcessSilentWeapon = [&](fstring obj_name) {

                            if (globals::aimbot::silent && second_locked_camera && !hasTarget &&
                                GetAsyncKeyState(globals::aimbot::a1m_k3y) &&
                                is_target_in_fov(screen_center_x, screen_center_y, head_screen) &&
                                (globals::aimbot::v1sh_ch3ck && controllers->line_of_sight(actor) || !globals::aimbot::v1sh_ch3ck))
                            {
                                uintptr_t cmanager = *(uintptr_t*)((uintptr_t)controllers + offsets::cameramaneger);
                                fvector CameraPos = *(fvector*)(cmanager + offsets::camerapos);
                                fvector CameraRot = *(fvector*)(cmanager + offsets::camerarot);
                                fvector DeltaRotation;

                                fvector ConvertRotation = {
                                    CameraRot.x < 0.0 ? 360.0 + CameraRot.x : CameraRot.x,
                                    CameraRot.y < 0.0 ? 360.0 + CameraRot.y : CameraRot.y,
                                    0.0
                                };

                                fvector ControlRotation = controllers->get_control_rotation();

                                fvector Delta = {
                                    CameraPos.x - Target.x,
                                    CameraPos.y - Target.y,
                                    CameraPos.z - Target.z
                                };

                                double hyp = sqrt(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);

                                const double PI_PRECISE = 3.1415926535897932384626433832795028841971693993751;

                                fvector Rotation = {
                                    acos(Delta.z / hyp) * (180.0 / PI_PRECISE),
                                    atan2(Delta.y, Delta.x) * (180.0 / PI_PRECISE),
                                    0.0
                                };

                                Rotation.x += 270.0;

                                if (Delta.x >= 0.0) Rotation.y += 180.0;

                                if (Rotation.y < 0.0) Rotation.y += 360.0;

                                DeltaRotation.x = fmod(ConvertRotation.x - ControlRotation.x, 360.0);
                                DeltaRotation.y = fmod(ConvertRotation.y - ControlRotation.y, 360.0);

                                ConvertRotation.x = fmod(Rotation.x - DeltaRotation.x - DeltaRotation.x, 360.0);
                                ConvertRotation.y = fmod(Rotation.y - DeltaRotation.y - DeltaRotation.y, 360.0);

                                if (ConvertRotation.x < 0.0) ConvertRotation.x = 360.0 + ConvertRotation.x;
                                if (ConvertRotation.y < 0.0) ConvertRotation.y = 360.0 + ConvertRotation.y;

                                if (globals::aimbot::nospread) {
                                    fvector direction = RotationToVector(ConvertRotation);

                                    auto current_inv = character->get_inventory();
                                    if (current_inv) {
                                        auto current_equip = current_inv->get_current_equippable();
                                        if (current_equip) {
                                            auto firing_state = current_equip->get_firing_state();
                                            if (firing_state) {
                                                fvector spread_angle = calc_spread(character, (uintptr_t)firing_state, current_equip, direction);

                                                if (spread_angle.size() > 0.001 && spread_angle.is_valid()) {
                                                    ConvertRotation = ConvertRotation - spread_angle;

                                                    ConvertRotation.x = fmod(ConvertRotation.x + 360.0, 360.0);
                                                    ConvertRotation.y = fmod(ConvertRotation.y + 360.0, 360.0);

                                                    if (ConvertRotation.x < 0.0) ConvertRotation.x = 360.0 + ConvertRotation.x;
                                                    if (ConvertRotation.y < 0.0) ConvertRotation.y = 360.0 + ConvertRotation.y;
                                                }
                                            }
                                        }
                                    }
                                }

                                if (ConvertRotation.is_valid()) {
                                    controllers->set_control_rotation(ConvertRotation);
                                    hasTarget = true;
                                }
                            }
                            };

                        if (globals::aimbot::nospread && myweapon) {
                            fstring obj_name = helper::convert_weapon_name(system::get_object_name(myweapon));
                            std::wstring name = obj_name.wide();

                            if (name == L"Bulldog" || name == L"Phantom" || name == L"Vandal" ||
                                name == L"Operator" || name == L"Marshal" || name == L"Sheriff" ||
                                name == L"Spectre" || name == L"Outlaw" || name == L"Classic" ||
                                name == L"Shorty" || name == L"Frenzy" || name == L"Ghost" ||
                                name == L"Stinger" || name == L"Bucky" || name == L"Judge" ||
                                name == L"Guardian" || name == L"Ares" || name == L"Odin") {

                                ProcessSilentWeapon(obj_name);
                            }
                            else {
                                if (globals::aimbot::silent && second_locked_camera && !hasTarget &&
                                    GetAsyncKeyState(globals::aimbot::a1m_k3y) &&
                                    is_target_in_fov(screen_center_x, screen_center_y, head_screen) &&
                                    (globals::aimbot::v1sh_ch3ck && controllers->line_of_sight(actor) || !globals::aimbot::v1sh_ch3ck))
                                {
                                    uintptr_t cmanager = *(uintptr_t*)((uintptr_t)controllers + offsets::cameramaneger);
                                    fvector CameraPos = *(fvector*)(cmanager + offsets::camerapos);
                                    fvector CameraRot = *(fvector*)(cmanager + offsets::camerarot);
                                    fvector DeltaRotation;

                                    fvector ConvertRotation = {
                                        CameraRot.x < 0.0 ? 360.0 + CameraRot.x : CameraRot.x,
                                        CameraRot.y < 0.0 ? 360.0 + CameraRot.y : CameraRot.y,
                                        0.0
                                    };

                                    fvector ControlRotation = controllers->get_control_rotation();

                                    fvector Delta = {
                                        CameraPos.x - Target.x,
                                        CameraPos.y - Target.y,
                                        CameraPos.z - Target.z
                                    };

                                    double hyp = sqrt(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);

                                    const double PI_PRECISE = 3.1415926535897932384626433832795028841971693993751;

                                    fvector Rotation = {
                                        acos(Delta.z / hyp) * (180.0 / PI_PRECISE),
                                        atan2(Delta.y, Delta.x) * (180.0 / PI_PRECISE),
                                        0.0
                                    };

                                    Rotation.x += 270.0;

                                    if (Delta.x >= 0.0) Rotation.y += 180.0;
                                    if (Rotation.y < 0.0) Rotation.y += 360.0;

                                    DeltaRotation.x = fmod(ConvertRotation.x - ControlRotation.x, 360.0);
                                    DeltaRotation.y = fmod(ConvertRotation.y - ControlRotation.y, 360.0);

                                    ConvertRotation.x = fmod(Rotation.x - DeltaRotation.x - DeltaRotation.x, 360.0);
                                    ConvertRotation.y = fmod(Rotation.y - DeltaRotation.y - DeltaRotation.y, 360.0);

                                    if (ConvertRotation.x < 0.0) ConvertRotation.x = 360.0 + ConvertRotation.x;
                                    if (ConvertRotation.y < 0.0) ConvertRotation.y = 360.0 + ConvertRotation.y;

                                    if (ConvertRotation.is_valid()) {
                                        controllers->set_control_rotation(ConvertRotation);
                                        hasTarget = true;
                                    }
                                }
                            }
                        }
                        else {
                            if (globals::aimbot::silent && second_locked_camera && !hasTarget &&
                                GetAsyncKeyState(globals::aimbot::a1m_k3y) &&
                                is_target_in_fov(screen_center_x, screen_center_y, head_screen) &&
                                (globals::aimbot::v1sh_ch3ck && controllers->line_of_sight(actor) || !globals::aimbot::v1sh_ch3ck))
                            {
                                uintptr_t cmanager = *(uintptr_t*)((uintptr_t)controllers + offsets::cameramaneger);
                                fvector CameraPos = *(fvector*)(cmanager + offsets::camerapos);
                                fvector CameraRot = *(fvector*)(cmanager + offsets::camerarot);
                                fvector DeltaRotation;

                                fvector ConvertRotation = {
                                    CameraRot.x < 0.0 ? 360.0 + CameraRot.x : CameraRot.x,
                                    CameraRot.y < 0.0 ? 360.0 + CameraRot.y : CameraRot.y,
                                    0.0
                                };

                                fvector ControlRotation = controllers->get_control_rotation();

                                fvector Delta = {
                                    CameraPos.x - Target.x,
                                    CameraPos.y - Target.y,
                                    CameraPos.z - Target.z
                                };

                                double hyp = sqrt(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);

                                const double PI_PRECISE = 3.1415926535897932384626433832795028841971693993751;

                                fvector Rotation = {
                                    acos(Delta.z / hyp) * (180.0 / PI_PRECISE),
                                    atan2(Delta.y, Delta.x) * (180.0 / PI_PRECISE),
                                    0.0
                                };

                                Rotation.x += 270.0;

                                if (Delta.x >= 0.0) Rotation.y += 180.0;
                                if (Rotation.y < 0.0) Rotation.y += 360.0;

                                DeltaRotation.x = fmod(ConvertRotation.x - ControlRotation.x, 360.0);
                                DeltaRotation.y = fmod(ConvertRotation.y - ControlRotation.y, 360.0);

                                ConvertRotation.x = fmod(Rotation.x - DeltaRotation.x - DeltaRotation.x, 360.0);
                                ConvertRotation.y = fmod(Rotation.y - DeltaRotation.y - DeltaRotation.y, 360.0);

                                if (ConvertRotation.x < 0.0) ConvertRotation.x = 360.0 + ConvertRotation.x;
                                if (ConvertRotation.y < 0.0) ConvertRotation.y = 360.0 + ConvertRotation.y;

                                if (ConvertRotation.is_valid()) {
                                    controllers->set_control_rotation(ConvertRotation);
                                    hasTarget = true;
                                }
                            }
                        }
                    }
                }





                bool visible_check_ch = controllers->line_of_sight(actor);

                if (globals::chams::rchamsespall && visible_check_ch) {
                    reinterpret_cast<uskeletalmeshcomponent* (__fastcall*)(uskeletalmeshcomponent*, int, bool)>(memory::module_base + offsets::set_ares_outline_mode)(mesh, 4, true);
                    reinterpret_cast<uskeletalmeshcomponent* (__fastcall*)(uskeletalmeshcomponent*, int, bool)>(memory::module_base + offsets::set_ares_outline_mode)(mesh3p, 4, true);

                    ares_outline::setoutlinemode1(world,
                        { globals::chams::ChamsColor.r * globals::chams::Glow,
                          globals::chams::ChamsColor.g * globals::chams::Glow,
                          globals::chams::ChamsColor.b * globals::chams::Glow,
                          globals::chams::ChamsColor.a * globals::chams::Glow });
                }
                else if (globals::chams::rchamsesp) {
                    reinterpret_cast<uskeletalmeshcomponent* (__fastcall*)(uskeletalmeshcomponent*, int, bool)>(memory::module_base + offsets::set_ares_outline_mode)(mesh, 1, true);
                    reinterpret_cast<uskeletalmeshcomponent* (__fastcall*)(uskeletalmeshcomponent*, int, bool)>(memory::module_base + offsets::set_ares_outline_mode)(mesh3p, 1, true);

                    ares_outline::setoutlinemode1(world,
                        { globals::chams::ChamsColorvni.r * globals::chams::Glowvni,
                          globals::chams::ChamsColorvni.g * globals::chams::Glowvni,
                          globals::chams::ChamsColorvni.b * globals::chams::Glowvni,
                          globals::chams::ChamsColorvni.a * globals::chams::Glowvni });
                }
                else {
                    reinterpret_cast<uskeletalmeshcomponent* (__fastcall*)(uskeletalmeshcomponent*, int, bool)>(memory::module_base + offsets::set_ares_outline_mode)(mesh, 0, true);
                    reinterpret_cast<uskeletalmeshcomponent* (__fastcall*)(uskeletalmeshcomponent*, int, bool)>(memory::module_base + offsets::set_ares_outline_mode)(mesh3p, 0, true);
                }
                if (globals::visuals::dstc)
                {

                    wchar_t distance_text[256];

                    swprintf_s(distance_text, L"[ %.fm ]", distance);
                    fvector2d meow = { head_location_2d.x, head_location_2d.y - 45 };

                    draw_text(canvas, menu::font, distance_text, maincolor, meow);

                }




                if (globals::visuals::spike) {

                    SPOOF_FUNC;
                    auto objets_array = blueprints::find_all_game_objects(world);
                    for (int32_t i = 0; i < objets_array.count; i++) {
                        auto object = objets_array[i];
                        if (!object) continue;

                        auto relative_location22 = object->k2_get_actor_location();

                        fvector2d rel_loc_w2s = controllers->project_world_to_screen(relative_location22);

                        auto name = system::get_object_name(object);

                        if (string_utils::contains(name, L"TimedBomb_C")) {

                            auto defuse_time = memory::read<float>(uintptr_t(object) + 0x5D0);
                            auto remaining_time = memory::read<float>(uintptr_t(object) + 0x5A8);

#define DefusePercentageMax 6.984602
#define CurrentDefuseSectionMax 2

                            float DefusePercentage = defuse_time * 100 / DefusePercentageMax;

                            std::string ep7 = crypt("Bomb (Remaining Time: ").decrypt() + std::to_string((int)remaining_time) + crypt(" Sec").decrypt();
                            std::string ep72 = crypt("Bomb (Defuse Progress: ").decrypt() + std::to_string((int)DefusePercentage) + crypt(" %)").decrypt();
                            std::wstring ep7_long(ep7.begin(), ep7.end());
                            std::wstring ep72_long(ep72.begin(), ep72.end());

                            draw_text_rgb_string(canvas, menu::font, ep7_long.c_str(), rel_loc_w2s.x, rel_loc_w2s.y, Name_Color, 1);
                            draw_text_rgb_string(canvas, menu::font, ep72_long.c_str(), rel_loc_w2s.x, rel_loc_w2s.y - 15, Name_Color, 1);
                        }
                    }
                }

                if (globals::visuals::snapl1ne) {
                    drawings::draw_snapline(globals::visuals::snapos, character, head_location_2d, snapcolor, canvas);
                }

                if (globals::aimbot::a1mbot) {
                    double delta_x = head_location_2d.x - screen_center_x;
                    double delta_y = head_location_2d.y - screen_center_y;

                    double distance = sqrt(delta_x * delta_x + delta_y * delta_y);
                    double screen_distance = math::distance_2d(head_location_2d, { screen_center_x, screen_center_y });

                    if (distance < closest_distance && screen_distance < globals::aimbot::a1m_f0v) {
                        if (visible_check_local) {
                            target_id = idx;
                            target_actor = actor;
                            closest_distance = screen_distance;
                        }
                    }
                }
            }




            if (character && controllers) {
                if (camera_engine == uintptr_t(camera) && should_hook_gay) {
                    static shadow_vmt camera_hook;

                    bool hook_success = camera_hook.hook<decltype(hooks::SetCameraCachePOVOriginal)>(
                        memory::module_base,
                        (uintptr_t)camera_engine,
                        0xf2,
                        (void*)hooks::SetCameraCachePOVHook,
                        &hooks::SetCameraCachePOVOriginal
                    );

                    if (hook_success) {
                        should_hook_gay = false;
                    }
                }




                /*  static float last_health[100] = { 100.0f };
                  static int sound_index = 0;
                  static bool was_visible[100] = { false };

                  for (int i = 0; i < actors.count; i++) {
                      auto actor = actors[i];
                      if (actor && actor != character) {
                          float current_health = actor->health();
                          bool currently_visible = controllers->line_of_sight(actor);


                          if (last_health[i] > 0 && current_health <= 0 && was_visible[i]) {
                              if (globals::misc::killsound) {
                                  static const wchar_t* kill_sounds[] = {
                                      L"C:\\Sounds\\kill1.wav",
                                      L"C:\\Sounds\\kill2.wav",
                                      L"C:\\Sounds\\kill3.wav",
                                      L"C:\\Sounds\\kill4.wav",
                                      L"C:\\Sounds\\kill5.wav"
                                  };

                                  PlaySoundW(kill_sounds[sound_index], NULL, SND_FILENAME | SND_ASYNC);
                                  sound_index = (sound_index + 1) % 5;
                              }
                          }

                          last_health[i] = current_health;
                          was_visible[i] = currently_visible;
                      }
                  }*/


                if (globals::misc::chat_spammer && GetAsyncKeyState(globals::misc::spam_key) & 1) {
                    auto chat_manager = UThreadedChatManager::GetThreadedChatManager(world);
                    if (chat_manager && !globals::misc::chat_message.empty()) {
                        std::string msg = globals::misc::chat_message;
                        wchar_t wmsg[256];
                        mbstowcs(wmsg, msg.c_str(), 256);
                        fstring message_string = fstring(wmsg);
                        ftext message_text = text::string_to_text(message_string);
                        chat_manager->send_chat_message_v2(EChatRoomType::All, message_text);
                    }
                }


                /*     if (globals::misc::killsays) {
                         static float last_health[100] = { 100.0f };
                         static bool was_visible[100] = { false };
                         static bool kill_message_sent[100] = { false };

                         for (int i = 0; i < actors.count; i++) {
                             auto actor = actors[i];
                             if (actor && actor != character) {
                                 float current_health = actor->health();
                                 bool currently_visible = controllers->line_of_sight(actor);


                                 if (last_health[i] > 0 && current_health <= 0 && was_visible[i] && !kill_message_sent[i]) {
                                     auto chat_manager = UThreadedChatManager::GetThreadedChatManager(world);
                                     if (chat_manager) {

                                         std::string msg = globals::misc::chat_message;

                                         wchar_t wmsg[256];
                                         mbstowcs(wmsg, msg.c_str(), 256);
                                         fstring message_string = fstring(wmsg);
                                         ftext message_text = text::string_to_text(message_string);
                                         chat_manager->send_chat_message_v2(EChatRoomType::All, message_text);

                                         kill_message_sent[i] = true;
                                     }
                                 }


                                 if (current_health > 0) {
                                     kill_message_sent[i] = false;
                                 }

                                 last_health[i] = current_health;
                                 was_visible[i] = currently_visible;
                             }
                         }
                     }*/






                if (target_id != -1 && target_id < actors.count && globals::aimbot::a1mbot)
                {

                    ashootercharacter* actor = actors[target_id];
                    if (!actor || !memory::IsValidPointer((uintptr_t)actor) || actor == character) continue;



                    uskeletalmeshcomponent* mesh = actor->get_mesh();
                    if (!mesh) continue;

                    if (actor->is_alive()) {

                        fvector2d head_screen;
                        fvector target = get_target_bone_matrix(mesh, globals::aimbot::a1m_b0ne);
                        fvector spread_angle;
                        static const fkey lmb_key{ fname{ string::string_to_name(crypt(L"LeftMouseButton").decrypt()) } };

                        if (!target.is_valid()) continue;

                        if (globals::aimbot::prediction) {
                            fvector velocity = actor->get_velocity();
                            
                            // Auto-Resolver: The only delay factor for hitscan is the aimbot's own smoothing.
                            // We offset the target perfectly proportional to the smoothing delay (Avg 200 FPS step).
                            float travel_time = globals::aimbot::a1m_sm00th / 200.0f; 
                            
                            target.x += (velocity.x * travel_time);
                            target.y += (velocity.y * travel_time);
                            target.z += (velocity.z * travel_time);
                        }

                        bool aim_key_pressed = GetAsyncKeyState(globals::aimbot::a1m_k3y);
                        // Recompute visibility for the actual selected target (not stale from the loop)
                        bool target_bVisible = (globals::aimbot::v1sh_ch3ck && controllers->line_of_sight(actor)) || !globals::aimbot::v1sh_ch3ck;
                        bool target_CanAutoWall = false;
                        if (globals::aimbot::auto_wall && !target_bVisible)
                        {
                            target_CanAutoWall = TraceHelper::CanShootThrough(controllers, character, actor, globals::aimbot::a1m_b0ne);
                        }
                        bool visible_check = target_bVisible || target_CanAutoWall;
                        auto current_wep = character->get_inventory()->get_current_equippable();
                        fstring obj_name = helper::convert_weapon_name(system::get_object_name(current_wep));

                        bool is_valid_weapon =
                            obj_name.wide() == L"Bulldog" || obj_name.wide() == L"Phantom" || obj_name.wide() == L"Vandal" ||
                            obj_name.wide() == L"Operator" || obj_name.wide() == L"Marshal" || obj_name.wide() == L"Sheriff" ||
                            obj_name.wide() == L"Spectre" || obj_name.wide() == L"Outlaw" || obj_name.wide() == L"Classic" ||
                            obj_name.wide() == L"Shorty" || obj_name.wide() == L"Frenzy" || obj_name.wide() == L"Ghost" ||
                            obj_name.wide() == L"Stinger" || obj_name.wide() == L"Bucky" || obj_name.wide() == L"Judge" ||
                            obj_name.wide() == L"Guardian" || obj_name.wide() == L"Ares" || obj_name.wide() == L"Odin";

                        bool can_shoot = (!globals::aimbot::v1sh_ch3ck || visible_check);


                        if (aim_key_pressed && can_shoot && is_valid_weapon) {

                            fvector CameraPos = globals::misc::tperson ? character->get_mesh()->get_bone_location(8) : camera->get_camera_location();
                            fvector ControlRotation = controllers->get_control_rotation();
                            fvector vector_pos = target - CameraPos;
                            double distance = vector_pos.size();

                            if (distance <= 0) continue;

                            double normalized_z = vector_pos.z / distance;
                            normalized_z = max(-1.0, min(1.0, normalized_z));

                            double x = -(acos(normalized_z) * (180.0 / M_PI) - 90.0);
                            double y = atan2(vector_pos.y, vector_pos.x) * (180.0 / M_PI);

                            fvector target_rotation(x, y, 0.0);
                            fvector new_aim_rotation;

                            if (globals::aimbot::reco1l_contr0l) {
                                fvector recoil = camera->get_camera_rotation() - ControlRotation;
                                new_aim_rotation = target_rotation - recoil * 2.0;
                            }
                            else {
                                new_aim_rotation = target_rotation;
                            }

                            fvector new_rotation = smooth(new_aim_rotation, ControlRotation, globals::aimbot::a1m_sm00th);
                            new_rotation.x = fmod(new_rotation.x + 360.0, 360.0);
                            new_rotation.y = fmod(new_rotation.y + 360.0, 360.0);

                            if (globals::aimbot::nospread && character->is_alive()) {
                                auto current_inv = character->get_inventory();
                                if (current_inv) {
                                    auto current_equip = current_inv->get_current_equippable();
                                    auto firing_state = memory::read<uint64_t>(uintptr_t(current_equip) + offsets::FiringStateComp);
                                    spread_angle = calc_spread(character, (uintptr_t)firing_state, current_equip, new_rotation);
                                    if (!spread_angle.is_null()) {
                                        new_rotation = new_rotation - spread_angle;
                                    }
                                }
                            }

                            if (!new_rotation.is_valid()) continue;
                            controllers->set_control_rotation(new_rotation);
                        }


                        bool auto_shot_active = globals::aimbot::auto_shot;
                        if (globals::aimbot::auto_shot_hold_key) {
                            auto_shot_active = auto_shot_active && aim_key_pressed;
                        }

                        if (globals::aimbot::nospread && auto_shot_active && visible_check && is_valid_weapon) {
                            fvector CameraPos = fvector(0, 0, 0);
                            fvector firing_direction = fvector(0, 0, 0);

                            character->get_firing_location_and_direction(&CameraPos, &firing_direction, false);
                            fvector ControlRotation = controllers->get_control_rotation();
                            fvector vector_pos = target - CameraPos;
                            double distance = vector_pos.size();

                            if (distance <= 0) continue;

                            double normalized_z = vector_pos.z / distance;
                            if (normalized_z < -1.0) normalized_z = -1.0;
                            if (normalized_z > 1.0) normalized_z = 1.0;

                            double x = -(acos(normalized_z) * (180.0 / 3.14159265358979323846) - 90.0);
                            double y = atan2(vector_pos.y, vector_pos.x) * (180.0 / 3.14159265358979323846);

                            fvector target_rotation(x, y, 0.0);
                            fvector new_aim_rotation;

                            if (globals::aimbot::reco1l_contr0l) {
                                fvector recoil = camera->get_camera_rotation() - ControlRotation;
                                new_aim_rotation = target_rotation - recoil * 2.0;
                            }
                            else {
                                new_aim_rotation = target_rotation;
                            }

                            fvector new_rotation = smooth(new_aim_rotation, ControlRotation, globals::aimbot::a1m_sm00th);

                            new_rotation.x = fmod(new_rotation.x + 360.0, 360.0);
                            new_rotation.y = fmod(new_rotation.y + 360.0, 360.0);

                            if (globals::misc::tperson) {
                                CameraPos = character->get_mesh()->get_bone_location(8);
                            }
                            else {
                                CameraPos = camera->get_camera_location();
                            }

                            if (globals::aimbot::nospread && character->is_alive()) {
                                auto current_inv = character->get_inventory();
                                if (current_inv) {
                                    auto current_equip = current_inv->get_current_equippable();
                                    auto firing_state = memory::read<uint64_t>(uintptr_t(current_equip) + offsets::FiringStateComp);
                                    spread_angle = calc_spread(character, (uintptr_t)firing_state, current_equip, new_rotation);

                                    if (!spread_angle.is_null()) {
                                        new_rotation = new_rotation - spread_angle;
                                    }
                                }
                            }
                            controllers->set_control_rotation(new_rotation);

                            static DWORD spread_comp_ready_time = 0;
                            static DWORD shoot_delay_time = 0;
                            static bool delay_pending = false;
                            static bool spread_locked = false;

                            if (globals::aimbot::nospread && !spread_locked)
                            {
                                spread_comp_ready_time = GetTickCount() + 200;
                                spread_locked = true;
                            }

                            if (spread_locked && GetTickCount() >= spread_comp_ready_time)
                            {
                                if (visible_check && !delay_pending)
                                {
                                    globals::stop_for_auto_shoot = true;
                                    shoot_delay_time = GetTickCount() + 20;
                                    delay_pending = true;
                                }

                                if (delay_pending && globals::aimbot::auto_shot && GetTickCount() >= shoot_delay_time)
                                {
                                    controllers->SimulateInputKey(lmb_key, true);
                                    controllers->SimulateInputKey(lmb_key, false);

                                    globals::stop_for_auto_shoot = false;
                                    delay_pending = false;
                                }
                            }

                            if (!globals::aimbot::nospread)
                            {
                                spread_locked = false;
                                delay_pending = false;
                                globals::stop_for_auto_shoot = false;
                                shoot_delay_time = 0;
                                spread_comp_ready_time = 0;
                            }
                        }
                    }
                }
            }




        } while (false);








        return draw_transition_o(viewportclient, canvas_, a3);

    }












    uworld* world;



    void init_hooks()
    {

      /*  AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);

        SetConsoleTitleA("VALORANT Debug Console");
        printf("=== VALORANT DEBUG CONSOLE STARTED ===\n");*/

        memory::module_base = memory::get_module(crypt(L"VALORANT-Win64-Shipping.exe"));
        if (!memory::module_base) {
            return;
        }


        (initialize_spoofcall)((uint8_t*)memory::module_base);


        variables.init_variables();


        Config->Initialize();


        uworld* UWorldClass = nullptr;
        uintptr_t* uworld_state_ptr = *(uintptr_t**)(memory::module_base + offsets::State);
        if (uworld_state_ptr) {
            UWorldClass = *(uworld**)uworld_state_ptr;
        }

        if (!UWorldClass) {
            return;
        }

        ugameinstance* gameinstance = memory::read<ugameinstance*>(uintptr_t(UWorldClass) + offsets::game_instance);
        if (!gameinstance) {
            return;
        }

        ulocalplayer* localplayer = gameinstance->local_players()[0];
        if (!localplayer) {
            return;
        }


        ugameviewportclient* viewportclient = localplayer->viewport_client();
        if (!viewportclient) {
            return;
        }


        aplayercontroller* LocalController = memory::read<aplayercontroller*>((uintptr_t)localplayer + 0x38);
        if (!LocalController) {
            return;
        }


        aplayercontroller* PlayerCameraManager = memory::read<aplayercontroller*>((uintptr_t)LocalController + offsets::cameramaneger);
        if (!PlayerCameraManager) {
            return;
        }

        uintptr_t Engine = memory::read<uintptr_t>((uintptr_t)gameinstance + 0x28);
        if (!Engine) {
            return;
        }


        menu::font = memory::read<uobject*>((uintptr_t)Engine + 0x98);
        if (!menu::font) {
            return;
        }


        LocalCameraLocation = memory::read<uintptr_t>(uintptr_t(PlayerCameraManager) + offsets::camerapos);
        LocalCameraFOV = memory::read<float>(uintptr_t(PlayerCameraManager) + offsets::camerafov);
        LocalCameraRotation = memory::read<uintptr_t>(uintptr_t(PlayerCameraManager) + offsets::camerarot);

        keys::space = string::string_to_name(crypt(L"SpaceBar").decrypt());
        keys::left_mouse = string::string_to_name(crypt(L"LeftMouseButton").decrypt());


        static shadow_vmt viewport_hook;

        bool hook_success = viewport_hook.hook<decltype(hooks::draw_transition_o)>(
            memory::module_base,
            (uintptr_t)viewportclient,
            0x63,
            (void*)hooks::hk_draw_transition,
            &hooks::draw_transition_o
        );

        if (!hook_success) {

            return;
        }


    }
}







