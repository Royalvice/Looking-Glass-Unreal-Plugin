#include "Render/LookingGlassRendering.h"
#include "Game/LookingGlassSceneCaptureComponent2D.h"

#include "ILookingGlassRuntime.h"
#include "Misc/LookingGlassLog.h"

#include "GlobalShader.h"
#include "PipelineStateCache.h"
#include "RHIStaticStates.h"
#include "TextureResource.h"
#include "ScreenRendering.h"
#include "CommonRenderResources.h"

#include "Runtime/Launch/Resources/Version.h"

void LookingGlass::CopyToQuiltShader_RenderThread(FRHICommandListImmediate& RHICmdList, const FCopyToQuiltRenderContext& Context)
{
    check(IsInRenderingThread());

    SCOPED_DRAW_EVENTF(RHICmdList, Scene, TEXT("CopyToQuiltShader_RenderThread ViewIndex %d_%d_%d"), Context.CurrentViewIndex, Context.ViewInfoIndex, Context.TotalViews);
    DISPLAY_HOLOPLAY_FUNC_TRACE(LookingGlassLogRender)

        const FLookingGlassTilingQuality& TilingValues = Context.TilingValues;

    // Set Render targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FRHIRenderPassInfo RPInfo(Context.QuiltTargetResource->GetRenderTargetTexture(), ERenderTargetActions::Load_Store);
    RHICmdList.BeginRenderPass(RPInfo, TEXT("CopyToQuiltShader_RenderThread"));

    // Set Viewport for tiling ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Legacy order: first view at bottom-left, last view at top-right
    // Optional order (bTopLeftFirstOrder): first view at top-left, last view at bottom-right
    int32 X = 0;
    int32 Y = 0;
    if (Context.bTopLeftFirstOrder)
    {
        // Row-major from top to bottom
        int32 Row = Context.CurrentViewIndex / TilingValues.TilesX; // 0 is top row
        int32 Col = Context.CurrentViewIndex % TilingValues.TilesX;
        X = Col * TilingValues.TileSizeX;
        Y = Row * TilingValues.TileSizeY;
    }
    else
    {
        // Original behavior: bottom-left to top-right
        int32 RI = TilingValues.GetNumTiles() - Context.CurrentViewIndex - 1;
        X = (Context.CurrentViewIndex % TilingValues.TilesX) * TilingValues.TileSizeX;
        Y = (RI / TilingValues.TilesX) * TilingValues.TileSizeY;
    }

    // The padding is necessary because the shader takes y from the opposite spot as this does
    int32 PaddingY = TilingValues.QuiltH - TilingValues.TilesY * TilingValues.TileSizeY;
    FVector2D Min(X, Y + PaddingY);
    FVector2D Max(Min.X + TilingValues.TileSizeX, Min.Y + TilingValues.TileSizeY);

    UE_LOG(LookingGlassLogRender, Verbose, TEXT("CurrentView %d, Min %s Max %s"), Context.CurrentViewIndex, *Min.ToString(), *Max.ToString());

    FGraphicsPipelineStateInitializer GraphicsPSOInit;
    RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

    // Get shaders. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const auto FeatureLevel = GMaxRHIFeatureLevel;
    auto ShaderMap = GetGlobalShaderMap(FeatureLevel);
    TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
    TShaderMapRef<FScreenPS> PixelShader(ShaderMap);

    // Set the graphic pipeline state. START ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
    GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
    GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
    GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
    GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
    GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
    GraphicsPSOInit.PrimitiveType = PT_TriangleList;

#if ENGINE_MAJOR_VERSION >= 5
    SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
#else
    SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
#endif
    // Set the graphic pipeline state. END --------------------------------------

    // Update shader parameters and resources parameters. START ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();
    PixelShader->SetParameters(BatchedParameters, TStaticSamplerState<SF_Bilinear>::GetRHI(), Context.TilingTextureResource->TextureRHI);
    RHICmdList.SetBatchedShaderParameters(RHICmdList.GetBoundPixelShader(), BatchedParameters);

    // Calculate view rect
    float U = 0.f, V = 0.f, SizeU = 1.f, SizeV = 1.f;
    FLookingGlassRenderingConfig::CalculateViewRect(U, V, SizeU, SizeV, Context.ViewRows, Context.ViewColumns, Context.TotalViews, Context.ViewInfoIndex);


    // Update shader parameters and resources parameters. END --------------------------------------

    // Issue Indexed draw call ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const FName RendererModuleName("Renderer");
    IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
    RendererModule->DrawRectangle(
        RHICmdList,
        Min.X, Min.Y,
        Max.X - Min.X, Max.Y - Min.Y,
        U, V,
        SizeU, SizeV,
        FIntPoint(Context.QuiltTargetResource->GetRenderTargetTexture()->GetSizeX(), Context.QuiltTargetResource->GetRenderTargetTexture()->GetSizeY()),
        FIntPoint(1, 1),
        VertexShader,
        EDRF_Default);

    RHICmdList.EndRenderPass();
}

