/***************************************************************************
  This is a library for the BMP280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include "PubSubClient.h" // Connect and publish to the MQTT broker



#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

const char* ssid = "SSID"; //SSID of the Wifi
const char* password =  "PASSWORD"; //Password of the Wifi

// MQTT
const char* mqtt_server = "XXX.XXX.XXX.XXX";  // IP of the MQTT broker
const char* pressure_topic = "home/office/pressure";
const char* temperature_topic = "home/office/temperature";
const char* mqtt_username = "MQTT_USERNAME"; // MQTT username
const char* mqtt_password = "MQTT_PASSWORD"; // MQTT password
const char* clientID = "NAME_OF_THE_CLIENT"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

String translateEncryptionType(wifi_auth_mode_t encryptionType) {
 
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}

void connect_MQTT(){
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }

}


void setup() {
  Serial.begin(115200);

  WiFi.disconnect(true);
  delay(3000);
  Serial.println(F("BMP280 test"));

  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
   connectToNetwork();
 
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
 

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {

    connect_MQTT();
    Serial.setTimeout(2000);

    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
      
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");

    // MQTT can only transmit strings
    String pressure_s=String((float)pressure); //Pa
    String temperature_s=String((float)temperature); //C

    Serial.println();
    // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
    if (client.publish(temperature_topic, String(temperature_s).c_str())) {
      Serial.println("Temperature sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(temperature_topic, String(temperature_s).c_str());
    }

    // PUBLISH to the MQTT Broker (topic = Pressure, defined at the beginning)
    if (client.publish(pressure_topic, String(pressure_s).c_str())) {
      Serial.println("Pressure sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Pressure failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(pressure_topic, String(pressure_s).c_str());
    }

    client.disconnect();  // disconnect from the MQTT broker
    delay(1000*60);       // print new values every 1 Minute
}
