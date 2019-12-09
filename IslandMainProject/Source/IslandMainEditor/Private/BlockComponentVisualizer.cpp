// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockComponentVisualizer.h"

#include "Public/Building/Components/BuildingBlockComponent.h"
#include "Public/Building/Actors/BlockSet.h"
#include "Public/Building/Actors/WorldBuildingGrid.h"




IMPLEMENT_HIT_PROXY(HTargetingVisProxy, HComponentVisProxy)
IMPLEMENT_HIT_PROXY(HTargetProxy, HTargetingVisProxy)

FBlockComponentVisualizer::FBlockComponentVisualizer()
{
}

FBlockComponentVisualizer::~FBlockComponentVisualizer()
{
}

void FBlockComponentVisualizer::OnRegister()
{
}

void FBlockComponentVisualizer::EndEditing()
{
}

//bool FBlockComponentVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient,
//	HComponentVisProxy* VisProxy, const FViewportClick& Click)
//{
//	bool bEditing = false;
//
//	/*if (VisProxy && VisProxy->Component->IsValid())
//	{
//		bEditing = true;
//
//		if (VisProxy->IsA(HTargetProxy::StaticGetType()))
//		{
//			HTargetProxy* Proxy = (HTargetProxy*)VisProxy;
//
//			SelectedTargetIndex = VisProxy->TargetIndex;
//		}
//	}
//	else
//	{
//		SelectedTargetIndex = INDEX_NONE;
//	}*/
//
//	return bEditing;
//}

void FBlockComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View,
	FPrimitiveDrawInterface* PDI)
{
	if(const UBuildingBlockComponent* blockComponent = Cast<const UBuildingBlockComponent>(Component))
	{
		const FLinearColor rootPointColor = FLinearColor(FColor::Blue);
		const FLinearColor localRootPointColor = FLinearColor(FColor::Cyan);

		
		const FLinearColor lineDefaultColor = FLinearColor(FColor::Silver);
		const FLinearColor lineNotAttachableColor = FLinearColor(FColor::Red);
		const FLinearColor lineOverlapColor = FLinearColor(FColor::Yellow);
		

		
		const float pointSize = 20.0f;
		const float blockSize = AWorldBuildingGrid::blockSize;
		
		TMap<FIntVector, FBuildingIndex> indexes =  blockComponent->GetIndexPositions();

		ABlockSet* blockSetParent = blockComponent->GetBlockSetParent();
		if (blockSetParent == nullptr)
			return;
	
		// Draw box lines for each index in this component
		int i = 0;
		for (auto& pair : indexes)
		{
			FBuildingIndex& buildIndex = pair.Value;
			FVector localIndexInWorldSpace = blockSetParent->GetActorLocation() + (FVector(buildIndex.LocalIndex.X, buildIndex.LocalIndex.Y, buildIndex.LocalIndex.Z) * blockSize);
			// Calculate cube points
			float z1 = blockSize / 2.0f;
			FVector p1 = localIndexInWorldSpace + FVector(blockSize / 2.0f, blockSize / 2.0f, z1);
			FVector p2 = localIndexInWorldSpace + FVector(blockSize / 2.0f, blockSize / -2.0f, z1);
			FVector p3 = localIndexInWorldSpace + FVector(blockSize / -2.0f, blockSize / 2.0f, z1);
			FVector p4 = localIndexInWorldSpace + FVector(blockSize / -2.0f, blockSize / -2.0f, z1);


			float z2 = z1 * -1.0f;
			FVector p11 = localIndexInWorldSpace + FVector(blockSize / 2.0f, blockSize / 2.0f, z2);
			FVector p22 = localIndexInWorldSpace + FVector(blockSize / 2.0f, blockSize / -2.0f, z2);
			FVector p33 = localIndexInWorldSpace + FVector(blockSize / -2.0f, blockSize / 2.0f, z2);
			FVector p44 = localIndexInWorldSpace + FVector(blockSize / -2.0f, blockSize / -2.0f, z2);


			FLinearColor lineColor;
			switch(blockComponent->BlockType)
			{
				case EBlockType::ATTACHABLE:
					lineColor = lineDefaultColor;
					break;
				case EBlockType::NOT_ATTACHABLE:
					lineColor = lineNotAttachableColor;
					break;
				case EBlockType::OVERLAP:
					lineColor = lineOverlapColor;
					break;
				default:
					lineColor = lineDefaultColor;
			}
			
			PDI->SetHitProxy(new HTargetProxy(Component, i));

			PDI->DrawLine(p1, p11, lineColor, SDPG_Foreground);
			PDI->DrawLine(p2, p22, lineColor, SDPG_Foreground);
			PDI->DrawLine(p3, p33, lineColor, SDPG_Foreground);
			PDI->DrawLine(p4, p44, lineColor, SDPG_Foreground);

			PDI->DrawLine(p1, p2, lineColor, SDPG_Foreground);
			PDI->DrawLine(p1, p3, lineColor, SDPG_Foreground);
			PDI->DrawLine(p4, p2, lineColor, SDPG_Foreground);
			PDI->DrawLine(p4, p3, lineColor, SDPG_Foreground);

			PDI->DrawLine(p11, p22, lineColor, SDPG_Foreground);
			PDI->DrawLine(p11, p33, lineColor, SDPG_Foreground);
			PDI->DrawLine(p44, p22, lineColor, SDPG_Foreground);
			PDI->DrawLine(p44, p33, lineColor, SDPG_Foreground);

			PDI->SetHitProxy(NULL);

			i++;
		}

		
		
		// Draw point for the local root index
		UBuildingBlockComponent* rootBlockComponent = blockSetParent->GetRootBlockComponent();
		FBuildingIndex localRootIndex;
		if (!blockComponent->GetLocalRootIndex(localRootIndex))
			return;
		FVector localRootIndexInWorldPosition = blockSetParent->GetActorLocation() + (FVector(localRootIndex.LocalIndex.X, localRootIndex.LocalIndex.Y, localRootIndex.LocalIndex.Z) * blockSize);
		
		// Draw local color
		if (rootBlockComponent == nullptr || blockComponent != rootBlockComponent)
		{
			PDI->DrawPoint(localRootIndexInWorldPosition, localRootPointColor, pointSize, SDPG_Foreground);
		}
		// Draw root of Blockset color
		else
		{
			PDI->DrawPoint(localRootIndexInWorldPosition, rootPointColor, pointSize, SDPG_Foreground);
		}
		
	}
}

//bool FBlockComponentVisualizer::HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key,
//	EInputEvent Event)
//{
//	if (Key == EKeys::Delete)
//	{
//		return true;
//	}
//	return false;
//}

