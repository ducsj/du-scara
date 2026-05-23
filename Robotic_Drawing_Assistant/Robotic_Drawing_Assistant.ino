/*
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║         🤖 MY ROBOT - CARD READER DRAWING ASSISTANT 🤖                       ║
║                                                                              ║
║   Hardware: ESP32-C3 + 3x SG90 Servo + IR Sensor + Audio                     ║
║                                                                              ║
║   What it does:                                                              ║
║   1. Detect card placed in front (IR sensor)                                  ║
║   2. Say the card name (letters, numbers, words, drawing names)              ║
║   3. Draw the shape on paper                                                  ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

══════════════════════════════════════════════════════════════════════════════
📦 STEP 1: LIBRARIES - Tell Arduino which extra code we need
══════════════════════════════════════════════════════════════════════════════
*/
#include <ESP32Servo.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
// ↑ AUDIO LIBRARIES for MAX98357 I2S amplifier
// Install: ESP8266Audio by Earle F. Philhower


/*
══════════════════════════════════════════════════════════════════════════════
🔌 STEP 2: PIN CONNECTIONS - Where each wire goes
══════════════════════════════════════════════════════════════════════════════

ESP32-C3 PIN LAYOUT:

    ┌─────────────────────┐
    │  ┌───┐          USB │   ← Your computer
    │  │   │              │
    └──┴───┴──────────────┘
         │
    [ESP32-C3 CHIP]

PIN ASSIGNMENTS:
   GPIO 2  → Servo Base (moves arm left/right)
   GPIO 3  → Servo Elbow (bends the arm)
   GPIO 4  → Servo Gripper (opens/closes claw)
   GPIO 5  → Button (hold 3 sec = calibration mode)
   GPIO 6  → Power LED (always on when powered)
   GPIO 7  → Status LED (blinks, shows what robot is doing)
   GPIO 20 → IR Sensor (detects cards)
   GPIO 21 → Audio LRC/WS (sound chip)
   GPIO 22 → Audio BCLK (sound chip)
   GPIO 18 → Audio DIN (sound chip)

*/

#define SERVO_BASE_PIN     2    // ← Base servo: rotates arm left/right
#define SERVO_ELBOW_PIN    3    // ← Elbow servo: bends the arm
#define SERVO_GRIP_PIN     4    // ← Gripper servo: opens/closes claw
#define BUTTON_PIN         5    // ← Button to enter calibration
#define LED_POWER_PIN      6    // ← Power indicator LED
#define LED_STATUS_PIN     7    // ← Status LED (blinks)
#define IR_SENSOR_PIN      20   // ← IR sensor for card detection

// Audio pins for MAX98357 I2S amplifier
#define AUDIO_LRC_PIN      21   // ← Left/Right Clock
#define AUDIO_BCLK_PIN     22   // ← Bit Clock
#define AUDIO_DIN_PIN      18   // ← Data In


/*
══════════════════════════════════════════════════════════════════════════════
⚙️ STEP 3: CALIBRATION VALUES - TUNE THESE AFTER TESTING
══════════════════════════════════════════════════════════════════════════════
*/

int BASE_ZERO_OFFSET   = 0;     // ← Your base servo offset (change after testing)
int ELBOW_ZERO_OFFSET  = 0;     // ← Your elbow servo offset (change after testing)

#define IR_THRESHOLD      500    // ← IR value for card detection (tune this!)

#define GRIP_OPEN_ANGLE   90     // ← Gripper when open (holding nothing)
#define GRIP_CLOSE_ANGLE   0      // ← Gripper when closed (holding pen)


/*
══════════════════════════════════════════════════════════════════════════════
📐 STEP 4: ARM PHYSICAL LIMITS
══════════════════════════════════════════════════════════════════════════════
*/

#define L1_ARM_LENGTH     80    // ← Upper arm length in mm (MEASURE YOURS!)
#define L2_ARM_LENGTH     70    // ← Forearm length in mm (MEASURE YOURS!)

// Safe angles (prevents damaging servos by going too far)
#define BASE_MIN    20          // ← Don't go below this
#define BASE_MAX    160         // ← Don't go above this
#define ELBOW_MIN   30          // ← Don't go below this
#define ELBOW_MAX   150         // ← Don't go above this

#define SERVO_SPEED 30          // ← Movement speed (lower = slower)


/*
══════════════════════════════════════════════════════════════════════════════
🦾 STEP 5: CREATE SERVO OBJECTS
══════════════════════════════════════════════════════════════════════════════
*/

Servo baseServo;      // ← Name for base rotation servo
Servo elbowServo;     // ← Name for elbow bending servo
Servo gripServo;       // ← Name for gripper claw servo

// Current angles (where each servo is right now)
int currentBase   = 90;   // Arm rotation (degrees)
int currentElbow  = 90;   // Elbow bend (degrees)
int currentGrip   = 90;   // Gripper state (degrees)


/*
══════════════════════════════════════════════════════════════════════════════
🔊 STEP 6: AUDIO SYSTEM - Make the robot SPEAK!
══════════════════════════════════════════════════════════════════════════════

AUDIO FILES YOU NEED TO CREATE (MP3 format, save in SPIFFS):

ALPHABET (A-Z):
  /audio/A.mp3, /audio/B.mp3, /audio/C.mp3 ... /audio/Z.mp3

NUMBERS (0-9):
  /audio/0.mp3, /audio/1.mp3, /audio/2.mp3 ... /audio/9.mp3

DRAWING NAMES:
  /audio/circle.mp3      → "Circle"
  /audio/square.mp3      → "Square"
  /audio/triangle.mp3    → "Triangle"
  /audio/star.mp3        → "Star"
  /audio/heart.mp3       → "Heart"
  /audio/cross.mp3       → "Cross"
  /audio/diamond.mp3     → "Diamond"

WORDS:
  /audio/draw.mp3        → "Draw"
  /audio/drawing.mp3     → "Drawing"
  /audio/ready.mp3       → "Ready"
  /audio/start.mp3       → "Start"
  /audio/done.mp3        → "Done"
  /audio/pickup.mp3      → "Pick up your drawing"
  /audio/place.mp3       → "Place card in front"
  /audio/error.mp3       → "Error"

EXAMPLE SENTENCES:
  /audio/hello.mp3       → "Hello! Let's draw!"
  /audio/circle_start.mp3 → "Drawing circle"
  /audio/square_start.mp3 → "Drawing square"
*/

// Audio objects
AudioFileSourceSPIFFS *audioFile = nullptr;
AudioGeneratorMP3 *audioMP3 = nullptr;
AudioOutputI2S *audioOut = nullptr;
bool audioInitialized = false;

// Sound effects (simple beeps)
void playBeep(int frequency, int durationMs) {
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

void playErrorBeep() {
    playBeep(220, 300);
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

// Initialize audio system for MAX98357
void initAudio() {
    Serial.println("🔊 Initializing audio system...");
    
    audioOut = new AudioOutputI2S();
    audioOut->setPinout(AUDIO_BCLK_PIN, AUDIO_DIN_PIN, AUDIO_LRC_PIN);
    audioOut->setGain(1.0);  // Volume (0.0 to 1.0)
    
    audioInitialized = true;
    Serial.println("✅ Audio ready!");
}

// Play an MP3 file from SPIFFS
bool playAudioFile(const char* filename) {
    if (!audioInitialized) {
        Serial.println("⚠️ Audio not initialized!");
        return false;
    }
    
    Serial.print("🔊 Playing: ");
    Serial.println(filename);
    
    // Close any previous file
    if (audioFile) {
        delete audioFile;
        audioFile = nullptr;
    }
    if (audioMP3) {
        delete audioMP3;
        audioMP3 = nullptr;
    }
    
    // Open audio file
    audioFile = new AudioFileSourceSPIFFS(filename);
    
    if (!audioFile->isOpen()) {
        Serial.print("❌ File not found: ");
        Serial.println(filename);
        playBeep(220, 200);  // Error beep
        return false;
    }
    
    // Create MP3 decoder and play
    audioMP3 = new AudioGeneratorMP3();
    
    if (audioMP3->begin(audioFile, audioOut)) {
        // Play until done
        while (audioMP3->isRunning()) {
            audioMP3->loop();
            delay(10);
        }
        audioMP3->stop();
        return true;
    }
    
    Serial.println("❌ Audio playback failed!");
    return false;
}

// Say a single letter (A-Z)
void sayLetter(char letter) {
    char filename[20];
    letter = toupper(letter);
    sprintf(filename, "/audio/%c.mp3", letter);
    playAudioFile(filename);
}

// Say a number (0-9)
void sayNumber(int num) {
    if (num < 0 || num > 9) return;
    char filename[20];
    sprintf(filename, "/audio/%d.mp3", num);
    playAudioFile(filename);
}

// Say a word (pre-recorded)
void sayWord(const char* word) {
    char filename[30];
    sprintf(filename, "/audio/%s.mp3", word);
    playAudioFile(filename);
}

// Spell out a word letter by letter
void spellWord(const char* word) {
    Serial.print("🔤 Spelling: ");
    Serial.println(word);
    
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        char c = word[i];
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        
        if (c >= 'A' && c <= 'Z') {
            Serial.print(c);
            Serial.print(" ");
            sayLetter(c);
            delay(200);
        }
        else if (c == ' ') {
            delay(500);  // Pause for space
            Serial.print(" ");
        }
        else if (c >= '0' && c <= '9') {
            Serial.print(c);
            sayNumber(c - '0');
            delay(200);
        }
    }
    Serial.println();
}

// Say a full sentence using multiple audio files
void saySentence(const char* files[], int count) {
    for (int i = 0; i < count; i++) {
        playAudioFile(files[i]);
        delay(300);
    }
}


/*
══════════════════════════════════════════════════════════════════════════════
🦾 STEP 7: SERVO MOVEMENT FUNCTIONS
══════════════════════════════════════════════════════════════════════════════
*/

void moveArm(int baseAngle, int elbowAngle) {
    baseAngle  += BASE_ZERO_OFFSET;
    elbowAngle += ELBOW_ZERO_OFFSET;

    baseAngle  = constrain(baseAngle,  BASE_MIN,  BASE_MAX);
    elbowAngle = constrain(elbowAngle, ELBOW_MIN, ELBOW_MAX);

    while (currentBase != baseAngle || currentElbow != elbowAngle) {

        if (currentBase < baseAngle) {
            currentBase = min(currentBase + 2, baseAngle);
            baseServo.write(currentBase);
        } else if (currentBase > baseAngle) {
            currentBase = max(currentBase - 2, baseAngle);
            baseServo.write(currentBase);
        }

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
    moveArm(90, 90);
    gripServo.write(GRIP_OPEN_ANGLE);
    currentGrip = GRIP_OPEN_ANGLE;
}


/*
══════════════════════════════════════════════════════════════════════════════
📐 STEP 8: MATH - How the robot knows WHERE to move
══════════════════════════════════════════════════════════════════════════════
*/

bool inverseKinematics(float targetX, float targetY, float &theta1, float &theta2) {
    float D = sqrt(targetX * targetX + targetY * targetY);
    float maxReach = L1_ARM_LENGTH + L2_ARM_LENGTH;
    float minReach = abs(L1_ARM_LENGTH - L2_ARM_LENGTH);

    if (D > maxReach || D < minReach) {
        Serial.println("ERROR: Target out of reach!");
        return false;
    }

    float cosTheta2 = (D * D - L1_ARM_LENGTH * L1_ARM_LENGTH - L2_ARM_LENGTH * L2_ARM_LENGTH)
                      / (2 * L1_ARM_LENGTH * L2_ARM_LENGTH);
    cosTheta2 = constrain(cosTheta2, -1.0, 1.0);
    theta2 = acos(cosTheta2);

    float psi = atan2(targetY, targetX);
    float beta = asin(constrain(L2_ARM_LENGTH * sin(theta2) / D, -1.0, 1.0));
    theta1 = psi - beta;

    theta1 = degrees(theta1);
    theta2 = degrees(theta2);

    return true;
}

bool moveToPosition(float x, float y) {
    float t1, t2;
    if (inverseKinematics(x, y, t1, t2)) {
        moveArm((int)t1, (int)t2);
        return true;
    }
    return false;
}


/*
══════════════════════════════════════════════════════════════════════════════
👁️ STEP 9: IR SENSOR - Detecting cards
══════════════════════════════════════════════════════════════════════════════
*/

int readIRSensor() {
    return analogRead(IR_SENSOR_PIN);
}

bool isCardDetected() {
    return readIRSensor() > IR_THRESHOLD;
}


/*
══════════════════════════════════════════════════════════════════════════════
💡 STEP 10: LED FUNCTIONS
══════════════════════════════════════════════════════════════════════════════
*/

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


/*
══════════════════════════════════════════════════════════════════════════════
🎨 STEP 11: DRAWING FUNCTIONS
══════════════════════════════════════════════════════════════════════════════
*/

void drawLine(float x1, float y1, float x2, float y2, int segments = 10) {
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
    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);

    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2 * PI;
        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);
        moveToPosition(x, y);
        delay(30);
    }

    gripServo.write(GRIP_OPEN_ANGLE);
}

void drawSquare(float cx, float cy, float size) {
    float half = size / 2;

    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);

    drawLine(cx - half, cy - half, cx + half, cy - half, 10);
    drawLine(cx + half, cy - half, cx + half, cy + half, 10);
    drawLine(cx + half, cy + half, cx - half, cy + half, 10);
    drawLine(cx - half, cy + half, cx - half, cy - half, 10);

    gripServo.write(GRIP_OPEN_ANGLE);
}

void drawStar(float cx, float cy, float outerR, float innerR) {
    float points[5][2];
    float innerPoints[5][2];

    for (int i = 0; i < 5; i++) {
        float angle = -PI/2 + i * 2 * PI / 5;
        points[i][0] = cx + outerR * cos(angle);
        points[i][1] = cy + outerR * sin(angle);

        float innerAngle = -PI/2 + (i * 2 + 1) * PI / 5;
        innerPoints[i][0] = cx + innerR * cos(innerAngle);
        innerPoints[i][1] = cy + innerR * sin(innerAngle);
    }

    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);

    moveToPosition(points[0][0], points[0][1]);
    for (int i = 0; i < 5; i++) {
        drawLine(points[i][0], points[i][1], innerPoints[i][0], innerPoints[i][1], 5);
        drawLine(innerPoints[i][0], innerPoints[i][1],
                 points[(i+1)%5][0], points[(i+1)%5][1], 5);
    }

    gripServo.write(GRIP_OPEN_ANGLE);
}

void drawTriangle(float cx, float cy, float size) {
    float h = size * sqrt(3) / 2;

    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);

    moveToPosition(cx, cy - size/2);
    drawLine(cx, cy - size/2, cx - size/2, cy + h/3, 10);
    drawLine(cx - size/2, cy + h/3, cx + size/2, cy + h/3, 10);
    drawLine(cx + size/2, cy + h/3, cx, cy - size/2, 10);

    gripServo.write(GRIP_OPEN_ANGLE);
}

void drawHeart(float cx, float cy, float size) {
    float r = size / 4;

    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);

    moveToPosition(cx - r, cy);
    
    // Left curve
    for (int i = 0; i <= 20; i++) {
        float angle = PI + (float)i / 20 * PI;
        float x = cx - r + r * cos(angle);
        float y = cy - r + r * sin(angle) + r*0.5;
        moveToPosition(x, y);
    }
    
    // Right curve
    for (int i = 0; i <= 20; i++) {
        float angle = (float)i / 20 * PI;
        float x = cx + r + r * cos(angle);
        float y = cy - r + r * sin(angle) + r*0.5;
        moveToPosition(x, y);
    }

    gripServo.write(GRIP_OPEN_ANGLE);
}

void drawDiamond(float cx, float cy, float size) {
    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);

    moveToPosition(cx, cy - size/2);      // Top
    drawLine(cx, cy - size/2, cx + size/2, cy, 10);     // Top to Right
    drawLine(cx + size/2, cy, cx, cy + size/2, 10);     // Right to Bottom
    drawLine(cx, cy + size/2, cx - size/2, cy, 10);     // Bottom to Left
    drawLine(cx - size/2, cy, cx, cy - size/2, 10);     // Left to Top

    gripServo.write(GRIP_OPEN_ANGLE);
}

void drawCross(float cx, float cy, float size) {
    float half = size / 2;
    float thick = size / 4;

    gripServo.write(GRIP_CLOSE_ANGLE);
    delay(300);

    // Vertical line
    moveToPosition(cx - thick/2, cy - half);
    drawLine(cx - thick/2, cy - half, cx - thick/2, cy + half, 10);
    drawLine(cx - thick/2, cy + half, cx + thick/2, cy + half, 10);
    drawLine(cx + thick/2, cy + half, cx + thick/2, cy - half, 10);
    drawLine(cx + thick/2, cy - half, cx - thick/2, cy - half, 10);
    
    // Horizontal line
    moveToPosition(cx - half, cy + thick/2);
    drawLine(cx - half, cy + thick/2, cx + half, cy + thick/2, 10);
    drawLine(cx + half, cy + thick/2, cx + half, cy - thick/2, 10);
    drawLine(cx + half, cy - thick/2, cx - half, cy - thick/2, 10);
    drawLine(cx - half, cy - thick/2, cx - half, cy + thick/2, 10);

    gripServo.write(GRIP_OPEN_ANGLE);
}


/*
══════════════════════════════════════════════════════════════════════════════
🎛️ STEP 12: CALIBRATION MODE
══════════════════════════════════════════════════════════════════════════════
*/

void calibrationMode() {
    Serial.println("╔════════════════════════════════════╗");
    Serial.println("║     CALIBRATION MODE               ║");
    Serial.println("╠════════════════════════════════════╣");
    Serial.println("║  1/2: Base ±5°                    ║");
    Serial.println("║  3/4: Elbow ±5°                   ║");
    Serial.println("║  5/6: Gripper open/close          ║");
    Serial.println("║  H: Home position                 ║");
    Serial.println("║  + / -: Base offset ±1°           ║");
    Serial.println("║  . / ,: Elbow offset ±1°           ║");
    Serial.println("║  Q: Quit and save                 ║");
    Serial.println("╚════════════════════════════════════╝");

    while (true) {
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
                    gripServo.write(GRIP_OPEN_ANGLE);
                    currentGrip = GRIP_OPEN_ANGLE;
                    break;
                case '6':
                    gripServo.write(GRIP_CLOSE_ANGLE);
                    currentGrip = GRIP_CLOSE_ANGLE;
                    break;
                case 'h':
                case 'H':
                    goHome();
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
                    goHome();
                    return;
            }

            Serial.print("Base: ");
            Serial.print(currentBase);
            Serial.print(" | Elbow: ");
            Serial.print(currentElbow);
            Serial.print(" | IR: ");
            Serial.println(readIRSensor());
        }
        delay(10);
    }
}


/*
══════════════════════════════════════════════════════════════════════════════
🎴 STEP 13: CARD DETECTION & RESPONSE
══════════════════════════════════════════════════════════════════════════════

YOUR CARDS SYSTEM:
This maps card types to what the robot says and draws.

CARD TYPES:
- ALPHABET: A-Z (says letter, spells it)
- NUMBERS: 0-9 (says number)
- SHAPES: circle, square, triangle, star, heart, cross, diamond
- WORDS: any text (spells it out)
*/

// Card type enumeration
enum CardType {
    CARD_NONE,
    CARD_ALPHABET,
    CARD_NUMBER,
    CARD_SHAPE,
    CARD_WORD
};

// Card data structure
struct Card {
    CardType type;
    String name;        // What to display
    String audioFile;    // Audio file to play
};

// Drawing shapes
enum Shape {
    SHAPE_CIRCLE,
    SHAPE_SQUARE,
    SHAPE_TRIANGLE,
    SHAPE_STAR,
    SHAPE_HEART,
    SHAPE_CROSS,
    SHAPE_DIAMOND
};

// Map IR sensor zones to cards (simplified - detect by position)
Card detectCard() {
    int irValue = readIRSensor();
    
    // For demo: If card detected, cycle through shapes
    static int currentShape = 0;
    
    if (isCardDetected()) {
        // Determine shape based on position or cycle
        Shape shapes[] = {SHAPE_CIRCLE, SHAPE_SQUARE, SHAPE_TRIANGLE, SHAPE_STAR, SHAPE_HEART};
        
        Card card;
        card.type = CARD_SHAPE;
        
        switch(shapes[currentShape]) {
            case SHAPE_CIRCLE:
                card.name = "CIRCLE";
                card.audioFile = "/audio/circle.mp3";
                break;
            case SHAPE_SQUARE:
                card.name = "SQUARE";
                card.audioFile = "/audio/square.mp3";
                break;
            case SHAPE_TRIANGLE:
                card.name = "TRIANGLE";
                card.audioFile = "/audio/triangle.mp3";
                break;
            case SHAPE_STAR:
                card.name = "STAR";
                card.audioFile = "/audio/star.mp3";
                break;
            case SHAPE_HEART:
                card.name = "HEART";
                card.audioFile = "/audio/heart.mp3";
                break;
            default:
                card.name = "CIRCLE";
                card.audioFile = "/audio/circle.mp3";
        }
        
        currentShape = (currentShape + 1) % 5;
        return card;
    }
    
    Card empty;
    empty.type = CARD_NONE;
    empty.name = "";
    empty.audioFile = "";
    return empty;
}

// Say the card name with pronunciation
void sayCardName(Card card) {
    Serial.print("🗣️  Saying: ");
    Serial.println(card.name);
    
    switch(card.type) {
        case CARD_SHAPE:
            // Play "Drawing [shape]"
            const char* drawing[] = {"/audio/drawing.mp3", card.audioFile.c_str()};
            saySentence(drawing, 2);
            break;
            
        case CARD_ALPHABET:
            // Spell letter: "A is for Apple" or just say "A"
            sayWord(card.audioFile.c_str());
            break;
            
        case CARD_NUMBER:
            // Say number: "Number 5" or just "5"
            sayWord(card.audioFile.c_str());
            break;
            
        case CARD_WORD:
            // Spell the word letter by letter
            spellWord(card.name.c_str());
            break;
            
        default:
            playBeep(440, 200);
    }
    
    delay(500);
}

// Draw the shape that matches the card
void drawCardShape(Card card) {
    if (card.type != CARD_SHAPE) return;
    
    Serial.print("🎨 Drawing: ");
    Serial.println(card.name);
    
    if (card.name == "CIRCLE") {
        moveArm(90, 60);
        delay(500);
        drawCircle(50, 50, 30);
    }
    else if (card.name == "SQUARE") {
        moveArm(120, 70);
        delay(500);
        drawSquare(70, 60, 25);
    }
    else if (card.name == "TRIANGLE") {
        moveArm(70, 65);
        delay(500);
        drawTriangle(40, 50, 30);
    }
    else if (card.name == "STAR") {
        moveArm(90, 50);
        delay(500);
        drawStar(50, 50, 25, 10);
    }
    else if (card.name == "HEART") {
        moveArm(100, 55);
        delay(500);
        drawHeart(50, 50, 40);
    }
    
    delay(500);
}


/*
══════════════════════════════════════════════════════════════════════════════
🔄 STEP 14: MAIN PROGRAM
══════════════════════════════════════════════════════════════════════════════
*/

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("╔══════════════════════════════════════════════════╗");
    Serial.println("║         MY ROBOT - DRAWING ASSISTANT             ║");
    Serial.println("║         with SPEECH & PRONUNCIATION              ║");
    Serial.println("╚══════════════════════════════════════════════════╝");
    Serial.println();
    Serial.println("🔊 AUDIO FILES NEEDED (in SPIFFS /audio/ folder):");
    Serial.println("   - /audio/circle.mp3, /audio/square.mp3, etc.");
    Serial.println("   - /audio/drawing.mp3");
    Serial.println("   - /audio/A.mp3 through /audio/Z.mp3");
    Serial.println("   - /audio/0.mp3 through /audio/9.mp3");
    Serial.println();

    // Attach servos
    baseServo.attach(SERVO_BASE_PIN);
    elbowServo.attach(SERVO_ELBOW_PIN);
    gripServo.attach(SERVO_GRIP_PIN);

    // Set pin modes
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_POWER_PIN, OUTPUT);
    pinMode(LED_STATUS_PIN, OUTPUT);
    pinMode(IR_SENSOR_PIN, INPUT);

    setPowerLED(true);
    playStartupJingle();

    // Initialize audio system
    initAudio();

    goHome();

    Serial.println("✅ System ready!");
    Serial.println();
    Serial.println("📋 HOW TO USE:");
    Serial.println("   1. Place paper under the gripper");
    Serial.println("   2. Attach pen/pencil to gripper");
    Serial.println("   3. Place card in front of IR sensor");
    Serial.println("   4. Robot will SAY the name, then DRAW!");
    Serial.println();
    Serial.println("🔧 HOLD BUTTON 3 SECONDS for calibration mode");
    Serial.println("📊 IR Threshold: " + String(IR_THRESHOLD));

    blinkStatusLED(3, 200);
}

void loop() {
    int irValue = readIRSensor();

    // Button check (hold 3 sec for calibration)
    static unsigned long buttonPressTime = 0;
    static bool buttonWasPressed = false;
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

    // Check for card
    static bool cardWasDetected = false;
    if (isCardDetected() && !cardWasDetected) {
        Serial.println("👁️ Card detected! IR value: " + String(irValue));
        setStatusLED(true);
        
        // Detect what card it is
        Card card = detectCard();
        
        // STEP 1: Say the card name
        sayCardName(card);
        
        // STEP 2: Draw the shape
        drawCardShape(card);
        
        // STEP 3: Say "Done"
        playSuccessBeep();
        sayWord("/audio/done.mp3");
        sayWord("/audio/pickup.mp3");
        
        Serial.println("✅ DONE! Pick up your drawing!");
        
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
        Serial.print(" | Card: ");
        Serial.print(isCardDetected() ? "YES" : "NO");
        Serial.print(" | Base: ");
        Serial.print(currentBase);
        Serial.print("° | Elbow: ");
        Serial.print(currentElbow);
        Serial.println("°");
        lastDebug = millis();
    }

    delay(50);
}