#include "ui_events.h"
#include "ui.h"                 // ui_Img, ui_btnsay
#include "tts_bridge.h"
#include "builtin_texts.h"
#include "esp_log.h"
#include "lvgl.h"

#include "esp_heap_caps.h"  

static const char* TAG_UI = "ui_events";

// таблица (кейс -> картинка); имена совпадают с твоими extern в ui.h
typedef void (*img_loader_t)(void);
typedef struct { const lv_img_dsc_t* img; img_loader_t load; } case_visual_t;

// --- НОВОЕ: функция, определяющая «прикреплённые» картинки
static inline bool is_pinned_image(const lv_img_dsc_t* d)
{
    // держим ui_img_1049104300 всегда в памяти
    return d == &ui_img_1049104300;
}

static const case_visual_t kVisuals[CASE_TXT_COUNT] = {
    [CASE_TXT_01] = { &ui_img_020_png, ui_img_020_png_load }, // Old Beacon, Norway
    [CASE_TXT_02] = { &ui_img_03_png,  ui_img_03_png_load  }, // Clocks Yard, Prague
    [CASE_TXT_03] = { &ui_img_04_png,  ui_img_04_png_load  }, // Deserted Railway Station, Argentina
    [CASE_TXT_04] = { &ui_img_05_png,  ui_img_05_png_load  }, // Village Covered with Snow, Japan
    [CASE_TXT_05] = { &ui_img_06_png,  ui_img_06_png_load  }, // Bridge across the River, Georgia
    [CASE_TXT_06] = { &ui_img_07_png,  ui_img_07_png_load  }, // Graffiti alley, Lisbon
    [CASE_TXT_07] = { &ui_img_08_png,  ui_img_08_png_load  }, // Desert Well, Morocco
    [CASE_TXT_08] = { &ui_img_09_png,  ui_img_09_png_load  }, // Misty Field, Poland
    [CASE_TXT_09] = { &ui_img_10_png,  ui_img_10_png_load  }, // Misty Field, Poland
    [CASE_TXT_10] = { &ui_img_11_png,  ui_img_11_png_load  }, // Misty Field, Poland
    [CASE_TXT_11] = { &ui_img_12_png,  ui_img_12_png_load  }, // Misty Field, Poland
    [CASE_TXT_12] = { &ui_img_13_png,  ui_img_13_png_load  }, // Misty Field, Poland
    [CASE_TXT_13] = { &ui_img_14_png,  ui_img_14_png_load  }, // Misty Field, Poland
    [CASE_TXT_14] = { &ui_img_15_png,  ui_img_15_png_load  }, // Misty Field, Poland
};

// Освободить все загруженные буферы, кроме исключения и «прикреплённых»
static void free_all_images_except(const lv_img_dsc_t* except_dsc)
{
    for (int i = 0; i < CASE_TXT_COUNT; ++i) {
        const lv_img_dsc_t* d = kVisuals[i].img;
        if (!d || d == except_dsc) continue;
        if (is_pinned_image(d)) continue;        // <-- НЕ трогаем pinned

        if (d->data) {
            ESP_LOGD(TAG_UI, "free image buffer: %p (case %d)", d->data, i);
            heap_caps_free((void*)d->data);
            ((lv_img_dsc_t*)d)->data = NULL;
            ((lv_img_dsc_t*)d)->data_size = 0;
        }
    }
}

void apply_image_for_case(builtin_text_case_t c)
{
    if (!ui_Img) return;
    if (c < 0 || c >= CASE_TXT_COUNT) return;

    const void* cur_src = lv_img_get_src(ui_Img);
    const lv_img_dsc_t* cur_dsc = NULL;
    if (cur_src && lv_img_src_get_type(cur_src) == LV_IMG_SRC_VARIABLE) {
        cur_dsc = (const lv_img_dsc_t*)cur_src;
    }

    free_all_images_except(cur_dsc);

    const case_visual_t v = kVisuals[c];
    if (v.load) v.load();
    if (v.img)  lv_img_set_src(ui_Img, v.img);

    if (cur_dsc && cur_dsc != v.img && cur_dsc->data) {
        if (!is_pinned_image(cur_dsc)) {         // <-- НЕ освобождать pinned
            ESP_LOGD(TAG_UI, "free previous image buffer: %p", cur_dsc->data);
            heap_caps_free((void*)cur_dsc->data);
            ((lv_img_dsc_t*)cur_dsc)->data = NULL;
            ((lv_img_dsc_t*)cur_dsc)->data_size = 0;
        }
    }
}

void on_btn_change_pressed(lv_event_t * e)
{
    (void)e;
    builtin_text_next();
    builtin_text_case_t c = builtin_text_get();
    ESP_LOGI(TAG_UI, "Built-in text changed to case #%d", (int)c);
    apply_image_for_case(c);
}

void on_btn_say_pressed(lv_event_t * e)
{
    (void)e;
    const char* text = get_builtin_text();
    lv_obj_add_state(ui_btnsay, LV_STATE_DISABLED);  // опционально
    start_tts_playback_c(text);
}

static void ui_notify_tts_finished_async(void *arg)
{
    (void)arg;
    lv_obj_clear_state(ui_btnsay, LV_STATE_DISABLED);
}

void ui_notify_tts_finished(void)
{
    lv_async_call(ui_notify_tts_finished_async, NULL);
}
