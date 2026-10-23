# Microconsole Firmware

A software-only handheld-console firmware project in C11. It runs Snake on a
virtual 64×32 monochrome display. The game is a workload for the firmware:
one-bit framebuffer drawing, page-based display updates, debounced button
inputs, timer-driven game logic, and a square-wave buzzer driver.

There is no physical board, vendor SDK, graphics library, or communications
protocol. The device registers and display memory are simulated in C.

## Run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/microconsole_sim
```

The simulator applies a fixed button script, prints five screen snapshots, and
reports display traffic, score, audio pin transitions, and `sizeof(console_t)`.
The first food position is scripted to demonstrate growth and the sound event.
The game rules and rendering still run through the firmware modules.

## Firmware modules

| Module | Embedded concept |
| --- | --- |
| `display.c` | 256-byte 1-bpp framebuffer, clipping, 8×8 sprites, dirty-page flush to virtual display RAM |
| `buttons.c` | Active-low virtual GPIO and 5 ms integrator debounce with edge detection |
| `buzzer.c` | 8 kHz timer-driven phase accumulator and virtual digital output pin |
| `console.c` | 1 ms timer ISR, 100 ms game cadence, bounded missed-tick accounting |
| `snake.c` | Fixed-size game state, deterministic PRNG, collision and growth logic |

All firmware state is statically sized; there is no heap allocation. The
simulator reports the size of the `console_t` structure on the host. That host
size is not a target-MCU RAM measurement because pointer and alignment sizes
can differ.

## Why the game is here

The game forces several drivers to work together under a small memory budget.
Every frame passes through the bit-packed display driver; every direction
change passes through button debounce; growth and death trigger the buzzer.
This makes low-level behavior visible in a way that a standalone driver test
cannot.

## Porting boundary

The virtual display RAM, GPIO input register, and speaker pin can be replaced
with target-specific register accesses. A real port would need an actual
display bus driver, interrupt setup, timing measurements, electrical input
configuration, and validation on hardware. This repository makes no claim of
board bring-up or real-time timing guarantees.

See [design notes](docs/design.md) and [test plan](docs/test-plan.md). MIT
licensed; see `LICENSE`.
