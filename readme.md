# CrowPanel TTS Console — ESP32-S3 Firmware for Speech Synthesis via GRC HxTTS

**Summary:** This repository provides firmware for the ELECROW CrowPanel Advance 5.0 (ESP32-S3, 800×480) with a LVGL UI to control the GRC HxTTS module over UART and a built‑in **quiz** mechanic. The panel shows a question with answer choices, then an answer as an image. The explanatory answer text is sent to HxTTS for playback.

---

## Repository Structure

- `firmware/` — flashing utility and prebuilt binaries.  
- `main/` — firmware source code (ESP-IDF): LVGL UI, UART manager, HxTTS integration, asset/image subsystem, etc.  
- `components/ui/` — SquareLine Studio project and exported LVGL resources.
- `assets/` — RAW frames (binary image representations). Packed into SPIFFS (`spiffs.bin`) at build time. 

---

# Features 

- **Quiz mode with two screens:**
  - **Question screen:** question text, answer choices, `Answer` button.
  - **Image screen:** `ui_img` widget (current photo), `learn more` button (speaks the associated text via HxTTS), `Change` button (arrow) — jumps to the **next question**.  
    **Cycle:** *Question i* → `Answer` → *Image i* → `Change` → *Question i+1* → … (looped).
- **SPIFFS → PSRAM → LVGL image pipeline:** binary RAW frames are stored in the flash FS. Before display, a frame is loaded entirely into a PSRAM buffer and rendered via LVGL/driver.
- **Image ↔ description binding:** each img has an associated descriptive text. The `learn more` button sends it to HxTTS for speech synthesis.
- **Embedded device focus:** Designed for ELECROW CrowPanel Advance 5.0-HMI. For detailed device hardware information, see [Device Hardware Documentation](https://www.elecrow.com/pub/wiki/CrowPanel_Advance_5.0-HMI_ESP32_AI_Display.html).  
- **HxTTS control:** Load text into the buffer, trigger playback, monitor playback status, and adjust volume using the GRC HxTTS module. For details, see [HxTTS repository](https://github.com/Grovety/HxTTS).  
- **Persistent settings:** Saved to NVS / file for convenient reuse.  

---

# Usage 

> **IMPORTANT:** Order of actions matters — first connect the CrowPanel to your computer via USB/Serial, then flash the board, then connect the GRC HxTTS module.

1. **Hardware setup (initial)**  
   - Set the function-select switch on the CrowPanel to **WM**(0,1) (UART1-OUT mode).  
   - Connect the CrowPanel to the PC via USB/Serial.  
   - **Do NOT connect the GRC HxTTS module yet.** The HxTTS module must be connected only after the panel firmware has been flashed (see note below).  
  
2. **Flashing the board**
 
   ### Building from Source

   1. Install ESP-IDF framework v5.4.  
   2. Clone the repository.  
   3. Dependencies are managed via the ESP-IDF component manager (`idf_component.yml`).  
   4. Build and flash with:  
      ```bash
      idf.py build
      idf.py -p PORT flash
      ```

   ### Flashing Prebuilt Images

- Use the strict flasher in `firmware/` (see `firmware/flash_tool.md`).
- Required files in `firmware/binaries/`:
  - `bootloader.bin` @ `0x0000`
  - `partition-table.bin` @ `0x8000`
  - `proj.bin` @ `0x10000`
  - `spiffs.bin` @ `0x110000`
  
3. **Hardware setup (post-flash)**  
   - After flashing completes and the device has rebooted, connect the GRC HxTTS module (UART0) and CrowPanel (UART1-OUT) using a 4-pin adapter.  
   - Connect the audio output (3.5 mm mini jack) from the HxTTS module to the speaker/headphones.
  
4. **Screen Logic & Controls**  
- **Question screen:** shows the question text and answer choices. The **`Answer`** button navigates to the image screen for the **same** item.
- **Image screen:** `ui_img` displays the frame from SPIFFS; 
  - **`Learn more`** — sends the associated descriptive text to HxTTS and starts playback;  
  - **`Change`** (arrow) — navigates to the **next question** (item *i+1*; looped).
- Service statuses/errors are visible in the serial log.

---

# Asset Subsystem (SPIFFS → PSRAM → LVGL)

- **Storage:** RAW frames reside in SPIFFS.
- **Rendering:** a frame is read **entirely** into a PSRAM buffer, then displayed via LVGL/driver.

---

# UI (SquareLine Studio / LVGL)

- UI sources: `components/ui/` (SquareLine project + generated LVGL files).
- **Screen 1 — question:**
  - question text, answer choices;
  - `Answer` button → navigates to the image screen for the current item.
- **Screen 2 — image:**
  - `ui_img` widget — current photo;
  - `learn more` button — speak the associated text via HxTTS;
  - `Change` button (arrow) — navigate to the **next question**.  
![](images/ui_scr2.png)
![](images/ui_scr1.png)  
---

# Dependencies

- **ESP-IDF Components:** All firmware dependencies are declared in `idf_component.yml` and are fetched automatically by the ESP-IDF component manager.  
- **LVGL 8.4:** Used for building the user interface (SquareLine Studio exports target LVGL).  
- **GRC HxTTS integration:** Serial/UART transport and register-level protocol required by the HxTTS module.  
