
#include "secrets.h"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "FirebaseESP32.h"  // Install Firebase ESP32 library by Mobizt v4.3.4

#define BLYNK_PRINT Serial
// IR sensor pins
#define IR_SENSOR1_PIN 5  // Pin connected to the first IR sensor (e.g., G4 on ESP32)
#define IR_SENSOR2_PIN 4  // Pin connected to the second IR sensor (e.g., G5 on ESP32)

// Define LED pins for parking spots
#define LED1_PIN 19  // LED for the first parking spot
#define LED2_PIN 21 // LED for the second parking spot

char auth[] = BLYNK_AUTH_TOKEN;
String FIREBASE_BUCKET = "";  
FirebaseData firebaseData;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

String myStatus = "";

int irSensor1Value, irSensor2Value;
char status[][5] = {"Free", "Occu"};
int manual1 = 0, manual2 = 0;

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

  pinMode(IR_SENSOR1_PIN, INPUT);  // Initialize the first IR sensor pin as an input
  pinMode(IR_SENSOR2_PIN, INPUT);  // Initialize the second IR sensor pin as an input
  pinMode(LED1_PIN, OUTPUT);  // Initialize the first LED pin as an output
  pinMode(LED2_PIN, OUTPUT);  // Initialize the second LED pin as an output
  digitalWrite(LED1_PIN, LOW);  // Ensure the first LED is off initially
  digitalWrite(LED2_PIN, LOW);  // Ensure the second LED is off initially


  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  setupWiFi();

  Blynk.begin(auth, SECRET_SSID, SECRET_PASS);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Serial.println(F("Smart parking system initialized."));

}

void computeSensorVals()
{
  // Read the IR sensor values
  irSensor1Value = digitalRead(IR_SENSOR1_PIN);
  irSensor2Value = digitalRead(IR_SENSOR2_PIN);
  irSensor1Value = irSensor1Value || manual1;
  irSensor2Value = irSensor2Value || manual2;
  digitalWrite(LED1_PIN, irSensor1Value);
  digitalWrite(LED2_PIN, irSensor2Value);
  Serial.print("Status : Slot0 -  ");
  Serial.print(status[irSensor1Value]);
  Serial.print("            Slot1 -  ");
  Serial.println(status[irSensor2Value]);
}

void updateBlynk()
{
  Blynk.virtualWrite(V0, irSensor1Value);  // Send status to Blynk virtual pin V0

  Blynk.virtualWrite(V1, irSensor2Value);  // Send status to Blynk virtual pin V0

  // // Check if the first parking spot is occupied
  // if (irSensor1Value == HIGH) {  // Assuming LOW indicates a car is present
  //   Serial.println(F("Parking spot 1 occupied"));
  //   digitalWrite(LED1_PIN, HIGH);  // Turn on the LED for the first spot
  //   // Blynk.virtualWrite(V0, "Spot 1: Occupied");  // Send status to Blynk virtual pin V0
  //   Blynk.virtualWrite(V0, 1);  // Send status to Blynk virtual pin V0
  // } else {
  //   Serial.println(F("Parking spot 1 available"));
  //   digitalWrite(LED1_PIN, LOW);  // Turn off the LED for the first spot
  //   // Blynk.virtualWrite(V0, "Spot 1: Available");  // Send status to Blynk virtual pin V0
  //   Blynk.virtualWrite(V0, 0);  // Send status to Blynk virtual pin V0
  // }

  // // Check if the second parking spot is occupied
  // if (irSensor2Value == HIGH) {  // Assuming LOW indicates a car is present
  //   Serial.println(F("Parking spot 2 occupied"));
  //   digitalWrite(LED2_PIN, HIGH);  // Turn on the LED for the second spot
  //   // Blynk.virtualWrite(V1, "Spot 2: Occupied");  // Send status to Blynk virtual pin V1
  //   Blynk.virtualWrite(V1, 1);  // Send status to Blynk virtual pin V1
  // } else {
  //   Serial.println(F("Parking spot 2 available"));
  //   digitalWrite(LED2_PIN, LOW);  // Turn off the LED for the second spot
  //   // Blynk.virtualWrite(V1, "Spot 2: Available");  // Send status to Blynk virtual pin V1
  //   Blynk.virtualWrite(V1, 0);  // Send status to Blynk virtual pin V1
  // }
}

void updateFirebase()
{

  // Check if the first parking spot is occupied
  if (irSensor1Value == HIGH) {  // Assuming LOW indicates a car is present
    Firebase.setBool(firebaseData, String(FIREBASE_BUCKET) + "/spot1", false);
  } else {
    Firebase.setBool(firebaseData, String(FIREBASE_BUCKET) + "/spot1", true);
  }

  // Check if the first parking spot is occupied
  if (irSensor2Value == HIGH) {  // Assuming LOW indicates a car is present
    Firebase.setBool(firebaseData, String(FIREBASE_BUCKET) + "/spot2", false);
  } else {
    Firebase.setBool(firebaseData, String(FIREBASE_BUCKET) + "/spot2", true);
  }
}

void loop() {
    Blynk.run();
    computeSensorVals();
    updateBlynk();
    updateFirebase();
    delay(5);
}

BLYNK_WRITE(V2) { // Button Widget is writing to pin V2
  manual1 = param.asInt();
  if (manual1 == 1) {  // If button is pressed
    Serial.println(F("Manual mode: Slot 1 reserved"));
  } 
}
BLYNK_WRITE(V3) { // Button Widget is writing to pin V3
  manual2 = param.asInt();
  if (manual2 == 1) {  // If button is pressed
    Serial.println(F("Manual mode: Slot 2 reserved"));
  } 
}

// BLYNK_WRITE(V9) { // Button Widget is writing to pin V1
//   int brightness = param.asInt();
//   analogWrite(LED_CONTROL, brightness);
// }