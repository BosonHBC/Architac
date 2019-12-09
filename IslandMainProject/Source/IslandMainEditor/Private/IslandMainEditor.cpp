#include "IslandMainEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

// Engine Objects
#include "Public/Building/Components/BuildingBlockComponent.h"


// Editor Tools
#include "BlockComponentVisualizer.h"

IMPLEMENT_GAME_MODULE(FIslandMainEditorModule, IslandMainEditor);

DEFINE_LOG_CATEGORY(IslandMainEditor)

#define LOCTEXT_NAMESPACE "IslandMainEditor"

void FIslandMainEditorModule::StartupModule()
{
	UE_LOG(IslandMainEditor, Warning, TEXT("IslandMainEditor: Log Started"));

	if(GUnrealEd != NULL)
	{
		TSharedPtr<FBlockComponentVisualizer> visualizer = MakeShareable(new FBlockComponentVisualizer());

		if(visualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(UBuildingBlockComponent::StaticClass()->GetFName(), visualizer);
			visualizer->OnRegister();
		}
	}
	
}

void FIslandMainEditorModule::ShutdownModule()
{
	UE_LOG(IslandMainEditor, Warning, TEXT("IslandMainEditor: Log Ended"));

	if(GUnrealEd != NULL)
	{
		GUnrealEd->UnregisterComponentVisualizer(UBuildingBlockComponent::StaticClass()->GetFName());
	}
	
}

#undef LOCTEXT_NAMESPACE