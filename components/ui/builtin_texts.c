#include "builtin_texts.h"

const char* get_builtin_text(void)
{
    switch ((builtin_text_case_t)BUILTIN_TEXT_CASE) {
        case CASE_HELLO:
            return "Hello! This is a built-in text to speech sample.";
        case CASE_RU_SHORT:
            return "Привет! Это встроенный пример текста для синтеза речи.";
        case CASE_STORY_KIDS:
            return "Once upon a time, a tiny robot learned to speak and tell bedtime stories.";
        case CASE_TECH_DEMO:
            return "System check. Buffer ready. Starting playback demonstration.";
        default:
            return "Default text.";
    }
}
