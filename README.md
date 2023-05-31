# Stückliste

| Nr. | Menge |         Bauteil                 |                    Links                    |                                                      Daten                                                                                                                        |       Bemerkung       |
|:---:| :---: |---------------------------------|                    :---:                    |                                                      :---:                                                                                                                        |         :---:         |
|  1  |   2   | AZ-Delivery ESP32 Dev Kit C V4  | [Link](https://www.amazon.de/dp/B07Z83H831) | [Datenblatt](https://cdn.shopify.com/s/files/1/1509/1638/files/ESP32_devKitCV4_datasheet.pdf?v=1675936435)                                                                        |                       |
|  2  |   1   | 2.8' TFT SPI 240x320 Display    | [Link](https://www.amazon.de/dp/B017FZTIO6) |                                                                                                                                                                                   |                       |
|  3  |   2   | 4-Ziffern 7-Segment Display     | [Link](https://www.amazon.de/dp/B00SLYARJQ) | [Herstellerseite](https://www.adafruit.com/product/881)                                                                                                                           |                       |
|  4  |   1   | DFPlayer Mini MP3 Player        | [Link](https://www.amazon.de/dp/B01LOMZF8Y) | [Datenblatt](https://raw.githubusercontent.com/Arduinolibrary/DFPlayer_Mini_mp3/master/DFPlayer%20Mini%20Manual.pdf), [Wiki](https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299),  [Codebeispiele](https://github.com/DFRobot/DFPlayer-Mini-mp3/tree/master/DFPlayer_Mini_Mp3/examples), [Beispiel 1](https://funduino.de/wp-content/uploads/2021/01/mp3-Player_DFPlayerMini.pdf),  [Beispiel 2](https://www.elektronik-kompendium.de/sites/praxis/bauteil_dfplayer-mini.htm) |                       |
|  5  |   2   | Lautsprecher 3 Watt 8 Ohm       | [Link](https://www.amazon.de/dp/B0894M97CJ) |                                                                                                                                                                                   |                       |
|  6  |   1   | Micro SD-Karte                  | [Link](https://www.amazon.de/dp/B08GY9NYRM) |                                                                                                                                                                                   |                       |
|  7  |   2   | 100k Ohm widerstand             | [Link](https://www.amazon.de/dp/B08QRTJN75) |                                                                                                                                                                                   |                       |
|  8  |   1   | Pilz-Taster                     | [Link](https://www.amazon.de/dp/BB07BWY4XFL) |                       |

# Arduino IDE Einstellungen
Ich habe zum programmieren der ESP32 die [Arduino IDE 2](https://www.arduino.cc/en/software#future-version-of-the-arduino-ide) genutzt, nach der Installation der Arduino IDE
müssen noch ein paar einstellungen vorgenommen werden.

### ESP32 (mit CP2102) Treiber installieren
1. Den [ESP32-Treiber](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads) (CP210x Universal Windows Driver) runterladen und entpacken
2. Den ESP32 mit einem USB-Kabel an den Rechner anschliessen
3. Den Geräte-Manager öffnen (Windows-Taste + X > Geräte-Manager)
4. In der Liste, unter "Anschlüsse (COM & LPT)" sollte der ESP32 mit einer Warnung stehen
5. Rechts-Klick > Treiber aktualisieren > Auf meinem Computer nach Treibern suchen
6. Durchsuchen... > [den Ordner mit Treiber auswählen] > Weiter
7. Nach der Installation sollte die Warnung im Geräte-Manager weg sein und können fortfahren

### ESP32 in der Arduino IDE installieren
1. Gehe in der Arduino IDE 2.0 zu Datei > Einstellungen
2. Kopiere die folgende Zeile und füge sie in das Feld „Zusätzlicher Boardverwalter-URLs“ ein und bestätige mit OK
`https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Gehe in der Arduino IDE 2.0 zu Werkzeuge > Board > Board-Verwaltung
4. In der Liste nach "esp32 von Espressif" suchen und installieren

### Arduino IDE Konfiguration für den ESP32
Die Konfiguration kann unter dem Menü-Punkt `Werkzeuge` vorgenommen werden.
- Board:              `ESP32 Dev Module`
- CPU Frequency:      `240MHz (WiFi/BT)`
- Core Debug Level:   `None`
- Flash Frequency:    `80MHz`
- Flash Mode:         `QIO`
- Flash Size:         `4MB (32Mb)`
- Partition Scheme:   `Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)`
- PSRAM:              `Disable`
- Upload Speed:       `921600`

## Bibliotheken die noch installiert werden müssen
| Nr. | Import                  |                    Name                     | Version |     Bemerkungen   |
|:---:| :---:                   |---------------------------------------------|  :---:  |        :---:      |
|  1  | ArduinoJson.h           | ArduinoJson von Benoit                      | v6.20.1 |                   |
|  2  | Adafruit_GFX.h          | Adafruit GFX Library von Adafruit           | v1.11.5 |                   |
|  3  | Adafruit_LEDBackpack.h  | Adafruit LED Backpack Library von Adafruit  | v1.3.2  |                   |
|  4  | DFRobotDFPlayerMini.h   | DFRobotDFPlayerMini von DFRobot             | v1.0.5  |                   |
|  5  | TFT_eSPI.h              | TFT_eSPI von Bodmer                         | v2.5.0  |   In der Bibliothek "TFT_eSPI" müssen noch Einstellungen vorgenommen werden, die unter dem Punkt "TFT_eSPI Einstellungen" zu finden sind     |                       |

### TFT_eSPI Einstellungen
Bei der Bibliothek "TFT_eSPI" müssen noch in der Datei "User_Setup.h" ein paar Einstellungen vorgenommen werden.
Die Datei "User_Setup.h" mit einem Editor öffnen und die folgenden Änderungen vornehmen, man findet die Datei unter "[Pfad zu deinem Arduino Workspace]\libraries\TFT_eSPI\User_Setup.h"

#### 1. Änderung
Ab Zeile 168 steht folgender Code
```c
// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
#define TFT_CS   PIN_D8  // Chip select control pin D8
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
//#define TFT_RST  -1    // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V
```
Der muss durch diesen Code ersetzt werden
```c
// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
//#define TFT_CS   PIN_D8  // Chip select control pin D8
//#define TFT_DC   PIN_D3  // Data Command control pin
//#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
//#define TFT_RST  -1    // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V
```

#### 2. Änderung
Ab Zeile 203 steht folgender Code
```c
// For ESP32 Dev board (only tested with ILI9341 display)
// The hardware SPI can be mapped to any pins

//#define TFT_MISO 19
//#define TFT_MOSI 23
//#define TFT_SCLK 18
//#define TFT_CS   15  // Chip select control pin
//#define TFT_DC    2  // Data Command control pin
//#define TFT_RST   4  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
```
Der muss durch diesen Code ersetzt werden
```c
// For ESP32 Dev board (only tested with ILI9341 display)
// The hardware SPI can be mapped to any pins

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
```

# Schaltungen
Hier die Schaltungen für die ESP32, es gibt 2 ESP32, der eine ESP32 wird in den Pilztaster eingebaut und der andere ESP32 kommt in die Uhr.

## Button
![alt text](https://github.com/Indivikar/mateCounter/blob/main/fritzing/Button_Schema.png?raw=true)

## Uhr
### MP3-Player
![alt text](https://github.com/Indivikar/mateCounter/blob/main/fritzing/DFPlayer_Mini_MP3_Player.png?raw=true)
![alt text](https://github.com/Indivikar/mateCounter/blob/main/fritzing/DFPlayer_Schema.png?raw=true)
### 7-Segment Display
![alt text](https://github.com/Indivikar/mateCounter/blob/main/fritzing/Display_7_Seg_Schema.png?raw=true)
### ILI9341 Display
![alt text](https://github.com/Indivikar/mateCounter/blob/main/fritzing/Display_ILI9341.png?raw=true)
### Alles zusammengefasst
![alt text](https://github.com/Indivikar/mateCounter/blob/main/fritzing/Display_Alles.png?raw=true)
