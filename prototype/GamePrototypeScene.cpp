#include "GamePrototypeScene.hpp"

namespace gameplay
{
void GamePrototypeScene::BeginPlay()
{
  SpawnPlayer();
  SpawnEnemy();
  BindSystems();
}

void GamePrototypeScene::Tick(float deltaSeconds, float currentTimeSeconds)
{
  (void)deltaSeconds;

  EnemyAI.PlayerCombat = &PlayerCombat;
  EnemyAI.Tick(deltaSeconds, currentTimeSeconds, EnemyAI.AttackRange - 10.0f);

  PlayerCombat.Tick(deltaSeconds, currentTimeSeconds);
  PlayerAnimation.Tick();
}

void GamePrototypeScene::SpawnPlayer()
{
  PlayerCombat.Stamina = new StaminaComponent();
  PlayerHealth.PlayerCombat = &PlayerCombat;
}

void GamePrototypeScene::SpawnEnemy()
{
  EnemyAI.Stamina = new StaminaComponent();
  EnemyHealth.XpOnDeath = 50;
}

void GamePrototypeScene::BindSystems()
{
  PlayerAnimation.Bind(&PlayerCombat, &PlayerHealth);
  PlayerInput.Bind(&PlayerCombat, &PlayerAnimation, nullptr);
}
} // namespace gameplay
