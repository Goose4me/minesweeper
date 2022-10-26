// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinesweeperBlockGrid.generated.h"

class AMinesweeperBlock;

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class AMinesweeperBlockGrid : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

private:
	UPROPERTY()
	TArray<AMinesweeperBlock*> MinesweeperBlocks;

	bool isOutOfSizeBounds(int sizeIndex) {
		return !(sizeIndex >= 0 && sizeIndex < Size);
	}

public:
	AMinesweeperBlockGrid();

	const TArray<AMinesweeperBlock*>& GetBlocks() {
		return MinesweeperBlocks;
	}

	/** How many blocks have been clicked */
	int32 Score{0};

	/** Number of blocks along each side of grid */
	UPROPERTY(Category=Grid, BlueprintReadOnly)
	int32 Size;

	UPROPERTY(Category = Grid, BlueprintReadOnly)
	int32 MinesCount;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// End AActor interface

public:
	UFUNCTION()
	void FirstTouch();
	void RevealAll();
	void BlankTouched(int BlockIndex);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
};
