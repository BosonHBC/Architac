#pragma once

#include "CoreMinimal.h"

#include "StaticLibrary.h"

#include "BuildingLibrary.generated.h"

class ABlockSet;

UENUM(BlueprintType, Category = "Block Set Functional Status")
enum class EBlockSetActiveStatus : uint8
{
	ENABLED UMETA(DisplayName = "Enabled"),
	DISABLED UMETA(DisplayName = "Disabled"),
};

UENUM(BlueprintType, Category = "Block Set Damage Status")
enum class EBlockSetDamageStatus : uint8
{
	DAMAGED	UMETA(DisplayName = "Damaged")
};

UENUM(BlueprintType, Category = "Building Block Set Material")
enum EBuildingBlockSetMaterialState
{
	BUILDING_BLOCK_MAT_UNPLACEABLE,
	BUILDING_BLOCK_MAT_PLACEABLE,
	BUILDING_BLOCK_MAT_SHOW_DELETE,
	BUILDING_BLOCK_MAT_STANDARD
};

UENUM(BlueprintType, Category = "SenderReceiverType")
enum class ESenderReceiverMessageType : uint8
{
	REGISTER_SENDER UMETA(DisplayName = "Register Sender"),
	UNREGISTER_SENDER UMETA(DisplayName = "Un Register Sender"),
};

USTRUCT(BlueprintType)
struct FBlockSetPropertiesRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FBlockSetPropertiesRow() {
		
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABlockSet> BuildingBlockSetBlueprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* Thumbnail;
};

USTRUCT(BlueprintType)
struct FBlockSetData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Set Data", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ABlockSet> BlockSetBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Set Data", meta = (AllowPrivateAccess = "true"))
		FDataTableRowHandle CostRecord;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Set Data", meta = (AllowPrivateAccess = "true"))
		FDataTableRowHandle PropertiesRecord;// Currently dont do shit. But maybe later
	FBlockSetData()
	{
		
	}
private:
	
};

UCLASS()
class UBuildingLibrary : public UObject
{
	GENERATED_BODY()


};

