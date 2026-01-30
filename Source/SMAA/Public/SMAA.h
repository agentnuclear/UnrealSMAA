// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSMAASceneViewExtension;

class FSMAAModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void OnPostEngineInit();
	TSharedPtr<FSMAASceneViewExtension, ESPMode::ThreadSafe> ViewExtension;
	bool bExtensionCreated = false;  // Track if we've created it
};
