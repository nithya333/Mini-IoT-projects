## IoT Mini Projects
This repository contains a series of mini projects based on the Internet of Things (IoT). These projects utilize various hardware sensors, actuators, ESP32, Arduino IDE, sensors, and cloud services like Firebase Realtime Database, ThingSpeak, and Blynk. The projects also feature web and mobile apps developed with tools such as Streamlit, Blynk, and MIT App Inventor. Below are the descriptions of the various projects included:

1. Smart Lighting System
   
Objective: To save energy by adjusting lighting based on ambient conditions.

Modes:

Auto Mode: Uses a Light Dependent Resistor (LDR) sensor to periodically measure the light levels in a room. If the light levels are low, the system will automatically turn on the lights; if the light levels are sufficient, the lights will be turned off.
Manual Mode: Allows users to manually control the lighting. The user can turn the light on or off using a physical switch, and the system will respond accordingly.


2. Intrusion Detection System (IDS)
   
Objective: Enhance security in homes, offices, and warehouses by detecting unauthorized movements.

Working Principle:

IR Sensor detects infrared radiation emitted by warm objects, such as humans.
When a person or object passes within the sensor's detection range, it triggers an alarm or sends an alert to a mobile app or web application.


3. Smart Parking System
   
Objective: To help drivers find available parking spaces efficiently.

Working Principle:

IR Sensors are placed in parking spots to detect whether the spot is occupied or available.
The system transmits this data to a cloud-based app or device, allowing drivers to check parking availability in real-time.
The app provides parking guidance and monitoring to reduce traffic congestion and save time for drivers.


4. IoT-Based Weather Monitoring System
   
Objective: To monitor weather conditions using IoT sensors and analyze data through cloud applications.

Working Principle:

Sensors like Temperature and Humidity sensors collect weather-related data.
The collected data is sent to cloud-based applications for storage and analysis.
Users can view real-time weather data and receive weather alerts.


5. Smart Irrigation System
   
Objective: To optimize water usage for agriculture by automatically adjusting irrigation based on soil moisture levels.

Working Principle:

Soil Moisture Sensors detect the moisture content in the soil.
The system controls the irrigation motor to release water only when the soil moisture level falls below a predefined threshold.
Data is sent to the cloud, where it is analyzed to optimize irrigation schedules.


6. Forest Fire Detection System
   
Objective: To detect and alert about forest fires at an early stage, helping to minimize damage.

Working Principle:

MQ135 Sensor detects smoke and gases emitted during a fire.
DHT11 Sensor measures temperature, which can be used to detect abnormal temperature rise indicative of a fire.
When smoke or high temperatures are detected, the system sends an alert via the web or mobile app.


## Tech Stack:

### Hardware Components:
* ESP32 microcontroller
* Sensors (LDR, PIR, IR, DHT11, MQ135, Soil Moisture)
* LED, Buzzer
* Other basic electronic components (Resistors, etc.)

### Cloud Services:
* Firebase Realtime Database
* ThingSpeak
* Blynk DB

### Web and Mobile Applications:
* Streamlit (Web app development)
* Blynk (Mobile app development)
* MIT App Inventor (For custom mobile apps)

### Software:
* Arduino IDE (For programming ESP32)
* Python (For Streamlit app development)

## Code Contributors:
M Nithyashree, Pavithra N, Krupa P Nadgir 
