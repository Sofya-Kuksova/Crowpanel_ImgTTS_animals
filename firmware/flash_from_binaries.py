#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
flash_from_binaries_strict.py

Zero-arg UX: just run the EXE in a folder that contains a sibling `binaries/`
directory. The tool will:
  1) auto-locate `binaries/`
  2) interactively ask you to pick a COM port
  3) flash four mandatory images to fixed offsets

Required files inside `binaries/`:
  bootloader.bin       -> 0x0000
  partition-table.bin  -> 0x8000
  proj.bin             -> 0x10000
  spiffs.bin           -> 0x110000

Hard-coded esptool settings (for esptool >= 5.x):
  --chip esp32s3
  --baud 460800
  --before default-reset
  --after  hard-reset
  write-flash
  --force
  --flash-mode dio
  --flash-freq 80m
  --flash-size 16MB
"""

from __future__ import annotations
import sys
import argparse
import subprocess
import runpy
from pathlib import Path
from typing import List, Tuple, Optional

# --- Fixed settings ---
CHIP = "esp32s3"
BAUD = 460800
FLASH_MODE = "dio"
FLASH_FREQ = "80m"
FLASH_SIZE = "16MB"

# Fixed flash map
SLOTS: List[Tuple[str, str]] = [
    ("0x0000",   "bootloader.bin"),
    ("0x8000",   "partition-table.bin"),
    ("0x10000",  "proj.bin"),
    ("0x110000", "spiffs.bin"),  # mandatory
]

# Prefer typical USB-UART bridges first in the picker
PREFERRED_VIDS = {0x10C4, 0x1A86, 0x0403}  # CP210x, CH340, FTDI


def find_serial_ports():
    """Return available serial ports using pyserial (empty list if pyserial is missing)."""
    try:
        import serial.tools.list_ports as list_ports  # type: ignore
    except Exception:
        return []
    return list(list_ports.comports())


def choose_port_interactive() -> Optional[str]:
    """Interactive COM port picker; returns the chosen device name (e.g., 'COM6') or None."""
    ports = find_serial_ports()
    if not ports:
        return None

    preferred, others = [], []
    for p in ports:
        try:
            if getattr(p, "vid", None) in PREFERRED_VIDS:
                preferred.append(p)
            else:
                others.append(p)
        except Exception:
            others.append(p)

    ordered = preferred + others
    if len(ordered) == 1:
        print(f"Found one port: {ordered[0].device}")
        return ordered[0].device

    print("Multiple serial ports found:")
    for i, p in enumerate(ordered, 1):
        desc = p.description or ""
        vid = getattr(p, "vid", None)
        pid = getattr(p, "pid", None)
        print(f"  {i}. {p.device} - {desc} VID:{vid} PID:{pid}")

    sel = input(f"Select number [1-{len(ordered)}] (Enter=1): ").strip()
    if sel == "":
        return ordered[0].device
    if sel.isdigit():
        idx = int(sel) - 1
        if 0 <= idx < len(ordered):
            return ordered[idx].device
    # allow typing exact device name
    for p in ordered:
        if p.device == sel:
            return sel
    print("Invalid selection.")
    return None


def build_esptool_args(port: str, pairs: List[Tuple[str, str]]) -> List[str]:
    """Build esptool >=5.x argument list."""
    args: List[str] = [
        "--chip", CHIP,
        "-p", port,
        "-b", str(BAUD),
        "--before", "default-reset",
        "--after", "hard-reset",
        "write-flash",
        "--force",
        "--flash-mode", FLASH_MODE,
        "--flash-freq", FLASH_FREQ,
        "--flash-size", FLASH_SIZE,
    ]
    for off, f in pairs:
        args.extend([off, f])
    return args


def run_esptool_subprocess(args: List[str]) -> int:
    """Run esptool via subprocess: `python -m esptool <args...>`."""
    cmd = [sys.executable or "python", "-m", "esptool"] + args
    print("Running:", " ".join(cmd))
    try:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        assert proc.stdout is not None
        for line in proc.stdout:
            print(line, end="")
        return proc.wait() or 0
    except KeyboardInterrupt:
        print("\nAborted by user.")
        try:
            proc.kill()  # type: ignore
        except Exception:
            pass
        return 2
    except Exception as e:
        print("Failed to start esptool:", e)
        return 3


def run_esptool_inproc(args: List[str]) -> int:
    """
    Run esptool inside the same process.
    Important: use 'esptool.__main__' like `python -m esptool` would.
    """
    old = sys.argv[:]
    try:
        sys.argv = ["esptool"] + args
        runpy.run_module("esptool.__main__", run_name="__main__")
        return 0
    except SystemExit as e:
        return int(getattr(e, "code", 0) or 0)
    except Exception as e:
        print("Error running esptool in-process:", e)
        return 3
    finally:
        sys.argv = old


def resolve_binaries_dir(cli_dir: Optional[str]) -> Optional[Path]:
    """
    Resolve binaries folder:
      1) if --dir provided -> use it
      2) if frozen EXE -> try '<EXE folder>/binaries'
      3) otherwise -> try '<current working dir>/binaries'
    Returns absolute Path or None if not found.
    """
    if cli_dir:
        p = Path(cli_dir).resolve()
        return p if p.is_dir() else None

    # When frozen, base dir is the EXE location
    if getattr(sys, "frozen", False):
        exe_dir = Path(sys.executable).resolve().parent
        cand = exe_dir / "binaries"
        if cand.is_dir():
            return cand

    # Fallback to CWD
    cand = Path.cwd() / "binaries"
    if cand.is_dir():
        return cand

    return None


def parse_args():
    p = argparse.ArgumentParser(
        description="CrowPanel flasher (STRICT). Zero-arg friendly: run next to a 'binaries/' folder."
    )
    # --dir is optional now; typically you don't need it
    p.add_argument("--dir", "-d", type=str, help="Optional override for binaries folder (defaults to ./binaries or EXE_DIR/binaries).")
    return p.parse_args()


def main() -> int:
    args = parse_args()

    bin_dir = resolve_binaries_dir(args.dir)
    if not bin_dir:
        print("Binaries folder not found.\n"
              "Expected:\n"
              "  <this EXE folder>\\binaries\\  (when running as EXE)\n"
              "or\n"
              "  <current working dir>\\binaries\\ (when running as .py)\n"
              "You can also pass --dir <path> to override.")
        return 2

    # Validate required images and build (offset -> file) list
    pairs: List[Tuple[str, str]] = []
    missing = []
    for off, name in SLOTS:
        path = bin_dir / name
        if path.exists():
            pairs.append((off, str(path)))
        else:
            missing.append(name)

    if missing:
        print("Missing required files in the binaries folder:")
        for m in missing:
            print("  -", m)
        print("\nExpected structure:")
        print("  binaries/")
        print("    bootloader.bin")
        print("    partition-table.bin")
        print("    proj.bin")
        print("    spiffs.bin   # MANDATORY")
        return 2

    # Sort by address (for nice deterministic output)
    try:
        pairs.sort(key=lambda ap: int(ap[0], 16))
    except Exception:
        pass

    print("Will flash (in address order):")
    for off, f in pairs:
        print(f"  {off} -> {f}")

    # Interactive port picker
    port = choose_port_interactive()
    if not port:
        print("No serial port found/selected. Connect the device and install drivers.")
        return 4
    print("Selected port:", port)

    esptool_args = build_esptool_args(port, pairs)
    print("\nEsptool arguments:")
    print("  ", " ".join(esptool_args))

    # In EXE -> run in-process; otherwise subprocess
    if getattr(sys, "frozen", False):
        print("\nRunning embedded esptool (in-process)...")
        rc = run_esptool_inproc(esptool_args)
    else:
        rc = run_esptool_subprocess(esptool_args)

    if rc == 0:
        print("\nDone: flashing finished successfully.")
    else:
        print(f"\nFinished with return code {rc}.")
    return rc


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("Aborted by user.")
        sys.exit(1)
