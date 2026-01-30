// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAARendering.h"
#include "SMAAShaders.h"
#include "SystemTextures.h"
#include "PostProcess/PostProcessing.h"
#include "ScreenPass.h"
#include "PixelShaderUtils.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "SceneView.h"
#include "SMAATextures.h"

namespace SMAARendering
{
	//Forward decleration for indivisual passes
	static FRDGTextureRef AddEdgeDetectionPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef SceneColor, FRDGTextureRef SceneDepth, const FSMAASettings& Settings);
	static FRDGTextureRef AddBlendingWeightPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef EdgeTexture, const FSMAASettings& Settings);
	static FRDGTextureRef AddNeighborhoodBlendingPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef SceneColor, FRDGTextureRef BlendTexture, const FSMAASettings& Settings);

	bool ShouldRenderSMAA(const FViewInfo& View)
	{
        // Don't run in wireframe or debug views
        if (View.Family->EngineShowFlags.Wireframe)
        {
            return false;
        }

        // Don't run in editor viewports unless game view is enabled
        if (View.bIsSceneCapture || View.bIsReflectionCapture)
        {
            return false;
        }

        return true;
	}

    FScreenPassTexture AddSMAAPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FScreenPassTexture& SceneColor, const FScreenPassTexture& SceneDepth, const FSMAASettings& Settings) {
        check(SceneColor.IsValid());

        RDG_EVENT_SCOPE(GraphBuilder, "SMAA");

        //Pass 1: Edge Detection
        FRDGTextureRef DepthTex = SceneDepth.IsValid() ? SceneDepth.Texture : SceneColor.Texture;
        FRDGTextureRef EdgeTexture = AddEdgeDetectionPass(GraphBuilder, View, SceneColor.Texture, DepthTex, Settings);

        //Pass 2: Blending Weight Calculation
        FRDGTextureRef BlendWeightTexture = AddBlendingWeightPass(GraphBuilder,View,EdgeTexture,Settings);

        // Pass 3: Neighborhood Blending
        FRDGTextureRef OutputTexture = AddNeighborhoodBlendingPass(GraphBuilder, View, SceneColor.Texture, BlendWeightTexture,Settings);

        return FScreenPassTexture(OutputTexture, SceneColor.ViewRect);
    }

    FRDGTextureRef AddEdgeDetectionPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef SceneColor, FRDGTextureRef SceneDepth, const FSMAASettings& Settings)
    {
        //Create edge texture(RG8)
        const FRDGTextureDesc EdgeDesc = FRDGTextureDesc::Create2D(SceneColor->Desc.Extent, PF_R8G8B8A8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_RenderTargetable);
        
        FRDGTextureRef EdgeTexture = GraphBuilder.CreateTexture(EdgeDesc, TEXT("SMAA.Edges"));

        //Setup shader params
        auto* PassParameters = GraphBuilder.AllocParameters<FSMAAEdgeDetectionPS::FParameters>();
        PassParameters->ColorTexture = SceneColor;
        PassParameters->ColorSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp>::GetRHI();
        PassParameters->DepthTexture = SceneDepth; 
        PassParameters->DepthSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp>::GetRHI();
        PassParameters->InvTextureSize = FVector2f(1.0f / SceneColor->Desc.Extent.X, 1.0f / SceneColor->Desc.Extent.Y);
        PassParameters->Threshold = Settings.EdgeDetectionThreshold;
        PassParameters->EdgeDetectionMode = static_cast<uint32>(Settings.EdgeDetectionMode);
        PassParameters->DebugMode = static_cast<uint32>(Settings.DebugMode);
        PassParameters->RenderTargets[0] = FRenderTargetBinding(EdgeTexture, ERenderTargetLoadAction::EClear);

        // ---- Shaders ----
        //TShaderMapRef<FScreenPassVS> VertexShader(View.ShaderMap);
        TShaderMapRef<FSMAAEdgeDetectionPS> PixelShader(View.ShaderMap);

        const FScreenPassTextureViewport OutputViewport(EdgeTexture);
        const FScreenPassTextureViewport InputViewport(SceneColor);

        AddDrawScreenPass(
            GraphBuilder,
            RDG_EVENT_NAME("SMAA EdgeDetection"),
            View,
            OutputViewport,
            InputViewport,
            PixelShader,
            PassParameters
        );


        return EdgeTexture;
    }

    FRDGTextureRef AddBlendingWeightPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef EdgeTexture, const FSMAASettings& Settings)
    {
        // Create blend weight texture (RGBA16F - 4 directional weights)
        const FRDGTextureDesc BlendDesc = FRDGTextureDesc::Create2D(
            EdgeTexture->Desc.Extent,
            PF_FloatRGBA,
            FClearValueBinding::Black,
            TexCreate_ShaderResource | TexCreate_RenderTargetable
        );

        FRDGTextureRef BlendTexture = GraphBuilder.CreateTexture(BlendDesc, TEXT("SMAA.BlendWeights"));

        // Load SMAA precomputed textures
        UTexture2D* AreaTex = FSMAATextures::Get().GetAreaTexture();
        UTexture2D* SearchTex = FSMAATextures::Get().GetSearchTexture();

        // Register with RDG (fallback to white if not loaded)
        FRDGTextureRef AreaTextureRDG = AreaTex && AreaTex->GetResource() ?
            GraphBuilder.RegisterExternalTexture(CreateRenderTarget(AreaTex->GetResource()->TextureRHI, TEXT("SMAA.AreaTex"))) :
            GraphBuilder.RegisterExternalTexture(GSystemTextures.WhiteDummy);

        FRDGTextureRef SearchTextureRDG = SearchTex && SearchTex->GetResource() ?
            GraphBuilder.RegisterExternalTexture(CreateRenderTarget(SearchTex->GetResource()->TextureRHI, TEXT("SMAA.SearchTex"))) :
            GraphBuilder.RegisterExternalTexture(GSystemTextures.WhiteDummy);

        // Set up shader parameters
        auto* PassParameters = GraphBuilder.AllocParameters<FSMAABlendingWeightPS::FParameters>();
        PassParameters->EdgeTexture = EdgeTexture;
        PassParameters->EdgeSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp>::GetRHI();
        PassParameters->AreaTexture = AreaTextureRDG;
        PassParameters->AreaSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp>::GetRHI();
        PassParameters->SearchTexture = SearchTextureRDG;
        PassParameters->SearchSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp>::GetRHI();

        PassParameters->InvTextureSize = FVector2f(
            1.0f / EdgeTexture->Desc.Extent.X,
            1.0f / EdgeTexture->Desc.Extent.Y
        );
        PassParameters->MaxSearchSteps = Settings.MaxSearchSteps;
        PassParameters->bUseCornerDetection = Settings.bUseCornerDetection ? 1u : 0u;
        PassParameters->bUseDiagonalDetection = Settings.bUseDiagonalDetection ? 1u : 0u;
        PassParameters->DebugMode = static_cast<uint32>(Settings.DebugMode);
        PassParameters->RenderTargets[0] = FRenderTargetBinding(BlendTexture, ERenderTargetLoadAction::EClear);

        TShaderMapRef<FSMAABlendingWeightPS> PixelShader(View.ShaderMap);

        const FScreenPassTextureViewport OutputViewport(BlendTexture);
        const FScreenPassTextureViewport InputViewport(EdgeTexture);

        AddDrawScreenPass(
            GraphBuilder,
            RDG_EVENT_NAME("SMAA BlendWeight"),
            View,
            OutputViewport,
            InputViewport,
            PixelShader,
            PassParameters
        );

        return BlendTexture;
    }

    FRDGTextureRef AddNeighborhoodBlendingPass(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef SceneColor, FRDGTextureRef BlendTexture, const FSMAASettings& Settings)
    {
        // Create output texture (same format as input)
        const FRDGTextureDesc OutputDesc = FRDGTextureDesc::Create2D(
            SceneColor->Desc.Extent,
            SceneColor->Desc.Format,
            FClearValueBinding::Black,
            TexCreate_ShaderResource | TexCreate_RenderTargetable
        );


        FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputDesc, TEXT("SMAA.Output"));

        // Set up shader parameters
        auto* PassParameters = GraphBuilder.AllocParameters<FSMAANeighborhoodBlendingPS::FParameters>();
        PassParameters->ColorTexture = SceneColor;
        PassParameters->ColorSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp>::GetRHI();
        PassParameters->BlendTexture = BlendTexture;
        PassParameters->BlendSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp>::GetRHI();
        PassParameters->InvTextureSize = FVector2f(
            1.0f / SceneColor->Desc.Extent.X,
            1.0f / SceneColor->Desc.Extent.Y
        );
        PassParameters->DebugMode = static_cast<uint32>(Settings.DebugMode);
        PassParameters->RenderTargets[0] = FRenderTargetBinding(OutputTexture, ERenderTargetLoadAction::EClear);

        TShaderMapRef<FSMAANeighborhoodBlendingPS> PixelShader(View.ShaderMap);
        const FScreenPassTextureViewport OutputViewport(OutputTexture);
        const FScreenPassTextureViewport InputViewport(SceneColor);

        AddDrawScreenPass(
            GraphBuilder,
            RDG_EVENT_NAME("SMAA Neighborhood"),
            View,
            OutputViewport,
            InputViewport,
            PixelShader,
            PassParameters
        );
        return OutputTexture;
    }

}

