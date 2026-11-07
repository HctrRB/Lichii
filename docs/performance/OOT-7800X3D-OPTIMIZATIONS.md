# Lichii (Fork para Ryzen 7 7800X3D + RTX 4070 SUPER)

## Plataforma de Pruebas
- **CPU**: AMD Ryzen 7 7800X3D (8C/16T Zen 4, 1 CCD, 96 MiB 3D V-Cache)
- **GPU**: NVIDIA GeForce RTX 4070 SUPER (12 GiB GDDR6X, Ada Lovelace)
- **RAM**: 32 GiB DDR5
- **SO**: Windows 11 25H2
- **Juego**: *The Legend of Zelda: Ocarina of Time* (Switch 2 Remake, Title ID `0400486024DEC000`, v1.1.1 NUP)
- **Renderizado**: Vulkan, 2560x1440 nativo + DLAA (Preset K, DLSS 310.9.16.0)

---

## Rendimiento Real Testeado en el Hardware del Proyecto

Datos empíricos extraídos de sesiones de telemetría continuas (7.800 fotogramas):

| Escenario / Zona | Tasa de fotogramas (FPS) | Tiempo de frame (ms) | Observaciones de telemetría |
| :--- | :--- | :--- | :--- |
| **Kakariko Village / Exteriores despejados** | **57.1 – 59.4 FPS** | 16.8 – 17.5 ms | Rendimiento fluido y estable cerca de 60 FPS. |
| **Hyrule Field (vistas abiertas / vegetación)** | **46.8 – 54.3 FPS** | 18.4 – 21.3 ms | Caída provocada por llamadas frecuentes de sincronización CPU (`OnCPURead`). |
| **Transición de zona / Carga inicial** | **39.9 FPS** (puntual) | ~25.0 ms | Pico transitorio de compilación de pipelines en segundo plano. |
| **Uso de BufferCache** | **846 – 848 MiB** | — | Estable dentro del margen de 1 GiB (sin tirones de recolección de basura). |
| **Consumo de VRAM** | **~4.2 GiB / 12 GiB** | — | Margen amplio libre en la RTX 4070 SUPER. |

---

## Optimizaciones de Arquitectura

### 1. Compilación Zen 4 y Perfil PGO
- **Clang 22.1.8 con ThinLTO**: Optimización en tiempo de enlace entre los 16 hilos Zen 4.
- **`-march=znver4 -mtune=znver4`**: Habilita AVX-512, BMI2 y predicción de saltos nativa de Zen 4.
- **OoT PGO (`lichii-windows.profdata`)**: Reconciliación de 12.209 llamadas indirectas y 169 hashes CFG de LLVM optimizados para el bucle principal de Ocarina of Time.

### 2. Afinidad de Hilos y Localidad de V-Cache
- Al disponer de un único CCD con 96 MiB de 3D V-Cache, se evita que los hilos compitan por las líneas de caché L3:
  - **Núcleos 0-3**: Emulación de CPU (Dynarmic JIT), servicios HLE y audio.
  - **Núcleos 4-7**: Hilo de renderizado Vulkan, hilo de fencing y pool de trabajadores en segundo plano.

### 3. BufferCache de 1 GiB
- La configuración estándar (~480 MiB) provocaba tirones continuos por recolección de basura en Hyrule Field y Castle Town (consumo real de 678 a 848 MiB).
- Este fork establece:
  - Límite soft: **960 MiB**
  - Límite hard: **1152 MiB**
- Mantiene el conjunto de datos activo en memoria sin vaciados forzados.

### 4. Integración DLSS 310.9.16.0 (DLAA)
- Evaluación nativa a `2560x1440` (modo DLAA, ratio 1.0) usando el preset Transformer K.
- Anti-Aliasing secundario desactivado (`anti_aliasing=0`) para evitar borrosidad.
- Filtrado anisotrópico 16x (`max_anisotropy=5`).
- Desactivación de reescalado 4K forzado (`EDEN_NX2_OCARINA_4K=0`).

---

## Switches de Ejecución (`nx2_switches.txt`)

| Switch | Valor Recomendado | Propósito |
| :--- | :--- | :--- |
| `EDEN_NX2_SPLIT` | `2` | Ejecución dividida de macros GPU front/back |
| `EDEN_NX2_FENCE_BATCH` | `1` | Agrupación por lotes de vallas GPU |
| `EDEN_NX2_FENCE_FLUSH_US` | `250` | Temporizador de descarga de valla a 250 µs |
| `EDEN_NX2_ASYNC_READ_PREFETCH` | `1` | Precarga asíncrona de trampas de lectura de memoria |
| `EDEN_NX2_OCARINA_4K` | `0` | DLAA nativo 1440p (evita tirones por escalado 4K) |
| `EDEN_NX2_DLSS_PRESET` | `K` | Preset Transformer de DLSS |
| `EDEN_NX2_PRELOAD_ALL_PIPELINES` | `0` | Protege los 12 GiB de VRAM de saturación al inicio |
| `EDEN_NX2_BUFFER_DOWNLOADS` | `1` | Descarga asíncrona de búferes GPU a CPU en fences |
| `EDEN_NX2_KEEP_ASTC` | `1` | Desactiva la recompresión redundante ASTC a BC3 |
