
#include "windows.h"
#include <iostream>
#include <dwmapi.h>
#include <thread>
#include <d3d11.h>
#include "globals.h"
#include <format>
#include "io.h"
#include "winbase.h"
#include "ImGui/imgui.h"
#include "memory.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "vectorh.h"
#include "hacks.h"


bool menu_open;


constexpr Vector3 CalculateAngle(
	const Vector3& localPosition,
	const Vector3& enemyPosition,
	const Vector3& viewAngles) noexcept
{
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}

struct Vector {
	Vector() noexcept
		: x(), y(), z() {}

	Vector(float x, float y, float z) noexcept
		: x(x), y(y), z(z) {}

	Vector& operator+(const Vector& v) noexcept {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector& operator-(const Vector& v) noexcept {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	float x, y, z;
};
struct ViewMatrix {
	ViewMatrix() noexcept
		: data() {}

	float* operator[](int index) noexcept {
		return data[index];
	}

	const float* operator[](int index) const noexcept {
		return data[index];
	}

	float data[4][4];
};

static bool world_to_screen(const Vector& world, Vector& screen, const ViewMatrix& vm) noexcept {
	float w = vm[3][0] * world.x + vm[3][1] * world.y + vm[3][2] * world.z + vm[3][3];

	if (w < 0.001f) {
		return false;
	}

	const float x = world.x * vm[0][0] + world.y * vm[0][1] + world.z * vm[0][2] + vm[0][3];
	const float y = world.x * vm[1][0] + world.y * vm[1][1] + world.z * vm[1][2] + vm[1][3];

	w = 1.f / w;
	float nx = x * w;
	float ny = y * w;

	const ImVec2 size = ImGui::GetIO().DisplaySize;

	screen.x = (size.x * 0.5f * nx) + (nx + size.x * 0.5f);
	screen.y = -(size.y * 0.5f * ny) + (ny + size.y * 0.5f);
	return true;
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
		return 1L;
	}
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}
	return DefWindowProc(window, message, w_param, l_param);
}
void create_directx(HWND window) {

}

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {
	//allocate this program a console
	if (!AllocConsole()) {
		return FALSE;
	};

	//redirect IO
	FILE* file{ nullptr };
	freopen_s(&file, "CONIN$", "r", stdout);
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stdout);

	// initialize memory class
	

	//wait for game
	DWORD pid = 0;
	if (!pid) {
		std::cout << std::format("waiting for game \n");

		do {

			pid = GetProcessId(mem.GetHandle());
			Sleep(200UL);

		} while (!pid);


	}

	std::cout << std::format("game found. process ID = {}\n", pid);
	// get module addresses
	const auto client = mem.GetModuleAddress("client.dll");
	const auto engine = mem.GetModuleAddress("engine.dll");

	if (!client) {
		std::cout << "Failed to get game module.\n";
		FreeConsole();
		return FALSE;
	}
	std::cout << std::format("Client -> {:#x}\n", client);

	if (!FreeConsole()) {
		return FALSE;
	}

	if (!FreeConsole()) {
		return FALSE;
	}

	const HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, client);

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW,
		wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"External Overlay Class";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		wc.lpszClassName,
		L"Frenchy csgo",
		WS_POPUP,
		0,
		0,
		1920,
		1080,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};
		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferDesc.Width = 0U;
	sd.BufferDesc.Height = 0U;
	constexpr D3D_FEATURE_LEVEL levels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer) {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else {
		return 1;
	}

	HWND menuhandle = FindWindow("Valve001", NULL);

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();


	ImGui::StyleColorsDark();



	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);


	bool running = true;
	while (running) {

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				running = false;

			}
		}

		if (!running) {
			break;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(menuhandle, &rc);
		ClientToScreen(menuhandle, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		io.ImeWindowHandle = menuhandle;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;


		if (GetAsyncKeyState(VK_INSERT) & 1)
			menu_open ^= 1;

		if (menu_open) {
			ImGui::GetStyle();

			ImGui::SetNextWindowSize({ 280, 290 });

			ImGui::Begin(
				"classified",
				&menu_open,
				ImGuiWindowFlags_NoSavedSettings

			);

			ImGui::Checkbox("glow", &globals::glow);
			ImGui::ColorEdit4("TeamGlowColor", globals::TeamGlowColor);;
			ImGui::Separator();


			ImGui::ColorEdit4("EnemyGLowColor", globals::EnemyGlowColor);;
			ImGui::Separator();

			ImGui::Checkbox("radar", &globals::radar);

			ImGui::Checkbox("bhop", &globals::Bhop);
			ImGui::Checkbox("no Recoil", &globals::NoRecoil);
			ImGui::Checkbox("triggerbot", &globals::Triggerbot);
			ImGui::Checkbox("fovchanger", &globals::fovchanger);
			ImGui::Checkbox("aimbot", &globals::Aimbot);
			ImGui::Checkbox("chams", &globals::chams);
			ImGui::Checkbox("esp", &globals::playeresp);
			ImGui::Checkbox("Fov", &globals::Fov);
			ImGui::End();
		}




		if (globals::Fov) {
			ImGui::GetBackgroundDrawList()->AddCircle({ 960, 540 }, 30.f, ImColor(0.f, 0.f, 1.f)); //fov
		}
		const auto LocalPlayer = mem.Read<std::uintptr_t>(client + offsets::Local_Player);
		if (LocalPlayer)
		{
			hack::hacks();
			const auto glowManager = mem.Read<std::uintptr_t>(client + offsets::GlowObjectManager);

			if (!glowManager)
				continue;



			const auto local_team = mem.Read<std::int32_t>(LocalPlayer + offsets::m_IteamNum);

			const auto view_matrix = mem.Read<ViewMatrix>(client + offsets::View_Matrix);


			for (int i = 1; i < 32; ++i) {
				const auto player = mem.Read<std::int32_t>(client + offsets::EntityList + i * 0x10);
				if (!player)
					continue;

				const auto glowManager = mem.Read<std::uintptr_t>(client + offsets::GlowObjectManager);

				if (!glowManager)
					continue;

				const auto localTeam = mem.Read<std::int32_t>(LocalPlayer + offsets::m_IteamNum);

				if (!localTeam)
					continue;


				const auto lifeState = mem.Read<std::int32_t>(player + offsets::m_lifeState);

				if (lifeState != 0)
					continue;

				const auto glowIndex = mem.Read<std::int32_t>(player + offsets::m_iGlowIndex);


				const auto team = mem.Read<std::int32_t>(player + offsets::m_IteamNum);
				if (!team)
					continue;
				if (globals::Bhop) {

					auto on_ground = mem.Read<bool>(LocalPlayer + offsets::m_fFlags);

					if (GetAsyncKeyState(VK_SPACE) && on_ground & (1 << 0))
						mem.Write<BYTE>(client + offsets::ForceJump, 6);
				}

				if (globals::glow) //glow
				{
					if (team == localTeam)
					{
						mem.Write(glowManager + (glowIndex * 0x38) + 0x8, globals::TeamGlowColor[0]);//red  team
						mem.Write(glowManager + (glowIndex * 0x38) + 0xc, globals::TeamGlowColor[1]);//green
						mem.Write(glowManager + (glowIndex * 0x38) + 0x10, globals::TeamGlowColor[2]);//blue
						mem.Write(glowManager + (glowIndex * 0x38) + 0x14, globals::TeamGlowColor[3]);//alpha big cock

					}
					else {
						mem.Write(glowManager + (glowIndex * 0x38) + 0x8, globals::EnemyGlowColor[0]);//red
						mem.Write(glowManager + (glowIndex * 0x38) + 0xc, globals::EnemyGlowColor[1]);//green    enemies
						mem.Write(glowManager + (glowIndex * 0x38) + 0x10, globals::EnemyGlowColor[2]);//blue
						mem.Write(glowManager + (glowIndex * 0x38) + 0x14, globals::EnemyGlowColor[3]);//alpha big cock


					}
					mem.Write(glowManager + (glowIndex * 0x38) + 0x28, true);
					mem.Write(glowManager + (glowIndex * 0x38) + 0x29, false);

				}
				if (globals::chams)
				{
				
					struct Color
					{
						std::uint8_t r, g, b;
					};

					mem.Write<Color>(player + offsets::m_clrRender, team == localTeam ? Color{ 0, 255, 0 } : Color{ 255, 0, 0 });
				}

				if (mem.Read<bool>(player + offsets::m_bDormant)) {
					continue;
				}

				if (mem.Read<uintptr_t>(player + offsets::m_IteamNum) == local_team) {

					continue;
				}

				if (mem.Read<int32_t>(player + offsets::m_lifeState) != 0) {
					continue;
				}

				const auto bones = mem.Read<DWORD>(player + offsets::m_BoneMatrix);

				if (!bones) {
					continue;
				}

				Vector head_pos{
					mem.Read<float>(bones + 0x30 * 8 + 0x0c),
					mem.Read<float>(bones + 0x30 * 8 + 0x1c),
					mem.Read<float>(bones + 0x30 * 8 + 0x2c),
				};

				auto feet_pos = mem.Read<Vector>(player + offsets::m_vecOrigin);

				Vector top;
				Vector bottom;
				if (world_to_screen(head_pos + Vector{ 0, 0,11.f }, top, view_matrix) && world_to_screen(feet_pos + Vector{ 0, 0,11.f }, bottom, view_matrix)) { //changed this changed boxed
					const float h = bottom.y - top.y;
					const float w = h * 0.35f;

					if (globals::playeresp) { //esp and name ect.. 
						ImGui::GetBackgroundDrawList()->AddRect({ top.x - w, top.y }, { top.x + w, bottom.y }, ImColor(255, 0, 0));
						ImGui::GetBackgroundDrawList()->AddText({ top.x - (w / 2), top.y - 10 }, ImColor(255, 255, 255), "Penis");
						
					}

				}

				if (globals::radar)
				{

					mem.Write(player + offsets::m_bSpotted, true);
				}



				if (globals::fovchanger) {
				pseudo: mem.Write<std::uintptr_t>(LocalPlayer + offsets::m_iFOV, 120);

				}

				//brightness
				const auto _this = static_cast<std::uintptr_t>(engine + offsets::model_ambient_min - 0x2c);
				mem.Write<std::int32_t>(engine + offsets::model_ambient_min, 400ULL ^ _this);



				if (globals::Triggerbot && GetAsyncKeyState(VK_SHIFT))
				{
					const auto crosshairId = mem.Read<std::int32_t>(LocalPlayer + offsets::m_iCrosshairId);

					if (!crosshairId || crosshairId > 64)
						continue;
					//gets the player entity then minuses 1 from crosshair id to get the actual Index of player
					const auto player = mem.Read<std::uintptr_t>(client + offsets::EntityList + (crosshairId - 1) * 0x10);

					//if playe is ded
					if (!mem.Read<std::int32_t>(player + offsets::m_iHealth))
						continue;

					//skip player if hes on our team
					if (mem.Read<std::int32_t>(player + offsets::m_IteamNum) !=
						localTeam)

					{
						mem.Write<std::int32_t>(client + offsets::ForceAttack, 6);
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						mem.Write<std::uintptr_t>(client + offsets::ForceAttack, 4);
					}
				}


				if (globals::Aimbot) {

					if (!GetAsyncKeyState(VK_RBUTTON))

						continue;
					// get local player
					const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::Local_Player);
					const auto localTeam = mem.Read<std::int32_t>(localPlayer + offsets::m_IteamNum);

					// eye position = origin + viewOffset
					const auto localEyePosition = mem.Read<Vector3>(localPlayer + offsets::m_vecOrigin) +
						mem.Read<Vector3>(localPlayer + offsets::m_vecViewOffset);

					const auto clientState = mem.Read<std::uintptr_t>(engine + offsets::dwClientState);

					const auto localPlayerId =
						mem.Read<std::int32_t>(clientState + offsets::ClientState_GetLocalPlayer);

					const auto viewAngles = mem.Read<Vector3>(clientState + offsets::ClientState_ViewAngles);
					const auto aimPunch = mem.Read<Vector3>(localPlayer + offsets::aimPunchAngle) * 2;

					// aimbot fov
					auto FovSize = 3.f;
					auto bestAngle = Vector3{ };
					const auto player = mem.Read<std::uintptr_t>(client + offsets::EntityList + i * 0x10);

					if (mem.Read<std::int32_t>(player + offsets::m_IteamNum) == localTeam)
						continue;

					if (mem.Read<bool>(player + offsets::m_bDormant))
						continue;

					if (mem.Read<std::int32_t>(player + offsets::m_lifeState))
						continue;

					if (mem.Read<std::int32_t>(player + offsets::m_bSpottedByMask) & (1 << localPlayerId))
					{
						const auto boneMatrix = mem.Read<std::uintptr_t>(player + offsets::m_BoneMatrix);

						// pos of player head in 3d space
						// 8 is the head bone index :)
						const auto playerHeadPosition = Vector3{
						  mem.Read<float>(boneMatrix + 0x30 * 8 + 0x0C),
						  mem.Read<float>(boneMatrix + 0x30 * 8 + 0x1C),
						  mem.Read<float>(boneMatrix + 0x30 * 8 + 0x2C)
						};

						const auto angle = CalculateAngle(
							localEyePosition,
							playerHeadPosition,
							viewAngles + aimPunch
						);

						const auto fov = std::hypot(angle.x, angle.y);

						if (fov < FovSize)
						{
							FovSize = fov;
							bestAngle = angle;
						}
					}


					// if we have a best angle, do aimbot
					if (!bestAngle.IsZero())

						mem.Write<Vector3>(clientState + offsets::ClientState_ViewAngles, viewAngles + bestAngle / 5.f); // smoothing
				}



			}


		}

		ImGui::Render();

		constexpr float clear_color[4] = { 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, clear_color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(0U, 0U);

	}
	void ImGui_ImplDX11_Shutdown();
	void ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	if (swap_chain) {
		swap_chain->Release();
	}

	if (device_context) {
		device_context->Release();
	}


	if (device) {
		device->Release();
	}

	if (render_target_view) {
		render_target_view->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);



}