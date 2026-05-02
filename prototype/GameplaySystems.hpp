#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace gameplay
{
struct EventDispatcher
{
  using Callback = std::function<void()>;

  void Subscribe(Callback callback)
  {
    listeners.push_back(std::move(callback));
  }

  void Broadcast() const
  {
    for (const auto& callback : listeners)
    {
      if (callback)
      {
        callback();
      }
    }
  }

private:
  std::vector<Callback> listeners;
};

enum class CombatState
{
  Idle,
  Attacking,
  Dodging,
  Blocking,
  Staggered,
  Dead
};

enum class BossState
{
  Intro,
  Phase1,
  Phase2,
  Phase3,
  Enraged,
  Dead
};

struct StaminaComponent
{
  float MaxStamina = 120.0f;
  float CurrentStamina = 120.0f;
  float RegenRate = 18.0f;
  float RegenDelaySeconds = 0.8f;
  float LastSpendTimeSeconds = -999.0f;
  bool bIsSprinting = false;
  bool bIsBlocking = false;

  bool HasStaminaFor(float cost) const;
  void Spend(float amount, float currentTimeSeconds);
  void Tick(float deltaSeconds, float currentTimeSeconds);
};

struct CombatComponent
{
  CombatState State = CombatState::Idle;
  float AttackRecoverySeconds = 0.45f;
  float LightAttackCost = 20.0f;
  float HeavyAttackCost = 35.0f;
  bool bAttackQueued = false;

  StaminaComponent* Stamina = nullptr;

  bool CanAttack(float cost) const;
  void RequestLightAttack(float currentTimeSeconds);
  void RequestHeavyAttack(float currentTimeSeconds);
  void FinishAttackRecovery();

  EventDispatcher OnAttackStarted;
  EventDispatcher OnAttackFinished;
};

struct ProgressionSubsystem
{
  int CurrentLevel = 1;
  int CurrentXP = 0;

  EventDispatcher OnXPChanged;
  EventDispatcher OnLevelUp;

  int XPForNextLevel(int level) const;
  void AddXP(int amount);
  void OnPlayerDeath();
};

struct BossStateComponent
{
  BossState State = BossState::Intro;
  float HealthPercent = 1.0f;

  EventDispatcher OnBossPhaseChanged;

  void UpdateHealth(float newPercent);
  void ChangeState(BossState newState);
};

struct SaveSubsystem
{
  std::string LastCheckpointId;

  EventDispatcher OnCheckpointActivated;

  void SaveCheckpoint(std::string checkpointId);
};

struct MemoryHintSubsystem
{
  struct Hint
  {
    std::string Tag;
    std::string VoiceLineId;
  };

  std::vector<Hint> Hints;
  float GlobalCooldownSeconds = 30.0f;
  float LastTriggerTimeSeconds = -999.0f;

  EventDispatcher OnMemoryHintTriggered;

  const Hint* SelectHintByTag(const std::string& tag) const;
  bool TryTriggerHint(const std::string& tag, float currentTimeSeconds);
};

struct AbilitySubsystem
{
  std::unordered_set<std::string> UnlockedAbilities;

  EventDispatcher OnAbilityUnlocked;

  bool CanUnlockAbility(const std::string& abilityTag,
                        const std::unordered_set<std::string>& requiredTags) const;
  void UnlockAbility(const std::string& abilityTag,
                     const std::unordered_set<std::string>& requiredTags);
};

struct RegionSubsystem
{
  std::unordered_set<std::string> UnlockedRegions;

  EventDispatcher OnRegionUnlocked;

  bool IsRegionUnlocked(const std::string& regionTag) const;
  void UnlockRegion(const std::string& regionTag);
};
} // namespace gameplay
