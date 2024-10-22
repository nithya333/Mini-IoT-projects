#define BLYNK_TEMPLATE_ID "TMPL3WhpJYlHa"
#define BLYNK_TEMPLATE_NAME "smart lighting"
#define BLYNK_AUTH_TOKEN "mWRD_66bjq1PRUImZ-ERqHonRFExzKym"

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include "secrets.h"
#include "FirebaseESP32.h"  // Install Firebase ESP32 library by Mobizt v4.3.4

#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
String FIREBASE_BUCKET = "";  
FirebaseData firebaseData;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

// Define LED pin
#define LED_PIN 2         // Pin connected to the LED
#define LED_CONTROL 21
#define LDR 4                                                                                                                                                        
#define threshold 2500
int buttonState = 0, stateChanged = 0;
float ldrVal;

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
  // Reading temperature and humidity data  // 
  ldrVal=analogRead(LDR);

  // Check if any reads failed and exit early (to try again).
  if (isnan(ldrVal)) {
    Serial.println(F("Failed to read from the LDR sensor!"));
    return;
  }
  if (ldrVal < threshold) {
    digitalWrite(LED_PIN, HIGH);  // Turn on the LED
   Serial.println("LED ON");
  } else 
   {
    digitalWrite(LED_PIN, LOW);   // Turn off the LED
   Serial.println("LED OFF");
  }
  Serial.print(F("LDR: "));
  Serial.println(ldrVal);
}


void updateBlynk()
{
  Blynk.run();
  // Send data to Blynk
  Blynk.virtualWrite(V4, ldrVal);  // Send ldr to virtual pin V1
 Blynk.virtualWrite(V1, ldrVal);
  if (ldrVal < threshold) {
        Blynk.virtualWrite(V3,"LED ON"); 
  } else 
   {
      Blynk.virtualWrite(V3,"LED OFF"); 
  }
}

void updateFirebase()
{
  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/LDR", ldrVal);
  if (ldrVal < threshold) {
        Firebase.set(firebaseData, String(FIREBASE_BUCKET) + "/LED_STATUS","ON");
  } else 
   {
       Firebase.set(firebaseData, String(FIREBASE_BUCKET) + "/LED_STATUS","OFF");
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


BLYNK_WRITE(V2) { // Button Widget is writing to pin V1
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
