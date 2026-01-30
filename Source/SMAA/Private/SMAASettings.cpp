// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAASettings.h"
#include "HAL/IConsoleManager.h"

// Console vars decleration
static TAutoConsoleVariable<int32> CVarSMAAEnable(
	TEXT("r.SMAA.Enable"),
	0,
	TEXT("Enable SMAA post-processing anti aliasing\n")
	TEXT("0: Diabled (default)\n")
	TEXT("1: Enabled\n"),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarSMAAQuality(
    TEXT("r.SMAA.Quality"),
    2,
    TEXT("SMAA quality preset\n")
    TEXT("0: Low    - 16 search steps, basic\n")
    TEXT("1: Medium - 32 search steps\n")
    TEXT("2: High   - 32 search steps + corner detection (default)\n")
    TEXT("3: Ultra  - 32 search steps + corner + diagonal detection\n"),
    ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarSMAAEdgeDetectionMode(
    TEXT("r.SMAA.EdgeDetectionMode"),
    0,
    TEXT("SMAA edge detection method\n")
    TEXT("0: Luma  - Luminance based (default, fastest)\n")
    TEXT("1: Color - RGB color difference\n")
    TEXT("2: Depth - Depth buffer based\n"),
    ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarSMAAThreshold(
    TEXT("r.SMAA.Threshold"),
    0.1f,
    TEXT("Edge detection threshold (0.05 - 0.15)\n")
    TEXT("Lower = more edges detected = softer image\n")
    TEXT("Higher = fewer edges = sharper but more aliasing\n")
    TEXT("Default: 0.1\n"),
    ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarSMAAMaxSearchSteps(
    TEXT("r.SMAA.MaxSearchSteps"),
    32,
    TEXT("Maximum search steps for edge pattern detection\n")
    TEXT("Higher = better quality but slower\n")
    TEXT("Default: 32\n"),
    ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarSMAACornerDetection(
    TEXT("r.SMAA.CornerDetection"),
    1,
    TEXT("Enable corner detection\n")
    TEXT("0: Disabled\n")
    TEXT("1: Enabled (default)\n"),
    ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarSMAADiagonalDetection(
    TEXT("r.SMAA.DiagonalDetection"),
    1,
    TEXT("Enable diagonal edge detection\n")
    TEXT("0: Disabled\n")
    TEXT("1: Enabled (default)\n"),
    ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarSMAADebugMode(
    TEXT("r.SMAA.DebugMode"),
    0,
    TEXT("SMAA debug visualization mode\n")
    TEXT("0: Disabled (default)\n")
    TEXT("1: Show edges\n")
    TEXT("2: Show blend weights\n")
    TEXT("3: Show final blend overlay\n"),
    ECVF_RenderThreadSafe
);

SMAASettings::SMAASettings()
{
}

SMAASettings::~SMAASettings()
{
}

FSMAASettings FSMAASettings::GetRuntimeSettings()
{
    FSMAASettings Settings;

    Settings.bEnabled = CVarSMAAEnable.GetValueOnAnyThread() != 0;
    Settings.QualityPreset = static_cast<ESMAAQualityPreset>(FMath::Clamp(CVarSMAAEdgeDetectionMode.GetValueOnAnyThread(), 0, 3));
    Settings.EdgeDetectionMode = static_cast<ESMAAEdgeDetectionMode>(FMath::Clamp(CVarSMAAEdgeDetectionMode.GetValueOnAnyThread(), 0, 2));
    Settings.EdgeDetectionThreshold = FMath::Clamp(CVarSMAAThreshold.GetValueOnAnyThread(), 0.01f, 0.5f);
    Settings.MaxSearchSteps = FMath::Clamp(CVarSMAAMaxSearchSteps.GetValueOnAnyThread(), 0, 112);
    Settings.bUseCornerDetection = CVarSMAACornerDetection.GetValueOnAnyThread() != 0;
    Settings.bUseDiagonalDetection = CVarSMAADiagonalDetection.GetValueOnAnyThread() != 0;
    Settings.DebugMode = static_cast<ESMAADebugMode>(FMath::Clamp(CVarSMAADebugMode.GetValueOnAnyThread(), 0, 3));

    
    return Settings;
}
