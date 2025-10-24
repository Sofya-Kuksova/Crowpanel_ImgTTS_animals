# CrowPanel TTS Console — ESP32-S3 and Desktop App for Text Input

**Summary:** This repository combines firmware for ELECROW CrowPanel Advance 5.0 (ESP32-S3, 800×480 display) with an LVGL-based UI and a desktop Python application for entering and sending text. The text displayed on the panel is forwarded via UART to the GRC HxTTS module, which synthesizes and plays speech based on its phoneme table.

---

## Repository Structure

- `firmware/` — flashing utility and prebuilt binaries.  
- `main/` — firmware source code (ESP-IDF): LVGL UI, UART manager, HxTTS integration, etc.  
- `python/app/` — desktop GUI application for entering and sending text.  
- `components/ui/` — SquareLine Studio project and exported LVGL resources.  

---

# Features 

- **End-to-end integration:** Desktop → CrowPanel → GRC HxTTS (text → speech).  
- **Embedded device focus:** Designed for ELECROW CrowPanel Advance 5.0-HMI. For detailed device hardware information, see [Device Hardware Documentation](https://www.elecrow.com/pub/wiki/CrowPanel_Advance_5.0-HMI_ESP32_AI_Display.html).  
- **Convenient desktop GUI:** Enter text, select COM port, send data, and view logs from the panel.  
- **Reliable transmission:** Compact JSON format with CRC32 checksum validation.  
- **LVGL UI (SquareLine Studio):** Simple interface with a text field (displaying data received from the desktop app) and a button to trigger playback.  
- **HxTTS control:** Load text into the buffer, trigger playback, monitor playback status, and adjust volume using the GRC HxTTS module. For details, see [HxTTS repository](https://github.com/Grovety/HxTTS).  
- **Persistent settings:** Saved to NVS / file for convenient reuse.  

---

# Usage 

> **IMPORTANT:** Order of actions matters — first connect the CrowPanel to your computer via USB/Serial, then flash the board, then connect the GRC HxTTS module, and finally launch the Python application.

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

   - Use `FlashTool_s3.exe` to flash the prebuilt images.
  
3. **Hardware setup (post-flash)**  
   - After flashing completes and the device has rebooted, connect the GRC HxTTS module (UART0) and CrowPanel (UART1-OUT) using a 4-pin adapter.  
   - Connect the audio output (3.5 mm mini jack) from the HxTTS module to the speaker/headphones.

4. **Running the Python app**  
   - Go to `python/app/`.  
   - Launch the app: `app.exe` or `python main.py`.  
   - Select the panel’s COM port, enter text, and press **Send**.  

5. **Verification**  
   - On successful transmission, the panel logs will show `Success`.  
   - The CrowPanel text field will display the received text.  
   - When you press the **Say** button on the panel, the firmware transmits the text to the GRC HxTTS module and triggers playback. The module first allocates and fills its buffer, then executes playback. It converts text into a phoneme table, synthesizes audio (PCM/codec internally), and outputs sound.  

---

# How to use Python app

1. Run `app.exe` from `python/app/` (or `python main.py` from `python`).  
2. In the **Text to Speech** tab, enter your text into the main field.  
3. In the **Settings** tab, select the device’s COM port (use **⟳** to refresh the list).  
4. Return to **Text to Speech** and press **Send** — the text will be sent to the panel.  
   Settings are saved in a platform-specific system config folder inside `CrowPanel/settings.json`.  
5. If you see the following in the logs:  
   ```bash
   Success
   ```  
   it means the panel received and processed the text.  
- Before updating firmware, close the app to release the COM port.  

### Example usage

Below is an example screenshot showing the application with filled fields, COM port selected, and successful response from the panel:

![](images/app_tts.png)  
![](images/app_settings.png)  
![](images/app_success.png)  

---

# Data Transmission Format

**JSON + CRC32** — compact JSON without whitespace (`cJSON_PrintUnformatted`), with field `crc32` containing an 8-digit uppercase HEX CRC32.  

The firmware removes `crc32`, recomputes the checksum on the remaining string, and compares it. If valid, it extracts `text` and forwards it to TTS. Responses include plain text statuses (`Success`, `CRC ERROR`, `Error parsing JSON`).  

---

# UI (SquareLine Studio / LVGL)

- The UI project is in `components/ui`, built in SquareLine Studio and exported to LVGL 8.4.  
- The screen contains:  
  - **Text field** — displays text received from the Python app via UART. This shows the user exactly what will be synthesized by the HxTTS module.  
  - **`Say` button** — when pressed, the firmware sends the text to the HxTTS module, which converts it to speech and outputs audio through its 3.5 mm jack.  
![](images/ui_layout.png)  
---

# Dependencies

- **ESP-IDF Components:** All firmware dependencies are declared in `idf_component.yml` and are fetched automatically by the ESP-IDF component manager.  
- **LVGL 8.4:** Used for building the user interface (SquareLine Studio exports target LVGL).  
- **GRC HxTTS integration:** Serial/UART transport and register-level protocol required by the HxTTS module.  
- **Python app:** Python 3.8+  
