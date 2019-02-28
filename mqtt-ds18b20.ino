#include "OneWire.h"
#include "DallasTemperature.h"
#include <WiFi.h>
#include <PubSubClient.h>

// Power wire (red) is plugged into 5V
// Ground Wire (black) is plugged into Ground
// Data wire (yellow) is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// Update these with values suitable for your network.
const char* ssid = "WIFI NAME";
const char* password = "WIFI PASS";
const char* mqtt_server = "MQTT SERVER IP";
const char* mqtt_user = "MQTT USER";
const char* mqtt_pass = "MQTT PASS";
const char* mqtt_sensorname = "MQTT NAME";
const char* mqtt_topic = "MQTT TOPIC";

WiFiClient espClient;
PubSubClient client(mqtt_server,1883,espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_sensorname,mqtt_user,mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(void)
{
 
  Serial.begin(115200);
  tempSensor.begin();
  setup_wifi();
  delay(1000); 
  //client.setServer(mqtt_server, 1883);
  
  if(client.connected()) {
    Serial.println("MQTT: Connected");
  }
}
 
void loop(void)
{
  if (!client.connected()) {
    reconnect();
  }
  float celsius, fahrenheit;
  tempSensor.requestTemperaturesByIndex(0);
  celsius = tempSensor.getTempCByIndex(0);
  fahrenheit = celsius * 1.8 + 32.0;

  Serial.print("Temperature: ");
  Serial.print(fahrenheit);
  Serial.println(" F");
  client.publish(mqtt_topic,String(fahrenheit).c_str());

  delay(15000);
}
