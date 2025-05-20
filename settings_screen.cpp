#include "settings_screen.h"
#include <lvgl.h>

// Puoi sostituire questa funzione con la tua schermata principale
extern void create_main_menu();

static lv_obj_t *settings_list = nullptr;

// Callback vuoto per la voce Connessioni WiFi (resta per futuro utilizzo)
static void on_wifi_connections_clicked(lv_event_t *e) {
    // Funzione da implementare in futuro per la gestione delle connessioni WiFi nelle impostazioni
    // show_saved_wifi_screen();
}

static void on_back_clicked(lv_event_t *e) {
    // Torna al menu principale
    create_main_menu();
}

void show_settings_menu() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    settings_list = lv_list_create(scr);
    lv_obj_set_size(settings_list, 240, 160);
    lv_obj_align(settings_list, LV_ALIGN_CENTER, 0, -10);

    // Voce Connessioni WiFi rimossa dal menu. La funzione resta per sviluppi futuri.

    // Puoi aggiungere altre voci di impostazioni qui

    // Pulsante indietro
    lv_obj_t *back_btn = lv_btn_create(scr);
    lv_obj_set_size(back_btn, 100, 38);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t *lbl = lv_label_create(back_btn);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT " Indietro");
    lv_obj_center(lbl);
    lv_obj_add_event_cb(back_btn, on_back_clicked, LV_EVENT_CLICKED, NULL);
}