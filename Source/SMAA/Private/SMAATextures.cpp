// SMAATextures.cpp
#include "SMAATextures.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

FSMAATextures& FSMAATextures::Get()
{
    static FSMAATextures Instance;
    return Instance;
}

FSMAATextures::FSMAATextures()
    : AreaTexture(nullptr)
    , SearchTexture(nullptr)
    , bTexturesLoaded(false)
{
}

void FSMAATextures::LoadTextures()
{
    if (bTexturesLoaded)
    {
        UE_LOG(LogTemp, Log, TEXT("SMAA: Textures already loaded, skipping"));
        return;
    }
    
    // Load Area Texture from plugin content
    AreaTexture = LoadObject<UTexture2D>(nullptr, TEXT("/SMAA/Textures/T_SMAA_AreaTex.T_SMAA_AreaTex"));
    if (AreaTexture)
    {
        UE_LOG(LogTemp, Log, TEXT("SMAA: Area texture loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SMAA: Failed to load Area texture from /SMAA/Textures/T_SMAA_AreaTex"));
    }
    
    // Load Search Texture from plugin content
    SearchTexture = LoadObject<UTexture2D>(nullptr, TEXT("/SMAA/Textures/T_SMAA_SearchTex.T_SMAA_SearchTex"));
    if (SearchTexture)
    {
        UE_LOG(LogTemp, Log, TEXT("SMAA: Search texture loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SMAA: Failed to load Search texture from /SMAA/Textures/T_SMAA_SearchTex"));
    }
    
    bTexturesLoaded = (AreaTexture != nullptr && SearchTexture != nullptr);
}

UTexture2D* FSMAATextures::GetAreaTexture()
{
    
    return AreaTexture;
}

UTexture2D* FSMAATextures::GetSearchTexture()
{
    
    return SearchTexture;
}

bool FSMAATextures::AreTexturesLoaded() const
{
    return bTexturesLoaded && AreaTexture != nullptr && SearchTexture != nullptr;
}
