// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphUtils.h"

/**
 * SMAA Edge Detection Pixel shader
 * First Pass detects edges in the image
 */
class FSMAAEdgeDetectionPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSMAAEdgeDetectionPS);
	SHADER_USE_PARAMETER_STRUCT(FSMAAEdgeDetectionPS, FGlobalShader );

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			//Input Color Texture
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ColorTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)

			//Depth texture for depthbased edge detection
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DepthTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, DepthSampler)

			//Settings
		SHADER_PARAMETER(FVector2f, InvTextureSize)
		SHADER_PARAMETER(float, Threshold)
		SHADER_PARAMETER(uint32, EdgeDetectionMode)
        SHADER_PARAMETER(uint32, DebugMode)

	
			//Output
		RENDER_TARGET_BINDING_SLOTS()
	
	
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		// SM5 and above (DX11+, Vulkan, Metal)
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("SMAA_EDGE_DETECTION"), 1);
	}

};

/**
 * SMAA Blending Weight Calculation Pixel Shader
 * Second pass - calculates blend weights using area and search textures
 */
class FSMAABlendingWeightPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FSMAABlendingWeightPS);
    SHADER_USE_PARAMETER_STRUCT(FSMAABlendingWeightPS, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        // Edge texture from previous pass
        SHADER_PARAMETER_RDG_TEXTURE(Texture2D, EdgeTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, EdgeSampler)

        // Precomputed SMAA textures
        SHADER_PARAMETER_RDG_TEXTURE(Texture2D, AreaTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, AreaSampler)
        SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SearchTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, SearchSampler)

        // Settings
        SHADER_PARAMETER(FVector2f, InvTextureSize)
        SHADER_PARAMETER(int32, MaxSearchSteps)
        SHADER_PARAMETER(uint32, bUseCornerDetection)
        SHADER_PARAMETER(uint32, bUseDiagonalDetection)
        SHADER_PARAMETER(uint32, DebugMode)


        // Output
        RENDER_TARGET_BINDING_SLOTS()
    END_SHADER_PARAMETER_STRUCT()

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        OutEnvironment.SetDefine(TEXT("SMAA_BLENDING_WEIGHT"), 1);
    }
};

/**
 * SMAA Neighborhood Blending Pixel Shader
 * Third pass - applies the blend weights to produce final anti-aliased image
 */
class FSMAANeighborhoodBlendingPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FSMAANeighborhoodBlendingPS);
    SHADER_USE_PARAMETER_STRUCT(FSMAANeighborhoodBlendingPS, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        // Original color texture
        SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ColorTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)

        // Blend weight texture from previous pass
        SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BlendTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, BlendSampler)

        // Settings
        SHADER_PARAMETER(FVector2f, InvTextureSize)
        SHADER_PARAMETER(uint32, DebugMode)


        // Output
        RENDER_TARGET_BINDING_SLOTS()
    END_SHADER_PARAMETER_STRUCT()

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        OutEnvironment.SetDefine(TEXT("SMAA_NEIGHBORHOOD_BLENDING"), 1);
    }
};