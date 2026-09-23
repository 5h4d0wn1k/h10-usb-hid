> **⚠️ EDUCATIONAL USE ONLY — AUTHORIZED TESTING ONLY.**
> This project exists for education, research, and **defense of systems you own
> or hold explicit written authorization to assess**. Unauthorized use is
> prohibited and may be illegal. Read [ETHICS.md](ETHICS.md) and
> [SCOPE.md](SCOPE.md) before use. Use at your own risk; **AS IS**, no warranty.

# H10 — USB HID Emulator

USB HID attack tool for hardware security testing — ESP32-S2/S3 firmware that emulates a USB
keyboard and mouse, injects keystrokes and combos, runs DuckyScript-style payloads, and
anti-sleep jiggle, with a Python host helper for offline demos.

![MIT](https://img.shields.io/badge/license-MIT-blue.svg)
![GitHub stars](https://img.shields.io/github/stars/5h4d0wn1k/h10-usb-hid)
![GitHub last commit](https://img.shields.io/github/last-commit/5h4d0wn1k/h10-usb-hid)
![GitHub issues](https://img.shields.io/github/issues/5h4d0wn1k/h10-usb-hid)

## Why

USB is a trust boundary that rarely stops to ask questions — which is exactly why HID
keystroke-injection research matters for hardware security. H10 demonstrates how a $15 ESP32-S2/S3
board with native USB OTG becomes a Human Interface Device: keyboard and mouse emulation, key
combinations, DuckyScript payload lines, and an anti-sleep jiggler, driven over a serial command
interface. It is an educational hardware-security and USB-red-team instrument. Keystroke injection
into any machine you do not own or lack written authorization to test is illegal; keep this on your
own lab bench.

## Features

- **USB keyboard emulation** — type strings, single keys, and combos (`ctrl+c`, `alt+f4`, `gui+r`).
- **USB mouse emulation** — relative move, click (left/right/middle), scroll, circle pattern,
  anti-sleep jiggle.
- **DuckyScript payloads** — `type`, `gui`, `ctrl`, `delay`, `enter` lines over serial.
- **Macros** — copy, paste, undo, redo, select-all, save, find, newtab, close, refresh, lock, run,
  alttab, screenshot.
- **Host helper** — `host/h10_cli.py --demo` (offline, exit 0) plus `hw_common.py`.

## Quickstart

Board: ESP32-S2 or ESP32-S3 (native USB OTG required). Toolchain: Arduino CLI.

```bash
# Build + flash (ESP32-S3 default FQBN)
arduino-cli compile --fqbn esp32:esp32:esp32s3 firmware/h10_usb_hid/h10_usb_hid.ino
arduino-cli upload --fqbn esp32:esp32:esp32s3 --port /dev/ttyUSB0 firmware/h10_usb_hid/h10_usb_hid.ino

# Offline host demo (no board needed)
python3 host/h10_cli.py --demo
python3 host/h10_cli.py --text "hello"
```

Serial commands start with `type`, `key`, `combo`, `macro`, `mouse`, `click`, `scroll`, `jiggle`,
`circle`, and `payload …`. See `firmware/README.md` for the full command reference and wiring.

## Tests

```bash
python3 -m unittest discover -s tests -v
```

## Project structure

- `firmware/h10_usb_hid/h10_usb_hid.ino` — ESP32-S2/S3 HID sketch.
- `host/` — `h10_cli.py` and `hw_common.py` host helpers.
- `tests/` — unit tests.

## Documentation

- [firmware/README.md](firmware/README.md) — board, wiring, and serial reference.
- [CONTRIBUTING.md](CONTRIBUTING.md)
- [SECURITY.md](SECURITY.md)
- [ETHICS.md](ETHICS.md) · [SCOPE.md](SCOPE.md)

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Keep the lab-only gates and legal notices intact.

## License

MIT — see [LICENSE](LICENSE).