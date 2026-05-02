#include "GameplaySystems.hpp"

namespace gameplay
{
bool StaminaComponent::HasStaminaFor(float cost) const
{
  return CurrentStamina >= cost;
}

void StaminaComponent::Spend(float amount, float currentTimeSeconds)
{
  CurrentStamina = std::max(0.0f, CurrentStamina - amount);
  LastSpendTimeSeconds = currentTimeSeconds;
}

void StaminaComponent::Tick(float deltaSeconds, float currentTimeSeconds)
{
  if (bIsSprinting || bIsBlocking)
  {
    return;
  }

  if (currentTimeSeconds - LastSpendTimeSeconds > RegenDelaySeconds)
  {
    CurrentStamina = std::min(MaxStamina, CurrentStamina + RegenRate * deltaSeconds);
  }
}

bool CombatComponent::CanAttack(float cost) const
{
  return State == CombatState::Idle && Stamina && Stamina->HasStaminaFor(cost);
}

void CombatComponent::RequestLightAttack(float currentTimeSeconds)
{
  if (CanAttack(LightAttackCost))
  {
    Stamina->Spend(LightAttackCost, currentTimeSeconds);
    State = CombatState::Attacking;
    OnAttackStarted.Broadcast();
  }
  else
  {
    bAttackQueued = true;
  }
}

void CombatComponent::RequestHeavyAttack(float currentTimeSeconds)
{
  if (CanAttack(HeavyAttackCost))
  {
    Stamina->Spend(HeavyAttackCost, currentTimeSeconds);
    State = CombatState::Attacking;
    OnAttackStarted.Broadcast();
  }
}

void CombatComponent::FinishAttackRecovery()
{
  State = CombatState::Idle;
  OnAttackFinished.Broadcast();
  if (bAttackQueued)
  {
    bAttackQueued = false;
    RequestLightAttack(0.0f);
  }
}

int ProgressionSubsystem::XPForNextLevel(int level) const
{
  return 100 + (level * level * 25);
}

void ProgressionSubsystem::AddXP(int amount)
{
  CurrentXP += amount;
  OnXPChanged.Broadcast();

  while (CurrentXP >= XPForNextLevel(CurrentLevel))
  {
    CurrentXP -= XPForNextLevel(CurrentLevel);
    ++CurrentLevel;
    OnLevelUp.Broadcast();
  }
}

void ProgressionSubsystem::OnPlayerDeath()
{
  CurrentXP = 0;
  OnXPChanged.Broadcast();
}

void BossStateComponent::UpdateHealth(float newPercent)
{
  HealthPercent = std::clamp(newPercent, 0.0f, 1.0f);

  if (HealthPercent <= 0.0f)
  {
    ChangeState(BossState::Dead);
    return;
  }

  if (HealthPercent < 0.35f && State == BossState::Phase2)
  {
    ChangeState(BossState::Phase3);
  }
  else if (HealthPercent < 0.65f && State == BossState::Phase1)
  {
    ChangeState(BossState::Phase2);
  }
}

void BossStateComponent::ChangeState(BossState newState)
{
  if (State == newState)
  {
    return;
  }

  State = newState;
  OnBossPhaseChanged.Broadcast();
}

void SaveSubsystem::SaveCheckpoint(std::string checkpointId)
{
  LastCheckpointId = std::move(checkpointId);
  OnCheckpointActivated.Broadcast();
}

const MemoryHintSubsystem::Hint* MemoryHintSubsystem::SelectHintByTag(const std::string& tag) const
{
  auto it = std::find_if(Hints.begin(), Hints.end(), [&](const Hint& hint) {
    return hint.Tag == tag;
  });

  if (it == Hints.end())
  {
    return nullptr;
  }

  return &(*it);
}

bool MemoryHintSubsystem::TryTriggerHint(const std::string& tag, float currentTimeSeconds)
{
  if (currentTimeSeconds - LastTriggerTimeSeconds < GlobalCooldownSeconds)
  {
    return false;
  }

  const Hint* hint = SelectHintByTag(tag);
  if (!hint)
  {
    return false;
  }

  LastTriggerTimeSeconds = currentTimeSeconds;
  OnMemoryHintTriggered.Broadcast();
  return true;
}

bool AbilitySubsystem::CanUnlockAbility(
  const std::string& abilityTag,
  const std::unordered_set<std::string>& requiredTags) const
{
  if (UnlockedAbilities.contains(abilityTag))
  {
    return false;
  }

  return std::all_of(requiredTags.begin(), requiredTags.end(), [&](const auto& tag) {
    return UnlockedAbilities.contains(tag);
  });
}

void AbilitySubsystem::UnlockAbility(const std::string& abilityTag,
                                     const std::unordered_set<std::string>& requiredTags)
{
  if (!CanUnlockAbility(abilityTag, requiredTags))
  {
    return;
  }

  UnlockedAbilities.insert(abilityTag);
  OnAbilityUnlocked.Broadcast();
}

bool RegionSubsystem::IsRegionUnlocked(const std::string& regionTag) const
{
  return UnlockedRegions.contains(regionTag);
}

void RegionSubsystem::UnlockRegion(const std::string& regionTag)
{
  if (UnlockedRegions.contains(regionTag))
  {
    return;
  }

  UnlockedRegions.insert(regionTag);
  OnRegionUnlocked.Broadcast();
}
} // namespace gameplay
