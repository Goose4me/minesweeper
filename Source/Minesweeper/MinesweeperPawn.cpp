// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperPawn.h"
#include "MinesweeperBlock.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "MinesweeperBlockGrid.h"
#include "MinesweeperGameMode.h"

AMinesweeperPawn::AMinesweeperPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create our components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	StaticMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	//Attach our components
	StaticMeshComp->SetupAttachment(RootComponent);
	SpringArmComp->SetupAttachment(StaticMeshComp);
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	//Assign SpringArm class variables.
	SpringArmComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 4000.0f), FRotator(90.0f, 90.0f, 0.0f));
	SpringArmComp->TargetArmLength = 400.f;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 3.0f;

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AMinesweeperPawn::BeginPlay() {
	Super::BeginPlay();

	if (Grid){
		FVector NewLocation{ 128,128,0 };
		
		NewLocation *= Grid->Size;

		LeftCornerBound = { Grid->GetActorLocation().X, Grid->GetActorLocation().Y };
		RightCornerBound = { Grid->GetActorLocation().X + NewLocation.X * 2, Grid->GetActorLocation().Y + NewLocation.Y * 2 };

		NewLocation += Grid->GetActorLocation();
		NewLocation.Z = GetActorLocation().Z;

		SetActorLocation(NewLocation);
	}
}

void AMinesweeperPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector Start, Dir, End;
		PC->DeprojectMousePositionToWorld(Start, Dir);
		End = Start + (Dir * 8000.0f);
		TraceForBlock(Start, End, false);
	}

	if (!MovementInput.IsZero())
	{
		//Scale our movement input axis values by 5000 units per second
		MovementInput = MovementInput.GetSafeNormal() * 5000.0f;
		FVector NewLocation = GetActorLocation();
		NewLocation += GetActorForwardVector() * MovementInput.X * DeltaSeconds;
		NewLocation += GetActorRightVector() * MovementInput.Y * DeltaSeconds;
		NewLocation += GetActorUpVector() * MovementInput.Z * DeltaSeconds;

		// TODO: Check out of bounds not by camera center
		// Check for camera out of bounds
		if (NewLocation.X > RightCornerBound.X) NewLocation.X = RightCornerBound.X;
		if (NewLocation.Y > RightCornerBound.Y) NewLocation.Y = RightCornerBound.Y;
		if (NewLocation.X < LeftCornerBound.X) NewLocation.X = LeftCornerBound.X;
		if (NewLocation.Y < LeftCornerBound.Y) NewLocation.Y = LeftCornerBound.Y;
		if (NewLocation.Z < 1000.f) NewLocation.Z = 1000.f;
		if (NewLocation.Z > 4000.f) NewLocation.Z = 4000.f;

		SetActorLocation(NewLocation);
	}
}

void AMinesweeperPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("CheckBlock", EInputEvent::IE_Pressed, this, &AMinesweeperPawn::CheckBlock);
	PlayerInputComponent->BindAction("MarkBlock", EInputEvent::IE_Pressed, this, &AMinesweeperPawn::MarkBlock);

	PlayerInputComponent->BindAxis("MoveRight", this, &AMinesweeperPawn::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AMinesweeperPawn::MoveUp);
	PlayerInputComponent->BindAxis("Zoom", this, &AMinesweeperPawn::Zoom);
}

void AMinesweeperPawn::CheckBlock()
{
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->CheckBlock();
	}
}

void AMinesweeperPawn::MarkBlock()
{
	if (CurrentBlockFocus)
	{
		CurrentBlockFocus->MarkBlock();
	}
}

void AMinesweeperPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid())
	{
		AMinesweeperBlock* HitBlock = Cast<AMinesweeperBlock>(HitResult.Actor.Get());
		if (CurrentBlockFocus != HitBlock)
		{
			if (CurrentBlockFocus)
			{
				CurrentBlockFocus->Highlight(false);
			}
			if (HitBlock)
			{
				HitBlock->Highlight(true);
			}
			CurrentBlockFocus = HitBlock;
		}
	}
	else if (CurrentBlockFocus)
	{
		CurrentBlockFocus->Highlight(false);
		CurrentBlockFocus = nullptr;
	}
}


void AMinesweeperPawn::MoveUp(float Value)
{
	MovementInput.Y = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}

void AMinesweeperPawn::MoveRight(float Value)
{
	MovementInput.X = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}

void AMinesweeperPawn::Zoom(float Value) {
	MovementInput.Z = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}