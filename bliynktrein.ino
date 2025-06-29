#define BLYNK_TEMPLATE_ID "user5"
#define BLYNK_TEMPLATE_NAME "user5@server.wyns.it"
#define BLYNK_PRINT Serial

// lybraries importeren
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <PubSubClient.h>

//wifi instelingen
char auth[] = "lbQNxJsI_j5gAbk4G_0Z75_3jKD0jYc8";
char ssid[] = "pass";
char pass[] = "pass";

// MQTT-instellingen
const char* mqttServer = "192.168.1.42";
const int mqttPort = 1883;
const char* mqttUser = "maart";
const char* mqttPassword = "password";
const char* clientID = "esp32_trein";

// GPIO-definities
#define LIGHT_SENSOR_PIN 34
const int motorPin = 23;
const int ledPin1 = 27;
const int ledPin2 = 16;

// WiFi & MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

const int veiligeSnelheid = 200;  // Maximale snelheid wanneer de slagboom gesloten is
int motorSpeed = 0;
int gewenstesnelheid;
bool slagboom_toe = false;        // Slagboomstatus
unsigned long lastLDRUpdate = 0;  // Tijdstip laatste update
bool Lampen = false;
bool blynklampen = true;
bool blynklampenoverscrijven = false;

// Callback-functie om MQTT-berichten te verwerken
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("MQTT bericht ontvangen [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Update Blynk
  // Op deze lijn stuur ik de inkomende MQTT berichten door naar Blynk op mijn telefoon. dit is om te kunnen zien in welke modus de trein zit. Dit is vooral om te debuggen.
  Blynk.virtualWrite(V0, "MQTT bericht ontvangen: " + String(topic) + " - " + message);

//Hieronder worden de MQTT berichten opgeslagen.
  if (String(topic) == "esp32/slagboom/sensor") {
    if (message == "true") {
      slagboom_toe = true;
      Serial.println("Slagboom GESLOTEN!");
    } else if (message == "false") {
      slagboom_toe = false;
      Serial.println("Slagboom OPEN!");
    }
  }
  if (String(topic) == "esp32/slagboom/lamp") {
    if (message == "true") {
      Lampen = true;
      Serial.println("Lampen  aan");
    } else if (message == "false") {
      Lampen = false;
      Serial.println("Lampen uit");
    }
  }
}

// Hieronder worden de Blynk instellingen opgeslagen  
// BLYNK-functie voor motorcontrole
BLYNK_WRITE(V1) {
  gewenstesnelheid = param.asInt();
}

// BLYNK-functie voor Lampen
BLYNK_WRITE(V2) {
  blynklampen = param.asInt();
}

BLYNK_WRITE(V3) {
  blynklampenoverscrijven = param.asInt();
  if (blynklampenoverscrijven == true) {
    Serial.println("blynk overscrijfmodus AAN!");
  } else {
    Serial.println("blynk overscrijfmodus UIT!");
  }
}

void setup() {
  // WiFi en Blynk opstarten
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(motorPin, OUTPUT);

  Serial.print("Verbinden met WiFi...");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(1000);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Verbonden!");
    Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);  // Stel callback in voor MQTT-berichten
  } else {
    Serial.println("\nWiFi Verbinden Mislukt! Herstarten...");
    delay(5000);
    ESP.restart();  // Automatisch herstarten bij WiFi-fout
  }

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
}

// Als de MQTT verbinding is verbroken, dan wordt er opnieuw verbinding gemaakt en wordt er gesubscribed op de juiste topics. 
void reconnect() {
  while (!client.connected()) {
    Serial.println("Verbinden met MQTT...");
    if (client.connect(clientID, mqttUser, mqttPassword)) {
      Serial.println("MQTT Verbonden");
      client.subscribe("esp32/slagboom/sensor");
      client.subscribe("esp32/slagboom/lamp");
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  // Dit wordt gebruikt zodat het proces actief blijft.
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  Blynk.run();
  // Slagboom logica - aanpassen op basis van LDR-waarde
  if (millis() - lastLDRUpdate >= 2000) {
    lastLDRUpdate = millis();
    int ldrValue = analogRead(LIGHT_SENSOR_PIN);
    Serial.print("LDR Waarde: ");
    Serial.println(ldrValue);

    Blynk.virtualWrite(V4, ldrValue);  // Stuur LDR naar Blynk (Virtuele Pin V4)
    // Hier wordt de LDR waarde doorgestuurd naar de slagboom via MQTT.
    client.publish("esp32/trein/ldr", String(ldrValue).c_str(), true);
  }

//Als de overschrijvingen-modus niet actief is wordt de status van de knop op de Blynk app gebruikt om de lampen aan of uit te zetten.
  if (blynklampenoverscrijven == true) {
    digitalWrite(ledPin1, blynklampen);
    digitalWrite(ledPin2, blynklampen);
    Blynk.setProperty(V6, "color", "#FFFFFF");
    Blynk.virtualWrite(V6, 255);
    if (blynklampen == true) {
      Blynk.setProperty(V5, "color", "#FFFFFF");
      Blynk.virtualWrite(V5, 255);
    } else {
      Blynk.setProperty(V5, "color", "#000000");
      Blynk.virtualWrite(V5, 0);
    }
  } // Als de overschrijving-modus niet actief is wordt de LDR sensor gebruikt.
  // Je kan ook zien welke modus actief is op de Blynk app door de led’s op de app.
  else {
    Blynk.setProperty(V6, "color", "#000000");
    Blynk.virtualWrite(V6, 255);
    digitalWrite(ledPin1, Lampen);
    digitalWrite(ledPin2, Lampen);
    if (Lampen == true) {
      Blynk.setProperty(V5, "color", "#FFFFFF");
      Blynk.virtualWrite(V5, 255);
    } else {
      Blynk.setProperty(V5, "color", "#000000");
      Blynk.virtualWrite(V5, 0);
    }
  }
 // Hier wordt er gereageerd op de MQTT berichten als de slagboom toe is.
  if (slagboom_toe == true) {
    // Als de slagboom toe is dan wordt er gekeken of dat de gewenste snelheid lager is dan de veilige snelheid. Als deze sneller, dan wordt deze op de veilige snelheid ingesteld.
    if (gewenstesnelheid < veiligeSnelheid) {
      motorSpeed = gewenstesnelheid;
       motorWrite();
    } else {
      motorSpeed = veiligeSnelheid;
       motorWrite();
    }
    Blynk.setProperty(V7, "color", "#FFFFFF");
    Blynk.virtualWrite(V7, 255);
   
  }
  // Als de slagboom open is, dan blijft de snelheid op de gewenste snelheid
  if (slagboom_toe == false) {
    motorSpeed = gewenstesnelheid;
    Blynk.setProperty(V7, "color", "#000000");
    Blynk.virtualWrite(V7, 0);
    motorWrite();
  }
}

void motorWrite() {
  // De functie motorWrite wordt gebruikt voor de snelheid effectief in te stellen op de ingestelde snelheid en wordt weergegeven op Blynk
  analogWrite(motorPin, motorSpeed);
  Blynk.virtualWrite(V8, motorSpeed);
}
