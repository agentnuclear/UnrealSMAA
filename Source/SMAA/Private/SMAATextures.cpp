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
        UE_LOG(LogTemp, Log, TEXT("SMAA: Area texture loaded successfully. Size: %dx%d, Format: %d"), AreaTexture->GetSizeX(), AreaTexture->GetSizeY(), (int32)AreaTexture->GetPixelFormat());
        
        // Ensure correct settings for SMAA (Linear, No Compression/High Quality)
        bool bChanged = false;
        
        // Critical: Prevent resizing to Power of Two
        if (AreaTexture->PowerOfTwoMode != ETexturePowerOfTwoSetting::None)
        {
            AreaTexture->PowerOfTwoMode = ETexturePowerOfTwoSetting::None;
            bChanged = true;
        }
        
        // Disable Mipmaps
        if (AreaTexture->MipGenSettings != TMGS_NoMipmaps)
        {
            AreaTexture->MipGenSettings = TMGS_NoMipmaps;
            bChanged = true;
        }

        if (AreaTexture->SRGB)
        {
            AreaTexture->SRGB = false;
            bChanged = true;
        }
        if (AreaTexture->CompressionSettings != TC_VectorDisplacementmap)
        {
             // Force uncompressed/linear settings
             AreaTexture->CompressionSettings = TC_VectorDisplacementmap;
             bChanged = true;
        }
        if (AreaTexture->Filter != TF_Bilinear)
        {
            AreaTexture->Filter = TF_Bilinear;
            bChanged = true;
        }

        if (bChanged)
        {
            AreaTexture->UpdateResource();
            UE_LOG(LogTemp, Warning, TEXT("SMAA: Forced AreaTexture settings to Linear/Bilinear/NoMips/NoPOT"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SMAA: Failed to load Area texture from /SMAA/Textures/T_SMAA_AreaTex"));
    }
    
    // Load Search Texture from plugin content
    SearchTexture = LoadObject<UTexture2D>(nullptr, TEXT("/SMAA/Textures/T_SMAA_SearchTex.T_SMAA_SearchTex"));
    if (SearchTexture)
    {
        UE_LOG(LogTemp, Log, TEXT("SMAA: Search texture loaded successfully. Size: %dx%d, Format: %d"), SearchTexture->GetSizeX(), SearchTexture->GetSizeY(), (int32)SearchTexture->GetPixelFormat());

        // Ensure correct settings for SMAA (Linear, No Compression/High Quality)
        bool bChanged = false;

        // Critical: Prevent resizing to Power of Two
        if (SearchTexture->PowerOfTwoMode != ETexturePowerOfTwoSetting::None)
        {
            SearchTexture->PowerOfTwoMode = ETexturePowerOfTwoSetting::None;
            bChanged = true;
        }
        
        // Disable Mipmaps
        if (SearchTexture->MipGenSettings != TMGS_NoMipmaps)
        {
            SearchTexture->MipGenSettings = TMGS_NoMipmaps;
             bChanged = true;
         }
         
         // Force UI Group (often allows NPOT)
         if (SearchTexture->LODGroup != TEXTUREGROUP_UI)
         {
             SearchTexture->LODGroup = TEXTUREGROUP_UI;
             bChanged = true;
         }

         if (SearchTexture->SRGB)
        {
            SearchTexture->SRGB = false;
            bChanged = true;
        }
        if (SearchTexture->CompressionSettings != TC_VectorDisplacementmap)
        {
             SearchTexture->CompressionSettings = TC_VectorDisplacementmap;
             bChanged = true;
        }
        if (SearchTexture->Filter != TF_Bilinear)
        {
            SearchTexture->Filter = TF_Bilinear;
            bChanged = true;
        }

        if (bChanged)
        {
            SearchTexture->UpdateResource();
            UE_LOG(LogTemp, Warning, TEXT("SMAA: Forced SearchTexture settings to Linear/Bilinear/NoMips/NoPOT"));
        }
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
