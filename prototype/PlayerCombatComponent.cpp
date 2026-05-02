#include "PlayerCombatComponent.hpp"

namespace gameplay
{
bool PlayerCombatComponent::CanAttack(float currentTimeSeconds, float staminaCost) const
{
  if (!Stamina)
  {
    return false;
  }

  if (State != PlayerCombatState::Idle)
  {
    return false;
  }

  if (currentTimeSeconds - LastAttackTimeSeconds < AttackCooldownSeconds)
  {
    return false;
  }

  return Stamina->HasStaminaFor(staminaCost);
}

void PlayerCombatComponent::RequestLightAttack(float currentTimeSeconds)
{
  ExecuteAttack(PlayerAttackType::Light, currentTimeSeconds);
}

void PlayerCombatComponent::RequestHeavyAttack(float currentTimeSeconds)
{
  ExecuteAttack(PlayerAttackType::Heavy, currentTimeSeconds);
}

void PlayerCombatComponent::StartBlocking()
{
  if (State == PlayerCombatState::GuardBroken)
  {
    return;
  }

  State = PlayerCombatState::Blocking;
  if (Stamina)
  {
    Stamina->bIsBlocking = true;
  }
}

void PlayerCombatComponent::StopBlocking()
{
  if (State == PlayerCombatState::Blocking)
  {
    State = PlayerCombatState::Idle;
  }

  if (Stamina)
  {
    Stamina->bIsBlocking = false;
  }
}

void PlayerCombatComponent::Tick(float deltaSeconds, float currentTimeSeconds)
{
  if (State == PlayerCombatState::GuardBroken && currentTimeSeconds >= GuardBreakEndTimeSeconds)
  {
    State = PlayerCombatState::Idle;
  }

  if (State == PlayerCombatState::Blocking && Stamina)
  {
    Stamina->Spend(BlockStaminaDrainPerSecond * deltaSeconds, currentTimeSeconds);
    if (Stamina->CurrentStamina <= 0.0f)
    {
      TriggerGuardBreak(currentTimeSeconds);
    }
  }
}

float PlayerCombatComponent::ModifyIncomingDamage(float baseDamage) const
{
  if (State == PlayerCombatState::Blocking)
  {
    return baseDamage * 0.35f;
  }

  return baseDamage;
}

void PlayerCombatComponent::ExecuteAttack(PlayerAttackType attackType, float currentTimeSeconds)
{
  if (State == PlayerCombatState::GuardBroken)
  {
    return;
  }

  if (attackType == PlayerAttackType::Heavy && ComboIndex < 2)
  {
    return;
  }

  PlayerAttackType selectedAttack = SelectComboAttack(attackType);
  float staminaCost = (selectedAttack == PlayerAttackType::Heavy) ? HeavyAttackCost : LightAttackCost;

  if (!CanAttack(currentTimeSeconds, staminaCost))
  {
    return;
  }

  Stamina->Spend(staminaCost, currentTimeSeconds);
  State = PlayerCombatState::Attacking;
  LastAttackTimeSeconds = currentTimeSeconds;
  OnAttackStarted.Broadcast();

  AdvanceCombo(selectedAttack);

  State = PlayerCombatState::Idle;
  OnAttackFinished.Broadcast();
}

PlayerAttackType PlayerCombatComponent::SelectComboAttack(PlayerAttackType requestedAttack) const
{
  if (ComboIndex == 2)
  {
    return PlayerAttackType::Heavy;
  }

  return PlayerAttackType::Light;
}

void PlayerCombatComponent::AdvanceCombo(PlayerAttackType performedAttack)
{
  if (ComboIndex >= ComboMax - 1)
  {
    ResetCombo();
    return;
  }

  if (ComboIndex == 1 && performedAttack == PlayerAttackType::Heavy)
  {
    ResetCombo();
    return;
  }

  ++ComboIndex;
}

void PlayerCombatComponent::ResetCombo()
{
  ComboIndex = 0;
}

void PlayerCombatComponent::TriggerGuardBreak(float currentTimeSeconds)
{
  State = PlayerCombatState::GuardBroken;
  GuardBreakEndTimeSeconds = currentTimeSeconds + GuardBreakRecoverySeconds;
  if (Stamina)
  {
    Stamina->bIsBlocking = false;
  }
  OnGuardBroken.Broadcast();
}
} // namespace gameplay
