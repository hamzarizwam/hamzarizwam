#include "AnimationStateController.hpp"

namespace gameplay
{
void AnimationStateController::Bind(PlayerCombatComponent* combat, HealthComponent* health)
{
  PlayerCombat = combat;
  Health = health;
}

void AnimationStateController::Tick()
{
  UpdateState();
}

void AnimationStateController::SetMoving(bool isMoving)
{
  bIsMoving = isMoving;
}

void AnimationStateController::SetDodging(bool isDodging)
{
  bIsDodging = isDodging;
}

void AnimationStateController::SetStaggered(bool isStaggered)
{
  bIsStaggered = isStaggered;
}

void AnimationStateController::UpdateState()
{
  if (Health && Health->bIsDead)
  {
    TransitionTo(AnimationState::Death);
    return;
  }

  if (bIsStaggered)
  {
    TransitionTo(AnimationState::Stagger);
    return;
  }

  if (bIsDodging)
  {
    TransitionTo(AnimationState::Dodge);
    return;
  }

  if (PlayerCombat)
  {
    AnimationState combatState = ResolveCombatState();
    if (combatState != AnimationState::Idle)
    {
      TransitionTo(combatState);
      return;
    }
  }

  if (bIsMoving)
  {
    TransitionTo(AnimationState::Move);
    return;
  }

  TransitionTo(AnimationState::Idle);
}

void AnimationStateController::TransitionTo(AnimationState newState)
{
  if (CurrentState == newState)
  {
    return;
  }

  CurrentState = newState;
  OnStateChanged.Broadcast();
}

AnimationState AnimationStateController::ResolveCombatState() const
{
  if (!PlayerCombat)
  {
    return AnimationState::Idle;
  }

  if (PlayerCombat->State == PlayerCombatState::Blocking)
  {
    return AnimationState::Block;
  }

  if (PlayerCombat->State == PlayerCombatState::Attacking)
  {
    return PlayerCombat->ComboIndex >= 2 ? AnimationState::AttackHeavy : AnimationState::AttackLight;
  }

  return AnimationState::Idle;
}
} // namespace gameplay
