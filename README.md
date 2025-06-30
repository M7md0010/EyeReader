# EyeReader

**EyeReader** is a wearable communication and control system designed for individuals with paralysis or speech impairments. Using eye blinks and winks, the system enables users to speak preset phrases or control home devices — completely hands-free.

---

## 🚀 Features
- Detects blink/wink durations using an IR sensor  
- Transmits data wirelessly via **ESP-NOW**  
- Plays audio messages using **DFPlayer Mini**  
- Supports **multiple eye-blink codes** to trigger:
  - Spoken sentences (like “I need help”, “I’m cold”, “I love you”)
  - Smart home device actions (lights, door, AC, etc.)  
- Vibrational feedback for long blinks

---

## 🛠 Tech Stack
- ESP32 (Transmitter & Receiver)
- IR Sensor (eye motion detection)
- DFPlayer Mini + MicroSD (for audio playback)
- Vibration motor for feedback
- ESP-NOW wireless communication
- C++ (Arduino framework)

---

## 📦 Hardware
| Component         | Purpose                         |
|------------------|----------------------------------|
| 2× ESP32 Boards   | Core controller (Tx & Rx)       |
| IR Sensor         | Blink detection                 |
| DFPlayer Mini     | Voice playback                  |
| Vibration Motor   | Feedback on long blink          |
| MicroSD Card      | Audio files                     |
| Wires, resistors  | Standard connections            |

---

## 🧠 How It Works

1. **User blinks or winks** → IR sensor detects duration  
2. Duration is **classified** into codes (short, medium, long)  
3. Data sent wirelessly via **ESP-NOW** to receiver ESP32  
4. Code gets **translated** into:
   - A spoken phrase (via MP3)
   - Or a home automation command (via GPIO toggle)  
5. System resets for next blink

---

## 🧪 Example Blink Codes
| Blink Pattern      | Action               |
|--------------------|----------------------|
| 0-0-2              | "I need to drink"     |
| 1-1-2              | "I am cold"           |
| 0-0-0-2            | "I love you"          |
| 1-0-1-2            | "Open/Close Door"     |
| 1-1-0-2            | "Help!!"              |
| 0-1-0-2            | "Medication schedule" |

> Each number represents a blink length:  
> `0 = short`, `1 = long`, `2 = end of sentence`

---

## 🖼️ Demo / Images
Add circuit diagrams, images of the glasses, or a short GIF/video demo here — boosts engagement and recruiter wow-factor.

---

## 📜 License
MIT — Feel free to remix or build upon it.

---

## 👤 Author
**Mohamed Sameh**  
[GitHub](https://github.com/m7md0100) • [LinkedIn](https://linkedin.com/in/yourname)

