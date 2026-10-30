# Inputs Firmware

This MPLAB X firmware project scans the physical buttons and sensors and
reports detected events to the other controllers over UART. The source is in
[`main.c`](main.c), with the UART routines in [`UART.c`](UART.c) and
[`UART.h`](UART.h).

## What it does

- Configures six active-low digital inputs.
- Detects a new activation and applies a simple 200 ms delay for debounce.
- Sends the activated input number as ASCII followed by carriage return and
  line feed (`CR/LF`).
- Configures UART for 9600 baud; the UART implementation uses a 4 MHz clock
  and `SPBRG = 25`.

The firmware contains definitions for up to twelve input numbers, but only
inputs 1–6 are configured and actively scanned in the current source. Input
events are transmitted as their channel numbers; their game meanings depend on
the connected hardware and the receiving firmware.

## Input pins

| Input | PIC16F628A pin |
|---|---|
| 1 | RB3 |
| 2 | RB0 |
| 3 | RA5 |
| 4 | RA4 |
| 5 | RA3 |
| 6 | RA2 |

UART uses RB1 for RX and RB2 for TX. Input signals are active low, consistent
with the pull-up input arrangement documented in the
[hardware manual](../Manual/Manual.pdf). Confirm the board's connector
numbering and signal reference before wiring.

## Build

Open this directory's MPLAB X project and build the production configuration
with a compatible Microchip XC8 toolchain. The checked-in project metadata
references XC8 v1.38. The generated firmware artifacts are under `dist/`;
they are build outputs rather than source.

## Related documentation

- [Project overview](../README.md)
- [Hardware manual](../Manual/Manual.pdf)
- [Main-controller firmware](../SuperWarioMAIN/SuperMarioMAIN.X/README.md)
- [Output-controller firmware](../SuperWarioOUTPUTS.X/README.md)
