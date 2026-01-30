// Copyright Epic Games, Inc. All Rights Reserved.

#include "SMAA.h"
#include "SMAASceneViewExtension.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "SMAATextures.h"
#include "SceneViewExtension.h" 

#define LOCTEXT_NAMESPACE "FSMAAModule"

void FSMAAModule::StartupModule()
{
    // Map the plugin's shader directory to a virtual path
    // This is CRITICAL - without this, shader compilation will fail
    FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SMAA"))->GetBaseDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugin/SMAA"), PluginShaderDir);

    UE_LOG(LogTemp, Log, TEXT("SMAA Plugin: Shader directory mapped to %s"), *PluginShaderDir);

    // Don't create the extension here - GEngine isn't ready yet!
    // Instead, register a delegate to create it when the engine is ready
    FCoreDelegates::OnPostEngineInit.AddRaw(this, &FSMAAModule::OnPostEngineInit);
    UE_LOG(LogTemp, Log, TEXT("SMAA Plugin: Initialized"));
}

void FSMAAModule::OnPostEngineInit()
{
    UE_LOG(LogTemp, Warning, TEXT("SMAA: Engine initialized, loading textures and creating scene view extension"));

    // CRITICAL: Load textures on GAME THREAD during startup
    FSMAATextures::Get().LoadTextures();

    // Just keep a reference - the static inside GetInstance keeps it alive
    SMAASceneViewExtension::GetInstance();
    bExtensionCreated = true;

    UE_LOG(LogTemp, Warning, TEXT("SMAA: Scene view extension registered"));
}

void FSMAAModule::ShutdownModule()
{
    // Unregister view extension
    //SMAAViewExtension.Reset();
    // The extension is kept alive by the static in GetInstance, so we don't need to manage it
    FCoreDelegates::OnPostEngineInit.RemoveAll(this);
    UE_LOG(LogTemp, Log, TEXT("SMAA Plugin: Module shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSMAAModule, SMAA)