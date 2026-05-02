#include <iostream>
#include <vector>

#include "AttackHitboxComponent.hpp"
#include "GamePrototypeScene.hpp"

int main()
{
  using namespace gameplay;

  GamePrototypeScene scene;
  scene.BeginPlay();

  AttackHitboxComponent playerHitbox;

  std::cout << "== Prototype Combat Simulation Start ==\n";

  float now = 0.0f;
  const float dt = 0.1f;

  // simulate a short combat exchange in a flat test arena
  for (int frame = 0; frame < 60; ++frame)
  {
    now += dt;

    if (frame == 2)
    {
      scene.PlayerInput.OnMoveForward(1.0f);
      scene.PlayerInput.OnLightAttack(now);
      playerHitbox.ActivateHitbox(AttackType::Light);
      playerHitbox.HandleOverlap(&scene.EnemyHealth, now);
      playerHitbox.DeactivateHitbox();
      std::cout << "[" << now << "] player light attack\n";
    }

    if (frame == 15)
    {
      scene.PlayerInput.OnHeavyAttack(now);
      playerHitbox.ActivateHitbox(AttackType::Heavy);
      playerHitbox.HandleOverlap(&scene.EnemyHealth, now);
      playerHitbox.DeactivateHitbox();
      std::cout << "[" << now << "] player heavy attack\n";
    }

    if (frame == 24)
    {
      scene.PlayerInput.OnBlockPressed();
      std::cout << "[" << now << "] player starts blocking\n";
    }

    if (frame == 34)
    {
      scene.PlayerInput.OnBlockReleased();
      std::cout << "[" << now << "] player stops blocking\n";
    }

    scene.Tick(dt, now);

    std::cout << "[" << now << "] enemy_hp=" << scene.EnemyHealth.CurrentHealth
              << " player_stamina=" << scene.PlayerCombat.Stamina->CurrentStamina << "\n";

    if (scene.EnemyHealth.bIsDead)
    {
      std::cout << "Enemy defeated. Prototype loop complete.\n";
      break;
    }
  }

  std::cout << "== Prototype Combat Simulation End ==\n";
  return 0;
}
