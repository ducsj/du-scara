# 🎛️ COMPLETE CALIBRATION & FINE-TUNING GUIDE

## For 3-Servo SCARA Robot (Base + Elbow + Pen)

---

# 📖 UNDERSTANDING YOUR ROBOT

## How It Works:

```
         PEN TIP
            │
            │ ← PEN SERVO (lifts up/down)
            │
        ┌───┴───┐
        │ ELBOW │ ← ELBOW SERVO (bends forearm)
        └───────┘
            │
        ┌───┴───┐
        │ UPPER  │ ← BASE SERVO rotates this
        │  ARM   │
        └───────┘
            │
        ╔═════╧═════╗
        ║   BASE   ║
        ╚═════════╝
```

## 3 Servos Explained:

| Servo | GPIO | Function | Angle Meaning |
|-------|------|----------|--------------|
| **BASE** | GPIO 2 | Rotates arm left/right | 0°=left, 90°=up, 180°=right |
| **ELBOW** | GPIO 3 | Bends forearm | 0°=folded, 90°=horizontal, 180°=forward |
| **PEN** | GPIO 4 | Lifts pen up/down | PEN_UP=off paper, PEN_DOWN=on paper |

---

# 🔧 STEP 1: HARDWARE TEST

## Before calibrating, test your wiring:

### Test 1: Servo Connection Test

```
1. Connect ESP32 to computer
2. Open Serial Monitor (115200 baud)
3. You should see startup message
4. Type 'H' and press Enter

EXPECTED: All servos move to center (90°)
IF NOTHING HAPPENS: Check servo wiring (orange wire to GPIO pin)
```

### Test 2: LED Test

```
1. Power LED (GPIO 6) should be ON (solid)
2. Status LED (GPIO 7) should blink 3 times on startup

IF LEDs don't light: Check LED wiring (remember 220Ω resistor!)
```

### Test 3: IR Sensor Test

```
1. With nothing in front of sensor, watch Serial Monitor
2. Type 'T' and press Enter
3. IR value should be LOW (100-300)

EXPECTED: ~100-300 (no card)
```

---

# 🎯 STEP 2: SERVO CENTERING

## Find the Center Position:

### Enter Calibration Mode:

```
1. Hold button for 3 seconds
2. Status LED blinks 5 times rapidly
3. Serial Monitor shows "CALIBRATION MODE"
4. You can now type commands
```

### Find Base Center:

```
1. Type 'H' → Go to home position (90°, 90°)
2. Observe the arm position

QUESTION: Is the arm pointing STRAIGHT UP (12 o'clock)?

IF YES → Base center is correct (BASE_OFFSET = 0)
IF LEFT → Arm points left → BASE_OFFSET should be POSITIVE (+)
IF RIGHT → Arm points right → BASE_OFFSET should be NEGATIVE (-)
```

**Example Adjustment:**

```
If arm points LEFT at 90°:
  Type '+' repeatedly until arm points straight
  Each press = +1°
  Record final value: BASE_OFFSET = ???
```

```
If arm points RIGHT at 90°:
  Type '-' repeatedly until arm points straight
  Each press = -1°
  Record final value: BASE_OFFSET = ???
```

### Find Elbow Center:

```
1. Still at home position (90°, 90°)
2. Observe elbow position

QUESTION: Is forearm HORIZONTAL (parallel to ground)?

IF YES → Elbow center is correct (ELBOW_OFFSET = 0)
IF DOWN → Forearm points down → ELBOW_OFFSET should be POSITIVE (+)
IF UP → Forearm points up → ELBOW_OFFSET should be NEGATIVE (-)
```

**Example Adjustment:**

```
If forearm points DOWN at 90°:
  Type '.' repeatedly until forearm is horizontal
  Each press = +1°
  Record final value: ELBOW_OFFSET = ???
```

---

# 🚫 STEP 3: FIND SAFE LIMITS

## Prevent Servo Damage!

### Find Base Limits:

```
⚠️ WARNING: Going too far can BREAK the servo!

TEST LEFT LIMIT:
1. Type '2' repeatedly (decrease angle by 5° each time)
2. STOP when you hear clicking or strain
3. Your LEFT limit is: current angle + 10°
   Example: Stopped at 15° → BASE_MIN = 25

TEST RIGHT LIMIT:
1. Type '1' repeatedly (increase angle by 5° each time)
2. STOP when you hear clicking or strain
3. Your RIGHT limit is: current angle - 10°
   Example: Stopped at 165° → BASE_MAX = 155
```

### Find Elbow Limits:

```
⚠️ WARNING: Going too far can BREAK the servo!

TEST MINIMUM (folded):
1. Type '4' repeatedly (decrease angle by 5° each time)
2. STOP when you hear clicking or strain
3. Your MIN limit is: current angle + 10°
   Example: Stopped at 25° → ELBOW_MIN = 35

TEST MAXIMUM (extended):
1. Type '3' repeatedly (increase angle by 5° each time)
2. STOP when you hear clicking or strain
3. Your MAX limit is: current angle - 10°
   Example: Stopped at 155° → ELBOW_MAX = 145
```

---

# ✏️ STEP 4: PEN POSITION

## Set Pen Up and Down Positions:

### Test Pen Up Position:

```
1. Type '5' → Pen moves to PEN_UP position
2. Observe if pen is off paper
3. If pen touches paper: Type '7' (increases PEN_UP by 5°)
4. If pen is too high: Type '8' (decreases PEN_UP by 5°)
5. Repeat until pen is JUST above paper
6. Record: PEN_UP = ???
```

### Test Pen Down Position:

```
1. Type '6' → Pen moves to PEN_DOWN position
2. Observe if pen touches paper
3. If pen doesn't touch: Decrease PEN_DOWN (closer to 0)
4. If pen presses too hard: Increase PEN_DOWN (closer to 90)
5. Adjust until pen JUST touches paper
6. Record: PEN_DOWN = ???
```

---

# 📐 STEP 5: ARM LENGTHS

## Measure Your Robot:

### Measure Upper Arm (L1):

```
1. Get a ruler (mm)
2. Measure from CENTER of base servo shaft
3. To CENTER of elbow servo shaft
4. Write down the measurement

EXAMPLE: 78mm

UPDATE CODE:
#define L1_LENGTH   78
```

### Measure Forearm (L2):

```
1. Measure from CENTER of elbow servo shaft
2. To TIP of the pen holder
3. Write down the measurement

EXAMPLE: 65mm

UPDATE CODE:
#define L2_LENGTH   65
```

### Why Accuracy Matters:

```
IF LENGTH IS WRONG:
  - Robot will miss target positions
  - Drawings will be offset
  - Circles won't be round
  - Squares will be lopsided

MEASURE CAREFULLY:
  - Use mm (millimeters)
  - Measure twice to confirm
  - Include all parts (servo horns, brackets)
```

---

# 👁️ STEP 6: IR SENSOR

## Set Card Detection Threshold:

### Read IR Values:

```
1. Open Serial Monitor
2. Type 'T' repeatedly while watching values

WITHOUT card:     → Write down this number
WITH white card: → Write down this number

EXAMPLE:
  No card:    ~180
  With card: ~850
```

### Calculate Threshold:

```
FORMULA: (no-card value + with-card value) / 2

EXAMPLE:
  (180 + 850) / 2 = 515
  
UPDATE CODE:
#define IR_THRESHOLD   515
```

### Test Detection:

```
1. Place card in front of IR sensor
2. Status LED should turn ON
3. Remove card
4. Status LED should turn OFF

IF IT DOESN'T WORK:
  - Increase threshold if card not detected
  - Decrease threshold if always detected
```

---

# 📋 CALIBRATION WORKSHEET

## Write Down Your Values:

```
╔══════════════════════════════════════════════════════════════╗
║  YOUR CALIBRATION VALUES                                  ║
╠══════════════════════════════════════════════════════════════╣
║                                                          ║
║  BASE_OFFSET = _____ (arm direction adjustment)          ║
║  ELBOW_OFFSET = _____ (elbow angle adjustment)           ║
║                                                          ║
║  BASE_MIN = _____ (left limit)                          ║
║  BASE_MAX = _____ (right limit)                         ║
║  ELBOW_MIN = _____ (folded limit)                       ║
║  ELBOW_MAX = _____ (extended limit)                     ║
║                                                          ║
║  PEN_UP = _____ (pen off paper)                         ║
║  PEN_DOWN = _____ (pen on paper)                        ║
║                                                          ║
║  L1_LENGTH = _____ mm (upper arm)                       ║
║  L2_LENGTH = _____ mm (forearm)                         ║
║                                                          ║
║  IR_THRESHOLD = _____ (card detection)                   ║
║                                                          ║
╚══════════════════════════════════════════════════════════════╝
```

---

# 🔄 FINE-TUNING

## Make Small Adjustments:

### Fine-Tune Base Center:

```
PROBLEM: Arm drifts left when drawing
SOLUTION: Decrease BASE_OFFSET (try -5, -10, etc.)

PROBLEM: Arm drifts right when drawing
SOLUTION: Increase BASE_OFFSET (try +5, +10, etc.)
```

### Fine-Tune Elbow:

```
PROBLEM: Drawings are too high on paper
SOLUTION: Increase ELBOW_OFFSET (try +5, +10, etc.)

PROBLEM: Drawings are too low on paper
SOLUTION: Decrease ELBOW_OFFSET (try -5, -10, etc.)
```

### Fine-Tune Pen:

```
PROBLEM: Pen drags too hard (breaks paper)
SOLUTION: Increase PEN_DOWN slightly

PROBLEM: Pen doesn't touch paper
SOLUTION: Decrease PEN_DOWN slightly
```

---

# 🧪 TEST YOUR CALIBRATION

## Run These Tests:

### Test 1: Draw a Circle

```
EXPECTED: Perfect circle in center of paper
IF OVAL: Arm lengths not accurate (re-measure L1, L2)
IF OFFSET: Base/Elbow offsets need adjustment
```

### Test 2: Draw a Square

```
EXPECTED: Equal sides, 90° corners
IF WOBBLY: Servo limits too wide (narrow the range)
IF TILTED: Base offset needs fine-tuning
```

### Test 3: Draw Letter "A"

```
EXPECTED: Capital A in center
IF CROOKED: Elbow offset needs fine-tuning
IF WRONG SIZE: Adjust DRAW_SIZE in code
```

---

# ⚠️ TROUBLESHOOTING

## Common Problems:

### Problem: Servo jittery/moving on its own

```
CAUSE: Not enough power
SOLUTION: Use external 5V power supply (USB can't power servos!)
```

### Problem: Arm doesn't reach target

```
CAUSE: Target outside workspace
SOLUTION: Target within (L1+L2) distance from center

EXAMPLE:
  L1=80mm, L2=70mm
  Max reach = 80 + 70 = 150mm
  Can't reach (180, 0) → Too far!
```

### Problem: Drawing is always offset same direction

```
CAUSE: Base or elbow offset wrong
SOLUTION: Re-calibrate offsets
```

### Problem: IR sensor not detecting

```
CAUSE: Wrong threshold
SOLUTION: 
  1. Type 'T' to see raw IR value
  2. Calculate new threshold
  3. Update IR_THRESHOLD
```

---

# ✅ FINAL CHECKLIST

Before testing, confirm:

- [ ] BASE_OFFSET set correctly
- [ ] ELBOW_OFFSET set correctly
- [ ] BASE_MIN / BASE_MAX set safely
- [ ] ELBOW_MIN / ELBOW_MAX set safely
- [ ] PEN_UP / PEN_DOWN set correctly
- [ ] L1_LENGTH measured accurately
- [ ] L2_LENGTH measured accurately
- [ ] IR_THRESHOLD calculated correctly
- [ ] All values updated in code
- [ ] Code re-uploaded to ESP32

---

# 📝 UPDATE YOUR CODE

After calibrating, update these values in your code:

```cpp
// At the top of your sketch:

int BASE_OFFSET = ???;      // Your measured value
int ELBOW_OFFSET = ???;     // Your measured value

#define BASE_MIN   ???       // Your left limit
#define BASE_MAX   ???       // Your right limit
#define ELBOW_MIN  ???       // Your folded limit
#define ELBOW_MAX  ???       // Your extended limit

#define PEN_UP     ???       // Pen off paper
#define PEN_DOWN   ???       // Pen on paper

#define L1_LENGTH  ???       // Upper arm mm
#define L2_LENGTH  ???       // Forearm mm

#define IR_THRESHOLD ???      // Card detection
```

---

# 🎉 YOU'RE DONE!

Once calibrated, your robot will:
- ✅ Point accurately with servos
- ✅ Draw shapes in correct positions
- ✅ Detect cards reliably
- ✅ Write letters and numbers correctly
- ✅ Respond to commands smoothly

**Questions? Just ask!** 😊