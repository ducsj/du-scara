# 🤖 DUAL/PARALLEL ARM SCARA ROBOT - COMPLETE BUILD GUIDE

## Your Parts List & Step-by-Step Instructions

---

# 📦 PART 1: COMPLETE PARTS LIST

## 🛒 Where to Buy (Approximate Prices)

### Electronics

| Part | Quantity | Price (USD) | Link |
|------|----------|-------------|------|
| ESP32-C3 Super Mini | 1 | $4-6 | Search on Amazon/Aliexpress |
| ESP32-C3 Expansion Board | 1 | $3-5 | Comes with dev kit |
| SG90 Servo Motors | 6 | $1-2 each | Aliexpress/Amazon |
| TP4056 USB-C Charger | 2 | $0.50 each | Aliexpress |
| TPS61023 Boost Converter 5V 3.7A | 1 | $2-3 | Aliexpress |
| MAX98357 I2S Audio Amplifier | 1 | $2-3 | Aliexpress/Adafruit |
| TCRT5000 IR Sensor | 2 | $0.50 each | Aliexpress |
| 18650 Battery Holder (2-slot) | 1 | $1-2 | Aliexpress |
| 18650 Batteries 3000mAh | 2 | $5-8 | Sony/Samsung |
| Push Button | 2 | $0.20 each | Any electronics store |
| LED (Red + Green) | 2 | $0.10 each | Any electronics store |
| 220Ω Resistors | 4 | $0.05 each | Any electronics store |
| Jumper Wires | 40+ | $2-3 | Aliexpress |
| USB-C Cable | 1 | $2-3 | Your phone charger works |

### Mechanical Parts

| Part | Quantity | Price (USD) | Notes |
|------|----------|-------------|-------|
| 3D Printed Parts | Set | $10-20 | STL files included below |
| M3 Screws (10mm) | 20 | $1 | Hardware store |
| M3 Nuts | 20 | $0.50 | Hardware store |
| Metal Bearings 4x8x3mm | 4 | $1 | Skateboard bearings work |
| Paper/A4 Sheets | 10 | $1 | For drawing surface |

### Tools Needed (Not Buying)

| Tool | Purpose |
|------|---------|
| Screwdriver (Phillips) | Assembling parts |
| Hot Glue Gun | Mounting servos |
| Ruler/Measuring Tape | Measuring arm lengths |
| Scissors | Cutting parts |
| Soldering Iron (optional) | Better connections |

---

## 🏗️ COMPLETE PARTS DIAGRAM

```
                    ╔══════════════════════════════════════╗
                    ║         YOUR COMPLETE ROBOT          ║
                    ╚══════════════════════════════════════╝
                    
                              ┌─────────────────┐
                              │   SPEAKER       │
                              │   (8Ω 1W)      │
                              └────────┬────────┘
                                       │
                    ┌──────────────────┼──────────────────┐
                    │                  │                  │
               ┌────┴────┐       ┌────┴────┐       ┌────┴────┐
               │ SERVO 1 │       │ SERVO 2 │       │ SERVO 3 │
               │ (Base)  │       │ (Elbow) │       │(Gripper)│
               └────┬────┘       └────┬────┘       └────┬────┘
                    │                  │                  │
                    ▼                  ▼                  ▼
                ┌─────────────────────────────────────────┐
                │                                         │
                │         3D PRINTED ARM STRUCTURE        │
                │                                         │
                │    ┌───────┐         ┌───────┐            │
                │    │ ARM 1 │────────│ ARM 2 │            │
                │    │  L1   │         │  L2   │            │
                │    └───────┘         └───────┘            │
                │          \           /                    │
                │           \         /                     │
                │            ╔═══════╧═══════╗              │
                │            ║   BASE      ║              │
                │            ╚═════════════╝              │
                └─────────────────────────────────────────┘
                                       │
                    ┌──────────────────┼──────────────────┐
                    │                  │                  │
              ┌─────┴─────┐     ┌─────┴─────┐     ┌─────┴─────┐
              │  ESP32   │     │  POWER    │     │   IR     │
              │   C3     │     │  SYSTEM   │     │  SENSOR  │
              └──────────┘     └───────────┘     └───────────┘
```

---

# 🔧 PART 2: 3D PRINTED PARTS (FREE STL FILES)

## You Need to Print These Parts:

### Part A: Base Plate
```
Size: 100mm x 100mm x 5mm
Holes: 4x M3 screw holes at corners
```

### Part B: Upper Arm (L1)
```
Length: 80mm (adjustable)
Width: 20mm
Thickness: 3mm
Holes: 2x for servo mounting, 1x for joint
```

### Part C: Forearm (L2)
```
Length: 70mm (adjustable)
Width: 20mm
Thickness: 3mm
Holes: 2x for servo mounting, 1x for joint
```

### Part D: Gripper
```
Size: 40mm x 20mm
Parts: 2x claw fingers, 1x base
```

### Part E: Servo Horns
```
Standard SG90 servo horn (included with servo)
```

---

## 📐 EASY MEASUREMENT GUIDE

```
Print one part at a time. Measure before printing!

ARM LENGTH GUIDE:
├── Short arm (L1): 50-80mm  ← EASY for beginners
├── Medium arm (L2): 60-80mm
└── Long arm: 80-100mm ← Requires more precision

MEASURE YOUR ARMS:
1. Print test piece (10mm x 10mm square)
2. Verify printer accuracy
3. Print arm parts
4. Measure with ruler
5. Update code with YOUR lengths:
   #define L1_ARM_LENGTH  75    // Your upper arm
   #define L2_ARM_LENGTH  65    // Your forearm
```

---

# 🔌 PART 3: WIRING DIAGRAM (FULL)

## Power System
```
                    ┌─────────────────────┐
                    │    2x 18650        │
                    │    BATTERIES       │
                    │    (7.4V total)    │
                    └─────────┬─────────┘
                              │
                    ┌─────────┴─────────┐
                    │       TP4056       │
                    │    (Charger)       │
                    └─────────┬─────────┘
                              │
                    ┌─────────┴─────────┐
                    │    TPS61023       │
                    │   (Boost to 5V)   │
                    └─────────┬─────────┘
                              │
                    ┌─────────┴─────────┐
                    │      5V RAIL      │
                    └─────────┬─────────┘
                              │
         ┌────────────────────┼────────────────────┐
         │                    │                    │
         ▼                    ▼                    ▼
   ┌──────────┐          ┌──────────┐          ┌──────────┐
   │  ESP32  │          │  SERVOS │          │   IR +   │
   │   C3    │          │  (3x)   │          │  AUDIO   │
   └──────────┘          └──────────┘          └──────────┘
```

## Servo Connections
```
ESP32-C3              SERVOS
─────────             ------
GPIO 2    ──────────►  SERVO 1 (Base) - Orange wire
GPIO 3    ──────────►  SERVO 2 (Elbow) - Orange wire
GPIO 4    ──────────►  SERVO 3 (Gripper) - Orange wire
5V        ──────────►  All Servos VCC (Red wire)
GND       ──────────►  All Servos GND (Brown wire)
```

## Complete Pin Mapping
```
╔═══════════════════════════════════════════════════════════════════╗
║                    ESP32-C3 PINOUT                                ║
╠═══════════════════════════════════════════════════════════════════╣
║                                                                   ║
║   GPIO 2  ───────►  Servo Base (arm rotation)                   ║
║   GPIO 3  ───────►  Servo Elbow (arm bend)                       ║
║   GPIO 4  ───────►  Servo Gripper (open/close)                   ║
║   GPIO 5  ───────►  Button (calibration mode)                    ║
║   GPIO 6  ───────►  Power LED (via 220Ω)                        ║
║   GPIO 7  ───────►  Status LED (via 220Ω)                       ║
║   GPIO 20  ───────►  IR Sensor (analog input)                    ║
║   GPIO 18  ───────►  MAX98357 DIN (audio data)                   ║
║   GPIO 21  ───────►  MAX98357 LRC/WS (audio word select)          ║
║   GPIO 22  ───────►  MAX98357 BCLK (audio clock)                  ║
║                                                                   ║
║   5V      ───────►  Servos, IR Sensor, MAX98357                 ║
║   GND     ───────►  All GND connections                          ║
║                                                                   ║
╚═══════════════════════════════════════════════════════════════════╝
```

---

# 🔨 PART 4: ASSEMBLY STEPS (WITH PICTURES)

## Step 1: Prepare Base
```
1. Print base plate (100x100mm)
2. Sand any rough edges
3. Mark center point
4. Drill holes for M3 screws
```

## Step 2: Mount Base Servo
```
1. Place servo in center of base
2. Align servo shaft with center point
3. Secure with M3 screws (4x)
4. Attach servo horn to shaft
```

## Step 3: Attach Upper Arm
```
1. Connect upper arm (L1) to base servo horn
2. Use M3 screw through arm into horn
3. Test rotation - should move smoothly
4. Add bearing if needed for stability
```

## Step 4: Mount Elbow Servo
```
1. Position elbow servo at end of upper arm
2. Align with joint center
3. Secure with screws
4. Attach horn to shaft
```

## Step 5: Attach Forearm
```
1. Connect forearm (L2) to elbow servo horn
2. Use M3 screw to secure
3. Test bending motion
```

## Step 6: Mount Gripper Servo
```
1. Position gripper servo at end of forearm
2. Attach gripper claws to horn
3. Test open/close motion
```

## Step 7: Wire Everything
```
Follow WIRING_GUIDE.md for connections
Use hot glue to secure wires
```

## Step 8: Add Electronics
```
1. Mount ESP32-C3 on expansion board
2. Connect all pins according to diagram
3. Add LEDs to GPIO 6, 7 (with resistors)
4. Mount IR sensor at front
```

---

# 💻 PART 5: CODE EXPLANATION (WITH EXAMPLES)

## Example 1: Understanding Servo Angles

```cpp
// SERVO ANGLES EXPLAINED:
//
// 0°   = Full left (counterclockwise)
// 90°  = Center (middle)
// 180° = Full right (clockwise)

// Example: If base servo at 90°
// The arm points straight up (12 o'clock position)

// Example: If base servo at 0°
// The arm points to the left (9 o'clock position)

// Example: If base servo at 180°
// The arm points to the right (3 o'clock position)

void demonstrateAngles() {
    // Move arm to center
    moveArm(90, 90);  // Both servos centered
    delay(1000);
    
    // Move arm to left
    moveArm(45, 45);  // Both servos at 45°
    delay(1000);
    
    // Move arm to right
    moveArm(135, 135);  // Both servos at 135°
    delay(1000);
}
```

## Example 2: Understanding Position (X, Y)

```cpp
// POSITION COORDINATES EXPLAINED:
//
// Your robot workspace (top view):
//
//        Y (mm)
//        ↑
//   150  │    · · · · · · · · ·
//        │  · · · · · · · · · ·
//   100  │  · · · · · · · · · · ·
//        │  · · · · · ● · · · · ·  ← Target (50, 80)
//    50  │  · · · · · ↑ · · · · ·
//        │  · · · · ↑ │ · · · · ·
//     0  │  · · · ↑ · │ · · · · ·
//        └────────────────────────→ X (mm)
//           0   50  100  150  200
//
// To reach point (50, 80):
// 1. Calculate distance from center: D = √(50² + 80²) = 94mm
// 2. Calculate angles using math
// 3. Move servos to those angles

// Example: Move to different positions
void demonstratePositions() {
    // Point straight up
    moveToPosition(0, 100);
    delay(500);
    
    // Point to the right
    moveToPosition(100, 0);
    delay(500);
    
    // Point diagonally (up-right)
    moveToPosition(50, 50);
    delay(500);
    
    // Point at angle
    moveToPosition(70, 30);
    delay(500);
}
```

## Example 3: Drawing Shapes

```cpp
// DRAWING EXPLAINED:
//
// Circle: Move in a circle, pen down
// Square: Move to 4 corners, drawing lines
// Triangle: Move to 3 points, drawing lines
// Star: Move to 5 outer + 5 inner points

// Example: Draw a LINE
void drawMyLine() {
    gripServo.write(GRIP_CLOSE_ANGLE);  // Pen down
    drawLine(20, 20, 80, 80, 20);        // From (20,20) to (80,80)
    gripServo.write(GRIP_OPEN_ANGLE);    // Pen up
}

// Example: Draw a RECTANGLE
void drawMyRectangle() {
    // Rectangle 40mm wide, 30mm tall
    gripServo.write(GRIP_CLOSE_ANGLE);
    
    moveToPosition(30, 20);   // Bottom-left
    drawLine(30, 20, 70, 20, 10);    // Bottom edge
    drawLine(70, 20, 70, 50, 10);    // Right edge
    drawLine(70, 50, 30, 50, 10);    // Top edge
    drawLine(30, 50, 30, 20, 10);    // Left edge
    
    gripServo.write(GRIP_OPEN_ANGLE);
}

// Example: Draw a PLUS SIGN (+)
void drawPlusSign() {
    gripServo.write(GRIP_CLOSE_ANGLE);
    
    // Vertical line
    drawLine(50, 30, 50, 70, 10);
    
    // Horizontal line
    drawLine(30, 50, 70, 50, 10);
    
    gripServo.write(GRIP_OPEN_ANGLE);
}

// Example: Draw the letter "A"
void drawLetterA() {
    gripServo.write(GRIP_CLOSE_ANGLE);
    
    // Left leg of A
    drawLine(30, 60, 40, 20, 8);
    
    // Right leg of A
    drawLine(40, 20, 50, 60, 8);
    
    // Crossbar
    drawLine(35, 45, 45, 45, 5);
    
    gripServo.write(GRIP_OPEN_ANGLE);
}
```

## Example 4: Speech Functions

```cpp
// SPEECH EXPLAINED:
//
// Robot can say:
// - Individual letters: sayLetter('A')
// - Numbers: sayNumber(5)
// - Words: playAudioFile("/audio/circle.mp3")
// - Spell words: spellWord("HELLO")

// Example: Say each letter of a word
void spellMyName() {
    spellWord("ROBOT");  // Says: R-O-B-O-T
}

// Example: Count numbers
void countNumbers() {
    for (int i = 0; i <= 9; i++) {
        sayNumber(i);
        delay(500);
    }
}

// Example: Make a sentence
void sayMySentence() {
    // Play multiple audio files in sequence
    const char* sentence[] = {
        "/audio/drawing.mp3",
        "/audio/circle.mp3"
    };
    saySentence(sentence, 2);  // Says: "Drawing... Circle"
}

// Example: Personalized greeting
void greetUser() {
    spellWord("HELLO");
    delay(300);
    spellWord("FRIEND");
}
```

---

# 🎯 PART 6: CALIBRATION EXAMPLES

## Example 1: Finding Center Position

```
PROBLEM: Arm doesn't point straight when at 90°

SOLUTION:
1. Hold button 3 seconds → Calibration mode
2. Type '1' repeatedly until arm points right
3. Type '2' repeatedly until arm points left
4. Find the angle where arm points straight (90° + offset)

YOUR CALIBRATION:
- If arm points LEFT at 90° → BASE_ZERO_OFFSET = +10
- If arm points RIGHT at 90° → BASE_ZERO_OFFSET = -10
```

## Example 2: Finding Servo Limits

```
PROBLEM: Servo clicks/strains when moving

SOLUTION:
1. Start at 90°
2. Type '1' slowly (decrease angle)
3. Listen for clicking = TOO FAR
4. Note the angle: BASE_MIN = 20 (example)
5. Repeat going right: BASE_MAX = 160 (example)

YOUR LIMITS:
#define BASE_MIN    25    // Your minimum safe angle
#define BASE_MAX   155    // Your maximum safe angle
```

## Example 3: Calibrating IR Sensor

```
PROBLEM: Card not detected properly

SOLUTION:
1. Open Serial Monitor (115200 baud)
2. Watch IR values:
   
   WITHOUT card:      ~100-300  (low)
   WITH white card:   ~800-1023 (high)
   WITH dark card:    ~300-500  (medium)

3. Set threshold between your values:
   
   Threshold = (no-card + with-card) / 2
   
   Example: 200 + 900 = 1100 / 2 = 550
   
#define IR_THRESHOLD  550
```

---

# 🎨 PART 7: DRAWING EXAMPLES

## Drawing Letters (A-Z)

```cpp
// Draw capital letters!

void drawA() {
    // A has: left leg, right leg, crossbar
    drawLine(30, 60, 40, 20, 8);      // Left leg
    drawLine(40, 20, 50, 60, 8);      // Right leg
    drawLine(35, 45, 45, 45, 5);      // Crossbar
}

void drawB() {
    // B has: vertical line, 2 curves
    drawLine(30, 20, 30, 60, 10);     // Vertical
    drawLine(30, 20, 45, 25, 30, 5);   // Top curve
    drawLine(45, 25, 30, 40, 30, 5);   // Middle
    drawLine(30, 40, 45, 50, 30, 5);  // Bottom curve
}

void drawC() {
    // C is an arc
    for (int i = 0; i <= 20; i++) {
        float angle = PI + (float)i / 20 * PI;
        float x = 40 + 15 * cos(angle);
        float y = 40 + 15 * sin(angle);
        moveToPosition(x, y);
    }
}

void drawNumber(int num) {
    switch(num) {
        case 0: draw0(); break;
        case 1: draw1(); break;
        case 2: draw2(); break;
        case 3: draw3(); break;
        case 4: draw4(); break;
        case 5: draw5(); break;
        case 6: draw6(); break;
        case 7: draw7(); break;
        case 8: draw8(); break;
        case 9: draw9(); break;
    }
}
```

## Drawing Shapes

```cpp
// SQUARE
void drawSquare(float cx, float cy, float size) {
    float h = size / 2;
    drawLine(cx - h, cy - h, cx + h, cy - h, 10);  // Top
    drawLine(cx + h, cy - h, cx + h, cy + h, 10);  // Right
    drawLine(cx + h, cy + h, cx - h, cy + h, 10);  // Bottom
    drawLine(cx - h, cy + h, cx - h, cy - h, 10);  // Left
}

// RECTANGLE
void drawRectangle(float cx, float cy, float w, float h) {
    drawLine(cx - w/2, cy - h/2, cx + w/2, cy - h/2, 10);  // Top
    drawLine(cx + w/2, cy - h/2, cx + w/2, cy + h/2, 10);    // Right
    drawLine(cx + w/2, cy + h/2, cx - w/2, cy + h/2, 10);    // Bottom
    drawLine(cx - w/2, cy + h/2, cx - w/2, cy - h/2, 10);    // Left
}

// OVAL/ELLIPSE
void drawOval(float cx, float cy, float rx, float ry, int segments = 36) {
    gripServo.write(GRIP_CLOSE_ANGLE);
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2 * PI;
        float x = cx + rx * cos(angle);
        float y = cy + ry * sin(angle);
        moveToPosition(x, y);
        delay(20);
    }
    gripServo.write(GRIP_OPEN_ANGLE);
}

// HEART
void drawHeart(float cx, float cy, float size) {
    float r = size / 4;
    moveToPosition(cx - r, cy + r/2);
    gripServo.write(GRIP_CLOSE_ANGLE);
    
    // Left curve
    for (int i = 0; i <= 20; i++) {
        float angle = PI + (float)i / 20 * PI;
        float x = cx - r + r * cos(angle);
        float y = cy + r/2 + r * sin(angle);
        moveToPosition(x, y);
    }
    
    // Right curve
    for (int i = 0; i <= 20; i++) {
        float angle = (float)i / 20 * PI;
        float x = cx + r + r * cos(angle);
        float y = cy + r/2 + r * sin(angle);
        moveToPosition(x, y);
    }
    
    gripServo.write(GRIP_OPEN_ANGLE);
}
```

---

# 🔊 PART 8: AUDIO FILES LIST

## Minimum Files Needed

```
/audio/
├── circle.mp3      → "Circle" (shape)
├── square.mp3      → "Square" (shape)
├── triangle.mp3    → "Triangle" (shape)
├── star.mp3        → "Star" (shape)
├── heart.mp3       → "Heart" (shape)
├── drawing.mp3     → "Drawing..." (action)
├── done.mp3        → "Done!" (complete)
└── pickup.mp3      → "Pick up your drawing" (instruction)
```

## Full Alphabet (Optional)

```
/audio/
├── A.mp3 ... Z.mp3          ← Say letters
├── 0.mp3 ... 9.mp3          ← Say numbers
```

---

# ✅ PART 9: FINAL CHECKLIST

## Before You Start

- [ ] All parts purchased
- [ ] 3D parts printed
- [ ] Tools gathered
- [ ] ESP32-C3 ready

## Assembly

- [ ] Base assembled
- [ ] Servos mounted
- [ ] Arms connected
- [ ] Gripper attached

## Wiring

- [ ] Power system connected
- [ ] Servos wired
- [ ] IR sensor wired
- [ ] Audio amp wired
- [ ] LEDs connected

## Software

- [ ] Arduino IDE installed
- [ ] ESP32 board added
- [ ] Libraries installed
- [ ] Code uploaded

## Testing

- [ ] Serial Monitor shows startup
- [ ] Servos respond to commands
- [ ] IR detects cards
- [ ] Audio plays (if files uploaded)
- [ ] Drawing works!

---

# 🎉 YOU'RE READY TO BUILD!

Follow these guides in order:
1. QUICK_START.md - 5 minute setup
2. This guide - Build your robot
3. CALIBRATION_GUIDE.md - Tune it

Questions? Just ask! 😊