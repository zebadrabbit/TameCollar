# TameCollar (PlatformIO)

Firmware for a wearable LED collar built around a **SparkFun Pro Micro (ATmega32U4, 5V/16MHz)** and a small **WS2812/NeoPixel ring**.

This repo is set up for **PlatformIO** so anyone can clone, build, and upload without guessing toolchains.

## What this project does

- Drives an 8‑pixel WS2812/NeoPixel ring with multiple “modes” (Idle/Peace/Warning/Danger + solid colors)
- Reads a 4‑button (or 4‑signal) remote on pull‑ups and changes modes on press
- Optional serial control: send `1`, `2`, `3`, `4` over Serial to change modes
- Includes power‑saving behavior for all modes except Danger

## Repo layout

- `src/main.cpp` — firmware (all logic lives here)
- `platformio.ini` — board, framework, upload/monitor configuration
- `include/`, `lib/`, `test/` — standard PlatformIO folders (currently unused except placeholders)

## Hardware

### Known target

- Board: SparkFun Pro Micro **5V / 16MHz** (ATmega32U4)
- LED: WS2812/NeoPixel ring (configured for 8 pixels)

### Wiring (default)

These are the defaults in `src/main.cpp` under `namespace Config`:

- NeoPixel data: `A9`
- Pixel count: `8`
- Remote inputs (pulled up internally):
  - Pin `7` (Remote 1)
  - Pin `6` (Remote 2)
  - Pin `5` (Remote 3)
  - Pin `4` (Remote 4)

Notes:
- Inputs use `INPUT_PULLUP`, so **pressed = LOW**.
- WS2812 strips/rings are sensitive to wiring quality. Use a **common ground**, and consider a series resistor on data and a bulk capacitor on power.

## Prerequisites

Pick one:

### Option A: VS Code + PlatformIO IDE (recommended)

1. Install VS Code
2. Install the **PlatformIO IDE** extension
3. Open this folder in VS Code

### Option B: PlatformIO CLI

- Install PlatformIO Core: https://platformio.org/install/cli
- Ensure `pio` is on your PATH

## Build

From the project root:

- Build: `pio run`
- Clean: `pio run -t clean`

This project currently builds the environment named `sparkfun_promicro8` (see `platformio.ini`).

## Upload (flash)

- Upload: `pio run -t upload -e sparkfun_promicro8`

### Selecting the correct COM port (Windows)

By default, PlatformIO will try to auto-detect the port.

If auto-detect fails, edit `platformio.ini` and set:

- `upload_port = COMx`
- `monitor_port = COMx`

You can list ports with:

- `pio device list`

## Serial monitor

- Monitor: `pio device monitor -b 9600`

(If you need a specific port: `pio device monitor -p COMx -b 9600`)

## Modes / behavior

- **Idle**: ring off
- **Peace / Warning / Solid colors**: run for a few animation “cycles”, fade out, then sleep and periodically wake
- **Danger**: stays on continuously (no power-saving loop)

Exact timing/brightness knobs are all in `src/main.cpp` under `namespace Config`.

## Customization guide

Most edits you’ll want are in `src/main.cpp` → `namespace Config`:

- `NeoPixelPin`, `PixelCount`, `StripBrightness`
- `RemotePin1..4` and button index mapping
- Sleep/fade timings (`SleepMs`, `FadeMs`) and cycle counts (`ActiveCycles*`)

If you change boards:

1. Update `platformio.ini` (`board = ...`, possibly `platform = ...`)
2. Confirm the NeoPixel pin maps correctly for the new MCU

## Troubleshooting

- **LED ring doesn’t light**: verify common ground, correct data pin, and that the ring is WS2812 (not APA102).
- **Upload fails**: Pro Micro bootloaders can be picky. Double-tap reset to enter bootloader, then upload again.
- **“Wrong COM port”**: use `pio device list` and set `upload_port` in `platformio.ini`.
- **Onboard LEDs won’t turn off**: the Pro Micro power LED is hardwired; firmware can’t disable it.

See also: `docs/TROUBLESHOOTING.md`.

## Contributing

PRs welcome — see `CONTRIBUTING.md` for guidelines.

## Maintainers

- GitHub: https://github.com/zebadrabbit
