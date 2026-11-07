# Lichii (Ryzen 7 7800X3D + RTX 4070 SUPER Fork)

## Test Platform
- **CPU**: AMD Ryzen 7 7800X3D (8C/16T Zen 4, 1 CCD, 96 MiB 3D V-Cache)
- **GPU**: NVIDIA GeForce RTX 4070 SUPER (12 GiB GDDR6X, Ada Lovelace)
- **RAM**: 32 GiB DDR5
- **OS**: Windows 11 25H2
- **Title**: *The Legend of Zelda: Ocarina of Time* (Switch 2 Remake, Title ID `0400486024DEC000`, v1.1.1 NUP)
- **Rendering**: Vulkan, 2560x1440 native + DLAA (Preset K, DLSS 310.9.16.0)

---

## Tested Hardware Performance Telemetry

Empirical benchmark data extracted from continuous telemetry sessions (7,800 frames):

| Scene / Area | Framerate (FPS) | Frame Time (ms) | Telemetry Observations |
| :--- | :--- | :--- | :--- |
| **Kakariko Village / Clear Overworld** | **57.1 – 59.4 FPS** | 16.8 – 17.5 ms | Smooth, stable performance near the 60 FPS target. |
| **Hyrule Field (open views / dense foliage)** | **46.8 – 54.3 FPS** | 18.4 – 21.3 ms | Frame drops caused by frequent CPU-GPU sync stalls (`OnCPURead`). |
| **Area Transitions / Initial Load** | **39.9 FPS** (momentary) | ~25.0 ms | Transitory spike from background pipeline compilation. |
| **BufferCache Usage** | **846 – 848 MiB** | — | Stable within the 1 GiB limit of this fork (zero GC thrashing). |
| **VRAM Footprint** | **~4.2 GiB / 12 GiB** | — | Ample VRAM headroom on the RTX 4070 SUPER. |

---

## Architectural Optimizations

### 1. Zen 4 Compilation and PGO Profile
- **Clang 22.1.8 with ThinLTO**: Cross-translation-unit link-time optimization tailored across all 16 Zen 4 threads.
- **`-march=znver4 -mtune=znver4`**: Enables AVX-512, BMI2, and Zen 4-native branch prediction.
- **OoT PGO (`lichii-windows.profdata`)**: Reconciled 12,209 indirect call targets and 169 LLVM CFG hashes profile-guided on real Ocarina of Time gameplay loops.

### 2. Thread Affinity and 3D V-Cache Locality
- Targeting a single 8-core CCD with 96 MiB 3D V-Cache, thread affinity prevents cache thrashing and core-hopping:
  - **Cores 0-3**: CPU emulation (Dynarmic JIT), HLE services, and audio processing.
  - **Cores 4-7**: Vulkan render submission thread, fencing thread, and background worker pool.

### 3. 1 GiB Expanded BufferCache
- The upstream default (~480 MiB) caused recurring garbage collection stalls in Hyrule Field and Castle Town (actual memory consumption ranges between 678 and 848 MiB).
- This fork configures:
  - Soft limit: **960 MiB**
  - Hard limit: **1152 MiB**
- Keeps active scene data resident in memory without periodic flushes.

### 4. DLSS 310.9.16.0 (DLAA) Integration
- Native `2560x1440` rasterization (DLAA mode, 1.0 scale ratio) using DLSS Transformer Preset K.
- Secondary anti-aliasing disabled (`anti_aliasing=0`) to eliminate double-pass blur.
- 16x Anisotropic Filtering (`max_anisotropy=5`).
- Forced 4K upscale disabled (`EDEN_NX2_OCARINA_4K=0`).

---

## Runtime Switches (`nx2_switches.txt`)

| Switch | Recommended Value | Purpose |
| :--- | :--- | :--- |
| `EDEN_NX2_SPLIT` | `2` | Split GPU front/back macro execution |
| `EDEN_NX2_FENCE_BATCH` | `1` | GPU fence batching |
| `EDEN_NX2_FENCE_FLUSH_US` | `250` | Fence flush timer at 250 µs |
| `EDEN_NX2_ASYNC_READ_PREFETCH` | `1` | Asynchronous memory trap read prefetch |
| `EDEN_NX2_OCARINA_4K` | `0` | Native 1440p DLAA (prevents 4K upscale stutter) |
| `EDEN_NX2_DLSS_PRESET` | `K` | DLSS Transformer Preset K |
| `EDEN_NX2_PRELOAD_ALL_PIPELINES` | `0` | Protects 12 GiB VRAM from startup saturation |
| `EDEN_NX2_BUFFER_DOWNLOADS` | `1` | Asynchronous GPU-to-CPU buffer downloads on fences |
| `EDEN_NX2_KEEP_ASTC` | `1` | Disables redundant GPU ASTC-to-BC3 recompression |
