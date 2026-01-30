// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * SMAA Quality Presets
 * These control search distances and prediction settings
 */

enum class ESMAAQualityPreset : uint8
{
	Low = 0,	//16 max serach steps
	Medium = 1,	//32 max search steps
	High = 2,	//32 max search steps + corner detection
	Ultra = 3	//32 max search steps + predication
};

/**
 * SMAA Edge Detection Method
 */

enum class ESMAAEdgeDetectionMode : uint8
{
	Luma = 0,	//Luminance based (fastest)
	Color = 1,	//RGB color difference(better for colored edges)
	Depth = 2,	//Depth based(good for geometry edges)
};

/**
 * Debug Methods
 */
enum class ESMAADebugMode : uint32
{
    None = 0,
    Edges = 1,
    BlendWeights = 2,
    SearchTexture = 3,
    AreaTexture = 4,
    SearchSteps = 5,
    TextureDimensions = 6
};

/**
 * Runtime SMAA configuration
 * These settings can be modified via console variables
 */
struct FSMAASettings
{
	//Enable/Disable
	bool bEnabled = false;

	//Quality preset
	ESMAAQualityPreset QualityPreset = ESMAAQualityPreset::High;

	//Edge Detection method
	ESMAAEdgeDetectionMode EdgeDetectionMode = ESMAAEdgeDetectionMode::Luma;

    ESMAADebugMode DebugMode = ESMAADebugMode::None;

    // Edge detection threshold (0.05 - 0.15 typical range)
    // Lower = more edges detected = more blurring
    // Higher = fewer edges = sharper but more aliasing
    float EdgeDetectionThreshold = 0.1f;

    // Local contrast adaptation factor (0.0 - 1.0)
    // Higher values make threshold adapt to local contrast
    float LocalContrastAdaptationFactor = 2.0f;

    // Maximum search steps for pattern detection
    // Determines how far the algorithm searches along edges
    int32 MaxSearchSteps = 32;

    // Whether to use diagonal edge detection
    // More expensive but handles diagonal edges better
    bool bUseDiagonalDetection = true;

    // Whether to use corner detection
    // Slightly more expensive but prevents corner rounding
    bool bUseCornerDetection = true;

    // Get settings from console variables
    static FSMAASettings GetRuntimeSettings();
};



class SMAA_API SMAASettings
{
public:
	SMAASettings();
	~SMAASettings();
};
