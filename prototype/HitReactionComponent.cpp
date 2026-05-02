#include "HitReactionComponent.hpp"

#include <algorithm>

namespace gameplay
{
void HitReactionComponent::ApplyHitReaction(float damageAmount, float currentTimeSeconds)
{
  float scaledDamage = ScaleDamageForBoss(damageAmount);

  if (scaledDamage >= StaggerThreshold)
  {
    CurrentReaction = HitReactionType::Staggered;
    OnStaggered.Broadcast();
    ApplyEnemyState(ComputeReactionDuration(scaledDamage), currentTimeSeconds);
    return;
  }

  if (scaledDamage >= KnockbackThreshold)
  {
    CurrentReaction = HitReactionType::Knockback;
    OnKnockback.Broadcast();
    ApplyEnemyState(ComputeReactionDuration(scaledDamage), currentTimeSeconds);
    return;
  }

  if (scaledDamage >= FlinchThreshold)
  {
    CurrentReaction = HitReactionType::Flinch;
    OnFlinch.Broadcast();
    ApplyEnemyState(ComputeReactionDuration(scaledDamage), currentTimeSeconds);
    return;
  }

  CurrentReaction = HitReactionType::None;
}

void HitReactionComponent::ClearReaction()
{
  CurrentReaction = HitReactionType::None;
}

float HitReactionComponent::ScaleDamageForBoss(float damageAmount) const
{
  if (!bIsBoss)
  {
    return damageAmount;
  }

  return damageAmount * BossReactionScale;
}

float HitReactionComponent::ComputeReactionDuration(float damageAmount) const
{
  float normalized = std::clamp(damageAmount / StaggerThreshold, 0.0f, 1.0f);
  return MinReactionSeconds + (MaxReactionSeconds - MinReactionSeconds) * normalized;
}

void HitReactionComponent::ApplyEnemyState(float durationSeconds, float currentTimeSeconds)
{
  if (!EnemyCombat)
  {
    return;
  }

  EnemyCombat->EnterStagger(durationSeconds, currentTimeSeconds);
}
} // namespace gameplay
