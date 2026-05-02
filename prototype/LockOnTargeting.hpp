#pragma once

#include <limits>
#include <vector>

#include "EnemyCombatAI.hpp"

namespace gameplay
{
struct LockOnTargeting
{
  bool bIsLockedOn = false;
  EnemyCombatAI* LockedTarget = nullptr;

  float MaxLockDistance = 1200.0f;
  float FieldOfViewDegrees = 90.0f;
  float TargetSwitchRadius = 800.0f;

  EventDispatcher OnLockOnStarted;
  EventDispatcher OnLockOnEnded;
  EventDispatcher OnTargetSwitched;

  void ToggleLockOn(const std::vector<EnemyCombatAI*>& candidates,
                    const std::vector<float>& candidateDistances,
                    const std::vector<float>& candidateAnglesDeg);
  void SwitchTarget(int direction,
                    const std::vector<EnemyCombatAI*>& candidates,
                    const std::vector<float>& candidateDistances);
  void Tick(float currentTimeSeconds, float targetDistance, bool bTargetAlive);

  float GetDesiredCameraYaw(float currentYaw, float targetYaw) const;
  bool ShouldStrafeMovement() const;

private:
  EnemyCombatAI* SelectBestTarget(const std::vector<EnemyCombatAI*>& candidates,
                                 const std::vector<float>& candidateDistances,
                                 const std::vector<float>& candidateAnglesDeg) const;
  void ClearLock();
  bool IsInFront(float angleDeg) const;
};
} // namespace gameplay
