#!/usr/bin/env python3
"""
================================================================
DIY ROBOTIC DRAWING ASSISTANT - COMPLETE COURSE
================================================================

Hardware You Have:
- ESP32-C3 Super Mini
- Expansion board for ESP32-C3
- 3x SG90 Servo Motors (2 arms + gripper)
- TP4056 USB-C LiPo charger
- TPS61023 5V 3.7A boost converter
- 2x 18650 Li-ion batteries (3.7V, 3000mAh)
- MAX98357 I2S 3W audio amplifier
- 8 ohm 1W speaker
- TCRT5000 IR reflective sensor (for card detection)
- Switch button
- LED indicators

This project: Card Reader → Robot Arm → Drawing + Voice Output

================================================================
LESSON 1: UNDERSTANDING YOUR HARDWARE
================================================================
"""

# ---------------------------------------------------------------
# PART 1.1: SERVO MOTORS (SG90)
# ---------------------------------------------------------------
SG90_INFO = """
=== SG90 SERVO MOTOR ===

Specs:
- Operating voltage: 4.8V - 6V
- Torque: 1.8 kg-cm (at 4.8V)
- Speed: 0.12 sec/60° (at 4.8V)
- Weight: 9g
- Rotation: 180° (limited by internal stops)

Wiring:
- Brown wire: GND
- Red wire: VCC (4.8-6V)
- Orange wire: Signal (PWM)

Control Signal (for ESP32):
- PWM frequency: 50Hz (20ms period)
- Pulse width: 500μs to 2500μs
  - 500μs = 0° (full left)
  - 1500μs = 90° (center)
  - 2500μs = 180° (full right)

For ESP32-C3:
  Servo Pin → GPIO any (we'll use GPIO 2, 3, 4)
  VCC → 5V (from boost converter)
  GND → GND
"""

# ---------------------------------------------------------------
# PART 1.2: ESP32-C3 PINOUT
# ---------------------------------------------------------------
ESP32_PINS = """
=== ESP32-C3 Super Mini Pinout ===

        ┌─────────────────┐
    5V  │  ○  ○  ○  ○  ○  │
   GND  │  ○  ○  ○  ○  ○  │
   3V3  │  ○  ○  ○  ○  ○  │
        └─────────────────┘
            │
         [ESP32C3]
            
GPIO Mapping:
- GPIO 0-1: Reserved (boot/debug)
- GPIO 2: Servo 1 (Base/Shoulder)
- GPIO 3: Servo 2 (Elbow)
- GPIO 4: Servo 3 (Gripper)
- GPIO 5: Button input
- GPIO 6: LED indicator
- GPIO 7-10: I2C/SPI (available)
- GPIO 20: IR sensor analog input
- GPIO 21-22: I2S audio (MAX98357)

PWM Capable GPIOs: 0-7 (8 channels)
"""

# ---------------------------------------------------------------
# PART 1.3: POWER SYSTEM
# ---------------------------------------------------------------
POWER_INFO = """
=== POWER SYSTEM DESIGN ===

Battery: 2x 18650 Li-ion in PARALLEL
- Nominal voltage: 3.7V
- Capacity: 3000mAh x 2 = 6000mAh
- Parallel = 3.7V, 6000mAh

TP4056 Charger:
- Input: 5V USB-C
- Output: 4.2V charging voltage
- Handles CC/CV charging automatically
- Red LED = charging, Blue LED = complete

TPS61023 Boost Converter:
- Input: 3.0V - 5.5V
- Output: 5V @ 3.7A (18.5W)
- Powers: ESP32, Servos, Audio

Power Budget:
- ESP32-C3: ~100mA
- 3x SG90: ~240mA (80mA each at load)
- MAX98357: ~500mA (max, at 3W)
- Buffer: ~1A total
- Battery life: ~6 hours continuous

WARNING: Don't power servos from ESP32 directly!
         Always use external 5V supply!
"""

# ---------------------------------------------------------------
# PART 1.4: IR SENSOR (TCRT5000)
# ---------------------------------------------------------------
IR_SENSOR = """
=== TCRT5000 IR REFLECTIVE SENSOR ===

Purpose: Detect drawing cards placed in front

How it works:
- Emits IR light
- Measures reflected light
- Dark surface = less reflection = lower reading
- Light surface = more reflection = higher reading

Wiring:
- VCC → 3.3V or 5V
- GND → GND
- AO (Analog Out) → ESP32 ADC pin (GPIO 20)

Calibration Values:
- Card surface: ~800-1023 (light detected)
- No card: ~100-300 (dark/nothing)
- Black line: ~50-150 (very dark)

You can detect:
- Card presence (is something there?)
- Different colors (grayscale values)
- Simple patterns/QR codes
"""

# ---------------------------------------------------------------
# PART 1.5: AUDIO SYSTEM (MAX98357)
# ---------------------------------------------------------------
AUDIO_INFO = """
=== MAX98357 I2S AUDIO AMPLIFIER ===

Purpose: Speak card names/drawing instructions

Connection to ESP32:
- LRCK (WS): GPIO 21
- BCLK: GPIO 22  
- DIN (DATA): Connect to WS or separate

Library: ESP8266Audio or ESP32-audioI2S

Audio Format: MP3 or WAV files

Tip: Pre-record audio files:
- "circle.wav" - for circle drawing
- "star.wav" - for star drawing
- "triangle.wav" - for triangle drawing
- "hello.wav" - greeting

Store on SPIFFS or SD card!
"""


def lesson1_hardware_overview():
    """Lesson 1: Learn your components"""
    print(SG90_INFO)
    print("\n" + "="*60)
    print("ESP32-C3 PINOUT")
    print("="*60)
    print(ESP32_PINS)
    print("\n" + "="*60)
    print("POWER SYSTEM")
    print("="*60)
    print(POWER_INFO)
    print("\n" + "="*60)
    print("IR SENSOR")
    print("="*60)
    print(IR_SENSOR)
    print("\n" + "="*60)
    print("AUDIO SYSTEM")
    print("="*60)
    print(AUDIO_INFO)


"""
================================================================
LESSON 2: ARDUINO CODE FOUNDATION
================================================================

The actual code for ESP32-C3 development!
"""

ARDUINO_TEMPLATE = '''
// ================================================================
// ROBOTIC DRAWING ASSISTANT - ESP32-C3 CODE
// ================================================================

#include <ESP32Servo.h>
#include <Wire.h>

// ================================================================
// PIN DEFINITIONS
// ================================================================
#define SERVO_BASE_PIN    2    // Shoulder/Base servo
#define SERVO_ELBOW_PIN   3    // Elbow servo  
#define SERVO_GRIP_PIN    4    // Gripper servo
#define BUTTON_PIN        5    // Input button
#define LED_POWER_PIN     6    // Power LED
#define LED_STATUS_PIN    7    // Status LED
#define IR_SENSOR_PIN     20   // IR sensor analog
#define IR_DIGITAL_PIN    21   // IR sensor digital (optional)

// ================================================================
// SERVO CONFIGURATION
// ================================================================
Servo baseServo;    // Shoulder rotation (0-180°)
Servo elbowServo;   // Elbow bend (0-180°)
Servo gripServo;    // Gripper open/close (0-90°)

// Servo calibration offsets (tune these!)
int BASE_ZERO_OFFSET = 0;     // Adjust if base isn't centered
int ELBOW_ZERO_OFFSET = 0;     // Adjust if elbow isn't centered
int GRIP_OPEN_ANGLE = 90;      // Gripper open position
int GRIP_CLOSE_ANGLE = 0;      // Gripper closed position

// Servo limits (safe range for your robot)
#define BASE_MIN    10
#define BASE_MAX    170
#define ELBOW_MIN   20
#define ELBOW_MAX   160
#define GRIP_MIN    0
#define GRIP_MAX    90

// Current servo positions
int currentBase = 90;
int currentElbow = 90;
int currentGrip = GRIP_OPEN_ANGLE;

// ================================================================
// SERVO ANGLE TO PULSEWIDTH CONVERSION
// ================================================================
// SG90: 500μs to 2500μs for 0° to 180°
// Formula: pulse = 500 + (angle * 2000 / 180)

int angleToPulse(int angle) {
    return 500 + (angle * 2000 / 180);
}

// Alternative: use ESP32Servo library (handles this automatically)
// Just use: servo.write(angle);

// ================================================================
// SERVO CONTROL FUNCTIONS
// ================================================================

void moveServo(Servo &servo, int &currentPos, int targetPos, int speed = 30) {
    // Speed: degrees per movement step (lower = slower)
    // Move gradually for smooth motion
    
    if (targetPos < currentPos) {
        for (int pos = currentPos; pos >= targetPos; pos--) {
            servo.write(pos);
            currentPos = pos;
            delay(speed);  // Control speed
        }
    } else {
        for (int pos = currentPos; pos <= targetPos; pos++) {
            servo.write(pos);
            currentPos = pos;
            delay(speed);
        }
    }
}

void moveServoInstant(Servo &servo, int &currentPos, int targetPos) {
    servo.write(targetPos);
    currentPos = targetPos;
}

// ================================================================
// HOME POSITION (CALIBRATION START)
// ================================================================
void goHome() {
    moveServo(baseServo, currentBase, 90 + BASE_ZERO_OFFSET, 20);
    moveServo(elbowServo, currentElbow, 90 + ELBOW_ZERO_OFFSET, 20);
    moveServo(gripServo, currentGrip, GRIP_OPEN_ANGLE, 20);
}

// ================================================================
// CALIBRATION MODE
// ================================================================
void calibrationMode() {
    Serial.println("=== CALIBRATION MODE ===");
    Serial.println("Controls:");
    Serial.println("  1/2: Base +/- 5°");
    Serial.println("  3/4: Elbow +/- 5°");
    Serial.println("  5/6: Gripper open/close");
    Serial.println("  H: Go home");
    Serial.println("  S: Save and exit");
    Serial.println("=======================");
    
    while (true) {
        if (Serial.available()) {
            char cmd = Serial.read();
            
            switch(cmd) {
                case '1': currentBase = constrain(currentBase + 5, BASE_MIN, BASE_MAX);
                          baseServo.write(currentBase); break;
                case '2': currentBase = constrain(currentBase - 5, BASE_MIN, BASE_MAX);
                          baseServo.write(currentBase); break;
                case '3': currentElbow = constrain(currentElbow + 5, ELBOW_MIN, ELBOW_MAX);
                          elbowServo.write(currentElbow); break;
                case '4': currentElbow = constrain(currentElbow - 5, ELBOW_MIN, ELBOW_MAX);
                          elbowServo.write(currentElbow); break;
                case '5': gripServo.write(GRIP_OPEN_ANGLE); currentGrip = GRIP_OPEN_ANGLE; break;
                case '6': gripServo.write(GRIP_CLOSE_ANGLE); currentGrip = GRIP_CLOSE_ANGLE; break;
                case 'h': case 'H': goHome(); break;
                case 's': case 'S':
                    Serial.println("Calibration saved!");
                    Serial.print("BASE_ZERO_OFFSET = "); Serial.println(BASE_ZERO_OFFSET);
                    Serial.print("ELBOW_ZERO_OFFSET = "); Serial.println(ELBOW_ZERO_OFFSET);
                    return;
            }
            
            // Print current positions
            Serial.print("Base: "); Serial.print(currentBase);
            Serial.print(" | Elbow: "); Serial.print(currentElbow);
            Serial.print(" | Grip: "); Serial.println(currentGrip);
        }
        delay(10);
    }
}

// ================================================================
// IR SENSOR READING
// ================================================================
int readIRSensor() {
    // Read analog value from IR sensor
    int rawValue = analogRead(IR_SENSOR_PIN);
    return rawValue;
}

bool isCardDetected() {
    int value = readIRSensor();
    // Threshold - tune this during calibration!
    #define IR_THRESHOLD 500
    return value > IR_THRESHOLD;
}

// ================================================================
// LED CONTROL
// ================================================================
void setPowerLED(bool on) {
    digitalWrite(LED_POWER_PIN, on ? HIGH : LOW);
}

void setStatusLED(bool on) {
    digitalWrite(LED_STATUS_PIN, on ? HIGH : LOW);
}

void blinkStatusLED(int times, int delayMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_STATUS_PIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_STATUS_PIN, LOW);
        delay(delayMs);
    }
}

// ================================================================
// SETUP
// ================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("Robotic Drawing Assistant v1.0");
    
    // Initialize servo objects
    baseServo.attach(SERVO_BASE_PIN);
    elbowServo.attach(SERVO_ELBOW_PIN);
    gripServo.attach(SERVO_GRIP_PIN);
    
    // Initialize pins
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_POWER_PIN, OUTPUT);
    pinMode(LED_STATUS_PIN, OUTPUT);
    pinMode(IR_SENSOR_PIN, INPUT);
    
    // Power LED on
    setPowerLED(true);
    
    // Go to home position
    goHome();
    
    // Blink to indicate ready
    blinkStatusLED(3, 200);
    
    Serial.println("System ready!");
}

// ================================================================
// MAIN LOOP
// ================================================================
void loop() {
    // Check IR sensor
    int irValue = readIRSensor();
    Serial.print("IR Sensor: ");
    Serial.println(irValue);
    
    if (isCardDetected()) {
        Serial.println("Card detected!");
        setStatusLED(true);
        
        // TODO: Identify card and draw
        // TODO: Play audio
        
        delay(500);
    } else {
        setStatusLED(false);
    }
    
    // Check button
    if (digitalRead(BUTTON_PIN) == LOW) {
        Serial.println("Button pressed - Starting calibration");
        blinkStatusLED(5, 100);
        calibrationMode();
    }
    
    delay(100);
}
'''


def lesson2_arduino_code():
    """Lesson 2: Understanding the Arduino code"""
    print("\n" + "="*60)
    print("LESSON 2: ESP32-C3 ARDUINO CODE")
    print("="*60)
    
    print("\n" + ARDUINO_TEMPLATE)
    
    print("""
NEXT STEPS:
1. Copy this code to Arduino IDE
2. Install these libraries:
   - ESP32Servo (by John Sanpe)
   
3. Select board: ESP32C3 Dev Module
4. Upload and open Serial Monitor at 115200 baud
5. Test calibration mode!
""")


"""
================================================================
LESSON 3: CALIBRATION PROCEDURE (CRITICAL!)
================================================================
"""

CALIBRATION_GUIDE = """
================================================================
CALIBRATION GUIDE - STEP BY STEP
================================================================

This is the most important lesson! Calibration ensures your
robot draws accurately.

=== WHAT YOU NEED TO CALIBRATE ===

1. SERVO OFFSETS
   - Every servo is slightly different
   - "Zero" position might not be exactly 90°
   - Offset tells the robot where "center" really is

2. SERVO LIMITS  
   - Mechanical stops prevent full 0-180° rotation
   - You need to find the actual safe range
   - Going beyond can damage servos!

3. ARM LENGTHS
   - Measure actual arm lengths in mm
   - For kinematics calculations

4. IR SENSOR THRESHOLD
   - What value means "card present"?
   - Depends on card color and ambient light

=== STEP 1: MOUNT YOUR SERVOS ===

1. Mount base servo (shoulder rotation)
   - Horizontal mounting
   - Will rotate left/right
   
2. Mount elbow servo
   - Attached to end of base arm
   - Controls arm bend
   
3. Mount gripper servo
   - At end of forearm
   - Opens and closes claws

=== STEP 2: CALIBRATE BASE SERVO ===

1. Upload code and open Serial Monitor
2. Type 'H' to go home (center position)
3. Observe the arm position
4. Is it pointing straight forward? YES → offset = 0
5. Is it pointing LEFT? → offset = +10, +20, etc.
6. Is it pointing RIGHT? → offset = -10, -20, etc.

ADJUST IN CODE:
  Find: int BASE_ZERO_OFFSET = 0;
  Change to your value

=== STEP 3: CALIBRATE ELBOW SERVO ===

1. Stay at home position
2. Is elbow at 90° (horizontal)? YES → offset = 0
3. Is elbow pointing UP? → offset = -10, -20
4. Is elbow pointing DOWN? → offset = +10, +20

ADJUST IN CODE:
  Find: int ELBOW_ZERO_OFFSET = 0;
  Change to your value

=== STEP 4: FIND SERVO LIMITS ===

THE PROBLEM: SG90 servos have 180° range but mechanical
            parts might not allow full rotation!

SAFE TESTING:
1. Slowly increase base servo from 10 to 170
2. Listen for strain/clicking (bad!)
3. Find the MIN and MAX that work smoothly

FOR BASE:
  - Type '1' repeatedly to increase by 5°
  - Find minimum: ~10-20°
  - Find maximum: ~160-170°
  - Update: #define BASE_MIN 20
            #define BASE_MAX 160

FOR ELBOW:
  - Same process
  - Elbows often need more restriction
  - Typical: 30° to 150°

FOR GRIPPER:
  - Find full open: ~80-90°
  - Find full close: ~0-10°
  - No strain should be heard!

=== STEP 5: CALIBRATE IR SENSOR ===

1. Open Serial Monitor (115200 baud)
2. Watch IR values:

WITHOUT card:
  Expected: ~50-200
  (Low value = dark/nothing)

WITH card:
  Expected: ~800-1023
  (High value = light reflection)

3. Tune threshold:
  Find: #define IR_THRESHOLD 500
  Adjust based on your readings!

TIPS:
  - Test with different card colors
  - Gray cards give intermediate values
  - White = high, Black = low

=== STEP 6: FINE-TUNE ITERATIVELY ===

Calibration is iterative! Repeat until perfect:

1. Adjust offsets → test home position
2. Verify limits don't cause strain
3. Test IR detection reliability
4. Check all movements are smooth

GOOD SIGN: Robot returns to same position reliably!

=== CALIBRATION RECORD SHEET ===

Save your values here:

BASE_ZERO_OFFSET:    _____
ELBOW_ZERO_OFFSET:   _____
GRIP_OPEN_ANGLE:     _____
GRIP_CLOSE_ANGLE:    _____
BASE_MIN:            _____
BASE_MAX:            _____
ELBOW_MIN:           _____
ELBOW_MAX:           _____
IR_THRESHOLD:        _____
"""


def lesson3_calibration():
    """Lesson 3: Full calibration guide"""
    print(CALIBRATION_GUIDE)


"""
================================================================
LESSON 4: KINEMATICS FOR YOUR 2-SERVO ROBOT
================================================================
"""

KINEMATICS_2SERVO = """
================================================================
2-SERVO ROBOT KINEMATICS
================================================================

Unlike a SCARA with stepper motors, your servo robot has
limited rotation (180° per servo) but simpler control!

=== ARM STRUCTURE ===

Base Servo (θ1):
- Rotates arm left/right
- Range: Limited by calibration (e.g., 20° to 160°)

Elbow Servo (θ2):
- Bends/folds the arm
- Range: Limited by calibration (e.g., 30° to 150°)

=== FORWARD KINEMATICS ===

Given: θ1 and θ2 angles
Find: X, Y position of gripper

X = L1 × cos(θ1) + L2 × cos(θ1 + θ2)
Y = L1 × sin(θ1) + L2 × sin(θ1 + θ2)

Where:
- L1 = upper arm length (mm)
- L2 = forearm length (mm)
- θ1 = base angle
- θ2 = elbow angle (relative to upper arm)

Example (L1=80mm, L2=70mm, θ1=45°, θ2=60°):
- X = 80×cos(45°) + 70×cos(105°)
- X = 56.6 + (-18.1) = 38.5mm
- Y = 80×sin(45°) + 70×sin(105°)
- Y = 56.6 + 67.6 = 124.2mm

=== INVERSE KINEMATICS ===

Given: X, Y target position
Find: θ1 and θ2 angles

This is HARDER but doable!

Steps:
1. Distance D = √(X² + Y²)
2. Check if reachable (L1-L2 < D < L1+L2)
3. θ2 = arccos((L1² + L2² - D²) / (2×L1×L2))
4. θ1 = atan2(Y, X) - arcsin(L2 × sin(θ2) / D)

=== WORKSPACE ===

Your robot can only reach points within:
- Max reach: L1 + L2
- Min reach: |L1 - L2|

Example (L1=80mm, L2=70mm):
- Max: 150mm from center
- Min: 10mm from center

=== PRACTICAL ANGLES ===

For a card-reading robot, these are useful positions:

1. READY_POSITION
   - Base: 90° (center)
   - Elbow: 60° (arm extended forward)
   - Use: Waiting for card

2. HOME_POSITION
   - Base: 90°
   - Elbow: 90°
   - Use: Resting/stopped

3. PICKUP_POSITION
   - Base: 90°
   - Elbow: 30°
   - Use: Closer to surface

4. GRIP_CLOSE
   - Gripper: 0°
   - Use: Holding card/paper

=== MOVEMENT PLANNING ===

For smooth drawing:

1. Rise up (Z+ safety height)
2. Move to start position
3. Lower down (Z-)
4. Draw path
5. Rise up
6. Move to next position
7. Repeat

Since you don't have Z-axis control, use:
- GRIP_OPEN = ready to pick
- GRIP_CLOSE = gripping card
"""


def lesson4_kinematics():
    """Lesson 4: Understanding robot math"""
    print(KINEMATICS_2SERVO)


"""
================================================================
LESSON 5: DRAWING PATTERNS
================================================================
"""

DRAWING_PATTERNS = """
================================================================
DRAWING PATTERNS & MOVEMENTS
================================================================

Now we program the robot to draw shapes!

=== DRAWING A LINE ===

To draw from point A to point B:

1. Find angles for point A (θ1A, θ2A)
2. Find angles for point B (θ1B, θ2B)
3. Interpolate between angles

LINE_DRAW algorithm:
- Divide line into N segments (e.g., 10)
- For each segment i:
  - t = i / N
  - θ1 = θ1A + t × (θ1B - θ1A)
  - θ2 = θ2A + t × (θ2B - θ2A)
  - Move to (θ1, θ2)
  - Small delay for drawing

=== DRAWING A CIRCLE ===

Circle with center (CX, CY) and radius R:

For angle φ from 0 to 360°:
- X = CX + R × cos(φ)
- Y = CY + R × sin(φ)
- Find IK for (X, Y)
- Move to angles

=== DRAWING A SQUARE ===

4 corners:
1. (CX-R, CY-R)  top-left
2. (CX+R, CY-R)  top-right  
3. (CX+R, CY+R)  bottom-right
4. (CX-R, CY+R)  bottom-left

Draw lines between corners!

=== DRAWING A STAR ===

5-pointed star:
- Outer radius: R
- Inner radius: r = R × 0.382
- Center: (CX, CY)

Points (starting from top):
1. (CX, CY-R)          top
2. (CX + r×sin(72°), CY - r×cos(72°))
3. (CX + R×sin(144°), CY - R×cos(144°))
4. (CX - R×sin(144°), CY - R×cos(144°))
5. (CX - r×sin(72°), CY - r×cos(72°))

Connect 1→3→5→2→4→1

=== EXAMPLE: DRAWING SEQUENCE ===

For "STAR" drawing:

void drawStar() {
    // Open gripper
    gripServo.write(GRIP_OPEN_ANGLE);
    delay(500);
    
    // Move to start point (top of star)
    moveToPosition(starPoints[0].x, starPoints[0].y);
    delay(300);
    
    // Close gripper to hold pen
    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);
    
    // Draw star by connecting points
    for (int i = 1; i < 5; i++) {
        moveToPosition(starPoints[i].x, starPoints[i].y);
        delay(100);
    }
    
    // Open gripper to release
    gripServo.write(GRIP_OPEN_ANGLE);
}
"""


def lesson5_drawing():
    """Lesson 5: Programming drawing movements"""
    print(DRAWING_PATTERNS)


"""
================================================================
LESSON 6: POWER MANAGEMENT & SAFETY
================================================================
"""

POWER_SAFETY = """
================================================================
POWER MANAGEMENT & SAFETY
================================================================

Your robot runs on 2x 18650 batteries. Critical knowledge!

=== BATTERY SAFETY ===

⚠️ WARNING: Lithium batteries can be DANGEROUS!

RULES:
1. Never discharge below 3.0V per cell
2. Never charge above 4.2V per cell
3. Never short circuit!
4. Never puncture battery
5. Use protected batteries if possible
6. Always monitor temperature

=== CHARGING ===

TP4056 handles charging automatically:
- Connect 5V USB-C
- Red LED = charging
- Blue LED = fully charged
- Typical charge time: 4-6 hours

=== POWER BUDGET ===

Components and current draw:
- ESP32-C3: ~50-100mA
- 3x SG90 (idle): ~30mA total
- 3x SG90 (moving): ~200-400mA
- MAX98357 @ 3W: ~700mA
- IR sensor: ~10mA
- LEDs: ~20mA

PEAK current: ~1.2A
AVERAGE current: ~300mA

Battery: 6000mAh
Battery life: 6000 / 300 = 20 hours (theoretical)
Realistic: 10-15 hours

=== BOOST CONVERTER EFFICIENCY ===

TPS61023 at 5V output:
- ~90-95% efficiency
- At 1A output: ~1.1A from battery
- At 0.5A output: ~0.55A from battery

=== LOW BATTERY DETECTION ===

Monitor battery voltage!
- 2 cells in parallel = same voltage
- Full: 4.2V
- Nominal: 3.7V
- Low: 3.4V
- Critical: 3.2V (stop!)

Arduino code:
float batteryVoltage = analogRead(BAT_ADC) * 3.3 / 4095 * 2;  // ×2 for voltage divider
if (batteryVoltage < 3.4) {
    // Flash warning LED
    // Stop motors
    // Play "low battery" audio
}

=== POWER-ON SEQUENCE ===

SAFE STARTUP:
1. Battery → Boost converter → ESP32 starts
2. ESP32 initializes (100ms)
3. Servos.attach() - IMPORTANT!
4. Read IR sensor
5. Wait for card

SAFE SHUTDOWN:
1. Open gripper
2. Go to home position
3. Detach servos (save power)
4. ESP32 deep sleep or power off

=== EXTENDING BATTERY LIFE ===

1. Detach servos when not moving
2. Use sleep mode between actions
3. Don't run all servos at once if possible
4. Use lower PWM frequency when idle
5. Turn off LEDs when not needed
"""


def lesson6_power_safety():
    """Lesson 6: Power management"""
    print(POWER_SAFETY)


"""
================================================================
LESSON 7: VOICE/AUDIO SYSTEM
================================================================
"""

AUDIO_SYSTEM = """
================================================================
AUDIO FEEDBACK SYSTEM
================================================================

MAX98357 I2S class D amplifier for voice output!

=== WIRING ===

MAX98357 → ESP32-C3:
- LRC (WS): GPIO 21
- BCLK: GPIO 22
- DIN: GPIO (same as WS or separate)

MAX98357:
- OUT+: Speaker +
- OUT-: Speaker -
- GND: Common ground
- VIN: 5V from boost converter
- SD: Enable (pull high or to GPIO)

=== AUDIO LIBRARIES ===

For ESP32, use:
- ESP8266Audio (works with ESP32)
- Or ESP32-audioI2S

Installation:
1. Arduino IDE → Sketch → Include Library → Manage Libraries
2. Search "ESP8266Audio"
3. Install

=== AUDIO FILE FORMAT ===

- Format: WAV or MP3
- Sample rate: 22050Hz or 44100Hz
- Bits: 16-bit
- Channels: Mono (better for speech)

Recording audio:
1. Use Audacity or online text-to-speech
2. Export as WAV
3. Convert to header file or store on SPIFFS

=== CODE EXAMPLE ===

#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

AudioFileSourceSPIFFS *file;
AudioGeneratorWAV *wav;
AudioOutputI2S *out;

void playSound(const char* filename) {
    file = new AudioFileSourceSPIFFS(filename);
    wav = new AudioGeneratorWAV();
    out = new AudioOutputI2S();
    
    wav->begin(file, out);
    while(wav->isRunning()) {
        delay(10);
    }
    wav->stop();
}

void setup() {
    SPIFFS.begin(true);
}

void loop() {
    if (cardDetected) {
        playSound("/circle.wav");
    }
}

=== TEXT-TO-SPEECH OPTIONS ===

1. ONLINE: Google TTS, Amazon Polly
2. OFFLINE: Espeak (Raspberry Pi)
3. PRE-RECORDED: Record human voice

=== USEFUL AUDIO FILES ===

- "hello.wav" - Greeting
- "circle.wav" - "I'm drawing a circle!"
- "star.wav" - "Look, a star!"
- "triangle.wav" - "Triangle time!"
- "done.wav" - Drawing complete
- "error.wav" - Something went wrong
- "lowbat.wav" - Battery low warning
"""


def lesson7_audio():
    """Lesson 7: Audio system"""
    print(AUDIO_SYSTEM)


"""
================================================================
LESSON 8: FINAL INTEGRATION
================================================================
"""

INTEGRATION = """
================================================================
PUTTING IT ALL TOGETHER
================================================================

Complete system integration guide!

=== HARDWARE WIRING DIAGRAM ===

BATTERY (2x 18650)
     │
     ├──────────────────► TP4056 (USB-C charging)
     │                        │
     │                        ▼
     │                   USB-C Cable
     │
     ▼
TPS61023 Boost Converter
     │
     ├──── 5V ────► ESP32-C3 (VIN)
     │                 │
     │                 ├──── 3.3V ────► IR Sensor (VCC)
     │                 │
     │                 ├──── GPIO 2 ────► Servo Base
     │                 │
     │                 ├──── GPIO 3 ────► Servo Elbow
     │                 │
     │                 ├──── GPIO 4 ────► Servo Gripper
     │                 │
     │                 ├──── GPIO 5 ────► Button (to GND)
     │                 │
     │                 ├──── GPIO 6 ────► LED Power
     │                 │
     │                 ├──── GPIO 7 ────► LED Status
     │                 │
     │                 ├──── GPIO 20 ────► IR Sensor (AO)
     │                 │
     │                 ├──── GPIO 21 ────► MAX98357 LRC
     │                 │
     │                 └──── GPIO 22 ────► MAX98357 BCLK
     │                                   
     ├──── 5V ────► Servos (VCC)
     │                 │
     └──── 5V ────► MAX98357 (VIN)
                      │
                      └──── Speaker (8 ohm 1W)

=== FINAL CHECKLIST ===

Before first power-on:

□ Batteries properly inserted
□ Wire connections soldered/wired correctly
□ No exposed wires touching
□ Boost converter set to 5V
□ Servos connected to 5V (not 3.3V!)
□ IR sensor getting 3.3V
□ Audio amplifier getting 5V
□ All GND connections common

=== TEST SEQUENCE ===

1. POWER TEST
   - Turn on power switch
   - Power LED should light
   - ESP32 should start (check Serial Monitor)
   
2. SERVO TEST
   - Type 'H' for home position
   - All servos should move smoothly
   - No clicking/straining
   
3. IR TEST
   - Put card near sensor
   - Serial Monitor should show value change
   
4. AUDIO TEST
   - Play test sound
   - Speaker should produce sound
   
5. FULL SEQUENCE TEST
   - Insert card → robot draws → says name → done!

=== TROUBLESHOOTING ===

Problem: ESP32 not responding
Solution: Check USB connection, press reset button

Problem: Servos jittering
Solution: External power! USB can't supply servos

Problem: IR readings inconsistent
Solution: Shield from ambient light, adjust threshold

Problem: No audio
Solution: Check I2S pin connections, test with simple tone

Problem: Robot keeps missing target
Solution: Recalibrate servo offsets

=== PERFORMANCE TIPS ===

1. Use slower servo speeds for accuracy
2. Add pause between movements
3. Use more interpolation points for curves
4. Keep arms short for more precision
5. Add counterweight to reduce jitter

=== FIRMWARE UPDATE PROCESS ===

1. Hold BOOT button on ESP32-C3
2. Press and release RESET
3. Upload new sketch
4. Release BOOT button

=== NEXT UPGRADES ===

Consider adding:
- OLED display for status
- SD card for more audio files
- More IR sensors for position detection
- Limit switches for calibration
- Force sensor for grip detection
"""


def lesson8_integration():
    """Lesson 8: Complete system integration"""
    print(INTEGRATION)


"""
================================================================
MAIN MENU
================================================================
"""


def main():
    print("""
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║     🤖 ROBOTIC DRAWING ASSISTANT - LEARNING COURSE 🤖       ║
║                                                              ║
║     For ESP32-C3 + 3x SG90 Servos + MAX98357 Audio           ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝

LESSONS AVAILABLE:
───────────────────────────────────────────────────────────────
  1. Hardware Overview      - Understand your components
  2. Arduino Code           - Base firmware structure
  3. Calibration Guide     - Tune your robot (CRITICAL!)
  4. Kinematics             - Robot math
  5. Drawing Patterns       - Program movements
  6. Power & Safety         - Battery management
  7. Audio System           - Voice feedback
  8. Integration           - Put it all together
───────────────────────────────────────────────────────────────

Run any lesson by calling its function:
  lesson1_hardware_overview()
  lesson2_arduino_code()
  lesson3_calibration()
  lesson4_kinematics()
  lesson5_drawing()
  lesson6_power_safety()
  lesson7_audio()
  lesson8_integration()

Or run all lessons automatically:
""")
    
    choice = input("Run all lessons? (y/n): ").lower().strip()
    
    if choice == 'y':
        print("\n" + "="*60)
        lesson1_hardware_overview()
        print("\n" + "="*60)
        lesson2_arduino_code()
        print("\n" + "="*60)
        lesson3_calibration()
        print("\n" + "="*60)
        lesson4_kinematics()
        print("\n" + "="*60)
        lesson5_drawing()
        print("\n" + "="*60)
        lesson6_power_safety()
        print("\n" + "="*60)
        lesson7_audio()
        print("\n" + "="*60)
        lesson8_integration()
        print("\n" + "="*60)
        print("🎉 COURSE COMPLETE! Time to build your robot!")
        print("="*60)


if __name__ == "__main__":
    main()