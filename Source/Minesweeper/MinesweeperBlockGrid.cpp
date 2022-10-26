// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperBlockGrid.h"
#include "MinesweeperBlock.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

static const TMap<FString, TPair<int, int>> nearMe{ {"Top", TPair<int,int>{1,0}},
								 {"TopLeft", TPair<int,int>{1,-1}},
								 {"TopRight", TPair<int,int>{1,1}},
								 {"Bottom", TPair<int,int>{-1,0}},
								 {"BottomLeft", TPair<int,int>{-1,-1}},
								 {"BottomRight", TPair<int,int>{-1,1}},
								 {"Left",TPair<int,int>{0,-1}},
								 {"Right",TPair<int,int>{0,1}} };

AMinesweeperBlockGrid::AMinesweeperBlockGrid()
{
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Set defaults
	Size = 8;
	BlockSpacing = 0;
	MinesCount = 10;
}

void AMinesweeperBlockGrid::BeginPlay()
{
	Super::BeginPlay();

	const FString JsonFilePath = FPaths::ProjectContentDir() + "/Settings/FieldSettings.json";

	if (FPaths::FileExists(JsonFilePath)) {
		FString JsonString; //Json converted to FString
		FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

		//Create a json object to store the information from the json string
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			// Set Size if exists
			if (JsonObject->GetIntegerField("Size") != NULL) {
				Size = JsonObject->GetIntegerField("Size");
			}

			// Set mines count variable if exists
			if (JsonObject->GetIntegerField("MinesCount") != NULL) {
				MinesCount = JsonObject->GetIntegerField("MinesCount");
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no file with path [%s]"), *JsonFilePath);
	}
	
	// Number of blocks
	const int32 NumBlocks = Size * Size;

	if (NumBlocks <= MinesCount) {
		MinesCount = NumBlocks - 1;
	}

	FVector origin{0,0,0}, extent{0,0,0};

	// Loop to spawn each block
	for(int32 BlockIndex=0; BlockIndex<NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex / Size) * (extent.X*2 + BlockSpacing); // Divide by dimension
		const float YOffset = (BlockIndex % Size) * (extent.Y*2 + BlockSpacing); // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AMinesweeperBlock*  NewBlock = GetWorld()->SpawnActor<AMinesweeperBlock>(BlockLocation, FRotator(0,0,0));

		NewBlock->GetActorBounds(false, origin, extent);

		// Tell the block about its owner
		if (NewBlock != nullptr)
		{
			NewBlock->OwningGrid = this;
			NewBlock->BlockIndex = BlockIndex;
		}

		MinesweeperBlocks.Add(NewBlock);
	}
}

void AMinesweeperBlockGrid::Tick(float DeltaTime) {
	//if (timerWidget) {
	//	UE_LOG(LogTemp, Warning, TEXT("Done"));

	//	timerWidget->TimerText->SetText(FText::FromString("Penis"));
	//}
}

void AMinesweeperBlockGrid::FirstTouch() {
	GetWorld()->GetAuthGameMode();
	FRandomStream stream(FDateTime::Now().ToUnixTimestamp());

	for (int i = 0; i < MinesCount; ++i) {
		int index = stream.RandRange(0, MinesweeperBlocks.Num() - 1);
		
		auto mineBlock = MinesweeperBlocks[index];

		while (mineBlock->GetRole() != BlockRole::NONE){
			index = stream.RandRange(0, MinesweeperBlocks.Num() - 1);
			mineBlock = MinesweeperBlocks[index];
		}

		mineBlock->SetRole(BlockRole::MINE);
		
		int row = index / Size;
		int column = index % Size;

		for (auto el : nearMe) {
			int rowToCheck = row + el.Value.Key;
			int columnToCheck = column + el.Value.Value;
			int blockIndexToCheck = rowToCheck * Size + columnToCheck;

			if (!isOutOfSizeBounds(rowToCheck) && !isOutOfSizeBounds(columnToCheck) && MinesweeperBlocks[blockIndexToCheck]->GetRole() != BlockRole::MINE) {
				MinesweeperBlocks[blockIndexToCheck]->IncreaseMineNearMeCount();
			}
		}
	}

	for (auto block : MinesweeperBlocks) {
		if (block->GetRole() == BlockRole::NONE) {
			block->SetRole(BlockRole::BLANK);
		}
	}
}

void AMinesweeperBlockGrid::RevealAll() {
	for (auto block : MinesweeperBlocks) {
		block->Reveal();
	}
}

void AMinesweeperBlockGrid::BlankTouched(int BlockIndex) {
	if (MinesweeperBlocks[BlockIndex]->GetMinesNearMe() != 0) {
		return;
	}

	int row = BlockIndex / Size;
	int column = BlockIndex % Size;

	for (auto el : nearMe) {
		int rowToCheck = row + el.Value.Key;
		int columnToCheck = column + el.Value.Value;
		int blockIndexToCheck = rowToCheck * Size + columnToCheck;
		
		if (isOutOfSizeBounds(rowToCheck) || isOutOfSizeBounds(columnToCheck)) {
			continue;
		}

		const auto block = MinesweeperBlocks[blockIndexToCheck];

		if (block->GetState() != BlockState::REVEALED && block->GetRole() == BlockRole::BLANK) {
			block->Reveal();

			BlankTouched(blockIndexToCheck);
		}
	}
}

#undef LOCTEXT_NAMESPACE
