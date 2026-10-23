# Design notes

## Display representation

The 64×32 framebuffer is 256 bytes. A byte stores eight vertical pixels, so
pixel `(x,y)` maps to `framebuffer[(y/8)*64+x]` and bit `y%8`. This layout makes
page copying simple. Each draw call changes only the relevant page's dirty
bit. `display_flush` copies only dirty 64-byte pages to the virtual display
model. Clipping avoids writes outside the framebuffer.

## Input and timing

The four virtual GPIO inputs are active-low, as if each button used a pull-up.
Each button has a saturating 0–5 integrator. Five consecutive 1 ms samples of
press produce one press edge; five released samples reset stable state.
Short bounces do not create an edge.

The millisecond timer ISR only increments a counter. The foreground loop polls
buttons and advances the game at 100 ms intervals. If the foreground loop is
late, it executes one step and counts skipped periods rather than replaying a
burst of stale frames. Signed subtraction handles timer wrap while deadlines
stay within half the 32-bit counter range.

The buzzer has a separate 125 µs virtual timer ISR. At 8 kHz it advances an
integer phase accumulator by the requested frequency. The virtual output pin
is high in the first half of the phase and low in the second. This is square
wave synthesis, not PWM audio with amplitude control.

## Game state

The Snake grid is 16×8 cells, each represented by a 4×4 pixel block. Body
coordinates live in fixed arrays of 128 entries, avoiding allocation. A
xorshift32 PRNG chooses food and scans to the next free cell if necessary.
Collision excludes the tail on a non-growing move because the tail vacates
that cell at the same step.
