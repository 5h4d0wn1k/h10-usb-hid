#!/usr/bin/env python3
"""H10 - USB HID Emulator host helper: ASCII->HID keycode validation + typing sim.
Only ever intends keys on machines the operator owns and controls.
Educational/authorized own-lab use only (see README "IMPORTANT").
"""
import argparse
import os
import sys

MOD = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, MOD)
from hw_common import DEMO_TAG

# Minimal ASCII -> (modifier, keycode) map (USB HID usage table subset)
LOWER = {c: i + 4 for i, c in enumerate("abcdefghijklmnopqrstuvwxyz")}
UPPER = {c: i + 4 for i, c in enumerate("ABCDEFGHIJKLMNOPQRSTUVWXYZ")}
DIGITS = {c: i + 0x1E for i, c in enumerate("1234567890")}
SPECIAL = {" ": (0, 0x2C), "-": (0, 0x2D), "=": (0, 0x2E), "[": (0, 0x2F),
           "]": (0, 0x30), "\\": (0, 0x31), ";": (0, 0x33), "'": (0, 0x34),
           ",": (0, 0x36), ".": (0, 0x37), "/": (0, 0x38), "\n": (0, 0x28)}


def map_char(c):
    if c in LOWER:
        return (0, LOWER[c])
    if c in UPPER:
        return (0x02, UPPER[c])
    if c in DIGITS:
        return (0, DIGITS[c])
    if c in SPECIAL:
        return SPECIAL[c]
    return None


def type_string(s):
    out = []
    for ch in s:
        m = map_char(ch)
        if m is None:
            out.append((ch, None))
        else:
            out.append((ch, {"mod": m[0], "key": m[1]}))
    return out


def run_demo():
    print("=== H10 USB HID keymap validation (offline) ===")
    for ch, m in type_string("Hello World 42"):
        if m is None:
            print("  %-3r  (unsupported)" % ch)
        else:
            print("  %-3r  mod=0x%02X key=0x%02X" % (ch, m["mod"], m["key"]))
    print(DEMO_TAG)
    return 0


def main(argv=None):
    p = argparse.ArgumentParser(
        description="H10 USB HID - ASCII->HID keycode mapper (offline)")
    p.add_argument("--demo", action="store_true", help="offline demo (exit 0)")
    p.add_argument("--text", help="string to map")
    args = p.parse_args(argv)
    if args.text:
        for ch, m in type_string(args.text):
            print("%-3r -> mod=0x%02X key=0x%02X" % (ch, m["mod"], m["key"]) if m else
                  ("%-3r -> unsupported" % ch))
        return 0
    return run_demo()


if __name__ == "__main__":
    sys.exit(main())
