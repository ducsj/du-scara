# 📖 CODE EXPLAINED - Simple Version

## Your Robot with SPEECH!

---

# WHAT YOUR ROBOT DOES

```
1. Card placed in front → IR sensor detects it
2. Robot says "Drawing... Circle" ← SPEAKS!
3. Robot draws the circle on paper
4. Robot says "Done! Pick up your drawing!" ← SPEAKS!
```

---

# CODE PARTS (in order)

## PART 1: LIBRARY
```cpp
#include <ESP32Servo.h>           // Control servo motors
#include "AudioFileSourceSPIFFS.h" // Read audio files
#include "AudioGeneratorMP3.h"     // Play MP3 files
#include "AudioOutputI2S.h"       // Audio output chip
```
**What:** Loads tools for servos and audio

---

## PART 2: PINS (Where wires connect)
```cpp
#define SERVO_BASE_PIN     2    // Base servo wire
#define SERVO_ELBOW_PIN    3    // Elbow servo wire
#define SERVO_GRIP_PIN     4    // Gripper servo wire
#define BUTTON_PIN         5    // Button wire
#define LED_POWER_PIN      6    // Power LED wire
#define LED_STATUS_PIN     7    // Status LED wire
#define IR_SENSOR_PIN     20    // IR sensor wire

// AUDIO PINS:
#define AUDIO_LRC_PIN     21    // Audio chip - Left/Right clock
#define AUDIO_BCLK_PIN    22    // Audio chip - Bit clock
#define AUDIO_DIN_PIN     18    // Audio chip - Data in
```
**What:** Names for each wire connection

---

## PART 3: CALIBRATION (Tune these!)
```cpp
int BASE_ZERO_OFFSET   = 0;     // If arm points wrong way
int ELBOW_ZERO_OFFSET = 0;     // If elbow points wrong way
#define IR_THRESHOLD     500     // When IR detects card
#define GRIP_OPEN_ANGLE  90     // Gripper open
#define GRIP_CLOSE_ANGLE  0     // Gripper closed
```
**What:** Values you adjust after testing

---

## PART 4: ARM SIZE (Measure your robot!)
```cpp
#define L1_ARM_LENGTH   80    // Upper arm mm
#define L2_ARM_LENGTH   70    // Forearm mm
#define BASE_MIN   20         // Don't go below 20°
#define BASE_MAX  160         // Don't go above 160°
#define ELBOW_MIN  30         // Don't go below 30°
#define ELBOW_MAX 150         // Don't go above 150°
```
**What:** Your robot's physical measurements

---

## PART 5: SERVO OBJECTS (Names)
```cpp
Servo baseServo;      // "This is the base servo"
Servo elbowServo;     // "This is the elbow servo"
Servo gripServo;      // "This is the gripper servo"
```
**What:** Name each motor so we can control it

---

## PART 6: AUDIO/SPEECH SYSTEM 🎤

### THIS IS YOUR SPEAKER SYSTEM!

```cpp
AudioFileSourceSPIFFS *audioFile;  // Reads MP3 files
AudioGeneratorMP3 *audioMP3;      // Decodes MP3
AudioOutputI2S *audioOut;         // Sends to speaker
```

### AUDIO FILES YOU NEED:

```
/audio/
├── A.mp3 ... Z.mp3          ← Say letters
├── 0.mp3 ... 9.mp3          ← Say numbers
├── circle.mp3                ← "Circle"
├── square.mp3                ← "Square"
├── triangle.mp3               ← "Triangle"
├── star.mp3                  ← "Star"
├── heart.mp3                 ← "Heart"
├── drawing.mp3               ← "Drawing..."
├── done.mp3                  ← "Done!"
└── pickup.mp3               ← "Pick up your drawing"
```

### SPEECH FUNCTIONS:

```cpp
void sayLetter('A')           // Says "A"
void sayNumber(5)             // Says "Five"
void spellWord("HELLO")       // Spell H-E-L-L-O
void playAudioFile("/audio/circle.mp3")  // Play "Circle"
```

**How to create audio files:**
1. Go to translate.google.com
2. Type "circle"
3. Click speaker 🔊
4. Right-click → Save audio
5. Name: circle.mp3

---

## PART 7: MOVEMENT

```cpp
void moveArm(baseAngle, elbowAngle)  // Move both servos
void goHome()                        // Return to start position
```

**Example:**
```cpp
moveArm(90, 60);   // Base 90°, Elbow 60°
```

---

## PART 8: MATH (Inverse Kinematics)

```cpp
bool moveToPosition(x, y)  // Move to X,Y coordinate
```

**Example:**
```cpp
moveToPosition(50, 50);  // Move tip to position (50mm, 50mm)
```

---

## PART 9: IR SENSOR

```cpp
int readIRSensor()          // Read IR value (0-4095)
bool isCardDetected()       // True if card present
```

**How it works:**
- IR sensor emits light
- Light bounces off card
- White card = HIGH value (800+)
- No card = LOW value (100-)

---

## PART 10: LEDs

```cpp
void setPowerLED(true)      // Power LED on/off
void setStatusLED(true)    // Status LED on/off
void blinkStatusLED(3, 200) // Blink 3 times, 200ms each
```

---

## PART 11: DRAWING SHAPES

```cpp
void drawCircle(cx, cy, radius)  // Draw circle
void drawSquare(cx, cy, size)     // Draw square
void drawTriangle(cx, cy, size)   // Draw triangle
void drawStar(cx, cy, size)       // Draw star
void drawHeart(cx, cy, size)     // Draw heart
```

---

## PART 12: CALIBRATION MODE

**Enter:** Hold button 3 seconds

**Commands:**
| Key | What it does |
|-----|--------------|
| `1` | Base +5° |
| `2` | Base -5° |
| `3` | Elbow +5° |
| `4` | Elbow -5° |
| `+` | Base offset +1 |
| `-` | Base offset -1 |
| `.` | Elbow offset +1 |
| `,` | Elbow offset -1 |
| `Q` | Quit |

---

## PART 13: MAIN PROGRAM (What robot does!)

```cpp
void setup() {
    // ONCE when power on:
    initAudio();      // Start speaker
    goHome();         // Move to start
    playBeep(523);    // Startup sound
}

void loop() {
    // REPEATS forever:
    
    if (cardDetected) {
        // 1. Say what it will draw
        playAudioFile("/audio/drawing.mp3");
        playAudioFile("/audio/circle.mp3");
        
        // 2. Draw the shape
        drawCircle(50, 50, 30);
        
        // 3. Say done
        playAudioFile("/audio/done.mp3");
        playAudioFile("/audio/pickup.mp3");
    }
}
```

---

# WHAT HAPPENS STEP BY STEP

```
POWER ON
    ↓
Setup runs once
    ↓
Startup jingle plays 🎵
    ↓
Go to home position
    ↓
[LOOP STARTS]
    ↓
Check IR sensor
    ↓
┌─────────────────────────────────────┐
│                                     │
│  NO CARD          CARD DETECTED     │
│    ↓                    ↓          │
│  Wait...         Say "Drawing..."    │
│  (LED blinks)    Say "Circle"      │
│                  Draw circle        │
│                  Say "Done!"        │
│                  Say "Pick up!"     │
│                  Go home            │
│                                     │
└─────────────────────────────────────┘
    ↓
[LOOP REPEATS]
```

---

# FILES YOU NEED

| File | Where | What |
|------|-------|------|
| `Robotic_Drawing_Assistant.ino` | Arduino IDE | Main code |
| `data/audio/*.mp3` | ESP32 memory | Audio files |

---

# QUICK REFERENCE

**Say a letter:** `sayLetter('A');`
**Say a number:** `sayNumber(5);`
**Spell a word:** `spellWord("HELLO");`
**Play audio:** `playAudioFile("/audio/circle.mp3");`

**Draw a shape:** `drawCircle(50, 50, 30);`

**Move arm:** `moveArm(90, 60);`

**Go home:** `goHome();`

---

# YOU'RE READY!

1. Upload code to ESP32
2. Create audio files
3. Upload audio to ESP32 SPIFFS
4. Test!

The robot will speak and draw! 🗣️🤖🎨