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


// PLEASE CONFIG!

// Wifi
const char* ssid = "SSID"; //SSID of the Wifi
const char* password =  "PASSWORD"; //Password of the Wifi
// MQTT
String clientID = "Batterie"; // IMPORTANT! MQTT client ID -> Name der Batteriemessung
const char* mqtt_server = "XXX.XXX.XXX.XXX";  // IP of the MQTT broker
const char* mqtt_username = "MQTT_USER"; // MQTT username
const char* mqtt_password = "MQTT_PASSWORD"; // MQTT password
// other
String voltage_topic = "home/" + clientID + "/voltage"; // MQTT topic

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

// FINISH
void restartEsp() {
  Serial.println("Restarting in 5 seconds");
  delay(5000);
  ESP.restart();  
}


//TODO ist nicht aktuell!!
void connectToNetwork() { 
  WiFi.begin(ssid, password);
  WiFi.setHostname(clientID.c_str());
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}

//TODO ist nicht aktuell!!
void disconnectToNetwork() {
 
  while (WiFi.status() == WL_CONNECTED) {
    delay(1000);
    Serial.println("Disestablish WiFi connection ..");
    WiFi.disconnect(true);
  }
  
  Serial.println("Disonnected from network");
}

//FINISH
void connect_MQTT(){
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID.c_str(), mqtt_username, mqtt_password)) {
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

 
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
 }

void loop() {
    
    connectToNetwork();
    Serial.setTimeout(2000);
    connect_MQTT();
    Serial.setTimeout(2000);

    float voltage = getvoltage();// TODO Spannungsmessung soll ausgeführt werden
      
    Serial.print(F("Voltage = "));
    Serial.print(voltage);
    Serial.println(" V");

    // MQTT can only transmit strings
    String voltage_v=String((float)voltage); 

    Serial.println();
    // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
    if (client.publish(voltage_topic.c_str(), String(voltage_v).c_str())) {
      Serial.println("Voltage sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Voltage failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID.c_str(), mqtt_username, mqtt_password);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(voltage_topic.c_str(), String(voltage_v).c_str());
    }

    
    client.disconnect();  // disconnect from the MQTT broker
    disconnectToNetwork();  //disconnect from WiFi
    delay(1000*60);       // print new values every 1 Minute
	//espRestart();		// Counter bis 1000 und dann eventuell neu starten
}