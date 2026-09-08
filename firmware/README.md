# USB HID Emulator Firmware

## Purpose

USB keyboard/mouse emulation for lab ergonomics and HID-keystroke research; only on machines you own and control.

## Board

- **Board**: ESP32-S2/S3 DevKit (native USB)
- **FQBN**: `esp32:esp32:esp32s3`
- **Sketch**: `h10_usb_hid/h10_usb_hid.ino`

## Wiring

```
ESP32-S3 DevKit plugged into the lab machine's USB port. Serial via UART0 (CP2102) or native USB.
```

## Build

```bash
arduino-cli compile --fqbn esp32:esp32:esp32s3 firmware/h10_usb_hid
# upload (example, ESP32-C6):
# arduino-cli upload --fqbn esp32:esp32:esp32s3 --port /dev/ttyACM0 firmware/h10_usb_hid
```

## Runtime

See the root README "IMPORTANT" section before powering on. This firmware is
for authorized own-lab study. Serial console exposes the interactive command
set described in the root README. All identifiers in the sketch are
placeholders (`lab-*` SSIDs, `00:11:22:33:44:55`, RFC 5737 / example.com).
