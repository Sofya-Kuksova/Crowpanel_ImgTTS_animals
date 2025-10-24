#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Прямой загрузчик бинарных картинок:
 * - конвертирует "S:assets/xxx.bin" -> "/spiffs/assets/xxx.bin"
 * - читает файл через stdio (fopen/fread)
 * - аллоцирует буфер в PSRAM (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)
 *
 * Возвращает указатель на буфер или NULL при ошибке.
 */
uint8_t* _ui_load_binary_direct(const char* fname_S, uint32_t size);

/* SquareLine вызывает UI_LOAD_IMAGE(...), подменяем на нашу функцию */
#define UI_LOAD_IMAGE _ui_load_binary_direct

#ifdef __cplusplus
}
#endif
