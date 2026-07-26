# Archive — 2022 High School Prototype

This directory contains the original EyeReader code and circuit images from the 2022 prototype.

These files are preserved for historical reference. The 2022 version used:
- 2× ESP32 DevKit boards communicating via **ESP-NOW**
- Generic IR reflectance sensor for blink detection
- Blocking `while` loops with `delay()` calls
- Breadboard construction with jumper wires

## Files

| File | Description |
|------|-------------|
| `EyeReaderTransmitter_ESP32.ino` | Original ESP32 transmitter sketch (IR sensor + ESP-NOW) |
| `EyeReaderReciever_ESP32.ino` | Original ESP32 receiver sketch (ESP-NOW + DFPlayer) |
| `Transmitter Circuit.jpg` | Fritzing-style wiring diagram for the transmitter |
| `Receiver Circuit.jpg` | Fritzing-style wiring diagram for the receiver |

> See the root [README.md](../README.md) for a detailed comparison with the 2026 remaster.
