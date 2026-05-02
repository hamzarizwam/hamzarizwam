#include "HealthComponent.hpp"

#include <algorithm>

namespace gameplay
{
void HealthComponent::Reset()
{
  CurrentHealth = MaxHealth;
  bIsDead = false;
}

void HealthComponent::TakeDamage(float amount, float currentTimeSeconds)
{
  if (bIsDead)
  {
    return;
  }

  float finalDamage = ApplyBlockReduction(amount);
  CurrentHealth = std::max(0.0f, CurrentHealth - finalDamage);
  OnDamaged.Broadcast();
  if (HitReaction)
  {
    HitReaction->ApplyHitReaction(finalDamage, currentTimeSeconds);
  }

  if (finalDamage >= StaggerThreshold)
  {
    OnStaggered.Broadcast();
  }

  if (CurrentHealth <= 0.0f)
  {
    HandleDeath();
  }
}

float HealthComponent::ApplyBlockReduction(float amount) const
{
  if (PlayerCombat)
  {
    return PlayerCombat->ModifyIncomingDamage(amount);
  }

  return amount;
}

void HealthComponent::HandleDeath()
{
  bIsDead = true;
  OnDeath.Broadcast();

  if (PlayerProgression && XpOnDeath > 0)
  {
    PlayerProgression->AddXP(XpOnDeath);
  }

  if (bIsBoss)
  {
    OnBossDeath.Broadcast();
  }
}
} // namespace gameplay
