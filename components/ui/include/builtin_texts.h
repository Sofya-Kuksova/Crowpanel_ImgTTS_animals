#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Выбери нужный кейс, просто поменяв define:
#define BUILTIN_TEXT_CASE  CASE_STORY_KIDS // ← поменяй на любой из enum ниже

typedef enum {
    CASE_HELLO = 0,
    CASE_RU_SHORT,
    CASE_STORY_KIDS,
    CASE_TECH_DEMO,
} builtin_text_case_t;

// Вернёт строку по выбранному BUILTIN_TEXT_CASE
const char* get_builtin_text(void);

#ifdef __cplusplus
}
#endif
