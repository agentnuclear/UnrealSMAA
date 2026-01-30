# SMAA Anti-Aliasing Plugin for Unreal Engine

A professional-grade SMAA (Subpixel Morphological Anti-Aliasing) implementation for Unreal Engine 5, providing high-quality edge anti-aliasing with minimal performance impact.

## Installation

1. Copy the `SMAA` plugin folder to your project's `Plugins` directory
2. Regenerate Visual Studio project files
3. Rebuild the project
4. Enable the plugin in `Edit > Plugins` (search for "SMAA")

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

// Local contrast adaptation
r.SMAA.LocalContrastAdaptationFactor 2.0
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
- **FinalBlend (3)**: Shows final blended output

Enable with: `r.SMAA.DebugMode 1`

## Platform Support
- **Windows**: DirectX 11, DirectX 12
- **Mac**: Metal
- **Linux**: Vulkan
- **Console**: Based on shader model support

Requires **SM5 or higher** (DirectX 11+ equivalent)

## Dependencies
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
- Reduce quality preset
- Switch to Luma edge detection mode
- Disable diagonal detection
- Lower max search steps

## Technical Details

### Shader Implementation
- **FSMAAEdgeDetectionPS**: Global pixel shader for edge detection
- **FSMAABlendingWeightPS**: Global pixel shader for weight calculation
- **Neighborhood Blending**: Uses built-in screen pass rendering

### Integration Points
- **FSceneViewExtension**: Post-rendering pipeline injection
- **RDG (Render Dependency Graph)**: Modern rendering graph integration
- **Console Variables**: Real-time configuration without recompilation

## License
Copyright Epic Games, Inc. All Rights Reserved.

## Credits
SMAA algorithm by Jorge Jiménez, Brawley, Reshetov, and Sousa (2011)
Unreal Engine integration and optimization by [Developer Name]
