#include "ui_events.h"
#include "ui.h"                 
#include "tts_bridge.h"
#include "builtin_texts.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_heap_caps.h"
#include <stdint.h> 

static const char* TAG_UI = "ui_events";

static lv_timer_t* s_question_tts_timer = NULL;

typedef void (*img_loader_t)(void);
typedef struct { const lv_img_dsc_t* img; img_loader_t load; } case_visual_t;


static const case_visual_t kVisuals[CASE_TXT_COUNT] = {
    [CASE_TXT_01] = { &ui_img_01_png, ui_img_01_png_load },
    [CASE_TXT_02] = { &ui_img_02_png, ui_img_02_png_load },
    [CASE_TXT_03] = { &ui_img_03_png, ui_img_03_png_load },
    [CASE_TXT_04] = { &ui_img_04_png, ui_img_04_png_load },
    [CASE_TXT_05] = { &ui_img_05_png, ui_img_05_png_load },
    [CASE_TXT_06] = { &ui_img_06_png, ui_img_06_png_load },
    [CASE_TXT_07] = { &ui_img_07_png, ui_img_07_png_load },
    [CASE_TXT_08] = { &ui_img_08_png, ui_img_08_png_load },
    [CASE_TXT_09] = { &ui_img_09_png, ui_img_09_png_load },
    [CASE_TXT_10] = { &ui_img_10_png, ui_img_10_png_load },

    /* Example for CASE_TXT_11 (custom):
    [CASE_TXT_11] = { &ui_img_11_png, ui_img_11_png_load },
    */
};

// -------------------- QUESTION BANK FOR Screen2 --------------------
typedef struct {
    const char* question;
    const char* A;
    const char* B;
    const char* C;
} qa_t;

static const qa_t kQA[CASE_TXT_COUNT] = {
    [CASE_TXT_01] = { "Which animal sleeps standing on one leg ?", "Elephant", "Flamingo", "Kangaroo" },
    [CASE_TXT_02] = { "Which animal can go without water for a whole week ?", "Camel", "Penguin", "Dolphin" },
    [CASE_TXT_03] = { "Who can change color to disappear in the ocean ?", "Chameleon", "Octopus", "Owl" },
    [CASE_TXT_04] = { "Who rolls into a spiky ball when scared ?", "Ant", "Hedgehog", "Elephant" },
    [CASE_TXT_05] = { "Who changes color not just for hiding, but for mood ?", "Chameleon", "Flamingo", "Camel" },
    [CASE_TXT_06] = { "Who talks without opening their mouth ?", "Dolphin", "Owl", "Penguin" },
    [CASE_TXT_07] = { "Which tiny creature can lift 50 times its own weight ?", "Ant", "Hedgehog", "Octopus" },
    [CASE_TXT_08] = { "Who can see in the dark better than anyone ?", "Owl", "Dolphin", "Chameleon" },
    [CASE_TXT_09] = { "Which bird can't fly, but swims perfectly ?", "Penguin", "Flamingo", "Owl" },
    [CASE_TXT_10] = { "Who remembers everything, even what never happened ?", "Elephant", "Camel", "Ant" },
    /* Example for a custom case:
    [CASE_TXT_11] = { "Your question?", "Option A", "Option B", "Option C" },
    */
};

static void tts_question_timer_cb(lv_timer_t* t)
{
    builtin_text_case_t c = (builtin_text_case_t)(uintptr_t)t->user_data;

    if (c >= 0 && c < CASE_TXT_COUNT) {
        const char* q = kQA[c].question;
        if (q && *q) {
            start_tts_playback_c(q);
        }
    }
    
    s_question_tts_timer = NULL;   
}

static bool is_pinned_image(const lv_img_dsc_t* dsc)
{
    return (dsc == &ui_img_1049104300);
}

static void free_all_other_images(const lv_img_dsc_t* except_dsc)
{
    for (int i = 0; i < CASE_TXT_COUNT; ++i) {
        const lv_img_dsc_t* d = kVisuals[i].img;
        if (!d || d == except_dsc) continue;
        if (is_pinned_image(d)) continue;

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
    if (c < 0 || c >= CASE_TXT_COUNT) return;

    const case_visual_t *cv = &kVisuals[c];
    if (cv->load) cv->load();                  

    if (ui_Img) {
        lv_img_set_src(ui_Img, cv->img);       
    }
    builtin_text_set(c);                       
    free_all_other_images(cv->img);            
}

static void fill_screen2_for_case(builtin_text_case_t c)
{
    if (!ui_Screen2) return;
    if (c < 0 || c >= CASE_TXT_COUNT) return;
    const qa_t* qa = &kQA[c];

    if (ui_que)   lv_label_set_text(ui_que, qa->question);
    if (ui_labA)  lv_label_set_text(ui_labA, qa->A);
    if (ui_LabB)  lv_label_set_text(ui_LabB, qa->B); 
    if (ui_LabC)  lv_label_set_text(ui_LabC, qa->C);

    if (s_question_tts_timer) {
        lv_timer_del(s_question_tts_timer);  
        s_question_tts_timer = NULL;
    }
    s_question_tts_timer = lv_timer_create(tts_question_timer_cb, 1000 /* ms */, (void*)(uintptr_t)c);
    lv_timer_set_repeat_count(s_question_tts_timer, 1); 

}


// -------------------- HANDLERS --------------------

void on_btn_change_pressed(lv_event_t * e)
{
    (void)e;
    builtin_text_next();
    builtin_text_case_t c = builtin_text_get();

    if (!ui_Screen2) {
        ui_Screen2_screen_init();
    }
    fill_screen2_for_case(c);
    lv_disp_load_scr(ui_Screen2);              
}

void on_btn_say_pressed(lv_event_t * e)
{
    (void)e;
    const char* text = get_builtin_text();
    lv_obj_add_state(ui_btnsay, LV_STATE_DISABLED);
    start_tts_playback_c(text);
}

void on_btn_answer_pressed(lv_event_t * e)
{
    (void)e;

    if (s_question_tts_timer) {
        lv_timer_del(s_question_tts_timer);
        s_question_tts_timer = NULL;
    }

    builtin_text_case_t c = builtin_text_get();

    if (!ui_Screen1) {
        ui_Screen1_screen_init();
    }
    apply_image_for_case(c);
    lv_disp_load_scr(ui_Screen1);
}

void on_text_update_from_uart(const char *text)
{
    (void)text;
}

static void ui_notify_tts_finished_async(void *arg)
{
    (void)arg;
    if (ui_btnsay) {
        lv_obj_clear_state(ui_btnsay, LV_STATE_DISABLED);
    }
}

void ui_notify_tts_finished(void)
{
    lv_async_call(ui_notify_tts_finished_async, NULL);
}

void ui_show_question_current_case(void)
{
    builtin_text_case_t c = builtin_text_get();    
    if (!ui_Screen2) {
        ui_Screen2_screen_init();
    }
    fill_screen2_for_case(c);
    lv_disp_load_scr(ui_Screen2);
}