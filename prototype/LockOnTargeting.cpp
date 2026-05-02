#include "LockOnTargeting.hpp"

namespace gameplay
{
void LockOnTargeting::ToggleLockOn(const std::vector<EnemyCombatAI*>& candidates,
                                  const std::vector<float>& candidateDistances,
                                  const std::vector<float>& candidateAnglesDeg)
{
  if (bIsLockedOn)
  {
    ClearLock();
    return;
  }

  LockedTarget = SelectBestTarget(candidates, candidateDistances, candidateAnglesDeg);
  bIsLockedOn = LockedTarget != nullptr;
  if (bIsLockedOn)
  {
    OnLockOnStarted.Broadcast();
  }
}

void LockOnTargeting::SwitchTarget(int direction,
                                  const std::vector<EnemyCombatAI*>& candidates,
                                  const std::vector<float>& candidateDistances)
{
  if (!bIsLockedOn || candidates.empty())
  {
    return;
  }

  std::vector<std::pair<float, EnemyCombatAI*>> sorted;
  sorted.reserve(candidates.size());

  for (size_t index = 0; index < candidates.size(); ++index)
  {
    if (!candidates[index])
    {
      continue;
    }

    if (candidateDistances[index] > TargetSwitchRadius)
    {
      continue;
    }

    sorted.emplace_back(candidateDistances[index], candidates[index]);
  }

  if (sorted.empty())
  {
    return;
  }

  std::sort(sorted.begin(), sorted.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.first < rhs.first;
  });

  int currentIndex = -1;
  for (size_t index = 0; index < sorted.size(); ++index)
  {
    if (sorted[index].second == LockedTarget)
    {
      currentIndex = static_cast<int>(index);
      break;
    }
  }

  int nextIndex = 0;
  if (currentIndex != -1)
  {
    int step = direction >= 0 ? 1 : -1;
    nextIndex = (currentIndex + step + static_cast<int>(sorted.size())) % static_cast<int>(sorted.size());
  }

  LockedTarget = sorted[nextIndex].second;
  OnTargetSwitched.Broadcast();
}

void LockOnTargeting::Tick(float currentTimeSeconds, float targetDistance, bool bTargetAlive)
{
  (void)currentTimeSeconds;

  if (!bIsLockedOn)
  {
    return;
  }

  if (!LockedTarget || !bTargetAlive || targetDistance > MaxLockDistance)
  {
    ClearLock();
  }
}

float LockOnTargeting::GetDesiredCameraYaw(float currentYaw, float targetYaw) const
{
  if (!bIsLockedOn)
  {
    return currentYaw;
  }

  return targetYaw;
}

bool LockOnTargeting::ShouldStrafeMovement() const
{
  return bIsLockedOn;
}

EnemyCombatAI* LockOnTargeting::SelectBestTarget(const std::vector<EnemyCombatAI*>& candidates,
                                                 const std::vector<float>& candidateDistances,
                                                 const std::vector<float>& candidateAnglesDeg) const
{
  float bestDistance = std::numeric_limits<float>::max();
  EnemyCombatAI* bestTarget = nullptr;

  for (size_t index = 0; index < candidates.size(); ++index)
  {
    EnemyCombatAI* candidate = candidates[index];
    if (!candidate)
    {
      continue;
    }

    if (!IsInFront(candidateAnglesDeg[index]))
    {
      continue;
    }

    float distance = candidateDistances[index];
    if (distance > MaxLockDistance)
    {
      continue;
    }

    if (distance < bestDistance)
    {
      bestDistance = distance;
      bestTarget = candidate;
    }
  }

  return bestTarget;
}

void LockOnTargeting::ClearLock()
{
  if (!bIsLockedOn)
  {
    return;
  }

  bIsLockedOn = false;
  LockedTarget = nullptr;
  OnLockOnEnded.Broadcast();
}

bool LockOnTargeting::IsInFront(float angleDeg) const
{
  return std::abs(angleDeg) <= FieldOfViewDegrees * 0.5f;
}
} // namespace gameplay
