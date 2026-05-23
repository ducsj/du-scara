/*
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║       🤖 3-SERVO SCARA ROBOT - SIMPLIFIED VERSION 🤖                       ║
║                                                                              ║
║   SERVO CONFIGURATION:                                                       ║
║   ├── SERVO 1 (GPIO 2): BASE - Rotates entire arm left/right               ║
║   ├── SERVO 2 (GPIO 3): ELBOW - Bends the arm (forearm)                    ║
║   └── SERVO 3 (GPIO 4): PEN   - Lifts pen up/down for drawing              ║
║                                                                              ║
║   WHAT IT DOES:                                                              ║
║   1. Detect card with IR sensor                                             ║
║   2. Draw shapes/letters/numbers on paper                                   ║
║   3. Speak the name of what's being drawn                                    ║
║                                                                              ║
║   HARDWARE: ESP32-C3 + 3x SG90 Servo + IR Sensor + MAX98357 Audio          ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

══════════════════════════════════════════════════════════════════════════════
📖 READ THIS FIRST - HOW THE ROBOT WORKS
══════════════════════════════════════════════════════════════════════════════

YOUR ROBOT STRUCTURE:

         PEN TIP (where drawing happens)
              │
              │ ← PEN SERVO lifts up/down
              │
         ┌────┴────┐
         │ ELBOW   │ ← ELBOW SERVO (bends forearm)
         └────┬────┘
              │
         ┌────┴────┐
         │ UPPER   │ ← BASE SERVO rotates this
         │  ARM    │
         └────┬────┘
              │
         ╔════╧════╗
         ║   BASE   ║ ← Mounts on base plate
         ╚═════════╝

MOVEMENT EXPLANATION:

1. BASE SERVO (Rotates arm left/right):
   - 0°   = Arm points LEFT
   - 90°  = Arm points UP
   - 180° = Arm points RIGHT
   
2. ELBOW SERVO (Bends forearm):
   - 0°   = Forearm folded back (compact)
   - 90°  = Forearm horizontal (extended)
   - 180° = Forearm folded forward
   
3. PEN SERVO (Lifts pen):
   - PEN_UP   = Pen is off paper (no drawing)
   - PEN_DOWN = Pen touches paper (drawing)

══════════════════════════════════════════════════════════════════════════════
🔌 PIN CONNECTIONS
══════════════════════════════════════════════════════════════════════════════

ESP32-C3 PIN    COMPONENT         DESCRIPTION
────────────    ─────────         ────────────
GPIO 2          SERVO 1 (BASE)   Base rotation servo
GPIO 3          SERVO 2 (ELBOW)  Elbow bend servo
GPIO 4          SERVO 3 (PEN)    Pen lift servo
GPIO 5          BUTTON           Calibration mode button
GPIO 6          LED (POWER)     Power indicator
GPIO 7          LED (STATUS)     Status indicator
GPIO 20         IR SENSOR        Card detection
GPIO 18         MAX98357 DIN     Audio data
GPIO 21         MAX98357 LRC     Audio word select
GPIO 22         MAX98357 BCLK    Audio clock

══════════════════════════════════════════════════════════════════════════════
⚙️ CALIBRATION SETTINGS - CONFIGURE YOUR ROBOT HERE
══════════════════════════════════════════════════════════════════════════════

⚠️ IMPORTANT: Update these values after running calibration!

*/

// ═══════════════════════════════════════════════════════════════════════════
// 🎯 SERVO CALIBRATION (Find these using calibration mode)
// ═══════════════════════════════════════════════════════════════════════════

// BASE SERVO OFFSET
// If arm doesn't point straight when at 90°, adjust this:
//   Positive value = arm moves LEFT
//   Negative value = arm moves RIGHT
int BASE_OFFSET = 0;    // Default: 0 (adjust after testing)

// ELBOW SERVO OFFSET
// If elbow doesn't sit at 90° when flat, adjust this:
//   Positive value = elbow bends DOWN
//   Negative value = elbow bends UP
int ELBOW_OFFSET = 0;   // Default: 0 (adjust after testing)

// ═══════════════════════════════════════════════════════════════════════════
// 🚫 SERVO SAFE LIMITS (Prevent breaking servos!)
// ═══════════════════════════════════════════════════════════════════════════

// BASE SERVO limits (typically 20-160° for SG90)
#define BASE_MIN    20      // Don't go below this (prevents strain)
#define BASE_MAX    160     // Don't go above this (prevents strain)

// ELBOW SERVO limits (typically 30-150°)
#define ELBOW_MIN   30      // Don't go below this
#define ELBOW_MAX   150     // Don't go above this

// ═══════════════════════════════════════════════════════════════════════════
// ✏️ PEN SERVO POSITIONS
// ═══════════════════════════════════════════════════════════════════════════

// These are angles for the pen servo (adjust after testing)
#define PEN_UP      90      // Pen lifted off paper
#define PEN_DOWN    30      // Pen touching paper
#define PEN_HOLD    60      // Pen holding (for gripping)

// ═══════════════════════════════════════════════════════════════════════════
// 📐 ARM LENGTHS (Measure with ruler!)
// ═══════════════════════════════════════════════════════════════════════════

// Upper arm length (base to elbow) in mm
// MEASURE: From center of base servo to center of elbow servo
#define L1_LENGTH   80      // Default: 80mm (adjust to YOUR measurement)

// Forearm length (elbow to pen tip) in mm
// MEASURE: From center of elbow servo to pen tip
#define L2_LENGTH   70      // Default: 70mm (adjust to YOUR measurement)

// ═══════════════════════════════════════════════════════════════════════════
// 👁️ IR SENSOR THRESHOLD
// ═══════════════════════════════════════════════════════════════════════════

// IR value for card detection
// Read Serial Monitor to find your values:
//   No card: ~100-300
//   Card present: ~800-1023
// SET THIS: (no-card value + card value) / 2
#define IR_THRESHOLD   500   // Default: 500 (adjust after testing)

// ═══════════════════════════════════════════════════════════════════════════
// 🎨 DRAWING SETTINGS
// ═══════════════════════════════════════════════════════════════════════════

#define DRAW_HOME_X    50    // Center X for drawing
#define DRAW_HOME_Y    50    // Center Y for drawing
#define DRAW_SIZE      30    // Default size for drawings (mm)
#define SPEED_SERVO    30    // Movement speed (lower = slower)

// ═══════════════════════════════════════════════════════════════════════════
// 🦾 CREATE SERVO OBJECTS
// ═══════════════════════════════════════════════════════════════════════════

Servo servoBase;    // Base rotation (arm left/right)
Servo servoElbow;    // Elbow bend (forearm)
Servo servoPen;     // Pen lift (up/down)

// Current positions
int currentBase  = 90;
int currentElbow = 90;
int currentPen   = 90;

// ═══════════════════════════════════════════════════════════════════════════
// 🔊 AUDIO SYSTEM
// ═══════════════════════════════════════════════════════════════════════════

AudioFileSourceSPIFFS *audioFile = nullptr;
AudioGeneratorMP3 *audioMP3 = nullptr;
AudioOutputI2S *audioOut = nullptr;
bool audioReady = false;

void initAudio() {
    audioOut = new AudioOutputI2S();
    audioOut->setPinout(22, 18, 21);  // BCLK, DIN, LRC
    audioOut->setGain(1.0);
    audioReady = true;
    Serial.println("🔊 Audio ready!");
}

bool playAudio(const char* filename) {
    if (!audioReady) return false;
    
    if (audioFile) { delete audioFile; audioFile = nullptr; }
    if (audioMP3) { delete audioMP3; audioMP3 = nullptr; }
    
    audioFile = new AudioFileSourceSPIFFS(filename);
    if (!audioFile->isOpen()) {
        Serial.print("❌ Missing: ");
        Serial.println(filename);
        return false;
    }
    
    audioMP3 = new AudioGeneratorMP3();
    if (audioMP3->begin(audioFile, audioOut)) {
        while (audioMP3->isRunning()) { audioMP3->loop(); delay(10); }
        audioMP3->stop();
        return true;
    }
    return false;
}

void playBeep(int freq, int ms) {
    int half = 500000 / freq;
    int count = (ms * 1000) / (half * 2);
    for (int i = 0; i < count; i++) {
        digitalWrite(7, HIGH);
        delayMicroseconds(half);
        digitalWrite(7, LOW);
        delayMicroseconds(half);
    }
}

void beepSuccess() {
    playBeep(880, 100);
    delay(100);
    playBeep(1320, 200);
}

void spellWord(const char* word) {
    for (int i = 0; i < strlen(word); i++) {
        char c = word[i];
        if (c >= 'a' && c <= 'z') c -= 32;
        if (c >= 'A' && c <= 'Z') {
            char file[20];
            sprintf(file, "/audio/%c.mp3", c);
            playAudio(file);
            delay(200);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// 🦾 MOVEMENT FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void moveBase(int angle) {
    angle += BASE_OFFSET;
    angle = constrain(angle, BASE_MIN, BASE_MAX);
    
    if (angle < currentBase) {
        for (int p = currentBase; p >= angle; p--) {
            servoBase.write(p);
            currentBase = p;
            delay(SPEED_SERVO);
        }
    } else {
        for (int p = currentBase; p <= angle; p++) {
            servoBase.write(p);
            currentBase = p;
            delay(SPEED_SERVO);
        }
    }
}

void moveElbow(int angle) {
    angle += ELBOW_OFFSET;
    angle = constrain(angle, ELBOW_MIN, ELBOW_MAX);
    
    if (angle < currentElbow) {
        for (int p = currentElbow; p >= angle; p--) {
            servoElbow.write(p);
            currentElbow = p;
            delay(SPEED_SERVO);
        }
    } else {
        for (int p = currentElbow; p <= angle; p++) {
            servoElbow.write(p);
            currentElbow = p;
            delay(SPEED_SERVO);
        }
    }
}

void movePen(int angle) {
    servoPen.write(angle);
    currentPen = angle;
}

void penUp() {
    movePen(PEN_UP);
    delay(200);
}

void penDown() {
    movePen(PEN_DOWN);
    delay(200);
}

void goHome() {
    moveBase(90);
    moveElbow(90);
    penUp();
}

// ═══════════════════════════════════════════════════════════════════════════
// 📐 KINEMATICS - Convert X,Y to servo angles
// ═══════════════════════════════════════════════════════════════════════════

bool moveToXY(float x, float y) {
    float D = sqrt(x*x + y*y);
    float maxReach = L1_LENGTH + L2_LENGTH;
    float minReach = abs(L1_LENGTH - L2_LENGTH);
    
    if (D > maxReach || D < minReach) {
        Serial.print("⚠️ Out of range: ");
        Serial.println(D);
        return false;
    }
    
    // Calculate elbow angle
    float cosT2 = (D*D - L1_LENGTH*L1_LENGTH - L2_LENGTH*L2_LENGTH) / (2*L1_LENGTH*L2_LENGTH);
    cosT2 = constrain(cosT2, -1.0, 1.0);
    float theta2 = acos(cosT2);
    
    // Calculate base angle
    float psi = atan2(y, x);
    float beta = asin(constrain(L2_LENGTH * sin(theta2) / D, -1.0, 1.0));
    float theta1 = psi - beta;
    
    moveBase(degrees(theta1));
    moveElbow(degrees(theta2));
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// 🎨 DRAWING FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void drawLine(float x1, float y1, float x2, float y2, int steps = 10) {
    penDown();
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        moveToXY(x1 + t*(x2-x1), y1 + t*(y2-y1));
        delay(30);
    }
    penUp();
}

void drawCircle(float cx, float cy, float r) {
    penDown();
    for (int i = 0; i <= 36; i++) {
        float a = (float)i / 36 * 2 * PI;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
        delay(20);
    }
    penUp();
}

void drawSquare(float cx, float cy, float size) {
    float h = size / 2;
    penDown();
    moveToXY(cx - h, cy - h);
    moveToXY(cx + h, cy - h);
    moveToXY(cx + h, cy + h);
    moveToXY(cx - h, cy + h);
    moveToXY(cx - h, cy - h);
    penUp();
}

void drawTriangle(float cx, float cy, float size) {
    float h = size * 0.866;  // Height of equilateral triangle
    penDown();
    moveToXY(cx, cy - size/2);
    moveToXY(cx - size/2, cy + h/3);
    moveToXY(cx + size/2, cy + h/3);
    moveToXY(cx, cy - size/2);
    penUp();
}

void drawStar(float cx, float cy, float outer, float inner) {
    float points[5][2], innerPts[5][2];
    
    for (int i = 0; i < 5; i++) {
        float a = -PI/2 + i * 2*PI/5;
        points[i][0] = cx + outer * cos(a);
        points[i][1] = cy + outer * sin(a);
        float b = -PI/2 + (i*2+1)*PI/5;
        innerPts[i][0] = cx + inner * cos(b);
        innerPts[i][1] = cy + inner * sin(b);
    }
    
    penDown();
    moveToXY(points[0][0], points[0][1]);
    for (int i = 0; i < 5; i++) {
        moveToXY(innerPts[i][0], innerPts[i][1]);
        moveToXY(points[(i+1)%5][0], points[(i+1)%5][1]);
    }
    penUp();
}

void drawHeart(float cx, float cy, float size) {
    float r = size / 4;
    penDown();
    moveToXY(cx - r, cy + r/2);
    
    // Left curve
    for (int i = 0; i <= 20; i++) {
        float a = PI + (float)i/20*PI;
        moveToXY(cx - r + r*cos(a), cy + r/2 + r*sin(a));
    }
    
    // Right curve
    for (int i = 0; i <= 20; i++) {
        float a = (float)i/20*PI;
        moveToXY(cx + r + r*cos(a), cy + r/2 + r*sin(a));
    }
    penUp();
}

// ═══════════════════════════════════════════════════════════════════════════
// ✏️ LETTER DRAWING (A-Z, 0-9)
// ═══════════════════════════════════════════════════════════════════════════

void drawA(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx, cy - s/2);
    moveToXY(cx + w/2, cy + s/2);
    moveToXY(cx - w/3, cy + s/6);
    moveToXY(cx + w/3, cy + s/6);
    penUp();
}

void drawB(float cx, float cy, float s) {
    float w = s * 0.7;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/4);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i/10;
        moveToXY(cx - w/2 + w*t, cy - s/4 + s/2*sin(t*PI));
    }
    penUp();
}

void drawC(float cx, float cy, float s) {
    float r = s / 2;
    penDown();
    for (int i = 0; i <= 20; i++) {
        float a = PI * 0.2 + (float)i/20 * PI * 1.6;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
    }
    penUp();
}

void drawD(float cx, float cy, float s) {
    float w = s * 0.7;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    for (int i = 0; i <= 15; i++) {
        float t = (float)i/15;
        moveToXY(cx - w/2 + w*sin(t*PI/2), cy - s/2 + s*t);
    }
    for (int i = 0; i <= 15; i++) {
        float t = (float)i/15;
        moveToXY(cx - w/2 + w*sin((1-t)*PI/2), cy + s/2 - s*t);
    }
    penUp();
}

void drawE(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx - w/2, cy);
    moveToXY(cx + w/3, cy);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx + w/2, cy + s/2);
    penUp();
}

void drawF(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx - w/2, cy);
    moveToXY(cx + w/3, cy);
    penUp();
}

void drawG(float cx, float cy, float s) {
    float r = s / 2;
    penDown();
    for (int i = 0; i <= 20; i++) {
        float a = PI * 0.2 + (float)i/20 * PI * 1.6;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
    }
    moveToXY(cx + r*0.5, cy);
    moveToXY(cx + r*0.5, cy - r*0.3);
    penUp();
}

void drawH(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx + w/2, cy + s/2);
    moveToXY(cx - w/2, cy);
    moveToXY(cx + w/2, cy);
    penUp();
}

void drawI(float cx, float cy, float s) {
    float w = s * 0.3;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx, cy - s/2);
    moveToXY(cx, cy + s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx + w/2, cy + s/2);
    penUp();
}

void drawJ(float cx, float cy, float s) {
    float w = s * 0.6;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx + w/2, cy + s/4);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i/10;
        moveToXY(cx + w/2 - w*t, cy + s/4 + s/4*sin(t*PI/2));
    }
    penUp();
}

void drawK(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx - w/2, cy);
    moveToXY(cx + w/2, cy + s/2);
    penUp();
}

void drawL(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx + w/2, cy + s/2);
    penUp();
}

void drawM(float cx, float cy, float s) {
    float w = s * 0.9;
    penDown();
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx, cy + s/4);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx + w/2, cy + s/2);
    penUp();
}

void drawN(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy + s/2);
    moveToXY(cx + w/2, cy - s/2);
    penUp();
}

void drawO(float cx, float cy, float s) {
    float r = s / 2;
    penDown();
    for (int i = 0; i <= 36; i++) {
        float a = (float)i/36 * 2*PI;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
    }
    penUp();
}

void drawP(float cx, float cy, float s) {
    float w = s * 0.7;
    penDown();
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i/10;
        moveToXY(cx + w/2 - w*t, cy - s/4 + s/2*sin(t*PI));
    }
    penUp();
}

void drawQ(float cx, float cy, float s) {
    float r = s / 2;
    penDown();
    for (int i = 0; i <= 36; i++) {
        float a = (float)i/36 * 2*PI;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
    }
    moveToXY(cx + r*0.6, cy + r*0.5);
    moveToXY(cx + r*1.2, cy + s/2);
    penUp();
}

void drawR(float cx, float cy, float s) {
    float w = s * 0.7;
    penDown();
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    for (int i = 0; i <= 8; i++) {
        float t = (float)i/8;
        moveToXY(cx + w/2 - w*t, cy - s/4 + s/2*t);
    }
    moveToXY(cx - w/2, cy);
    moveToXY(cx + w/2, cy + s/2);
    penUp();
}

void drawS(float cx, float cy, float s) {
    float r = s / 3;
    penDown();
    for (int i = 0; i <= 12; i++) {
        float a = PI - PI/3 + (float)i/12*PI*2/3;
        moveToXY(cx + r*cos(a), cy - s/4 + r*sin(a));
    }
    for (int i = 0; i <= 12; i++) {
        float a = PI/3 + (float)i/12*PI*2/3;
        moveToXY(cx + r*cos(a), cy + s/4 + r*sin(a));
    }
    penUp();
}

void drawT(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx, cy - s/2);
    moveToXY(cx, cy + s/2);
    penUp();
}

void drawU(float cx, float cy, float s) {
    float r = s / 3;
    penDown();
    moveToXY(cx - s/2, cy - s/2);
    moveToXY(cx - s/2, cy + s/4);
    for (int i = 0; i <= 12; i++) {
        float a = PI/2 + (float)i/12*PI;
        moveToXY(cx + r*cos(a), cy + s/4 + r*sin(a));
    }
    moveToXY(cx + s/2, cy + s/4);
    moveToXY(cx + s/2, cy - s/2);
    penUp();
}

void drawV(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx, cy + s/2);
    moveToXY(cx + w/2, cy - s/2);
    penUp();
}

void drawW(float cx, float cy, float s) {
    float w = s * 0.9;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx - w/4, cy + s/2);
    moveToXY(cx, cy - s/2);
    moveToXY(cx + w/4, cy + s/2);
    moveToXY(cx + w/2, cy - s/2);
    penUp();
}

void drawX(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy + s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx + w/2, cy - s/2);
    penUp();
}

void drawY(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/4, cy - s/2);
    moveToXY(cx, cy);
    moveToXY(cx + w/4, cy - s/2);
    moveToXY(cx, cy);
    moveToXY(cx, cy + s/2);
    penUp();
}

void drawZ(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - w/2, cy - s/2);
    moveToXY(cx + w/2, cy - s/2);
    moveToXY(cx - w/2, cy + s/2);
    moveToXY(cx + w/2, cy + s/2);
    penUp();
}

// NUMBER DRAWING
void draw0(float cx, float cy, float s) { drawO(cx, cy, s); }
void draw1(float cx, float cy, float s) {
    penDown();
    moveToXY(cx - s/6, cy - s/3);
    moveToXY(cx, cy - s/2);
    moveToXY(cx, cy + s/2);
    moveToXY(cx - s/4, cy + s/2);
    moveToXY(cx + s/4, cy + s/2);
    penUp();
}
void draw2(float cx, float cy, float s) {
    float r = s/3;
    penDown();
    moveToXY(cx - s/2, cy - s/3);
    moveToXY(cx + s/2, cy - s/3);
    for (int i = 0; i <= 15; i++) {
        float a = PI - PI/3 + (float)i/15*PI*2/3;
        moveToXY(cx + r*cos(a), cy - s/3 + r*sin(a));
    }
    moveToXY(cx - s/2, cy + s/2);
    moveToXY(cx + s/2, cy + s/2);
    penUp();
}
void draw3(float cx, float cy, float s) {
    float r = s/3;
    penDown();
    moveToXY(cx, cy - s/2);
    moveToXY(cx + s/3, cy - s/2);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i/10;
        moveToXY(cx + s/3 - s/3*t, cy - s/4 + s/2*sin(t*PI));
    }
    for (int i = 0; i <= 10; i++) {
        float t = (float)i/10;
        moveToXY(cx + s/3 - s/3*t, cy + s/4 + s/4*sin(t*PI));
    }
    penUp();
}
void draw4(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx + w/4, cy - s/2);
    moveToXY(cx - w/2, cy + s/6);
    moveToXY(cx + w/2, cy + s/6);
    moveToXY(cx + w/4, cy - s/2);
    moveToXY(cx + w/4, cy + s/2);
    penUp();
}
void draw5(float cx, float cy, float s) {
    penDown();
    moveToXY(cx + s/2, cy - s/2);
    moveToXY(cx - s/3, cy - s/2);
    moveToXY(cx - s/2, cy);
    for (int i = 0; i <= 12; i++) {
        float t = (float)i/12;
        moveToXY(cx - s/2 + s*t, cy + s/4 + s/4*sin(t*PI));
    }
    penUp();
}
void draw6(float cx, float cy, float s) {
    float r = s/3;
    penDown();
    for (int i = 0; i <= 20; i++) {
        float a = PI + (float)i/20*PI;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
    }
    for (int i = 0; i <= 18; i++) {
        float a = -PI/2 + (float)i/18*PI;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
    }
    penUp();
}
void draw7(float cx, float cy, float s) {
    float w = s * 0.8;
    penDown();
    moveToXY(cx - s/2, cy - s/2);
    moveToXY(cx + s/2, cy - s/2);
    moveToXY(cx - s/4, cy + s/2);
    penUp();
}
void draw8(float cx, float cy, float s) {
    float r = s/4;
    penDown();
    for (int i = 0; i <= 20; i++) {
        float a = (float)i/20*2*PI;
        moveToXY(cx + r*cos(a), cy - s/4 + r*sin(a));
    }
    for (int i = 0; i <= 20; i++) {
        float a = (float)i/20*2*PI;
        moveToXY(cx + r*cos(a), cy + s/4 + r*sin(a));
    }
    penUp();
}
void draw9(float cx, float cy, float s) {
    float r = s/3;
    penDown();
    for (int i = 0; i <= 18; i++) {
        float a = (float)i/18*PI;
        moveToXY(cx + r*cos(a), cy - r*sin(a));
    }
    for (int i = 0; i <= 18; i++) {
        float a = PI/2 - (float)i/18*PI*1.5;
        moveToXY(cx + r*cos(a), cy + r*sin(a));
    }
    penUp();
}

void drawLetter(char c) {
    c = toupper(c);
    switch(c) {
        case 'A': drawA(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'B': drawB(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'C': drawC(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'D': drawD(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'E': drawE(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'F': drawF(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'G': drawG(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'H': drawH(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'I': drawI(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'J': drawJ(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'K': drawK(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'L': drawL(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'M': drawM(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'N': drawN(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'O': drawO(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'P': drawP(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'Q': drawQ(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'R': drawR(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'S': drawS(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'T': drawT(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'U': drawU(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'V': drawV(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'W': drawW(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'X': drawX(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'Y': drawY(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case 'Z': drawZ(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '0': draw0(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '1': draw1(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '2': draw2(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '3': draw3(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '4': draw4(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '5': draw5(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '6': draw6(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '7': draw7(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '8': draw8(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
        case '9': draw9(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE); break;
    }
}

void drawShape(const char* name) {
    if (strcmp(name, "CIRCLE") == 0) drawCircle(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE);
    else if (strcmp(name, "SQUARE") == 0) drawSquare(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE);
    else if (strcmp(name, "TRIANGLE") == 0) drawTriangle(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE);
    else if (strcmp(name, "STAR") == 0) drawStar(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE, DRAW_SIZE*0.4);
    else if (strcmp(name, "HEART") == 0) drawHeart(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE*1.2);
    else drawCircle(DRAW_HOME_X, DRAW_HOME_Y, DRAW_SIZE);
}

// ═══════════════════════════════════════════════════════════════════════════
// 💡 LED FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void setPowerLED(bool on) { digitalWrite(6, on ? HIGH : LOW); }
void setStatusLED(bool on) { digitalWrite(7, on ? HIGH : LOW); }
void blinkLED(int times, int ms) {
    for (int i = 0; i < times; i++) {
        digitalWrite(7, HIGH);
        delay(ms);
        digitalWrite(7, LOW);
        delay(ms);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// 👁️ IR SENSOR
// ═══════════════════════════════════════════════════════════════════════════

int readIR() { return analogRead(20); }
bool cardDetected() { return readIR() > IR_THRESHOLD; }

// ═══════════════════════════════════════════════════════════════════════════
// 🎛️ CALIBRATION MODE
// ═══════════════════════════════════════════════════════════════════════════

void calibrationMode() {
    Serial.println("╔══════════════════════════════════════════╗");
    Serial.println("║   🎛️  CALIBRATION MODE               ║");
    Serial.println("╠══════════════════════════════════════════╣");
    Serial.println("║  1/2: Base ±5°                       ║");
    Serial.println("║  3/4: Elbow ±5°                      ║");
    Serial.println("║  5/6: Pen up/down                    ║");
    Serial.println("║  7/8: Pen angle ±5°                  ║");
    Serial.println("║  H: Home position                    ║");
    Serial.println("║  T: Test IR sensor                   ║");
    Serial.println("║  + / -: Base offset ±1°             ║");
    Serial.println("║  . / ,: Elbow offset ±1°           ║");
    Serial.println("║  Q: Quit and save                    ║");
    Serial.println("╚══════════════════════════════════════════╝");
    
    while (true) {
        if (Serial.available()) {
            char cmd = Serial.read();
            
            switch(cmd) {
                case '1': currentBase = constrain(currentBase + 5, BASE_MIN, BASE_MAX); servoBase.write(currentBase); break;
                case '2': currentBase = constrain(currentBase - 5, BASE_MIN, BASE_MAX); servoBase.write(currentBase); break;
                case '3': currentElbow = constrain(currentElbow + 5, ELBOW_MIN, ELBOW_MAX); servoElbow.write(currentElbow); break;
                case '4': currentElbow = constrain(currentElbow - 5, ELBOW_MIN, ELBOW_MAX); servoElbow.write(currentElbow); break;
                case '5': servoPen.write(PEN_UP); break;
                case '6': servoPen.write(PEN_DOWN); break;
                case '7': PEN_UP = constrain(PEN_UP + 5, 0, 180); break;
                case '8': PEN_UP = constrain(PEN_UP - 5, 0, 180); break;
                case 'h': case 'H': goHome(); break;
                case 't': case 'T': Serial.print("IR: "); Serial.println(readIR()); break;
                case '+': BASE_OFFSET++; Serial.print("BASE_OFFSET="); Serial.println(BASE_OFFSET); break;
                case '-': BASE_OFFSET--; Serial.print("BASE_OFFSET="); Serial.println(BASE_OFFSET); break;
                case '.': ELBOW_OFFSET++; Serial.print("ELBOW_OFFSET="); Serial.println(ELBOW_OFFSET); break;
                case ',': ELBOW_OFFSET--; Serial.print("ELBOW_OFFSET="); Serial.println(ELBOW_OFFSET); break;
                case 'q': case 'Q':
                    Serial.println("✓ SAVED VALUES:");
                    Serial.print("BASE_OFFSET="); Serial.println(BASE_OFFSET);
                    Serial.print("ELBOW_OFFSET="); Serial.println(ELBOW_OFFSET);
                    Serial.print("PEN_UP="); Serial.println(PEN_UP);
                    Serial.print("PEN_DOWN="); Serial.println(PEN_DOWN);
                    Serial.print("BASE_MIN="); Serial.println(BASE_MIN);
                    Serial.print("BASE_MAX="); Serial.println(BASE_MAX);
                    Serial.print("ELBOW_MIN="); Serial.println(ELBOW_MIN);
                    Serial.print("ELBOW_MAX="); Serial.println(ELBOW_MAX);
                    Serial.print("IR_THRESHOLD="); Serial.println(IR_THRESHOLD);
                    Serial.print("L1_LENGTH="); Serial.println(L1_LENGTH);
                    Serial.print("L2_LENGTH="); Serial.println(L2_LENGTH);
                    goHome();
                    return;
            }
            
            Serial.print("Base="); Serial.print(currentBase);
            Serial.print("° Elbow="); Serial.print(currentElbow);
            Serial.print("° IR="); Serial.println(readIR());
        }
        delay(10);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// 🔄 SETUP
// ═══════════════════════════════════════════════════════════════════════════

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("╔══════════════════════════════════════════╗");
    Serial.println("║   🤖 3-SERVO SCARA ROBOT v2.0          ║");
    Serial.println("║   BASE + ELBOW + PEN                   ║");
    Serial.println("╚══════════════════════════════════════════╝");
    
    servoBase.attach(2);
    servoElbow.attach(3);
    servoPen.attach(4);
    
    pinMode(5, INPUT_PULLUP);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(20, INPUT);
    
    setPowerLED(true);
    playBeep(523, 150);
    delay(50);
    playBeep(659, 150);
    delay(50);
    playBeep(784, 150);
    delay(50);
    playBeep(1047, 150);
    
    initAudio();
    goHome();
    
    Serial.println();
    Serial.println("✅ Ready!");
    Serial.print("📐 L1="); Serial.print(L1_LENGTH); Serial.print("mm L2="); Serial.print(L2_LENGTH); Serial.println("mm");
    Serial.print("🎯 IR Threshold="); Serial.println(IR_THRESHOLD);
    Serial.println("📋 Hold button 3 sec for calibration");
    
    blinkLED(3, 200);
}

// ═══════════════════════════════════════════════════════════════════════════
// 🔄 LOOP
// ═══════════════════════════════════════════════════════════════════════════

bool wasPressed = false;
unsigned long pressTime = 0;
bool cardWasDetected = false;
int demoIndex = 0;
const char* demoItems[] = {"A","B","C","1","2","3","CIRCLE","SQUARE","STAR"};
int demoCount = 9;

void loop() {
    // Button check
    bool pressed = (digitalRead(5) == LOW);
    if (pressed && !wasPressed) pressTime = millis();
    if (pressed && !wasPressed && millis() - pressTime > 3000) {
        Serial.println("🔧 CALIBRATION!");
        blinkLED(5, 100);
        calibrationMode();
    }
    wasPressed = pressed;
    
    // Card detection
    if (cardDetected() && !cardWasDetected) {
        Serial.println("👁️ Card!");
        setStatusLED(true);
        beepSuccess();
        
        const char* item = demoItems[demoIndex];
        demoIndex = (demoIndex + 1) % demoCount;
        
        Serial.print("🗣️ "); Serial.println(item);
        
        bool isShape = (strcmp(item, "CIRCLE")==0 || strcmp(item, "SQUARE")==0 || 
                       strcmp(item, "STAR")==0 || strcmp(item, "TRIANGLE")==0);
        
        if (isShape) {
            char file[30];
            sprintf(file, "/audio/%s.mp3", item);
            playAudio(file);
            delay(500);
            drawShape(item);
        } else {
            if (strlen(item) == 1) {
                char c = item[0];
                if (c >= 'A' && c <= 'Z') {
                    char file[20];
                    sprintf(file, "/audio/%c.mp3", c);
                    playAudio(file);
                    delay(300);
                }
            }
            drawLetter(item[0]);
        }
        
        beepSuccess();
        Serial.println("✅ Done!");
        goHome();
        cardWasDetected = true;
        delay(2000);
    }
    else if (!cardDetected()) {
        cardWasDetected = false;
        setStatusLED(false);
    }
    
    // Debug
    static unsigned long last = 0;
    if (millis() - last > 2000) {
        Serial.print("📊 IR="); Serial.print(readIR());
        Serial.print(" | Threshold="); Serial.print(IR_THRESHOLD);
        Serial.print(" | Card="); Serial.print(cardDetected()?"YES":"NO");
        Serial.print(" | Base="); Serial.print(currentBase);
        Serial.print("° Elbow="); Serial.print(currentElbow);
        Serial.println("°");
        last = millis();
    }
    
    delay(50);
}