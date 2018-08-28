#include "sdk.h"
#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "ESP.h"
#include "imgui\imconfig.h"
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_internal.h"
#include "ImGUI\stb_rect_pack.h"
#include "ImGUI\stb_textedit.h"
#include "ImGUI\stb_truetype.h"
#include "ImGUI\dx9\imgui_dx9.h"
#include "Items.h"
#include "event_log.h"
#include "GameUtils.h"
#include "render.h"
#include "BacktrackingHelper.h"
#include <Shlobj.h>
#include <intrin.h>

IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
DWORD dwOld_D3DRS_COLORWRITEENABLE;

void SaveState(IDirect3DDevice9 * pDevice)
{
	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->GetVertexShader(&vertShader);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
}

void RestoreState(IDirect3DDevice9 * pDevice) // not restoring everything. Because its not needed.
{
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	pDevice->SetVertexDeclaration(vertDec);
	pDevice->SetVertexShader(vertShader);
}

bool ClientVariables::Save(std::string file_name)
{
	CreateDirectory("C:\\opaihook", NULL);

	std::string file_path = "C:\\opaihook\\" + file_name + ".cfg";

	std::fstream file(file_path, std::ios::out | std::ios::in | std::ios::trunc);
	file.close();

	file.open(file_path, std::ios::out | std::ios::in);
	if (!file.is_open())
	{
		file.close();
		return false;
	}

	const size_t settings_size = sizeof(ClientVariables);
	for (int i = 0; i < settings_size; i++)
	{
		byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
		for (int x = 0; x < 8; x++)
		{
			file << (int)((current_byte >> x) & 1);
		}
	}

	file.close();

	return true;
}

bool ClientVariables::Load(std::string file_name)
{
	std::string file_path = "C:\\opaihook\\" + file_name + ".cfg";

	std::fstream file;
	file.open(file_path, std::ios::out | std::ios::in);
	if (!file.is_open())
	{
		file.close();
		return false;
	}

	std::string line;
	while (file)
	{
		std::getline(file, line);

		const size_t settings_size = sizeof(ClientVariables);
		if (line.size() > settings_size * 8)
		{
			file.close();
			return false;
		}
		for (int i = 0; i < settings_size; i++)
		{
			byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
			for (int x = 0; x < 8; x++)
			{
				if (line[(i * 8) + x] == '1')
					current_byte |= 1 << x;
				else
					current_byte &= ~(1 << x);
			}
			*reinterpret_cast<byte*>(uintptr_t(this) + i) = current_byte;
		}
	}

	file.close();

	return true;
}

void ClientVariables::CreateConfig(std::string name)
{
	CreateDirectory("C:\\opaihook\\", NULL); CreateDirectory("C:\\opaihook\\", NULL);

	std::ofstream ofs("C:\\opaihook\\" + name + ".cfg");

}

char* string_as_array(std::string* str)
{
	return str->empty() ? NULL : &*str->begin();
}

void ClientVariables::Delete(std::string name) {

	std::string l1 = "C:\\opaihook\\";
	std::string l2 = ".cfg";
	l1 = l1 + name + l2;
	const char* cfg = string_as_array(&l1);
	const int result = remove(cfg);
}

std::vector<std::string> ClientVariables::GetConfigs()
{
	std::vector<std::string> configs;

	WIN32_FIND_DATA ffd;
	auto directory = "C:\\opaihook\\*";
	auto hFind = FindFirstFile(directory, &ffd);

	while (FindNextFile(hFind, &ffd))
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			std::string file_name = ffd.cFileName;
			if (file_name.size() < 4) // .cfg
				continue;

			std::string end = file_name;
			end.erase(end.begin(), end.end() - 4); // erase everything but the last 4 letters
			if (end != ".cfg")
				continue;

			file_name.erase(file_name.end() - 4, file_name.end()); // erase the .cfg part
			configs.push_back(file_name);
		}
	}

	return configs;
}

LockCursor_t oLockCursor;

void __stdcall Hooks::hk_lockcursor()
{
	if (csgo::ShowMenu)
	{
		g_pSurface->unlockcursor();
		return;
	}

	oLockCursor(g_pSurface);
}


namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	IMGUI_API bool ComboBoxArray(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}
	
	void SelectTabs(int *selected, const char* items[], int item_count, ImVec2 size = ImVec2(0, 0))
	{
		auto color_grayblue = GetColorU32(ImVec4(0.48, 0.48, 0.48, 0.20));
		auto color_deepblue = GetColorU32(ImVec4(0.12, 0.12, 0.12, 0.15));
		auto color_shade_hover = GetColorU32(ImVec4(0.8, 0.8, 0.8, 0.05));
		auto color_shade_clicked = GetColorU32(ImVec4(0.1, 0.1, 0.1, 0.1));
		auto color_black_outlines = GetColorU32(ImVec4(0, 0, 0, 1));

		ImGuiStyle &style = GetStyle();
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		std::string names;
		for (size_t i = 0; i < item_count; i++)
			names += items[i];

		ImGuiContext* g = GImGui;
		const ImGuiID id = window->GetID(names.c_str());
		const ImVec2 label_size = CalcTextSize(names.c_str(), NULL, true);

		ImVec2 Min = window->DC.CursorPos;
		ImVec2 Max = ((size.x <= 0 || size.y <= 0) ? ImVec2(Min.x + GetContentRegionMax().x - style.WindowPadding.x, Min.y + label_size.y * 2) : Min);

		ImRect bb(Min, Max);
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, &id))
			return;

		PushClipRect(ImVec2(Min.x, Min.y - 1), ImVec2(Max.x, Max.y + 1), false);

		window->DrawList->AddRectFilledMultiColor(Min, Max, color_grayblue, color_grayblue, color_deepblue, color_deepblue); // Main gradient.

		ImVec2 mouse_pos = GetMousePos();
		bool mouse_click = g->IO.MouseClicked[0];

		float TabSize = ceil((Max.x - Min.x) / item_count);

		for (size_t i = 0; i < item_count; i++)
		{
			ImVec2 Min_cur_label = ImVec2(Min.x + (int)TabSize * i, Min.y);
			ImVec2 Max_cur_label = ImVec2(Min.x + (int)TabSize * i + (int)TabSize, Max.y);

			// Imprecision clamping. gay but works :^)
			Max_cur_label.x = (Max_cur_label.x >= Max.x ? Max.x : Max_cur_label.x);

			if (mouse_pos.x > Min_cur_label.x && mouse_pos.x < Max_cur_label.x &&
				mouse_pos.y > Min_cur_label.y && mouse_pos.y < Max_cur_label.y)
			{
				if (mouse_click)
					*selected = i;
				else if (i != *selected)
					window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_hover);
			}

			if (i == *selected) {
				window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_clicked);
				window->DrawList->AddRectFilledMultiColor(Min_cur_label, Max_cur_label, color_deepblue, color_deepblue, color_grayblue, color_grayblue);
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1.5f, Min_cur_label.y - 1), ImVec2(Max_cur_label.x - 0.5f, Min_cur_label.y - 1), color_black_outlines);
			}
			else
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1, Min_cur_label.y), ImVec2(Max_cur_label.x, Min_cur_label.y), color_black_outlines);
			window->DrawList->AddLine(ImVec2(Max_cur_label.x - 1, Max_cur_label.y), ImVec2(Max_cur_label.x - 1, Min_cur_label.y - 0.5f), color_black_outlines);

			const ImVec2 text_size = CalcTextSize(items[i], NULL, true);
			float pad_ = style.FramePadding.x + g->FontSize + style.ItemInnerSpacing.x;
			ImRect tab_rect(Min_cur_label, Max_cur_label);
			RenderTextClipped(Min_cur_label, Max_cur_label, items[i], NULL, &text_size, style.WindowTitleAlign, &tab_rect);
		}

		window->DrawList->AddLine(ImVec2(Min.x, Min.y - 0.5f), ImVec2(Min.x, Max.y), color_black_outlines);
		window->DrawList->AddLine(ImVec2(Min.x, Max.y), Max, color_black_outlines);
		PopClipRect();
	}
}

const char* KeyStrings[] = {
	"",
	"Mouse 1",
	"Mouse 2",
	"Cancel",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

};

PresentFn oPresent;

tReset oResetScene;

void spread_crosshair(IDirect3DDevice9* pDevice)
{
	if (Menu.Visuals.SpreadCrosshair)
	{
		if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
		{
			int r, g, b, a;
			r = Menu.Colors.SpreadCrosshair[0] * 255;
			g = Menu.Colors.SpreadCrosshair[1] * 255;
			b = Menu.Colors.SpreadCrosshair[2] * 255;
			a = Menu.Colors.SpreadCrosshair[3] * 255;
			if (csgo::LocalPlayer && csgo::MainWeapon && csgo::MainWeapon->IsValid())
			{
				float radius = csgo::spread;
				if (csgo::LocalPlayer->GetHealth() > 0 && csgo::LocalPlayer->isAlive())
				{
					if (radius > 0)
						g_pRender->CircleFilledDualColor(csgo::Screen.width / 2, csgo::Screen.height / 2, radius, 0, full, 100, D3DCOLOR_ARGB(a, r, g, b), D3DCOLOR_ARGB(255, 255, 255, 255), pDevice);
				}
			}
		}
	}
}

void GUI_Init(IDirect3DDevice9* pDevice)
{
	ImGui_ImplDX9_Init(csgo::Window, pDevice);

	ImGuiStyle        _style = ImGui::GetStyle();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.71f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.06f, 0.06f, 0.06f, 0.01f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.71f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.66f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.37f, 0.00f, 0.12f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.00f, 0.13f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.30f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.30f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.00f, 0.33f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.10f, 0.10f, 0.90f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.00f, 0.33f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.00f, 0.36f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.00f, 0.30f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	_style.WindowPadding = ImVec2(8, 8);      // Padding within a window
	_style.WindowMinSize = ImVec2(32, 32);    // Minimum window size
	_style.WindowRounding = 9.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
	_style.WindowTitleAlign = ImVec2(5.f, 0.5f);// Alignment for title bar text
	_style.FramePadding = ImVec2(4, 3);      // Padding within a framed rectangle (used by most widgets)
	_style.FrameRounding = 0.0f;             // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
	_style.ItemSpacing = ImVec2(0, 4);      // Horizontal and vertical spacing between widgets/lines
	_style.ItemInnerSpacing = ImVec2(4, 4);      // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
	_style.TouchExtraPadding = ImVec2(0, 0);      // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
	_style.IndentSpacing = 21.0f;            // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
	_style.ColumnsMinSpacing = 6.0f;             // Minimum horizontal spacing between two columns
	_style.ScrollbarSize = 8.0f;            // Width of the vertical scrollbar, Height of the horizontal scrollbar
	_style.ScrollbarRounding = 9.0f;             // Radius of grab corners rounding for scrollbar
	_style.GrabMinSize = 10.0f;            // Minimum width/height of a grab box for slider/scrollbar
	_style.GrabRounding = 0.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
	_style.ButtonTextAlign = ImVec2(0.5f, 0.5f);// Alignment of button text when button is larger than text.
	_style.DisplayWindowPadding = ImVec2(22, 22);    // Window positions are clamped to be visible within the display area by at least this amount. Only covers regular windows.
	_style.DisplaySafeAreaPadding = ImVec2(4, 4);      // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
	_style.AntiAliasedLines = true;             // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
	_style.CurveTessellationTol = 1.25f;            // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
	csgo::Init = true;
}

void draw_menu()
{
	if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
		ImGui::GetIO().MouseDrawCursor = Menu.Gui.Opened;
	else
		ImGui::GetIO().MouseDrawCursor = true;

	ImGui_ImplDX9_NewFrame();

	if (Menu.Gui.Opened)
		csgo::ShowMenu = true;
	else
		csgo::ShowMenu = false;

	if (csgo::ShowMenu)
	{
		static const char* tabs[] = {
			"       ragebot",
			"       legitbot",
			"       visuals",
			"         misc"
		};

		ImGui::SetNextWindowSize(ImVec2(650, 500));
		ImGui::Begin("opaihook", &Menu.Gui.Opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		{
			ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoInputs | ImGuiWindowFlags_ShowBorders | ImGuiColorEditFlags_PickerHueWheel);

			static int page = 0;
			ImGui::SelectTabs(&page, tabs, ARRAYSIZE(tabs));

			ImGui::Spacing();

			switch (page)
			{
			case 0:
			{
				static const char* ragetabs[] = {
					" main",
					" antiaim"
				};

				static int pages = 0;
				ImGui::SelectTabs(&pages, ragetabs, ARRAYSIZE(ragetabs));
				switch (pages)
				{
				case 0:
				{
					ImGui::Checkbox("Enable ragebot", &Menu.Ragebot.EnableAimbot);
					ImGui::Checkbox("Silent", &Menu.Ragebot.SilentAimbot);
					ImGui::Checkbox("No Recoil", &Menu.Ragebot.NoRecoil);
					if (!Menu.Misc.AntiUT) {
						ImGui::Checkbox("No Spread", &Menu.Ragebot.NoSpread);
					}
					ImGui::Checkbox("Auto Revolver", &Menu.Ragebot.AutomaticRevolver);
					ImGui::Checkbox("Revolver Delay", &Menu.Ragebot.NewAutomaticRevolver);
					if (Menu.Ragebot.NewAutomaticRevolver)
						ImGui::SliderInt("Delay Amount", &Menu.Ragebot.NewAutomaticRevolverFactor, 5, 20, "%.0f");
					ImGui::Checkbox("Auto Fire", &Menu.Ragebot.AutomaticFire);
					ImGui::Checkbox("Auto Scope", &Menu.Ragebot.AutomaticScope);
					ImGui::Checkbox("Auto Wall", &Menu.Ragebot.Autowall);
					ImGui::SliderInt("Hitchance", &Menu.Ragebot.Minhitchance, 0, 100, "%.0f%%");
					ImGui::SliderInt("Min damage", &Menu.Ragebot.Mindamage, 0, 100, "%.0f%%");
					ImGui::Checkbox("Backtrack", &Menu.Ragebot.PositionAdjustment);
					if (Menu.Ragebot.PositionAdjustment) {
						static const char* ticks[] = {
							"Last record",
							"All records"
						};
						ImGui::Combo("Type", &Menu.Ragebot.TickType, ticks, ARRAYSIZE(ticks));
					}
					ImGui::Checkbox("Resolver", &Menu.Ragebot.AutomaticResolver);
					if (Menu.Ragebot.AutomaticResolver)
						ImGui::Combo("Override", &Menu.Ragebot.ResolverOverride, KeyStrings, ARRAYSIZE(KeyStrings));
				//	ImGui::Checkbox("Animation Fix (Local Player)", &Menu.Ragebot.AnimFix);
					ImGui::Checkbox("Fake latency", &Menu.Ragebot.FakeLatency);
					if (Menu.Ragebot.FakeLatency) {
						ImGui::SliderFloat("value", &Menu.Ragebot.FakeLatencyAmount, 0.15, 1, "%.2f");
					}

					ImGui::Combo("Priority Hitbox", &Menu.Ragebot.Hitbox, HitboxMode, ARRAYSIZE(HitboxMode));
					ImGui::Combo("Hitscan Type", &Menu.Ragebot.Hitscan, HitscanMode, ARRAYSIZE(HitscanMode));

					ImGui::SliderFloat("Pointscale", &Menu.Ragebot.pointscale, 0.f, 100.f, "%.2f%%");
				}
				break;
				case 1:
				{
					ImGui::Checkbox("Enable###_anti_aim", &Menu.Antiaim.AntiaimEnable);
					if (Menu.Antiaim.AntiaimEnable)
					{
						static const char* aaType[] =
						{
							"Stand",
							"Move",
							"Air"
						};

						static const char* Pitch[] =
						{
							"Off",
							"Emotion",
							"Down",
							"Fake down",
							"Fake up",
							"Zero",
							"Fake Zero"
						};

						static const char* yaw[] =
						{
							"Off",
							"Backward",
							"Backward jitter",
							"Backward spin",
							"Spinbot",
							"Random"
						};

						static const char* direction[] =
						{
							"Off",
							"on binds",
							"freestanding"
						};

						ImGui::Combo("type", &Menu.Antiaim.aatype, aaType, ARRAYSIZE(aaType));

						switch (Menu.Antiaim.aatype)
						{
						case 0:
							ImGui::Combo("pitch (Stand)", &Menu.Antiaim.Stand.pitch, Pitch, ARRAYSIZE(Pitch));
							ImGui::Combo("yaw (Stand)", &Menu.Antiaim.Stand.yaw, yaw, ARRAYSIZE(yaw));
							ImGui::Combo("fake yaw (Stand)", &Menu.Antiaim.Stand.fakeyaw, yaw, ARRAYSIZE(yaw));
							break;
						case 1:
							ImGui::Combo("pitch (Move)", &Menu.Antiaim.Move.pitch, Pitch, ARRAYSIZE(Pitch));
							ImGui::Combo("yaw (Move)", &Menu.Antiaim.Move.yaw, yaw, ARRAYSIZE(yaw));
							ImGui::Combo("fake yaw (Move)", &Menu.Antiaim.Move.fakeyaw, yaw, ARRAYSIZE(yaw));
							break;
						case 2:
							ImGui::Combo("pitch (Air)", &Menu.Antiaim.Air.pitch, Pitch, ARRAYSIZE(Pitch));
							ImGui::Combo("yaw (Air)", &Menu.Antiaim.Air.yaw, yaw, ARRAYSIZE(yaw));
							ImGui::Combo("fake yaw (Air)", &Menu.Antiaim.Air.fakeyaw, yaw, ARRAYSIZE(yaw));
							break;
						}
						
					    ImGui::Combo("angle selection", &Menu.Antiaim.DirectonType, direction, ARRAYSIZE(direction));
						ImGui::Checkbox("LBY indicator", &Menu.Antiaim.Indicator);
						ImGui::Checkbox("AntiAim Lines", &Menu.Antiaim.Lines);
						ImGui::SliderInt("LBY Delta", &Menu.Antiaim.LBYDelta, -180, 180);
						ImGui::SliderInt("Spin Speed", &Menu.Antiaim.SpinSpeed, -50, 50);
						ImGui::SliderInt("Jitter Range", &Menu.Antiaim.JitterRange, -180, 180);
						ImGui::Checkbox("Knife bot", &Menu.Misc.KnifeBot);
						ImGui::Checkbox("Zeus bot", &Menu.Misc.ZeusBot);

						ImGui::Checkbox("Enable Fakelag", &Menu.Misc.FakelagEnable);

						if (Menu.Misc.FakelagEnable) {
							ImGui::SliderInt("Factor ##amount_fakelag", &Menu.Misc.FakelagAmount, 1, 15);

							ImGui::Combo("Mode###fake_lag_mode", &Menu.Misc.FakelagMode, FakelagMode, ARRAYSIZE(FakelagMode));
							ImGui::Checkbox("On Ground", &Menu.Misc.FakelagOnground);
						}
					}
				}
				break;
				}
			}
			break;
			case 1:
			{
				static const char* WeaponArray[] =
				{
					"GLOCK",
					"CZ75",
					"P250",
					"FIVESEVEN",
					"DEAGLE",
					"DUALS",
					"TEC9",
					"P2000",
					"USPS",
					"REVOLVER",
					"MAC10",
					"MP9",
					"MP7",
					"UMP45",
					"BIZON",
					"P90",
					"GALIL",
					"FAMAS",
					"AK47",
					"M4A4",
					"M4A1S",
					"SG553",
					"AUG",
					"SSG08",
					"AWP",
					"G3SG1",
					"SCAR20",
					"NOVA",
					"XM1014",
					"SAWEDOFF",
					"MAG7",
					"M249",
					"NEGEV"
				};
				static const char* SettingType[] = {
					"Weapon in hand",
					"Weapon list"
				};
				static const char* AimbotType[] = {
					"Linear",
					"Curve"
				};
				static const char* BacktrackType[] = {
					"Off",
					"Support",
					"Full"
				};
				static const char* SmoothType[] = {
					"Auto",
					"Factor"
				};
				static const char* DelayType[] = {
					"Off",
					"Auto",
					"Auto + shot"
				};
				int WeaponID = -1;


				ImGui::Checkbox("Enable LegitBot", &Menu.LegitBot.bEnable);
				if (Menu.Ragebot.EnableAimbot)
				{
					ImGui::Text("Turn off ragebot before you turn it on.");
					Menu.LegitBot.bEnable = false;
				}
				ImGui::Combo("Weapon", &Menu.LegitBot.iSettingType, SettingType, IM_ARRAYSIZE(SettingType));

				if (csgo::MainWeapon && !(csgo::MainWeapon->GetWeaponType() == WEPCLASS_KNIFE || csgo::MainWeapon->GetWeaponType() == WEPCLASS_INVALID) && Menu.LegitBot.iSettingType == 0)
					WeaponID = csgo::MainWeapon->GetWeaponNum();
				else if (Menu.LegitBot.iSettingType == 1)
				{
					ImGui::Combo("Select Weapon", &Menu.LegitBot.iCustomIndex, WeaponArray, IM_ARRAYSIZE(WeaponArray));
					WeaponID = Menu.LegitBot.iCustomIndex;
				}

				ImGui::Combo("AimBot style", &Menu.LegitBot.iAimType[WeaponID], AimbotType, IM_ARRAYSIZE(AimbotType));
				ImGui::Combo("Backtrack style", &Menu.LegitBot.iBackTrackType[WeaponID], BacktrackType, IM_ARRAYSIZE(BacktrackType));
				ImGui::Combo("Smooth style", &Menu.LegitBot.iSmoothType[WeaponID], SmoothType, IM_ARRAYSIZE(SmoothType));
				if (Menu.LegitBot.iSmoothType[WeaponID] == 1)
					ImGui::SliderFloat("Smooth amount", &Menu.LegitBot.flSmooth[WeaponID], 0, 1, "%.2fms");
				ImGui::SliderFloat("Fov", &Menu.LegitBot.flFov[WeaponID], 0, 50, "%.1f°");
				ImGui::SliderFloat("Target switch delay", &Menu.LegitBot.flSwitchTargetDelay[WeaponID], 0, 1, "%.2fms");
				ImGui::Combo("Delay type", &Menu.LegitBot.iDelay[WeaponID], DelayType, IM_ARRAYSIZE(DelayType));
			}
			break;
			case 2:
			{
				static const char* visuals[] = {
					" main ",
					" chams",
					" effects"
				};

				static int pagez = 0;
				ImGui::SelectTabs(&pagez, visuals, ARRAYSIZE(visuals));

				switch(pagez)
				{
				case 0:
				{
					ImGui::Checkbox("Enable###esp_active", &Menu.Visuals.EspEnable);
					ImGui::Checkbox("Box", &Menu.Visuals.BoundingBox);
					ImGui::SameLine();
					ImGui::ColorEdit3("###boxcolor", Menu.Colors.BoundingBox);

					ImGui::Checkbox("Out of FOV arrows", &Menu.Visuals.Radar);
					ImGui::SameLine();
					ImGui::ColorEdit3("###radclr", Menu.Colors.Radar);
					ImGui::Checkbox("Skeleton", &Menu.Visuals.Bones);
					ImGui::SameLine();
					ImGui::ColorEdit3("###Skeletonsclr", Menu.Colors.Skeletons);

					ImGui::Checkbox("Ammo", &Menu.Visuals.Ammo);
					ImGui::SameLine();
					ImGui::ColorEdit3("###ammoclr", Menu.Colors.ammo);

					ImGui::Checkbox("Glow", &Menu.Visuals.Glow);
					ImGui::SameLine();
					ImGui::ColorEdit4("###glowclr ", Menu.Colors.Glow, ImGuiColorEditFlags_AlphaBar);

					ImGui::Checkbox("Local Glow", &Menu.Visuals.LGlow);

					ImGui::SameLine();
					ImGui::Checkbox("Pulse", &Menu.Visuals.PulseLGlow);
					ImGui::SameLine();
					ImGui::ColorEdit4("###loclglowclr", Menu.Colors.LGlow, ImGuiColorEditFlags_AlphaBar);


					ImGui::Checkbox("Grenade trajectory", &Menu.Visuals.GrenadePrediction);
					ImGui::SameLine();
					ImGui::ColorEdit3("###prokecileclr ", Menu.Colors.GrenadePrediction);

					ImGui::Checkbox("Name", &Menu.Visuals.Name);
					ImGui::SameLine();
					ImGui::ColorEdit3("###namecc", Menu.Colors.Name);
					ImGui::Checkbox("Health", &Menu.Visuals.Health);
					ImGui::Checkbox("Weapon", &Menu.Visuals.Weapon);
					ImGui::SameLine();
					ImGui::ColorEdit3("###weaponc", Menu.Colors.wpn);

					ImGui::Checkbox("Flags", &Menu.Visuals.Flags);
					ImGui::SameLine();
					if (Menu.Visuals.Flags) {
						ImGui::SameLine();
						ImGui::Checkbox("Armor", &Menu.Visuals.Armor);
						ImGui::SameLine();
						ImGui::Checkbox("Scoped", &Menu.Visuals.Scoped);
					}
					ImGui::SameLine();
					ImGui::ColorEdit3("###infosfasf", Menu.Colors.f);

					ImGui::Checkbox("Hitmarker", &Menu.Visuals.Hitmarker);
					if (Menu.Visuals.Hitmarker) {
						static const char* hitsound[] =
						{
							"default",
							"bameware",
							"click",
							"pop up",
							"gamesense",
							"stapler",
							"pop up 2"
						};
						ImGui::Combo("Sound", &Menu.Visuals.htSound, hitsound, IM_ARRAYSIZE(hitsound));
						ImGui::SliderInt("Size###hittt", &Menu.Visuals.hitmarkerSize, 2, 10, "%.0f%%");
					}

					ImGui::Text("Thirdperson");
					ImGui::Combo("Key", &Menu.Misc.TPKey, KeyStrings, IM_ARRAYSIZE(KeyStrings));
					ImGui::SliderInt("Distance", &Menu.Visuals.thirdperson_dist, 0, 500, "%.0f%");
					ImGui::Combo("Show angles", &Menu.Misc.TPangles, ThirdpersonAngles, IM_ARRAYSIZE(ThirdpersonAngles));
				}
				break;
				case 1:
				{
					ImGui::Checkbox("Enable###chamz", &Menu.Visuals.ChamsEnable);

					ImGui::Combo("Type", &Menu.Visuals.ChamsStyle, ModelsMode, IM_ARRAYSIZE(ModelsMode));

					ImGui::Checkbox("Player", &Menu.Visuals.ChamsPlayer);
					ImGui::SameLine();
					ImGui::ColorEdit3("###playerclr", Menu.Colors.PlayerChams);

					ImGui::Checkbox("Player XQZ", &Menu.Visuals.ChamsPlayerWall);
					ImGui::SameLine();
					ImGui::ColorEdit3("###playerxqzclr", Menu.Colors.PlayerChamsWall);

					ImGui::Checkbox("Local Player##ofodofod", &Menu.Visuals.ChamsL);
					ImGui::SameLine();
					ImGui::ColorEdit3("###localclr111rrr", Menu.Colors.PlayerChamsl);

					ImGui::Checkbox("Show backtrack", &Menu.Visuals.ShowBacktrack);
					ImGui::SameLine();
					ImGui::ColorEdit3("###backtrackchams ", Menu.Colors.ChamsHistory);

					ImGui::Checkbox("Styles hands", &Menu.Misc.WireHand);
					ImGui::SameLine();
					ImGui::ColorEdit3("###styleszzxazsd ", Menu.Colors.styleshands);

					ImGui::SliderInt("Local Alpha", &Menu.Visuals.playeralpha, 0, 255, "%.0f%%");
				}
				break;
				case 2:
				{
					ImGui::Combo("Skybox", &Menu.Visuals.Skybox, Skyboxmode, IM_ARRAYSIZE(Skyboxmode));
					ImGui::Checkbox("Spread Crosshair", &Menu.Visuals.SpreadCrosshair);
					ImGui::SameLine();
					ImGui::ColorEdit4("###spreadcolr ", Menu.Colors.SpreadCrosshair, ImGuiColorEditFlags_AlphaBar);

					ImGui::Checkbox("Force Crosshair", &Menu.Visuals.ForceCrosshair);
					ImGui::Checkbox("Nightmode", &Menu.Visuals.nightmode);
					ImGui::Checkbox("Blend props", &Menu.Colors.Props);
					ImGui::Checkbox("Blend model on scope", &Menu.Visuals.blendonscope);
					ImGui::Checkbox("Remove smoke", &Menu.Visuals.Nosmoke);
					ImGui::Checkbox("Remove flash", &Menu.Visuals.NoFlash);
					ImGui::Checkbox("Remove visual recoil", &Menu.Visuals.Novisrevoil);
					ImGui::Checkbox("Remove scope", &Menu.Visuals.Noscope);
					ImGui::Checkbox("Remove zoom", &Menu.Misc.static_scope);
					ImGui::Checkbox("Remove post processing", &Menu.Visuals.RemoveParticles);
				}
				break;
				}
			}
			break;
			case 3:
			{
				static const char* miscz[] = {
					" main  ",
					" cfg"
				};

				static int pagezz = 0;
				ImGui::SelectTabs(&pagezz, miscz, ARRAYSIZE(miscz));

				switch (pagezz)
				{
				case 0:
				{
					ImGui::Checkbox("Anti untrusted", &Menu.Misc.AntiUT);
					ImGui::Checkbox("Bunny hop", &Menu.Misc.AutoJump);
					ImGui::Checkbox("Auto strafe", &Menu.Misc.AutoStrafe);
					ImGui::Combo("Circle Strafe", &Menu.Misc.circlestrafekey, KeyStrings, ARRAYSIZE(KeyStrings));

					ImGui::SliderInt("Player Viewmodel", &Menu.Misc.PlayerViewmodel, -120, 120, "%.0f%%");
					ImGui::SliderInt("Player FOV", &Menu.Misc.PlayerFOV, -50, 50, "%.0f%%");

					static const char* bullet[] = {
						"Beam",
						"Line and box"
					};

					ImGui::Checkbox("Enable Bullet tracer", &Menu.Visuals.BulletTracers);
					ImGui::Combo("Type###bullettracer", &Menu.Visuals.bulletType, bullet, IM_ARRAYSIZE(bullet));
					ImGui::ColorEdit3("Color", Menu.Colors.Bulletracer);
					if (Menu.Visuals.bulletType == 1) {
						ImGui::SameLine();
						ImGui::ColorEdit4("Box Color##bullet", Menu.Colors.BulletracerBox, ImGuiColorEditFlags_AlphaBar);
					}
				}
				break;
				case 1:
				{
					using namespace ImGui;

					InputText("##CFG", Menu.Misc.configname, 128);
					static int sel;
					std::string config;
					std::vector<std::string> configs = Menu.GetConfigs();
					if (configs.size() > 0) {
						ComboBoxArray("configs", &sel, configs);
						Spacing();
						Separator();
						Spacing();
						config = configs[Menu.Misc.ConfigSelection];
					}
					Menu.Misc.ConfigSelection = sel;

					if (configs.size() > 0) {
						if (Button("load cfg", ImVec2(100, 20)))
						{
							Menu.Load(config);
						}
					}
					SameLine();

					if (configs.size() >= 1) {
						if (Button("save cfg", ImVec2(100, 20)))
						{
							Menu.Save(config);
						}
					}
					SameLine();
					if (Button("new cfg", ImVec2(100, 20)))
					{
						std::string ConfigFileName = Menu.Misc.configname;
						if (ConfigFileName.size() < 1)
						{
							ConfigFileName = "null";
						}
						Menu.CreateConfig(ConfigFileName);
					}
				}
				break;
				}
				
			}
			break;
			}
		}
		ImGui::End();
	}
}

HRESULT __stdcall Hooks::D3D9_EndScene(IDirect3DDevice9* pDevice)
{
	HRESULT result = d3d9VMT->GetOriginalMethod<EndSceneFn>(42)(pDevice);
	
	if (!csgo::Init)
		GUI_Init(pDevice);

	static void* dwReturnAddress = _ReturnAddress();

	if (dwReturnAddress == _ReturnAddress())
	{
		SaveState(pDevice);

		spread_crosshair(pDevice);
		draw_menu();
		ImGui::Render();

		RestoreState(pDevice);
	}
	return result;
}


HRESULT __stdcall Hooks::hkdReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresParam)
{
	if (!csgo::Init)
		return oResetScene(pDevice, pPresParam);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	auto hr = oResetScene(pDevice, pPresParam);

	ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}