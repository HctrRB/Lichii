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

## Lichii (Fork para Ryzen 7 7800X3D)

Esta rama (`oot-7800x3d-4070s`) es un fork de Lichii con adaptaciones de bajo nivel para *The Legend of Zelda: Ocarina of Time* (Switch 2 Remake, Title ID `0400486024DEC000`, v1.1.1 NUP), adaptado para la arquitectura **AMD Ryzen 7 7800X3D** (8C/16T Zen 4, 1 CCD, 96 MiB 3D V-Cache) y **NVIDIA GeForce RTX 4070 SUPER (12 GiB GDDR6X)** con 32 GiB DDR5 en Windows 11.

### Rendimiento real testeado (AMD Ryzen 7 7800X3D + RTX 4070 SUPER, 2560x1440 DLAA):
| Zona / Escenario | Tasa de fotogramas (FPS) | Tiempo de frame (ms) | Observaciones de telemetría |
| :--- | :--- | :--- | :--- |
| **Kakariko Village / Zonas despejadas** | **57.1 – 59.4 FPS** | 16.8 – 17.5 ms | Rendimiento fluido y estable cerca de los 60 FPS objetivo. |
| **Hyrule Field (vistas abiertas / vegetación)** | **46.8 – 54.3 FPS** | 18.4 – 21.3 ms | Caída provocada por llamadas frecuentes de sincronización CPU (`OnCPURead`). |
| **Transición de zona / Carga inicial** | **39.9 FPS** (puntual) | ~25.0 ms | Pico transitorio de compilación de pipelines en segundo plano. |
| **Uso de BufferCache** | **846 – 848 MiB** | — | Estable dentro del margen de 1 GiB (sin tirones de recolección de basura). |
| **Consumo de VRAM** | **~4.2 GiB / 12 GiB** | — | Margen amplio libre en la RTX 4070 SUPER. |

### Cambios y características del fork:
- **Compilador Zen 4 + PGO**: Construido con Clang 22.1.8 utilizando `-march=znver4`, `-mtune=znver4`, ThinLTO (`-flto=thin`) y perfil PGO entrenado con partidas reales de OoT.
- **Afinidad de hilos por núcleos**: Evita saltos entre núcleos del CCD único del 7800X3D asignando hilos de CPU/audio a los núcleos 0-3 y Vulkan/fencing a los núcleos 4-7.
- **DLAA nativo 1440p (DLSS 310.9.16.0)**: Integración de Streamline a resolución nativa `2560x1440` (DLAA, Preset K), filtrado anisotrópico 16x y sin reescalado forzado a 4K (`EDEN_NX2_OCARINA_4K=0`).
- **BufferCache ampliado a 1 GiB**: Límites de 960 MiB (soft) y 1152 MiB (hard) para evitar el thrashing de GC que ocurría con los 480 MiB por defecto.
- **Sondeo de canal a 50 µs**: Menor latencia de sincronización en `gpu_thread.cpp`.
- **Mejoras upstream de Build 116 incorporadas**:
  - Protección contra desalojo de pipelines en caché de disco (`#550`).
  - Escala dinámica de FPS hasta 10 fps (`#584`).
  - Corrección de silencio en cola sysmix de audio (`#585`).
  - Límites de dimensiones de framebuffers y margen de stream buffer (`#427`).
  - Crecimiento dinámico del almacenamiento de descriptores (`#495`).
  - Opción de bypass de sharpening (`#616`).
- **Compatibilidad de caché v54**: Protocolo actualizado compatible con instrucciones NaN-safe.
- **Guía de optimización**: Consulta [`docs/performance/OOT-7800X3D-OPTIMIZATIONS.md`](docs/performance/OOT-7800X3D-OPTIMIZATIONS.md) para más detalles.

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
