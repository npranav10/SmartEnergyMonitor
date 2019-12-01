
 /****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "****"
#define WLAN_PASS       "****"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "****"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "****"
#define AIO_KEY         "****"
WiFiClient client;

int buff;
//float watt1;
char watt[5];
int bill_amount = 0;
unsigned int energyTariff = 8.0;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME,
AIO_KEY);
Adafruit_MQTT_Publish Power = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME
"/feeds/Power");
Adafruit_MQTT_Publish bill = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bill");
void MQTT_connect();
void setup() {
Serial.begin(115200);
delay(10);
Serial.println(F("Adafruit MQTT demo"));
// Connect to WiFi access point.
Serial.println(); Serial.println();
Serial.print("Connecting to ");
Serial.println(WLAN_SSID);
WiFi.begin(WLAN_SSID, WLAN_PASS);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println();
Serial.println("WiFi connected");
Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

void loop() {
MQTT_connect();

int i=0;
float watt1;
if(Serial.available() > 0 ){
delay(100); //allows all serial sent to be received together
while(Serial.available() && i<5) {
watt[i++] = Serial.read();
}
watt[i++]='\0';
}
watt1 = atof(watt);
bill_amount = watt1 * (energyTariff/1000); // 1unit = 1kwH
Serial.print(F("\nSending Power val "));
Serial.println(watt1);
Serial.print("...");
if (! Power.publish(watt1)) {
Serial.println(F("Failed Publishing Power"));
} else {
Serial.println(F("Successfully published Power"));
}Serial.print(F("\nSending Bill val "));
Serial.println(bill_amount);
Serial.print("...");
if (! bill.publish(bill_amount)) {
Serial.println(F("Failed Publishing Bill Amount"));
} else {
Serial.println(F("Successfully published Bill"));
}
if (bill_amount==4){
for (int i =0; i<=2; i++)
{
bill.publish(bill_amount);
delay(5000);
}
bill_amount =6;
}
delay(5000);
}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
int8_t ret;
// Stop if already connected.
if (mqtt.connected()) {
return;
}
Serial.print("Connecting to MQTT... ");
uint8_t retries = 3;
while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
Serial.println(mqtt.connectErrorString(ret));
Serial.println("Retrying MQTT connection in 5 seconds...");
mqtt.disconnect();
delay(5000); // wait 5 seconds
retries--;
if (retries == 0) {
// basically die and wait for WDT to reset me
while (1);
}
}
Serial.println("MQTT Connected!");
}
