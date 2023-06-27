
#include "hacks.h"
#include "vector.h"

constexpr Vector3 CalculateAngle(const Vector3& localPosition,
                                 const Vector3& enemyPosition,
                                 const Vector3& viewAngles) noexcept {
  return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}

void hack::Aimbot() {
  const auto client = mem.GetModuleAddress("client.dll");
  const auto engine = mem.GetModuleAddress("engine.dll");
  const auto& LocalPlayer =
      mem.Read<std::uintptr_t>(client + offsets::Local_Player);

  for (int i = 1; i < 32; ++i) {
    const auto player =
        mem.Read<std::int32_t>(client + offsets::EntityList + i * 0x10);
    if (!player) continue;

    const auto localTeam =
        mem.Read<std::int32_t>(LocalPlayer + offsets::m_IteamNum);

    const auto team = mem.Read<std::int32_t>(player + offsets::m_IteamNum);
    if (!team) continue;

    if (globals::Aimbot) {
      if (!GetAsyncKeyState(VK_SHIFT)) continue;
      // get local player
      const auto localPlayer =
          mem.Read<std::uintptr_t>(client + offsets::Local_Player);
      const auto localTeam =
          mem.Read<std::int32_t>(localPlayer + offsets::m_IteamNum);

      // eye position = origin + viewOffset
      const auto localEyePosition =
          mem.Read<Vector3>(localPlayer + offsets::m_vecOrigin) +
          mem.Read<Vector3>(localPlayer + offsets::m_vecViewOffset);

      const auto clientState =
          mem.Read<std::uintptr_t>(engine + offsets::dwClientState);

      const auto localPlayerId = mem.Read<std::int32_t>(
          clientState + offsets::ClientState_GetLocalPlayer);

      const auto viewAngles =
          mem.Read<Vector3>(clientState + offsets::ClientState_ViewAngles);
      const auto aimPunch =
          mem.Read<Vector3>(localPlayer + offsets::aimPunchAngle) * 2;

      // aimbot fov
      auto FovSize = (globals::AimbotFovSize * 3.141592);

      auto bestAngle = Vector3{};

      const auto player =
          mem.Read<std::uintptr_t>(client + offsets::EntityList + i * 0x10);

      if (mem.Read<std::int32_t>(player + offsets::m_IteamNum) == localTeam)
        continue;

      if (mem.Read<bool>(player + offsets::m_bDormant)) continue;

      if (mem.Read<std::int32_t>(player + offsets::m_lifeState)) continue;

      if (mem.Read<std::int32_t>(player + offsets::m_bSpottedByMask) &
          (1 << localPlayerId)) {
        const auto boneMatrix =
            mem.Read<std::uintptr_t>(player + offsets::m_BoneMatrix);

        // pos of player head in 3d space
        // 8 is the head bone index :)
        const auto playerHeadPosition =
            Vector3{mem.Read<float>(boneMatrix + 0x30 * 8 + 0x0C),
                    mem.Read<float>(boneMatrix + 0x30 * 8 + 0x1C),
                    mem.Read<float>(boneMatrix + 0x30 * 8 + 0x2C)};

        const auto angle = CalculateAngle(localEyePosition, playerHeadPosition,
                                          viewAngles + aimPunch);

        const auto fov = std::hypot(angle.x, angle.y);

        if (fov < FovSize) {
          FovSize = fov;
          bestAngle = angle;
        }
      }
      if (globals::Aimbotsmoothing) {
        // if we have a best angle, do aimbot
        if (!bestAngle.IsZero())

          mem.Write<Vector3>(clientState + offsets::ClientState_ViewAngles,
                             viewAngles + bestAngle / 5.f);  // smoothing
      };
    }
  }
}
