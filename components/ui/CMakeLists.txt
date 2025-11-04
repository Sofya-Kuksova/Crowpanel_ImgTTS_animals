set(SRCS 
    ui_Screen1.c
    ui_Screen2.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_font_Font1.c
    ui_font_Font3.c
    ui_font_Font4.c
    ui_font_Font5.c
    ui_img_manager.c

    # IMAGES — you MUST enumerate all ui_img_XX_png.c you actually use.
    # Add new files here when you add new cases (CASE_TXT_11, 12, ...).
    ui_img_01_png.c;
    ui_img_02_png.c;
    ui_img_03_png.c;
    ui_img_04_png.c;
    ui_img_05_png.c;
    ui_img_06_png.c;
    ui_img_07_png.c;
    ui_img_08_png.c;
    ui_img_09_png.c;
    ui_img_10_png.c;

    # Additional static assets (e.g., icons)
    ui_img_1049104300.c
    tts_bridge_impl.c
    builtin_texts.c )

set(INCLUDE_DIRS "include")

idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    REQUIRES lvgl__lvgl espressif__esp32_display_panel
)
