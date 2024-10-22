#include "secrets.h"
#include <DHT.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "FirebaseESP32.h"  // Install Firebase ESP32 library by Mobizt v4.3.4

#define DHTPIN 4          // Pin connected to the DHT sensor (G4 on ESP32)
#define DHTTYPE DHT11     // DHT 11 sensor type
#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
String FIREBASE_BUCKET = "";  
FirebaseData firebaseData;

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

String myStatus = "";

DHT dht(DHTPIN, DHTTYPE);

// Define LED pin
#define LED_PIN 2         // Pin connected to the LED
#define LED_CONTROL 21
int buttonState = 0, stateChanged = 0;
float hum, temp, hic;

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
  pinMode(LED_CONTROL, OUTPUT); // Initialize the LED pin as an output
  digitalWrite(LED_PIN, HIGH); // Ensure the LED is off initially

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
  // Reading temperature and humidity data  // 
  hum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp)) {
    Serial.println(F("Failed to read from the DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(temp, hum, false);

  Serial.print(F("Humidity: "));
  Serial.print(hum);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C  Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C"));
}

void updateThingspeak()
{
  // set the fields with the values
  ThingSpeak.setField(1, hum);
  ThingSpeak.setField(2, temp);
  ThingSpeak.setField(3, hic);

  // figure out the status message
  // if(number1 > number2){
  //   myStatus = String("field1 is greater than field2"); 
  // }
  // else if(number1 < number2){
  //   myStatus = String("field1 is less than field2");
  // }
  // else{
  //   myStatus = String("field1 equals field2");
  // }
  
  // set the status
  // ThingSpeak.setStatus(myStatus);
  
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
  Blynk.virtualWrite(V5, hum);  // Send humidity to virtual pin V5
  Blynk.virtualWrite(V6, temp);  // Send temperature to virtual pin V6
  Blynk.virtualWrite(V7, hic);  // Send temperature to virtual pin V6
}

void updateFirebase()
{
  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Humidity", hum);
  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Temperature", temp);
  Firebase.setInt(firebaseData, String(FIREBASE_BUCKET) + "/Heat Index", hic);
  // Firebase.getString(firebaseData, String(FIREBASE_BUCKET) + "/lamp");
  // if ((firebaseData.stringData()).toInt() != buttonState)
  // {
  //     buttonState = (firebaseData.stringData()).toInt();
  //     stateChanged = 1;
  // }
  // Serial.println(buttonState);
}

void loop() {
  for (int i = 0; i < 10; i++)
  {
    computeSensorVals();
    updateBlynk();
    updateFirebase();
    delay(100);
  }
  updateThingspeak();
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

// BLYNK_WRITE(V9) { // Button Widget is writing to pin V1
//   int brightness = param.asInt();
//   analogWrite(LED_CONTROL, brightness);
// }