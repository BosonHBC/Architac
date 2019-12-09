// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StaticLibrary.h"
#include "BuildingLibrary.h"
#include "Damageable.h"
#include "BlockSet.generated.h"

class UMeshComponent;
class UStaticMeshComponent;
class UBuildingBlockComponent;
class UMaterialInterface;
class UBoxAttachmentCollisionComponent;
class UCreatureLandPoint;
enum EBuildingBlockSetMaterialState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingDestroyed, ABlockSet*, i_building);

// TODO NEED TO RENAMED. Now that we have environmentals this should be called -> ABlockSet. Planning on doing this when we
// reach a stable state where no one is modifying the project.
UCLASS()
class ISLANDMAINPROJECT_API ABlockSet : public AActor, public IDamageable
{
	GENERATED_BODY()

#pragma region UI functions
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UIInteraction)
		TMap<FIntVector, AActor*> copyrelationdata;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UIInteraction)
		bool CanProcessNextAction = true;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void InteractWithUI();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void OpenWindow();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void CloseWindow();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void FocusThisWindow(AActor* sender);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void MoveIndicator(FIntVector direction, bool istheoriginalwindow);
	UFUNCTION(BlueprintCallable, Category = UIInteraction)
		FIntVector GetInverseDirectionFromKey(AActor * actor);

#pragma  endregion

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Set Properties", meta = (AllowPrivateAccess = "true"))
		bool PlayerCanDelete = false;
	
	//TODO Update scope accessors
	// Requires attachment adjacency for attachment building
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Component Requirement Properties", meta = (AllowPrivateAccess = "true"))
		bool RequireAttachment = true;

	// Requires overlap for attachment building
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Component Requirement Properties", meta = (AllowPrivateAccess = "true"))
		bool RequireOverlap = false;

	// Requires collision box overlap checking for attachment building
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Component Requirement Properties", meta = (AllowPrivateAccess = "true"))
		bool RequireBoxOverlapCollisionCheck = false;

	// Layers of overlap boxes for attachment building
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Component Requirement Properties", meta = (AllowPrivateAccess = "true"))
		TArray<TEnumAsByte<EObjectTypeQuery>>  BoxOverlapObjectTypes;

	// Custom blueprint override for attachment building
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Block Component Requirement Properties", meta = (AllowPrivateAccess = "true"))
		bool RequireCustomBlueprintCheck = false;

	// Custom blueprint override for attachment building
	UFUNCTION(BlueprintImplementableEvent, Category = "Block Component Requirement Function")
		bool CustomBlueprintAttachCheck();

	// Sets default values for this actor's properties
	ABlockSet();


	// Accessors --------
	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		TSet<UMeshComponent*> GetBlockSetMeshComponents() const ;

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		TArray<UBuildingBlockComponent*> GetBuildingBlocks() const;

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		UBuildingBlockComponent* GetRootBlockComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		FIntVector GetRootWorldIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		FRotator GetBlockSetRotation() const;

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		TArray<UBoxAttachmentCollisionComponent*> GetBoxAttachmentCollisions() const;

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		EBlockSetActiveStatus GetBlockSetActiveStatus() const;

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		TSet<ABlockSet*>  GetBuildingBlockSetInDistance(FIntVector Distance) const;

	// Modifiers --------
	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		void MoveBuildingBlockSet(FIntVector indexPosition, FVector worldPosition);

	// Function will round to ints. Best rotate in 90 degree amounts or shit will fuck up
	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		void RotateBuildingBlockSet(FRotator blockSetRotation);

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		void SetMaterial(EBuildingBlockSetMaterialState state, UMaterialInterface* material = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		void EnableBlockSet();

	UFUNCTION(BlueprintCallable, Category = "Building Block Set")
		void DisableBlockSet();

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	virtual void PostLoad() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Hierarchy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Parent", meta = (AllowPrivateAccess = "true"))
		USceneComponent* MainParent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh Parent", meta = (AllowPrivateAccess = "true"))
		USceneComponent* MeshParent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Functional Parent", meta = (AllowPrivateAccess = "true"))
		USceneComponent* FunctionalParent;
	
	// Saved Root Block Component. Constructed in Blueprint however so that a blueprint reference can be used instead.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Block Set Properties")
		UBuildingBlockComponent* RootBlockComponent;
public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// Stores Rotation State of Block Set
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block Set Properties", meta = (AllowPrivateAccess = "true"))
		FRotator m_blockSetRotation;

	// Stores Blocking Information of Block Set
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block Set Properties", meta = (AllowPrivateAccess = "true"))
		TArray<UBuildingBlockComponent*> m_blocks;

#pragma region Material Management
	
	// Stores Material Information
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Block Set Properties", meta = (AllowPrivateAccess = "true"))
		TSet<UStaticMeshComponent*> m_staticMeshes;
	TMap<UStaticMeshComponent*, TArray<UMaterialInterface*>> m_originalStaticMeshMaterialMap;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Block Set Properties", meta = (AllowPrivateAccess = "true"))
		TSet<USkeletalMeshComponent*> m_skeletalMeshes;
	TMap<USkeletalMeshComponent*, TArray<UMaterialInterface*>> m_originalSkeletalMeshMaterialMap;

	// For Attachment requirements via overlapping collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block Set Properties", meta = (AllowPrivateAccess = "true"))
		TArray<UBoxAttachmentCollisionComponent*> m_boxOverlapsForAttachment;

	TMap<UStaticMeshComponent*, TArray<UMaterialInterface*>> m_originalMaterialMap;

	// Material Setting
	void SetMaterialOnAllMeshes(UMaterialInterface* material);
	void ResetMaterialOnAllMeshes();

#pragma endregion 

#pragma region IDamageable
protected:
	UPROPERTY(BlueprintReadWrite, Category = Damageable)
		UCreatureLandPoint* LandPointComp;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Damageable)
		void GetDamage(const FDamageInfo& dInfo);
	UFUNCTION(BlueprintImplementableEvent, Category = Damageable)
		void BPOnGetdamage(float ratioOfHealth);
	void Die();
	// Health additional to block amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Damageable)
		float ExtraHealth = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Damageable)
		bool CanGetDamage = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		EBlockSetActiveStatus m_activeStatus = EBlockSetActiveStatus::DISABLED;
	
	bool m_GetDamageCoolDownFinish = true;

	FOnBuildingDestroyed OnGetKill;
private:	
	void InitializeBlockSetHealth();

public:
	UFUNCTION(BlueprintCallable, Category = Damageable)
		bool IsFullHealth();

	UCreatureLandPoint* GetLandPoint() { return LandPointComp; }



	FOnBuildingDestroyed * GetOnGetDestroyDelegate() { return &OnGetKill; }
#pragma endregion

#if WITH_EDITOR
protected:
	virtual void EditorApplyTranslation(const FVector&DeltaTranslation, bool bAltDown, bool bShiftDown, bool cCtrlDown) override;

	virtual void EditorApplyRotation(const FRotator&DeltaRotation, bool bAltDown, bool bShiftDown, bool cCtrlDown) override;

	class AWorldBuildingGrid* currentWorldEditorOnly{};
#endif
};
