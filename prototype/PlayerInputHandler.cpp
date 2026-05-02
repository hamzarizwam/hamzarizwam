#include "PlayerInputHandler.hpp"

namespace gameplay
{
void PlayerInputHandler::Bind(PlayerCombatComponent* combat,
                              AnimationStateController* animationController,
                              LockOnTargeting* lockOn)
{
  PlayerCombat = combat;
  AnimationController = animationController;
  LockOnSystem = lockOn;
}

void PlayerInputHandler::OnMoveForward(float value)
{
  MoveForward = value;
  UpdateMovementState();
}

void PlayerInputHandler::OnMoveRight(float value)
{
  MoveRight = value;
  UpdateMovementState();
}

void PlayerInputHandler::OnLookYaw(float value)
{
  LookYaw += value * MouseSensitivity;
}

void PlayerInputHandler::OnLookPitch(float value)
{
  LookPitch += value * MouseSensitivity;
}

void PlayerInputHandler::OnLightAttack(float currentTimeSeconds)
{
  if (PlayerCombat)
  {
    PlayerCombat->RequestLightAttack(currentTimeSeconds);
  }
}

void PlayerInputHandler::OnHeavyAttack(float currentTimeSeconds)
{
  if (PlayerCombat)
  {
    PlayerCombat->RequestHeavyAttack(currentTimeSeconds);
  }
}

void PlayerInputHandler::OnBlockPressed()
{
  if (PlayerCombat)
  {
    PlayerCombat->StartBlocking();
  }
}

void PlayerInputHandler::OnBlockReleased()
{
  if (PlayerCombat)
  {
    PlayerCombat->StopBlocking();
  }
}

void PlayerInputHandler::OnDodge()
{
  if (AnimationController)
  {
    AnimationController->SetDodging(true);
  }
}

void PlayerInputHandler::OnLockOnToggle(const std::vector<EnemyCombatAI*>& candidates,
                                        const std::vector<float>& candidateDistances,
                                        const std::vector<float>& candidateAnglesDeg)
{
  if (LockOnSystem)
  {
    LockOnSystem->ToggleLockOn(candidates, candidateDistances, candidateAnglesDeg);
  }
}

void PlayerInputHandler::TickMovement()
{
  if (AnimationController)
  {
    AnimationController->SetDodging(false);
  }
}

void PlayerInputHandler::UpdateMovementState()
{
  if (!AnimationController)
  {
    return;
  }

  bool isMoving = (MoveForward * MoveForward + MoveRight * MoveRight) > 0.01f;
  AnimationController->SetMoving(isMoving);
}
} // namespace gameplay
