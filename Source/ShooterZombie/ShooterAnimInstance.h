// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERZOMBIE_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
		UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties();
		virtual void NativeInitializeAnimation() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class AShooterMain* Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAccelerating;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAiming;
	
};
