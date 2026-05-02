#include "AttackHitboxComponent.hpp"

namespace gameplay
{
void AttackHitboxComponent::ActivateHitbox(AttackType attackType)
{
  CurrentAttack = attackType;
  bHitboxActive = true;
  ResetSwing();
  OnHitboxActivated.Broadcast();
}

void AttackHitboxComponent::DeactivateHitbox()
{
  bHitboxActive = false;
  OnHitboxDeactivated.Broadcast();
}

void AttackHitboxComponent::ResetSwing()
{
  HitTargets.clear();
}

void AttackHitboxComponent::HandleOverlap(HealthComponent* targetHealth, float currentTimeSeconds)
{
  if (!bHitboxActive || !targetHealth)
  {
    return;
  }

  if (HitTargets.contains(targetHealth))
  {
    return;
  }

  HitTargets.insert(targetHealth);

  float damage = (CurrentAttack == AttackType::Heavy) ? HeavyDamage : LightDamage;
  targetHealth->TakeDamage(damage, currentTimeSeconds);
  OnHitConfirmed.Broadcast();
}

float AttackHitboxComponent::GetCurrentHitboxRadius() const
{
  return (CurrentAttack == AttackType::Heavy) ? HeavyHitboxRadius : LightHitboxRadius;
}
} // namespace gameplay
