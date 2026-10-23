# Test plan

Run the build and test commands in the README. The CTest suite checks:

| Case | Expected result |
| --- | --- |
| Edge and out-of-range pixels | Correct bit positions, no out-of-bounds draw |
| Dirty display pages | Two changed pages cause exactly two 64-byte writes |
| 8×8 sprite near boundary | Visible pixels clipped to the 64×32 screen |
| Button bounce | Four samples do not create a press; five stable samples do |
| Held button | Only one press edge until release and re-press |
| Snake growth and reverse turn | Food grows body; 180° turn is ignored |
| Body collision | Game enters dead state |
| Late foreground call | Missed game periods counted, one step executed |
| Buzzer duration | 10 ms at 8 kHz is 80 ISR ticks; output ends low |

`microconsole_sim` is an integration exercise: the scripted input keeps the
game alive, scores once, updates the display, and toggles the virtual audio
pin. These are host-simulation observations, not physical-device measurements.
For a manual demo on a Unix terminal, run `microconsole_play` and use WASD to
steer, R to restart, and Q to quit.
