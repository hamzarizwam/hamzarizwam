#pragma once

#include <algorithm>

#include "GameplaySystems.hpp"
#include "PlayerCombatComponent.hpp"

namespace gameplay
{
enum class EnemyAIState
{
  Idle,
  Chase,
  Attack,
  Staggered
};

struct EnemyCombatAI
{
  EnemyAIState State = EnemyAIState::Idle;

  StaminaComponent* Stamina = nullptr;
  PlayerCombatComponent* PlayerCombat = nullptr;

  float LightAttackCost = 16.0f;
  float HeavyAttackCost = 28.0f;
  float AttackCooldownSeconds = 0.9f;
  float GuardRespectDelaySeconds = 0.6f;
  float AttackRange = 180.0f;
  float ChaseRange = 750.0f;
  float StaggerRecoverySeconds = 1.0f;

  float LastAttackTimeSeconds = -999.0f;
  float GuardDelayEndTimeSeconds = -999.0f;
  float StaggerEndTimeSeconds = -999.0f;

  EventDispatcher OnAttackStarted;
  EventDispatcher OnAttackFinished;
  EventDispatcher OnStaggered;

  void Tick(float deltaSeconds, float currentTimeSeconds, float distanceToPlayer);
  void OnPlayerGuardBreak(float currentTimeSeconds);
  void EnterStagger(float durationSeconds, float currentTimeSeconds);

private:
  bool CanAttack(float currentTimeSeconds, float staminaCost) const;
  void UpdateState(float currentTimeSeconds, float distanceToPlayer);
  void TryAttack(float currentTimeSeconds, float staminaCost);
  bool PlayerIsGuarding() const;
};
} // namespace gameplay
