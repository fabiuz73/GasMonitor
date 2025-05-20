#include "weather_screen.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "weather_images.h"

// Dati posizione/località
String latitude = "44.3177";
String longitude = "9.3224";
String location = "Caperana";
String timezone = "Europe/Rome";

// Variabili per i dati meteo
String current_date;
String last_weather_update;
String temperature;
String humidity;
int is_day;
int weather_code = 0;
String weather_description;

// Celsius o Fahrenheit
#define TEMP_CELSIUS 1
#if TEMP_CELSIUS
  String temperature_unit = "";
  const char degree_symbol[] = "\u00B0C";
#else
  String temperature_unit = "&temperature_unit=fahrenheit";
  const char degree_symbol[] = "\u00B0F";
#endif

// Oggetti LVGL globali
static lv_obj_t * weather_image;
static lv_obj_t * text_label_date;
static lv_obj_t * text_label_temperature;
static lv_obj_t * text_label_humidity;
static lv_obj_t * text_label_weather_description;
static lv_obj_t * text_label_time_location;

// Timer LVGL per aggiornamento dati
static lv_timer_t * weather_timer = nullptr;

// Forward declaration (deve essere visibile globalmente)
extern void create_main_menu();

// Funzione: aggiorna etichette e icona meteo
static void get_weather_description(int code) {
    LV_IMAGE_DECLARE(image_weather_sun);
    LV_IMAGE_DECLARE(image_weather_cloud);
    LV_IMAGE_DECLARE(image_weather_rain);
    LV_IMAGE_DECLARE(image_weather_thunder);
    LV_IMAGE_DECLARE(image_weather_snow);
    LV_IMAGE_DECLARE(image_weather_night);

    switch (code) {
        case 0: case 1:
            lv_image_set_src(weather_image, is_day ? &image_weather_sun : &image_weather_night);
            weather_description = "SERENO";
            break;
        case 2:
            lv_image_set_src(weather_image, &image_weather_cloud);
            weather_description = "PARZ. NUVOLOSO";
            break;
        case 3:
            lv_image_set_src(weather_image, &image_weather_cloud);
            weather_description = "NUVOLOSO";
            break;
        case 45: case 48:
            lv_image_set_src(weather_image, &image_weather_cloud);
            weather_description = "NEBBIA";
            break;
        case 51: case 53:
            lv_image_set_src(weather_image, &image_weather_rain);
            weather_description = "PIOGGIA LEGGERA";
            break;
        case 55:
            lv_image_set_src(weather_image, &image_weather_rain);
            weather_description = "PIOGGIA FORTE";
            break;
        case 56: case 57:
            lv_image_set_src(weather_image, &image_weather_rain);
            weather_description = "PIOGGIA/GHIACCIO";
            break;
        case 61: case 80:
            lv_image_set_src(weather_image, &image_weather_rain);
            weather_description = "PIOGGIA LEGGERA";
            break;
        case 63: case 81:
            lv_image_set_src(weather_image, &image_weather_rain);
            weather_description = "PIOGGIA MOD.";
            break;
        case 65: case 82:
            lv_image_set_src(weather_image, &image_weather_rain);
            weather_description = "PIOGGIA FORTE";
            break;
        case 66: case 67:
            lv_image_set_src(weather_image, &image_weather_rain);
            weather_description = "PIOGGIA GELATA";
            break;
        case 71: case 73: case 75: case 77: case 85: case 86:
            lv_image_set_src(weather_image, &image_weather_snow);
            weather_description = "NEVE";
            break;
        case 95:
            lv_image_set_src(weather_image, &image_weather_thunder);
            weather_description = "TEMPORALE";
            break;
        case 96: case 99:
            lv_image_set_src(weather_image, &image_weather_thunder);
            weather_description = "GRANDINE";
            break;
        default:
            weather_description = "SCONOSCIUTO";
            break;
    }
}

// Funzione: scarica dati meteo da open-meteo.com
static void get_weather_data() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://api.open-meteo.com/v1/forecast?latitude=" + latitude +
                     "&longitude=" + longitude +
                     "&current=temperature_2m,relative_humidity_2m,is_day,weather_code" +
                     temperature_unit + "&timezone=" + timezone;
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            if (!error) {
                const char* datetime = doc["current"]["time"];
                temperature = String(doc["current"]["temperature_2m"]);
                humidity = String(doc["current"]["relative_humidity_2m"]);
                is_day = String(doc["current"]["is_day"]).toInt();
                weather_code = String(doc["current"]["weather_code"]).toInt();

                String datetime_str = String(datetime);
                int splitIndex = datetime_str.indexOf('T');
                current_date = datetime_str.substring(0, splitIndex);
                last_weather_update = datetime_str.substring(splitIndex + 1, splitIndex + 9); // time
            }
        }
        http.end();
    }
}

// Timer callback: aggiorna dati e GUI
static void timer_cb(lv_timer_t * timer){
    LV_UNUSED(timer);
    get_weather_data();
    get_weather_description(weather_code);
    lv_label_set_text(text_label_date, current_date.c_str());
    lv_label_set_text(text_label_temperature, (" " + temperature + degree_symbol).c_str());
    lv_label_set_text(text_label_humidity, (humidity + "%").c_str());
    lv_label_set_text(text_label_weather_description, weather_description.c_str());
    lv_label_set_text(text_label_time_location, ("Aggiornamento: " + last_weather_update + " | " + location).c_str());
}

// Callback del pulsante Indietro
static void back_to_menu_cb(lv_event_t *e) {
    // Ferma il timer per non lasciarlo attivo in background
    if (weather_timer) {
        lv_timer_del(weather_timer);
        weather_timer = nullptr;
    }
    create_main_menu();
}

// Funzione principale di schermata meteo
void show_weather_screen(lv_event_t *e) {
    // Pulisci lo schermo
    lv_obj_clean(lv_scr_act());

    // --- Pulsante "Indietro" angolo alto sinistra ---
    lv_obj_t *back_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(back_btn, 38, 38); // dimensione compatta
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 4, 4);
    lv_obj_add_event_cb(back_btn, back_to_menu_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    // Solo simbolo freccia per non far uscire il testo
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_center(back_label);

    // Scarica dati meteo iniziali
    get_weather_data();

    // -- Evidenziazione con cerchio colorato dietro l'immagine --
    lv_obj_t * circle = lv_obj_create(lv_scr_act());
    lv_obj_set_size(circle, 90, 90); // grandezza cerchio
    lv_obj_align(circle, LV_ALIGN_TOP_MID, 0, 22); // leggermente più in alto rispetto all'immagine
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(circle, lv_palette_main(LV_PALETTE_BLUE), 0); // Cambia colore se vuoi!
    lv_obj_set_style_bg_opa(circle, LV_OPA_30, 0);
    lv_obj_set_style_border_width(circle, 0, 0);

    // Immagine meteo centrata in alto SENZA set_size (usa la dimensione reale)
    weather_image = lv_image_create(lv_scr_act());
    lv_obj_align(weather_image, LV_ALIGN_TOP_MID, 0, 32);
    get_weather_description(weather_code);

    // Data in alto a destra, allineata
    text_label_date = lv_label_create(lv_scr_act());
    lv_label_set_text(text_label_date, current_date.c_str());
    lv_obj_set_style_text_font(text_label_date, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(text_label_date, lv_palette_main(LV_PALETTE_TEAL), 0);
    lv_obj_align(text_label_date, LV_ALIGN_TOP_RIGHT, -10, 16);

    // Icona e valore temperatura
    LV_IMAGE_DECLARE(image_weather_temperature);
    lv_obj_t * weather_image_temperature = lv_image_create(lv_scr_act());
    lv_image_set_src(weather_image_temperature, &image_weather_temperature);
    lv_obj_set_size(weather_image_temperature, 32, 32);
    lv_obj_align(weather_image_temperature, LV_ALIGN_LEFT_MID, 22, -24);
    text_label_temperature = lv_label_create(lv_scr_act());
    lv_label_set_text(text_label_temperature, (" " + temperature + degree_symbol).c_str());
    lv_obj_set_style_text_font(text_label_temperature, &lv_font_montserrat_20, 0);
    lv_obj_align(text_label_temperature, LV_ALIGN_LEFT_MID, 62, -22);

    // Icona e valore umidità
    LV_IMAGE_DECLARE(image_weather_humidity);
    lv_obj_t * weather_image_humidity = lv_image_create(lv_scr_act());
    lv_image_set_src(weather_image_humidity, &image_weather_humidity);
    lv_obj_set_size(weather_image_humidity, 32, 32);
    lv_obj_align(weather_image_humidity, LV_ALIGN_LEFT_MID, 22, 24);
    text_label_humidity = lv_label_create(lv_scr_act());
    lv_label_set_text(text_label_humidity, (humidity + "%").c_str());
    lv_obj_set_style_text_font(text_label_humidity, &lv_font_montserrat_20, 0);
    lv_obj_align(text_label_humidity, LV_ALIGN_LEFT_MID, 62, 26);

    // Descrizione meteo in basso
    text_label_weather_description = lv_label_create(lv_scr_act());
    lv_label_set_text(text_label_weather_description, weather_description.c_str());
    lv_obj_set_style_text_font(text_label_weather_description, &lv_font_montserrat_18, 0);
    lv_obj_align(text_label_weather_description, LV_ALIGN_BOTTOM_MID, 0, -36);

    // Testo aggiornamento/località in basso piccolo
    text_label_time_location = lv_label_create(lv_scr_act());
    lv_label_set_text(text_label_time_location, ("Aggiornamento: " + last_weather_update + " | " + location).c_str());
    lv_obj_set_style_text_font(text_label_time_location, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(text_label_time_location, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(text_label_time_location, LV_ALIGN_BOTTOM_MID, 0, -10);

    // Timer: ogni 3 minuti aggiorna meteo (180000 ms)
    if (weather_timer) lv_timer_del(weather_timer);
    weather_timer = lv_timer_create(timer_cb, 180000, NULL);
    lv_timer_ready(weather_timer);
}