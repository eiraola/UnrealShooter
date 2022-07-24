// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterMain.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine.h"
void UShooterAnimInstance::UpdateAnimationProperties()
{
	
	if (Character == nullptr) {
		Character = Cast<AShooterMain>(TryGetPawnOwner());
	}
	
	if (Character) {
		FVector* velocity = new FVector(Character->GetVelocity());
		bHasWeapon = Character->HasWeapon;
		if (velocity != nullptr) {
			speed = velocity->Size();
			bIsInAir = Character->GetCharacterMovement()->IsFalling();
		}
		
	}
	if(Character){
		bIsAiming = Character->IsAiming();
		FRotator CameraRotation = Character->GetBaseAimRotation();
		FRotator MovementRotarion = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
		MovementOffset = UKismetMathLibrary::NormalizedDeltaRotator(CameraRotation, MovementRotarion).Yaw;
		FString MovementRotationMessage = FString::Printf(TEXT("MovementOffset: %f"), MovementOffset);
		//FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotarion.Yaw);
		//FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), CameraRotation.Yaw);
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, MovementRotationMessage);
			//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, MovementRotationMessage);
		}
	}
	
	//if(Character->GetCharacterMovement() != nullptr){
	//if (Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f) {
	//	bIsAccelerating = true;
	//}
	//else {
	//	bIsAccelerating = false;
	//}
	//}
	
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AShooterMain>(TryGetPawnOwner());
	
}
