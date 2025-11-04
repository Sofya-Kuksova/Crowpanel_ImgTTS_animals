# Edit Scenario

## 1) Project Structure

```
components/
  ui/
    include/
      builtin_texts.h          (←) case IDs; keep CASE_TXT_COUNT last        
    builtin_texts.c            (←) Screen1 TTS text per case (get_builtin_text)
    ui_events.c                (←) Screen1 image mapping (kVisuals) + Screen2 Q&A (kQA)
    ui_img_01_png.c            (←) image wrappers (01..10 exist already)
    ui_img_02_png.c
    ...
    ui_img_10_png.c
    CMakeLists.txt             (←) list every ui_img_XX_png.c you actually use
assets/                        (←) .bin files for scenario     
    ui_img_third_01_png.bin
    ui_img_third_02_png.bin
    ui_img_third_03_png.bin
```
---

## 2) Produce RAW Binary Images

### Option A — SquareLine Studio (SLS)

1. Open the project in **SquareLine Studio** (LVGL v8).  
2. Create/Configure export:
   - set the **export path**;
   - set output format to **binary raw**.
3. Import images: *Assets* → **Import** (PNG / UI frame sample size 578×339).  
4. Export/Generate: **Export UI files**.
   - Copy generated `*.bin` from `export_root/drive/assets` to `/assets`.
   - Copy generated C arrays `img_*.c` from your “UI file export path” to `/components/ui`.

### Option B — LVGL Image Converter (official online tool)

1. Open **LVGL Image Converter**.  
2. Upload PNG/JPG.  
3. Parameters:
   - **Color format:** `True color (RGB565)`;
   - **Alpha:** `None` (or as required);
   - **Output:** `Binary`.  
4. Download `*.bin` and place them in `/assets`.  
   For C arrays, select **C array** and save resulting `img_*.c` files into `/components/ui`.

---

## 3) Editng scenario Step-by-Step (with inline code anchors)

### 3.1 Define the cases (IDs)

Edit `components/ui/include/builtin_texts.h` — add new case IDs **before** `CASE_TXT_COUNT`. Keep `CASE_TXT_COUNT` last.

```c
typedef enum {
    CASE_TXT_01 = 0,
    /* ... existing ... */
    CASE_TXT_10,
    /* add new cases here */
    // CASE_TXT_11,
    // CASE_TXT_12,
    CASE_TXT_COUNT   /* must stay last */
} builtin_text_case_t;
```

---

### 3.2 Provide TTS text for Screen1

Edit `components/ui/builtin_texts.c` → implement text for every case in `get_builtin_text()` (add a `case` for each new ID).

```c
// components/ui/builtin_texts.c
const char* get_builtin_text(void) {
    switch (builtin_text_get()) {
        case CASE_TXT_01: return "…";
        /* ... existing ... */
        // case CASE_TXT_11: return "Your custom TTS text…";
        default: return "";
    }
}
```

---

### 3.3 Prepare raw image binaries (.bin)

Convert your PNG/JPG to LVGL **binary** (`True color / RGB565`) and place them under:

```
assets/
  ui_img_third_11_png.bin
  ui_img_third_12_png.bin
  ui_img_third_33_png.bin
```

> Use the exact filenames you’ll reference in your C image wrappers (next step).

---

### 3.4 Create C image wrappers (one per image)

After generate **C arrays** for images (`img_*.c`), store them **in the scenario folder**  

```
components/ui/
  ui_img_third_11_png.c
  ui_img_third_22_png.c
  ui_img_third_33_png.c
```

---

### 3.5 Map cases → images for Screen1

Edit `components/ui/ui_events.c`. Add entries to `kVisuals[]` that map each new case to its image descriptor and loader.

```c
typedef void (*img_loader_t)(void);
typedef struct { const lv_img_dsc_t* img; img_loader_t load; } case_visual_t;

static const case_visual_t kVisuals[CASE_TXT_COUNT] = {
    [CASE_TXT_01] = { &ui_img_01_png, ui_img_01_png_load },
    /* ... existing ... */
    // [CASE_TXT_11] = { &ui_img_11_png, ui_img_11_png_load },
};
```

---

### 3.6 Provide Screen2 question & options

In the same file (`components/ui/ui_events.c`), add entries to `kQA[]` for each new case.

```c
typedef struct { const char* question; const char* A; const char* B; const char* C; } qa_t;

static const qa_t kQA[CASE_TXT_COUNT] = {
    [CASE_TXT_01] = { "Which …?", "A", "B", "C" },
    /* ... existing ... */
    // [CASE_TXT_11] = { "Your question?", "Option A", "Option B", "Option C" },
};
```

---

### 3.7 Include your new wrappers in the build

Make sure every new `ui_img_XX_png.c` is listed in `components/ui/CMakeLists.txt`.

```cmake
# components/ui/CMakeLists.txt
set(SRCS
  ...
  ui_img_01_png.c;
  ...
  ui_img_10_png.c;
  # add your new wrappers:
  # ui_img_11_png.c;
  # ui_img_12_png.c;
  ...
)
```

---

### 3.8 Rebuild (clean when assets change)

```
idf.py fullclean
idf.py build
idf.py -p COMx flash monitor
```

> The clean build ensures the SPIFFS image is rebuilt with your new `.bin` files.
