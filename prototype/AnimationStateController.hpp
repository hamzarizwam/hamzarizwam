#pragma once

#include "HealthComponent.hpp"
#include "PlayerCombatComponent.hpp"

namespace gameplay
{
enum class AnimationState
{
  Idle,
  Move,
  AttackLight,
  AttackHeavy,
  Block,
  Dodge,
  Stagger,
  Death
};

struct AnimationStateController
{
  AnimationState CurrentState = AnimationState::Idle;

  PlayerCombatComponent* PlayerCombat = nullptr;
  HealthComponent* Health = nullptr;

  bool bIsMoving = false;
  bool bIsDodging = false;
  bool bIsStaggered = false;

  EventDispatcher OnStateChanged;

  void Bind(PlayerCombatComponent* combat, HealthComponent* health);
  void Tick();

  void SetMoving(bool isMoving);
  void SetDodging(bool isDodging);
  void SetStaggered(bool isStaggered);

private:
  void UpdateState();
  void TransitionTo(AnimationState newState);
  AnimationState ResolveCombatState() const;
};
} // namespace gameplay
