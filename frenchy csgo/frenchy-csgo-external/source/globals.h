#pragma once
#include "cstdint"
#include "cstddef"

namespace globals
{
	inline std::uintptr_t clientAddress = 0;
	inline std::uintptr_t engine = 0;
	inline bool glow = false;
	inline float glowColor[]{ 1.f, 0.f, 0.f, 1.f };

	inline bool fglow = false;
	inline float fglowColor[]{ 1.f, 0.f, 0.f, 1.f };
	inline bool playeresp = false;
	inline bool radar = false;
	inline bool Bhop = false;
	inline bool Triggerbot = false;
	inline bool chams = false;
	inline bool Fov = false;
	/*inline float chamsColor[]{ 1.f, 0.f, 0.f, 1.f };*/

}

namespace offsets
{
	constexpr auto View_Matrix = 0x4DEFD14;
	constexpr auto Local_Player = 0xDE997C;
	constexpr auto GlowObjectManager = 0x5359980;
	constexpr auto EntityList = 0x4DFEECC;
	constexpr auto ForceJump = 0x52BAC28;
	constexpr auto ForceAttack = 0x322CD38;
	constexpr auto m_clrRender = 0x70;
	constexpr auto model_ambient_min = 0x5A1194;
	constexpr auto m_IteamNum = 0xF4;
	constexpr auto m_lifeState = 0x25F;
	constexpr auto m_iGlowIndex = 0x10488;
	constexpr auto m_bSpotted = 0x93D;
	constexpr auto m_iHealth = 0x100;
	constexpr auto m_iCrosshairId = 0x11838;
	constexpr auto m_BoneMatrix = 0x26A8;
	constexpr auto m_vecOrigin = 0x138;
    constexpr auto m_bDormant = 0xED;
	constexpr auto m_fFlags = 0x104;
}
