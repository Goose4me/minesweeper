// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinesweeperBlock.generated.h"

UENUM()
enum class BlockState : uint8 {
	IDLE = 0,
	MARKED,
	REVEALED
};

UENUM()
enum class BlockRole : uint8 {
	NONE = 0,
	BLANK,
	MINE
};

/** A block that can be clicked */
UCLASS(minimalapi)
class AMinesweeperBlock : public AActor
{
	GENERATED_BODY()

private:
	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* BlockMesh;

	/** Current state */
	UPROPERTY()
	BlockState CurrentState{ BlockState::IDLE };

	/** Current state */
	UPROPERTY()
	BlockRole CurrentRole{ BlockRole::NONE };

	int MinesNearMeCount{ 0 };

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* MinesNearMeCountText;

public:

	AMinesweeperBlock();

	/** Pointer to white material used on the focused block */
	UPROPERTY()
	class UMaterial* BaseMaterial;

	/** Pointer to blue material used on inactive blocks */
	UPROPERTY()
	class UMaterialInstance* BlueMaterial;

	/** Pointer to orange material used on active blocks */
	UPROPERTY()
	class UMaterialInstance* OrangeMaterial;

	/** Pointer to red material used on mine blocks */
	UPROPERTY()
	class UMaterialInstance* RedMaterial;

	/** Pointer to red material used on marked blocks */
	UPROPERTY()
	class UMaterialInstance* BrownMaterial;

	/** Grid that owns us */
	UPROPERTY()
	class AMinesweeperBlockGrid* OwningGrid{nullptr};

	UPROPERTY()
	int BlockIndex{-1};

	void CheckBlock();
	void MarkBlock();

	void Highlight(bool bOn);
	void Reveal();

	void SetRole(BlockRole role);
	BlockRole GetRole() const;

	BlockState GetState() const{
		return CurrentState;
	}

	void IncreaseMineNearMeCount();

	int GetMinesNearMe() {
		return MinesNearMeCount;
	}

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
};



