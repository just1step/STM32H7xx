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

- `M101`: enable pressure mode. This turns on the air pump and selects the pressure valve path.
- `M102`: enable vacuum mode. This turns on the air pump and selects the vacuum valve path.
- `M103`: stop the air pump and de-energize both pump valves.

### Airslide control

- `M104`: enable `AIRSLIDE1`. This command is only valid while the system is already in pressure mode.
- `M105`: disable `AIRSLIDE1`.

### Silver paste and solder paste valve control

- `M108`: trigger a pulse on the silver paste valve (`AUX7`).
- `M109`: trigger a pulse on the solder paste valve (`AUX8`).
- `M110`: force the silver paste valve output low.
- `M111`: force the solder paste valve output low.

Both pulse commands support an optional `P` argument in milliseconds:

```gcode
M108
M108 P120
M109 P80
```

If no `P` argument is supplied, the board map defaults are used:

- `SILVER_PASTE_CLAMP_PULSE_MS = 50`
- `SOLDER_PASTE_CLAMP_PULSE_MS = 50`

---
2024-12-19
