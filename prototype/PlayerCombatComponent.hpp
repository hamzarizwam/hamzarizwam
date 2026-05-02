#pragma once

#include <algorithm>
#include <string>

#include "GameplaySystems.hpp"

namespace gameplay
{
enum class PlayerAttackType
{
  None,
  Light,
  Heavy
};

enum class PlayerCombatState
{
  Idle,
  Attacking,
  Blocking,
  GuardBroken
};

struct PlayerCombatComponent
{
  PlayerCombatState State = PlayerCombatState::Idle;

  StaminaComponent* Stamina = nullptr;

  float LightAttackCost = 18.0f;
  float HeavyAttackCost = 36.0f;
  float BlockStaminaDrainPerSecond = 12.0f;
  float AttackCooldownSeconds = 0.45f;
  float GuardBreakRecoverySeconds = 1.2f;

  float LastAttackTimeSeconds = -999.0f;
  float GuardBreakEndTimeSeconds = -999.0f;

  int ComboIndex = 0;
  int ComboMax = 3;

  EventDispatcher OnAttackStarted;
  EventDispatcher OnAttackFinished;
  EventDispatcher OnGuardBroken;

  bool CanAttack(float currentTimeSeconds, float staminaCost) const;
  void RequestLightAttack(float currentTimeSeconds);
  void RequestHeavyAttack(float currentTimeSeconds);
  void StartBlocking();
  void StopBlocking();
  void Tick(float deltaSeconds, float currentTimeSeconds);

  float ModifyIncomingDamage(float baseDamage) const;

private:
  void ExecuteAttack(PlayerAttackType attackType, float currentTimeSeconds);
  PlayerAttackType SelectComboAttack(PlayerAttackType requestedAttack) const;
  void AdvanceCombo(PlayerAttackType performedAttack);
  void ResetCombo();
  void TriggerGuardBreak(float currentTimeSeconds);
};
} // namespace gameplay
