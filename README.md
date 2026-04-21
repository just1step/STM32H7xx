# grblHAL driver for STM32H7xx processors

This is a port of [grblHAL](https://www.github.com/grblhAL) for the STM32H7xx series of processors, originally based on the F7 driver from the official repository.

## Supported boards
- [Nucleo-H743ZI](https://www.st.com/en/evaluation-tools/nucleo-h743zi.html)
- [Nucleo-H723ZG](https://www.st.com/en/evaluation-tools/nucleo-h723zg.html)
- [WeAct MiniSTM32H7xx  (H743 & H723)](https://github.com/WeActTC/MiniSTM32H7xx)
- [BTT SKR 3 EZ (H743 & H723)](https://www.biqu.equipment/products/bigtreetech-btt-skr-3-ez-control-board-mainboard-for-3d-printer)
- [BTT Octopus Max EZ](https://biqu.equipment/products/bigtreetech-btt-octopusmax-ez-for-3d-printer)
- [BTT Scylla](https://biqu.equipment/products/bigtreetech-scylla-v1-0)

## Building

### Web App

Builds for the above boards are supported through the upstream [web app](http://svn.io-engineering.com:8080/).

### Local builds

Local builds are supported from both STMCubeIDE and PlatformIO. Board specific settings have been included in the IDE configurations where possible - to avoid the need for code changes when building for different boards. (Please see the upstream Wiki-page [compiling grblHAL](https://github.com/grblHAL/core/wiki/Compiling-GrblHAL) for further instructions for how to import the project, configure the driver and compile).

### GitHub CI builds

PlatformIO builds for a number of configurations are run automatically as a GitHub Action on each push to the master branch. For convenience, the resulting firmwares are archived in the artifacts for each run, and can be found under the Actions tab on the GitHub repo (these files remain available for 90 days).

## H725 custom M-codes

The `H725 Core V250919` board adaptation adds a small set of board-specific M-codes in `Src/my_plugin.c` for pump, valve and airslide control.

### Pump and pneumatic mode control

- `M101 P1`: enable pressure mode. This turns on the air pump and selects the pressure valve path.
- `M101 P2`: enable vacuum mode. This turns on the air pump and selects the vacuum valve path.
- `M101 P0`: stop the air pump and de-energize both pump valves.

### Airslide control

- `M102 Y1`: enable the silver paste airslide.
- `M102 Y0`: disable the silver paste airslide.
- `M102 X1`: enable the solder paste airslide.
- `M102 X0`: disable the solder paste airslide.

If either airslide is turned on, the system must already be in pressure mode.

### Silver paste and solder paste valve control

- `M103 Y1`: trigger a pulse on the silver paste valve (`AUX7`).
- `M103 X1`: trigger a pulse on the solder paste valve (`AUX8`).
- `M103 Z1`: trigger a pulse on the UV valve (`AUX10`).

Both pulse commands support an optional `P` argument in milliseconds:

```gcode
M103 Y1
M103 Y1 P120
M103 X1 P80
M103 Z1
M103 Z1 P1000
```

If no `P` argument is supplied, the board map defaults are used:

- `SILVER_PASTE_CLAMP_PULSE_MS = 50`
- `SOLDER_PASTE_CLAMP_PULSE_MS = 50`

### Suction nozzle control

- `M104 P1`: enable the suction nozzle valve (`AUX9`). This command is only valid while the system is already in vacuum mode.
- `M104 P0`: disable the suction nozzle valve (`AUX9`).

## H725 M-code 中文说明

下面这组命令是 `H725 Core V250919` 板级适配里新增的自定义控制命令，主要用于气泵、气动滑台、点胶阀和贴片吸头控制。

### 1. 气泵模式控制

- `M101 P1`
  作用：切换到压力模式。
  行为：打开气泵，并切换到吹气路径。

- `M101 P2`
  作用：切换到真空模式。
  行为：打开气泵，并切换到吸气路径。

- `M101 P0`
  作用：关闭气泵。
  行为：关闭气泵，并关闭两路气泵电磁阀。

### 2. 气动滑台控制

- `M102 Y1`
  作用：打开银浆滑台。

- `M102 Y0`
  作用：关闭银浆滑台。

- `M102 X1`
  作用：打开锡膏滑台。

- `M102 X0`
  作用：关闭锡膏滑台。

注意：
- 只要命令里有滑台打开动作，也就是 `X1` 或 `Y1`，当前系统必须已经处于压力模式，否则命令会被拒绝。

### 3. 点胶脉冲阀控制

- `M103 Y1`
  作用：触发一次银浆点胶阀脉冲。

- `M103 X1`
  作用：触发一次锡膏点胶阀脉冲。

- `M103 Z1`
  作用：触发一次 UV 阀脉冲。

- `M103 Y1 P120`
  作用：触发一次银浆点胶阀脉冲，脉宽为 `120ms`。

- `M103 X1 P80`
  作用：触发一次锡膏点胶阀脉冲，脉宽为 `80ms`。

- `M103 Z1 P1000`
  作用：触发一次 UV 阀脉冲，脉宽为 `1000ms`。

说明：
- `Y` 表示银浆阀。
- `X` 表示锡膏阀。
- `Z` 表示 UV 阀。
- `P` 表示脉宽，单位为毫秒。
- 如果不带 `P` 参数，则使用板级默认值。

当前默认脉宽：
- `SILVER_PASTE_CLAMP_PULSE_MS = 50`
- `SOLDER_PASTE_CLAMP_PULSE_MS = 50`

### 4. 贴片吸头控制

- `M104 P1`
  作用：打开贴片吸头电磁阀。

- `M104 P0`
  作用：关闭贴片吸头电磁阀。

注意：
- `M104 P1` 只有在当前已经处于真空模式时才允许执行。
- 如果当前不是 `M101 P2` 真空模式，吸头打开命令会被拒绝。

### 5. 推荐使用顺序

压力相关动作推荐顺序：

```gcode
M101 P1
M102 Y1
M103 Y1 P100
M102 Y0
```

真空吸头相关动作推荐顺序：

```gcode
M101 P2
M104 P1
M104 P0
M101 P0
```

---
2024-12-19
