# GasMonitor
monitoraggio Gas con Scheda display gialla economica ESP32 – CYD (ESP32-2432S028R)
# GasMonitor Project

## Descrizione
GasMonitor è un progetto basato su ESP32 che utilizza un display TFT touch per creare un'interfaccia grafica (GUI). Questo progetto integra la libreria LVGL per creare un menu interattivo e supporta sensori per il monitoraggio ambientale.

## Funzionalità
- **Menu Interattivo**: Un'interfaccia grafica con pulsanti touch.
- **Lettura Sensori**: Supporto per sensori ambientali (es. gas, temperatura, umidità).
- **Salvataggio su SD**: Log dei dati dei sensori in formato CSV.
- **Connettività Wi-Fi**: Invio dei dati a un server remoto.
- **Notifiche e Allarmi**: Allarmi visivi e acustici per condizioni critiche.

## File Inclusi
- **GasMonitor.ino**: Codice principale del progetto.
- **User_Setup.h**: Configurazione per il display TFT.
- **lv_conf.h**: Configurazione della libreria LVGL.
- **README.md**: Documentazione del progetto.

## Modello di Scheda Utilizzato
- **Scheda ESP32**: Scheda display gialla economica ESP32 – CYD (ESP32-2432S028R).
  - [Link al prodotto su Amazon](https://randomnerdtutorials.com/?s=ESP32-2432S028R).

## Collegamenti Hardware
Ecco i collegamenti principali per il progetto:
- **TFT Display**:
  - MISO → GPIO39
  - MOSI → GPIO32
  - CLK → GPIO25
  - CS → GPIO33
  - DC → GPIO2
  - RST → Collegato a 3.3V
- **Touchscreen**:
  - T_IRQ → GPIO36
  - T_CS → GPIO33
  - T_CLK → GPIO25
  - T_MOSI → GPIO32
  - T_MISO → GPIO39

## Librerie Necessarie
Assicurati di installare le seguenti librerie prima di compilare il progetto:
- [`lvgl`](https://github.com/lvgl/lvgl)
- [`TFT_eSPI`](https://github.com/Bodmer/TFT_eSPI)
- [`XPT2046_Touchscreen`](https://github.com/PaulStoffregen/XPT2046_Touchscreen)

## Esempi Utili
- **Esempi LVGL**: [Documentazione ufficiale degli esempi LVGL](https://docs.lvgl.io/master/examples.html).
- **Tutorial ESP32-2432S028R**: [Random Nerd Tutorials](https://randomnerdtutorials.com/?s=ESP32-2432S028R).

## Come Usare
1. **Carica il codice**: Usa l'IDE Arduino o PlatformIO per caricare il file `GasMonitor.ino` sulla tua scheda ESP32.
2. **Configura i file**: Modifica i file `User_Setup.h` e `lv_conf.h` in base al tuo hardware.
3. **Collega l'hardware**: Segui i collegamenti indicati sopra.
4. **Avvia il progetto**: Collega la scheda al computer o a una fonte di alimentazione e osserva il funzionamento.

## Miglioramenti Futuri
1. **Dashboard in Tempo Reale**: Aggiungere grafici per visualizzare i dati in tempo reale.
2. **OTA Updates**: Aggiornamenti firmware tramite Wi-Fi per semplificare il deployment delle nuove versioni.
3. **Espandibilità Hardware**: Integrazione di moduli GPS e monitoraggio della batteria per applicazioni mobili.
4. **Notifiche Push**: Invia notifiche a uno smartphone in caso di eventi critici.
5. **Modalità di Risparmio Energetico**: Implementare una modalità a basso consumo per prolungare la durata della batteria.

## Note
Questo progetto è in continuo sviluppo. Sentiti libero di contribuire o suggerire miglioramenti tramite GitHub!
