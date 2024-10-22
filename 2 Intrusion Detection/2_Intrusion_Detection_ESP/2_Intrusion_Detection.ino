
#include "secrets.h"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "FirebaseESP32.h"  // Install Firebase ESP32 library by Mobizt v4.3.4

#define IR_SENSOR_PIN 4  // Pin connected to the IR sensor (e.g., G4 on ESP32)
#define BLYNK_PRINT Serial
#define LED_PIN 2         // Pin connected to the LED
#define LED_CONTROL 21

char auth[] = BLYNK_AUTH_TOKEN;
String FIREBASE_BUCKET = "";  
FirebaseData firebaseData;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

String myStatus = "";

int modeButton = 0;
int irSensorVal;

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

  pinMode(IR_SENSOR_PIN, INPUT); // Initialize the LED pin as an output
  pinMode(LED_CONTROL, OUTPUT); // Initialize the LED pin as an output
  digitalWrite(LED_PIN, LOW); // Ensure the LED is off initially

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  setupWiFi();

  Blynk.begin(auth, SECRET_SSID, SECRET_PASS);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Serial.println(F("Intrusion detection system initialized."));

}

void computeSensorVals()
{
  // Read the IR sensor value
  irSensorVal = digitalRead(IR_SENSOR_PIN);
}

void updateBlynk()
{

  // Check if an intrusion is detected
  if (irSensorVal == HIGH) {  // Assuming LOW indicates an intrusion
    Serial.println(F("Intrusion detected!"));
    digitalWrite(LED_CONTROL, HIGH);  // Turn on the LED
    Blynk.virtualWrite(V0, "Intrusion detected!");  // Send alert to Blynk virtual pin V0
    Blynk.virtualWrite(V1, 1);  // Send alert to Blynk virtual pin V0

    // TASK : 2
    //Description : Trigger an event
    Blynk.logEvent("intrusion_detected","Intrusion is detected");
  
  // Note : Event definition should be created in the blynk IoT cloud before configuring it. 
  } else {
    digitalWrite(LED_CONTROL, LOW);  // Turn off the LED
    Blynk.virtualWrite(V0, "All clear");  // Send status to Blynk virtual pin V0
    Blynk.virtualWrite(V1, 0);  // Send status to Blynk virtual pin V0
  }
}

void updateFirebase()
{
  // Check if an intrusion is detected
  if (irSensorVal == HIGH) {  // Assuming LOW indicates an intrusion
  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Intrusion", 1);
  Firebase.setString(firebaseData, String(FIREBASE_BUCKET) + "/IntrusionStatus", "Intrusion Detected");
  } else {
    Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Intrusion", 0);
    Firebase.setString(firebaseData, String(FIREBASE_BUCKET) + "/IntrusionStatus", "All clear");
  }

  // Firebase.getInt(firebaseData, String(FIREBASE_BUCKET) + "/Mode");
  // if ((firebaseData.stringData()).toInt() != buttonState)
  // {
  //     buttonState = (firebaseData.stringData()).toInt();
  //     stateChanged = 1;
  // }
  // Serial.println(buttonState);
}

void loop() {
  Blynk.run();
    if (modeButton == 0)
    {
      computeSensorVals();
      updateBlynk();
      updateFirebase();
    }
    else
    {
    Serial.print(".");
    }
    delay(100);
}


BLYNK_WRITE(V2) { // Button Widget is writing to pin V1
  modeButton = param.asInt();

  Serial.print(F("Button state: "));
  Serial.println(modeButton);

  if (modeButton == 1) {  // If button is pressed
    Serial.println(F("Manual mode"));
  } else {
    Serial.println(F("Auto mode"));
  }
}

// BLYNK_WRITE(V9) { // Button Widget is writing to pin V1
//   int brightness = param.asInt();
//   analogWrite(LED_CONTROL, brightness);
// }