# GasMonitor

Monitoraggio Gas con Scheda display gialla economica ESP32 – CYD (ESP32-2432S028R)

---

## Descrizione

**GasMonitor** è un progetto basato su ESP32 che utilizza un display TFT touch per creare un'interfaccia grafica utente (GUI) avanzata tramite la libreria [LVGL](https://lvgl.io/). Il sistema è pensato per il monitoraggio di gas e parametri ambientali, con salvataggio dati, notifiche e possibilità di espansione hardware.

---

## Foto e Pinout della Scheda

Questa è la scheda utilizzata nel progetto, la **ESP32-2432S028R**:

![ESP32-2432S028R Pinout](https://user-images.githubusercontent.com/your-image-path.png)

- **ESP-WROOM-32**: Modulo principale ESP32
- **RESET / BOOT**: Pulsanti di reset e avvio
- **Micro-USB**: Alimentazione e programmazione
- **TF**: Slot per microSD
- **Extended IO**: Espansione I/O
- **Temperature and humidity interface**: Connettore per sensori ambientali
- **RGB LED**: LED multicolore integrato
- **Speak**: Uscita per altoparlante
- **Power supply base**: Connettore alimentazione esterna

---

## Funzionalità

- **Menu Interattivo**: GUI touch con LVGL per navigazione semplice.
- **Lettura Sensori**: Supporto per sensori di gas, temperatura, umidità.
- **Salvataggio su SD**: Log dati sensori su microSD in CSV.
- **Connettività Wi-Fi**: Invio dati a server remoto o cloud.
- **Notifiche e Allarmi**: Allarmi visivi/acustici per condizioni critiche.
- **Espandibilità**: Supporto per moduli aggiuntivi (es. GPS, batteria, altoparlante).

---

## File Inclusi

- `GasMonitor.ino`: Codice principale del progetto.
- `User_Setup.h`: Configurazione per il display TFT e i pin.
- `lv_conf.h`: Configurazione della libreria LVGL.
- `README.md`: Documentazione del progetto.

---

## Modello di Scheda Utilizzato

- **Scheda ESP32**: Scheda display gialla economica ESP32 – CYD (ESP32-2432S028R).  
  [Tutorial Random Nerd Tutorials](https://randomnerdtutorials.com/?s=ESP32-2432S028R)

---

## Collegamenti Hardware

| Funzione      | Pin ESP32-2432S028R | Note                   |
|:--------------|:--------------------|:-----------------------|
| TFT MISO      | GPIO39              |                        |
| TFT MOSI      | GPIO32              |                        |
| TFT CLK       | GPIO25              |                        |
| TFT CS        | GPIO33              |                        |
| TFT DC        | GPIO2               |                        |
| TFT RST       | 3.3V                | Fisso a VCC            |
| Touch T_IRQ   | GPIO36              |                        |
| Touch T_CS    | GPIO33              |                        |
| Touch T_CLK   | GPIO25              |                        |
| Touch T_MOSI  | GPIO32              |                        |
| Touch T_MISO  | GPIO39              |                        |

---

## Librerie Necessarie

Assicurati di installare queste librerie tramite Library Manager dell’IDE Arduino o PlatformIO:

- [`lvgl`](https://github.com/lvgl/lvgl)
- [`TFT_eSPI`](https://github.com/Bodmer/TFT_eSPI)
- [`XPT2046_Touchscreen`](https://github.com/PaulStoffregen/XPT2046_Touchscreen)

---

## Esempi e Risorse Utili

- [Esempi ufficiali LVGL](https://docs.lvgl.io/master/examples.html)
- [Guida TFT_eSPI](https://github.com/Bodmer/TFT_eSPI#readme)
- [Esempi XPT2046_Touchscreen](https://github.com/PaulStoffregen/XPT2046_Touchscreen#examples)
- [Tutorial ESP32-2432S028R su Random Nerd Tutorials](https://randomnerdtutorials.com/?s=ESP32-2432S028R)

---

## Come Usare

1. **Carica il codice**  
   Usa l’IDE Arduino o PlatformIO per caricare `GasMonitor.ino` sulla scheda ESP32.
2. **Configura i file**  
   Modifica `User_Setup.h` e `lv_conf.h` secondo il tuo hardware.
3. **Collega l’hardware**  
   Segui la tabella sopra per i collegamenti corretti.
4. **Avvia il progetto**  
   Collega la scheda via USB o alimentazione esterna e osserva il display.

---

## Miglioramenti Futuri

1. **Dashboard in Tempo Reale**  
   Visualizzazione dati tramite grafici in tempo reale.
2. **OTA Updates**  
   Aggiornamenti firmware tramite Wi-Fi.
3. **Espandibilità Hardware**  
   Supporto per GPS, monitor batteria, sensori aggiuntivi.
4. **Notifiche Push**  
   Invia notifiche su smartphone per eventi critici.
5. **Modalità Risparmio Energetico**  
   Implementazione modalità a basso consumo per uso prolungato.

---

## Note

Questo progetto è in continuo sviluppo: sentiti libero di contribuire, segnalare bug o suggerire miglioramenti tramite GitHub!

---

**Crediti immagine scheda:**  
Se hai caricato l’immagine nel repository, aggiorna il link in questo README con il percorso corretto, ad esempio:  
`![ESP32-2432S028R Pinout](docs/esp32-2432s028r-pinout.jpg)`
