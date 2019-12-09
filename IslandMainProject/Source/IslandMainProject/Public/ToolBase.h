// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ToolBase.generated.h"


USTRUCT(BlueprintType)
struct FUseableTool : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FUseableTool() {}
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FName ToolID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TSubclassOf<class AToolBase> ToolActorClass;
};

UCLASS()
class ISLANDMAINPROJECT_API AToolBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolBase();

	UFUNCTION(BlueprintCallable, Category = Tool)
	FORCEINLINE FName GetToolID() const { return ToolID; }
	FORCEINLINE bool GetIsUsing() const { return m_using; }

	virtual void StartUseTool();
	virtual void UseToolFinish();
	void SetMover(class AToolMover* mover);
	void ToggleTool(bool enable);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tool)
		FName ToolID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SuperMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* InteractPointComp;

	UFUNCTION(BlueprintImplementableEvent)
		void OnInteractableEnterBP(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInteractableLeftBP(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintImplementableEvent)
		void DuringInteractable();

	virtual	void OnInteractableEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual	void OnInteractableLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tool, meta = (AllowPrivateAccess = "true"))
	float CoolDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tool)
	bool m_using;

	bool m_enable;

	FTimerHandle m_CastHandle;
	class AToolMover* m_parentMover;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
