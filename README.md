# H10 — USB HID Emulator

USB keyboard/mouse emulation and keystroke injection with ESP32-S2/S3.

## Overview

This project implements a USB Human Interface Device emulator that:
- Emulates USB keyboard for keystroke injection
- Emulates USB mouse with movement, click, and scroll
- Supports key combinations (Ctrl+C, Alt+F4, Win+R, etc.)
- Predefined macros (Copy, Paste, Lock Screen, etc.)
- DuckyScript-compatible payload lines
- Anti-sleep mouse jiggler
- Serial command interface for interactive use

## Hardware

| Component | Connection | Role |
|-----------|------------|------|
| ESP32-S2/S3 DevKit | Main board | Native USB OTG |
| USB Cable | Native USB port | HID device connection |

## Requirements

**Board**: ESP32-S2 or ESP32-S3 with native USB OTG support.

Standard ESP32 does NOT have USB OTG hardware. Compile with:
```
arduino-cli compile --fqbn esp32:esp32:esp32s2
```

## Serial Commands

### Keyboard
| Command | Description |
|---------|-------------|
| `type <text>` | Type a string character by character |
| `key <name>` | Press a single key (enter, tab, f1, etc.) |
| `combo <mod+key>` | Key combination (ctrl+c, alt+f4) |
| `macro <name>` | Predefined macro |

### Mouse
| Command | Description |
|---------|-------------|
| `mouse <dx> <dy>` | Move mouse relative |
| `click [right\|middle]` | Click mouse button |
| `scroll <amount>` | Scroll wheel |
| `jiggle [amplitude]` | Anti-sleep jiggle |
| `circle [radius]` | Draw circle pattern |

### Payloads
| Command | Description |
|---------|-------------|
| `payload string <text>` | DuckyScript: type string |
| `payload gui <key>` | DuckyScript: GUI+key |
| `payload ctrl <key>` | DuckyScript: Ctrl+key |
| `payload delay <ms>` | DuckyScript: delay |
| `payload enter` | DuckyScript: Enter key |

## Key Codes

**Keys**: enter, tab, esc, backspace, space, delete, home, end, pgup, pgdn, up, down, left, right, f1-f12

**Modifiers**: ctrl, shift, alt, gui (win/meta)

**Macros**: copy, paste, undo, redo, selectall, save, find, newtab, close, refresh, lock, run, alttab, screenshot

## Serial Output

```
=== H10 — USB HID Emulator ===
[+] USB HID initialized

[*] Typing: Hello World
[+] Done.
[*] Combo: ctrl+c
[+] Combo: ctrl+c
[*] Mouse move: 100, 0
[+] Done.
```

## Build & Flash

```bash
# Using Arduino CLI (ESP32-S2)
arduino-cli compile --fqbn esp32:esp32:esp32s2 firmware/h10_usb_hid.ino
arduino-cli upload --fqbn esp32:esp32:esp32s2 --port /dev/ttyUSB0 firmware/h10_usb_hid.ino

# Standard ESP32 compiles but USB HID features are inactive
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/h10_usb_hid.ino
```

## Legal Disclaimer

## IMPORTANT: Read before use.

This project is provided for **educational and authorized security testing purposes only**.

### Authorization Requirements
- You MUST have explicit written permission from the system owner before using this tool
- Unauthorized keystroke injection into computer systems is illegal under federal and state laws
- This tool should ONLY be used on systems you own or have written authorization to test

### Legal Framework
- **Computer Fraud and Abuse Act (CFAA)**: Unauthorized access to computer systems is a federal crime
- **Wiretap Act (18 U.S.C. § 2511)**: Interception of electronic communications without consent is illegal
- **State Laws**: Many states have additional computer crime and wiretapping statutes
- **GDPR/CCPA**: Keystroke logging may capture personal data subject to privacy regulations

### Acceptable Use
- Testing USB HID attack vectors on your own systems
- Authorized penetration testing with written scope
- Academic research in controlled lab environments
- Security education and training
- Developing legitimate USB peripherals

### Prohibited Use
- Injecting keystrokes into systems you don't own
- Keystroke logging without user consent
- Bypassing authentication systems illegally
- Any activity that violates applicable laws or regulations
- Commercial use without proper licensing

### No Warranty
This software is provided "AS IS" without warranty of any kind. The author is not responsible for any misuse or damage caused by this software.

### Responsible Disclosure
If you discover vulnerabilities using this tool, follow responsible disclosure practices:
1. Report to the vendor/owner privately
2. Allow reasonable time for remediation
3. Do not exploit beyond proof of concept

## Live Lab Test Plan

Run ONLY on an isolated, authorized own-lab bench against devices, networks,
and spectrum **you own**. No third-party callers, bystanders, or spectrum users
may be within range of any test transmission.

1. **Isolate** - Put the DUT in a shielded/Faraday enclosure or a room with no
   third-party devices in range. Use attenuators on any transmit path.
2. **Own devices only** - Every target (AP, remote, tag, GPS module, drone FC,
   receiver) must be your own hardware.
3. **Lowest power, shortest duration** - Start at minimum TX power / duty cycle
   and use only the seconds needed.
4. **Record** - Save before/after logs to `reports/` (git-ignored). Never
   capture or store third-party traffic.
5. **Cleanup** - Restore placeholder SSIDs (`lab-*`), MACs (`00:11:22:33:44:55`),
   example.com / RFC5737 addresses, and clear any captured data from the device.

> Jammer / spoofer / replay projects are **proofs for study and simulation**
> only. They refuse live interference scenarios: a live bench trigger requires
> the `LAB_*` allowlist environment variable AND explicit `--yes` confirmation,
> and even then only against your own hardware in a shielded bench.

## Metrics

| Metric | Target | Where |
|---|---|---|
| Firmware compile | `arduino-cli compile --fqbn esp32:esp32:esp32s3 firmware/h10_usb_hid` PASS | CI/local |
| Host helper | `python3 host/h10_cli.py --demo` exits 0 (offline) | host/ |
| Unit tests | `python3 -m unittest discover -s tests` passes | tests/ |
| py_compile | every `host/*.py` compiles clean | CI/local |

## License

MIT
