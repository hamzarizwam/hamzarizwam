#pragma once

#include "AnimationStateController.hpp"
#include "EnemyCombatAI.hpp"
#include "HealthComponent.hpp"
#include "PlayerCombatComponent.hpp"
#include "PlayerInputHandler.hpp"

namespace gameplay
{
struct GamePrototypeScene
{
  PlayerCombatComponent PlayerCombat;
  HealthComponent PlayerHealth;
  AnimationStateController PlayerAnimation;
  PlayerInputHandler PlayerInput;

  EnemyCombatAI EnemyAI;
  HealthComponent EnemyHealth;

  void BeginPlay();
  void Tick(float deltaSeconds, float currentTimeSeconds);

private:
  void SpawnPlayer();
  void SpawnEnemy();
  void BindSystems();
};
} // namespace gameplay
