#include "wifi_manager.h"
#include <WiFi.h>
#include <lvgl.h>
#include <cstring>

// Puoi sostituire questa funzione con la tua schermata principale
extern void create_main_menu();

static lv_obj_t *wifi_status_label = nullptr;
static lv_obj_t *wifi_scan_list = nullptr;
static lv_obj_t *password_popup = nullptr;
static lv_obj_t *password_ta = nullptr;
static lv_obj_t *kb = nullptr;
static char selected_ssid[SSID_MAXLEN] = {0};
static char last_password[PASSWORD_MAXLEN] = {0};

// --- UTILITY EEPROM ---
bool is_wifi_saved(const char* ssid) {
    for (int i = 0; i < MAX_WIFI_SAVED; ++i) {
        WifiConfig cred = read_wifi_config(i);
        if (strcmp(cred.ssid, ssid) == 0) return true;
    }
    return false;
}

int find_wifi_slot_by_ssid(const char* ssid) {
    for (int i = 0; i < MAX_WIFI_SAVED; ++i) {
        WifiConfig cred = read_wifi_config(i);
        if (strcmp(cred.ssid, ssid) == 0) return i;
    }
    return -1;
}

int find_free_wifi_slot() {
    for (int i = 0; i < MAX_WIFI_SAVED; ++i) {
        WifiConfig cred = read_wifi_config(i);
        if (strlen(cred.ssid) == 0) return i;
    }
    return -1;
}

void print_saved_wifi_slots() {
    Serial.println("Stato slot reti salvate:");
    for (int i = 0; i < MAX_WIFI_SAVED; ++i) {
        WifiConfig cred = read_wifi_config(i);
        Serial.printf("Slot %d: SSID='%s' PWD='%s'\n", i, cred.ssid, cred.password);
    }
}

// --- POPUP PASSWORD ---
static void close_password_popup() {
    if (kb) {
        lv_obj_del(kb);
        kb = nullptr;
    }
    if (password_popup) {
        lv_obj_del(password_popup);
        password_popup = nullptr;
    }
    password_ta = nullptr;
    selected_ssid[0] = 0;
    last_password[0] = 0;
}

static void password_kb_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_READY) {
        const char *pwd = lv_textarea_get_text(password_ta);
        strncpy(last_password, pwd, PASSWORD_MAXLEN-1);
        last_password[PASSWORD_MAXLEN-1] = 0;
        Serial.printf("Password inserita per '%s': %s\n", selected_ssid, last_password);

        if(strlen(selected_ssid) > 0 && strlen(last_password) > 0) {
            Serial.printf("Provo a connettermi a '%s'...\n", selected_ssid);
            WiFi.disconnect(false);
            WiFi.begin(selected_ssid, last_password);

            int slot = find_wifi_slot_by_ssid(selected_ssid);
            if(slot < 0) slot = find_free_wifi_slot();
            if(slot >= 0) {
                save_wifi_config(slot, selected_ssid, last_password);
                Serial.printf("Rete '%s' salvata in slot %d\n", selected_ssid, slot);
            } else {
                Serial.println("Nessuno slot libero per salvare la rete! Cancella una rete salvata dal menu.");
                print_saved_wifi_slots();
            }
        }
        close_password_popup();
    } else if (code == LV_EVENT_CANCEL) {
        Serial.println("Inserimento password annullato.");
        close_password_popup();
    }
}

void show_password_popup(const char* ssid) {
    strncpy(selected_ssid, ssid, SSID_MAXLEN-1);
    selected_ssid[SSID_MAXLEN-1] = 0;

    Serial.printf("Richiesta password per rete '%s'\n", selected_ssid);

    password_popup = lv_obj_create(lv_scr_act());
    lv_obj_set_size(password_popup, 220, 120);
    lv_obj_center(password_popup);

    lv_obj_t *label = lv_label_create(password_popup);
    lv_label_set_text_fmt(label, "Password per\n%s:", ssid);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    password_ta = lv_textarea_create(password_popup);
    lv_textarea_set_password_mode(password_ta, true);
    lv_textarea_set_one_line(password_ta, true);
    lv_obj_set_width(password_ta, 170);
    lv_obj_align(password_ta, LV_ALIGN_CENTER, 0, 10);

    kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_textarea(kb, password_ta);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(kb, password_kb_event_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(kb, password_kb_event_cb, LV_EVENT_CANCEL, NULL);
}

// ================================
// 1. Schermata Stato & Scansione
// ================================
void show_wifi_status_and_scan_screen() {
    Serial.println("Apro schermata WiFi...");

    lv_obj_clean(lv_scr_act());

    // Stato attuale (in alto)
    wifi_status_label = lv_label_create(lv_scr_act());
    char buf[128];
    snprintf(buf, sizeof(buf), "Stato: %s\nSSID: %s\nIP: %s\nSegnale: %ddBm",
        WiFi.status() == WL_CONNECTED ? "Connesso" : "Disconnesso",
        WiFi.status() == WL_CONNECTED ? WiFi.SSID().c_str() : "-",
        WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString().c_str() : "-",
        WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0);
    lv_label_set_text(wifi_status_label, buf);
    lv_obj_align(wifi_status_label, LV_ALIGN_TOP_LEFT, 10, 10);

    // Lista reti disponibili (molto in basso)
    wifi_scan_list = lv_list_create(lv_scr_act());
    lv_obj_set_size(wifi_scan_list, 240, 110);
    lv_obj_align(wifi_scan_list, LV_ALIGN_TOP_MID, 0, 90);

    WiFi.scanDelete();
    int n = WiFi.scanNetworks();
    Serial.printf("Trovate %d reti WiFi\n", n);

    for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        int32_t rssi = WiFi.RSSI(i);
        char label[64];
        snprintf(label, sizeof(label), "%s [%ddBm]", ssid.c_str(), rssi);

        lv_obj_t *btn = lv_list_add_btn(wifi_scan_list, LV_SYMBOL_WIFI, label);
        lv_obj_add_event_cb(btn, [](lv_event_t *e){
            int idx = lv_obj_get_index((lv_obj_t*)lv_event_get_target(e));
            Serial.printf("Selezionata rete in scansione, indice %d\n", idx);
            on_wifi_connect_clicked(idx);
        }, LV_EVENT_CLICKED, NULL);
    }

    // --- PULSANTI IN FONDO ALLO SCHERMO ---
    int btn_w = 60, btn_h = 44;
    int btn_y = 250;

    // Indietro (sinistra)
    lv_obj_t *back_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(back_btn, btn_w, btn_h);
    lv_obj_set_pos(back_btn, 20, btn_y);
    lv_obj_t *lbl_back = lv_label_create(back_btn);
    lv_label_set_text(lbl_back, LV_SYMBOL_LEFT);
    lv_obj_center(lbl_back);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0xFF4040), LV_PART_MAIN);
    lv_obj_add_event_cb(back_btn, [](lv_event_t *) {
        Serial.println("Torno al menu principale...");
        create_main_menu();
    }, LV_EVENT_CLICKED, NULL);

    // Aggiorna (centro)
    lv_obj_t *refresh_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(refresh_btn, btn_w, btn_h);
    lv_obj_set_pos(refresh_btn, 90, btn_y);
    lv_obj_t *lbl_refresh = lv_label_create(refresh_btn);
    lv_label_set_text(lbl_refresh, LV_SYMBOL_REFRESH);
    lv_obj_center(lbl_refresh);
    lv_obj_set_style_bg_color(refresh_btn, lv_color_hex(0x40C040), LV_PART_MAIN);
    lv_obj_add_event_cb(refresh_btn, [](lv_event_t *) {
        Serial.println("Aggiorno la scansione WiFi...");
        show_wifi_status_and_scan_screen();
    }, LV_EVENT_CLICKED, NULL);

    // Salvate (destra)
    lv_obj_t *saved_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(saved_btn, btn_w, btn_h);
    lv_obj_set_pos(saved_btn, 160, btn_y);
    lv_obj_t *lbl_saved = lv_label_create(saved_btn);
    lv_label_set_text(lbl_saved, LV_SYMBOL_SAVE);
    lv_obj_center(lbl_saved);
    lv_obj_set_style_bg_color(saved_btn, lv_color_hex(0x4060FF), LV_PART_MAIN);
    lv_obj_add_event_cb(saved_btn, [](lv_event_t *) {
        Serial.println("Apro schermata reti salvate...");
        show_saved_wifi_screen();
    }, LV_EVENT_CLICKED, NULL);
}

// ==================================
// 2. Schermata Reti WiFi Salvate
// ==================================
void show_saved_wifi_screen() {
    Serial.println("Apro schermata reti salvate...");
    print_saved_wifi_slots();

    lv_obj_clean(lv_scr_act());
    lv_obj_t *saved_wifi_list = lv_list_create(lv_scr_act());
    // Ingrandisci il riquadro lista per vedere meglio i tasti
    lv_obj_set_size(saved_wifi_list, 260, 210);
    lv_obj_align(saved_wifi_list, LV_ALIGN_TOP_MID, 0, 10);

    bool any = false;
    for (int i = 0; i < MAX_WIFI_SAVED; ++i) {
        WifiConfig cred = read_wifi_config(i);
        if (strlen(cred.ssid) > 0) {
            any = true;
            char label[SSID_MAXLEN + 32];
            bool is_current = (WiFi.status() == WL_CONNECTED && strcmp(cred.ssid, WiFi.SSID().c_str()) == 0);
            snprintf(label, sizeof(label), "%s%s", cred.ssid, is_current ? " (connesso)" : "");

            lv_obj_t* btn = lv_list_add_btn(saved_wifi_list, LV_SYMBOL_WIFI, label);

            // Container orizzontale allungato per i pulsanti
            lv_obj_t* btn_cont = lv_obj_create(saved_wifi_list);
            lv_obj_set_size(btn_cont, 200, 44); // Allungato, ma non esagerato
            lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_align(btn_cont, LV_ALIGN_TOP_RIGHT, 0, 0);

            // Pulsante Connetti/Disconnetti
            lv_obj_t* connect_btn = lv_btn_create(btn_cont);
            lv_obj_set_size(connect_btn, 90, 36);
            if (is_current) {
                lv_obj_set_style_bg_color(connect_btn, lv_color_hex(0xFFA500), LV_PART_MAIN);
                lv_obj_t* lbl = lv_label_create(connect_btn);
                lv_label_set_text(lbl, "Disconnetti");
                lv_obj_center(lbl);
                lv_obj_add_event_cb(connect_btn, [](lv_event_t *){
                    Serial.println("Disconnessione dalla rete WiFi attuale...");
                    on_wifi_disconnect_clicked();
                }, LV_EVENT_CLICKED, NULL);
            } else {
                lv_obj_set_style_bg_color(connect_btn, lv_color_hex(0x40C040), LV_PART_MAIN);
                lv_obj_t* lbl = lv_label_create(connect_btn);
                lv_label_set_text(lbl, "Connetti");
                lv_obj_center(lbl);
                int idx = i;
                lv_obj_add_event_cb(connect_btn, [](lv_event_t *e){
                    int idx = (intptr_t)lv_event_get_user_data(e);
                    Serial.printf("Connessione a rete salvata slot %d...\n", idx);
                    on_wifi_saved_connect_clicked(idx);
                }, LV_EVENT_CLICKED, (void*)(intptr_t)idx);
            }

            // Pulsante Elimina
            lv_obj_t* del_btn = lv_btn_create(btn_cont);
            lv_obj_set_size(del_btn, 70, 36);
            lv_obj_set_style_bg_color(del_btn, lv_color_hex(0xFF4040), LV_PART_MAIN);
            lv_obj_t* del_lbl = lv_label_create(del_btn);
            lv_label_set_text(del_lbl, "Elimina");
            lv_obj_center(del_lbl);
            int idx = i;
            lv_obj_add_event_cb(del_btn, [](lv_event_t *e){
                int idx = (intptr_t)lv_event_get_user_data(e);
                Serial.printf("Eliminazione della rete salvata slot %d...\n", idx);
                erase_wifi_config(idx);
                show_saved_wifi_screen();
            }, LV_EVENT_CLICKED, (void*)(intptr_t)idx);
        }
    }
    if (!any) {
        lv_obj_t *lbl = lv_label_create(saved_wifi_list);
        lv_label_set_text(lbl, "Nessuna rete salvata.");
        lv_obj_center(lbl);
    }

    // Bottone indietro in basso
    lv_obj_t *back_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(back_btn, 100, 38);
    lv_obj_set_pos(back_btn, 10, 250);
    lv_obj_t *lbl = lv_label_create(back_btn);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT " Indietro");
    lv_obj_center(lbl);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0xFF4040), LV_PART_MAIN);
    lv_obj_add_event_cb(back_btn, [](lv_event_t *){
        Serial.println("Torno alla schermata di scansione WiFi...");
        show_wifi_status_and_scan_screen();
    }, LV_EVENT_CLICKED, NULL);
}

// ==================
// Funzioni LOGICA
// ==================

void on_wifi_connect_clicked(int scan_index) {
    String ssid = WiFi.SSID(scan_index);
    Serial.printf("Hai scelto la rete: %s\n", ssid.c_str());
    int saved_slot = find_wifi_slot_by_ssid(ssid.c_str());
    if (saved_slot >= 0) {
        Serial.printf("Rete '%s' già salvata. Richiedo password (per modifica o riconferma).\n", ssid.c_str());
        show_password_popup(ssid.c_str());
    } else {
        Serial.printf("Rete '%s' non salvata. Richiedo password all'utente.\n", ssid.c_str());
        show_password_popup(ssid.c_str());
    }
}

void on_wifi_saved_connect_clicked(int saved_index) {
    WifiConfig cred = read_wifi_config(saved_index);
    if (strlen(cred.ssid) > 0) {
        Serial.printf("Connessione a rete salvata '%s' (slot %d)...\n", cred.ssid, saved_index);
        WiFi.disconnect(false);
        WiFi.begin(cred.ssid, cred.password);
    }
    show_saved_wifi_screen();
}

void on_wifi_saved_delete_clicked(int saved_index) {
    Serial.printf("Eliminazione della rete salvata slot %d...\n", saved_index);
    erase_wifi_config(saved_index);
    show_saved_wifi_screen();
}

void on_wifi_disconnect_clicked() {
    Serial.println("Disconnessione dalla rete WiFi...");
    WiFi.disconnect(false);
    show_saved_wifi_screen();
}