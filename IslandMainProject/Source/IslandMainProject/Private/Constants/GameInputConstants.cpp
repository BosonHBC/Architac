// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInputConstants.h"
#include "NameTypes.h"

// Constants for Inputs to avoid magic strings

// TODO something better than static FNames?

// Misc
const FName GameInputConstants::CONFIRM = FName("Confirm");
const FName GameInputConstants::CHANGE_GAME_MODE = FName("ChangeGameMode");

// Build Move Keyboard
const FName GameInputConstants::MOVE_UP = FName("MoveUp");
const FName GameInputConstants::MOVE_DOWN = FName("MoveDown");
const FName GameInputConstants::MOVE_LEFT = FName("MoveLeft");
const FName GameInputConstants::MOVE_RIGHT = FName("MoveRight");
const FName GameInputConstants::MOVE_ASCEND = FName("MoveAscend");
const FName GameInputConstants::MOVE_DESCEND = FName("MoveDescend");

// Build Cycle Piece
const FName GameInputConstants::CYCLE_BLOCKSET_LEFT = FName("CyclePieceLeft");
const FName GameInputConstants::CYCLE_BLOCKSET_RIGHT = FName("CyclePieceRight");

// Build UI
const FName GameInputConstants::BUILD_TOGGLE_MODE = FName("Build_ToggleMode");
const FName GameInputConstants::BUILD_GOTO_SELECT = FName("Build_GoToSelect");
const FName GameInputConstants::BUILD_GOTO_OBSERVE = FName("Build_GoToObserve");
const FName GameInputConstants::BUILD_GOTO_PLACE = FName("Build_GoToPlace");
const FName GameInputConstants::BUILD_GOTO_DELETE = FName("Build_GoToDelete");

// Axis
const FName GameInputConstants::X_CAMERA_AXIS = FName("XCameraAxis");
const FName GameInputConstants::Y_CAMERA_AXIS = FName("YCameraAxis");

const FName GameInputConstants::X_MOVE_AXIS = FName("XMoveAxis");
const FName GameInputConstants::Y_MOVE_AXIS = FName("YMoveAxis");
const FName GameInputConstants::Z_MOVE_AXIS = FName("ZMoveAxis");

const FName GameInputConstants::ROTATE_MOVE_AXIS = FName("RotateMoveAxis");





