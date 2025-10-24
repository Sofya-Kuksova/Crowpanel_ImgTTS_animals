#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"

static const char *TAG = "UIIMG";

/* Превращаем "S:assets/xxx.bin" -> "/spiffs/assets/xxx.bin" */
static void map_path(char *dst, size_t dst_sz, const char *src)
{
    if (src && src[0] == 'S' && src[1] == ':') {
        const char *p = src + 2;   // после "S:"
        if (*p == '/') p++;        // пропустить возможный '/'
        snprintf(dst, dst_sz, "/spiffs/%s", p);  // у тебя файлы лежат в /spiffs/assets/...
    } else {
        snprintf(dst, dst_sz, "%s", src ? src : "");
    }
}

/* Прямая загрузка бинарника в PSRAM */
uint8_t* _ui_load_binary_direct(const char* fname_S, uint32_t size)
{
    char real[256];
    map_path(real, sizeof(real), fname_S);

    ESP_LOGI(TAG, "load %s (%u bytes)", real, (unsigned)size);

    FILE *fp = fopen(real, "rb");
    if (!fp) {
        ESP_LOGE(TAG, "fopen failed: %s", real);
        return NULL;
    }

    uint8_t *buf = (uint8_t*)heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) {
        ESP_LOGE(TAG, "heap_caps_malloc(%u) failed", (unsigned)size);
        fclose(fp);
        return NULL;
    }

    size_t rd = fread(buf, 1, size, fp);
    fclose(fp);

    if (rd != size) {
        ESP_LOGE(TAG, "fread short: %u/%u", (unsigned)rd, (unsigned)size);
        heap_caps_free(buf);
        return NULL;
    }

    return buf;
}
