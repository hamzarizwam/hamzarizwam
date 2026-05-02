#pragma once

#include "GameplaySystems.hpp"
#include "PlayerCombatComponent.hpp"
#include "HitReactionComponent.hpp"

namespace gameplay
{
struct EnemyCombatAI;

struct HealthComponent
{
  float MaxHealth = 100.0f;
  float CurrentHealth = 100.0f;
  float StaggerThreshold = 35.0f;
  int XpOnDeath = 0;
  bool bIsBoss = false;
  bool bIsDead = false;

  PlayerCombatComponent* PlayerCombat = nullptr;
  EnemyCombatAI* EnemyCombat = nullptr;
  HitReactionComponent* HitReaction = nullptr;
  ProgressionSubsystem* PlayerProgression = nullptr;

  EventDispatcher OnDamaged;
  EventDispatcher OnStaggered;
  EventDispatcher OnDeath;
  EventDispatcher OnBossDeath;

  void Reset();
  void TakeDamage(float amount, float currentTimeSeconds);

private:
  float ApplyBlockReduction(float amount) const;
  void HandleDeath();
};
} // namespace gameplay
