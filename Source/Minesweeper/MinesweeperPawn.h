// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MinesweeperPawn.generated.h"

UCLASS()
class AMinesweeperPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
private:
	FVector2D LeftCornerBound{};
	FVector2D RightCornerBound{};
	FVector2D BoundOffset{};

	UPROPERTY(Category = Zooming, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float maxZoomDistance{ 4000.f };

	UPROPERTY(Category = Zooming, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float minZoomDistance{ 1000.f };

protected:
	void BeginPlay() override;

	void MoveUp(float Value);
	void MoveRight(float Value);
	void Zoom(float Value);

	//Input variable
	FVector MovementInput;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
	class AMinesweeperBlockGrid* Grid;

	void CheckBlock();
	void MarkBlock();
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class AMinesweeperBlock* CurrentBlockFocus = nullptr;
};
