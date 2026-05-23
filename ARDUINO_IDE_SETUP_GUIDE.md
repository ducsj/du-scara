# 📸 ARDUINO IDE SETUP - PICTURE GUIDE

## Step-by-Step Screenshots

---

# 1️⃣ DOWNLOAD ARDUINO IDE

```
🌐 Go to: https://www.arduino.cc/en/software

   ┌─────────────────────────────────────────────┐
   │                                             │
   │   ARDUINO IDE 2.x (NEW!)                    │
   │   ━━━━━━━━━━━━━━━━━━━━                       │
   │   The new desktop classic.                  │
   │   Download the Arduino IDE 2.x to get the   │
   │   latest features and improvements!          │
   │                                             │
   │   [Download options]                         │
   │   ┌─────────┐ ┌─────────┐ ┌─────────┐       │
   │   │ WINDOWS │ │   MAC   │ │ LINUX   │       │
   │   └─────────┘ └─────────┘ └─────────┘       │
   │                                             │
   └─────────────────────────────────────────────┘

Click your operating system → Download → Install
```

---

# 2️⃣ OPEN ARDUINO IDE

```
After installing, you'll see:

   ┌─────────────────────────────────────────────┐
   │  ARDUINO IDE                                │
   │  ┌─────────────────────────────────────┐    │
   │  │                                     │    │
   │  │    // Your code goes here           │    │
   │  │    void setup() {                   │    │
   │  │    }                                │    │
   │  │    void loop() {                    │    │
   │  │    }                                │    │
   │  │                                     │    │
   │  └─────────────────────────────────────┘    │
   │                                             │
   │  [✓] [→] [🔍]                              │
   └─────────────────────────────────────────────┘

This is where you'll paste your code!
```

---

# 3️⃣ ADD ESP32 BOARD SUPPORT

## Step 3a: Open Preferences

```
Click: File → Preferences

   ┌─────────────────────────────────────────────┐
   │  Preferences                               │
   │  ┌─────────────────────────────────────┐    │
   │  │ Sketchbook location:  [Browse...]    │    │
   │  │                                     │    │
   │  │ Editor language:    English         │    │
   │  │ Editor font size:   14              │    │
   │  │                                     │    │
   │  │ ☑ Show verbose output during:       │    │
   │  │    [☑] compilation  [☑] upload      │    │
   │  │                                     │    │
   │  │ Additional boards manager URLs:    │    │
   │  │ ┌─────────────────────────────────┐│    │
   │  │ │                                 ││    │
   │  │ └─────────────────────────────────┘│    │
   │  │                          [OK] [Cancel]│    │
   │  └─────────────────────────────────────┘    │
   └─────────────────────────────────────────────┘
```

## Step 3b: Add ESP32 URL

```
1. Find "Additional boards manager URLs"

2. Click inside the text box

3. Copy and paste this URL:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

4. Click OK

   ┌─────────────────────────────────────────────┐
   │  Additional boards manager URLs:            │
   │  ┌─────────────────────────────────────┐    │
   │  │ https://raw.githubusercontent.com/  │    │
   │  │ espressif/arduino-esp32/gh-pages/    │    │
   │  │ package_esp32_index.json            │    │
   │  └─────────────────────────────────────┘    │
   │                                     [OK]    │
   └─────────────────────────────────────────────┘
```

---

# 4️⃣ INSTALL ESP32 BOARD

## Step 4a: Open Boards Manager

```
Click: Tools → Board → Boards Manager

   ┌─────────────────────────────────────────────┐
   │  Boards Manager                             │
   │                                             │
   │  [Search boxes...]                         │
   │                                             │
   │                                             │
   │                                             │
   │                                             │
   │                                             │
   │                                             │
   │                                             │
   └─────────────────────────────────────────────┘
```

## Step 4b: Search for ESP32

```
1. Click in search box

2. Type: esp32

3. You should see:

   ┌─────────────────────────────────────────────┐
   │  Boards Manager                             │
   │  [🔍 esp32..............................]   │
   │                                             │
   │  ┌─────────────────────────────────────┐    │
   │  │  ESP32 by Espressif Systems          │    │
   │  │  ⭐⭐⭐⭐⭐  │ Install │ More info │    │
   │  │  Latest version: 2.0.x               │    │
   │  │                                     │    │
   │  └─────────────────────────────────────┘    │
   │                                             │
   └─────────────────────────────────────────────┘

4. Click: Install
```

---

# 5️⃣ INSTALL LIBRARIES

## Step 5a: Open Library Manager

```
Click: Sketch → Include Library → Manage Libraries

   ┌─────────────────────────────────────────────┐
   │  Library Manager                            │
   │  [🔍 .................................]     │
   │                                             │
   │                                             │
   │                                             │
   └─────────────────────────────────────────────┘
```

## Step 5b: Install ESP32Servo

```
1. Type in search: ESP32Servo

2. Find: "ESP32Servo by John Sanpe"

3. Click: Install

   ┌─────────────────────────────────────────────┐
   │  ESP32Servo                                 │
   │  ⭐⭐⭐⭐⭐  │ INSTALLED │ More info │    │
   │  Provides common functionality for ESP32   │
   │  servo motors                               │
   └─────────────────────────────────────────────┘
```

## Step 5c: Install ESP8266Audio

```
1. Type in search: ESP8266Audio

2. Find: "ESP8266Audio by Earle F. Philhower"

3. Click: Install

   ┌─────────────────────────────────────────────┐
   │  ESP8266Audio                               │
   │  ⭐⭐⭐⭐⭐  │ INSTALLED │ More info │    │
   │  Audio file and i2s playing library         │
   └─────────────────────────────────────────────┘
```

---

# 6️⃣ PASTE AND SAVE CODE

## Step 6a: New Sketch

```
Click: File → New

   ┌─────────────────────────────────────────────┐
   │  Arduino IDE                         [−][□][×]│
   │  ┌─────────────────────────────────────┐    │
   │  │ void setup() {                      │    │
   │  │   // put your setup code here       │    │
   │  │ }                                   │    │
   │  │                                     │    │
   │  │ void loop() {                       │    │
   │  │   // put your main code here        │    │
   │  │ }                                   │    │
   │  └─────────────────────────────────────┘    │
   └─────────────────────────────────────────────┘
```

## Step 6b: Paste Code

```
1. Go back to my code file

2. Click on the code, select all (Ctrl+A or Cmd+A)

3. Copy (Ctrl+C or Cmd+C)

4. Back to Arduino IDE

5. Click in the code area

6. Paste (Ctrl+V or Cmd+V)

   ┌─────────────────────────────────────────────┐
   │  Arduino IDE                         [−][□][×]│
   │  ┌─────────────────────────────────────┐    │
   │  │ /*                                    │    │
   │  │ ╔═════════════════════════════════╗ │    │
   │  │ ║                                 ║ │    │
   │  │ ║   YOUR CODE GOES HERE!          ║ │    │
   │  │ ║                                 ║ │    │
   │  │ ╚═════════════════════════════════╝ │    │
   │  │ */                                  │    │
   │  └─────────────────────────────────────┘    │
   └─────────────────────────────────────────────┘
```

## Step 6c: Save Sketch

```
Click: File → Save As

   ┌─────────────────────────────────────────────┐
   │  Save sketch as...                          │
   │                                             │
   │  Name: [Robotic_Drawing_Assistant     ]     │
   │                                             │
   │  Location:                                  │
   │  ┌─────────────────────────────────────┐    │
   │  │ Documents/Arduino                    │    │
   │  └─────────────────────────────────────┘    │
   │                                             │
   │                              [Cancel][Save]│
   └─────────────────────────────────────────────┘
```

---

# 7️⃣ SELECT BOARD AND PORT

## Step 7a: Select ESP32-C3 Board

```
Click: Tools → Board → ESP32 Arduino → ESP32C3 Dev Module

   ┌─────────────────────────────────────────────┐
   │  Tools                                      │
   │  ├── Board: ESP32 Arduino         ▶        │
   │  │   ├── ESP32                  ▶           │
   │  │   │   └── ESP32C3 Dev Module  ← SELECT │
   │  │   └── ESP32S3                               │
   │  │                                         │
   │  ├── Port: COM3 (ESP32...        ▶         │
   │  │   └── COM3                                 │
   │  └──                                       │
   └─────────────────────────────────────────────┘
```

## Step 7b: Select COM Port

```
Click: Tools → Port → COM3 (or whatever port shows up)

   ┌─────────────────────────────────────────────┐
   │  Tools                                      │
   │  ├── Board: ESP32 Arduino         ▶        │
   │  ├── Port: COM3 (ESP32...        ▶         │
   │  │   ├── COM3 ← SELECT THIS!                 │
   │  │   └── COM4                               │
   │  └──                                       │
   └─────────────────────────────────────────────┘

⚠️ If no port shows, check:
   - USB cable connected?
   - Drivers installed?
   - Try different USB cable
```

---

# 8️⃣ UPLOAD CODE

## Step 8a: Click Upload Button

```
Look for this button:

   ┌─────────────────────────────────────────────┐
   │  [✓] [→] [🔍]                               │
   │       ↑                                      │
   │       This one!                             │
   └─────────────────────────────────────────────┘

Click the RIGHT ARROW (→)
```

## Step 8b: Wait for Upload

```
You should see at the bottom:

   Uploading...
   [████████████░░░░░░░░░░] 80%

   ⏳ Wait until it says:
   
   ✓ Done uploading.
```

## Step 8c: If Upload Fails

```
If you see "Failed to connect" or red error:

1. Hold the BOOT button on ESP32-C3

2. While holding BOOT, press RESET button once

3. Release BOOT button

4. Click Upload again

   ESP32-C3 Board:
   ┌────────────────┐
   │  BOOT │ RESET │
   │   ○   │   ○   │
   └────────────────┘
        ↑
   Hold BOOT while pressing RESET
```

---

# 9️⃣ OPEN SERIAL MONITOR

```
Click: Tools → Serial Monitor

   OR press: Ctrl+Shift+M (Windows) / Cmd+Shift+M (Mac)

   ┌─────────────────────────────────────────────┐
   │  Serial Monitor                       [×]  │
   │  ┌─────────────────────────────────────┐    │
   │  │ ╔══════════════════════════════════╗ │    │
   │  │ ║                                 ║ │    │
   │  │ ║   Your robot's messages here!   ║ │    │
   │  │ ║                                 ║ │    │
   │  │ ╚══════════════════════════════════╝ │    │
   │  │                                     │    │
   │  └─────────────────────────────────────┘    │
   │                                             │
   │  Baud: [115200 ▼]  [Clear output]          │
   └─────────────────────────────────────────────┘

SET BAUD TO: 115200
```

---

# ✅ YOU'RE DONE SETUP!

```
┌─────────────────────────────────────────────┐
│                                             │
│   ✅ Arduino IDE configured                 │
│   ✅ ESP32 board installed                  │
│   ✅ Libraries installed                    │
│   ✅ Code uploaded                          │
│   ✅ Serial Monitor open                    │
│                                             │
│   Now wire your robot and test!              │
│                                             │
└─────────────────────────────────────────────┘
```

---

# 📱 NEXT STEPS

1. Wire your robot (see WIRING_GUIDE.md)

2. Connect power supply

3. Test in Serial Monitor:
   - You should see "System ready!"
   - IR values changing when you move card
   - Servos moving when you send commands

4. Hold button 3 seconds to enter calibration mode

5. Fine-tune your robot (see CALIBRATION_GUIDE.md)

---

Need help? Just ask! 😊