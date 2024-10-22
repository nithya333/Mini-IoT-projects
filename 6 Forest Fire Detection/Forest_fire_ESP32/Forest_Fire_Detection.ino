#define BLYNK_TEMPLATE_ID "TMPL3lYAXQtuu"
#define BLYNK_TEMPLATE_NAME "ForestFire"
#define BLYNK_AUTH_TOKEN "M_HFRbtt2q3oHbh_Pjp0gnHi-PsJPvmi"
#include<MQ135.h>
#include <DHT.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "FirebaseESP32.h"  // Install Firebase ESP32 library by Mobizt v4.3.4

#define DHTPIN 4         // Pin connected to the DHT sensor (G4 on ESP32)
#define DHTTYPE DHT11     // DHT 11 sensor type
#define BLYNK_PRINT Serial
#define PIN_MQ135 15

char auth[] = BLYNK_AUTH_TOKEN;
String FIREBASE_BUCKET = "";  
FirebaseData firebaseData;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;


String myStatus = "";

DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135(PIN_MQ135);
// Define LED pin
#define LED_PIN 2         // Pin connected to the LED
int buttonState = 0, stateChanged = 0;
int temp_threshold=40;
int smoke_threshold=400;
float tempC,tempF,MQ135;

void setupWiFi() {

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting the setup"));

  dht.begin();
  // Wait a few seconds between measurements.
  delay(2000);

  pinMode(LED_PIN, OUTPUT); // Initialize the LED pin as an output

  digitalWrite(LED_PIN, HIGH); // Ensure the LED is off initially

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  setupWiFi();

  Blynk.begin(auth, SECRET_SSID, SECRET_PASS);
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void computeSensorVals()
{
 
  // Read temperature as Celsius (the default)
  tempC = dht.readTemperature();
  tempF = dht.readTemperature(true);
  MQ135=analogRead(PIN_MQ135);

  // Check if any reads failed and exit early (to try again).
  if (isnan(tempC)  ) {
    Serial.println(F("Failed to read from the DHT sensor!"));
    return;
  }
  if (isnan(MQ135)  ) {
    Serial.println(F("Failed to read from the Smoke sensor!"));
    return;
  }

  if(tempC>temp_threshold|| MQ135>smoke_threshold){
   digitalWrite(LED_PIN,HIGH);
    Serial.println("Fire Detected ");
 
  }else{
     digitalWrite(LED_PIN,LOW);
      Serial.println("No Fire  ");
  }

  Serial.print(F("Temperature: "));
  Serial.print(tempC);
  Serial.print(F("°C  "));
  Serial.print(tempF);
  Serial.println(F("°F"));
    Serial.print(F("Smoke: "));
  Serial.println(MQ135);
}


void updateBlynk()
{
  Blynk.run();
  // Send data to Blynk
  Blynk.virtualWrite(V0, tempC);  // Send humidity to virtual pin V5
  Blynk.virtualWrite(V1, tempF);  // Send temperature to virtual pin V6
  Blynk.virtualWrite(V3,MQ135); 
  if(tempC>temp_threshold|| MQ135>smoke_threshold)
  Blynk.virtualWrite(V2, "Fire Detected ");  // Send temperature to virtual pin V6
  else
  Blynk.virtualWrite(V2, "No Fire");
}

void updateFirebase()
{

  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Temperature", tempC);
 // Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Temperature/Farenheit", tempF);
  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Smoke", MQ135);
  
  if(tempC>temp_threshold || MQ135>smoke_threshold){
    Firebase.set(firebaseData, String(FIREBASE_BUCKET) + "/LED_STATUS", "ON");
  }else{
      Firebase.set(firebaseData, String(FIREBASE_BUCKET) + "/LED_STATUS", "OFF");
  }

}

void loop() {
  for (int i = 0; i < 7; i++)
  {
    computeSensorVals();
    updateBlynk();
    updateFirebase();
    delay(100);
  }

  // delay(15000); // Wait 20 seconds to update the channel again
}

