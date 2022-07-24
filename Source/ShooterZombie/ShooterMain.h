// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterMain.generated.h"

UCLASS()
class SHOOTERZOMBIE_API AShooterMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterMain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void CalculateCrossHairSpread(float Deltatime);
	void StartCrosshairBulletFire();
	UFUNCTION()
	void FinishCrosshairBulletFire();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BlaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	class USoundCue*  shootingSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	bool HasWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	bool bIsAiming;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float regularFOV;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float zoomedFOV;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float currentFOV;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float FOVZoomInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float hipTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float hipLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float AimTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float AimLookUpRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs)
	float CrosshairSpreadMultiplier;						
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs)
	float CrosshairVelocityFactor;							 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs)
	float CrosshairAimFactor;								
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs)
	float CrosshairShooterFactor;							
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs)
	float CrosshairInAirFactor;
private:
	void MoveForward(float value);
	void MoveRight(float value);
	void Aim();
	void StopAim();
	void SetAiminFOV(float DeltaTime);
	void SetLookRates();



	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapons, meta = (AllowPrivateAccess = true))
	AActor* Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	class UAnimMontage* montage;
	float ShootTimeDuration;
	bool bIsFiringBullet;
	bool bIsFireButtonPressed;
	bool bShouldFire;
	float AutomaticFireRate;
	FTimerHandle AutoFiretimer;
	FTimerHandle CrosshairShootTimer;
	void TurnAtRate(float value);
	void LookUpRate(float value);
	void FireWeapon();
	void PlayShootAnimation();
	void SetFireButtonPressed();
	void SetFireButtonReleased();
	void StartFireTimer();
	UFUNCTION(BlueprintCallable)
	void AutoFireReset();
	UFUNCTION(BlueprintCallable)
	bool TraceUnderCrosshairs(FHitResult& OutHitResult);
public:
	UFUNCTION(BlueprintCallable)
	bool IsAiming();
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetCamera() const { return FollowCamera; }
	UFUNCTION(BlueprintCallable)
		float GetCrosshairSpreadMultiplier() { return CrosshairSpreadMultiplier; };
	void SetWeapon(AActor* weapon);

};
