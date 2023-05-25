#include <Bounce2.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Pin auf dem Board
#define BUTTON_PIN 33

// Einstellungen fürs WiFi (WLan) 
// https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
const char* ssid = "";
const char* password = "";
const int connectDelay = 1000; // Pause zwischen den Connect versuchen, 1000 = 1 Sekunde 

// Link zur API
const char* serverUrl = "";
const String jwtToken = "";

Bounce button = Bounce();

void setup() {
  Serial.begin(115200);

  // Setup für den Button
  pinMode(BUTTON_PIN, INPUT);
  button.attach(BUTTON_PIN);
  button.interval(10);

  // Verbindung zum WiFi, das Programm bleibt hier so lange stehen, 
  // bis eine Verbindung aufgebaut wurde
  ConnectToWiFi();
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    // Wenn es eine Verbindung zum WiFi gibt, dann weiter mit dem Programm
    button.update();
    if (button.fell()) {
      Serial.println("Button pressed!");
       
      HTTPClient http; 

      // Sende den Befehl zum Server, das die Zahl um 1 erhöht werden soll
      http.begin(serverUrl);
      http.addHeader("Authorization", "Bearer " + jwtToken);
      http.addHeader("Connection", "close");
      
      // Lese die Antwort vom Server, ob alles geklappt hat
      bool isResponsed = getResponse(http.POST(""), http.getString());

      http.end();
    }
  } else {
    // Wenn es keine Verbindung mehr zum WiFi gibt, dann versuche wieder eine Verbindung aufzubauen.
    // Das Programm bleibt hier so lange stehen, bis eine Verbindung aufgebaut wurde
    ConnectToWiFi();
  }
}

void ConnectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(connectDelay);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

bool getResponse(int httpResponseCode, String payload) {
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      Serial.println(payload);
      return true;
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      return false;
    }
}
