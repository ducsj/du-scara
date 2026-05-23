# 🔌 QUICK WIRING GUIDE

## Print this and keep it next to your robot!

---

## ESP32-C3 Super Mini Pinout

```
                    USB (your computer)
                        │
                    ┌────┴────┐
                    │         │
              ┌─────┤         ├─────┐
    5V ───────┤  ○  │         │  ○  ├────── GND
              └─────┤         ├─────┘
                    │  [ESP32] │
                    │    C3    │
                    └─────┬────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
       GPIO             GPIO             GPIO
         │                │                │
    ┌────┴────┐      ┌────┴────┐      ┌────┴────┐
    │  0-4    │      │  5-10   │      │  20-21  │
    └─────────┘      └─────────┘      └─────────┘
```

---

## CONNECT EVERYTHING

### POWER SYSTEM (5V Rail)

```
BATTERY (7.4V) ──┬──► TP4056 Charger ──► USB-C (charging)
                 │
                 └──► TPS61023 Boost ──► 5V Rail ──┬──► ESP32 (VIN)
                                                    ├──► Servos (VCC)
                                                    ├──► IR Sensor (VCC)
                                                    └──► Audio Amp (VCC)
```

### SERVOS → ESP32

```
SERVO WIRES:              CONNECT TO ESP32:
─────────────             ─────────────────
Brown (GND)    ─────────►  GND (common ground)
Red (VCC 5V)   ─────────►  5V Rail
Orange (Signal) ─────────►  GPIO 2, 3, or 4

SERVO BASE (rotates arm):     GPIO 2  ◄── Orange wire
SERVO ELBOW (bends arm):      GPIO 3  ◄── Orange wire  
SERVO GRIPPER (opens/closes): GPIO 4  ◄── Orange wire
```

### BUTTON → ESP32

```
BUTTON:            CONNECT TO:
───────            ────────────
One side  ───────►  GPIO 5
Other side ───────►  GND

[Internal pullup: ESP32 makes it HIGH by default]
[When pressed: Goes LOW]
```

### LEDS → ESP32 (with resistors!)

```
⚠️ ALWAYS USE 220Ω RESISTOR IN SERIES WITH LED! ⚠️

LED WIRING:
GPIO 6 ──┤ 220Ω ├──► Power LED (+) ──► GND
GPIO 7 ──┤ 220Ω ├──► Status LED (+) ──► GND

[LED (-) goes to GND]
[Resistor protects LED from burning out]
```

### IR SENSOR → ESP32

```
TCRT5000 IR SENSOR:     CONNECT TO:
─────────────────       ────────────
VCC (or +)  ─────────►  3.3V or 5V
GND (-)     ─────────►  GND
AO (Analog Out) ─────►  GPIO 20

[Digital output (DO) not used - we use analog]
```

### AUDIO AMP → ESP32

```
MAX98357 I2S AMP:       CONNECT TO:
───────────────         ────────────
VIN (5V)     ─────────►  5V Rail
GND          ─────────►  GND
LRC/WS       ─────────►  GPIO 21
BCLK         ─────────►  GPIO 22
DIN          ─────────►  GPIO 21 (or separate)
GAIN         ─────────►  GND (for 3W)
SD           ─────────►  3.3V (enable)

SPEAKER:              CONNECT TO:
────────              ────────────
Positive (+) ───────►  MAX98357 OUT+
Negative (-) ───────►  MAX98357 OUT-
```

---

## COMPLETE WIRING TABLE

| Component | Wire | Connect To | Pin/GPIO |
|-----------|------|------------|----------|
| **Servo Base** | Brown | GND | - |
| | Red | 5V | - |
| | Orange | Signal | GPIO 2 |
| **Servo Elbow** | Brown | GND | - |
| | Red | 5V | - |
| | Orange | Signal | GPIO 3 |
| **Servo Gripper** | Brown | GND | - |
| | Red | 5V | - |
| | Orange | Signal | GPIO 4 |
| **Button** | One leg | GPIO 5 | - |
| | Other leg | GND | - |
| **Power LED** | Anode (+) | GPIO 6 (via 220Ω) | - |
| | Cathode (-) | GND | - |
| **Status LED** | Anode (+) | GPIO 7 (via 220Ω) | - |
| | Cathode (-) | GND | - |
| **IR Sensor** | VCC | 3.3V or 5V | - |
| | GND | GND | - |
| | AO (analog) | GPIO 20 | - |
| **MAX98357** | VIN | 5V | - |
| | GND | GND | - |
| | LRC/WS | GPIO 21 | - |
| | BCLK | GPIO 22 | - |
| | DIN | GPIO 21 | - |
| | GAIN | GND | - |
| | SD | 3.3V | - |
| **Speaker** | + | MAX98357 OUT+ | - |
| | - | MAX98357 OUT- | - |
| **Battery** | + | TP4056 + | - |
| | - | TP4056 - | - |
| **Battery** | Out+ | TPS61023 IN+ | - |
| | Out- | TPS61023 IN- | - |

---

## POWER RAIL DISTRIBUTION

```
                    ┌─────────────────┐
                    │   5V RAIL       │
                    │  (From Boost)   │
                    └────────┬────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
        ▼                    ▼                    ▼
   ┌─────────┐         ┌─────────┐         ┌─────────┐
   │ ESP32   │         │ SERVOS  │         │  IR +   │
   │ 3.3V   │         │  (5V)   │         │  AUDIO  │
   │ Reg.   │         │         │         │  (5V)   │
   └─────────┘         └─────────┘         └─────────┘
        │                    │                    │
        ▼                    ▼                    ▼
   GPIO 20,21,22         GPIO 2,3,4           VCC pins
   (IR, Audio)          (Servo signals)       (Power)
```

---

## TESTING CHECKLIST

### Before Power On:
- [ ] All GND wires connected together?
- [ ] 5V rail has power from boost converter?
- [ ] No loose wires touching?
- [ ] Resistors on LED wires?

### First Power On:
- [ ] Power LED lights up?
- [ ] ESP32 gets warm (normal)?
- [ ] No smoke or burning smell?

### After Upload:
- [ ] Serial Monitor shows startup message?
- [ ] Startup jingle plays?
- [ ] Robot goes to home position?
- [ ] Status LED blinks 3 times?

### Calibration Test:
- [ ] Hold button 3 seconds
- [ ] Calibration mode activates?
- [ ] Commands 1-6 work?
- [ ] + / - and . / , adjust offsets?

### IR Sensor Test:
- [ ] No card: IR value ~100-300
- [ ] White card: IR value ~800-1000
- [ ] Card detection triggers?

---

## COMMON PROBLEMS

| Problem | Cause | Fix |
|---------|-------|-----|
| Servos jitter | Not enough power | Use external 5V supply |
| Servos don't move | Wrong pin connection | Check wiring |
| IR always high | Sensor pointed at light | Shield from ambient light |
| IR always low | Card too far | Keep card 2-5cm away |
| No sound | Wrong I2S pins | Check GPIO 21, 22 |
| ESP32 not responding | Bad USB connection | Try different cable |

---

## SAFETY REMINDERS

⚠️ **NEVER** connect 5V to ESP32-C3's GPIO pins (they're 3.3V max!)

⚠️ **ALWAYS** use resistors on LEDs!

⚠️ **DO NOT** power servos from ESP32's internal regulator!

⚠️ **DO NOT** reverse battery polarity!

---

## QUICK CONNECT (Minimal Version)

If you want to test JUST the servos first:

```
ESP32-C3          SERVOS
─────────         ──────
GPIO 2     ────►  Base Servo (Signal)
GPIO 3     ────►  Elbow Servo (Signal)  
GPIO 4     ────►  Gripper Servo (Signal)
5V         ────►  All Servos (VCC)
GND        ────►  All Servos (GND)
```

Add the rest (button, LEDs, IR, audio) after servos work!