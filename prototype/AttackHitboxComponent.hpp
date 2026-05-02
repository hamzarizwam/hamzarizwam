#pragma once

#include <unordered_set>

#include "HealthComponent.hpp"

namespace gameplay
{
enum class AttackType
{
  Light,
  Heavy
};

struct AttackHitboxComponent
{
  float LightHitboxRadius = 75.0f;
  float HeavyHitboxRadius = 110.0f;
  float LightDamage = 25.0f;
  float HeavyDamage = 45.0f;

  bool bHitboxActive = false;
  AttackType CurrentAttack = AttackType::Light;

  EventDispatcher OnHitboxActivated;
  EventDispatcher OnHitboxDeactivated;
  EventDispatcher OnHitConfirmed;

  void ActivateHitbox(AttackType attackType);
  void DeactivateHitbox();
  void ResetSwing();

  void HandleOverlap(HealthComponent* targetHealth, float currentTimeSeconds);

  float GetCurrentHitboxRadius() const;

private:
  std::unordered_set<const HealthComponent*> HitTargets;
};
} // namespace gameplay
