# Soulslike Action RPG Systems Architecture & Prototype Logic

## Overview
This document defines a modular, scalable system architecture and prototype logic for a single-player, third-person, Soulslike action RPG. The design emphasizes weighty combat, stamina management, pattern-based enemies, minimal HUD, and exploration-driven storytelling with memory hints.

Key pillars:
- **Modular systems** (Combat, Stamina, XP/Leveling, Boss AI, Checkpoints, Memory Hints, Ability Tree, Region Unlocking).
- **Data-driven** configuration (Data Assets / Data Tables) for rapid tuning.
- **Event-based** communications between systems for clean dependencies.

---

## Core System Architecture (Unreal-Style)

### Gameplay Modules
- **UGameInstanceSubsystems**
  - `UProgressionSubsystem` (XP, leveling, stats)
  - `UAbilitySubsystem` (ability unlocks, ability registry)
  - `URegionSubsystem` (region unlocks & gates)
  - `UMemoryHintSubsystem` (memory hint pool, trigger cooldowns)
  - `USaveSubsystem` (checkpoint saves)

### Gameplay Actors/Components
- **Character Components**
  - `UCombatComponent`
  - `UStaminaComponent`
  - `UDamageReceiverComponent`
  - `UAbilityComponent`
- **Enemy/Boss Components**
  - `UBossStateComponent` (state machine, phase transitions)
  - `UAttackPatternComponent`
  - `UDamageReceiverComponent`
- **World**
  - `ACheckpointActor`
  - `ARegionGateActor`
  - `AMemoryHintTrigger`

### Data-Driven Assets
- `UWeaponData` (weapon stats, stamina cost, animations)
- `UAbilityData` (ability details, unlock requirements)
- `UBossData` (phases, patterns, health thresholds)
- `URegionData` (unlock conditions, gate requirements)
- `UMemoryHintData` (voice lines, cooldowns, trigger filters)
- `UProgressionCurveData` (XP per level)

### Event System
Use Gameplay Tags + Event Dispatchers for decoupled logic:
- `OnXPChanged`
- `OnLevelUp`
- `OnBossPhaseChanged`
- `OnAbilityUnlocked`
- `OnCheckpointActivated`
- `OnRegionUnlocked`
- `OnMemoryHintTriggered`

---

# SYSTEM PROTOTYPE LOGIC

## 1. Combat System
**Goals:** weighty timing, stamina gating, clear windows, readable enemy patterns.

### Component: `UCombatComponent`
**Responsibilities:**
- Track combat state (idle, attacking, dodging, blocking, staggered)
- Queue attacks and enforce recovery frames
- Resolve hit detection, damage, poise, knockback

**Pseudo (C++ style):**
```cpp
enum class ECombatState { Idle, Attacking, Dodging, Blocking, Staggered, Dead };

class UCombatComponent : public UActorComponent
{
public:
  ECombatState State = ECombatState::Idle;
  float AttackRecoveryTime = 0.45f;
  bool bAttackQueued = false;

  void RequestLightAttack()
  {
    if (CanAttack())
    {
      ConsumeStamina(LightAttackCost);
      PlayAttackMontage(LightAttack);
      State = ECombatState::Attacking;
      SetRecoveryTimer(AttackRecoveryTime);
    }
    else
    {
      bAttackQueued = true; // optional for heavy weapons
    }
  }

  void RequestHeavyAttack()
  {
    if (CanAttack())
    {
      ConsumeStamina(HeavyAttackCost);
      PlayAttackMontage(HeavyAttack);
      State = ECombatState::Attacking;
      SetRecoveryTimer(HeavyAttackRecovery);
    }
  }

  bool CanAttack()
  {
    return State == ECombatState::Idle && Stamina->HasStaminaFor(CurrentWeaponCost);
  }

  void OnAttackRecoveryFinished()
  {
    State = ECombatState::Idle;
    if (bAttackQueued) { bAttackQueued = false; RequestLightAttack(); }
  }
};
```

### Weighty Combat Features
- **Pre-windup** before hit frames
- **Commitment**: no animation cancel except specific abilities
- **Hitstop** to emphasize impact
- **Poise break** to stagger enemies after threshold damage

---

## 2. Stamina System
**Goals:** stamina limits aggressive play, regenerates when idle, slows regen during block.

### Component: `UStaminaComponent`
```cpp
class UStaminaComponent : public UActorComponent
{
public:
  float MaxStamina = 120.f;
  float CurrentStamina = 120.f;
  float RegenRate = 18.f;
  float RegenDelay = 0.8f;
  float LastSpendTime = -999.f;

  void Spend(float Amount)
  {
    CurrentStamina = FMath::Max(CurrentStamina - Amount, 0.f);
    LastSpendTime = GetWorldTime();
  }

  void Tick(float DeltaTime)
  {
    if (GetWorldTime() - LastSpendTime > RegenDelay)
    {
      CurrentStamina = FMath::Min(CurrentStamina + RegenRate * DeltaTime, MaxStamina);
    }
  }

  bool HasStaminaFor(float Cost) const { return CurrentStamina >= Cost; }
};
```

**Stamina rules:**
- Dodging, blocking, and all attacks consume stamina.
- Stamina cannot regenerate while sprinting or in a blocking state.

---

## 3. XP & Leveling System
**Goals:** permanent loss on death, no corpse recovery, XP used for stats/abilities/upgrades.

### Subsystem: `UProgressionSubsystem`
```cpp
class UProgressionSubsystem : public UGameInstanceSubsystem
{
public:
  int32 CurrentLevel = 1;
  int32 CurrentXP = 0;

  void AddXP(int32 Amount)
  {
    CurrentXP += Amount;
    if (CurrentXP >= XPForNextLevel(CurrentLevel))
    {
      LevelUp();
    }
  }

  void LevelUp()
  {
    CurrentLevel++;
    Broadcast(OnLevelUp);
  }

  void OnPlayerDeath()
  {
    CurrentXP = 0; // Permanent loss
    Broadcast(OnXPChanged);
  }
};
```

**XP Spend Categories:**
- **Stats** (health, stamina, strength, dexterity, resolve)
- **Ability Unlocks**
- **Weapon Upgrades**

---

## 4. Boss Behavior States
**Goals:** multi-phase bosses with readable patterns.

### Component: `UBossStateComponent`
```cpp
enum class EBossState { Intro, Phase1, Phase2, Phase3, Enraged, Dead };

class UBossStateComponent : public UActorComponent
{
public:
  EBossState State = EBossState::Intro;
  float HealthPercent = 1.0f;

  void UpdateHealth(float NewPercent)
  {
    HealthPercent = NewPercent;
    if (HealthPercent < 0.65f && State == EBossState::Phase1) ChangeState(Phase2);
    if (HealthPercent < 0.35f && State == EBossState::Phase2) ChangeState(Phase3);
  }

  void ChangeState(EBossState NewState)
  {
    State = NewState;
    ApplyPhaseModifiers(NewState);
    Broadcast(OnBossPhaseChanged);
  }
};
```

**Boss pattern system:**
- Attack sets stored in `UBossData`
- Each phase unlocks specific patterns
- Transition events can spawn adds, change arena, add hazards

---

## 5. Checkpoint Save System
**Goals:** light save system, respawn at last checkpoint, no XP recovery.

### Actor: `ACheckpointActor`
```cpp
class ACheckpointActor : public AActor
{
public:
  bool bActivated = false;

  void ActivateCheckpoint(APlayerCharacter* Player)
  {
    bActivated = true;
    SaveSubsystem->SaveCheckpoint(this->GetActorTransform());
    Broadcast(OnCheckpointActivated);
  }
};
```

### Subsystem: `USaveSubsystem`
```cpp
class USaveSubsystem : public UGameInstanceSubsystem
{
public:
  FTransform LastCheckpoint;

  void SaveCheckpoint(FTransform NewCheckpoint)
  {
    LastCheckpoint = NewCheckpoint;
    SaveToSlot();
  }

  void RespawnPlayer(APlayerCharacter* Player)
  {
    Player->SetActorTransform(LastCheckpoint);
  }
};
```

---

## 6. Memory Hint System
**Goals:** indirect clues, voice/memory fragments triggered by exploration and state.

### Subsystem: `UMemoryHintSubsystem`
```cpp
class UMemoryHintSubsystem : public UGameInstanceSubsystem
{
public:
  TArray<UMemoryHintData*> Hints;
  float GlobalCooldown = 30.f;
  float LastTriggerTime = -999.f;

  void TryTriggerHint(FGameplayTag ContextTag)
  {
    if (GetWorldTime() - LastTriggerTime < GlobalCooldown) return;

    UMemoryHintData* Hint = SelectHintByContext(ContextTag);
    if (Hint)
    {
      PlayVoiceLine(Hint->VoiceLine);
      LastTriggerTime = GetWorldTime();
      Broadcast(OnMemoryHintTriggered);
    }
  }
};
```

**Trigger examples:**
- Entering a new region
- Discovering key ruins
- Approaching boss arenas
- Equipping special weapons

---

## 7. Ability Unlock Tree
**Goals:** abilities unlocked through bosses/memories/hidden areas. Data-driven unlock requirements.

### Ability Unlock Flow
```cpp
class UAbilitySubsystem : public UGameInstanceSubsystem
{
public:
  TSet<FGameplayTag> UnlockedAbilities;

  bool CanUnlockAbility(UAbilityData* Ability)
  {
    return RequirementsMet(Ability->UnlockRequirements);
  }

  void UnlockAbility(UAbilityData* Ability)
  {
    if (CanUnlockAbility(Ability))
    {
      UnlockedAbilities.Add(Ability->AbilityTag);
      Broadcast(OnAbilityUnlocked);
    }
  }
};
```

**Ability Requirements Examples:**
- Defeat specific boss
- Find memory fragment in region
- Player level threshold

---

## 8. Region Unlocking System
**Goals:** 4 interconnected regions, unlock by bosses or story items.

### Subsystem: `URegionSubsystem`
```cpp
class URegionSubsystem : public UGameInstanceSubsystem
{
public:
  TSet<FGameplayTag> UnlockedRegions;

  void UnlockRegion(FGameplayTag RegionTag)
  {
    UnlockedRegions.Add(RegionTag);
    Broadcast(OnRegionUnlocked);
  }

  bool IsRegionUnlocked(FGameplayTag RegionTag)
  {
    return UnlockedRegions.Contains(RegionTag);
  }
};
```

### Actor: `ARegionGateActor`
```cpp
class ARegionGateActor : public AActor
{
public:
  FGameplayTag RequiredRegionTag;

  bool CanOpenGate(URegionSubsystem* RegionSubsystem)
  {
    return RegionSubsystem->IsRegionUnlocked(RequiredRegionTag);
  }
};
```

---

# Additional Notes for Indie Scale
- Use **Data Assets** to define all weapons, abilities, bosses, and regions.
- Keep components isolated to allow incremental development.
- Use **Gameplay Tags** to avoid tight coupling between story events and systems.
- Use minimal HUD feedback (stamina/HP only) and convey world lore through audio cues and environmental storytelling.

---

## Suggested Next Steps
- Implement a vertical slice of Dead Lands with 1 boss and 2 abilities.
- Add 1 memory hint chain that reveals the false parents’ betrayal.
- Balance stamina costs to reinforce slow, deliberate pacing.

