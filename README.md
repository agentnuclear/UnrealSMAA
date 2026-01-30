# SMAA Anti-Aliasing Plugin for Unreal Engine

A professional-grade SMAA (Subpixel Morphological Anti-Aliasing) implementation for Unreal Engine 5, providing high-quality edge anti-aliasing with minimal performance impact.

## Installation

1. Copy the `SMAA` plugin folder to your project's `Plugins` directory
2. Regenerate Visual Studio project files
3. Rebuild the project
4. Enable the plugin in `Edit > Plugins` (search for "SMAA")

[This Plugin is still in development and is not build friendly at the moment]

## Usage

### Basic Setup
The plugin automatically registers with the rendering pipeline on engine initialization. No additional setup is required beyond installation.

### Configuration via Console Variables

All SMAA settings can be controlled via console commands:

```
// Enable/disable SMAA
r.SMAA.Enabled 1|0

// Quality presets: 0=Low, 1=Medium, 2=High, 3=Ultra
r.SMAA.QualityPreset 2

// Edge detection mode: 0=Luma, 1=Color, 2=Depth
r.SMAA.EdgeDetectionMode 0

// Edge detection sensitivity (0.05-0.20)
r.SMAA.Threshold 0.1

// Diagonal edge detection
r.SMAA.UseDiagonalDetection 1

// Corner detection
r.SMAA.UseCornerDetection 1

// Max search steps
r.SMAA.MaxSearchSteps 32

// Debug visualization: 0=Off, 1=Edges, 2=BlendWeights, 3=FinalBlend
r.SMAA.DebugMode 0

```

## Architecture

### Module Structure
- **FSMAAModule**: Plugin module manager handling initialization and shader path mapping
- **FSMAASceneViewExtension**: Injects SMAA post-processing into Unreal's rendering pipeline
- **SMAARendering**: Orchestrates the three rendering passes
- **SMAAShaders**: GPU shader classes for edge detection and weight calculation
- **FSMAASettings**: Runtime configuration storage and console variable management

### Three-Pass Pipeline

#### Pass 1: Edge Detection
- Input: Scene color (and optional depth)
- Output: Edge texture
- Supports three detection modes:
  - **Luma**: Fast, luminance-based detection
  - **Color**: RGB color difference detection
  - **Depth**: Geometry-based edge detection

#### Pass 2: Blending Weight Calculation
- Input: Edge texture
- Output: Blend weight texture
- Uses precomputed area and search textures
- Configurable maximum search steps
- Optional diagonal and corner detection

#### Pass 3: Neighborhood Blending
- Input: Original scene color + blend weight texture
- Output: Anti-aliased final image
- Applies calculated blend weights to smooth edges

## Performance Considerations

### Quality Tiers
- **Low**: ~0.5ms (16 search steps)
- **Medium**: ~1.0ms (32 search steps)
- **High**: ~1.2ms (32 search steps + corner detection)
- **Ultra**: ~1.5ms (32 search steps + prediction)

*Performance varies by resolution and hardware*

### Optimization Tips
1. Use **Luma** edge detection for best performance
2. Start with **High** quality preset and adjust based on requirements
3. Disable **DiagonalDetection** if performance is critical
4. Use **Color** mode only for scenes with distinct color edges
5. **Depth** mode is useful but more expensive

## Debug Features

### Visualization Modes
- **None (0)**: Normal output
- **Edges (1)**: Shows detected edges in red
- **BlendWeights (2)**: Visualizes calculated blend weights
- **SearchTexture (3)**: Displays the precomputed search texture
- **AreaTexture (4)**: Displays the precomputed area texture
- **SearchSteps (5)**: Visualizes search step patterns
- **TextureDimensions (6)**: Shows texture dimension information

Enable with: `r.SMAA.DebugMode 1`

## Platform Support
- **Windows**: DirectX 11, DirectX 12
- **Mac**: Metal
- **Linux**: Vulkan
- **Console**: Based on shader model support

Requires **SM5 or higher** (DirectX 11+ equivalent)

The plugin uses RDG (Render Dependency Graph) for efficient multi-pass rendering, ensuring compatibility with modern rendering backends.

### Public Dependencies
- **Core**: Base engine functionality
- **CoreUObject**: Object system
- **Engine**: Core engine systems

### Private Dependencies
- **RenderCore**: Shader compilation and RHI types
- **Renderer**: FSceneViewExtension, RDG (Render Dependency Graph), PostProcess integration
- **RHI**: RHI command lists and resources
- **Projects**: Plugin manager for shader path mapping
- Core, CoreUObject, Engine (public)
- RenderCore, Renderer, RHI, Projects (private)

## Troubleshooting

### Shader Compilation Errors
- Ensure shader directory is properly mapped in module startup
- Check that plugin shaders are in `Plugins/SMAA/Shaders/`
- Verify shader compilation environment defines

### SMAA Not Appearing
- Check that plugin is enabled: `r.SMAA.Enabled 1`
- Verify post-processing is enabled in project settings
- Check console for initialization warnings

### Performance Issues
  - Supports three detection modes via shader permutations
  - Optimized for both single-pass and multi-pass rendering
  - Includes optional debug visualization
  
- **FSMAABlendingWeightPS**: Global pixel shader for weight calculation
  - Uses precomputed area and search textures for efficient pattern matching
  - Configurable maximum search steps and corner detection
  - Supports diagonal edge detection

- **Neighborhood Blending**: Built-in screen pass rendering using calculated blend weights

### Integration Points
- **FSceneViewExtension**: Injects SMAA into post-processing pipeline via `SubscribeToPostProcessingPass()`
- **RDG (Render Dependency Graph)**: Modern rendering graph integration for multi-pass rendering
- **Console Variables**: Real-time configuration without recompilation
- **Precomputed Textures**: Area and search textures stored in plugin content for efficient weight lookups

### Module Loading and Initialization
- **Loading Phase**: `PostConfigInit` - Ensures shader system is ready
- **Startup Flow**: 
  1. `StartupModule()`: Maps shader directory to virtual path `/Plugin/SMAA`
  2. `OnPostEngineInit()`: Creates scene view extension and loads precomputed textures on game thread
  3. `ShutdownModule()`: Cleanup and deregistration

### Resource Management
- **Precomputed Textures**: Area and search textures loaded from plugin content (T_SMAA_AreaTex, T_SMAA_SearchTex)
- **Dynamic Textures**: Edge detection and blend weight textures created dynamically during rendering
- **Thread Safety**: Scene view extension uses thread-safe shared pointers (ESPMode::ThreadSafe)

## Advanced Settings

### Fine-Tuning Edge Detection
- **Threshold**: Controls edge sensitivity
  - Lower values (0.05): Detect more edges, more blurring
  - Higher values (0.15): Detect fewer edges, sharper but more aliasing
  
- **LocalContrastAdaptationFactor**: Adapts threshold to local image contrast
  - Prevents over-blurring in high-contrast areas
  - Typical range: 1.0 - 3.0

### Maximum Search Steps
- **Low (16)**: Faster, adequate for most edges
- **Medium (32)**: Better pattern detection, recommended default
- **High (32 + corners)**: Preserves corner sharpness
- **Ultra (32 + corners + prediction)**: Maximum quality for distinctive edges

## Performance Profiling

The plugin integrates with Unreal's GPU profiling system. Use the following commands:

```
// Enable GPU profiling
stat unit
stat unitgraph

// Profile individual passes in the RDG graph
r.RDG.Debug 1
```

## Version and Status

- **Version**: 1.0
- **Plugin Status**: Beta version
- **Loading Phase**: PostConfigInit (ensures compatibility with rendering system)
- **Content Support**: Plugin includes content assets (precomputed SMAA textures)

## License
Copyright Epic Games, Inc. All Rights Reserved.

## Credits
SMAA algorithm by Jorge Jiménez, Brawley, Reshetov, and Sousa (2011)
Unreal Engine integration and optimization by GroundZero
- **RDG (Render Dependency Graph)**: Modern rendering graph integration
- **Console Variables**: Real-time configuration without recompilation

## License
Copyright Epic Games, Inc. All Rights Reserved.

## Credits
SMAA algorithm by Jorge Jiménez, Brawley, Reshetov, and Sousa (2011)
Unreal Engine integration and optimization by [Developer Name]
