# ⚡ QUICK START - 5 MINUTES TO FIRST CODE

## No Experience Needed!

---

# 🚀 START HERE (In Order)

## STEP 1: Download Arduino IDE (2 min)

```
1. Open your browser
2. Go to: https://www.arduino.cc/en/software
3. Click: WINDOWS (or Mac/Linux)
4. Click: "Just Download"
5. Install the downloaded file (click Next, Next, Finish)
```

---

## STEP 2: Open Arduino IDE (30 sec)

```
Find and click the Arduino icon on your computer:
   ┌──────────┐
   │ ARDUINO  │
   │    ()    │
   └──────────┘
```

---

## STEP 3: Add ESP32 Support (2 min)

```
1. Click: File → Preferences

2. Find: "Additional boards manager URLs"

3. Click inside the box and paste this:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

4. Click: OK

5. Click: Tools → Board → Boards Manager

6. Type: esp32

7. Click: "ESP32 by Espressif Systems" → Install
```

---

## STEP 4: Install Libraries (2 min)

```
1. Click: Sketch → Include Library → Manage Libraries

2. Type in search: ESP32Servo

3. Click: "ESP32Servo" → Install

4. Repeat for: ESP8266Audio
```

---

## STEP 5: Paste My Code (1 min)

```
1. Come back to my code file
2. Click "Select All" 
3. Copy (Ctrl+C)
4. Back to Arduino IDE
5. Click in the window
6. Paste (Ctrl+V)
7. Click: File → Save As → "MyRobot"
```

---

## STEP 6: Upload to Robot (1 min)

```
1. Connect ESP32-C3 to computer with USB cable

2. Click: Tools → Board → ESP32 Arduino → ESP32C3 Dev Module

3. Click: Tools → Port → (Select the COM port that appears)

4. Click the RIGHT ARROW button (→) to upload
   ┌────────────────┐
   │ [✓] [→] [🔍]  │  ← This button!
   └────────────────┘

5. Wait for "Done uploading."
```

---

## STEP 7: See It Work! (30 sec)

```
1. Click: Tools → Serial Monitor

2. Set dropdown to: 115200

3. You should see:
   ╔═══════════════════════════════╗
   ║  MY ROBOT - DRAWING ASSISTANT ║
   ║  ✅ System ready!             ║
   ╚═══════════════════════════════╝
```

---

# 🎉 YOU DID IT!

```
✅ Arduino IDE installed
✅ ESP32 board added
✅ Libraries installed
✅ Code uploaded
✅ Robot responding!
```

---

# 📖 FILES YOU HAVE

| File | What It Does |
|------|-------------|
| `SETUP_BEGINNER_GUIDE.md` | Full detailed guide |
| `ARDUINO_IDE_SETUP_GUIDE.md` | Picture guide |
| `WIRING_GUIDE.md` | How to wire everything |
| `CODE_EXPLAINED.md` | What the code means |
| `AUDIO_FILES_GUIDE.md` | Make the robot speak |
| `CALIBRATION_GUIDE.md` | Fine-tune your robot |

---

# ❓ STUCK? 

### "Board not found"
→ Hold BOOT, press RESET, release BOOT, try upload again

### "Libraries not found"
→ Sketch → Include Library → Manage Libraries → Install again

### "COM port not showing"
→ Try different USB cable (needs data wires)

### "Servos not moving"
→ Need external 5V power supply!

---

# 🔜 NEXT: Wire Your Robot

See `WIRING_GUIDE.md` for how to connect all parts!

---

**Questions? Just ask! 😊**