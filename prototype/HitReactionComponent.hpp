#pragma once

#include "EnemyCombatAI.hpp"

namespace gameplay
{
enum class HitReactionType
{
  None,
  Flinch,
  Knockback,
  Staggered
};

struct HitReactionComponent
{
  float FlinchThreshold = 15.0f;
  float KnockbackThreshold = 35.0f;
  float StaggerThreshold = 60.0f;
  float BossReactionScale = 0.5f;

  float MinReactionSeconds = 0.2f;
  float MaxReactionSeconds = 1.4f;

  bool bIsBoss = false;
  HitReactionType CurrentReaction = HitReactionType::None;

  EnemyCombatAI* EnemyCombat = nullptr;

  EventDispatcher OnFlinch;
  EventDispatcher OnKnockback;
  EventDispatcher OnStaggered;

  void ApplyHitReaction(float damageAmount, float currentTimeSeconds);
  void ClearReaction();

private:
  float ScaleDamageForBoss(float damageAmount) const;
  float ComputeReactionDuration(float damageAmount) const;
  void ApplyEnemyState(float durationSeconds, float currentTimeSeconds);
};
} // namespace gameplay
