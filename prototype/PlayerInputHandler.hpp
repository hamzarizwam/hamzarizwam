#pragma once

#include "AnimationStateController.hpp"
#include "LockOnTargeting.hpp"
#include "PlayerCombatComponent.hpp"

namespace gameplay
{
struct PlayerInputHandler
{
  PlayerCombatComponent* PlayerCombat = nullptr;
  AnimationStateController* AnimationController = nullptr;
  LockOnTargeting* LockOnSystem = nullptr;

  float MouseSensitivity = 0.7f;

  float MoveForward = 0.0f;
  float MoveRight = 0.0f;
  float LookYaw = 0.0f;
  float LookPitch = 0.0f;

  void Bind(PlayerCombatComponent* combat,
            AnimationStateController* animationController,
            LockOnTargeting* lockOn);

  void OnMoveForward(float value);
  void OnMoveRight(float value);
  void OnLookYaw(float value);
  void OnLookPitch(float value);

  void OnLightAttack(float currentTimeSeconds);
  void OnHeavyAttack(float currentTimeSeconds);
  void OnBlockPressed();
  void OnBlockReleased();
  void OnDodge();
  void OnLockOnToggle(const std::vector<EnemyCombatAI*>& candidates,
                      const std::vector<float>& candidateDistances,
                      const std::vector<float>& candidateAnglesDeg);

  void TickMovement();

private:
  void UpdateMovementState();
};
} // namespace gameplay
