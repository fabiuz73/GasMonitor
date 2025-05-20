#include "gps_screen.h"
#include <Arduino.h>
void create_main_menu();

void show_gps_screen(lv_event_t *e) {
    Serial.println("Callback: Schermata GPS aperta");
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Schermata GPS");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 100, 40);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT " Indietro");
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, [](lv_event_t *e){
        Serial.println("Ritorno al menu principale");
        create_main_menu();
    }, LV_EVENT_CLICKED, NULL);
}