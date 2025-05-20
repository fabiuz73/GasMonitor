#pragma once
#include "gestione_eeprom.h"

// Mostra schermata stato e reti disponibili
void show_wifi_status_and_scan_screen();

// Mostra schermata reti salvate
void show_saved_wifi_screen();

// Gestione eventi clic scansione/salvate/disconnetti
void on_wifi_connect_clicked(int scan_index);
void on_wifi_saved_connect_clicked(int saved_index);
void on_wifi_saved_delete_clicked(int saved_index);
void on_wifi_disconnect_clicked();

// Mostra popup password
void show_password_popup(const char* ssid);