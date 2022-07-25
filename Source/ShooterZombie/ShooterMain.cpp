// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMain.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Rifle.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Item.h"
#include "Components/WidgetComponent.h"
#include "DrawDebugHelpers.h"
#include "Weapon.h"
// Sets default values
AShooterMain::AShooterMain()
{
	bIsAiming = false;
	HasWeapon = false;
	BaseTurnRate = 25;
	BlaseLookUpRate = 25;
	regularFOV = 0.f;
	zoomedFOV = 40.f;
	currentFOV = 0.0f;
	FOVZoomInterpSpeed = 15.f;
	hipTurnRate = 90;
	hipLookUpRate = 90;
	AimTurnRate = 50.5;
	AimLookUpRate = 50.5;
	ShootTimeDuration = 0.05f;
	bIsFiringBullet = true;
	bIsFireButtonPressed = false;
	AutomaticFireRate = 0.25;
	bShouldFire = false;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraBoom->SocketOffset =  FVector(30.0f, 50.0f, 50.0f);
	FollowCamera->bUsePawnControlRotation = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);
	GetCharacterMovement()->AirControl = 0.02f;
	GetCharacterMovement()->JumpZVelocity = 600.0f;

	bShouldCheckForItems = false;
	NumberOverlappedItems = 0;

}

// Called when the game starts or when spawned
void AShooterMain::BeginPlay()
{
	Super::BeginPlay();
	if (FollowCamera) {
		regularFOV = FollowCamera->FieldOfView;
		currentFOV = FollowCamera->FieldOfView;
	}
	NumberOverlappedItems = 0;
	SpawnDefaultWeapon();
}

void AShooterMain::CalculateCrossHairSpread(float Deltatime)
{
	FVector2D walkSpeedRange(0.0f, 600.f);
	FVector2D velocityMultiplyReange(0.0f, 1.0f);
	FVector velocity = GetVelocity();
	velocity.Z = 0.0f;
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, Deltatime, 2.25f);
	}
	else {
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, Deltatime, 30.25f);
	}
	if (bIsAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.5f, Deltatime, 2.25f);
	}
	else {
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, Deltatime, 30.25f);
	}
	if (bIsFiringBullet)
	{
		CrosshairShooterFactor = FMath::FInterpTo(CrosshairShooterFactor, 0.3f, Deltatime, 60.0f);
	}
	else {
		CrosshairShooterFactor = FMath::FInterpTo(CrosshairShooterFactor, 0.0f, Deltatime, 60.0f);
	}
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(walkSpeedRange, velocityMultiplyReange,velocity.Size() );
	CrosshairSpreadMultiplier = 0.5 + CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairAimFactor + CrosshairShooterFactor;
}

void AShooterMain::StartCrosshairBulletFire()
{
	bIsFiringBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this,&AShooterMain::FinishCrosshairBulletFire, ShootTimeDuration, false);
}

void AShooterMain::FinishCrosshairBulletFire()
{
	bIsFiringBullet = false;
}

void AShooterMain::SpawnDefaultWeapon()
{
	if (InitWeapon)
	{
		FActorSpawnParameters params;
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(InitWeapon);
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("FuturisticSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(DefaultWeapon, GetMesh());
		}
	}
}



// Called every frame
void AShooterMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetAiminFOV(DeltaTime);
	CalculateCrossHairSpread(DeltaTime);
	CheckForItems();
	
}
void AShooterMain::SetAiminFOV(float DeltaTime) {
	if (bIsAiming) {
		currentFOV = FMath::FInterpTo(currentFOV, zoomedFOV,DeltaTime, FOVZoomInterpSpeed);
	}
	else {
		currentFOV = FMath::FInterpTo(currentFOV, regularFOV, DeltaTime, FOVZoomInterpSpeed);
		
	}
	if (FollowCamera)
		FollowCamera->FieldOfView = currentFOV;

}
void AShooterMain::SetLookRates()
{
}
void AShooterMain::CheckForItems()
{
	if (!bShouldCheckForItems) return;
	FHitResult ItemTraceResult;
	TraceUnderCrosshairs(ItemTraceResult);
	if (ItemTraceResult.bBlockingHit)
	{
		AItem* HitItem = Cast<AItem>(ItemTraceResult.GetActor());
		if (HitItem && HitItem->GetWidgetComponent())
		{
			if (CurrentCheckedItem && CurrentCheckedItem != HitItem) {
				CurrentCheckedItem->GetWidgetComponent()->SetVisibility(false);
			}
			CurrentCheckedItem = HitItem;
			HitItem->GetWidgetComponent()->SetVisibility(true);
			return;
		}
		
	}
	if (CurrentCheckedItem) {
		CurrentCheckedItem->GetWidgetComponent()->SetVisibility(false);
		CurrentCheckedItem = nullptr;
	}
}
// Called to bind functionality to input
void AShooterMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterMain::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterMain::LookUpRate);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterMain::SetFireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterMain::SetFireButtonReleased);
	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &AShooterMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShooterMain::StopJumping);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShooterMain::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShooterMain::StopAim);

}

void AShooterMain::MoveForward(float value)
{
	if (Controller != nullptr && value != 0.0f) {
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation =  FRotator(0, Rotation.Yaw, 0);
		FRotationMatrix* rMatrix = new  FRotationMatrix(YawRotation);
		FVector direction = rMatrix->GetUnitAxis(EAxis::X) ;
		AddMovementInput( direction, value);
		
	}
}

void AShooterMain::MoveRight(float value)
{
	if (Controller != nullptr && value != 0.0f) {
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);
		FRotationMatrix* rMatrix = new  FRotationMatrix(YawRotation);
		FVector direction = rMatrix->GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, value);

	}
}

void AShooterMain::Aim()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bIsAiming = true;
	BaseTurnRate = AimTurnRate;
	BlaseLookUpRate = AimLookUpRate;
	
}

void AShooterMain::StopAim()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsAiming = false;
	BaseTurnRate = hipTurnRate;
	BlaseLookUpRate = hipLookUpRate;
	
}

void AShooterMain::TurnAtRate(float value)
{
	AddControllerPitchInput(value * BlaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterMain::LookUpRate(float value)
{
	AddControllerYawInput(value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterMain::FireWeapon()
{	
	if (!Weapon) return;
	if (shootingSound) {
		//USoundConcurrency* sconc = USoundConcurrency();
		UGameplayStatics::PlaySound2D(this, shootingSound);
		ARifle* rifle = Cast<ARifle>(Weapon);
		if (rifle) {
			rifle->Shoot();
			UAnimInstance* anim = GetMesh()->GetAnimInstance();
			if (anim && montage) {
				StartCrosshairBulletFire();
				PlayShootAnimation();
			}
		}

	}
}

void AShooterMain::PlayShootAnimation()
{
	UAnimInstance* anim = GetMesh()->GetAnimInstance();
	if (!bIsAiming) {
		anim->Montage_Play(montage);
		anim->Montage_JumpToSection(FName("FireHip"), montage);
	}
	else {
		anim->Montage_Play(montage);
		anim->Montage_JumpToSection(FName("Fire"), montage);
	}
}

void AShooterMain::SetWeapon(AActor* weapon)
{
	
	if (weapon == Weapon) return;
	ARifle* rifle = Cast<ARifle>(weapon);
	if (rifle) {
		HasWeapon = true;
		const USkeletalMeshSocket* RightHandSock = GetMesh()->GetSocketByName("WeaponSocket");
		RightHandSock->AttachActor(rifle, GetMesh());
		Weapon = rifle;
	}
}
void AShooterMain::InCrementOverlappedItems(int amount)
{
	NumberOverlappedItems += amount;
	if (NumberOverlappedItems > 0)
	{
		bShouldCheckForItems = true;
	}
}
void AShooterMain::DecreaseOverlappedItems(int amount)
{
	NumberOverlappedItems -= amount;
	if (NumberOverlappedItems <= 0)
	{
		NumberOverlappedItems = 0;
		bShouldCheckForItems = false;
	}
}
bool AShooterMain::IsAiming() {
	return bIsAiming;

}
void AShooterMain::SetFireButtonPressed(){
	bIsFireButtonPressed = true;
	bShouldFire = true;
	StartFireTimer();
}
void AShooterMain::SetFireButtonReleased(){
	bIsFireButtonPressed = false;
}

void AShooterMain::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFiretimer, this, &AShooterMain::AutoFireReset,AutomaticFireRate);
	}
}

void AShooterMain::AutoFireReset()
{
	bShouldFire = true;
	if (bIsFireButtonPressed)
	{
		StartFireTimer();
	}
}

bool AShooterMain::TraceUnderCrosshairs(FHitResult& OutHitResult)
{
	FVector WorldPosition;
	FVector WorldDirection;
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}
	
	FVector2D crosshairLocation(viewportSize.X / 2.0f, viewportSize.Y / 2.0f);
	bool bCrosshhairPos = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, WorldPosition, WorldDirection);
	if (bCrosshhairPos)
	{
		FVector Start{ WorldPosition };
		FVector End{ WorldPosition + WorldDirection * 5000000.f};
		GetWorld()->LineTraceSingleByChannel(OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			return true;
		}
	}
	return false;
}




