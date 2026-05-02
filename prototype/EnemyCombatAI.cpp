#include "EnemyCombatAI.hpp"

namespace gameplay
{
void EnemyCombatAI::Tick(float deltaSeconds, float currentTimeSeconds, float distanceToPlayer)
{
  if (State == EnemyAIState::Staggered)
  {
    if (currentTimeSeconds >= StaggerEndTimeSeconds)
    {
      State = EnemyAIState::Idle;
    }
    return;
  }

  UpdateState(currentTimeSeconds, distanceToPlayer);

  if (State != EnemyAIState::Attack)
  {
    return;
  }

  float staminaCost = LightAttackCost;
  if (Stamina && Stamina->CurrentStamina >= HeavyAttackCost)
  {
    staminaCost = HeavyAttackCost;
  }

  TryAttack(currentTimeSeconds, staminaCost);
}

void EnemyCombatAI::OnPlayerGuardBreak(float currentTimeSeconds)
{
  State = EnemyAIState::Staggered;
  StaggerEndTimeSeconds = currentTimeSeconds + StaggerRecoverySeconds;
  OnStaggered.Broadcast();
}

void EnemyCombatAI::EnterStagger(float durationSeconds, float currentTimeSeconds)
{
  State = EnemyAIState::Staggered;
  StaggerEndTimeSeconds = currentTimeSeconds + durationSeconds;
  OnStaggered.Broadcast();
}

bool EnemyCombatAI::CanAttack(float currentTimeSeconds, float staminaCost) const
{
  if (!Stamina)
  {
    return false;
  }

  if (currentTimeSeconds - LastAttackTimeSeconds < AttackCooldownSeconds)
  {
    return false;
  }

  if (!Stamina->HasStaminaFor(staminaCost))
  {
    return false;
  }

  if (currentTimeSeconds < GuardDelayEndTimeSeconds)
  {
    return false;
  }

  return true;
}

void EnemyCombatAI::UpdateState(float currentTimeSeconds, float distanceToPlayer)
{
  if (distanceToPlayer <= AttackRange)
  {
    State = EnemyAIState::Attack;
  }
  else if (distanceToPlayer <= ChaseRange)
  {
    State = EnemyAIState::Chase;
  }
  else
  {
    State = EnemyAIState::Idle;
  }

  if (State == EnemyAIState::Attack && PlayerIsGuarding())
  {
    GuardDelayEndTimeSeconds = std::max(GuardDelayEndTimeSeconds,
                                        currentTimeSeconds + GuardRespectDelaySeconds);
  }
}

void EnemyCombatAI::TryAttack(float currentTimeSeconds, float staminaCost)
{
  if (!CanAttack(currentTimeSeconds, staminaCost))
  {
    return;
  }

  Stamina->Spend(staminaCost, currentTimeSeconds);
  LastAttackTimeSeconds = currentTimeSeconds;
  OnAttackStarted.Broadcast();
  OnAttackFinished.Broadcast();
}

bool EnemyCombatAI::PlayerIsGuarding() const
{
  return PlayerCombat && PlayerCombat->State == PlayerCombatState::Blocking;
}
} // namespace gameplay
