// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperBlock.h"
#include "MinesweeperBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"
#include "Components/TextRenderComponent.h"

AMinesweeperBlock::AMinesweeperBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BrownMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
			, BrownMaterial(TEXT("/Game/Puzzle/Meshes/BrownMaterial.BrownMaterial"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.f,1.f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	BrownMaterial = ConstructorStatics.BrownMaterial.Get();

	MinesNearMeCountText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("MinesText"));
	MinesNearMeCountText->SetRelativeLocation(FVector(0, 0.f, 58.f));
	MinesNearMeCountText->SetRelativeRotation(FRotator(90.f, 0.f, 180.f));
	MinesNearMeCountText->SetRelativeScale3D(FVector(6));
	MinesNearMeCountText->SetText(FText::AsNumber(0));
	MinesNearMeCountText->SetVerticalAlignment(EVRTA_TextCenter);
	MinesNearMeCountText->SetHorizontalAlignment(EHTA_Center);
	MinesNearMeCountText->SetupAttachment(DummyRoot);
	MinesNearMeCountText->SetVisibility(false);
}

void AMinesweeperBlock::CheckBlock()
{
	if (CurrentRole == BlockRole::NONE) {
		CurrentRole = BlockRole::BLANK;

		if (OwningGrid) {
			OwningGrid->FirstTouch();
		}
	}
	else if (CurrentRole == BlockRole::MINE) {
		
		if (OwningGrid) {
			OwningGrid->RevealAll();
		}

		return;
	}

	if (CurrentState != BlockState::REVEALED) {
		Reveal();

		if (MinesNearMeCount == 0) {
			if (OwningGrid) {
				OwningGrid->BlankTouched(BlockIndex);
			}
		}
	}
}

void AMinesweeperBlock::MarkBlock()
{
	if (CurrentState == BlockState::IDLE)
	{
		CurrentState = BlockState::MARKED;

		// Change material
		BlockMesh->SetMaterial(0, BrownMaterial);
	} else if (CurrentState == BlockState::MARKED) {
		// Change material
		BlockMesh->SetMaterial(0, BaseMaterial);

		CurrentState = BlockState::IDLE;
	}
}

void AMinesweeperBlock::Highlight(bool bOn)
{
	// Do not highlight if the block has already been activated.
	if (CurrentState!= BlockState::IDLE)
	{
		return;
	}

	if (bOn)
	{
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else
	{
		BlockMesh->SetMaterial(0, BlueMaterial);
	}
}

void AMinesweeperBlock::Reveal() {

	if (CurrentState == BlockState::REVEALED) {
		return;
	}

	switch (CurrentRole)
	{
	case BlockRole::NONE:
		GetBlockMesh()->SetMaterial(0, BaseMaterial);
		CurrentState = BlockState::IDLE;
		break;
	case BlockRole::BLANK:
		GetBlockMesh()->SetMaterial(0, OrangeMaterial);
		break;
	case BlockRole::MINE:
		GetBlockMesh()->SetMaterial(0, RedMaterial);
		break;
	default:
		break;
	}

	CurrentState = BlockState::REVEALED;

	if(MinesNearMeCount>0) {
		MinesNearMeCountText->SetVisibility(true);
	}
}

void AMinesweeperBlock::SetRole(BlockRole role) {
	CurrentRole = role;

	if (role == BlockRole::MINE) {
		MinesNearMeCount = 0;
		MinesNearMeCountText->SetText(FText::AsNumber(0));
	}
}

BlockRole AMinesweeperBlock::GetRole() const {
	return CurrentRole;
}

void AMinesweeperBlock::IncreaseMineNearMeCount() {
	MinesNearMeCount++;

	MinesNearMeCountText->SetText(FText::AsNumber(MinesNearMeCount));
}