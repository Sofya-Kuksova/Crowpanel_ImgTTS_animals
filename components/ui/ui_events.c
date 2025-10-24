#include "ui_events.h"
#include "ui.h"           
#include <string.h>
#include <stdlib.h>

#include "tts_bridge.h"
#include "builtin_texts.h"


void on_btn_change_pressed(lv_event_t * e)
{
	// Your code here
}

/* ----------------- UI callbacks ----------------- */


void on_btn_say_pressed(lv_event_t * e)
{
    (void)e;
    const char* text = get_builtin_text();  // берём выбранный кейс
    start_tts_playback_c(text);             // используем уже существующий путь
}


/* ----------------- TTS finished notification ----------------- */

static void ui_notify_tts_finished_async(void *arg)
{
    (void)arg;
    lv_obj_clear_state(ui_btnsay, LV_STATE_DISABLED);
}

void ui_notify_tts_finished(void)
{
    lv_async_call(ui_notify_tts_finished_async, NULL);
}
