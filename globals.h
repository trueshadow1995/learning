#pragma once
#include "cstdint"
#include "cstddef"
#include <d3d11.h>




namespace globals
{
	inline std::uintptr_t clientAddress = 0;
	inline std::uintptr_t engine = 0;
	inline bool glow = false;
	inline float TeamGlowColor[]{ 0.f, 1.f, 0.f, 1.f };

	inline float EnemyGlowColor[]{ 1.f, 0.f, 0.f, 1.f };
	inline bool playeresp = false;
	inline bool radar = false;
	inline bool Bhop = false;
	inline bool Triggerbot = false;

	inline bool Fov = false;

	inline bool Aimbot = false;
	inline bool fovchanger = false;
	inline bool NoRecoil = false;
	inline bool chams = false;
	inline float FriendChamColor[]{ 0.f, 1.f, 0.f, 1.f };
	inline float EnemyChamColor[]{ 1.f, 0.f, 0.f, 1.f };
};

namespace offsets
{
	constexpr auto m_iFOV = 0x31F4;
	constexpr auto m_iShotsFired = 0x103E0;
	constexpr auto View_Matrix = 0x4DEFD54;
	constexpr auto Local_Player = 0xDE997C;
	constexpr auto GlowObjectManager = 0x5359988;
	constexpr auto EntityList = 0x4DFEF0C;
	constexpr auto ForceJump = 0x52BAC68;
	constexpr auto ForceAttack = 0x322CD48;;
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
	constexpr auto m_bSpottedByMask = 0x980;
	constexpr auto m_vecViewOffset = 0x108;
	constexpr auto m_aimPunchAngleVel = 0x3048;
	constexpr auto aimPunchAngle = 0x303C;
	constexpr auto dwClientState = 0x59F19C;
	constexpr auto ClientState_GetLocalPlayer = 0x180;
	constexpr auto ClientState_ViewAngles = 0x4D90;
	constexpr auto m_viewPunchAngle = 0x3030;
	constexpr auto dwClientState_PlayerInfo = 0x52C0;

}
