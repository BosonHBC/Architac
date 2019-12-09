// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Damageable.h"
#include "Containers/Map.h"
#include "Building/Actors/BlockSet.h"
#include "VenturePawn.generated.h"

UENUM(BlueprintType)
enum class EVenturePlayerState : uint8 
{
	E_EXPLORE UMETA(DisplayName = "Explore"),
	E_UIINTERACT UMETA(DisplayName = "Interact with ui"),
};

UCLASS()
class ISLANDMAINPROJECT_API AVenturePawn : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
#pragma region UI functions
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UIInteraction, meta = (AllowPrivateAccess = "true"))
		class ABlockSet* InteractingBlockSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UIInteraction)
		TMap<FIntVector, AActor*> copyrelationdata;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void OpenWindow();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void CloseWindow();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void FocusThisWindow(AActor* sender);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = UIInteraction)
		void MoveUIIndicator(FIntVector direction, bool istheoriginalwindow);
	UFUNCTION(BlueprintCallable, Category = UIInteraction)
		FIntVector GetKeyFromValueForRelationData(AActor* actor);

	float m_previousx = 0.0f;
	float m_previousy = 0.0f;

#pragma  endregion

	// Sets default values for this character's properties
	AVenturePawn();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComp; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UInventoryComponent* GetInventoryComponent() { return InventoryComp; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EVenturePlayerState VenturePawnState;

	/* ----Useful HEAD
	void InteractWithTool(class AAPInteractItemBase* interactBase);
	*/

private:
	bool m_lockInput;
protected:
	virtual void BeginPlay() override;
#pragma region PlayerInputFunctions
	void MoveForward(float axis);
	void MoveRight(float axis);
	void Jump();
	void Crouch();
	void UnCrouch();
	void OnMouseClick();
	void OnMouseHold();
	void OnMouseRelease();
	void OnMouseRightClick();
	void Interact();

	UFUNCTION(BlueprintImplementableEvent, Category = Input)
		void BPSwitchToItem1();
	UFUNCTION(BlueprintImplementableEvent, Category = Input)
		void BPSwitchToItem2();
	UFUNCTION(BlueprintImplementableEvent, Category = Input)
		void BPSwitchToItem3();

	void SwitchToItem1();
	void SwitchToItem2();
	void SwitchToItem3();
	void SwitchToItem4();

#pragma endregion


#pragma region Tools

protected:
	class AToolMover* m_toolMover;
	UPROPERTY(EditDefaultsOnly, Category = Tool)
		TSubclassOf<class AToolMover> ToolMoverClass;

	UPROPERTY(EditDefaultsOnly, Category = Tool)
	FVector MoverRelativeLocation;
	UPROPERTY(EditDefaultsOnly, Category = Tool)
	FRotator MoverRelativeRotatoin;

	bool m_OnMouseHold;

	void SpawnMover();
	void UpdateMoverTransformation();

#pragma endregion

#pragma region CharacterMovement
public:
	// When player is attacking, restrict the control of the player
	void ChangeCharacterVelocity(float _speed);
	void BlockCharacterRotation(float RotationRate_Yaw);
#pragma endregion
# pragma region Damageable
protected:
	// Interface stuffs, only override get damage function and the die function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Damageable)
		void GetDamage(const FDamageInfo& dInfo);
	UFUNCTION(BlueprintImplementableEvent, Category = Damageable)
		void BPOnGetdamage(float ratioOfHealth);

	void Die();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damageable)
		bool m_GetDamageCoolDownFinish = true;
	
	FVector m_knockback;
	void HandleKnockBack();
# pragma endregion

private:
	/** Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComp;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* InteractPointComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCameraControlComponent* CameraControlComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent* InventoryComp;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
