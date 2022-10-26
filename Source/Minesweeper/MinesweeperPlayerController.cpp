// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperPlayerController.h"

AMinesweeperPlayerController::AMinesweeperPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Hand;
	SetInputMode(FInputModeGameAndUI());
}
