// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DataTable.h"
#include "StaticLibrary.h"
#include "ResourceGenerationComponent.h"
#include "IndicatorComponent.generated.h"

UENUM(BlueprintType)
enum class EIndicatorState : uint8
{
	E_DAMAGE    UMETA(DisplayName = "DAMAGE"),
	E_NOPWER    UMETA(DisplayName = "NOPWER"),
	E_INPUTTIP  UMETA(DisplayName = "INPUTTIP"),
	E_RESOURCE  UMETA(DisplayName = "RESOURCE"),
};


UCLASS( ClassGroup=(EnemyAI), Blueprintable, meta=(BlueprintSpawnableComponent) )
class ISLANDMAINPROJECT_API UIndicatorComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIndicatorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Indicator)
		float PerceptionRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Indicator)
		bool IsFrontSide = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Indicator)
		EIndicatorState IndicatorState;
	UFUNCTION(BlueprintCallable, Category = Indicator)
		UTexture2D* GetTexture()const {return texture;}
	UFUNCTION(BlueprintCallable, Category = Indicator)
		void SetTexture(UTexture2D * i_texture) {texture = i_texture;}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* PerceptionCapsuleComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Indicator)
		UTexture2D* texture;
	UFUNCTION(BlueprintImplementableEvent, Category = Indicator)
		void BPOnBeginPercept();
	UFUNCTION(BlueprintImplementableEvent, Category = Indicator)
		void BPOnStopPercept();
	UFUNCTION()
		void OnBeginPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnStopPercept(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool IsAlwayPerceptable = true;;

	// For rotation animation
	float rotationspeed = 40.0f;

	FRotator rotation = FRotator(0, 0, 0);

	float timer = 0.0;

	// For swapping texture
	UPROPERTY(EditDefaultsOnly, Category = Indicator)
	FDataTableRowHandle UIIndicatorDataTable;
	UPROPERTY()
	FBuildingIndicationRow m_UIindicatortablerow;


	// For different type of data
	UPROPERTY()
		class   UInventoryComponent* m_inventorycomponent;
	UPROPERTY()
		class	UResourceGenerationComponent* m_generationcomponent;
	UPROPERTY()
		class ABlockSet* thisbuilding;
};
