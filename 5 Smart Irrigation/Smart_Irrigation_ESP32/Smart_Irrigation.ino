#define BLYNK_TEMPLATE_ID "TMPL3_WBb2TtA"
#define BLYNK_TEMPLATE_NAME "smart irrigation"
#define BLYNK_AUTH_TOKEN "peW44oq6kn5hoGThF5YExcW0eRk5-CuP"

#include <DHT.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "FirebaseESP32.h"  // Install Firebase ESP32 library by Mobizt v4.3.4


#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
String FIREBASE_BUCKET = "";  
FirebaseData firebaseData;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

String myStatus = "";

// Define LED pin
#define LED_PIN 2         // Pin connected to the LED
#define LED_CONTROL 21
#define moisture_sensor 4
int buttonState = 0, stateChanged = 0;
int moistureLevel;

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

    // Wait a few seconds between measurements.
  delay(2000);

  pinMode(LED_PIN, OUTPUT); // Initialize the LED pin as an output
  pinMode(LED_CONTROL, OUTPUT); // Initialize the LED pin as an output
  digitalWrite(LED_PIN, LOW); // Ensure the LED is off initially

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  setupWiFi();

  Blynk.begin(auth, SECRET_SSID, SECRET_PASS);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void computeSensorVals()
{
  moistureLevel=analogRead(moisture_sensor);

  // Check if any reads failed and exit early (to try again).
  if (isnan(moistureLevel)) {
    Serial.println(F("Failed to read from the DHT sensor!"));
    return;
  }
  Serial.print(F("Moisture Level: "));
  Serial.println(moistureLevel);

  if(moistureLevel>2500){
    Serial.println("Start Irrigation");
    digitalWrite(LED_PIN, HIGH);
   
  }
  else{
     Serial.println("Stop Irrigation");
    digitalWrite(LED_PIN, LOW);
      
  }
}

void updateThingspeak()
{
  // set the fields with the values
  ThingSpeak.setField(1,moistureLevel);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

void updateBlynk()
{
  Blynk.run();
  // Send data to Blynk
  Blynk.virtualWrite(V1, moistureLevel); 
   // Send temperature to virtual pin V6
 
  if(moistureLevel>2500){
     Blynk.logEvent("Irrigation","Start Irrigation");
       Blynk.virtualWrite(V0, moistureLevel); 
  }
  else{
   Blynk.logEvent("Irrigation","Stop irrigation");
   
  }
}

void updateFirebase()
{
  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/moisture", moistureLevel);
   if(moistureLevel>2500){
 
    Firebase.set(firebaseData, String(FIREBASE_BUCKET) + "/Motor_STATUS", "ON");
  }
  else{
   
        Firebase.set(firebaseData, String(FIREBASE_BUCKET) + "/Motor_STATUS","OFF");
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
  updateThingspeak();
  // delay(15000); // Wait 20 seconds to update the channel again
}


BLYNK_WRITE(V3) { // Button Widget is writing to pin V1
  if (param.asInt() != buttonState)
  {
      buttonState = param.asInt();
      stateChanged = 1;
  }

  Serial.print(F("Button state: "));
  Serial.println(buttonState);

  if (buttonState == 1) { // If button is pressed
    Serial.println(F("Button pressed"));
    digitalWrite(LED_CONTROL, HIGH);
  } else {
    Serial.println(F("Button released"));
    digitalWrite(LED_CONTROL, LOW);
  }
}

// BLYNK_WRITE(V9) { // Button Widget is writing to pin V1
//   int brightness = param.asInt();
//   analogWrite(LED_CONTROL, brightness);
// }