// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SMAA : ModuleRules
{
	public SMAA(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "SMAA/Private",
                // Access to PostProcess internals - needed for proper integration
                System.IO.Path.Combine(EngineDirectory, "Source/Runtime/Renderer/Private"),
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "RenderCore",      // For shader compilation, RHI types
                "Renderer",        // For FSceneViewExtension, RDG, PostProcess
                "RHI",            // For RHI command lists, resources
                "Projects",       // For IPluginManager (shader path mapping)
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
