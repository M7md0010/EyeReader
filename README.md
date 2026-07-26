# EyeReader — Assistive Blink-to-Speech & Smart-Home Wearable

> A wearable eye-blink interface that translates eyelid gestures into spoken phrases and smart-home commands for individuals with paralysis or speech impairments.

**Authors:** Mohamed Sameh & Mohamed Elawady  
**License:** MIT

---

## Table of Contents

- [Project Overview](#project-overview)
- [2022 → 2026 Evolution](#2022--2026-evolution)
- [Hardware Specifications (2026 Remaster)](#hardware-specifications-2026-remaster)
- [Firmware Architecture (2026 Remaster)](#firmware-architecture-2026-remaster)
- [Repository Structure](#repository-structure)
- [Build & Production Files](#build--production-files)
- [Blink Code Reference](#blink-code-reference)
- [Getting Started](#getting-started)
- [License](#license)

---

## Project Overview

EyeReader is a wearable assistive device that decodes voluntary eye-blink patterns — measured via a Time-of-Flight proximity sensor — into a set of pre-programmed actions. In **Speaking Mode**, blink codes trigger audio playback of phrases such as *"I need to drink"* or *"Help!!"* through a DFPlayer Mini. In **Smart-Home Mode**, the same blink vocabulary toggles GPIO-controlled appliances (lights, fans, doors, AC, etc.).

The system comprises two units:

| Unit | Role |
|------|------|
| **Transmitter** | Glasses-mounted sensor module. Detects eyelid proximity, classifies blink duration via hardware interrupts, and transmits events over BLE. |
| **Receiver** | Bedside or wearable hub. Decodes blink sequences, drives audio playback, and controls smart-home relays. |

---

## 2022 → 2026 Evolution

This project was originally built in 2022 as a high-school engineering prototype and has since been remastered as a university-level Computer Engineering portfolio piece.

| Aspect | 2022 Prototype | 2026 Remaster |
|--------|---------------|---------------|
| **MCU** | 2× ESP32 DevKits (breadboarded) | Nordic nRF52840 SoC on a custom 4-layer HDI PCB |
| **Sensor** | Generic IR reflectance sensor | VL53L0X Time-of-Flight sensor over a flexible printed circuit (FPC) |
| **Wireless** | ESP-NOW (Wi-Fi based, high power) | Bluetooth Low Energy 5.0 (BLE) |
| **Antenna** | External PCB trace (hand-routed) | Standard built-in onboard zig-zag trace antenna (validated RF performance) |
| **Power Management** | USB-only, no battery | MCP73831 single-cell LiPo charger with integrated thermal regulation |
| **Haptic Driver** | Direct GPIO (no protection) | BSS138 N-Channel MOSFET + 1N4148 flyback diode motor driver |
| **Form Factor** | Breadboard + jumper wires (~120 × 80 mm) | 15 × 50 mm 4-layer HDI PCB |
| **Firmware Model** | Blocking `while` loops with `delay()` calls | Event-driven FreeRTOS architecture with hardware interrupts |
| **Power Consumption** | ~80 mA continuous (Wi-Fi always on) | ~8 µA deep sleep (BLE advertising + `waitForEvent()`) |
| **Code** | Monolithic `.ino` sketches | Modular C++ with ISR-driven state machine and RTOS software timers |

> The original 2022 code and Fritzing circuit images are preserved in the [`/Archive_2022`](Archive_2022/) directory for reference.

---

## Hardware Specifications (2026 Remaster)

### PCB

- **Dimensions:** 15 × 50 mm, 4-layer HDI stack-up
- **Fabrication:** Standard JLCPCB/PCBWay-compatible Gerber output
- **Design Tool:** KiCad 8.x

### Core Components

| Component | Part Number | Function |
|-----------|-------------|----------|
| **SoC** | Nordic nRF52840 | ARM Cortex-M4F @ 64 MHz, BLE 5.0, 1 MB Flash, 256 KB RAM |
| **ToF Sensor** | VL53L0X (ST) | 940 nm VCSEL laser ranging, I²C, hardware interrupt on threshold crossing. Mounted on FPC for flexible glasses-frame routing. |
| **LiPo Charger** | MCP73831 (Microchip) | Single-cell Li-Ion/LiPo charge management, 500 mA programmable charge current, thermal regulation |
| **Haptic Driver** | BSS138 + 1N4148 | Low-side N-FET switch for ERM vibration motor with flyback protection diode |

### RF Design

The RF front-end utilizes the **standard built-in onboard zig-zag trace antenna** pattern routed on the PCB top layer. This antenna geometry is a well-characterized design for 2.4 GHz BLE applications and was selected for its compact footprint and proven radiation efficiency without requiring an external antenna or matching network. The design has been validated for reliable BLE communication at the target operating range.

### Power Architecture

```
USB-C → MCP73831 → LiPo Cell (3.7V)
                 ↘ LDO → nRF52840 (1.8V–3.3V)
                       → VL53L0X (2.8V via XSHUT gating)
```

- **Deep sleep current:** ~8 µA (nRF52840 System ON, RAM retained, BLE advertising at 244 ms interval)
- **Active sensing current:** ~12 mA (ToF continuous ranging at 50 ms period)

---

## Firmware Architecture (2026 Remaster)

The firmware has been completely re-architected from the original ESP32 blocking-loop design to an **event-driven RTOS model** on the nRF52840.

### Key Design Decisions

1. **Hardware Interrupt-Driven Sensing:**  
   The VL53L0X is configured with a programmable distance threshold. When the eyelid crosses this threshold, the sensor asserts its GPIO interrupt pin. An ISR (`tof_isr`) captures the timestamp on both edges — eliminating continuous polling and enabling the MCU to remain in deep sleep between blinks.

2. **FreeRTOS Software Timers for Haptics:**  
   Haptic feedback (vibration motor pulse on long blinks ≥ 1000 ms) is managed by a one-shot FreeRTOS `SoftwareTimer` rather than a blocking `delay()`. This keeps the main loop non-blocking and power-efficient.

3. **Dynamic Threshold Detection:**  
   The blink threshold (currently 20 mm) is designed to be dynamically calibrated via a button ISR (pin reserved, TODO in firmware). This accommodates per-user anatomical variation.

4. **BLE Notification Model:**  
   Blink duration is transmitted as a 32-bit unsigned integer via a BLE GATT characteristic with `NOTIFY` property. The receiver subscribes to notifications and processes blink codes asynchronously — no polling or request/response overhead.

5. **Ultra-Low Power Loop:**  
   After processing a blink event, the main loop calls `waitForEvent()` (ARM WFE instruction), immediately returning the CPU to deep sleep. The loop thread is also suspended at startup via `suspendLoop()`, ensuring zero idle power waste.

### Firmware Files

| File | Description |
|------|-------------|
| [`Firmware/Transmitter.cpp`](Firmware/Transmitter.cpp) | nRF52840 transmitter — ISR-driven blink detection, BLE GATT server, FreeRTOS haptic timer |
| [`Firmware/EyeReaderReciever.ino`](Firmware/EyeReaderReciever.ino) | ESP32 BLE client receiver — blink code decoder, DFPlayer audio, smart-home GPIO control |

---

## Repository Structure

```
EyeReader/
├── Hardware/              # KiCad 8.x project files (schematic, PCB, netlist)
│   ├── v1.0.kicad_pro
│   ├── v1.0.kicad_sch
│   ├── v1.0.kicad_pcb
│   └── ...
├── Firmware/              # nRF52840 + ESP32 BLE firmware (2026)
│   ├── Transmitter.cpp
│   └── EyeReaderReciever.ino
├── Docs/                  # Generated documentation (schematic PDFs, etc.)
├── Production/            # Manufacturing output (Gerbers, drill files)
├── Archive_2022/          # Original high-school prototype code & images
│   ├── EyeReaderTransmitter_ESP32.ino
│   ├── EyeReaderReciever_ESP32.ino
│   ├── Transmitter Circuit.jpg
│   └── Receiver Circuit.jpg
├── generate_production.sh # KiCad CLI export automation script
└── README.md              # This file
```

---

## Build & Production Files

### Automated Export Script

A bash script is provided to generate all manufacturing and documentation files from the KiCad project:

```bash
./generate_production.sh
```

This script uses the KiCad CLI to:
1. **Export schematic PDF** → `Docs/EyeReader_Schematic.pdf`
2. **Export Gerber files** → `Production/`
3. **Export drill files** → `Production/`

> **Prerequisites:** KiCad 8.x must be installed and `kicad-cli` available on `$PATH`.

---

## Blink Code Reference

Each blink is classified by duration:  
- `0` = Short blink (1–2 s)  
- `1` = Long blink (2–3 s)  
- `2` = End-of-sequence marker (0.5–1 s)  
- **Mode switch** = Very long blink (3–5 s)

### Speaking Mode

| Code | Phrase |
|------|--------|
| `0-0-2` | "I need to drink" |
| `0-1-2` | "I need to eat" |
| `1-0-0-2` | "I need to obey nature's call" |
| `1-1-0-2` | "Help!!" |
| `1-0-1-2` | "Hello" |
| `1-1-2` | "I am cold" |
| `1-1-1-2` | "Thanks" |
| `0-1-0-2` | "Give me a hug" |
| `0-0-1-2` | "I need to shower" |
| `0-0-0-2` | "I love you" |
| `0-1-1-2` | "Tell me a joke" |

### Smart-Home Mode

| Code | Action |
|------|--------|
| `0-0-2` | Toggle lamp |
| `0-1-2` | Toggle TV |
| `1-0-0-2` | Toggle fan |
| `1-1-0-2` | Open/close curtains |
| `1-0-1-2` | Open/close door |
| `1-1-2` | Toggle AC/heater |
| `1-1-1-2` | Read a book |
| `0-1-0-2` | Medication schedule |
| `0-0-1-2` | Monitor cameras |
| `0-0-0-2` | Make a call |
| `0-1-1-2` | Measure heart rate |

---

## Getting Started

### Hardware
1. Open the KiCad project from `Hardware/v1.0.kicad_pro`.
2. Review the schematic and PCB layout.
3. Run `./generate_production.sh` to generate Gerber and drill files for PCB fabrication.

### Firmware
1. **Transmitter (nRF52840):** Open `Firmware/Transmitter.cpp` in the Arduino IDE with the Adafruit nRF52 BSP installed. Select the `Adafruit Feather nRF52840` board (or your target) and upload.
2. **Receiver (ESP32):** Open `Firmware/EyeReaderReciever.ino` in the Arduino IDE with the ESP32 board package. Upload to the receiver ESP32.

### Dependencies
- [Adafruit nRF52 Arduino BSP](https://github.com/adafruit/Adafruit_nRF52_Arduino)
- [Adafruit VL53L0X Library](https://github.com/adafruit/Adafruit_VL53L0X)
- [DFRobotDFPlayerMini Library](https://github.com/DFRobot/DFRobotDFPlayerMini)

---

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

**Mohamed Sameh** · [GitHub](https://github.com/m7md0010)  
**Mohamed Elawady** · Co-Author
