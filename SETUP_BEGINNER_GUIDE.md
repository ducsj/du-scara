# 🚀 COMPLETE BEGINNER SETUP GUIDE

## From Zero to Robot in 5 Steps!

---

# 📦 STEP 1: DOWNLOAD REQUIRED SOFTWARE

### 1.1 Arduino IDE (FREE - Required for coding)

```
1. Open your browser and go to:
   👉 https://www.arduino.cc/en/software

2. Click: "Windows" or "Mac" or "Linux" (based on your computer)

3. Click: "Just Download"

4. Wait for file to download (about 200MB)

5. Double-click the downloaded file to install
   - Click "I Agree"
   - Click "Next"
   - Click "Install"
   - Click "Finish"

✅ Arduino IDE is now installed!
```

### 1.2 Python (FREE - For learning course)

```
FOR WINDOWS:
1. Go to: https://www.python.org/downloads/
2. Click: "Download Python 3.x.x"
3. Run the downloaded file
4. IMPORTANT: Check ✅ "Add Python to PATH"
5. Click "Install Now"

FOR MAC:
1. Open Terminal app
2. Type: brew install python3
   (Install Homebrew first if needed: https://brew.sh)

FOR LINUX:
1. Open Terminal
2. Type: sudo apt install python3
```

---

# 📂 STEP 2: GET THE FILES TO YOUR COMPUTER

### Option A: Download Each File One by One

```
1. Look at the file list on the LEFT side of this screen

2. Click on each file you want:
   - Click "Robotic_Drawing_Assistant.ino" → Download
   - Click "WIRING_GUIDE.md" → Download
   - Click "CODE_EXPLAINED.md" → Download
   - Click "AUDIO_FILES_GUIDE.md" → Download
   - Click "CALIBRATION_GUIDE.md" → Download

3. Save all files in one folder (like "MyRobot")
```

### Option B: Copy-Paste the Code

```
1. Click on "Robotic_Drawing_Assistant.ino"

2. Click "Select All" or drag to select ALL the code

3. Right-click → Copy

4. Open Arduino IDE
5. File → New
6. Paste (Ctrl+V or Cmd+V)

7. File → Save As → "Robotic_Drawing_Assistant"
```

---

# ⚙️ STEP 3: SETUP ARDUINO IDE FOR ESP32

### 3.1 Add ESP32 Board Support

```
1. Open Arduino IDE

2. Click: File → Preferences

3. Look for "Additional Boards Manager URLs"

4. Copy this text and paste there:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

5. Click: OK

6. Click: Tools → Board → Boards Manager

7. Type in search box: "esp32"

8. Click: "ESP32 by Espressif Systems" → Install

9. Wait for installation (may take 5 minutes)

✅ ESP32 support installed!
```

### 3.2 Install Servo Library

```
1. Click: Sketch → Include Library → Manage Libraries

2. Type in search box: "ESP32Servo"

3. Click: "ESP32Servo by John Sanpe" → Install

4. Wait for installation
```

### 3.3 Install Audio Library (for speaker)

```
1. Click: Sketch → Include Library → Manage Libraries

2. Type in search box: "ESP8266Audio"

3. Click: "ESP8266Audio by Earle F. Philhower" → Install

4. Wait for installation

✅ Audio library installed!
```

---

# 🔌 STEP 4: WIRE YOUR ROBOT

See `WIRING_GUIDE.md` for detailed diagrams.

### Quick Wire Summary:

```
ESP32-C3 PIN    → CONNECT TO
────────────────────────────────
GPIO 2          → Servo Base (orange wire)
GPIO 3          → Servo Elbow (orange wire)
GPIO 4          → Servo Gripper (orange wire)
GPIO 5          → Button → GND
GPIO 6          → Power LED (+) → 220Ω resistor → GND
GPIO 7          → Status LED (+) → 220Ω resistor → GND
GPIO 20         → IR Sensor (AO)
GPIO 18         → MAX98357 DIN
GPIO 21         → MAX98357 LRC
GPIO 22         → MAX98357 BCLK
5V              → Servos, IR, Audio Amp (all VCC)
GND             → All GND
```

### Power System:
```
Battery (2x 18650) → TP4056 → TPS61023 → 5V Rail → ESP32 + Servos + Audio
```

---

# 📤 STEP 5: UPLOAD CODE TO ESP32

### 5.1 Connect ESP32 to Computer

```
1. Use USB cable to connect ESP32-C3 to your computer

2. Wait for drivers to install (usually automatic)

3. Check which COM port:
   - Windows: Device Manager → Ports → COM3 or COM4 etc.
   - Mac/Linux: /dev/cu.usbserial-XXXX
```

### 5.2 Select Board and Port

```
In Arduino IDE:

1. Click: Tools → Board → ESP32 Arduino → "ESP32C3 Dev Module"

2. Click: Tools → Port → Select your COM port

3. Click: Tools → Upload Speed → "115200"
```

### 5.3 Upload the Code

```
1. Copy all code from Robotic_Drawing_Assistant.ino
   (Select all, copy, paste into Arduino IDE)

2. Click: Upload button (➡️) or Ctrl+U

3. Wait for "Uploading..." to finish

4. If you see "Failed to connect":
   - Hold BOOT button on ESP32
   - Press and release RESET button
   - Release BOOT button
   - Click Upload again

✅ Code uploaded!
```

---

# 🎤 STEP 6: CREATE AUDIO FILES (For Speaking)

### 6.1 Create Audio Files

```
1. Create folder: data/audio/

2. Go to: https://translate.google.com

3. Type each word and click speaker 🔊
   - Right-click speaker → "Save audio as"
   - Save as circle.mp3, square.mp3, etc.

FILES YOU NEED:
├── circle.mp3      (say "circle")
├── square.mp3      (say "square")
├── triangle.mp3    (say "triangle")
├── star.mp3        (say "star")
├── heart.mp3       (say "heart")
├── drawing.mp3     (say "drawing...")
├── done.mp3        (say "done!")
└── pickup.mp3      (say "pick up your drawing")
```

### 6.2 Upload Audio to ESP32

```
1. Install ESP32 Sketch Data Upload tool:
   - Tools → ESP32 Sketch Data Upload

2. Make sure your files are in:
   your_sketch_folder/data/audio/*.mp3

3. Click: Tools → ESP32 Sketch Data Upload

4. Wait for upload to complete

✅ Audio files uploaded!
```

---

# 🧪 STEP 7: TEST YOUR ROBOT

### 7.1 Open Serial Monitor

```
1. In Arduino IDE, click: Tools → Serial Monitor

2. Set baud rate to: 115200

3. You should see:
   ╔══════════════════════════════════════════════════╗
   ║         MY ROBOT - DRAWING ASSISTANT             ║
   ║         with SPEECH & PRONUNCIATION              ║
   ╚══════════════════════════════════════════════════╝
   ✅ System ready!
```

### 7.2 Test Calibration

```
1. Hold the button for 3 seconds

2. You should see "CALIBRATION MODE"

3. Type commands:
   - "1" → Base moves right
   - "2" → Base moves left
   - "3" → Elbow bends up
   - "4" → Elbow bends down
   - "5" → Gripper opens
   - "6" → Gripper closes
   - "H" → Go home
   - "Q" → Quit
```

### 7.3 Test Card Detection

```
1. Place a white card in front of IR sensor

2. IR value should change in Serial Monitor

3. If card detected:
   - Status LED lights up
   - Robot says "Drawing... Circle"
   - Robot draws circle
   - Robot says "Done!"
```

---

# 🔧 CALIBRATION STEPS

### After building your robot:

```
1. HOLD BUTTON 3 SECONDS
   → Enters calibration mode

2. Use commands to fine-tune:
   - "1" / "2" → Move base left/right
   - "3" / "4" → Move elbow up/down
   - "+" / "-" → Adjust base center
   - "." / "," → Adjust elbow center

3. Write down your values:
   BASE_ZERO_OFFSET = ???
   ELBOW_ZERO_OFFSET = ???

4. Press "Q" to save

5. Update code with your values
```

---

# ❓ TROUBLESHOOTING

### Problem: "ESP32 board not found"
```
Solution:
1. Install CP210x USB driver:
   https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
2. Try a different USB cable (needs data wires, not just power)
```

### Problem: Servos not moving
```
Solution:
1. Check power supply (need 5V external power!)
2. Check GND connections
3. Check servo wires (orange = signal)
```

### Problem: "Libraries not found"
```
Solution:
1. Sketch → Include Library → Manage Libraries
2. Search and install:
   - ESP32Servo
   - ESP8266Audio
```

### Problem: No audio/sound
```
Solution:
1. Check MAX98357 connections
2. Check GPIO 18, 21, 22 wiring
3. Check audio files uploaded to SPIFFS
```

---

# 📋 CHECKLIST

Before testing, confirm you have:

```
SOFTWARE:
☐ Arduino IDE installed
☐ ESP32 board support added
☐ ESP32Servo library installed
☐ ESP8266Audio library installed

FILES:
☐ Robotic_Drawing_Assistant.ino (in Arduino)
☐ Audio files in data/audio/ folder

HARDWARE:
☐ ESP32-C3 connected to computer
☐ All servos wired correctly
☐ Power supply connected (5V)
☐ IR sensor connected
☐ Speaker connected

TESTING:
☐ Serial Monitor shows startup message
☐ Servos respond to commands
☐ IR sensor detects cards
☐ Audio plays (if files uploaded)
```

---

# 🎉 YOU'RE READY!

Follow the steps in order. If you get stuck, the guides are here to help!

Need more help? Just ask! 😊