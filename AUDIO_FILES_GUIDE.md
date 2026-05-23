# 🔊 HOW TO CREATE AUDIO FILES

## For Your Robot to Speak!

---

## 📁 AUDIO FILE SETUP

### 1. Create Folder Structure
```
Arduino/libraries/ESP8266Audio/src/
                    │
                    └── (install ESP8266Audio library)

DATA FOLDER (for SPIFFS upload):
your_sketch_folder/
    │
    └── data/
        └── audio/
            ├── A.mp3
            ├── B.mp3
            ├── C.mp3
            ...
            ├── 0.mp3
            ├── 1.mp3
            ├── 2.mp3
            ...
            ├── circle.mp3
            ├── square.mp3
            ├── triangle.mp3
            ├── star.mp3
            ├── heart.mp3
            ├── diamond.mp3
            ├── cross.mp3
            ├── drawing.mp3
            ├── done.mp3
            ├── pickup.mp3
            ├── hello.mp3
            ├── start.mp3
            └── error.mp3
```

---

## 🎤 METHOD 1: RECORD YOUR VOICE (Best Quality)

### Tools Needed:
- Smartphone voice recorder, OR
- Computer microphone
- Audio editing: Audacity (free) or GarageBand

### Steps:
1. **Record your voice** saying each word/letter
2. **Export as MP3** at 64kbps (small file size OK)
3. **Name correctly** (A.mp3, circle.mp3, etc.)
4. **Save to data/audio/** folder
5. **Upload to ESP32 SPIFFS**

### Tips:
- Speak clearly and slowly
- Same distance from microphone each time
- Keep files short (1-3 seconds for letters, 2-4 seconds for words)

---

## 🎙️ METHOD 2: TEXT-TO-SPEECH (Free & Easy)

### Option A: Google Translate TTS (Quick)
1. Go to: https://translate.google.com
2. Type your word (e.g., "circle")
3. Click the speaker icon 🔊 to hear
4. **Right-click** speaker → "Save audio as" → circle.mp3
5. Repeat for all words

**Pros:** Free, no signup
**Cons:** Limited control, may have Google branding

---

### Option B: ttsmp3.com (Better Quality)
1. Go to: https://ttsmp3.com
2. Select language: English (US)
3. Type your text
4. Choose voice (default is fine)
5. Click "Convert Text to Speech"
6. Download MP3
7. Name correctly

**Pros:** Better quality, many voices
**Cons:** Free version has watermark

---

### Option C: ElevenLabs (AI Voice - Best)
1. Go to: https://elevenlabs.io
2. Sign up (free tier)
3. Choose AI voice
4. Type text, generate audio
5. Download MP3

**Pros:** Natural sounding, no watermark
**Cons:** Free tier limited

---

## 🎵 AUDIO FILES TO CREATE

### ALPHABET (A-Z) - 26 files
```
A.mp3  → "A" (the letter)
B.mp3  → "B" (the letter)
C.mp3  → "C" (the letter)
...
Z.mp3  → "Z" (the letter)
```

### NUMBERS (0-9) - 10 files
```
0.mp3  → "Zero"
1.mp3  → "One"
2.mp3  → "Two"
3.mp3  → "Three"
4.mp3  → "Four"
5.mp3  → "Five"
6.mp3  → "Six"
7.mp3  → "Seven"
8.mp3  → "Eight"
9.mp3  → "Nine"
```

### SHAPE NAMES - 7 files
```
circle.mp3    → "Circle"
square.mp3    → "Square"
triangle.mp3 → "Triangle"
star.mp3     → "Star"
heart.mp3    → "Heart"
diamond.mp3  → "Diamond"
cross.mp3    → "Cross"
```

### UTILITY WORDS - 6 files
```
drawing.mp3  → "Drawing" (say before shape name)
done.mp3     → "Done" (after drawing)
pickup.mp3   → "Pick up your drawing" (at the end)
start.mp3    → "Start"
hello.mp3   → "Hello! Let's draw!"
error.mp3    → "Error"
```

---

## 🎬 CREATING SENTENCES

### How sentences work in code:
```cpp
// Say "Drawing Circle"
const char* sentence[] = {
    "/audio/drawing.mp3",   // First part
    "/audio/circle.mp3"    // Second part
};
saySentence(sentence, 2);
```

### Example sentences to create:

| File | Text |
|------|------|
| `drawing.mp3` | "Drawing..." |
| `circle.mp3` | "Circle" |
| `done.mp3` | "Done!" |
| `pickup.mp3` | "Pick up your drawing!" |
| `hello.mp3` | "Hello! Let's draw something!" |
| `place.mp3` | "Place a card to start" |
| `start_drawing.mp3` | "Starting to draw" |

---

## 📱 QUICK TEMPLATE FOR GOOGLE TRANSLATE

Copy and create these quickly:

### Shapes:
- circle
- square
- triangle  
- star
- heart
- diamond
- cross

### Words to say first:
- "Drawing"
- "Done"
- "Pick up your drawing"
- "Hello"
- "Let's draw"

---

## 🔧 HOW TO UPLOAD TO ESP32 SPIFFS

### Using Arduino IDE Plugin:

1. **Install ESP32 Sketch Data Upload tool:**
   - Tools → ESP32 Sketch Data Upload
   
2. **Prepare files:**
   - Put all MP3 files in `data/audio/` folder
   - Folder structure:
     ```
     sketch_folder/
       sketch.ino
       data/
         audio/
           circle.mp3
           square.mp3
           etc...
     ```

3. **Upload:**
   - Tools → ESP32 Sketch Data Upload
   - Wait for upload to complete
   - Files appear in ESP32's SPIFFS memory

---

## 🧪 TEST YOUR AUDIO

### After uploading, test each file:

```cpp
// Add this to setup() to test:
void testAudio() {
    playAudioFile("/audio/hello.mp3");  // Should say "Hello"
    delay(1000);
    playAudioFile("/audio/circle.mp3"); // Should say "Circle"
    delay(1000);
    spellWord("HELLO");                  // Should spell H-E-L-L-O
}
```

---

## 📊 AUDIO FILE REQUIREMENTS

| Property | Value |
|----------|-------|
| Format | MP3 or WAV |
| Sample Rate | 22050Hz or 44100Hz |
| Bit Rate | 64-128 kbps |
| Channels | Mono |
| Duration | 1-5 seconds |
| File Size | < 100KB each |

---

## 🎯 MINIMUM FILES TO START

You can start with just these files:

```
data/audio/
├── circle.mp3    (required for demo)
├── square.mp3    (optional)
├── triangle.mp3 (optional)
├── star.mp3      (optional)
├── drawing.mp3   (optional)
├── done.mp3      (optional)
└── hello.mp3    (optional)
```

If a file is missing, the robot will just play a beep instead.

---

## 🎓 RECORDING TIPS

1. **Same distance** from microphone every time
2. **Quiet room** - no background noise
3. **Speak clearly** - don't rush
4. **Same volume** for all files
5. **End with slight pause** - sounds more natural

### Good recording:
```
[Start speaking clearly]
"Circle"
[Pause 0.5s]
[End]
```

### Bad recording:
```
[Starts too soon]
"circccccl"
[Ends abruptly]
```

---

## 📝 CHECKLIST

Before testing, make sure you have:

- [ ] `data/audio/` folder created
- [ ] MP3 files named exactly right
- [ ] Files uploaded to SPIFFS
- [ ] Audio system initialized in code
- [ ] MAX98357 connected to GPIO 18, 21, 22
- [ ] Speaker connected to MAX98357

---

## ❓ TROUBLESHOOTING

| Problem | Cause | Fix |
|---------|-------|-----|
| No sound | Wrong I2S pins | Check GPIO 18, 21, 22 |
| Static noise | Bad connection | Check speaker wires |
| File not found | Wrong filename | Check spelling exactly |
| Cut off audio | File too long | Shorten to 3-4 seconds |
| Distorted | Volume too high | Lower gain on MAX98357 |

---

## 💡 PRO TIPS

1. **Batch create files** with Google Translate for quick testing
2. **Upgrade to quality recordings** later when you have time
3. **Use consistent voice** for all files
4. **Add silence** at end of files for natural pause
5. **Test as you go** - don't create 50 files then test!