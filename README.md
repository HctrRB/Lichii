<!--
# SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
# SPDX-License-Identifier: GPL-3.0-or-later

# SPDX-FileCopyrightText: 2018 yuzu Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later
-->

<p align="center">
  <img src="dist/branding/lichii_icon_with_logo.png" alt="Lichii" width="600">
</p>

# Lichii

Lichii is an experimental, open-source Nintendo Switch and Switch 2 emulator written in C++. It builds on Eglantine, Eden, yuzu and Sudachi, preserving their contributions while developing this fork's game compatibility and renderer fixes.

This is an independent fork, not an official release of those upstream projects. Lichii is not affiliated with Nintendo.

Visit the [official Lichii website](https://lichii.zortos.me) to find games and compare community compatibility reports.

## Lichii OoT 7800X3D Definitive Edition

This specialized build branch (`oot-7800x3d-4070s`) provides an optimized, ultra-low-latency deployment of Lichii specifically tailored for *The Legend of Zelda: Ocarina of Time* (Switch 2 Remake, Title ID `0400486024DEC000`) on **AMD Ryzen 7 7800X3D** (8C/16T Zen 4, single CCD, 96 MiB 3D V-Cache) and **NVIDIA GeForce RTX 4070 SUPER (12 GiB GDDR6X)**.

### Key Architectural Improvements:
- **Zen 4 Microarchitecture & PGO**: Compiled with LLVM/Clang 22.1.8 using `-march=znver4`, `-mtune=znver4`, ThinLTO (`-flto=thin`), and a dedicated Profile-Guided Optimization (PGO) profile trained on real Ocarina of Time gameplay sessions.
- **Single-CCD Thread Affinity**: Eliminates cross-core scheduling latency on the 7800X3D by pinning CPU emulation and audio/host services to cores 0-3 while pinning the Vulkan GPU worker threads to cores 4-7.
- **Native 1440p DLAA (DLSS 310.9.16.0)**: Streamline 2.14.16 integration running native `2560x1440` DLAA (`sl::DLSSMode::eDLAA`, Transformer Preset K) with 16x Anisotropic Filtering and secondary AA disabled, eliminating upscaling distortion while delivering crystal-clear sub-pixel stability.
- **1 GiB Expanded BufferCache**: Increases soft (960 MiB) and hard (1152 MiB) limits to comfortably accommodate Ocarina of Time's 678–848 MiB memory footprint in Castle Town and Hyrule Field, completely eliminating garbage collection stalls.
- **50 µs Semaphore Channel Polling**: Ultra-low-latency channel sync in `gpu_thread.cpp` reducing frame presentation variance.
- **Build 116 Upstream Stability Imports**:
  - Vulkan disk-cache pipeline eviction protection (`#550`)
  - Dynamic FPS real-time scaling down to 10 fps (`#584`)
  - Audio sysmix queue silence protection (`#585`)
  - Framebuffer device dimension clamping and 64 KiB stream buffer tail padding (`#427`)
  - Descriptor payload dynamic storage growth (`#495`)
  - Sharpening anti-aliasing network bypass option (`#616`)
- **Vulkan Pipeline Cache v54**: Fully compatible with the modern NaN-safe instruction format, resolving legacy storage pointer exceptions.
- **Comprehensive Documentation**: See [`docs/performance/OOT-7800X3D-OPTIMIZATIONS.md`](docs/performance/OOT-7800X3D-OPTIMIZATIONS.md) for detailed telemetry analysis and runtime switch configuration.

## Downloads and setup

Published builds belong on this fork's [releases page](https://github.com/zortos293/Eglantine/releases). The repository still uses its historical Eglantine name; the desktop application is now Lichii.

On Windows, extract the entire build archive and run **lichii.exe**. Only use games and system files you have legally obtained. Games, keys and firmware are not included.

### Existing saves and settings

The rebrand intentionally retains the existing Eden/Eglantine data directories and configuration keys. It does not move or delete saves, settings or shader caches. Back up your saves before testing experimental builds.

## Compatibility and issue reports

Compatibility and performance vary by game, hardware and driver. Rendering errors, stutters and crashes remain possible. Passing automated tests does not establish that a game renders correctly, and this project makes no universal frame-rate guarantee.

Browse [community compatibility reports](https://lichii.zortos.me/reports) for results with each player's build and setup.

Desktop reporting requires a build containing [PR #303](https://github.com/zortos293/Lichii/pull/303) and is not available in build 88. In builds that include it, play and stop a Switch 2 game, then right-click that game and choose **Submit compatibility report**. Only the latest session in the current Lichii process is eligible.

Confirm the matching website catalog entry and review the editable machine, game, setup, and enabled add-on details prefilled where available. Check the public preview and give explicit consent before submitting. Nothing uploads automatically, and the native v1 reporter does not read or upload logs or files.

When [reporting an issue](https://github.com/zortos293/Eglantine/issues), include:

- The exact build, game version, CPU, GPU and driver version.
- Relevant settings and steps to reproduce the problem.
- A screenshot and the log from the affected session, after checking it for personal information.

Do not upload game files, keys, firmware or private account information.

## Building and contributing

See the [build guide](docs/Build.md), [user documentation](docs/user/README.md) and [coding guidelines](docs/Coding.md). Some inherited documentation and internal build target names still use upstream names. The GUI build target remains `yuzu`; its output is `lichii.exe` on Windows and `lichii` on other desktop platforms.

Keep changes focused and distinguish code-level checks from actual gameplay testing.

## Artwork
Art created by YOB (yobslab)
The supplied Lichii logo, icon and library artwork are in [dist/branding](dist/branding). Windows, Linux and macOS icon resources are generated from that artwork by [Generate-LichiiIcons.ps1](tools/Generate-LichiiIcons.ps1).

## Support Lichii

Lichii is free and stays free. If you want to help, you can donate in crypto. Donations help pay for the AI tools and build machines used to develop this open-source project faster.

**Solana (SOL)**
```text
JepEjfyWjGjhJvHSrqj52KtLjcbbbmUh1iGqerYBF4w
```

**Ethereum (ETH)**
```text
0x055ABa1b80Bc35C836Ce7E9F47c2b3d6C75b5534
```

**Bitcoin (BTC)**
```text
bc1qcvxdpf4qvdhqp8ee4s355cjmxrzsphsdwpl8w4
```

**BNB Smart Chain (BNB)**
```text
0x055ABa1b80Bc35C836Ce7E9F47c2b3d6C75b5534
```

**Polygon**
```text
0x055ABa1b80Bc35C836Ce7E9F47c2b3d6C75b5534
```

Send each coin on its matching network. Coins sent on the wrong network can't be recovered.

> [!NOTE]
> Donations are voluntary gifts and are non-refundable. A donation does not buy any product, service, perk, early access, exclusive build, priority support or say in the project. Every release stays free and public for everyone.

## Credits and license

Lichii inherits the work of the Eglantine, Eden, yuzu and Sudachi projects and their contributors, including Eglantine's original developer Camille LaVey. Original copyright notices and third-party licenses are retained.

See [LICENSE.txt](LICENSE.txt) and the source files for applicable license terms.
