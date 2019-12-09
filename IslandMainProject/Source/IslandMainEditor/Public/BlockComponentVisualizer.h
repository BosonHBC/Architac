// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ComponentVisualizer.h"
#include "InputCoreTypes.h"
#include "UnrealClient.h"

#include "ComponentVisualizer.h"

/**Base class for clickable targeting editing proxies*/
struct HTargetingVisProxy : public HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HTargetingVisProxy(const UActorComponent* InComponent)
		: HComponentVisProxy(InComponent, HPP_Wireframe)
	{}
};

/**Proxy for target*/
struct HTargetProxy : public HTargetingVisProxy
{
	DECLARE_HIT_PROXY();

	HTargetProxy(const UActorComponent* InComponent, int32 InTargetIndex)
		: HTargetingVisProxy(InComponent)
		, TargetIndex(InTargetIndex)
	{}

	int32 TargetIndex;
};

//class FTargetingVisualizerCommands : public TCommands < FTargetingVisualizerCommands >
//{
//public:
//	FTargetingVisualizerCommands() : TCommands <FTargetingVisualizerCommands>
//		(
//			"TargetingComponentVisualizer",
//			LOCTEXT("TagetingComponentVisualizer", "Targeting Component Visualizer"),
//			NAME_None,
//			FEditorStyle::GetStyleSetName()
//			) {}
//
//	virtual void RegisterCommands() override
//	{
//		UI_COMMAND(Duplicate, "Duplicate Target", "Duplicate the current target.", EUserInterfaceActionType::Button, FInputGesture());
//	}
//
//public:
//	/** Duplicate Target */
//	TSharedPtr<FUICommandInfo> Duplicate;
//
//};


/**
 * 
 */
class FBlockComponentVisualizer : public FComponentVisualizer
{
public:
	FBlockComponentVisualizer();
	~FBlockComponentVisualizer();

	virtual void OnRegister() override;
	virtual void EndEditing() override;

	//virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	
	//virtual bool HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
private:
};
