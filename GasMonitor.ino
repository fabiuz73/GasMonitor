#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// Touchscreen pins
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

// Funzioni per il menu
void show_sensors_screen(lv_event_t *e) {
  Serial.println("Callback: Schermata Sensori aperta");
}

void show_battery_screen(lv_event_t *e) {
  Serial.println("Callback: Schermata Batteria aperta");
}

void show_settings_screen(lv_event_t *e) {
  Serial.println("Callback: Schermata Setting aperta");
}

void show_sd_screen(lv_event_t *e) {
  Serial.println("Callback: Schermata SD aperta");
}

void show_weather_screen(lv_event_t *e) {
  Serial.println("Callback: Schermata Meteo aperta");
}

void show_gps_screen(lv_event_t *e) {
  Serial.println("Callback: Schermata GPS aperta");
}

void show_chart_menu(lv_event_t *e) {
  Serial.println("Callback: Schermata Grafici aperta");
}

void start_system(lv_event_t *e) {
  Serial.println("Callback: Sistema Avviato");
}

// Funzione per creare il menu principale con layout 2x4 e miglioramenti grafici
void create_main_menu() {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean(scr);  // Pulisci lo schermo attuale

  Serial.println("Menu principale caricato con layout 2x4 e miglioramenti grafici");

  // Titolo
  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "GasMonitor Menu");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  // Griglia: 2 colonne, 4 righe
  static lv_coord_t col_dsc[] = {110, 110, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

  lv_obj_t *container = lv_obj_create(scr);
  lv_obj_set_size(container, SCREEN_WIDTH, SCREEN_HEIGHT - 40);  // Lascia spazio per il titolo
  lv_obj_align(container, LV_ALIGN_CENTER, 0, 20);
  lv_obj_set_layout(container, LV_LAYOUT_GRID);
  lv_obj_set_grid_dsc_array(container, col_dsc, row_dsc);

  // Stile per i pulsanti
  lv_style_t btn_style;
  lv_style_init(&btn_style);
  lv_style_set_radius(&btn_style, 10);
  lv_style_set_pad_all(&btn_style, 10);
  lv_style_set_border_width(&btn_style, 2);

  // Funzione helper per creare un pulsante
  auto create_button = [&](const char *text, lv_color_t color, int col, int row, lv_event_cb_t event_cb) {
    lv_obj_t *btn = lv_btn_create(container);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_size(btn, 100, 50);  // Dimensione uniforme per tutti i pulsanti
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_bg_color(btn, color, LV_STATE_DEFAULT);
    lv_obj_add_style(btn, &btn_style, LV_STATE_DEFAULT);
    return btn;
  };

  // Creazione dei pulsanti con colori distinti
  create_button("Sensori", lv_palette_main(LV_PALETTE_RED), 0, 0, show_sensors_screen);
  create_button("Batteria", lv_palette_main(LV_PALETTE_GREEN), 1, 0, show_battery_screen);
  create_button("Setting", lv_palette_main(LV_PALETTE_BLUE), 0, 1, show_settings_screen);
  create_button("SD", lv_palette_main(LV_PALETTE_ORANGE), 1, 1, show_sd_screen);
  create_button("Meteo", lv_palette_main(LV_PALETTE_TEAL), 0, 2, show_weather_screen);
  create_button("GPS", lv_palette_main(LV_PALETTE_INDIGO), 1, 2, show_gps_screen);
  create_button("Grafici", lv_palette_main(LV_PALETTE_CYAN), 0, 3, show_chart_menu);
  create_button("Avvia", lv_palette_main(LV_PALETTE_PINK), 1, 3, start_system);
}

void setup() {
  Serial.begin(115200);  // Inizializza il monitor seriale
  Serial.println("Inizializzazione del Sistema GasMonitor...");

  lv_init();

  // Inizializzazione touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);  // Rotazione verticale
  lv_display_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
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

  create_main_menu();
}

void loop() {
  lv_task_handler();  // Gestione degli eventi LVGL
  lv_tick_inc(5);     // Incremento del timer di LVGL
  delay(5);           // Ritardo per evitare sovraccarichi
}