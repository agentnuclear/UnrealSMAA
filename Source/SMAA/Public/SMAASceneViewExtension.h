// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

/**
 * Scene View Extension - Injects SMAA into the rendering pipeline
 * This is the bridge between our plugin and Unreal's renderer
 */
class SMAA_API SMAASceneViewExtension : public FSceneViewExtensionBase
{
public:
	SMAASceneViewExtension(const FAutoRegister& InAutoRegister);

	virtual ~SMAASceneViewExtension() = default;

	//FSceneViewExtentionBase interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}

	virtual void PostRenderViewFamily_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		FSceneViewFamily& InViewFamily
	) override;

	virtual void SubscribeToPostProcessingPass(
		EPostProcessingPass PassId,
		FAfterPassCallbackDelegateArray& InOutPassCallbacks,
		bool bIsPassEnabled
	) override;

	//Determines if this extension should be active for the current frame
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext & Context) const override;

	// Static instance that auto-registers
	static TSharedRef<SMAASceneViewExtension, ESPMode::ThreadSafe> GetInstance();

private:
	FScreenPassTexture PostProcessPassCallback_RenderThread(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		const FPostProcessMaterialInputs& Inputs
	);

};
