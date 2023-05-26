/* StartCode
*
*   1 = Display-Test
*   2 = versuche WiFi Verbindung herzustellen
*   3 = WiFi Verbindung hergestellt
*   4 = stelle eine Verbindung zum Json-File auf der Website her
*   5 = das Json-File auslesen
*/

/*  Pins 7-Segment Display
*   
*   3V3 - Rot
*   22  - Weiss
*   21  - Braun
*   GND - Schwarz
*/

/*  Pins DFPlayer Mini
*
*   ESP32 - DFPlayer
*   ----------------
*   5V    - Rot
*   17    - RX
*   16    - TX
*   GND   - GND
*         - SPK_1 (Anschluss Lautsprecher)
*         - SPK_2 (Anschluss Lautsprecher)
*/

// WLan
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
// Json
#include <ArduinoJson.h>
// Display
#include <Adafruit_GFX.h> // https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_LEDBackpack.h>
// DFPlayer (mp3-Player)
#include <DFRobotDFPlayerMini.h>

// Display
#include <SPI.h>
#include <FS.h> // Font files are stored in SPIFFS, so load the library
#include <TFT_eSPI.h>
#include <stdlib.h>
#include "U8g2_for_TFT_eSPI.h"
//#include "logo_ohne_inhalt_90px.h"
#include "logo_ohne_inhalt_130px.h"

struct {
  uint8_t           addr;         // I2C address
  Adafruit_7segment seg7;         // 7segment object
} disp[] = {
  { 0x71, Adafruit_7segment() },  // High digits Instagram
  { 0x70, Adafruit_7segment() }   // Low digits Instagram
};

// Config WiFi (WLan) 
char ssid[] = "";       // your network SSID (name)
char password[] = "";  // your network key

// Config API
const String serverUrl = "";
const String urlPathCounter = serverUrl + "/counter";
const String urlPathSetRankUpPlayed = serverUrl + "/setrankupplayed";
const String jwtToken = "";
unsigned long delayBetweenStartSequence = 3000; // Zeit zwischen den Checks in der Startsequenz
unsigned long delayBetweenChecks = 10000;        // Zeit zwischen den Checks auf dem API-Server (Daten-Server)

// Config Display 7-Segment
const int brightnessNumber = 15; // Helligkeit der Displays, Wert zwischen 0 - 15 möglich

// Config Display ILI9341 240 x 320
uint16_t displaySizeX = 320;  // Start-Pos. in x auf dem Display
uint16_t displaySizeY = 240;  // Start-Pos. in x auf dem Display

// Config ProgressBar
uint16_t x = 10;  // Start-Pos. in x auf dem Display
uint16_t y = 140;  // Start-Pos. in y auf dem Display
uint16_t w = 300;  // Breite der ProgressBar
uint16_t h = 20;  // Höhe der ProgressBar

const int counterMax = 200; // Das Maximum, bis zu dem der Zähler zählen soll
int counter = 0; // Der aktuelle Zählerstand
unsigned long previousMillis = 0; // Die Zeit des vorherigen Durchlaufs
const unsigned long interval = 50; // Die Zeit zwischen jedem Durchlauf (1 Sekunde)

// Config DFPlayer 
uint8_t audioVolume = 30; // Lautstärke

// Settings Display ILI9341
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
U8g2_for_TFT_eSPI u8f; 

uint16_t colorBackground = tft.color565(255, 255, 255);
uint16_t colorProgressBar = tft.color565(0, 0, 0);
uint16_t colorProgressBarBorder = tft.color565(0, 0, 0);

int numberOftextLines = 0;
bool refreshDisplayILI9341 = false; // erst wenn es eine Änderung gegeben hat, das Display ILI9341 aktualisieren

// Settings DFPlayer
HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
bool playAudio = false;
int numberOfAllMP3 = 0;

// Variablen API
long countNumber = 99999999;
String rankNumber = "0";
String rankName = "";
int rankProgressInPercent = 0;

void setup() {
  Serial.begin(115200);

  // Geräte initialisieren
  initDisplays7Seg();
  initDisplayILI9341();
  initDFPlayer();

  // Startsequenz
  testDisplays7Seg();

  Serial.println(F("Setup Ende"));
}

void loop() {

  if(!WL_CONNECTED) {
    connectToWiFi();
  }

  refreshDisplayILI9341 = false;

  // stelle eine Verbindung zum API-Server her und hole die Daten (Json-String)
  String jsonString = getJson();

  // den Json-String decodieren und die Zahl aus dem JsonDocument holen
  countNumber = getCount(jsonString);
  rankNumber = getRankNumber(jsonString);
  rankName = getRankName(jsonString);
  rankProgressInPercent = getRankProgressInPercent(jsonString);
  audioVolume = getAudioVolume(jsonString);
  
  // die Zahl aufs 7-Segment Display schreiben
  setDisplay7Seg(countNumber);

  // ILI9341 Display nur aktualisieren, wenn es Änderungen gegeben hat
  if(refreshDisplayILI9341) {
    clearDisplayILI9341();
    setImage(5, 130, 130, rankNumber);
    addProgressBar();
    setProgress(rankProgressInPercent);
    setText(10, 191, 3, true, rankName);
  }

  // spielt ein Audio, wenn in der Json "play_rankup_audio:true" ist
  playRankUpAudio(jsonString);

  delay(delayBetweenChecks);
}

void playRankUpAudio(String jsonString) {
  if(numberOfAllMP3 <= 0) {
    numberOfAllMP3 = myDFPlayer.readFileCounts();
  }

  if(!playAudio) { 
      playAudio = isPlayRankUpAudio(jsonString);
      
  }

  if(playAudio) {   
   // if (myDFPlayer.available()) {
      int i = random(1, numberOfAllMP3 + 1);
      //audioVolume = getAudioVolume(jsonString);

      Serial.print("Play Titel: ");
      Serial.print(i);
      Serial.print("/");
      Serial.println(numberOfAllMP3);      
      Serial.print("audioVolume: ");
      Serial.println(audioVolume);
      delay(50);
      
      myDFPlayer.volume(audioVolume);  // Set volume value (0~30).

      if(numberOfAllMP3 <= 0){
        myDFPlayer.play(1);
      } else {
        myDFPlayer.play(i);
      }
 
      delay(50);
      //myDFPlayer.next();  
      playAudio = false; 
      setRankUpPlayed(); // Wenn RankUpAudio gespielt wurde, dann sende es per HTTP zum Server
 //   }  
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  } 
}

void initDisplays7Seg() {
  for (uint8_t i = 0; i < 2; i++) {  // Initialize displays
    disp[i].seg7.begin(disp[i].addr);
    //disp[i].setBrightness(brightnessNumber);
  }
}

void clearDisplays7Seg() {
  for (uint8_t i = 0; i < 2; i++) {  // Initialize displays
    disp[i].seg7.clear();
    disp[i].seg7.writeDisplay();
  }
}

void connectToWiFi() {
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int countTryConnetion = 0;


  //while (true) {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    showTryConnection(countTryConnetion++);
    delay(500);

    if(countTryConnetion >= 10){
      countTryConnetion = 0;
    }
  } 

  if(WL_CONNECTED) {
    setStartCode(3);    //  WiFi Verbindung hergestellt
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
  } 
}

void showTryConnection(int versuche) {
  if(versuche == 0){
      clearDisplays7Seg();                            // leere Display
  }

  if(versuche < 5){
    disp[1].seg7.writeDigitRaw(4 - versuche, 64); // setze den Strich in der Mitte
    disp[1].seg7.writeDisplay();                  // gib alles auf dem Display aus
  } else {
    disp[0].seg7.writeDigitRaw(9 - versuche, 64); // setze den Strich in der Mitte
    disp[0].seg7.writeDisplay();                  // gib alles auf dem Display aus
  }
}

void testDisplays7Seg() {
  clearDisplays7Seg();

  // Display-Test
  setStartCode(1); 
  delay(delayBetweenStartSequence);
  setDisplay7Seg(88888888);
  delay(delayBetweenStartSequence);

  // versuche WiFi Verbindung herzustellen 
  setStartCode(2); 
  delay(delayBetweenStartSequence); 
  connectToWiFi(); 
  delay(delayBetweenStartSequence);  
  
  // stelle eine Verbindung zum API-Server her und hole die Daten (Json-String)
  setStartCode(4); 
  String jsonString = getJson();
  delay(delayBetweenStartSequence);

  // den Json-String decodieren und die Zahl aus dem JsonDocument holen
  setStartCode(5); 
  countNumber = getCount(jsonString);
  delay(delayBetweenStartSequence);

  // die Zahl aufs Display schreiben
  setDisplay7Seg(countNumber);
}

void setErrorCode(int errorCode) {
  // Alte Code
/*
  clearDisplays7Seg();                          // lösche den Buffer
  disp[1].seg7.print(errorCode);            // schreibe die Zahl
  disp[1].seg7.writeDigitRaw(0, 121);       // schreibe das E
  disp[1].seg7.writeDigitRaw(1, 0x80);      // schreibe den Punkt
  disp[1].seg7.writeDisplay();              // gib alles auf dem Display aus
*/

  // Neue Code 
  clearDisplays7Seg();                          // lösche den Buffer
  disp[1].seg7.print(errorCode);            // schreibe die Zahl
  disp[0].seg7.writeDigitRaw(0, 121);       // schreibe das E
  disp[0].seg7.writeDigitRaw(1, 80);        // schreibe das r
  disp[0].seg7.writeDigitRaw(3, 80);        // schreibe das r
  disp[0].seg7.writeDisplay();              // gib alles auf dem Display aus
  disp[1].seg7.writeDisplay();              // gib alles auf dem Display aus
  
}

void setStartCode(int startCode) {
  clearDisplays7Seg();                          // lösche den Buffer
  disp[1].seg7.print(startCode);            // schreibe die Zahl
  disp[1].seg7.writeDigitRaw(0, 109);       // schreibe das S
  disp[1].seg7.writeDigitRaw(1, 0x80);      // schreibe den Punkt
  disp[1].seg7.writeDisplay();              // gib alles auf dem Display aus

}

void setDisplay7Seg(long followerCount){
    uint16_t hi = followerCount / 10000, // Value on left (high digits) display
             lo = followerCount % 10000; // Value on right (low digits) display
    
    //        uint16_t hi = 5678, // Value on left (high digits) display
    //                 lo = 1234; // Value on right (low digits) display
    
    disp[0].seg7.print(hi, DEC);   // Write values to each display...
    disp[1].seg7.print(lo, DEC);
    
    // print() does not zero-pad the displays; this may produce a gap
    // between the high and low sections. Here we add zeros where needed...
    if(hi) {
      if(lo < 1000) {
        disp[1].seg7.writeDigitNum(0, 0);
        if(lo < 100) {
          disp[1].seg7.writeDigitNum(1, 0);
          if(lo < 10) {
            disp[1].seg7.writeDigitNum(3, 0);
          }
        }
      }
     } else {
       disp[0].seg7.clear(); // Clear 'hi' display
     }
     disp[0].seg7.writeDisplay(); // Push data to displays
     disp[1].seg7.writeDisplay();
}

long getCount(String jsonString){

  StaticJsonDocument<256> doc;
  deserializeJson(doc, jsonString);

  long countNumber = doc["count"].as<long>();
  Serial.print("Count: ");
  Serial.println(countNumber);

  return countNumber;

//  Beispiele
//  follower = doc["socialMedia"]["instagram"]["follower"].as<long>();
//  auto name = doc["name"].as<const char*>();
//  auto stars = doc["stargazers"]["totalCount"].as<long>();
//  auto issues = doc["issues"]["totalCount"].as<int>()
}

String getRankNumber(String jsonString){

  StaticJsonDocument<256> doc;
  deserializeJson(doc, jsonString);

  String value = doc["rankNumber"].as<String>();

  // nur wenn sich Daten geändert haben, das Display ILI9341 aktualisieren
  if(!refreshDisplayILI9341 && !value.equals(rankNumber)) {
    refreshDisplayILI9341 = true;
  }

  Serial.print("rankNumber: ");
  Serial.println(value);

  return value;
}

String getRankName(String jsonString){

  StaticJsonDocument<256> doc;
  deserializeJson(doc, jsonString);

  String value = doc["rankName"].as<String>();

  // nur wenn sich Daten geändert haben, das Display ILI9341 aktualisieren
  if(!refreshDisplayILI9341 && !value.equals(rankName)) {
    refreshDisplayILI9341 = true;
  }

  Serial.print("rankName: ");
  Serial.println(value);

  return value;
}

int getRankProgressInPercent(String jsonString){

  StaticJsonDocument<256> doc;
  deserializeJson(doc, jsonString);

  int value = doc["rankProgress"].as<int>();

  // nur wenn sich Daten geändert haben, das Display ILI9341 aktualisieren
  if(!refreshDisplayILI9341 && value != rankProgressInPercent) {
    refreshDisplayILI9341 = true;
  }

  Serial.print("rankProgressInPercent: ");
  Serial.println(value);

  return value;
}

int getAudioVolume(String jsonString){

  StaticJsonDocument<256> doc;
  deserializeJson(doc, jsonString);

  uint8_t value = doc["audioVolumeWatch"].as<uint8_t>(); 
  Serial.print("audioVolume: ");
  Serial.println(value);

  return value;
}

bool isPlayRankUpAudio(String jsonString){

  StaticJsonDocument<256> doc;
  deserializeJson(doc, jsonString);

  bool isPlayAudio = doc["playRankUpAudio"].as<bool>();
    Serial.print("playRankUpAudio: ");
  Serial.println(isPlayAudio);
  return isPlayAudio;

//  Beispiele
//  follower = doc["socialMedia"]["instagram"]["follower"].as<long>();
//  auto name = doc["name"].as<const char*>();
//  auto stars = doc["stargazers"]["totalCount"].as<long>();
//  auto issues = doc["issues"]["totalCount"].as<int>()
}

/*
void readJson(String jsonString){

  StaticJsonDocument<256> doc;
  deserializeJson(doc, jsonString);
  
  //follower = doc["socialMedia"]["instagram"]["follower"].as<long>();
  follower = doc["count"].as<long>();
  // follower = doc["edge_followed_by"]["count"].as<long>();
  Serial.print("Json Count: ");
  Serial.println(follower);

//  Beispiele
//  auto name = doc["name"].as<const char*>();
//  auto stars = doc["stargazers"]["totalCount"].as<long>();
//  auto issues = doc["issues"]["totalCount"].as<int>()
}
*/

String getJson(){
  String payload = "kein Json gefunden";

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + jwtToken);
    http.addHeader("Connection", "close");

    int httpCode = http.GET();
    if (httpCode > 0) {
      payload = http.getString();      
    } else {
      Serial.println("HTTP GET request failed");
    }
    
    http.end();
  }

    Serial.println(payload);
    return payload;
}

String setRankUpPlayed(){
  String payload = "kein Json gefunden";

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(urlPathSetRankUpPlayed);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + jwtToken);
    http.addHeader("Connection", "close");

    int httpCode = http.GET();
    if (httpCode > 0) {
      payload = http.getString();      
    } else {
      Serial.println("HTTP GET request failed");
    }
    
    http.end();
  }

    Serial.println(payload);
    return payload;
}

// ------------------------------------------------------
//                Display ILI9341 240 x 320
// ------------------------------------------------------

void initDisplayILI9341() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(colorBackground);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_WHITE);

  u8f.begin(tft);

  //setImage(5, 130, 130, "99");

  // init die ProgressBar
  addProgressBar();

  //setProgress(225);

  // https://forums.adafruit.com/viewtopic.php?t=70719
  // https://www.arduinoforum.de/arduino-Thread-TFT-eSPI-und-Umlaute
  //setText(10, 140, 3, textColor, true, "Der Text ist sehr lang mal sehen ob das passt");
  //setText(10, 191, 3, textColor, true, "MMMMMMMMMMM längerer Text, malll sehen ob das auch geht, 0123456789");
  //setText(10, 191, 3, true, "Mate Gelegenheitstrinker");
}

void setImage(int yLocationImage, int imageWidth, int imageHeight, String rankNumber) {
  int32_t zwi = displaySizeX - imageWidth;
  int32_t xLocationImage = zwi / 2;

  tft.pushImage(xLocationImage, yLocationImage, imageWidth, imageHeight, logo_ohne_inhalt_130px);

  u8f.setFontMode(0);                 // use u8g2 none transparent mode
  u8f.setFontDirection(0);            // left to right (this is default)
  u8f.setForegroundColor(TFT_BLACK);  // apply color
  u8f.setBackgroundColor(TFT_WHITE);
  //u8f.setFont(u8g2_font_helvR14_tf); // wird perfekt angezeigt
  //u8f.setFont(u8g2_font_helvR18_tf); // Text geht über das Display
  //u8f.setFont(u8g2_font_helvB18_tf); // Text kein Abstand zum Display
  //u8f.setFont(u8g2_font_helvR24_tf); // wird perfekt angezeigt
  //u8f.setFont(u8g2_font_helvB24_tf); // wird perfekt angezeigt
  //u8f.setFont(u8g2_font_logisoso16_tf); // Text geht über das Display
  //u8f.setFont(u8g2_font_fur20_tf); // Text geht über das Display
  u8f.setFont(u8g2_font_fub42_tf);


    // String to CharArray
    char buffer[rankNumber.length() + 1];
    rankNumber.toCharArray(buffer, sizeof(buffer));

    int16_t lineWidth = u8f.getUTF8Width(buffer);
    int16_t lineHeight = u8f.getFontAscent() - u8f.getFontDescent();
  
    int xLocationImageText = (displaySizeX - lineWidth) / 2;
    int yLocationImageText = (imageHeight / 2) + (lineHeight / 2) - 1;

    // Serial.print("lineWidth");
    // Serial.print(" -> ");
    // Serial.println(lineWidth);

    // Serial.print("lineHeight");
    // Serial.print(" -> ");
    // Serial.println(lineHeight);

    // Serial.print("xLocationImageText");
    // Serial.print(" -> ");
    // Serial.println(xLocationImageText);

    // Serial.print("yLocationImageText");
    // Serial.print(" -> ");
    // Serial.println(yLocationImageText);

    u8f.setCursor(xLocationImageText, yLocationImageText);                // start writing at this position
    u8f.print(rankNumber);

    //u8f.drawUTF8(xLocationImageText, lineHeight, buffer);                 // UTF-8 string: "<" 550 448 664 ">"
    //u8f.u8g2_DrawStr(u8g2_font_helvR24_tf, xLocationImageText, lineHeight, buffer);  
    //u8f.drawUTF8(50, 50, buffer);
}

void setText(int16_t x, int16_t y, uint8_t s, bool isCenterH, char* text) {
  setText(10, 191, 3, true, String(text));
}

void setText(int16_t x, int16_t y, uint8_t s, bool isCenterH, String words) {

  //String words = String(text);
  int numberOfWords = wordsCount(words);

  // Serial.print("numberOfWords: ");
  // Serial.println(numberOfWords);

  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  u8f.setFontMode(0);                 // use u8g2 none transparent mode
  u8f.setFontDirection(0);            // left to right (this is default)
  u8f.setForegroundColor(TFT_BLACK);  // apply color
  u8f.setBackgroundColor(TFT_WHITE);
  //u8f.setFont(u8g2_font_helvR14_tf); // wird perfekt angezeigt
  //u8f.setFont(u8g2_font_helvR18_tf); // Text geht über das Display
  //u8f.setFont(u8g2_font_helvB18_tf); // Text kein Abstand zum Display
  //u8f.setFont(u8g2_font_helvR24_tf); // wird perfekt angezeigt
  //u8f.setFont(u8g2_font_helvB24_tf); // wird perfekt angezeigt
  //u8f.setFont(u8g2_font_logisoso16_tf); // Text geht über das Display
  //u8f.setFont(u8g2_font_fur20_tf); // Text geht über das Display
  u8f.setFont(u8g2_font_helvR24_tf);

  String* wordList = getArrayWords(words, numberOfWords, " ");
  String* textLines = getTextLines(wordList, numberOfWords);

  // Serial.print("numberOftextLines: ");
  // Serial.println(numberOftextLines);

  for (int i = 0; i < numberOftextLines; i++) {

    // Serial.print("textLines");
    // Serial.print(" -> ");
    // Serial.println(textLines[i]);

    // String to CharArray
    char buffer[textLines[i].length() + 1];
    textLines[i].toCharArray(buffer, sizeof(buffer));

    int16_t lineWidth = u8f.getUTF8Width(buffer);
    int16_t lineHeight = u8f.getFontAscent() - u8f.getFontDescent();
  
    x = (displaySizeX - lineWidth) / 2;

    u8f.drawUTF8(x, y, buffer);                 // UTF-8 string: "<" 550 448 664 ">"

    y = y + lineHeight + 5;

  }
}

void clearDisplayILI9341() {
  tft.fillRect(0, 0, displaySizeX, displaySizeY, TFT_WHITE);
}

String* getTextLines(String* wordList, int numberOfWords) {

  int lineSize = 0;
  int lines = 1;
  String stringWithSep = "";

  for (int i = 0; i < numberOfWords; i++) {
    String nextLine = "";
    if(i >= numberOfWords - 1) { 
      nextLine = wordList[i]; // wenn es nur ein Wort ist
    } else {
      nextLine = wordList[i] + " "; // wenn es mehrere Wörter sind
    }

    // String to CharArray
    char buffer[nextLine.length() + 1];
    nextLine.toCharArray(buffer, sizeof(buffer));

    int16_t textWidth = u8f.getUTF8Width(buffer);
    int16_t lineHeight = u8f.getFontAscent() - u8f.getFontDescent();

    // Serial.print("textWidth");
    // Serial.print(" -> ");
    // Serial.println(textWidth);

    // Serial.print("lineHeight");
    // Serial.print(" -> ");
    // Serial.println(lineHeight);

    // Serial.print("lineSize");
    // Serial.print(" -> ");
    // Serial.println(lineSize);

    // Wieviel Platz ist noch auf dem Display für die Line
    //textMarginX
    int restlichePlatz = displaySizeX - lineSize;

    // Serial.print("restlichePlatz");
    // Serial.print(" -> ");
    // Serial.println(restlichePlatz);

    // Wenn es keinen Platz mehr auf der Line gibt, dann Trennzeichen einfügen
    if(textWidth > restlichePlatz) {
        stringWithSep += ";";
        lineSize = 0;
        ++lines;

      // Serial.print("Break");
      // Serial.print(" -> ");
      // Serial.println(stringWithSep);
    }

    lineSize = lineSize + textWidth;

    stringWithSep += nextLine;

    // Serial.print("str[100]");
    // Serial.print(" -> ");
    // Serial.println(stringWithSep);
  }

  numberOftextLines = lines;

  if(lines > 2) {
    Serial.print("Dieser String hat mehr als 2 Zeilen");
    Serial.print(" -> ");
    Serial.println(stringWithSep);
    // for (int i = 0; i < numberOfWords; i++) {
    //   Serial.println(stringWithSep);
    // }
    // Serial.println("");
  }

  String* textLines = getArrayWords(stringWithSep, lines, " ;");
  return textLines;
}

String* getArrayWords(String words, int numberOfWords, String separator) {

    // Serial.print("numberOfWords: ");
    // Serial.println(numberOfWords);
    // Serial.print("words: ");
    // Serial.println(words);


  String* wordList = new String[numberOfWords];
  for (int i = 0; i < numberOfWords; i++) {
    int spaceIndex = words.indexOf(separator);
    if (spaceIndex == -1) {
      wordList[i] = words;
    } else {
      wordList[i] = words.substring(0, spaceIndex);
      words = words.substring(spaceIndex + separator.length()); // entferne die Separator, am Anfang vom String
    }
  }

  return wordList;
}

int wordsCount(String words) {
  int wordCount = 0;
  while (words.indexOf(" ") != -1) {
    wordCount++;
    words = words.substring(words.indexOf(" ") + 1);
  }
  wordCount++; // fügen Sie das letzte Wort hinzu
  return wordCount;
}

void addProgressBar() {
  tft.drawFastHLine(x, y, w, colorProgressBarBorder);
  tft.drawFastHLine(x, y+h, w, colorProgressBarBorder);
  tft.drawFastVLine(x, y, h+1, colorProgressBarBorder);
  tft.drawFastVLine(x + w, y, h+1, colorProgressBarBorder);
  tft.fillRect(x+1, y+1, 0, h-1, colorProgressBar);
}

void setProgress(int16_t progressInPercent) {
  int value = percentToValue(progressInPercent); // Gibt den gerundeten Wert für 50% zurück
     Serial.print("progressInPercent: ");
     Serial.println(value);

  tft.fillRect(x+1, y+1, value, h-1, colorProgressBar);
  //tft.fillRect(x+1, y+1, progress, h-1, colorProgressBar);
}

int percentToValue(int percent) {
  float value = percent / 100.0 * 300; // Berechne den Wert aus dem Prozentsatz (3.0f = 300 / 100)
  int roundedValue = round(value); // Runde den Wert auf den nächsten ganzen Wert
  return min(roundedValue, 300); // Begrenze den Wert auf 300 (oder einen kleineren Wert)
}

// ------------------------------------------------------
//                        DFPlayer
// ------------------------------------------------------

void initDFPlayer() {

  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);  // speed, type, RX, TX

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial, true, false)) {
  //if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    
    Serial.println(myDFPlayer.readType(),HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }

  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(audioVolume);  // Set volume value (0~30).

  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
//  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  
  //----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);
  
  //----Mp3 control----
//  myDFPlayer.sleep();     //sleep
//  myDFPlayer.reset();     //Reset the module
//  myDFPlayer.enableDAC();  //Enable On-chip DAC
//  myDFPlayer.disableDAC();  //Disable On-chip DAC
//  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15

  int delayms=100;

  //----Read imformation----
  //Serial.println(F("readState--------------------"));
  //Serial.println(myDFPlayer.readState()); //read mp3 state
  //Serial.println(F("readVolume--------------------"));
  //Serial.println(myDFPlayer.readVolume()); //read current volume
  //Serial.println(F("readEQ--------------------"));
  //Serial.println(myDFPlayer.readEQ()); //read EQ setting
  //Serial.println(F("readFileCounts--------------------"));
  //Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  //Serial.println(F("readCurrentFileNumber--------------------"));
  //Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
  //Serial.println(F("readFileCountsInFolder--------------------"));
  //Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
  //Serial.println(F("--------------------"));

  //myDFPlayer.randomAll();
  delay(500);
  //myDFPlayer.next();
  printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.

  //myDFPlayer.play(random(1, myDFPlayer.readFileCounts()));  //Play the first mp3
  //Serial.println(F("myDFPlayer.play(1)"));
  //myDFPlayer.play(1);  //Play the first mp3

} 

// Gibt Detailmeldung vom DFPlayer aus, um verschiedene Fehler und Zustände anzuzeigen.
// Aufruf: printDetail(myDFPlayer.readType(), myDFPlayer.read());
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
