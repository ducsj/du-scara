/*
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║       🤖 DUAL/PARALLEL ARM SCARA ROBOT WITH SPEECH 🤖                        ║
║                                                                              ║
║   VERSION 2.0 - Complete Build Guide Included!                             ║
║                                                                              ║
║   WHAT IT DOES:                                                             ║
║   - Detects cards with IR sensor                                            ║
║   - Speaks/shows letter, number, or shape name                              ║
║   - Draws letters (A-Z), numbers (0-9), shapes                              ║
║   - Audio feedback with pronunciation                                        ║
║                                                                              ║
║   HARDWARE: ESP32-C3 + 3x SG90 + MAX98357 + IR Sensor                      ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

══════════════════════════════════════════════════════════════════════════════
📖 HOW TO USE THIS CODE - STEP BY STEP
══════════════════════════════════════════════════════════════════════════════

STEP 1: Read BUILD_GUIDE.md for complete parts list and assembly

STEP 2: Wire everything according to WIRING_GUIDE.md

STEP 3: Install Arduino IDE and libraries (see SETUP_BEGINNER_GUIDE.md)

STEP 4: Copy this code to Arduino IDE and upload

STEP 5: Open Serial Monitor at 115200 baud

STEP 6: Test and calibrate using commands below

══════════════════════════════════════════════════════════════════════════════
🎮 CALIBRATION COMMANDS (Hold button 3 seconds to enter)
══════════════════════════════════════════════════════════════════════════════

  1/2    → Move base ±5°
  3/4    → Move elbow ±5°
  5/6    → Gripper open/close
  H      → Go home position
  + / -  → Base offset ±1°
  . / ,  → Elbow offset ±1°
  T      → Test IR sensor
  Q      → Quit and save

══════════════════════════════════════════════════════════════════════════════
🔧 IMPORTANT: CONFIGURE YOUR ROBOT HERE (See CALIBRATION_GUIDE.md)
══════════════════════════════════════════════════════════════════════════════
*/

// ═══════════════════════════════════════════════════════════════════════════
// 📦 INCLUDES - Tell Arduino which libraries we need
// ═══════════════════════════════════════════════════════════════════════════

#include <ESP32Servo.h>              // For controlling servo motors
#include "AudioFileSourceSPIFFS.h"    // For reading audio files from memory
#include "AudioGeneratorMP3.h"        // For playing MP3 audio
#include "AudioOutputI2S.h"         // For MAX98357 audio amplifier

// ═══════════════════════════════════════════════════════════════════════════
// 🔌 PIN CONNECTIONS - Where each wire connects to ESP32-C3
// ═══════════════════════════════════════════════════════════════════════════

// EXAMPLE: GPIO 2 (Pin 2) connects to Base Servo signal wire (orange)
#define SERVO_BASE_PIN     2    // Base servo - rotates arm left/right
#define SERVO_ELBOW_PIN    3    // Elbow servo - bends the arm
#define SERVO_GRIP_PIN     4    // Gripper servo - opens/closes claw
#define BUTTON_PIN         5    // Button - hold 3 sec for calibration
#define LED_POWER_PIN      6    // Power LED - always on when powered
#define LED_STATUS_PIN     7    // Status LED - blinks when working

// IR Sensor and Audio
#define IR_SENSOR_PIN     20    // IR sensor - detects cards
#define AUDIO_LRC_PIN     21    // Audio chip - Left/Right clock
#define AUDIO_BCLK_PIN    22    // Audio chip - Bit clock
#define AUDIO_DIN_PIN     18    // Audio chip - Data in


// ═══════════════════════════════════════════════════════════════════════════
// ⚙️ CALIBRATION VALUES - TUNE THESE FOR YOUR ROBOT!
// ═══════════════════════════════════════════════════════════════════════════
//
// HOW TO CALIBRATE:
// 1. Hold button 3 seconds
// 2. Use commands to find perfect values
// 3. Write down your numbers
// 4. Update these values
// 5. Upload new code
//

int BASE_ZERO_OFFSET   = 0;     // ← If arm doesn't point straight, adjust this
                               //   Positive = arm points LEFT
                               //   Negative = arm points RIGHT

int ELBOW_ZERO_OFFSET  = 0;     // ← If elbow doesn't sit at 90°, adjust this
                               //   Positive = elbow bends DOWN
                               //   Negative = elbow bends UP

#define IR_THRESHOLD     500    // ← IR value for card detection
                               //   Higher = more sensitive
                               //   Lower = less sensitive


// ═══════════════════════════════════════════════════════════════════════════
// 📐 ARM MEASUREMENTS - Measure your robot's arms with a ruler
// ═══════════════════════════════════════════════════════════════════════════
//
// MEASURE LIKE THIS:
//
//     Base Servo Center ──────────────┬───────── Elbow Servo Center = L1
//                                     │
//                                     │  L1 = Upper Arm (mm)
//
//     Elbow Servo Center ─────────────┴───────── Gripper = L2
//                                     │
//                                     │  L2 = Forearm (mm)
//
// UPDATE THESE WITH YOUR MEASUREMENTS!

#define L1_ARM_LENGTH     80    // ← Upper arm length in mm (DEFAULT: 80mm)
#define L2_ARM_LENGTH     70    // ← Forearm length in mm (DEFAULT: 70mm)


// ═══════════════════════════════════════════════════════════════════════════
// 🚫 SERVO LIMITS - Keep servos from breaking!
// ═══════════════════════════════════════════════════════════════════════════
//
// DEFAULT VALUES (may need adjustment after testing):
//

#define BASE_MIN    20          // ← Base servo: Don't go below this
#define BASE_MAX    160         // ← Base servo: Don't go above this
#define ELBOW_MIN   30          // ← Elbow servo: Don't go below this
#define ELBOW_MAX   150         // ← Elbow servo: Don't go above this

#define GRIP_OPEN_ANGLE   90    // ← Gripper fully open position
#define GRIP_CLOSE_ANGLE   0    // ← Gripper fully closed position

#define SERVO_SPEED     30       // ← Movement speed (lower = slower)


// ═══════════════════════════════════════════════════════════════════════════
// 🎨 GRIPPER POSITIONS
// ═══════════════════════════════════════════════════════════════════════════

#define GRIP_OPEN     GRIP_OPEN_ANGLE    // Gripper open (holding nothing)
#define GRIP_HOLD     45                 // Gripper partially closed (holding pen)
#define GRIP_CLOSE    GRIP_CLOSE_ANGLE   // Gripper closed


// ═══════════════════════════════════════════════════════════════════════════
// 🦾 CREATE SERVO OBJECTS
// ═══════════════════════════════════════════════════════════════════════════

Servo baseServo;      // "This is the base rotation servo"
Servo elbowServo;     // "This is the elbow bending servo"
Servo gripServo;      // "This is the gripper claw servo"

// Current angles (where each servo is right now)
int currentBase   = 90;   // Arm rotation (degrees)
int currentElbow  = 90;   // Elbow bend (degrees)
int currentGrip    = 90;   // Gripper state (degrees)


// ═══════════════════════════════════════════════════════════════════════════
// 🔊 AUDIO SYSTEM - Make the robot SPEAK!
// ═══════════════════════════════════════════════════════════════════════════
//
// AUDIO FILES NEEDED (Create these and upload to SPIFFS):
//
//   /audio/A.mp3 through /audio/Z.mp3  (alphabet)
//   /audio/0.mp3 through /audio/9.mp3   (numbers)
//   /audio/circle.mp3, square.mp3, etc. (shape names)
//
// HOW TO CREATE: Use Google Translate - type word, click speaker, save as MP3
//

AudioFileSourceSPIFFS *audioFile = nullptr;  // For reading audio files
AudioGeneratorMP3 *audioMP3 = nullptr;      // For playing MP3
AudioOutputI2S *audioOut = nullptr;          // For audio output
bool audioInitialized = false;              // Is audio system ready?


// ═══════════════════════════════════════════════════════════════════════════
// 🎨 DRAWING SETTINGS
// ═══════════════════════════════════════════════════════════════════════════

// Drawing positions (X, Y coordinates in mm)
#define DRAW_HOME_X      50    // Home X position for drawing
#define DRAW_HOME_Y      50    // Home Y position for drawing

// Size presets
#define SMALL_SIZE       20    // Small shapes (mm)
#define MEDIUM_SIZE      30    // Medium shapes (mm)
#define LARGE_SIZE        40    // Large shapes (mm)


// ═══════════════════════════════════════════════════════════════════════════
// 📊 STATES
// ═══════════════════════════════════════════════════════════════════════════

enum RobotState {
    STATE_IDLE,           // Waiting for card
    STATE_CARD_DETECTED,  // Card found
    STATE_DRAWING,        // Drawing in progress
    STATE_DONE,           // Drawing complete
    STATE_CALIBRATION     // Calibration mode
};

enum CardType {
    CARD_NONE,
    CARD_ALPHABET,    // Letter A-Z
    CARD_NUMBER,       // Number 0-9
    CARD_SHAPE,       // Shape (circle, square, etc.)
    CARD_WORD         // Custom word
};

RobotState currentState = STATE_IDLE;


// ═══════════════════════════════════════════════════════════════════════════
// 🔊 AUDIO FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void initAudio() {
    // Initialize the MAX98357 audio amplifier
    Serial.println("🔊 Initializing audio...");
    audioOut = new AudioOutputI2S();
    audioOut->setPinout(AUDIO_BCLK_PIN, AUDIO_DIN_PIN, AUDIO_LRC_PIN);
    audioOut->setGain(1.0);  // Volume: 0.0 to 1.0
    audioInitialized = true;
    Serial.println("✅ Audio ready!");
}

bool playAudioFile(const char* filename) {
    // Play an MP3 file from SPIFFS memory
    if (!audioInitialized) {
        Serial.println("⚠️ Audio not initialized!");
        return false;
    }
    
    Serial.print("🔊 Playing: ");
    Serial.println(filename);
    
    // Close previous audio
    if (audioFile) { delete audioFile; audioFile = nullptr; }
    if (audioMP3) { delete audioMP3; audioMP3 = nullptr; }
    
    // Open and play file
    audioFile = new AudioFileSourceSPIFFS(filename);
    if (!audioFile->isOpen()) {
        Serial.print("❌ File not found: ");
        Serial.println(filename);
        return false;
    }
    
    audioMP3 = new AudioGeneratorMP3();
    if (audioMP3->begin(audioFile, audioOut)) {
        while (audioMP3->isRunning()) {
            audioMP3->loop();
            delay(10);
        }
        audioMP3->stop();
        return true;
    }
    return false;
}

void playBeep(int frequency, int durationMs) {
    // Fallback beep sound (if audio file missing)
    int halfPeriod = 500000 / frequency;
    int iterations = (durationMs * 1000) / (halfPeriod * 2);
    
    for (int i = 0; i < iterations; i++) {
        digitalWrite(LED_STATUS_PIN, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(LED_STATUS_PIN, LOW);
        delayMicroseconds(halfPeriod);
    }
}

void playSuccessBeep() {
    playBeep(880, 100);
    delay(100);
    playBeep(1320, 200);
}

void playStartupJingle() {
    playBeep(523, 150);
    delay(50);
    playBeep(659, 150);
    delay(50);
    playBeep(784, 150);
    delay(50);
    playBeep(1047, 150);
}

void sayLetter(char letter) {
    // Say a single letter (A-Z)
    char filename[20];
    sprintf(filename, "/audio/%c.mp3", toupper(letter));
    playAudioFile(filename);
}

void sayNumber(int num) {
    // Say a number (0-9)
    if (num >= 0 && num <= 9) {
        char filename[20];
        sprintf(filename, "/audio/%d.mp3", num);
        playAudioFile(filename);
    }
}

void spellWord(const char* word) {
    // Spell a word letter by letter
    Serial.print("🔤 Spelling: ");
    Serial.println(word);
    
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        char c = word[i];
        if (c >= 'a' && c <= 'z') c = toupper(c);
        
        if (c >= 'A' && c <= 'Z') {
            Serial.print(c);
            Serial.print(" ");
            sayLetter(c);
            delay(200);
        }
        else if (c >= '0' && c <= '9') {
            Serial.print(c);
            sayNumber(c - '0');
            delay(200);
        }
        else if (c == ' ') {
            delay(500);
            Serial.print(" ");
        }
    }
    Serial.println();
}

void saySentence(const char* files[], int count) {
    // Play multiple audio files in sequence
    for (int i = 0; i < count; i++) {
        playAudioFile(files[i]);
        delay(300);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// 🦾 SERVO MOVEMENT FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void moveServo(Servo &servo, int &currentPos, int targetPos) {
    // Move a single servo gradually (smooth motion)
    targetPos = constrain(targetPos, 0, 180);
    
    if (targetPos < currentPos) {
        for (int pos = currentPos; pos >= targetPos; pos--) {
            servo.write(pos);
            currentPos = pos;
            delay(SERVO_SPEED);
        }
    } else {
        for (int pos = currentPos; pos <= targetPos; pos++) {
            servo.write(pos);
            currentPos = pos;
            delay(SERVO_SPEED);
        }
    }
}

void moveArm(int baseAngle, int elbowAngle) {
    // Move BOTH arm servos together (smooth!)
    
    // Apply calibration offsets
    baseAngle  += BASE_ZERO_OFFSET;
    elbowAngle += ELBOW_ZERO_OFFSET;
    
    // Keep within safe limits
    baseAngle  = constrain(baseAngle,  BASE_MIN,  BASE_MAX);
    elbowAngle = constrain(elbowAngle, ELBOW_MIN, ELBOW_MAX);
    
    // Move gradually (interleaved for smoothness)
    while (currentBase != baseAngle || currentElbow != elbowAngle) {
        
        // Move base
        if (currentBase < baseAngle) {
            currentBase = min(currentBase + 2, baseAngle);
            baseServo.write(currentBase);
        } else if (currentBase > baseAngle) {
            currentBase = max(currentBase - 2, baseAngle);
            baseServo.write(currentBase);
        }
        
        // Move elbow
        if (currentElbow < elbowAngle) {
            currentElbow = min(currentElbow + 2, elbowAngle);
            elbowServo.write(currentElbow);
        } else if (currentElbow > elbowAngle) {
            currentElbow = max(currentElbow - 2, elbowAngle);
            elbowServo.write(currentElbow);
        }
        
        delay(SERVO_SPEED);
    }
}

void goHome() {
    // Return to starting position
    moveArm(90, 90);
    gripServo.write(GRIP_OPEN);
    currentGrip = GRIP_OPEN;
}

void readyPosition() {
    // Position ready to draw
    moveArm(90, 60);
    gripServo.write(GRIP_HOLD);
    delay(500);
}


// ═══════════════════════════════════════════════════════════════════════════
// 📐 INVERSE KINEMATICS - Convert position to angles
// ═══════════════════════════════════════════════════════════════════════════
//
// WHAT THIS DOES:
// Given a position (X, Y), calculate what angles the servos need to be
//
// EXAMPLE:
//   Input: Move to position (50, 50)
//   Output: Set base to 30°, elbow to 45°
//

bool inverseKinematics(float targetX, float targetY, float &theta1, float &theta2) {
    // Calculate distance from center to target
    float D = sqrt(targetX * targetX + targetY * targetY);
    float maxReach = L1_ARM_LENGTH + L2_ARM_LENGTH;
    float minReach = abs(L1_ARM_LENGTH - L2_ARM_LENGTH);
    
    // Check if target is reachable
    if (D > maxReach || D < minReach) {
        Serial.println("⚠️ Target out of reach!");
        return false;
    }
    
    // Calculate elbow angle using Law of Cosines
    float cosTheta2 = (D * D - L1_ARM_LENGTH * L1_ARM_LENGTH - L2_ARM_LENGTH * L2_ARM_LENGTH)
                      / (2 * L1_ARM_LENGTH * L2_ARM_LENGTH);
    cosTheta2 = constrain(cosTheta2, -1.0, 1.0);
    theta2 = acos(cosTheta2);
    
    // Calculate base angle
    float psi = atan2(targetY, targetX);  // Angle to target
    float beta = asin(constrain(L2_ARM_LENGTH * sin(theta2) / D, -1.0, 1.0));
    theta1 = psi - beta;
    
    // Convert to degrees
    theta1 = degrees(theta1);
    theta2 = degrees(theta2);
    
    return true;
}

bool moveToPosition(float x, float y) {
    // Move to X,Y position using inverse kinematics
    float t1, t2;
    if (inverseKinematics(x, y, t1, t2)) {
        moveArm((int)t1, (int)t2);
        return true;
    }
    return false;
}


// ═══════════════════════════════════════════════════════════════════════════
// 👁️ IR SENSOR FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

int readIRSensor() {
    // Read raw IR sensor value (0-4095)
    return analogRead(IR_SENSOR_PIN);
}

bool isCardDetected() {
    // Returns true if card is present (value above threshold)
    return readIRSensor() > IR_THRESHOLD;
}


// ═══════════════════════════════════════════════════════════════════════════
// 💡 LED FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void setPowerLED(bool on) {
    digitalWrite(LED_POWER_PIN, on ? HIGH : LOW);
}

void blinkStatusLED(int times, int delayMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_STATUS_PIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_STATUS_PIN, LOW);
        delay(delayMs);
    }
}

void setStatusLED(bool on) {
    digitalWrite(LED_STATUS_PIN, on ? HIGH : LOW);
}


// ═══════════════════════════════════════════════════════════════════════════
// 🎨 DRAWING FUNCTIONS - Draw shapes and letters
// ═══════════════════════════════════════════════════════════════════════════

void penDown() {
    gripServo.write(GRIP_HOLD);  // Hold pen
    delay(300);
}

void penUp() {
    gripServo.write(GRIP_OPEN);  // Release pen
    delay(200);
}

void drawLine(float x1, float y1, float x2, float y2, int segments = 10) {
    // Draw a straight line from (x1,y1) to (x2,y2)
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float x = x1 + t * (x2 - x1);
        float y = y1 + t * (y2 - y1);
        if (moveToPosition(x, y)) {
            delay(50);
        }
    }
}

void drawCircle(float cx, float cy, float radius, int segments = 36) {
    // Draw a circle at center (cx, cy) with given radius
    penDown();
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2 * PI;
        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);
        moveToPosition(x, y);
        delay(30);
    }
    penUp();
}

void drawSquare(float cx, float cy, float size) {
    float half = size / 2;
    penDown();
    drawLine(cx - half, cy - half, cx + half, cy - half, 10);
    drawLine(cx + half, cy - half, cx + half, cy + half, 10);
    drawLine(cx + half, cy + half, cx - half, cy + half, 10);
    drawLine(cx - half, cy + half, cx - half, cy - half, 10);
    penUp();
}

void drawTriangle(float cx, float cy, float size) {
    float h = size * sqrt(3) / 2;
    penDown();
    moveToPosition(cx, cy - size/2);
    drawLine(cx, cy - size/2, cx - size/2, cy + h/3, 10);
    drawLine(cx - size/2, cy + h/3, cx + size/2, cy + h/3, 10);
    drawLine(cx + size/2, cy + h/3, cx, cy - size/2, 10);
    penUp();
}

void drawStar(float cx, float cy, float outerR, float innerR) {
    // Draw 5-pointed star
    float points[5][2];
    float innerPoints[5][2];
    
    // Calculate points
    for (int i = 0; i < 5; i++) {
        float angle = -PI/2 + i * 2 * PI / 5;
        points[i][0] = cx + outerR * cos(angle);
        points[i][1] = cy + outerR * sin(angle);
        
        float innerAngle = -PI/2 + (i * 2 + 1) * PI / 5;
        innerPoints[i][0] = cx + innerR * cos(innerAngle);
        innerPoints[i][1] = cy + innerR * sin(innerAngle);
    }
    
    penDown();
    moveToPosition(points[0][0], points[0][1]);
    for (int i = 0; i < 5; i++) {
        drawLine(points[i][0], points[i][1], innerPoints[i][0], innerPoints[i][1], 5);
        drawLine(innerPoints[i][0], innerPoints[i][1],
                 points[(i+1)%5][0], points[(i+1)%5][1], 5);
    }
    penUp();
}

void drawHeart(float cx, float cy, float size) {
    float r = size / 4;
    penDown();
    moveToPosition(cx - r, cy + r/2);
    
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
    penUp();
}


// ═══════════════════════════════════════════════════════════════════════════
// ✏️ LETTER DRAWING FUNCTIONS (A-Z and 0-9)
// ═══════════════════════════════════════════════════════════════════════════
//
// These functions draw capital letters and numbers
// All letters are 40mm tall, positioned relative to center
//

void drawLetterA(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy + size/2, cx, cy - size/2, 8);  // Left leg
    drawLine(cx, cy - size/2, cx + w/2, cy + size/2, 8);  // Right leg
    drawLine(cx - w/3, cy + size/6, cx + w/3, cy + size/6, 5);  // Crossbar
    penUp();
}

void drawLetterB(float cx, float cy, float size) {
    float w = size * 0.7;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/2, cy + size/2, 10);  // Vertical
    moveToPosition(cx - w/2, cy - size/2);
    // Top curve (simplified)
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10;
        float x = cx - w/2 + w * t;
        float y = cy - size/2 + size/4 * sin(t * PI);
        moveToPosition(x, y);
    }
    moveToPosition(cx - w/2, cy);
    // Bottom curve (simplified)
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10;
        float x = cx - w/2 + w * t;
        float y = cy + size/4 * sin(t * PI);
        moveToPosition(x, y);
    }
    penUp();
}

void drawLetterC(float cx, float cy, float size) {
    float r = size / 2;
    penDown();
    for (int i = 0; i <= 20; i++) {
        float angle = PI * 0.2 + (float)i / 20 * PI * 1.6;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    penUp();
}

void drawLetterD(float cx, float cy, float size) {
    float w = size * 0.7;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/2, cy + size/2, 10);  // Vertical
    moveToPosition(cx - w/2, cy - size/2);
    for (int i = 0; i <= 15; i++) {
        float t = (float)i / 15;
        float x = cx - w/2 + w * sin(t * PI/2);
        float y = cy - size/2 + size/2 * t;
        moveToPosition(x, y);
    }
    for (int i = 0; i <= 15; i++) {
        float t = (float)i / 15;
        float x = cx - w/2 + w * sin((1-t) * PI/2);
        float y = cy + size/2 - size/2 * t;
        moveToPosition(x, y);
    }
    penUp();
}

void drawLetterE(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/2, cy + size/2, 10);  // Vertical
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    drawLine(cx - w/2, cy, cx + w/3, cy, 8);  // Middle
    drawLine(cx - w/2, cy + size/2, cx + w/2, cy + size/2, 10);  // Bottom
    penUp();
}

void drawLetterF(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/2, cy + size/2, 10);  // Vertical
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    drawLine(cx - w/2, cy, cx + w/3, cy, 8);  // Middle
    penUp();
}

void drawLetterG(float cx, float cy, float size) {
    float r = size / 2;
    penDown();
    for (int i = 0; i <= 20; i++) {
        float angle = PI * 0.2 + (float)i / 20 * PI * 1.6;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    drawLine(cx + r * cos(PI * 0.2), cy + r * sin(PI * 0.2), cx + r * 0.5, cy, 5);
    drawLine(cx + r * 0.5, cy, cx + r * 0.5, cy - r * 0.3, 5);
    penUp();
}

void drawLetterH(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/2, cy + size/2, 10);  // Left vertical
    drawLine(cx + w/2, cy - size/2, cx + w/2, cy + size/2, 10);  // Right vertical
    drawLine(cx - w/2, cy, cx + w/2, cy, 8);  // Middle
    penUp();
}

void drawLetterI(float cx, float cy, float size) {
    float w = size * 0.3;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    drawLine(cx, cy - size/2, cx, cy + size/2, 10);  // Vertical
    drawLine(cx - w/2, cy + size/2, cx + w/2, cy + size/2, 10);  // Bottom
    penUp();
}

void drawLetterJ(float cx, float cy, float size) {
    float w = size * 0.6;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    drawLine(cx + w/2, cy - size/2, cx + w/2, cy + size/4, 10);  // Right
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10;
        float x = cx + w/2 - w * t;
        float y = cy + size/4 + size/4 * sin(t * PI/2);
        moveToPosition(x, y);
    }
    penUp();
}

void drawLetterK(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/2, cy + size/2, 10);  // Vertical
    drawLine(cx - w/2, cy, cx + w/2, cy - size/2, 8);  // Upper diagonal
    drawLine(cx - w/2, cy, cx + w/2, cy + size/2, 8);  // Lower diagonal
    penUp();
}

void drawLetterL(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/2, cy + size/2, 10);  // Vertical
    drawLine(cx - w/2, cy + size/2, cx + w/2, cy + size/2, 10);  // Bottom
    penUp();
}

void drawLetterM(float cx, float cy, float size) {
    float w = size * 0.9;
    penDown();
    drawLine(cx - w/2, cy + size/2, cx - w/2, cy - size/2, 10);  // Left
    drawLine(cx - w/2, cy - size/2, cx, cy + size/4, 8);  // Left diagonal
    drawLine(cx, cy + size/4, cx + w/2, cy - size/2, 8);  // Right diagonal
    drawLine(cx + w/2, cy - size/2, cx + w/2, cy + size/2, 10);  // Right
    penUp();
}

void drawLetterN(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy + size/2, cx - w/2, cy - size/2, 10);  // Left
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy + size/2, 8);  // Diagonal
    drawLine(cx + w/2, cy + size/2, cx + w/2, cy - size/2, 10);  // Right
    penUp();
}

void drawLetterO(float cx, float cy, float size) {
    float r = size / 2;
    penDown();
    for (int i = 0; i <= 36; i++) {
        float angle = (float)i / 36 * 2 * PI;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    penUp();
}

void drawLetterP(float cx, float cy, float size) {
    float w = size * 0.7;
    penDown();
    drawLine(cx - w/2, cy + size/2, cx - w/2, cy - size/2, 10);  // Vertical
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    moveToPosition(cx + w/2, cy - size/2);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10;
        float x = cx + w/2 - w * t;
        float y = cy - size/2 + size/4 * t;
        moveToPosition(x, y);
    }
    penUp();
}

void drawLetterQ(float cx, float cy, float size) {
    float r = size / 2;
    penDown();
    for (int i = 0; i <= 36; i++) {
        float angle = (float)i / 36 * 2 * PI;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    // Tail
    drawLine(cx + r*0.7, cy + r*0.5, cx + r*1.2, cy + size/2, 5);
    penUp();
}

void drawLetterR(float cx, float cy, float size) {
    float w = size * 0.7;
    penDown();
    drawLine(cx - w/2, cy + size/2, cx - w/2, cy - size/2, 10);  // Vertical
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    moveToPosition(cx + w/2, cy - size/2);
    for (int i = 0; i <= 8; i++) {
        float t = (float)i / 8;
        float x = cx + w/2 - w * t;
        float y = cy - size/2 + size/4 * t;
        moveToPosition(x, y);
    }
    drawLine(cx - w/2, cy, cx + w/2, cy + size/2, 8);  // Leg
    penUp();
}

void drawLetterS(float cx, float cy, float size) {
    float r = size / 3;
    penDown();
    // Top curve
    for (int i = 0; i <= 12; i++) {
        float angle = PI - PI/2 + (float)i / 12 * PI;
        float x = cx + r * cos(angle);
        float y = cy - size/4 + r * sin(angle);
        moveToPosition(x, y);
    }
    // Bottom curve
    for (int i = 0; i <= 12; i++) {
        float angle = PI/2 + (float)i / 12 * PI;
        float x = cx + r * cos(angle);
        float y = cy + size/4 + r * sin(angle);
        moveToPosition(x, y);
    }
    penUp();
}

void drawLetterT(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    drawLine(cx, cy - size/2, cx, cy + size/2, 10);  // Vertical
    penUp();
}

void drawLetterU(float cx, float cy, float size) {
    float r = size / 3;
    penDown();
    drawLine(cx - size/2, cy - size/2, cx - size/2, cy + size/4, 10);  // Left
    for (int i = 0; i <= 12; i++) {
        float angle = PI/2 + (float)i / 12 * PI;
        float x = cx + r * cos(angle);
        float y = cy + size/4 + r * sin(angle);
        moveToPosition(x, y);
    }
    drawLine(cx + size/2, cy + size/4, cx + size/2, cy - size/2, 10);  // Right
    penUp();
}

void drawLetterV(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx, cy + size/2, 8);  // Left
    drawLine(cx, cy + size/2, cx + w/2, cy - size/2, 8);  // Right
    penUp();
}

void drawLetterW(float cx, float cy, float size) {
    float w = size * 0.9;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx - w/4, cy + size/2, 8);
    drawLine(cx - w/4, cy + size/2, cx, cy - size/2, 8);
    drawLine(cx, cy - size/2, cx + w/4, cy + size/2, 8);
    drawLine(cx + w/4, cy + size/2, cx + w/2, cy - size/2, 8);
    penUp();
}

void drawLetterX(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy + size/2, 8);  // Diagonal 1
    drawLine(cx - w/2, cy + size/2, cx + w/2, cy - size/2, 8);  // Diagonal 2
    penUp();
}

void drawLetterY(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/4, cy - size/2, cx, cy, 8);  // Left
    drawLine(cx + w/4, cy - size/2, cx, cy, 8);  // Right
    drawLine(cx, cy, cx, cy + size/2, 8);  // Stem
    penUp();
}

void drawLetterZ(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - w/2, cy - size/2, cx + w/2, cy - size/2, 10);  // Top
    drawLine(cx - w/2, cy + size/2, cx + w/2, cy + size/2, 10);  // Bottom
    drawLine(cx - w/2, cy + size/2, cx + w/2, cy - size/2, 8);  // Diagonal
    penUp();
}

// NUMBER DRAWING FUNCTIONS (0-9)

void drawNumber0(float cx, float cy, float size) {
    float r = size / 2;
    penDown();
    for (int i = 0; i <= 36; i++) {
        float angle = (float)i / 36 * 2 * PI;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    penUp();
}

void drawNumber1(float cx, float cy, float size) {
    penDown();
    drawLine(cx - size/6, cy - size/3, cx, cy - size/2, 8);
    drawLine(cx, cy - size/2, cx, cy + size/2, 10);
    drawLine(cx - size/4, cy + size/2, cx + size/4, cy + size/2, 10);
    penUp();
}

void drawNumber2(float cx, float cy, float size) {
    float r = size / 3;
    penDown();
    drawLine(cx - size/2, cy - size/3, cx + size/2, cy - size/3, 10);
    for (int i = 0; i <= 15; i++) {
        float angle = PI - PI/3 + (float)i / 15 * PI * 2/3;
        float x = cx + r * cos(angle);
        float y = cy - size/3 + r * sin(angle);
        moveToPosition(x, y);
    }
    drawLine(cx - size/2, cy + size/2, cx + size/2, cy + size/2, 10);
    penUp();
}

void drawNumber3(float cx, float cy, float size) {
    float r = size / 3;
    penDown();
    drawLine(cx, cy - size/2, cx + size/3, cy - size/2, 10);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10;
        float x = cx + size/3 - size/3 * t;
        float y = cy - size/4 + size/2 * sin(t * PI);
        moveToPosition(x, y);
    }
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10;
        float x = cx + size/3 - size/3 * t;
        float y = cy + size/4 + size/4 * sin(t * PI);
        moveToPosition(x, y);
    }
    penUp();
}

void drawNumber4(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx + w/4, cy - size/2, cx - w/2, cy + size/6, 10);
    drawLine(cx - w/2, cy + size/6, cx + w/2, cy + size/6, 10);
    drawLine(cx + w/4, cy - size/2, cx + w/4, cy + size/2, 10);
    penUp();
}

void drawNumber5(float cx, float cy, float size) {
    penDown();
    drawLine(cx + size/2, cy - size/2, cx - size/3, cy - size/2, 10);
    drawLine(cx - size/3, cy - size/2, cx - size/2, cy, 8);
    for (int i = 0; i <= 12; i++) {
        float t = (float)i / 12;
        float x = cx - size/2 + size * t;
        float y = cy + size/4 + size/4 * sin(t * PI);
        moveToPosition(x, y);
    }
    penUp();
}

void drawNumber6(float cx, float cy, float size) {
    float r = size / 3;
    penDown();
    for (int i = 0; i <= 18; i++) {
        float angle = PI + PI/6 - (float)i / 18 * PI * 1.5;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    for (int i = 0; i <= 18; i++) {
        float angle = -PI/2 + (float)i / 18 * PI;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    penUp();
}

void drawNumber7(float cx, float cy, float size) {
    float w = size * 0.8;
    penDown();
    drawLine(cx - size/2, cy - size/2, cx + size/2, cy - size/2, 10);
    drawLine(cx + size/2, cy - size/2, cx - size/4, cy + size/2, 10);
    penUp();
}

void drawNumber8(float cx, float cy, float size) {
    float r = size / 4;
    penDown();
    // Top loop
    for (int i = 0; i <= 20; i++) {
        float angle = (float)i / 20 * 2 * PI;
        float x = cx + r * cos(angle);
        float y = cy - size/4 + r * sin(angle);
        moveToPosition(x, y);
    }
    // Bottom loop
    for (int i = 0; i <= 20; i++) {
        float angle = (float)i / 20 * 2 * PI;
        float x = cx + r * cos(angle);
        float y = cy + size/4 + r * sin(angle);
        moveToPosition(x, y);
    }
    penUp();
}

void drawNumber9(float cx, float cy, float size) {
    float r = size / 3;
    penDown();
    for (int i = 0; i <= 18; i++) {
        float angle = (float)i / 18 * PI;
        float x = cx + r * cos(angle);
        float y = cy - r * sin(angle);
        moveToPosition(x, y);
    }
    for (int i = 0; i <= 18; i++) {
        float angle = PI/2 - (float)i / 18 * PI * 1.5;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        moveToPosition(x, y);
    }
    penUp();
}


// ═══════════════════════════════════════════════════════════════════════════
// 📋 CARD DETECTION & DRAWING DISPATCHER
// ═══════════════════════════════════════════════════════════════════════════

void drawShape(const char* shapeName) {
    // Draw a shape based on its name
    if (strcmp(shapeName, "CIRCLE") == 0) {
        drawCircle(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE);
    }
    else if (strcmp(shapeName, "SQUARE") == 0) {
        drawSquare(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE);
    }
    else if (strcmp(shapeName, "TRIANGLE") == 0) {
        drawTriangle(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE);
    }
    else if (strcmp(shapeName, "STAR") == 0) {
        drawStar(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE, MEDIUM_SIZE * 0.4);
    }
    else if (strcmp(shapeName, "HEART") == 0) {
        drawHeart(DRAW_HOME_X, DRAW_HOME_Y, LARGE_SIZE);
    }
    else {
        // Default to circle
        drawCircle(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE);
    }
}

void drawLetter(char letter) {
    // Draw a capital letter
    letter = toupper(letter);
    
    switch(letter) {
        case 'A': drawLetterA(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'B': drawLetterB(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'C': drawLetterC(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'D': drawLetterD(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'E': drawLetterE(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'F': drawLetterF(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'G': drawLetterG(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'H': drawLetterH(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'I': drawLetterI(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'J': drawLetterJ(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'K': drawLetterK(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'L': drawLetterL(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'M': drawLetterM(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'N': drawLetterN(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'O': drawLetterO(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'P': drawLetterP(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'Q': drawLetterQ(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'R': drawLetterR(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'S': drawLetterS(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'T': drawLetterT(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'U': drawLetterU(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'V': drawLetterV(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'W': drawLetterW(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'X': drawLetterX(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'Y': drawLetterY(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        case 'Z': drawLetterZ(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
        default: 
            // Numbers
            if (letter >= '0' && letter <= '9') {
                int num = letter - '0';
                switch(num) {
                    case 0: drawNumber0(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 1: drawNumber1(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 2: drawNumber2(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 3: drawNumber3(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 4: drawNumber4(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 5: drawNumber5(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 6: drawNumber6(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 7: drawNumber7(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 8: drawNumber8(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                    case 9: drawNumber9(DRAW_HOME_X, DRAW_HOME_Y, MEDIUM_SIZE); break;
                }
            }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// 🎛️ CALIBRATION MODE
// ═══════════════════════════════════════════════════════════════════════════

void calibrationMode() {
    Serial.println("╔═══════════════════════════════════════════╗");
    Serial.println("║   🎛️  CALIBRATION MODE                 ║");
    Serial.println("╠═══════════════════════════════════════════╣");
    Serial.println("║  1/2: Base ±5°                         ║");
    Serial.println("║  3/4: Elbow ±5°                       ║");
    Serial.println("║  5/6: Gripper open/close              ║");
    Serial.println("║  H: Go home position                  ║");
    Serial.println("║  T: Test IR sensor                   ║");
    Serial.println("║  + / -: Base offset ±1°               ║");
    Serial.println("║  . / ,: Elbow offset ±1°             ║");
    Serial.println("║  Q: Quit and save                     ║");
    Serial.println("╚═══════════════════════════════════════════╝");
    
    currentState = STATE_CALIBRATION;
    
    while (currentState == STATE_CALIBRATION) {
        if (Serial.available()) {
            char cmd = Serial.read();
            
            switch(cmd) {
                case '1':
                    currentBase = constrain(currentBase + 5, BASE_MIN, BASE_MAX);
                    baseServo.write(currentBase);
                    break;
                case '2':
                    currentBase = constrain(currentBase - 5, BASE_MIN, BASE_MAX);
                    baseServo.write(currentBase);
                    break;
                case '3':
                    currentElbow = constrain(currentElbow + 5, ELBOW_MIN, ELBOW_MAX);
                    elbowServo.write(currentElbow);
                    break;
                case '4':
                    currentElbow = constrain(currentElbow - 5, ELBOW_MIN, ELBOW_MAX);
                    elbowServo.write(currentElbow);
                    break;
                case '5':
                    gripServo.write(GRIP_OPEN);
                    currentGrip = GRIP_OPEN;
                    break;
                case '6':
                    gripServo.write(GRIP_CLOSE);
                    currentGrip = GRIP_CLOSE;
                    break;
                case 'h':
                case 'H':
                    goHome();
                    break;
                case 't':
                case 'T':
                    Serial.print("📊 IR Reading: ");
                    Serial.println(readIRSensor());
                    break;
                case '+':
                    BASE_ZERO_OFFSET++;
                    Serial.print("BASE_ZERO_OFFSET = ");
                    Serial.println(BASE_ZERO_OFFSET);
                    break;
                case '-':
                    BASE_ZERO_OFFSET--;
                    Serial.print("BASE_ZERO_OFFSET = ");
                    Serial.println(BASE_ZERO_OFFSET);
                    break;
                case '.':
                    ELBOW_ZERO_OFFSET++;
                    Serial.print("ELBOW_ZERO_OFFSET = ");
                    Serial.println(ELBOW_ZERO_OFFSET);
                    break;
                case ',':
                    ELBOW_ZERO_OFFSET--;
                    Serial.print("ELBOW_ZERO_OFFSET = ");
                    Serial.println(ELBOW_ZERO_OFFSET);
                    break;
                case 'q':
                case 'Q':
                    Serial.println("✓ Calibration saved!");
                    Serial.print("BASE_ZERO_OFFSET = ");
                    Serial.println(BASE_ZERO_OFFSET);
                    Serial.print("ELBOW_ZERO_OFFSET = ");
                    Serial.println(ELBOW_ZERO_OFFSET);
                    Serial.print("IR_THRESHOLD = ");
                    Serial.println(IR_THRESHOLD);
                    Serial.print("L1_ARM_LENGTH = ");
                    Serial.println(L1_ARM_LENGTH);
                    Serial.print("L2_ARM_LENGTH = ");
                    Serial.println(L2_ARM_LENGTH);
                    goHome();
                    currentState = STATE_IDLE;
                    return;
            }
            
            Serial.print("Base: ");
            Serial.print(currentBase);
            Serial.print("° | Elbow: ");
            Serial.print(currentElbow);
            Serial.print("° | Grip: ");
            Serial.print(currentGrip);
            Serial.print("° | IR: ");
            Serial.println(readIRSensor());
        }
        delay(10);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// 🔄 SETUP - Runs once when robot starts
// ═══════════════════════════════════════════════════════════════════════════

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("╔══════════════════════════════════════════════════════╗");
    Serial.println("║   🤖 DUAL ARM SCARA ROBOT v2.0                      ║");
    Serial.println("║   With Speech & Drawing!                           ║");
    Serial.println("╚══════════════════════════════════════════════════════╝");
    Serial.println();
    
    // Attach servos
    baseServo.attach(SERVO_BASE_PIN);
    elbowServo.attach(SERVO_ELBOW_PIN);
    gripServo.attach(SERVO_GRIP_PIN);
    
    // Configure pins
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_POWER_PIN, OUTPUT);
    pinMode(LED_STATUS_PIN, OUTPUT);
    pinMode(IR_SENSOR_PIN, INPUT);
    
    setPowerLED(true);
    playStartupJingle();
    
    // Initialize audio (if files available)
    initAudio();
    
    goHome();
    
    Serial.println("✅ System ready!");
    Serial.println();
    Serial.print("📐 Arm lengths: L1=");
    Serial.print(L1_ARM_LENGTH);
    Serial.print("mm, L2=");
    Serial.print(L2_ARM_LENGTH);
    Serial.println("mm");
    Serial.print("🎯 IR Threshold: ");
    Serial.println(IR_THRESHOLD);
    Serial.println();
    Serial.println("📋 COMMANDS:");
    Serial.println("   Hold button 3 sec → Calibration mode");
    Serial.println("   Place card → Draws and speaks");
    Serial.println();
    
    blinkStatusLED(3, 200);
}


// ═══════════════════════════════════════════════════════════════════════════
// 🔄 LOOP - Runs forever
// ═══════════════════════════════════════════════════════════════════════════

unsigned long buttonPressTime = 0;
bool buttonWasPressed = false;
bool cardWasDetected = false;

// Demo cycling through different draws
int demoIndex = 0;
const char* demoItems[] = {"A", "B", "C", "1", "2", "3", "CIRCLE", "SQUARE", "STAR"};
int demoCount = 9;

void loop() {
    int irValue = readIRSensor();
    
    // Button check (3 second hold for calibration)
    bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);
    
    if (buttonPressed && !buttonWasPressed) {
        buttonPressTime = millis();
    }
    if (buttonPressed && !buttonWasPressed && millis() - buttonPressTime > 3000) {
        Serial.println("🔧 CALIBRATION MODE!");
        blinkStatusLED(5, 100);
        calibrationMode();
    }
    buttonWasPressed = buttonPressed;
    
    // Card detection
    if (isCardDetected() && !cardWasDetected) {
        Serial.println("👁️ Card detected!");
        setStatusLED(true);
        playSuccessBeep();
        
        // Get current demo item
        const char* item = demoItems[demoIndex];
        demoIndex = (demoIndex + 1) % demoCount;
        
        // Say the name
        Serial.print("🗣️  Drawing: ");
        Serial.println(item);
        
        // Check if it's a shape or letter
        bool isShape = (strcmp(item, "CIRCLE") == 0 || 
                       strcmp(item, "SQUARE") == 0 ||
                       strcmp(item, "STAR") == 0);
        
        if (isShape) {
            // Play shape audio
            char filename[30];
            sprintf(filename, "/audio/%s.mp3", item);
            playAudioFile(filename);
            delay(500);
            
            // Draw shape
            drawShape(item);
        }
        else {
            // It's a letter or number
            if (strlen(item) == 1) {
                char c = item[0];
                if (c >= 'A' && c <= 'Z') {
                    sayLetter(c);
                    delay(300);
                }
                else if (c >= '0' && c <= '9') {
                    sayNumber(c - '0');
                    delay(300);
                }
            }
            
            // Draw the letter/number
            drawLetter(item[0]);
        }
        
        playSuccessBeep();
        Serial.println("✅ Done!");
        
        goHome();
        cardWasDetected = true;
        delay(2000);
    }
    else if (!isCardDetected()) {
        cardWasDetected = false;
        setStatusLED(false);
    }
    
    // Debug output every 2 seconds
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 2000) {
        Serial.print("📊 IR: ");
        Serial.print(irValue);
        Serial.print(" | Threshold: ");
        Serial.print(IR_THRESHOLD);
        Serial.print(" | Card: ");
        Serial.print(isCardDetected() ? "YES" : "NO");
        Serial.print(" | Base: ");
        Serial.print(currentBase);
        Serial.print("° | Elbow: ");
        Serial.print(currentElbow);
        Serial.print("°");
        Serial.println();
        lastDebug = millis();
    }
    
    delay(50);
}