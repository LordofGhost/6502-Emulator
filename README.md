# 6502 Emulator

This is a C++23 emulator for my own breadboard 6502 computer. The hardware came first:
after following [Ben Eater's 6502 computer series](https://eater.net/6502) and building the
machine myself, I wanted a software that makes it easier to understand the system and
develop programs for the real computer.

![6502](https://github.com/user-attachments/assets/8fc72d10-481b-4bbd-b332-685b25106403)
*My Ben Eater-inspired breadboard 6502 computer.*

## What It Emulates

The emulator follows the main parts and address layout of the computer I built:

- [W65C02S CPU](https://eater.net/datasheets/w65c02s.pdf)
- [W65C22 VIA](https://eater.net/datasheets/w65c22.pdf) for I/O
- [AT28C256](https://eater.net/datasheets/28c256.pdf) 32 KiB EEPROM for ROM
- [AS6C62256](https://eater.net/datasheets/hm62256b.pdf) 32 KiB SRAM, currently mapped into the lower RAM area

| Address range | Device |
| --- | --- |
| `$0000-$3FFF` | RAM |
| `$6000-$600F` | I/O |
| `$8000-$FFFF` | ROM |

## Architecture

The emulator is built around small hardware-like components connected by a shared bus and
driven by a clock.

- `Main.cpp` initializes  and starts the emulator.
- Every emulated device implements `Component::onClockCycle(Phase)`.
- The `CrystalOscillator` runs each cycle in two phases, `Ph1` and `Ph2`, and calls every component for each phase. This stems from the basic functionality of the individual components and how they communicate via the bus during a cycle.
- The CPU drives the address bus, data bus, and read/write state; memory and I/O components react through address decoding.
- CPU instructions are modeled as queued per-phase micro-operations, which keeps reset, fetch, and instruction timing explicit.

## Project Layout

```text
src/
  Main.cpp                  emulator entry point and component wiring
  components/
    cpu/                    W65C02 CPU model
    memory/                 RAM, ROM, and memory dump helpers
    io/                     W65C22 VIA placeholder
    clock/                  two-phase oscillator
    Bus.h                   shared address/data bus
  tools/                    logging and conversion helpers
  exceptions/               emulator exception types
```

## Quick Start

```sh
cmake -S . -B build
cmake --build build
./build/src/emulator -p path/to/rom.bin
```

The ROM image must be exactly 32 KiB, matching the emulated AT28C256 EEPROM. Useful runtime
flags:

- `-l` creates one Markdown log when the emulator stops.
- `-L` creates Markdown logs during clock cycles, limited to 100 files per run.
- `-e` continues after emulator exceptions instead of stopping at the first one.

## Status

- Basic project structure, component interfaces, ROM loading, RAM access, reset/fetch flow, and LDA-focused CPU work are implemented.
- Markdown logging can record CPU, RAM, ROM, bus, clock, I/O, and exception state according to `logsConfig.json`.
- The full CPU instruction set, W65C22 behavior, richer I/O handling, and an interactive CLI are still planned.

## Credits

This project is inspired by [Ben Eater's 6502 computer](https://eater.net/6502), whose videos
and schematics helped me build and understand the original breadboard machine.
