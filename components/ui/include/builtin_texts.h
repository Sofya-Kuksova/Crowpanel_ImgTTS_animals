#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CASE_TXT_01 = 0,
    CASE_TXT_02,
    CASE_TXT_03,
    CASE_TXT_04,
    CASE_TXT_05,
    CASE_TXT_06,
    CASE_TXT_07,
    CASE_TXT_08,
    CASE_TXT_09,
    CASE_TXT_10,
    /* Add more here if needed:
       CASE_TXT_11,
       CASE_TXT_12, */
    CASE_TXT_COUNT  /* total number of cases — must remain last */
} builtin_text_case_t;

const char* get_builtin_text(void);
void builtin_text_next(void);
void builtin_text_set(builtin_text_case_t c);
builtin_text_case_t builtin_text_get(void);

#ifdef __cplusplus
}
#endif
