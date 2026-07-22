#pragma once
#include <Windows.h>



wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

namespace menu
{
	uobject* font;

	flinearcolor RGBtoFLC(float r, float g, float b, float a = 1.0f)
	{
		float gamma = 1.8f;

		return {
			powf(r / 255.0f, gamma),
			powf(g / 255.0f, gamma),
			powf(b / 255.0f, gamma),
			a
		};
	}

	flinearcolor RGBtoFLC2(float r, float g, float b, float a = 1.0f)
	{
		return { r / 255, g / 255, b / 255, a };
	}

	void cooltext(ucanvas* canvas, const wchar_t* text, fvector2d pos) {
		canvas->k2_drawtext(menu::font, text, fvector2d(pos.x, pos.y), fvector2d(0.99, 0.96),
			flinearcolor{ 1.0f, 1.0f, 1.0f, 1.0f }, 0.0f, RGBtoFLC(0, 0, 0),
			fvector2d(0, 0), false, true, false, RGBtoFLC(0, 0, 0));
	}

	void draw_textyyy(ucanvas* canvas, uobject* font, const wchar_t* text, flinearcolor color, fvector2d pos, bool centered = false) {
		if (!canvas || !font || !text) return;

		canvas->k2_drawtext(
			font,
			text,
			pos,
			{ 1.00f, 1.00f },
			color,
			0.f,
			{ 0, 0, 0, 0.30f },
			{ 0, 0 },
			true,
			true,
			true,
			{ 0, 0, 0, 0.45f }
		);
	}

	fvector2d CalcTextSize(const wchar_t* text) {
		HDC hdc = GetDC(nullptr); 
		if (!hdc)
			return { 0, 0 };

		SIZE size;
		GetTextExtentPoint32W(hdc, text, (int)wcslen(text), &size);
		ReleaseDC(nullptr, hdc);

		return fvector2d{ static_cast<float>(size.cx), static_cast<float>(size.cy) };
	}

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

	flinearcolor HSVtoRGB(float h, float s, float v) {
		float r, g, b;

		int i = static_cast<int>(h * 6);
		float f = h * 6 - i;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1 - (1 - f) * s);

		switch (i % 6) {
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
		default: r = g = b = 0.0f;
		}

		return flinearcolor{ r, g, b, 1.0f };

	}

	// x9 ware
	/*namespace Colors
	{
		flinearcolor Text{ 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 255.0f / 255.0f }; //
		flinearcolor Text_Shadow{ 0.0f, 0.0f, 0.0f, 1.0f };
		flinearcolor Text_Outline{ 0.0f, 0.0f, 0.0f, 1.0f };
		flinearcolor Text_Active = RGBtoFLC(184, 51, 255);  // Updated purple

		flinearcolor Button_Idle{ 10.0 / 255.0f, 10.0 / 255.0f, 10.0 / 255.0f, 0.8f }; //
		flinearcolor Button_Hovered = RGBtoFLC(7, 7, 7);
		flinearcolor Button_Active = RGBtoFLC(184, 51, 255);  // Updated purple

		flinearcolor Checkbox_Idle = RGBtoFLC(15, 15, 15);
		flinearcolor Checkbox_Hovered = RGBtoFLC(20, 20, 20);
		flinearcolor Checkbox_Enabled = RGBtoFLC(184, 51, 255); // Updated purple

		flinearcolor Combobox_Idle{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		flinearcolor Combobox_Hovered{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		flinearcolor Combobox_Elements{ 0.239f, 0.42f, 0.0f, 0.5f };

		flinearcolor Slider_Idle = RGBtoFLC(8, 8, 8);
		flinearcolor Slider_Hovered = RGBtoFLC(14, 14, 14);
		flinearcolor Slider_Progress = RGBtoFLC(184, 51, 255);  // Updated purple
		flinearcolor Slider_Button = RGBtoFLC(184, 51, 255);  // Updated purple

		flinearcolor ColorPicker_Background{ 0.006f, 0.006f, 0.006f, 0.4f };
	}*/

	namespace Accent
	{
		flinearcolor Title = RGBtoFLC(200, 200, 40);

		flinearcolor AccentBar1 = flinearcolor(220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f);

		flinearcolor ToggleOn = RGBtoFLC(200, 200, 40);
		flinearcolor ToggleOff = RGBtoFLC(25, 25, 25);
	}

	namespace Colors
	{
		flinearcolor Text{ 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 255.0f / 255.0f }; //
		flinearcolor Text_Shadow{ 0.0f, 0.0f, 0.0f, 1.0f };
		flinearcolor Text_Outline{ 0.0f, 0.0f, 0.0f, 1.0f };
		flinearcolor Text_Active{ 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 0.8f };

		flinearcolor Button_Idle{ 10.0 / 255.0f, 10.0 / 255.0f, 10.0 / 255.0f, 0.8f / 255.0f }; //
		flinearcolor Button_Hovered{ 15.0f / 255.0f, 15.0f / 255.0f, 15.0f / 255.0f, 1.f };
		flinearcolor Button_Active{ 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 0.8f };

		flinearcolor Checkbox_Idle = RGBtoFLC(15, 15, 15);
		flinearcolor Checkbox_Hovered = RGBtoFLC(20, 20, 20);
		flinearcolor Checkbox_Enabled = RGBtoFLC(128, 128, 128); //

		flinearcolor Combobox_Idle{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		flinearcolor Combobox_Hovered{ 200.0f / 255.0f, 150.0f / 255.0f, 40.0f / 255.0f };
		flinearcolor Combobox_Elements{ 0.239f, 0.42f, 0.0f, 0.5f };

		flinearcolor Slider_Idle = RGBtoFLC(8, 8, 8);
		flinearcolor Slider_Hovered = RGBtoFLC(14, 14, 14);
		flinearcolor Slider_Progress = RGBtoFLC(128, 128, 128);
		flinearcolor Slider_Button = RGBtoFLC(128, 128, 128);

		flinearcolor ColorPicker_Background{ 0.006f, 0.006f, 0.006f, 0.4f };
	}

	namespace PostRenderer
	{
		struct DrawList
		{
			int type = -1; //1 = FilledRect, 2 = TextLeft, 3 = TextCenter, 4 = Draw_Line, 5 = Text, 6 = FilledRectEx
			fvector2d pos;
			fvector2d size;
			flinearcolor color;
			const wchar_t* name;
			bool outline;

			fvector2d from;
			fvector2d to;
			int thickness;

			// For new Text function
			fvector2d scale;
			flinearcolor outline_color;
			fvector2d shadow_offset;
			bool center;

			// For new FilledRect function
			float width;
			float height;
		};
		DrawList drawlist[128];

		void drawFilledRect(fvector2d pos, float w, float h, flinearcolor color) // OLD
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 1;
					drawlist[i].pos = pos;
					drawlist[i].size = fvector2d{ w, h };
					drawlist[i].color = color;
					return;
				}
			}
		}

		void FilledRect(fvector2d pos, float w, float h, flinearcolor color) // NEW
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 6;
					drawlist[i].pos = pos;
					drawlist[i].width = w;
					drawlist[i].height = h;
					drawlist[i].color = color;
					return;
				}
			}
		}

		void TextLeft(const wchar_t* name, fvector2d pos, flinearcolor color, bool outline) // OLD
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

		void TextCenter(const wchar_t* name, fvector2d pos, flinearcolor color, bool outline) // OLD
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

		void Draw_Line(fvector2d from, fvector2d to, int thickness, flinearcolor color) // OLD
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

		void Text(const wchar_t* text, fvector2d pos, fvector2d scale, flinearcolor color, // NEW
			float angle, flinearcolor shadow_color, fvector2d shadow_offset,
			bool center, bool outline, flinearcolor outline_color)
		{
			for (int i = 0; i < 128; i++)
			{
				if (drawlist[i].type == -1)
				{
					drawlist[i].type = 5;
					drawlist[i].name = text;
					drawlist[i].pos = pos;
					drawlist[i].scale = scale;
					drawlist[i].color = color;
					drawlist[i].outline_color = outline_color;
					drawlist[i].shadow_offset = shadow_offset;
					drawlist[i].center = center;
					drawlist[i].outline = outline;
					return;
				}
			}
		}
	}

	ucanvas* canvas;


	bool hover_element = false;
	fvector2d menu_pos = fvector2d{ 0, 0 };
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	fvector2d first_element_pos = fvector2d{ 0, 0 };

	fvector2d last_element_pos = fvector2d{ 0, 0 };
	fvector2d last_element_size = fvector2d{ 0, 0 };

	int current_element = -1;
	fvector2d current_element_pos = fvector2d{ 0, 0 };
	fvector2d current_element_size = fvector2d{ 0, 0 };
	int elements_count = 0;

	bool sameLine = false;

	bool pushY = false;
	float pushYvalue = 0.0f;

	void SetupCanvas(ucanvas* _canvas)
	{
		canvas = _canvas;
	}

	void Draw_Line(fvector2d from, fvector2d to, int thickness, flinearcolor color)
	{
		canvas->k2_drawline(fvector2d{ from.x, from.y }, fvector2d{ to.x, to.y }, thickness, color);
	}
	void drawFilledRect(fvector2d initial_pos, float w, float h, flinearcolor color)
	{
		for (float i = 0.0f; i < h; i += 1.0f)
			canvas->k2_drawline(fvector2d{ initial_pos.x, initial_pos.y + i }, fvector2d{ initial_pos.x + w, initial_pos.y + i }, 1.0f, color);
	}
	void DrawGradientLine(fvector2d screenpos_a, fvector2d screenpos_b, flinearcolor color_a, flinearcolor color_c, flinearcolor color_b, float thickness, int num_segments)
	{
		for (int i = 0; i < num_segments; i++)
		{
			float t1 = static_cast<float>(i) / num_segments;
			float t2 = static_cast<float>(i + 1) / num_segments;

			fvector2d start = screenpos_a + (screenpos_b - screenpos_a) * t1;
			fvector2d end = screenpos_a + (screenpos_b - screenpos_a) * t2;

			float t_mid = (t1 + t2) / 2.0f;

			flinearcolor color;

			if (t_mid <= 0.5f)
			{
				float u = 2.0f * t_mid;  // Maps t_mid [0, 0.5] to u [0, 1]
				color.r = color_a.r * (1.0f - u) + color_c.r * u;
				color.g = color_a.g * (1.0f - u) + color_c.g * u;
				color.b = color_a.b * (1.0f - u) + color_c.b * u;
				color.a = color_a.a * (1.0f - u) + color_c.a * u;
			}
			else
			{
				float v = 2.0f * (t_mid - 0.5f);  // Maps t_mid [0.5, 1] to v [0, 1]
				color.r = color_c.r * (1.0f - v) + color_b.r * v;
				color.g = color_c.g * (1.0f - v) + color_b.g * v;
				color.b = color_c.b * (1.0f - v) + color_b.b * v;
				color.a = color_c.a * (1.0f - v) + color_b.a * v;
			}

			canvas->k2_drawline(start, end, thickness, color);
		}
	}

	// DrawGradientLine(startPos, endPos, red, green, blue, 2.0f, 20);
	void drawGradientFilledRect(fvector2d initial_pos, float w, float h, flinearcolor color_a, flinearcolor color_c, flinearcolor color_b, int num_segments)
	{
		for (float i = 0.0f; i < h; i += 1.0f)
			DrawGradientLine(fvector2d{ initial_pos.x, initial_pos.y + i }, fvector2d{ initial_pos.x + w, initial_pos.y + i }, color_a, color_c, color_b, 1.0f, num_segments);
	}

	void drawGradientFilledRectVertical(fvector2d initial_pos, float w, float h, flinearcolor color_a, flinearcolor color_c, flinearcolor color_b, int num_segments)
	{
		for (float i = 0.0f; i < w; i += 1.0f)
			DrawGradientLine(fvector2d{ initial_pos.x + i, initial_pos.y }, fvector2d{ initial_pos.x + i, initial_pos.y + h }, color_a, color_c, color_b, 1.0f, num_segments);
	}


	void draw_filled_rect(ucanvas* canvas, float x, float y, float width, float height, flinearcolor color) {
		for (float i = 0; i < height; i++) {
			canvas->k2_drawline(
				{ x, y + i },
				{ x + width, y + i },
				1.0f,
				color
			);
		}
	}

	void draw_rect(ucanvas* canvas, float x, float y, float width, float height, flinearcolor color) {
		canvas->k2_drawline({ x, y }, { x + width, y }, 1.0f, color);
		canvas->k2_drawline({ x + width, y }, { x + width, y + height }, 1.0f, color);
		canvas->k2_drawline({ x + width, y + height }, { x, y + height }, 1.0f, color);
		canvas->k2_drawline({ x, y + height }, { x, y }, 1.0f, color);
	}

	fvector2d CursorPos()
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		return fvector2d{ (float)cursorPos.x, (float)cursorPos.y };
	}
	bool MouseInZone(fvector2d pos, fvector2d size)
	{
		fvector2d cursor_pos = CursorPos();

		if (cursor_pos.x > pos.x && cursor_pos.y > pos.y)
			if (cursor_pos.x < pos.x + size.x && cursor_pos.y < pos.y + size.y)
				return true;

		return false;
	}

	void drawTriangle(fvector2d p1, fvector2d p2, fvector2d p3, flinearcolor color, int thickness = 1)
	{
		Draw_Line(p1, p2, thickness, color);
		Draw_Line(p2, p3, thickness, color);
		Draw_Line(p3, p1, thickness, color);
	}
	void drawCircle(fvector2d center, float radius, flinearcolor color, int thickness = 1, int segments = 64)
	{
		float angleStep = (2.0f * 3.14159265f) / segments;

		for (int i = 0; i < segments; ++i)
		{
			float angle1 = i * angleStep;
			float angle2 = (i + 1) * angleStep;

			fvector2d p1 = fvector2d(center.x + cosf(angle1) * radius, center.y + sinf(angle1) * radius);
			fvector2d p2 = fvector2d(center.x + cosf(angle2) * radius, center.y + sinf(angle2) * radius);

			Draw_Line(p1, p2, thickness, color);
		}
	}

	//void DrawFilledTriangle(const fvector2d& p1, const fvector2d& p2, const fvector2d& p3, const flinearcolor& color)
	//{
	//    std::vector<fvector2d> verts = { p1, p2, p3 };
	//    std::sort(verts.begin(), verts.end(), [](const fvector2d& a, const fvector2d& b) {
	//        return a.y < b.y;
	//        });

	//    const fvector2d& v0 = verts[0];
	//    const fvector2d& v1 = verts[1];
	//    const fvector2d& v2 = verts[2];

	//    auto interp = [](float a, float b, float t) {
	//        return a + (b - a) * t;
	//        };

	//    float dy1 = v1.y - v0.y;
	//    float dy2 = v2.y - v0.y;
	//    float dy3 = v2.y - v1.y;

	//    for (int y = static_cast<int>(v0.y); y <= static_cast<int>(v2.y); y++) {
	//        if (y < v1.y && dy1 != 0) {
	//            float t1 = (y - v0.y) / dy1;
	//            float t2 = (y - v0.y) / dy2;

	//            float xa = interp(v0.x, v1.x, t1);
	//            float xb = interp(v0.x, v2.x, t2);
	//            if (xa > xb) std::swap(xa, xb);
	//            Draw_Line(fvector2d(xa, y), fvector2d(xb, y), 1.0f, color);
	//        }
	//        else if (dy3 != 0) {
	//            float t1 = (y - v1.y) / dy3;
	//            float t2 = (y - v0.y) / dy2;

	//            float xa = interp(v1.x, v2.x, t1);
	//            float xb = interp(v0.x, v2.x, t2);
	//            if (xa > xb) std::swap(xa, xb);
	//            Draw_Line(fvector2d(xa, y), fvector2d(xb, y), 1.0f, color);
	//        }
	//    }
	//}

	//bool Draw_Cursor(bool draw)
	//{
	//    if (!draw)
	//        return false;

	//    fvector2d mousePos = CursorPos();

	//    // Fixed size
	//    float size = 8.0f;
	//    float height = size * 2.0f;

	//    fvector2d tip = mousePos;
	//    fvector2d left = fvector2d(mousePos.x - size, mousePos.y + height);
	//    fvector2d right = fvector2d(mousePos.x + size, mousePos.y + height);

	//    // Time for fluid animation
	//    float t = static_cast<float>(GetTickCount64()) / 1000.0f;

	//    // Animated triangle fill with per-line brightness shift (fake fluid)
	//    std::vector<fvector2d> verts = { tip, left, right };
	//    std::sort(verts.begin(), verts.end(), [](const fvector2d& a, const fvector2d& b) {
	//        return a.y < b.y;
	//        });

	//    const fvector2d& v0 = verts[0];
	//    const fvector2d& v1 = verts[1];
	//    const fvector2d& v2 = verts[2];

	//    auto interp = [](float a, float b, float t) {
	//        return a + (b - a) * t;
	//        };

	//    float dy1 = v1.y - v0.y;
	//    float dy2 = v2.y - v0.y;
	//    float dy3 = v2.y - v1.y;

	//    for (int y = static_cast<int>(v0.y); y <= static_cast<int>(v2.y); y++) {
	//        float pulse = 0.5f + 0.5f * sinf(t * 2.0f); // slow smooth global pulse
	//        flinearcolor fluidColor = Accent::accent_color_new;
	//        fluidColor.a = pulse;

	//        if (y < v1.y && dy1 != 0) {
	//            float t1 = (y - v0.y) / dy1;
	//            float t2 = (y - v0.y) / dy2;

	//            float xa = interp(v0.x, v1.x, t1);
	//            float xb = interp(v0.x, v2.x, t2);
	//            if (xa > xb) std::swap(xa, xb);
	//            Draw_Line(fvector2d(xa, y), fvector2d(xb, y), 1.0f, fluidColor);
	//        }
	//        else if (dy3 != 0) {
	//            float t1 = (y - v1.y) / dy3;
	//            float t2 = (y - v0.y) / dy2;

	//            float xa = interp(v1.x, v2.x, t1);
	//            float xb = interp(v0.x, v2.x, t2);
	//            if (xa > xb) std::swap(xa, xb);
	//            Draw_Line(fvector2d(xa, y), fvector2d(xb, y), 1.0f, fluidColor);
	//        }
	//    }

	//    return true;
	//}

	void SameLine()
	{
		sameLine = true;
	}
	void PushNextElementY(float y, bool from_last_element = true)
	{
		pushY = true;
		if (from_last_element)
			pushYvalue = last_element_pos.y + last_element_size.y + y;
		else
			pushYvalue = y;
	}
	void NextColumn(float x)
	{
		offset_x = x;
		PushNextElementY(first_element_pos.y, false);
	}
	void ClearFirstPos()
	{
		first_element_pos = fvector2d{ 0, 0 };
	}

	void xDrawTextRGB(const wchar_t* text, float x, float y, flinearcolor color)
	{
		canvas->k2_drawtext(font, text, { x, y }, { 1.1, 1.1 }, { 1.f,1.f,1.f,1.0f }, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, 0, 0, 0, { 0, 0, 0, 1 });
	}
	void yDrawTextRGB(const wchar_t* text, float x, float y, flinearcolor color)
	{
		canvas->k2_drawtext(font, text, { x, y }, { 1.1, 1.1 }, { 1.f,1.f,1.f,1.0f }, 0.f, { 0, 0, 0, 1 }, { 0, 0 }, 1, 0, 0, { 0, 0, 0, 1 });
	}
	void TextLeft(const wchar_t* name, fvector2d pos, flinearcolor color, bool outline)
	{
		xDrawTextRGB(name, pos.x, pos.y, { 1,1,1,1 });
	}
	void TextCenter(const wchar_t* name, fvector2d pos, flinearcolor color, bool outline, bool kekw = 0)
	{
		if (kekw)
			yDrawTextRGB(name, pos.x, pos.y - 10, color);
		else
			yDrawTextRGB(name, pos.x, pos.y - 10, { 1.f,1.f,1.f,1.0f });
	}

	void GetColor(flinearcolor* color, float* r, float* g, float* b, float* a)
	{
		*r = color->r;
		*g = color->g;
		*b = color->b;
		*a = color->a;
	}
	UINT32 GetColorUINT(int r, int g, int b, int a)
	{
		UINT32 result = (BYTE(a) << 24) + (BYTE(r) << 16) + (BYTE(g) << 8) + BYTE(b);
		return result;
	}
	void DrawFilledCircle(fvector2d pos, float r, flinearcolor color)
	{
		float smooth = 0.07f;

		double PI = 3.14159265359;
		int size = (int)(2.0f * PI / smooth) + 1;

		float angle = 0.0f;
		int i = 0;

		for (; angle < 2 * PI; angle += smooth, i++)
		{
			Draw_Line(fvector2d{ pos.x, pos.y }, fvector2d{ pos.x + cosf(angle) * r, pos.y + sinf(angle) * r }, 1.0f, color);
		}
	}
	void DrawCircle(fvector2d pos, int radius, int numSides, flinearcolor Color)
	{
		float PI = 3.1415927f;

		float Step = PI * 2.0 / numSides;
		int Count = 0;
		fvector2d V[128];
		for (float a = 0; a < PI * 2.0; a += Step) {
			float X1 = radius * cos(a) + pos.x;
			float Y1 = radius * sin(a) + pos.y;
			float X2 = radius * cos(a + Step) + pos.x;
			float Y2 = radius * sin(a + Step) + pos.y;
			V[Count].x = X1;
			V[Count].y = Y1;
			V[Count + 1].x = X2;
			V[Count + 1].y = Y2;
			//Draw_Line(fvector2d{ pos.x, pos.y }, fvector2d{ X2, Y2 }, 1.0f, Color); // Points from Centre to ends of circle
			Draw_Line(fvector2d{ V[Count].x, V[Count].y }, fvector2d{ X2, Y2 }, 1.0f, Color);// Circle Around
		}
	}

	fvector2d dragPos;

	float GetSmoothTime() {
		return static_cast<float>(GetTickCount64()) / 1000.0f;
	}

	flinearcolor ApplyHueShift(flinearcolor color, float shift)
	{
		// Convert RGB to HSV
		float r = color.r;
		float g = color.g;
		float b = color.b;

		float max = max(r, max(g, b));
		float min = min(r, min(g, b));
		float delta = max - min;

		float h = 0.0f;
		float s = (max > 0.0001f) ? delta / max : 0.0f;
		float v = max;

		if (delta > 0.0001f) {
			if (max == r) {
				h = (g - b) / delta;
			}
			else if (max == g) {
				h = 2.0f + (b - r) / delta;
			}
			else {
				h = 4.0f + (r - g) / delta;
			}
			h *= 60.0f;
			if (h < 0.0f) h += 360.0f;
		}

		// Apply hue shift
		h += shift * 180.0f / 3.14159265f; // Convert radians to degrees
		while (h >= 360.0f) h -= 360.0f;
		while (h < 0.0f) h += 360.0f;

		// Convert HSV back to RGB
		float c = v * s;
		float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
		float m = v - c;

		flinearcolor result;
		if (h < 60.0f) {
			result = { c, x, 0.0f };
		}
		else if (h < 120.0f) {
			result = { x, c, 0.0f };
		}
		else if (h < 180.0f) {
			result = { 0.0f, c, x };
		}
		else if (h < 240.0f) {
			result = { 0.0f, x, c };
		}
		else if (h < 300.0f) {
			result = { x, 0.0f, c };
		}
		else {
			result = { c, 0.0f, x };
		}

		result.r += m;
		result.g += m;
		result.b += m;
		result.a = color.a; // Preserve original alpha

		return result;
	}

	inline float smoothstep(float edge0, float edge1, float x)
	{
		// Clamp x to range [0, 1] relative to [edge0, edge1]
		float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		// Cubic Hermite interpolation
		return t * t * (3.0f - 2.0f * t);
	}

	int current_tab;

	// v2

	bool Window(const wchar_t* name, fvector2d* pos, fvector2d size, bool isOpen)
	{
		elements_count = 0;

		if (!isOpen)
			return false;

		//// Get screen boundaries
		//static float screen_width = 1920.0f;
		//static float screen_height = 1080.0f;

		float screen_width = (float)GetSystemMetrics(SM_CXSCREEN);
		float screen_height = (float)GetSystemMetrics(SM_CYSCREEN);

		bool isHovered = MouseInZone(fvector2d{ pos->x, pos->y }, fvector2d{ size.x, 23.0f });

		// Drop last element
		if (current_element != -1 && !GetAsyncKeyState(0x1))
		{
			current_element = -1;
		}

		// Drag
		if (hover_element && GetAsyncKeyState(0x1))
		{
			// Handle element dragging logic
		}
		else if ((isHovered || dragPos.x != 0) && !hover_element)
		{
			if (input::is_mouse_clicked(0, elements_count, true))
			{
				fvector2d cursorPos = CursorPos();

				cursorPos.x -= size.x;
				cursorPos.y -= size.y;

				if (dragPos.x == 0)
				{
					dragPos.x = (cursorPos.x - pos->x);
					dragPos.y = (cursorPos.y - pos->y);
				}

				// Calculate new position
				float new_x = cursorPos.x - dragPos.x;
				float new_y = cursorPos.y - dragPos.y;

				// Keep window inside screen boundaries
				if (new_x < 0) new_x = 0;
				if (new_y < 0) new_y = 0;
				if (new_x + size.x > screen_width) new_x = screen_width - size.x;
				if (new_y + size.y > screen_height) new_y = screen_height - size.y;

				pos->x = new_x;
				pos->y = new_y;
			}
			else
			{
				dragPos = fvector2d{ 0, 0 };
			}
		}
		else
		{
			hover_element = false;
		}

		offset_x = 8.0f; offset_y = 2.0f; // Changed to 23 for header height
		menu_pos = fvector2d{ pos->x, pos->y };
		first_element_pos = fvector2d{ 0, 0 };
		current_element_pos = fvector2d{ 0, 0 };
		current_element_size = fvector2d{ 0, 0 };

		// Main outer black border (490x420)
		drawFilledRect(fvector2d(pos->x, pos->y), size.x, size.y, RGBtoFLC(0.0f, 0.0f, 0.0f, 1.0f));

		// Inner dark gray border (488x418) - 1px offset
		drawFilledRect(fvector2d(pos->x + 1, pos->y + 1), size.x - 2, size.y - 2, RGBtoFLC(35.0f, 35.0f, 35.0f));

		// First Header Section
		// First header background (486x23) - 2px offset from top
		drawFilledRect(fvector2d(pos->x + 2, pos->y + 2), size.x - 4, 23, RGBtoFLC(27.0f, 27.0f, 27.0f));

		// First header accent line (486x1) - below first header
		drawFilledRect(fvector2d(pos->x + 2, pos->y + 25), size.x - 4, 1, RGBtoFLC(249.0f, 46.0f, 254.0f));

		// Second Header Section
		// Second header background (486x23) - starts after first header + accent line
		drawFilledRect(fvector2d(pos->x + 2, pos->y + 26), size.x - 4, 23, RGBtoFLC(27.0f, 27.0f, 27.0f));

		// Second header accent line (486x1) - below second header
		drawFilledRect(fvector2d(pos->x + 2, pos->y + 49), size.x - 4, 1, RGBtoFLC(249.0f, 46.0f, 254.0f));

		// Main content area background (486x392) - starts after both headers
		drawFilledRect(fvector2d(pos->x + 2, pos->y + 50), size.x - 4, size.y - 52 - 23, RGBtoFLC(18.0f, 18.0f, 18.0f));

		// Bottom Header Section
		drawFilledRect(fvector2d(pos->x + 2, pos->y + size.y - (23 + 2)), size.x - 4, 23, RGBtoFLC(27.0f, 27.0f, 27.0f));

		drawFilledRect(fvector2d(pos->x + 2, pos->y + size.y - (23 + 2)), size.x - 4, 1, RGBtoFLC(249.0f, 46.0f, 254.0f));

		canvas->k2_drawtext(font, name, fvector2d(pos->x + 8, pos->y + size.y - ((23 / 2) + 2)), fvector2d(0.95f, 0.85f), RGBtoFLC(255, 255, 255, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), false, true, false, RGBtoFLC(0, 0, 0));

		canvas->k2_drawtext(font, L"Welcome Sheyko, UID 67", fvector2d(pos->x + (size.x - 8) - 79, pos->y + ((23 / 2) + 2)), fvector2d(0.95f, 0.85f), RGBtoFLC(255, 255, 255, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, false, RGBtoFLC(0, 0, 0));

		canvas->k2_drawtext(font, L"Private build / 0 days left", fvector2d(pos->x + (size.x - 8) - 80, pos->y + size.y - ((23 / 2) + 2)), fvector2d(0.95f, 0.85f), RGBtoFLC(255, 255, 255, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, false, RGBtoFLC(0, 0, 0));

		return true;
	}

	bool ButtonTab2(const wchar_t* name, fvector2d size, bool active)
	{
		elements_count++;

		fvector2d padding = fvector2d{ 0, 0 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x; // No gap
			pos.y = last_element_pos.y;
		}

		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		bool isHovered = MouseInZone(fvector2d{ pos.x, pos.y }, size);

		if (!sameLine)
			offset_y += size.y + padding.y;

		fvector2d textPos = fvector2d{ pos.x + size.x / 2, pos.y + size.y / 2 };

		// Text rendering (unchanged)
		if (active)
		{
			canvas->k2_drawtext(font, name, textPos, fvector2d(0.95f, 0.85f), RGBtoFLC(255, 255, 255, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, false, RGBtoFLC(0, 0, 0));
		}
		else if (isHovered) {
			canvas->k2_drawtext(font, name, textPos, fvector2d(0.95f, 0.85f), RGBtoFLC(135, 135, 135), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, false, RGBtoFLC(0, 0, 0));
		}
		else
		{
			canvas->k2_drawtext(font, name, textPos, fvector2d(0.95f, 0.85f), RGBtoFLC(100, 100, 100), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, false, RGBtoFLC(0, 0, 0));
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;

		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			return true;

		return false;
	}

	void SectionWrapper(const wchar_t* name, fvector2d size)
	{
		fvector2d padding = fvector2d{ 0, 0 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine)
		{
			pos.x = menu_pos.x + padding.x + offset_x;
			pos.y = menu_pos.y + padding.y + offset_y;
		}

		// Outer rectangle (228x80) - #0E0E0E
		drawFilledRect(fvector2d{ pos.x, pos.y }, size.x, size.y, RGBtoFLC(14.0f, 14.0f, 14.0f));

		// Middle rectangle (226x78) - #232323
		drawFilledRect(fvector2d{ pos.x + 1, pos.y + 1 }, size.x - 2, size.y - 2, RGBtoFLC(30.0f, 30.0f, 30.0f));

		// Inner rectangle (224x76) - #1B1B1B
		drawFilledRect(fvector2d{ pos.x + 2, pos.y + 2 }, size.x - 4, size.y - 4, RGBtoFLC(24.0f, 24.0f, 24.0f));

		canvas->k2_drawtext(font, name, fvector2d(pos.x + 6, pos.y + 2), fvector2d(0.95f, 0.85f), RGBtoFLC(255, 255, 255, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), false, false, false, RGBtoFLC(0, 0, 0));

		offset_y += 25;
		sameLine = false;
	}

	void Checkbox(const wchar_t* name, bool* value, bool risky = false)
	{
		elements_count++;

		fvector2d padding = fvector2d{ 13, 11 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y;
		}

		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		bool isHovered = MouseInZone(fvector2d{ pos.x, pos.y }, fvector2d{ 9, 9 });

		// Text positioned 11px away from box
		fvector2d textPos = fvector2d{ pos.x + 9 + 11, pos.y - 4.5f };

		// Outer rectangle (9x9) - #292929
		drawFilledRect(fvector2d{ pos.x, pos.y }, 9.0f, 9.0f, RGBtoFLC(41.0f, 41.0f, 41.0f));

		// Inner rectangle (7x7) - #151515 - 1px offset
		drawFilledRect(fvector2d{ pos.x + 1, pos.y + 1 }, 7.0f, 7.0f, RGBtoFLC(21.0f, 21.0f, 21.0f));

		if (risky) {
			canvas->k2_drawtext(font, name, textPos, fvector2d(0.90f, 0.85f), RGBtoFLC(170, 41, 41, 1.2f), 0.0f,
				RGBtoFLC(0, 0, 0), fvector2d(0, 0), false, false, false, Colors::Text_Outline);
		}
		else {
			canvas->k2_drawtext(font, name, textPos, fvector2d(0.90f, 0.85f), RGBtoFLC(255, 255, 255, 1.2f), 0.0f,
				RGBtoFLC(0, 0, 0), fvector2d(0, 0), false, false, false, Colors::Text_Outline);
		}

		if (!sameLine)
			offset_y += 9 + padding.y; // Changed to 9

		if (*value)
		{
			if (risky) {
				drawFilledRect(fvector2d{ pos.x + 1, pos.y + 1 }, 9.0f - 2, 9.0f - 2, RGBtoFLC(170, 41, 41));
			}
			else {
				drawFilledRect(fvector2d{ pos.x + 1, pos.y + 1 }, 9.0f - 2, 9.0f - 2, RGBtoFLC(249.0f, 46.0f, 254.0f));
			}
		}
		else
		{
			drawFilledRect(fvector2d{ pos.x + 2, pos.y + 2 }, 5.0f, 5.0f, RGBtoFLC(21, 21, 21));
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = fvector2d{ 9, 9 }; // Changed to 9
		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;

		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			*value = !*value;
	}

	bool Button(const wchar_t* name)
	{
		elements_count++;

		fvector2d padding = fvector2d{ 0, 14 };
		float section_start_x = offset_x;
		float section_width = 228.0f;
		float button_width = 176.0f;
		float button_height = 20.0f;
		float button_width_half = button_width / 2.0f;

		fvector2d pos = fvector2d{
			menu_pos.x + section_start_x + (section_width / 2.0f) - button_width_half,
			menu_pos.y + padding.y + offset_y
		};

		if (sameLine) {
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y;
		}
		if (pushY) {
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		bool isHovered = MouseInZone(fvector2d{ pos.x, pos.y }, fvector2d{ button_width, button_height });

		// Fade animation
		static DWORD64 clickTime = 0;
		static bool    isAnimating = false;

		float fadeAlpha = 0.0f;
		if (isAnimating) {
			DWORD64 elapsed = GetTickCount64() - clickTime;
			if (elapsed < 1000)
				fadeAlpha = 1.0f - (elapsed / 1000.0f);
			else
				isAnimating = false;
		}

		// Background
		flinearcolor bgColor;
		if (isAnimating) {
			float c = 44.0f * (1.0f - fadeAlpha) + fadeAlpha * 255.0f;
			bgColor = RGBtoFLC((int)c, (int)c, (int)c);
		}
		else {
			bgColor = RGBtoFLC(0, 0, 0);
		}
		drawFilledRect(fvector2d{ pos.x, pos.y }, button_width, button_height, bgColor);

		// Inner
		flinearcolor innerColor;
		if (isHovered) {
			if (isAnimating) {
				float c = 24.0f * (1.0f - fadeAlpha) + fadeAlpha * 255.0f;
				innerColor = RGBtoFLC((int)c, (int)c, (int)c);
			}
			else {
				innerColor = RGBtoFLC(24, 24, 24);
			}
			hover_element = true;
		}
		else {
			if (isAnimating) {
				float c = 44.0f * (1.0f - fadeAlpha) + fadeAlpha * 255.0f;
				innerColor = RGBtoFLC((int)c, (int)c, (int)c);
			}
			else {
				innerColor = RGBtoFLC(44, 44, 44);
			}
		}
		drawFilledRect(fvector2d{ pos.x + 1, pos.y + 1 }, button_width - 2, button_height - 2, innerColor);

		if (!sameLine)
			offset_y += button_height + padding.y;

		// Text
		flinearcolor textColor;
		if (isAnimating) {
			float c = 200.0f * (1.0f - fadeAlpha) + fadeAlpha * 200.0f;
			textColor = RGBtoFLC((int)c, (int)c, (int)c);
		}
		else {
			textColor = RGBtoFLC(200, 200, 200);
		}

		fvector2d textPos = fvector2d{ pos.x + button_width / 2, pos.y + button_height / 2 };
		canvas->k2_drawtext(font, name, textPos, fvector2d(0.95f, 0.80f),
			textColor, 0.0f, Colors::Text_Shadow,
			fvector2d(0, 0), true, true, false, Colors::Text_Outline);

		sameLine = false;
		last_element_pos = pos;
		last_element_size = fvector2d{ button_width, button_height };
		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;

		bool clicked = false;
		if (isHovered && menu::input::is_mouse_clicked(0, elements_count, false)) {
			clicked = true;
			isAnimating = true;
			clickTime = GetTickCount64();
		}

		return clicked;
	}

	bool checkbox_enabled[256];
	void Combobox(fvector2d size, int* value, const wchar_t* arg, ...)
	{
		elements_count++;

		fvector2d padding = fvector2d{ 33, 11 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y;
		}

		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 20.0f;
			offset_y = pos.y - menu_pos.y;
		}

		bool isHovered = MouseInZone(fvector2d{ pos.x, pos.y }, size);

		float combo_width = size.x;
		float combo_height = size.y;
		float item_content_height = size.y - 2; // Content area

		if (!sameLine)
			offset_y += combo_height + padding.y;

		// DEFAULT STATE (NOT OPEN)
		if (!checkbox_enabled[elements_count])
		{
			// Outer border (221x20)
			drawFilledRect(fvector2d{ pos.x, pos.y }, combo_width, combo_height, RGBtoFLC(36, 36, 36));

			// Inner background (219x18)
			drawFilledRect(fvector2d{ pos.x + 1, pos.y + 1 }, combo_width - 2, combo_height - 2, RGBtoFLC(22, 22, 22));

			// Draw selected item text
			va_list args;
			va_start(args, arg);
			const wchar_t* current_arg = arg;
			int current_num = 0;
			while (current_arg != NULL && current_num <= *value)
			{
				if (current_num == *value)
				{
					fvector2d selectedTextPos = fvector2d{ pos.x + 5, pos.y + combo_height / 2 };
					canvas->k2_drawtext(font, current_arg, selectedTextPos, fvector2d(0.95f, 0.80f),
						RGBtoFLC(200, 200, 200), 0.0f, RGBtoFLC(0, 0, 0),
						fvector2d(0, 0), false, true, false, Colors::Text_Outline);
					break;
				}
				current_num++;
				current_arg = va_arg(args, const wchar_t*);
			}
			va_end(args);

			// Draw dropdown arrow (pointing up when closed - NORMAL)
			float arrow_x = pos.x + combo_width - 15.0f;
			float arrow_y = pos.y + combo_height / 2.0f;

			drawFilledRect(fvector2d{ arrow_x, arrow_y - 1 }, 5.0f, 1.0f, RGBtoFLC(179, 179, 179));
			drawFilledRect(fvector2d{ arrow_x + 1, arrow_y }, 3.0f, 1.0f, RGBtoFLC(179, 179, 179));
			drawFilledRect(fvector2d{ arrow_x + 2, arrow_y + 1 }, 1.0f, 1.0f, RGBtoFLC(179, 179, 179));
		}
		// OPEN STATE
		else
		{
			hover_element = true;

			// Draw main dropdown (closed state but shown)
			drawFilledRect(fvector2d{ pos.x, pos.y }, combo_width, combo_height, RGBtoFLC(36, 36, 36));
			drawFilledRect(fvector2d{ pos.x + 1, pos.y + 1 }, combo_width - 2, combo_height - 2, RGBtoFLC(22, 22, 22));

			// Draw selected item in main dropdown
			va_list args2;
			va_start(args2, arg);
			const wchar_t* current_arg2 = arg;
			int current_num2 = 0;
			while (current_arg2 != NULL && current_num2 <= *value)
			{
				if (current_num2 == *value)
				{
					fvector2d selectedTextPos = fvector2d{ pos.x + 5, pos.y + combo_height / 2 };
					canvas->k2_drawtext(font, current_arg2, selectedTextPos, fvector2d(0.95f, 0.80f),
						RGBtoFLC(200, 200, 200), 0.0f, RGBtoFLC(0, 0, 0),
						fvector2d(0, 0), false, true, false, Colors::Text_Outline);
					break;
				}
				current_num2++;
				current_arg2 = va_arg(args2, const wchar_t*);
			}
			va_end(args2);

			// Draw dropdown arrow (pointing down when open)
			float arrow_x = pos.x + combo_width - 15.0f;
			float arrow_y = pos.y + combo_height / 2.0f;

			drawFilledRect(fvector2d{ arrow_x, arrow_y + 1 }, 5.0f, 1.0f, RGBtoFLC(179, 179, 179));
			drawFilledRect(fvector2d{ arrow_x + 1, arrow_y }, 3.0f, 1.0f, RGBtoFLC(179, 179, 179));
			drawFilledRect(fvector2d{ arrow_x + 2, arrow_y - 1 }, 1.0f, 1.0f, RGBtoFLC(179, 179, 179));

			// Draw dropdown items BELOW main dropdown
			fvector2d item_pos = fvector2d{ pos.x, pos.y + combo_height }; // Directly below

			// Count total items first
			va_list count_args;
			va_start(count_args, arg);
			const wchar_t* count_arg = arg;
			int item_count = 0;
			while (count_arg != NULL)
			{
				item_count++;
				count_arg = va_arg(count_args, const wchar_t*);
			}
			va_end(count_args);

			// Now draw items with SHARED borders (NO GAP)
			va_list arguments;
			va_start(arguments, arg);
			int num = 0;

			for (const wchar_t* current_arg3 = arg; current_arg3 != NULL; current_arg3 = va_arg(arguments, const wchar_t*))
			{
				// Calculate position for this item
				float item_top = item_pos.y;
				float item_height = item_content_height;

				// Add 1px for top border on first item
				if (num == 0)
				{
					item_top += 0; // Will draw top border separately
					item_height = item_content_height;
				}

				bool isItemHovered = MouseInZone(fvector2d{ item_pos.x, item_top },
					fvector2d{ combo_width, item_height });

				// Click detection
				if (isItemHovered && input::is_mouse_clicked(0, elements_count, false))
				{
					*value = num;
					checkbox_enabled[elements_count] = false;
				}

				// Draw LEFT border (always)
				PostRenderer::drawFilledRect(fvector2d{ item_pos.x, item_top }, 1, item_height, RGBtoFLC(36, 36, 36));

				// Draw RIGHT border (always)
				PostRenderer::drawFilledRect(fvector2d{ item_pos.x + combo_width - 1, item_top }, 1, item_height, RGBtoFLC(36, 36, 36));

				// Draw TOP border (ONLY for first item)
				if (num == 0)
				{
					PostRenderer::drawFilledRect(fvector2d{ item_pos.x, item_top }, combo_width, 1, RGBtoFLC(36, 36, 36));
				}

				// Draw BOTTOM border (ONLY for last item)
				if (num == item_count - 1)
				{
					PostRenderer::drawFilledRect(fvector2d{ item_pos.x, item_top + item_height - 1 }, combo_width, 1, RGBtoFLC(36, 36, 36));
				}

				// Draw inner background
				float inner_top = item_top + (num == 0 ? 1 : 0); // Offset for top border on first item
				float inner_height = item_height - (num == 0 ? 1 : 0) - (num == item_count - 1 ? 1 : 0);

				if (num == *value) // SELECTED ITEM
				{
					PostRenderer::drawFilledRect(fvector2d{ item_pos.x + 1, inner_top }, combo_width - 2, inner_height, RGBtoFLC(26, 26, 26));
				}
				else // REGULAR ITEM
				{
					PostRenderer::drawFilledRect(fvector2d{ item_pos.x + 1, inner_top }, combo_width - 2, inner_height, RGBtoFLC(22, 22, 22));
				}

				// Draw item text
				fvector2d itemTextPos = fvector2d{ item_pos.x + 5, inner_top + inner_height / 2 };
				PostRenderer::Text(current_arg3, itemTextPos, fvector2d(0.95, 0.85),
					RGBtoFLC(200, 200, 200), 0.0f, RGBtoFLC(0, 0, 0),
					fvector2d(0, 0), false, false, Colors::Text_Outline);

				// Move to next item - NO vertical borders between items
				item_pos.y += item_content_height; // Move down 18px (content only, no borders)
				num++;
			}
			va_end(arguments);

			// Update element bounds
			if (checkbox_enabled[elements_count])
			{
				current_element_pos = pos;
				current_element_size = fvector2d{ combo_width, item_pos.y - pos.y };
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = fvector2d{ combo_width, combo_height };

		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;

		// Handle opening/closing
		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = !checkbox_enabled[elements_count];
		}

		// Close when clicking outside
		if (!isHovered && checkbox_enabled[elements_count] &&
			input::is_mouse_clicked(0, elements_count, false))
		{
			checkbox_enabled[elements_count] = false;
		}
	}

	void SliderFloat(const wchar_t* name, float* value, float min, float max, const char* format = "%.0f")
	{
		elements_count++;

		float outer_width = 152.0f;     // Rectangle 31: 152x8
		float outer_height = 8.0f;
		float inner_width = 150.0f;     // Rectangle 33: 150x6
		float inner_height = 6.0f;

		fvector2d size = fvector2d{ outer_width, 38 };
		fvector2d slider_size = fvector2d{ outer_width, outer_height };
		fvector2d adjust_zone = fvector2d{ 0, 20 };
		fvector2d padding = fvector2d{ 33, 11 };

		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y;
		}

		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		// Calculate positions - FIXED: Move bar 2px to the right
		float slider_y = pos.y + padding.y + 5.0f;
		float inner_x = pos.x + 1 + 2;  // +1 from outer + 2px correction
		float inner_y = slider_y + 1;

		bool isHovered = MouseInZone(fvector2d{ pos.x + 2, slider_y - adjust_zone.y }, // Also adjust hover zone
			fvector2d{ outer_width, outer_height + adjust_zone.y * 1.5f });

		if (!sameLine)
			offset_y += size.y + padding.y;

		// MINUS BUTTON - adjust position to match new bar position
		float minus_button_x = (pos.x + 2) - 10.0f - 2.5f; // Adjusted for 2px bar shift
		float minus_button_center_x = minus_button_x + 2.5f;
		float minus_button_y = slider_y + outer_height / 2.0f;

		// Draw minus button visual (5x1 horizontal line) - Rectangle 30
		drawFilledRect(fvector2d{ minus_button_center_x - 2.5f, minus_button_y - 0.5f }, 5.0f, 1.0f, RGBtoFLC(166, 166, 166));

		// Check if minus button is clicked (10x10 area)
		bool isMinusHovered = MouseInZone(fvector2d{ minus_button_x - 2.5f, minus_button_y - 5.0f }, fvector2d{ 10.0f, 10.0f });
		if (isMinusHovered && input::is_mouse_clicked(0, elements_count, false))
		{
			*value -= 1.0f;
			if (*value < min) *value = min;
		}

		// Rectangle 31 - Outer track background (152x8) - #252525 - FIXED: Move 2px right
		drawFilledRect(fvector2d{ pos.x + 2, slider_y },
			outer_width, outer_height, RGBtoFLC(37.0f, 37.0f, 37.0f));

		// Rectangle 33 - Inner track background (150x6) - #171717
		drawFilledRect(fvector2d{ inner_x, inner_y }, inner_width, inner_height, RGBtoFLC(23.0f, 23.0f, 23.0f));

		// Calculate filled portion based on value
		float fill_percentage = (*value - min) / (max - min);
		float filled_width = inner_width * fill_percentage;

		// Draw filled portion - Rectangle 32 (full width based on value) - #F92EFE
		if (filled_width > 0)
		{
			drawFilledRect(fvector2d{ inner_x, inner_y }, filled_width, inner_height, RGBtoFLC(249, 46, 254));
		}

		// Handle dragging - FIXED: Use correct inner_x position
		if (isHovered || current_element == elements_count)
		{
			if (input::is_mouse_clicked(0, elements_count, true))
			{
				current_element = elements_count;

				fvector2d cursorPos = CursorPos();
				// Calculate value based on inner track position
				float relative_x = cursorPos.x - inner_x;
				*value = (relative_x / inner_width) * (max - min) + min;

				if (*value < min) *value = min;
				if (*value > max) *value = max;
			}

			hover_element = true;
		}

		// Value text position - FIXED: Use correct inner_x for calculation
		char buffer[32];
		sprintf_s(buffer, format, *value);

		float value_text_x = inner_x + filled_width;
		fvector2d valueTextPos = fvector2d{ value_text_x, slider_y + outer_height + 1.0f };

		canvas->k2_drawtext(font, s2wc(buffer), valueTextPos, fvector2d(0.85f, 0.80f),
			RGBtoFLC(255, 255, 255, 1.2f), 0.0f, Colors::Text_Shadow,
			fvector2d(0, 0), false, false, false, Colors::Text_Outline);

		// PLUS BUTTON - adjust position to match new bar position
		float plus_button_x = (pos.x + 2) + outer_width + 10.0f; // Adjusted for 2px bar shift
		float plus_button_center_x = plus_button_x + 2.5f;
		float plus_button_y = slider_y + outer_height / 2.0f;

		// Draw plus button visual - Rectangle 29 (5x1 horizontal line)
		drawFilledRect(fvector2d{ plus_button_center_x - 2.5f, plus_button_y - 0.5f }, 5.0f, 1.0f, RGBtoFLC(166, 166, 166));
		// Draw plus button visual - Rectangle 28 (1x5 vertical line)
		drawFilledRect(fvector2d{ plus_button_center_x - 0.5f, plus_button_y - 2.5f }, 1.0f, 5.0f, RGBtoFLC(166, 166, 166));

		// Check if plus button is clicked (10x10 area)
		bool isPlusHovered = MouseInZone(fvector2d{ plus_button_x - 2.5f, plus_button_y - 5.0f }, fvector2d{ 10.0f, 10.0f });
		if (isPlusHovered && input::is_mouse_clicked(0, elements_count, false))
		{
			*value += 1.0f;
			if (*value > max) *value = max;
		}

		// Name text (left side) - FIXED: Keep original position
		fvector2d nameTextPos = fvector2d{ pos.x, pos.y + 1.0f };
		canvas->k2_drawtext(font, name, nameTextPos, fvector2d(0.90f, 0.85f),
			RGBtoFLC(255, 255, 255, 1.2f), 0.0f,
			RGBtoFLC(0, 0, 0), fvector2d(0, 0),
			false, false, false, Colors::Text_Outline);

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;

		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;
	}

	void SliderInt(const wchar_t* name, int* value, int min, int max, const char* format = "%d")
	{
		elements_count++;

		float outer_width = 152.0f;     // Rectangle 31: 152x8
		float outer_height = 8.0f;
		float inner_width = 150.0f;     // Rectangle 33: 150x6
		float inner_height = 6.0f;

		fvector2d size = fvector2d{ outer_width, 38 };
		fvector2d slider_size = fvector2d{ outer_width, outer_height };
		fvector2d adjust_zone = fvector2d{ 0, 20 };
		fvector2d padding = fvector2d{ 33, 11 };

		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y;
		}

		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		// Calculate positions - FIXED: Move bar 2px to the right
		float slider_y = pos.y + padding.y + 5.0f;
		float inner_x = pos.x + 1 + 2;  // +1 from outer + 2px correction
		float inner_y = slider_y + 1;

		bool isHovered = MouseInZone(fvector2d{ pos.x + 2, slider_y - adjust_zone.y }, // Also adjust hover zone
			fvector2d{ outer_width, outer_height + adjust_zone.y * 1.5f });

		if (!sameLine)
			offset_y += size.y + padding.y;

		// MINUS BUTTON - adjust position to match new bar position
		float minus_button_x = (pos.x + 2) - 10.0f - 2.5f; // Adjusted for 2px bar shift
		float minus_button_center_x = minus_button_x + 2.5f;
		float minus_button_y = slider_y + outer_height / 2.0f;

		// Draw minus button visual (5x1 horizontal line) - Rectangle 30
		drawFilledRect(fvector2d{ minus_button_center_x - 2.5f, minus_button_y - 0.5f }, 5.0f, 1.0f, RGBtoFLC(166, 166, 166));

		// Check if minus button is clicked (10x10 area)
		bool isMinusHovered = MouseInZone(fvector2d{ minus_button_x - 2.5f, minus_button_y - 5.0f }, fvector2d{ 10.0f, 10.0f });
		if (isMinusHovered && input::is_mouse_clicked(0, elements_count, false))
		{
			*value -= 1;
			if (*value < min) *value = min;
		}

		// Rectangle 31 - Outer track background (152x8) - #252525 - FIXED: Move 2px right
		drawFilledRect(fvector2d{ pos.x + 2, slider_y },
			outer_width, outer_height, RGBtoFLC(37.0f, 37.0f, 37.0f));

		// Rectangle 33 - Inner track background (150x6) - #171717
		drawFilledRect(fvector2d{ inner_x, inner_y }, inner_width, inner_height, RGBtoFLC(23.0f, 23.0f, 23.0f));

		// Calculate filled portion based on value
		float fill_percentage = (float)(*value - min) / (float)(max - min);
		float filled_width = inner_width * fill_percentage;

		// Draw filled portion - Rectangle 32 (full width based on value) - #F92EFE
		if (filled_width > 0)
		{
			drawFilledRect(fvector2d{ inner_x, inner_y }, filled_width, inner_height, RGBtoFLC(249, 46, 254));
		}

		// Handle dragging - FIXED: Use correct inner_x position
		if (isHovered || current_element == elements_count)
		{
			if (input::is_mouse_clicked(0, elements_count, true))
			{
				current_element = elements_count;

				fvector2d cursorPos = CursorPos();
				// Calculate value based on inner track position
				float relative_x = cursorPos.x - inner_x;
				float temp_value = (relative_x / inner_width) * (max - min) + min;

				// Round to nearest integer
				*value = (int)roundf(temp_value);

				if (*value < min) *value = min;
				if (*value > max) *value = max;
			}

			hover_element = true;
		}

		// Value text position - FIXED: Use correct inner_x for calculation
		char buffer[32];
		sprintf_s(buffer, format, *value);

		float value_text_x = inner_x + filled_width;
		fvector2d valueTextPos = fvector2d{ value_text_x, slider_y + outer_height + 1.0f };

		canvas->k2_drawtext(font, s2wc(buffer), valueTextPos, fvector2d(0.85f, 0.80f),
			RGBtoFLC(255, 255, 255, 1.2f), 0.0f, Colors::Text_Shadow,
			fvector2d(0, 0), false, false, false, Colors::Text_Outline);

		// PLUS BUTTON - adjust position to match new bar position
		float plus_button_x = (pos.x + 2) + outer_width + 10.0f; // Adjusted for 2px bar shift
		float plus_button_center_x = plus_button_x + 2.5f;
		float plus_button_y = slider_y + outer_height / 2.0f;

		// Draw plus button visual - Rectangle 29 (5x1 horizontal line)
		drawFilledRect(fvector2d{ plus_button_center_x - 2.5f, plus_button_y - 0.5f }, 5.0f, 1.0f, RGBtoFLC(166, 166, 166));
		// Draw plus button visual - Rectangle 28 (1x5 vertical line)
		drawFilledRect(fvector2d{ plus_button_center_x - 0.5f, plus_button_y - 2.5f }, 1.0f, 5.0f, RGBtoFLC(166, 166, 166));

		// Check if plus button is clicked (10x10 area)
		bool isPlusHovered = MouseInZone(fvector2d{ plus_button_x - 2.5f, plus_button_y - 5.0f }, fvector2d{ 10.0f, 10.0f });
		if (isPlusHovered && input::is_mouse_clicked(0, elements_count, false))
		{
			*value += 1;
			if (*value > max) *value = max;
		}

		// Name text (left side) - FIXED: Keep original position
		fvector2d nameTextPos = fvector2d{ pos.x, pos.y + 1.0f };
		canvas->k2_drawtext(font, name, nameTextPos, fvector2d(0.90f, 0.85f),
			RGBtoFLC(255, 255, 255, 1.2f), 0.0f,
			RGBtoFLC(0, 0, 0), fvector2d(0, 0),
			false, false, false, Colors::Text_Outline);

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;

		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;
	}

	static int active_textbox = -1;
	bool Textbox(fvector2d size, char* buffer, size_t buffer_size)
	{
		elements_count++;

		float outer_width = 171.0f;   // Rectangle 166: 171x20
		float outer_height = 20.0f;
		float inner_width = outer_width - 2.0f;   // Rectangle 167: 169x18
		float inner_height = outer_height - 2.0f;

		fvector2d padding = { 40, 14 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine) {
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y;
		}

		if (pushY) {
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		bool isHovered = MouseInZone(pos, fvector2d{ outer_width, outer_height });

		if (!sameLine)
			offset_y += outer_height + padding.y;

		// Rectangle 166 - Outer with transparency (171x20)
		drawFilledRect(fvector2d{ pos.x, pos.y }, outer_width, outer_height, RGBtoFLC(0, 0, 0, 0.25f));

		// Rectangle 167 - Inner background (169x18)
		float inner_x = pos.x + (outer_width - inner_width) / 2.0f; // Center inner
		float inner_y = pos.y + (outer_height - inner_height) / 2.0f;

		if (active_textbox == elements_count) {
			// Active state - slightly lighter
			drawFilledRect(fvector2d{ inner_x, inner_y }, inner_width, inner_height, RGBtoFLC(35, 35, 35));
		}
		else {
			// Inactive state
			drawFilledRect(fvector2d{ inner_x, inner_y }, inner_width, inner_height, RGBtoFLC(44, 44, 44));
		}

		// Hover effect
		if (isHovered && active_textbox != elements_count) {
			drawFilledRect(fvector2d{ inner_x + 1, inner_y + 1 }, inner_width - 2, inner_height - 2, RGBtoFLC(40, 40, 40));
		}

		static int active_textbox = -1;
		static bool already_pressed = false;
		bool edited = false;

		// Click/focus handling
		if (isHovered && input::is_mouse_clicked(0, elements_count, false)) {
			already_pressed = true;
			active_textbox = elements_count;
		}
		else if (!isHovered && input::is_mouse_clicked(0, elements_count, false)) {
			active_textbox = -1;
		}

		// Render text inside the box
		std::wstring buffer_wide(buffer, buffer + strlen(buffer));
		fvector2d textPos = fvector2d{ inner_x + 5.0f, inner_y + inner_height / 2.0f };
		canvas->k2_drawtext(font, buffer_wide.c_str(), textPos, fvector2d(0.90f, 0.85f),
			RGBtoFLC(255, 255, 255, 1.2f), 0.0f, Colors::Text_Shadow,
			fvector2d(0, 0), false, true, false, Colors::Text_Outline);

		// Blinking cursor + input logic
		if (active_textbox == elements_count) {
			// Blinking cursor
			if (static_cast<int>(GetTickCount() / 400) % 2)
				canvas->k2_drawtext(font, L"_",
					fvector2d{ textPos.x + buffer_wide.length() * 8.0f, textPos.y },
					fvector2d(0.90f, 0.85f), RGBtoFLC(255, 255, 255, 1.2f), 0,
					Colors::Text_Shadow, fvector2d(0, 0), false, true, false, Colors::Text_Outline);

			BYTE keyboard_state[256];
			GetKeyboardState(keyboard_state);

			bool ctrl_down = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

			for (int vkey = 0; vkey < 255; ++vkey) {
				SHORT state = GetAsyncKeyState(vkey);
				bool down = (state & 0x1); // On key press (not hold)

				if (down) {
					// --- PASTE SUPPORT (Ctrl+V) ---
					if (ctrl_down && (vkey == 'V' || vkey == 'v')) {
						if (OpenClipboard(nullptr)) {
							HANDLE hData = GetClipboardData(CF_UNICODETEXT); // Use wide clipboard
							if (hData) {
								LPCWSTR wstr = (LPCWSTR)GlobalLock(hData);
								if (wstr) {
									// Convert to ANSI
									char temp[512] = {};
									WideCharToMultiByte(CP_ACP, 0, wstr, -1, temp, sizeof(temp) - 1, nullptr, nullptr);
									// Paste as much as fits in the buffer
									size_t space = buffer_size - strlen(buffer) - 1;
									strncat(buffer, temp, space);
									edited = true;
									GlobalUnlock(hData);
								}
							}
							CloseClipboard();
						}
						continue; // Don't process V as normal char
					}

					// Backspace
					if (vkey == VK_BACK && strlen(buffer) > 0) {
						buffer[strlen(buffer) - 1] = '\0';
						edited = true;
					}
					// Enter: lose focus
					else if (vkey == VK_RETURN) {
						active_textbox = -1;
					}
					// Typing
					else if (strlen(buffer) < buffer_size - 1) {
						WCHAR unicode_char[4] = {};
						int res = ToUnicode(vkey, MapVirtualKey(vkey, MAPVK_VK_TO_VSC), keyboard_state, unicode_char, 4, 0);
						if (res > 0 && iswprint(unicode_char[0])) {
							char ansi_char[4] = {};
							WideCharToMultiByte(CP_ACP, 0, unicode_char, res, ansi_char, sizeof(ansi_char), nullptr, nullptr);
							strncat(buffer, ansi_char, buffer_size - strlen(buffer) - 1);
							edited = true;
						}
					}
				}
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = fvector2d{ outer_width, outer_height };

		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;

		return edited;
	}

	void InputField(const wchar_t* label, std::string* value, size_t max_len)
	{
		elements_count++;

		float outer_width = 171.0f;
		float outer_height = 20.0f;
		float inner_width = outer_width - 2.0f;
		float inner_height = outer_height - 2.0f;

		fvector2d padding = { 33, 11 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x,
								   menu_pos.y + padding.y + offset_y };

		if (sameLine) {
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y;
		}
		if (pushY) {
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		bool isHovered = MouseInZone(pos, fvector2d{ outer_width, outer_height });

		if (!sameLine)
			offset_y += outer_height + padding.y + 4.0f;

		// Label au dessus
		canvas->k2_drawtext(font, label,
			fvector2d{ pos.x, pos.y - 12.f },
			fvector2d(0.85f, 0.80f),
			RGBtoFLC(200, 200, 200, 1.f), 0.f,
			Colors::Text_Shadow, fvector2d(0, 0),
			false, false, false, Colors::Text_Outline);

		// Outer border
		drawFilledRect(fvector2d{ pos.x, pos.y }, outer_width, outer_height,
			(active_textbox == elements_count)
			? RGBtoFLC(249, 46, 254)   // active : bordure rose
			: RGBtoFLC(36, 36, 36));   // inactif : gris

		// Inner background
		float inner_x = pos.x + 1.f;
		float inner_y = pos.y + 1.f;
		drawFilledRect(fvector2d{ inner_x, inner_y }, inner_width, inner_height,
			(active_textbox == elements_count)
			? RGBtoFLC(30, 30, 30)
			: RGBtoFLC(22, 22, 22));

		// Clamp value a max_len pour eviter crash
		if (value->size() > max_len - 1)
			value->resize(max_len - 1);

		// Render text
		std::wstring wval(value->begin(), value->end());
		canvas->k2_drawtext(font, wval.c_str(),
			fvector2d{ inner_x + 5.f, inner_y + inner_height / 2.f },
			fvector2d(0.90f, 0.85f),
			RGBtoFLC(255, 255, 255, 1.2f), 0.f,
			Colors::Text_Shadow, fvector2d(0, 0),
			false, true, false, Colors::Text_Outline);

		// Curseur clignotant
		if (active_textbox == elements_count) {
			if ((int)(GetTickCount() / 400) % 2) {
				float cursor_x = inner_x + 5.f + (float)wval.size() * 7.5f;
				canvas->k2_drawtext(font, L"|",
					fvector2d{ cursor_x, inner_y + inner_height / 2.f },
					fvector2d(0.85f, 0.80f),
					RGBtoFLC(249, 46, 254, 1.f), 0.f,
					Colors::Text_Shadow, fvector2d(0, 0),
					false, true, false, Colors::Text_Outline);
			}
		}

		// Focus click
		if (isHovered && input::is_mouse_clicked(0, elements_count, false))
			active_textbox = elements_count;
		else if (!isHovered && input::is_mouse_clicked(0, elements_count, false))
			active_textbox = -1;

		// Input clavier
		if (active_textbox == elements_count) {
			hover_element = true;

			BYTE kb[256];
			GetKeyboardState(kb);
			bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

			for (int vk = 0; vk < 255; vk++) {
				if (!(GetAsyncKeyState(vk) & 0x1)) continue;

				// Ctrl+V paste
				if (ctrl && (vk == 'V')) {
					if (OpenClipboard(nullptr)) {
						HANDLE h = GetClipboardData(CF_UNICODETEXT);
						if (h) {
							LPCWSTR ws = (LPCWSTR)GlobalLock(h);
							if (ws) {
								char tmp[512] = {};
								WideCharToMultiByte(CP_UTF8, 0, ws, -1, tmp, 511, nullptr, nullptr);
								size_t space = max_len - value->size() - 1;
								if (space > 0)
									value->append(tmp, min(space, strlen(tmp)));
								GlobalUnlock(h);
							}
						}
						CloseClipboard();
					}
					continue;
				}

				if (vk == VK_BACK) {
					if (!value->empty()) value->pop_back();
				}
				else if (vk == VK_RETURN) {
					active_textbox = -1;
				}
				else if (value->size() < max_len - 1) {
					WCHAR wc[4] = {};
					int r = ToUnicode(vk, MapVirtualKey(vk, MAPVK_VK_TO_VSC), kb, wc, 4, 0);
					if (r > 0 && iswprint(wc[0])) {
						char mb[4] = {};
						WideCharToMultiByte(CP_UTF8, 0, wc, r, mb, 3, nullptr, nullptr);
						value->append(mb);
					}
				}
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = fvector2d{ outer_width, outer_height };
		if (first_element_pos.x == 0.f) first_element_pos = pos;
	}

	std::string VirtualKeyCodeToString(UCHAR virtualKey)
	{
		// Return empty string for 0 or invalid keys
		if (virtualKey == 0 || virtualKey == -1)
		{
			return "";
		}

		// Handle mouse buttons first
		switch (virtualKey)
		{
		case VK_LBUTTON: return "MB1";
		case VK_RBUTTON: return "MB2";
		case VK_MBUTTON: return "MB3";
		case VK_XBUTTON1: return "MB4";
		case VK_XBUTTON2: return "MB5";
		}

		// Handle special keys with custom names
		switch (virtualKey)
		{
		case VK_INSERT: return "INS";
		case VK_DELETE: return "DEL";
		case VK_HOME: return "HOME";
		case VK_END: return "END";
		case VK_PRIOR: return "PGUP";
		case VK_NEXT: return "PGDN";
		case VK_LEFT: return "LEFT";
		case VK_RIGHT: return "RIGHT";
		case VK_UP: return "UP";
		case VK_DOWN: return "DOWN";
		case VK_SPACE: return "SPACE";
		case VK_RETURN: return "ENTER";
		case VK_ESCAPE: return "ESC";
		case VK_BACK: return "BACK";
		case VK_TAB: return "TAB";
		case VK_CAPITAL: return "CAPS";
		case VK_NUMLOCK: return "NUMLK";
		case VK_SCROLL: return "SCRLK";
		case VK_PAUSE: return "PAUSE";
		case VK_SNAPSHOT: return "PRTSC";
		case VK_LSHIFT: return "LSHIFT";
		case VK_RSHIFT: return "RSHIFT";
		case VK_LCONTROL: return "LCTRL";
		case VK_RCONTROL: return "RCTRL";
		case VK_LMENU: return "LALT";
		case VK_RMENU: return "RALT";
		case VK_LWIN: return "LWIN";
		case VK_RWIN: return "RWIN";
		case VK_APPS: return "MENU";
		case VK_ADD: return "+";
		case VK_SUBTRACT: return "-";
		case VK_MULTIPLY: return "*";
		case VK_DIVIDE: return "/";
		case VK_DECIMAL: return ".";
		case VK_OEM_PLUS: return "=";
		case VK_OEM_MINUS: return "-";
		case VK_OEM_COMMA: return ",";
		case VK_OEM_PERIOD: return ".";
		case VK_OEM_1: return ";";
		case VK_OEM_2: return "/";
		case VK_OEM_3: return "`";
		case VK_OEM_4: return "[";
		case VK_OEM_5: return "\\";
		case VK_OEM_6: return "]";
		case VK_OEM_7: return "'";
		}

		// Handle number pad keys
		if (virtualKey >= VK_NUMPAD0 && virtualKey <= VK_NUMPAD9)
		{
			return "NUM" + std::to_string(virtualKey - VK_NUMPAD0);
		}

		// Handle F keys
		if (virtualKey >= VK_F1 && virtualKey <= VK_F24)
		{
			return "F" + std::to_string(virtualKey - VK_F1 + 1);
		}

		// Handle letter keys (A-Z)
		if (virtualKey >= 'A' && virtualKey <= 'Z')
		{
			return std::string(1, (char)virtualKey);
		}

		// Handle number keys (0-9)
		if (virtualKey >= '0' && virtualKey <= '9')
		{
			return std::string(1, (char)virtualKey);
		}

		// For any other keys, use the default Windows name
		UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
		CHAR szName[128];

		// Set extended flag for certain keys
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
		}

		int result = GetKeyNameTextA(scanCode << 16, szName, 128);

		// Convert the Windows name to uppercase for consistency
		std::string keyName = szName;
		std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::toupper);

		return keyName;
	}

	int active_hotkey = -1;
	bool already_pressed = false;

	int active_hotkey_labeled = -1;
	bool already_pressed_labeled = false;

	void Hotkey(const char* name, fvector2d size, int* key)
	{
		elements_count++;
		fvector2d padding = fvector2d{ 158, -2 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };
		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y + (last_element_size.y / 2) - size.y / 2;
		}
		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}
		bool isHovered = MouseInZone(fvector2d{ pos.x, pos.y }, size);

		if (!sameLine)
			offset_y += size.y + padding.y;

		if (active_hotkey == elements_count)
		{
			fvector2d textPos = fvector2d{ pos.x + size.x / 2, pos.y + size.y / 2 };
			std::string displayText = "[ ... ]";
			canvas->k2_drawtext(font, s2wc(displayText.c_str()), textPos, fvector2d(0.80f, 0.75f), RGBtoFLC(120, 120, 120, 1.2f), 0.0f, Colors::Text_Shadow, fvector2d(0, 0), true, true, true, RGBtoFLC(0, 0, 0, 0.1f));

			if (!menu::input::is_any_mouse_down())
				already_pressed = false;

			if (!already_pressed)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code))
					{
						*key = code;
						active_hotkey = -1;
					}
				}
			}
		}
		else
		{
			fvector2d textPos = fvector2d{ pos.x + size.x / 2, pos.y + size.y / 2 };
			std::string displayText;
			if (*key == 0 || *key == -1)
				displayText = "[ - ]";
			else
				displayText = "[ " + VirtualKeyCodeToString(*key) + " ]";

			canvas->k2_drawtext(font, s2wc(displayText.c_str()), textPos, fvector2d(0.80f, 0.75f), RGBtoFLC(120, 120, 120, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, true, RGBtoFLC(0, 0, 0, 0.1f));

			if (isHovered)
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					already_pressed = true;
					active_hotkey = elements_count;
					for (int code = 0; code < 255; code++)
						if (GetAsyncKeyState(code)) {}
				}
			}
			else
			{
				if (input::is_mouse_clicked(0, elements_count, false))
					active_hotkey = -1;
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;
	}

	void HotkeyLabeled(const char* name, int* key)
	{
		elements_count++;
		fvector2d padding = fvector2d{ 158, -2 };
		fvector2d size = fvector2d{ 9, 18 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };

		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y + (last_element_size.y / 2) - size.y / 2;
		}
		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}

		bool isHovered = MouseInZone(fvector2d{ pos.x, pos.y }, size);

		if (!sameLine)
			offset_y += size.y + padding.y;

		// Label a gauche de la hotkey
		fvector2d labelPos = fvector2d{ pos.x - 10, pos.y + size.y / 2 };
		canvas->k2_drawtext(font, s2wc(name), labelPos,
			fvector2d(0.80f, 0.75f),
			RGBtoFLC(120, 120, 120, 1.2f),
			0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0),
			true, true, true, RGBtoFLC(0, 0, 0, 0.1f));

		if (active_hotkey_labeled == elements_count)
		{
			fvector2d textPos = fvector2d{ pos.x + size.x / 2, pos.y + size.y / 2 };
			std::string displayText = "[ ... ]";
			canvas->k2_drawtext(font, s2wc(displayText.c_str()), textPos, fvector2d(0.80f, 0.75f), RGBtoFLC(120, 120, 120, 1.2f), 0.0f, Colors::Text_Shadow, fvector2d(0, 0), true, true, true, RGBtoFLC(0, 0, 0, 0.1f));

			if (!menu::input::is_any_mouse_down())
				already_pressed_labeled = false;

			if (!already_pressed_labeled)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code))
					{
						*key = code;
						active_hotkey_labeled = -1;
					}
				}
			}
		}
		else
		{
			fvector2d textPos = fvector2d{ pos.x + size.x / 2, pos.y + size.y / 2 };
			std::string displayText;
			if (*key == 0 || *key == -1)
				displayText = "[ - ]";
			else
				displayText = "[ " + VirtualKeyCodeToString(*key) + " ]";

			canvas->k2_drawtext(font, s2wc(displayText.c_str()), textPos, fvector2d(0.80f, 0.75f), RGBtoFLC(120, 120, 120, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, true, RGBtoFLC(0, 0, 0, 0.1f));

			if (isHovered)
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					already_pressed_labeled = true;
					active_hotkey_labeled = elements_count;
					for (int code = 0; code < 255; code++)
						if (GetAsyncKeyState(code)) {}
				}
			}
			else
			{
				if (input::is_mouse_clicked(0, elements_count, false))
					active_hotkey_labeled = -1;
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;
	}


	int active_hotkey2 = -1;
	bool already_pressed2 = false;
	void Hotkey2(const char* name, fvector2d size, int* key)
	{
		elements_count++;
		fvector2d padding = fvector2d{ 191, 4 };
		fvector2d pos = fvector2d{ menu_pos.x + padding.x + offset_x, menu_pos.y + padding.y + offset_y };
		if (sameLine)
		{
			pos.x = last_element_pos.x + last_element_size.x + padding.x;
			pos.y = last_element_pos.y + (last_element_size.y / 2) - size.y / 2;
		}
		if (pushY)
		{
			pos.y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.y - menu_pos.y;
		}
		bool isHovered = MouseInZone(fvector2d{ pos.x, pos.y }, size);

		if (!sameLine)
			offset_y += size.y + padding.y;

		if (active_hotkey2 == elements_count)
		{
			//Text
			fvector2d textPos = fvector2d{ pos.x + size.x / 2, pos.y + size.y / 2 };
			std::string displayText = "[ ... ]";

			canvas->k2_drawtext(font, s2wc(displayText.c_str()), textPos, fvector2d(0.80f, 0.75f), RGBtoFLC(150, 150, 150, 1.2f), 0.0f, Colors::Text_Shadow, fvector2d(0, 0), true, true, true, RGBtoFLC(0, 0, 0, 0.1f));

			if (!menu::input::is_any_mouse_down())
			{
				already_pressed2 = false;
			}

			if (!already_pressed2)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code))
					{
						*key = code;
						active_hotkey2 = -1;
					}
				}
			}
		}
		else
		{
			//Text
			fvector2d textPos = fvector2d{ pos.x + size.x / 2, pos.y + size.y / 2 };
			std::string displayText;
			if (*key == 0 || *key == -1) // Check if key is not set
			{
				displayText = "[ - ]";
			}
			else
			{
				displayText = "[ " + VirtualKeyCodeToString(*key) + " ]";
			}

			canvas->k2_drawtext(font, s2wc(displayText.c_str()), textPos, fvector2d(0.80f, 0.75f), RGBtoFLC(150, 150, 150, 1.2f), 0.0f, RGBtoFLC(0, 0, 0), fvector2d(0, 0), true, true, true, RGBtoFLC(0, 0, 0, 0.1f));

			if (isHovered)
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					already_pressed2 = true;
					active_hotkey2 = elements_count;

					//Queue Fix
					for (int code = 0; code < 255; code++)
						if (GetAsyncKeyState(code)) {}
				}
			}
			else
			{
				if (input::is_mouse_clicked(0, elements_count, false))
				{
					active_hotkey2 = -1;
				}
			}
		}

		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.x == 0.0f)
			first_element_pos = pos;
	}

	void Render()
	{
		for (int i = 0; i < 128; i++)
		{
			if (PostRenderer::drawlist[i].type != -1)
			{
				// OLD: FilledRect
				if (PostRenderer::drawlist[i].type == 1)
				{
					menu::drawFilledRect(PostRenderer::drawlist[i].pos,
						PostRenderer::drawlist[i].size.x,
						PostRenderer::drawlist[i].size.y,
						PostRenderer::drawlist[i].color);
				}
				// OLD: TextLeft
				else if (PostRenderer::drawlist[i].type == 2)
				{
					canvas->k2_drawtext(font, PostRenderer::drawlist[i].name,
						PostRenderer::drawlist[i].pos,
						fvector2d(0.98, 0.98),
						PostRenderer::drawlist[i].color,
						0.0f, Colors::Text_Shadow,
						fvector2d(0, 0), false, false,
						PostRenderer::drawlist[i].outline, Colors::Text_Outline);
				}
				// OLD: TextCenter
				else if (PostRenderer::drawlist[i].type == 3)
				{
					menu::TextCenter(PostRenderer::drawlist[i].name,
						PostRenderer::drawlist[i].pos,
						PostRenderer::drawlist[i].color,
						PostRenderer::drawlist[i].outline);
				}
				// OLD: Draw_Line
				else if (PostRenderer::drawlist[i].type == 4)
				{
					Draw_Line(PostRenderer::drawlist[i].from,
						PostRenderer::drawlist[i].to,
						PostRenderer::drawlist[i].thickness,
						PostRenderer::drawlist[i].color);
				}
				// NEW: Text (with full parameters)
				else if (PostRenderer::drawlist[i].type == 5)
				{
					canvas->k2_drawtext(font,
						PostRenderer::drawlist[i].name,
						PostRenderer::drawlist[i].pos,
						PostRenderer::drawlist[i].scale,
						PostRenderer::drawlist[i].color,
						0.0f,
						PostRenderer::drawlist[i].outline_color,
						PostRenderer::drawlist[i].shadow_offset,
						PostRenderer::drawlist[i].center,
						true,
						PostRenderer::drawlist[i].outline,
						Colors::Text_Outline);
				}
				// NEW: FilledRectEx
				else if (PostRenderer::drawlist[i].type == 6)
				{
					menu::drawFilledRect(PostRenderer::drawlist[i].pos,
						PostRenderer::drawlist[i].width,
						PostRenderer::drawlist[i].height,
						PostRenderer::drawlist[i].color);
				}

				PostRenderer::drawlist[i].type = -1;
			}
		}
	}
}

