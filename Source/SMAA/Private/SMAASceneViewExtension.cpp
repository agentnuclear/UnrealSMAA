// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAASceneViewExtension.h"
#include "SMAARendering.h"
#include "SMAASettings.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessMaterial.h"
#include "SceneView.h"
#include "SceneViewExtension.h"
#include "SceneViewExtensionContext.h"




SMAASceneViewExtension::SMAASceneViewExtension(const FAutoRegister& InAutoRegister) : FSceneViewExtensionBase(InAutoRegister)
{
    UE_LOG(LogTemp, Log, TEXT("SMAA Scene View Extension created"));
}


FScreenPassTexture SMAASceneViewExtension::PostProcessPassCallback_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs)
{
    UE_LOG(LogTemp, Warning, TEXT("SMAA: PostProcessPassCallback_RenderThread called"));

    //const FViewInfo& ViewInfo = static_cast<const FViewInfo&>(View);
    //FSMAASettings Settings = FSMAASettings::GetRuntimeSettings();

    //if (!SMAARendering::ShouldRenderSMAA(ViewInfo))
    //{
    //    // Return the original input unchanged
    //    FScreenPassTexture Output;
    //    Output.Texture = Inputs.GetInput(EPostProcessMaterialInput::SceneColor).Texture;
    //    Output.ViewRect = ViewInfo.ViewRect;
    //    return Output;
    //}

    //// Get input scene color - UE 5.1 way
    //FScreenPassTexture SceneColor(
    //    Inputs.GetInput(EPostProcessMaterialInput::SceneColor).Texture,
    //    ViewInfo.ViewRect
    //);

    //if (!SceneColor.IsValid())
    //{
    //    // Return the original input unchanged
    //    FScreenPassTexture Output;
    //    Output.Texture = Inputs.GetInput(EPostProcessMaterialInput::SceneColor).Texture;
    //    Output.ViewRect = ViewInfo.ViewRect;
    //    return Output;
    //}

    //UE_LOG(LogTemp, Warning, TEXT("SMAA: Running SMAA passes via callback"));

    //return SMAARendering::AddSMAAPasses(GraphBuilder, ViewInfo, SceneColor, Settings);

    const FViewInfo& ViewInfo = static_cast<const FViewInfo&>(View);
    FSMAASettings Settings = FSMAASettings::GetRuntimeSettings();

    FScreenPassTexture SceneColor = Inputs.GetInput(EPostProcessMaterialInput::SceneColor);
    
    FScreenPassTexture SceneDepth;
    // Note: SceneDepth access via Inputs.SceneTextures is currently disabled due to compilation issues.
    // The plugin will fallback to using SceneColor for edge detection (Luma/Color mode).
    // if (Inputs.SceneTextures)
    // {
    //     SceneDepth = FScreenPassTexture((*Inputs.SceneTextures)->SceneDepthTexture, SceneColor.ViewRect);
    // }

    if (!SceneColor.IsValid() || !Settings.bEnabled)
    {
        return SceneColor;
    }

    if (!SMAARendering::ShouldRenderSMAA(ViewInfo))
    {
        return SceneColor;
    }

    return SMAARendering::AddSMAAPasses(
        GraphBuilder,
        ViewInfo,
        SceneColor,
        SceneDepth,
        Settings
    );

}


void SMAASceneViewExtension::PostRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily)
{
    UE_LOG(LogTemp, Warning, TEXT("SMAA: PostRenderViewFamily_RenderThread called"));
}

void SMAASceneViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
    UE_LOG(LogTemp, Warning, TEXT("SMAA: SubscribeToPostProcessingPass called for pass %d"), (int32)PassId);

    // Subscribe to Tonemap pass
    if (PassId == EPostProcessingPass::Tonemap)
    {
        FSMAASettings Settings = FSMAASettings::GetRuntimeSettings();
        if (Settings.bEnabled && bIsPassEnabled)
        {
            UE_LOG(LogTemp, Warning, TEXT("SMAA: Adding callback to Tonemap pass"));

            InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(
                this,
                &SMAASceneViewExtension::PostProcessPassCallback_RenderThread
            ));
        }
    }
}

bool SMAASceneViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
    FSMAASettings Settings = FSMAASettings::GetRuntimeSettings();
    bool bActive = Settings.bEnabled;

    if (bActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("SMAA: IsActiveThisFrame = true"));
    }

    return bActive;
}

// Static instance creator
TSharedRef<SMAASceneViewExtension, ESPMode::ThreadSafe> SMAASceneViewExtension::GetInstance()
{
    static TSharedRef<SMAASceneViewExtension, ESPMode::ThreadSafe> Instance =
        FSceneViewExtensions::NewExtension<SMAASceneViewExtension>();
    return Instance;
}

