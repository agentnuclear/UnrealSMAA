// SMAATextures.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

/**
 * Manager for SMAA precomputed textures
 * Loads and caches the Area and Search textures
 */
class FSMAATextures
{
public:
    static FSMAATextures& Get();
    void LoadTextures();
    
    // Get the textures (will load on first access)
    UTexture2D* GetAreaTexture();
    UTexture2D* GetSearchTexture();
    
    // Check if textures are loaded
    bool AreTexturesLoaded() const;

private:
    FSMAATextures();
    ~FSMAATextures() = default;
    
    // Prevent copying
    FSMAATextures(const FSMAATextures&) = delete;
    FSMAATextures& operator=(const FSMAATextures&) = delete;
    
    
    
    UTexture2D* AreaTexture;
    UTexture2D* SearchTexture;
    bool bTexturesLoaded;
};
