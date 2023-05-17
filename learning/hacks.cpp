#include "hacks.h"

#include <dwmapi.h>

#include <format>

#include "globals.h"
#include "vectorh.h"

struct vect_2d {
  float x = {}, y = {};
};

void hack::hacks() {
  const auto client = mem.GetModuleAddress("client.dll");
  const auto engine = mem.GetModuleAddress("engine.dll");
  const auto& LocalPlayer =
      mem.Read<std::uintptr_t>(client + offsets::Local_Player);

  for (int i = 1; i < 32; ++i) {
    const auto player =
        mem.Read<std::int32_t>(client + offsets::EntityList + i * 0x10);
    if (!player) continue;
    if (globals::NoRecoil) {
      auto oldyPunch = vect_2d{};

      const auto& shotsFired =
          mem.Read<std::int32_t>(LocalPlayer + offsets::m_iShotsFired);

      if (shotsFired) {
        const auto& clientState =
            mem.Read<std::uintptr_t>(engine + offsets::dwClientState);
        const auto& viewAngles =
            mem.Read<vect_2d>(clientState + offsets::ClientState_ViewAngles);

        const auto& aimPunch =
            mem.Read<vect_2d>(LocalPlayer + offsets::aimPunchAngle);

        auto newAngles = vect_2d{viewAngles.x + oldyPunch.x - aimPunch.x * 2.f,
                                 viewAngles.y + oldyPunch.y - aimPunch.y * 2.f};

        if (newAngles.x > 89.f) newAngles.x = 89.f;

        if (newAngles.x < -89.f) newAngles.x = -89.f;

        while (newAngles.y > 180.f) newAngles.y -= 360.f;

        while (newAngles.y < -180.f) newAngles.y += 360.f;

        mem.Write<vect_2d>(clientState + offsets::ClientState_ViewAngles,
                           newAngles);

        oldyPunch.x = aimPunch.x * 2.f;
        oldyPunch.y = aimPunch.y * 2.f;

        } else
        oldyPunch.x = oldyPunch.y = 0.f;

      
      }
        
    }

    if (globals::fovchanger) {
    seudo:
      mem.Write<std::uintptr_t>(LocalPlayer + offsets::m_iFOV,
                                globals::Fovamount);
    }

    if (globals::Bhop) {
      auto on_ground = mem.Read<bool>(LocalPlayer + offsets::m_fFlags);

      if (GetAsyncKeyState(VK_SPACE) && on_ground & (1 << 0))
        mem.Write<BYTE>(client + offsets::ForceJump, 6);
    }
  }