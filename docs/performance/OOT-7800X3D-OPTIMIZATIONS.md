# Lichii OoT 7800X3D + RTX 4070 SUPER Optimization Guide

## Target Platform
- **CPU**: AMD Ryzen 7 7800X3D (8C/16T Zen 4, 1 CCD, 96 MiB 3D V-Cache)
- **GPU**: NVIDIA GeForce RTX 4070 SUPER (12 GiB GDDR6X, Ada Lovelace)
- **RAM**: 32 GiB DDR5
- **OS**: Windows 11 25H2
- **Game**: *The Legend of Zelda: Ocarina of Time* (Switch 2 Remake, Title ID `0400486024DEC000`, v1.1.1 NUP)

---

## Architecture Optimizations

### 1. Compiler Toolchain & Microarchitecture Tuning
- **Clang 22.1.8 with ThinLTO**: Multi-threaded link-time optimization linking across all 16 Zen 4 threads.
- **`-march=znver4 -mtune=znver4`**: Enables hardware AVX-512, BMI2, and Zen 4 branch prediction pipelines.
- **OoT PGO (`lichii-windows.profdata`)**: Reconciled 12,209 indirect call targets and 169 LLVM CFG hashes specifically for Ocarina of Time's hottest code paths (Maxwell 3D decode, BufferCache lookups, and Dynarmic JIT).

### 2. Thread Pinning & V-Cache Locality
- Single CCD design requires avoiding inter-thread cache thrashing:
  - **Cores 0-3**: Core emulation (Dynarmic guest CPU, HLE services, Audio).
  - **Cores 4-7**: Vulkan rendering thread, GPU fencing thread, BackgroundWorker pool.

### 3. BufferCache 1 GiB Architecture
- Standard builds set buffer GC thresholds to ~480 MiB.
- In Ocarina of Time, Hyrule Field and Castle Town exceed 650 MiB, causing 10+ stutters over 100 ms in unoptimized builds.
- Our build raises:
  - Soft threshold: **960 MiB**
  - Hard threshold: **1152 MiB**
- Retains working sets in memory without triggers.

### 4. DLSS 310.9.16.0 Streamline Integration
- Evaluates native `2560x1440` (DLAA mode, ratio 1.0) using Next-Gen Transformer Preset K.
- Disables secondary AA (`anti_aliasing=0`) to avoid blur.
- Sets 16x Anisotropic Filtering (`max_anisotropy=5`).
- Clamps upscale distortion with `EDEN_NX2_OCARINA_4K=0`.

---

## Runtime Switches (`nx2_switches.txt`)

| Switch | Recommended Value | Purpose |
| :--- | :--- | :--- |
| `EDEN_NX2_SPLIT` | `2` | Split GPU front/back macro execution |
| `EDEN_NX2_FENCE_BATCH` | `1` | Batch GPU syncpoint fences |
| `EDEN_NX2_FENCE_FLUSH_US` | `250` | 250 µs fence flush timeout |
| `EDEN_NX2_ASYNC_READ_PREFETCH` | `1` | Asynchronous memory trap prefetch |
| `EDEN_NX2_OCARINA_4K` | `0` | Native 1440p DLAA (prevents 4K upscale stutter) |
| `EDEN_NX2_DLSS_PRESET` | `K` | Transformer DLSS model preset |
| `EDEN_NX2_PRELOAD_ALL_PIPELINES` | `0` | Protects 12 GiB VRAM from synchronous boot exhaust |
| `EDEN_NX2_BUFFER_DOWNLOADS` | `1` | Asynchronous GPU-to-CPU buffer download on fences |
| `EDEN_NX2_KEEP_ASTC` | `1` | Skips redundant ASTC-to-BC3 compute recompression |
