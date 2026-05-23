# 🤖 ROBOTIC DRAWING ASSISTANT - CALIBRATION & TEST GUIDE

## Your Hardware Checklist

| Item | Part | Status |
|------|------|--------|
| ✓ | ESP32-C3 Super Mini | |
| ✓ | Expansion board | |
| ✓ | 3x SG90 Servo Motors | |
| ✓ | TP4056 USB-C Charger | |
| ✓ | TPS61023 5V Boost Converter | |
| ✓ | 2x 18650 Li-ion Battery (3000mAh) | |
| ✓ | MAX98357 I2S Audio Amplifier | |
| ✓ | 8Ω 1W Speaker | |
| ✓ | TCRT5000 IR Sensor | |
| ✓ | Switch Button | |
| ✓ | LED Indicators (Power + Status) | |

---

## 🔌 Wiring Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         POWER RAIL (5V)                          │
│                              │                                   │
│      ┌──────────────────────┼──────────────────────┐             │
│      │                      │                      │             │
│      ▼                      ▼                      ▼             │
│  ┌────────┐           ┌──────────┐           ┌──────────┐         │
│  │ Servo  │           │ Servo    │           │ Servo    │         │
│  │ Base   │           │ Elbow    │           │ Gripper  │         │
│  │ GPIO 2 │           │ GPIO 3   │           │ GPIO 4   │         │
│  └────────┘           └──────────┘           └──────────┘         │
│                                                                  
│  ┌──────────────────────────────────────────────────────────┐    
│  │                     ESP32-C3                             │    
│  │                                                          │    
│  │  GPIO 2 ──────────┐                                     │    
│  │  GPIO 3 ──────────┼── Servo Signals                     │    
│  │  GPIO 4 ──────────┘                                     │    
│  │  GPIO 5 ────────── Button (pulled HIGH)                  │    
│  │  GPIO 6 ────────── Power LED (via 220Ω resistor)         │    
│  │  GPIO 7 ────────── Status LED (via 220Ω resistor)        │    
│  │  GPIO 20 ───────── IR Sensor Analog Out                   │    
│  │  GPIO 21 ───────── MAX98357 LRC/WS                        │    
│  │  GPIO 22 ───────── MAX98357 BCLK                          │    
│  │                                                          │    
│  └──────────────────────────────────────────────────────────┘    
│                              │                                   │
│                              ▼                                   │
│                     ┌─────────────────┐                          │
│                     │   MAX98357     │                          │
│                     │    Amplifier   │                          │
│                     └────────┬────────┘                          │
│                              │                                   │
│                              ▼                                   │
│                         ┌─────────┐                              │
│                         │ Speaker │                              │
│                         │  8Ω 1W  │                              │
│                         └─────────┘                              │
└─────────────────────────────────────────────────────────────────┘
```

---

## 📋 CALIBRATION WORKSHEET

### Step 1: Servo Offset Calibration

Record your observations and adjustments:

```
╔════════════════════════════════════════════════════════════════╗
║  CALIBRATION RECORD - SERVO OFFSETS                            ║
╠════════════════════════════════════════════════════════════════╣
║                                                                ║
║  BASE SERVO (GPIO 2):                                          ║
║  ─────────────────────────────────────────────────────────────║
║  Default "center" position: 90°                                 ║
║  Actual behavior: [ ] Points left  [ ] Points right  [ ] OK    ║
║                                                                ║
║  If pointing LEFT, increase BASE_ZERO_OFFSET by +5, +10, etc.   ║
║  If pointing RIGHT, decrease BASE_ZERO_OFFSET by -5, -10, etc.  ║
║                                                                ║
║  Your BASE_ZERO_OFFSET value: ______                           ║
║                                                                ║
║  ELBOW SERVO (GPIO 3):                                         ║
║  ─────────────────────────────────────────────────────────────║
║  Default "center" position: 90°                                ║
║  Actual behavior: [ ] Points up  [ ] Points down  [ ] OK     ║
║                                                                ║
║  If pointing UP, decrease ELBOW_ZERO_OFFSET by -5, -10, etc.   ║
║  If pointing DOWN, increase ELBOW_ZERO_OFFSET by +5, +10, etc. ║
║                                                                ║
║  Your ELBOW_ZERO_OFFSET value: ______                          ║
║                                                                ║
╚════════════════════════════════════════════════════════════════╝
```

### Step 2: Servo Limits Calibration

**DANGER: Going beyond safe limits can damage servos!**

```
╔════════════════════════════════════════════════════════════════╗
║  CALIBRATION RECORD - SERVO LIMITS                             ║
╠════════════════════════════════════════════════════════════════╣
║                                                                ║
║  BASE SERVO LIMITS:                                            ║
║  ─────────────────────────────────────────────────────────────║
║  Test minimum: Slowly decrease from 90° to find limit          ║
║  Sounds like clicking? That's the mechanical stop!             ║
║  Minimum safe angle: ____°                                     ║
║                                                                ║
║  Test maximum: Slowly increase from 90° to find limit          ║
║  Sounds like clicking? That's the mechanical stop!             ║
║  Maximum safe angle: ____°                                    ║
║                                                                ║
║  Recommended safety margin: subtract 10° from limits           ║
║  YOUR BASE_MIN: ____°    YOUR BASE_MAX: ____°                  ║
║                                                                ║
║  ELBOW SERVO LIMITS:                                           ║
║  ─────────────────────────────────────────────────────────────║
║  Minimum safe angle: ____°                                     ║
║  Maximum safe angle: ____°                                    ║
║                                                                ║
║  YOUR ELBOW_MIN: ____°    YOUR ELBOW_MAX: ____°                ║
║                                                                ║
║  GRIPPER SERVO:                                                ║
║  ─────────────────────────────────────────────────────────────║
║  Full open angle: ____°                                        ║
║  Full close angle: ____°                                       ║
║                                                                ║
║  YOUR GRIP_OPEN_ANGLE: ____°    YOUR GRIP_CLOSE_ANGLE: ____°    ║
║                                                                ║
╚════════════════════════════════════════════════════════════════╝
```

### Step 3: IR Sensor Calibration

```
╔════════════════════════════════════════════════════════════════╗
║  CALIBRATION RECORD - IR SENSOR                                ║
╠════════════════════════════════════════════════════════════════╣
║                                                                ║
║  TEST 1: NO CARD (Background)                                 ║
║  ─────────────────────────────────────────────────────────────║
║  IR Reading: _______                                          ║
║  (Low value = dark/no reflection)                              ║
║                                                                ║
║  TEST 2: WHITE CARD                                            ║
║  ─────────────────────────────────────────────────────────────║
║  IR Reading: _______                                          ║
║  (High value = light reflection)                               ║
║                                                                ║
║  TEST 3: BLACK CARD                                           ║
║  ─────────────────────────────────────────────────────────────║
║  IR Reading: _______                                          ║
║  (Lower value = less reflection)                              ║
║                                                                ║
║  THRESHOLD CALCULATION:                                        ║
║  ─────────────────────────────────────────────────────────────║
║  Formula: (No-card value + Card value) / 2                    ║
║  Suggested threshold: _______                                  ║
║                                                                ║
║  YOUR IR_THRESHOLD: _______                                    ║
║                                                                ║
╚════════════════════════════════════════════════════════════════╝
```

### Step 4: Arm Length Measurement

```
╔════════════════════════════════════════════════════════════════╗
║  CALIBRATION RECORD - ARM LENGTHS                              ║
╠════════════════════════════════════════════════════════════════╣
║                                                                ║
║  Measure from joint center to joint center:                    ║
║                                                                ║
║  L1 (Upper Arm): _______ mm                                   ║
║  ┌─────────────────────────────────┐                           ║
║  │  Base Servo Center              │                          ║
║  │       ══════════════════════     │ ← Measure this           ║
║  │                    Elbow Center │                          ║
║  └─────────────────────────────────┘                           ║
║                                                                ║
║  L2 (Forearm): _______ mm                                     ║
║  ┌─────────────────────────────────┐                           ║
║  │  Elbow Center                   │                          ║
║  │       ══════════════════════     │ ← Measure this           ║
║  │                     Gripper     │                          ║
║  └─────────────────────────────────┘                           ║
║                                                                ║
║  YOUR L1_ARM_LENGTH: _______    YOUR L2_ARM_LENGTH: _______    ║
║                                                                ║
║  WORKSPACE CALCULATION:                                        ║
║  Max Reach = L1 + L2 = _______ mm                              ║
║  Min Reach = |L1 - L2| = _______ mm                           ║
║                                                                ║
╚════════════════════════════════════════════════════════════════╝
```

---

## 🧪 TEST PROCEDURES

### Test 1: Power On Test
1. Turn on power switch
2. Power LED should light up (solid)
3. ESP32 should boot (status LED blinks 3 times)
4. Serial output shows "System ready!"

**Expected Result:** ✓ PASS / ✗ FAIL

### Test 2: Servo Movement Test
1. Open Serial Monitor at 115200 baud
2. Type 'H' and press Enter
3. All servos should move to center position
4. Gripper should open

**Expected Result:** ✓ PASS / ✗ FAIL

### Test 3: Calibration Mode Test
1. Hold button for 3 seconds
2. Status LED should blink 5 times rapidly
3. "Calibration mode activated!" in Serial Monitor
4. Type commands:
   - '1' should increase base angle
   - '2' should decrease base angle
   - '3' should increase elbow angle
   - '4' should decrease elbow angle
   - '5' should open gripper
   - '6' should close gripper
   - 'H' should go home
   - 'Q' should quit

**Expected Result:** ✓ PASS / ✗ FAIL

### Test 4: IR Sensor Test
1. With nothing in front of sensor, watch Serial Monitor
2. IR value should be LOW (~50-200)
3. Place white card in front of sensor (2-5cm distance)
4. IR value should be HIGH (~800-1023)

**Expected Result:** ✓ PASS / ✗ FAIL

### Test 5: Card Detection Test
1. Have IR_THRESHOLD set correctly
2. Hold card in front of sensor
3. Status LED should light up
4. "Card detected!" in Serial Monitor
5. Remove card, LED should turn off

**Expected Result:** ✓ PASS / ✗ FAIL

### Test 6: Drawing Test
1. Place paper under robot
2. Attach pen to gripper
3. Trigger card detection
4. Robot should draw shapes (circle, square, star, triangle)

**Expected Result:** ✓ PASS / ✗ FAIL

---

## 📝 FINAL CALIBRATION VALUES

After completing all tests, record your final values:

```
╔════════════════════════════════════════════════════════════════╗
║  FINAL CALIBRATION VALUES                                      ║
╠════════════════════════════════════════════════════════════════╣
║                                                                ║
║  // === SERVO CALIBRATION ===                                  ║
║  int BASE_ZERO_OFFSET = _____;     // Your base offset          ║
║  int ELBOW_ZERO_OFFSET = _____;    // Your elbow offset         ║
║                                                                ║
║  // === SERVO LIMITS ===                                        ║
║  #define BASE_MIN    _____;        // Base minimum              ║
║  #define BASE_MAX    _____;        // Base maximum              ║
║  #define ELBOW_MIN   _____;        // Elbow minimum             ║
║  #define ELBOW_MAX   _____;        // Elbow maximum             ║
║                                                                ║
║  // === GRIPPER ANGLES ===                                      ║
║  #define GRIP_OPEN_ANGLE   _____;  // Gripper open             ║
║  #define GRIP_CLOSE_ANGLE   _____;  // Gripper close           ║
║                                                                ║
║  // === IR SENSOR ===                                          ║
║  #define IR_THRESHOLD       _____;  // Detection threshold      ║
║                                                                ║
║  // === ARM LENGTHS ===                                        ║
║  #define L1_ARM_LENGTH     _____;  // Upper arm (mm)            ║
║  #define L2_ARM_LENGTH     _____;  // Forearm (mm)             ║
║                                                                ║
╚════════════════════════════════════════════════════════════════╝
```

---

## 🔧 TROUBLESHOOTING GUIDE

### Problem: ESP32 not responding
- Check USB connection
- Press reset button on ESP32-C3
- Hold BOOT button while uploading

### Problem: Servos jittering
- **Cause:** Not enough power from USB
- **Fix:** Use external 5V power supply (battery via boost converter)
- Servos need more current than USB can provide!

### Problem: Servo not moving at all
- Check wiring (GND, VCC, Signal)
- Check if correct pin in code matches your wiring
- Try a different servo

### Problem: IR readings inconsistent
- Shield sensor from ambient light
- Ensure consistent card distance (2-5cm works best)
- Recalibrate threshold

### Problem: Robot arm doesn't reach target position
- Target is outside workspace (too far or too close)
- Recalculate arm lengths
- Check servo limits

### Problem: Drawing is off-center
- Adjust BASE_ZERO_OFFSET
- Adjust ELBOW_ZERO_OFFSET
- Recalculate arm lengths

### Problem: Audio not working
- Check I2S wiring (GPIO 21, 22)
- Test with simple beep first
- Check speaker connections

---

## 📁 File Locations

Your files are saved at:

```
/workspace/project/
├── DIY_Robot_Drawing_Assistant_Course.py  ← Learning course
├── Robotic_Drawing_Assistant/
│   └── Robotic_Drawing_Assistant.ino     ← Arduino sketch
├── SCARA_Robot_Course.py                  ← Original SCARA course
└── scara_visualizer.py                     ← Visual diagrams
```

---

## 📚 What's Next?

After calibration and testing:

1. **Add Audio Files** - Record voice clips for card types
2. **Add Display** - OLED for status information  
3. **Add More Cards** - Program different drawings per card
4. **Improve Gripper** - Design 3D printed pen holder
5. **Add Z-Axis** - Servo-controlled pen lift

Happy building! 🔧🤖