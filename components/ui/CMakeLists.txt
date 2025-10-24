set(SRCS 
    ui_Screen1.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_font_Font1.c
    ui_img_manager.c
    ui_img_792570069.c
    ui_img_1049104300.c
    tts_bridge_impl.c
    builtin_texts.c )

# при необходимости укажите папки заголовков, например "include"
set(INCLUDE_DIRS "include")

# регистрируем компонент через ESP-IDF API — НИКОГДА не используйте add_library здесь
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    REQUIRES lvgl__lvgl espressif__esp32_display_panel
)
