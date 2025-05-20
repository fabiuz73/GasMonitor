#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// Include tutti gli header delle schermate
#include "sensors_screen.h"
#include "battery_screen.h"
#include "settings_screen.h"
#include "sd_screen.h"
#include "weather_screen.h"
#include "gps_screen.h"
#include "charts_screen.h"
#include "start_screen.h"
#include "bluetooth_screen.h"
#include "wifi_manager.h"
#include "gestione_eeprom.h"

// Pin del touchscreen
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
uint32_t draw_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 10];

// ---- CALLBACK per il menu principale ----
static void show_sensors_screen_cb(lv_event_t *e)         { show_sensors_screen(e); }
static void show_battery_screen_cb(lv_event_t *e)         { show_battery_screen(e); }
static void show_settings_menu_cb(lv_event_t *e)          { Serial.println("Premuto: Setting"); show_settings_menu(); }
static void show_sd_screen_cb(lv_event_t *e)              { show_sd_screen(e); }
static void show_weather_screen_cb(lv_event_t *e)         { show_weather_screen(e); }
static void show_gps_screen_cb(lv_event_t *e)             { show_gps_screen(e); }
static void show_charts_screen_cb(lv_event_t *e)          { show_charts_screen(e); }
static void show_start_screen_cb(lv_event_t *e)           { show_start_screen(e); }
static void show_wifi_screen_cb(lv_event_t *e)            { show_wifi_status_and_scan_screen(); }
static void show_bluetooth_screen_cb(lv_event_t *e)       { show_bluetooth_screen(e); }

// Funzione per creare il menu principale con stile colorato, effetto 3D e 2x5 pulsanti
void create_main_menu() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // Barra superiore stile minimal
    lv_obj_t *header = lv_obj_create(scr);
    lv_obj_set_size(header, SCREEN_WIDTH, 42);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_shadow_width(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    // Titolo centrale nella barra
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "GasMonitor");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

    // Griglia menu (2 colonne x 5 righe)
    static lv_coord_t col_dsc[] = {130, 130, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {52, 52, 52, 52, 52, LV_GRID_TEMPLATE_LAST};

    lv_obj_t *container = lv_obj_create(scr);
    lv_obj_set_size(container, SCREEN_WIDTH, SCREEN_HEIGHT - 45);
    lv_obj_align(container, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_layout(container, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(container, col_dsc, row_dsc);
    lv_obj_set_style_bg_color(container, lv_color_white(), 0);
    lv_obj_set_style_radius(container, 16, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_shadow_width(container, 0, 0);

    // Stile icona
    static lv_style_t icon_style;
    lv_style_init(&icon_style);
    lv_style_set_text_color(&icon_style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_font(&icon_style, &lv_font_montserrat_20);

    auto create_button = [&](const char *text, const char *icon_unicode, int col, int row, lv_color_t color1, lv_color_t color2, lv_event_cb_t event_cb) {
        lv_style_t *btn_style = new lv_style_t;
        lv_style_init(btn_style);
        lv_style_set_radius(btn_style, 18);
        lv_style_set_bg_color(btn_style, color1);
        lv_style_set_bg_grad_color(btn_style, color2);
        lv_style_set_bg_grad_dir(btn_style, LV_GRAD_DIR_VER);
        lv_style_set_border_width(btn_style, 2);
        lv_style_set_border_color(btn_style, lv_color_black());
        lv_style_set_bg_opa(btn_style, LV_OPA_COVER);

        lv_style_set_shadow_width(btn_style, 12);
        lv_style_set_shadow_color(btn_style, lv_palette_darken(LV_PALETTE_GREY, 3));
        lv_style_set_shadow_ofs_x(btn_style, 0);
        lv_style_set_shadow_ofs_y(btn_style, 6);
        lv_style_set_border_side(btn_style, LV_BORDER_SIDE_FULL);

        lv_obj_t *btn = lv_btn_create(container);
        lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_size(btn, 120, 44);

        lv_obj_add_style(btn, btn_style, LV_STATE_DEFAULT);

        lv_obj_t *icon = lv_label_create(btn);
        lv_label_set_text(icon, icon_unicode);
        lv_obj_add_style(icon, &icon_style, LV_STATE_DEFAULT);
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 8, 0);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 40, 0);

        return btn;
    };

    create_button("Sensori",    LV_SYMBOL_SAVE,         0, 0, lv_color_hex(0xE53935), lv_color_hex(0xB71C1C), show_sensors_screen_cb);
    create_button("Batteria",   LV_SYMBOL_BATTERY_FULL, 1, 0, lv_color_hex(0x43A047), lv_color_hex(0x1B5E20), show_battery_screen_cb);
    create_button("Setting",    LV_SYMBOL_SETTINGS,     0, 1, lv_color_hex(0x1E88E5), lv_color_hex(0x0D47A1), show_settings_menu_cb); // <-- Modifica qui
    create_button("SD",         LV_SYMBOL_SD_CARD,      1, 1, lv_color_hex(0xFDD835), lv_color_hex(0xF9A825), show_sd_screen_cb);
    create_button("Meteo",      LV_SYMBOL_REFRESH,      0, 2, lv_color_hex(0x00ACC1), lv_color_hex(0x006064), show_weather_screen_cb);
    create_button("GPS",        LV_SYMBOL_GPS,          1, 2, lv_color_hex(0x8E24AA), lv_color_hex(0x4A148C), show_gps_screen_cb);
    create_button("Grafica",    LV_SYMBOL_CHARGE,       0, 3, lv_color_hex(0xF4511E), lv_color_hex(0xBF360C), show_charts_screen_cb);
    create_button("Avvia",      LV_SYMBOL_PLAY,         1, 3, lv_color_hex(0x6D4C41), lv_color_hex(0x3E2723), show_start_screen_cb);
    create_button("WiFi",       LV_SYMBOL_WIFI,         0, 4, lv_color_hex(0x039BE5), lv_color_hex(0x01579B), show_wifi_screen_cb);
    create_button("Bluetooth",  LV_SYMBOL_BLUETOOTH,    1, 4, lv_color_hex(0x76FF03), lv_color_hex(0x33691E), show_bluetooth_screen_cb);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Inizializzazione del Sistema GasMonitor...");

    lv_init();

    // Inizializzazione touchscreen
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(0);

    // Inizializzazione display
    lv_display_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);

    // Inizializzazione touch driver per LVGL
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, [](lv_indev_t *indev, lv_indev_data_t *data) {
        if (touchscreen.tirqTouched() && touchscreen.touched()) {
            TS_Point p = touchscreen.getPoint();
            data->point.x = map(p.x, 200, 3700, 0, SCREEN_WIDTH);
            data->point.y = map(p.y, 240, 3800, 0, SCREEN_HEIGHT);
            data->state = LV_INDEV_STATE_PRESSED;
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    });

    // Inizializza la EEPROM (per gestione reti WiFi)
    init_wifi_eeprom();

    create_main_menu();
}

void loop() {
    lv_task_handler();
    lv_tick_inc(5);
    delay(5);
}