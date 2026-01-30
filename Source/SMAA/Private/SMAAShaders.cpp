// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAAShaders.h"
#include "ShaderParameterUtils.h"
#include "RenderGraphUtils.h"

//Implementing the shader classes


IMPLEMENT_GLOBAL_SHADER(FSMAAEdgeDetectionPS,
    "/Plugin/SMAA/Private/SMAAEdgeDetection.usf",  // Virtual shader path
    "MainPS",                                       // Entry point function name
    SF_Pixel);                                      // Shader frequency (pixel shader)

IMPLEMENT_GLOBAL_SHADER(FSMAABlendingWeightPS,
    "/Plugin/SMAA/Private/SMAABlendingWeight.usf",
    "MainPS",
    SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(FSMAANeighborhoodBlendingPS,
    "/Plugin/SMAA/Private/SMAANeighborhoodBlending.usf",
    "MainPS",
    SF_Pixel);

