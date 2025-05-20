#pragma once

#define SSID_MAXLEN     32
#define PASSWORD_MAXLEN 64
#define MAX_WIFI_SAVED  5

typedef struct {
    char ssid[SSID_MAXLEN];
    char password[PASSWORD_MAXLEN];
} WifiConfig;

// Inizializza la EEPROM per la gestione WiFi
void init_wifi_eeprom();

// Legge la configurazione WiFi da uno slot
WifiConfig read_wifi_config(int slot);

// Salva la configurazione WiFi in uno slot
void save_wifi_config(int slot, const char* ssid, const char* password);

// Cancella una configurazione WiFi dallo slot
void erase_wifi_config(int slot);

// Cancella tutte le reti salvate
void erase_all_wifi_configs();