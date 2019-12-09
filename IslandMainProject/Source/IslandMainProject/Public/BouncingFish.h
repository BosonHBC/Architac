// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Damageable.h"
#include "BouncingFish.generated.h"

UCLASS()
class ISLANDMAINPROJECT_API ABouncingFish : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABouncingFish();

protected:

#pragma region GroundMovement
	void TraceToGround();
	void BounceToRandomDirection();
	float m_BounceDistanceThreshold;
	UPROPERTY(EditAnywhere, Category = FishGroundMovement)
		float ImpulsePower;
	UPROPERTY(EditAnywhere, Category = FishGroundMovement)
		float TailMoveSpeed;
	UPROPERTY(EditAnywhere, Category = FishGroundMovement)
		float MaxSwingAngle;
	UPROPERTY(EditAnywhere, Category = FishGroundMovement)
		float HorizontalBounceScalar;

#pragma endregion
# pragma region Damageable

	// Interface stuffs, only override get damage function and the die function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Damageable)
	void GetDamage(const FDamageInfo& dInfo);
	UFUNCTION(BlueprintImplementableEvent, Category = Damageable)
		void BPOnGetdamage(float ratioOfHealth);
	void Die();

	void DropAllItems();

# pragma endregion

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RotateBodyYaw(float t);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SuperMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* SphereComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UWidgetComponent* WidgetComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		class UInventoryComponent* LootComp;

	float m_startTime;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};