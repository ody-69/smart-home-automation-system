# Smart Home Automation System

An Arduino-based smart home system with password-protected door access, automatic climate control, ambient lighting, an intrusion security mode, and a live LCD status display — fully controllable over Bluetooth.

## Features

- **Password-Protected Door Access** — Servo-driven door lock that opens on a correct code sent over Bluetooth, with support for resetting the password (old-password verification required)
- **Automatic Climate Control** — Reads ambient temperature and drives a cooling fan (via motor driver) once it crosses a threshold; the threshold can be tuned live with a potentiometer or set remotely over Bluetooth
- **Adaptive Lighting** — LDR-based ambient light sensing combined with a manual switch to automatically turn lights on in low-light conditions
- **Security Mode** — Toggleable arming system: when active, monitors a window IR sensor and PIR motion sensor and triggers a buzzer/LED alert
- **Live LCD Dashboard** — 16x2 I2C LCD showing real-time temperature, security mode state, and door status, refreshing only when a value changes
- **Bluetooth Command Interface** — Full remote control and status polling from a phone (see command list below)

## Hardware

- Arduino (Uno/Nano-class board)
- 16x2 I2C LCD display
- HC-05/HC-06 Bluetooth module
- Servo motor (door lock mechanism)
- DC motor + driver (cooling fan)
- Temperature sensor (analog)
- LDR (light-dependent resistor)
- IR sensors (door state + window/security)
- PIR motion sensor
- Buzzer, status LED, potentiometer, manual light switch

## Tools & Requirements

- Arduino IDE
- Libraries: `LiquidCrystal_I2C`, `Servo`, `SoftwareSerial`

## How to Run

1. Wire the components according to the pin definitions at the top of `SmartHomeProject.ino`
2. Upload the sketch via Arduino IDE
3. Pair a phone with the Bluetooth module and send these commands through any Bluetooth serial terminal app:

| Command | Action |
|---|---|
| `door_code` | Prompts for the door code, opens the door if correct |
| `open_door` / `close_door` | Manual door control |
| `reset_pass` | Change the door password (requires the old one) |
| `security_on` / `security_off` | Arm/disarm the security system |
| `temp_limit` | Set a custom temperature threshold for the fan |
| `status` | Get current temperature, door state, and security mode |

## Author

Eyad Ashraf Zaki — Mechatronics & Robotics Engineering, Ain Shams University