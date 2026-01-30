// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScreenPass.h"
#include "SMAASettings.h"


class FRDGBuilder;
class FViewInfo;

 /**
  * SMAA Rendering - Orchestrates the three SMAA passes
  */
namespace SMAARendering
{
    /**
     * Main entry point - adds all SMAA passes to the render graph
     *
     * @param GraphBuilder - RDG builder for adding passes
     * @param View - Current view being rendered
     * @param SceneColor - Input scene color texture (before SMAA)
     * @param Settings - SMAA configuration settings
     * @return Anti-aliased output texture
     */
    FScreenPassTexture AddSMAAPasses(
        FRDGBuilder& GraphBuilder,
        const FViewInfo& View,
        const FScreenPassTexture& SceneColor,
        const FScreenPassTexture& SceneDepth,
        const FSMAASettings& Settings
    );

    /**
     * Check if SMAA should render for this view
     */
    bool ShouldRenderSMAA(const FViewInfo& View);
}

