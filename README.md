# Signals
An Arduino sketch that implements running lights, flashers, and turn signals

This sketch uses PWM to emulate two levels of brightness on a shared single filament at each lamp position, assuming Front and Rear Left, and Front and Rear Right.

The inputs are:
    D2, Left Signal switch.
    D3, Right Signal switch.
    D4, Running Lights.
    D5 Four way Flashers.

It is assumed that running lights are dim, blinkers are bright, and whether running lights are on or off, an active blinker is fully off in its off state.

Flashers override any blinker selection. That is, if the flashers are active, all light flash, regardless of blinker selection.
