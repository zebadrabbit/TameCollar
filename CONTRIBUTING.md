# Contributing

Thanks for helping improve this project.

## Development setup

This project is built with PlatformIO.

- VS Code + PlatformIO IDE is the easiest setup.
- CLI is also supported.

Useful commands:

- Build: `pio run`
- Upload: `pio run -t upload -e sparkfun_promicro8`
- Monitor: `pio device monitor -b 9600`

## Style / conventions

- Keep changes focused and avoid large refactors unless necessary.
- Prefer adjusting constants in `namespace Config` over sprinkling new literals throughout the code.
- When adding new behavior, document it in README and/or `docs/`.

## Pull requests

Please include:

- What hardware you tested on (board + LED ring)
- What changed and why
- Any new config values added (and defaults)

## Reporting issues

When filing an issue, include:

- Your board model (Pro Micro 5V/16MHz vs 3.3V/8MHz)
- OS and PlatformIO version
- The output of `pio run -v` if it’s a build issue
- A photo or wiring description if it’s a hardware symptom
