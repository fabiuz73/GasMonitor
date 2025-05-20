#include "gestione_eeprom.h"
#include <EEPROM.h>
#include <string.h>

#define EEPROM_SIZE (MAX_WIFI_SAVED * (SSID_MAXLEN + PASSWORD_MAXLEN))

void init_wifi_eeprom() {
    EEPROM.begin(EEPROM_SIZE);
}

WifiConfig read_wifi_config(int slot) {
    WifiConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    if (slot < 0 || slot >= MAX_WIFI_SAVED) return cfg;
    int base = slot * (SSID_MAXLEN + PASSWORD_MAXLEN);
    for (int i = 0; i < SSID_MAXLEN; ++i)
        cfg.ssid[i] = EEPROM.read(base + i);
    cfg.ssid[SSID_MAXLEN-1] = 0;
    for (int i = 0; i < PASSWORD_MAXLEN; ++i)
        cfg.password[i] = EEPROM.read(base + SSID_MAXLEN + i);
    cfg.password[PASSWORD_MAXLEN-1] = 0;
    return cfg;
}

void save_wifi_config(int slot, const char* ssid, const char* password) {
    if (slot < 0 || slot >= MAX_WIFI_SAVED) return;
    int base = slot * (SSID_MAXLEN + PASSWORD_MAXLEN);
    for (int i = 0; i < SSID_MAXLEN; ++i)
        EEPROM.write(base + i, ssid && i < (int)strlen(ssid) ? ssid[i] : 0);
    for (int i = 0; i < PASSWORD_MAXLEN; ++i)
        EEPROM.write(base + SSID_MAXLEN + i, password && i < (int)strlen(password) ? password[i] : 0);
    EEPROM.commit();
}

void erase_wifi_config(int slot) {
    if (slot < 0 || slot >= MAX_WIFI_SAVED) return;
    int base = slot * (SSID_MAXLEN + PASSWORD_MAXLEN);
    for (int i = 0; i < (SSID_MAXLEN + PASSWORD_MAXLEN); ++i)
        EEPROM.write(base + i, 0);
    EEPROM.commit();
}

void erase_all_wifi_configs() {
    for (int slot = 0; slot < MAX_WIFI_SAVED; ++slot)
        erase_wifi_config(slot);
}