/*
 * Based on https://github.com/Smartnestcz/examples/blob/master/Devices/light/light.ino
 *
 * Test with:
 *   curl -u <MQTT_USERNAME>:<MQTT_PASSWORD> -d ON mqtt://<MQTT_BROKER>/<MQTT_CLIENT>/directive/powerState
 *
 * Compile with (for mkr wifi 1010):
 *   arduino-cli compile --fqbn arduino:samd:mkrwifi1010
 *
 * Upload with (for mkr wifi 1010):
 *   arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:samd:mkrwifi1010
 *  or for mac:
 *   arduino-cli upload -p /dev/cu.usbmodemNNNN --fqbn arduino:samd:mkrwifi1010
 *
 * See available boards with:
 *   arduino-cli board list
 */
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
  #include <WiFi.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
#endif
#include <PubSubClient.h>
#include "secrets.h"

#define FIRMWARE_VERSION "1.0.0"
#define RELAY_PIN 0

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void startWifi();
void startMqtt();
void checkMqtt();
int splitTopic(char* topic, char* tokens[], int tokensNumber);
void callback(char* topic, byte* payload, unsigned int length);
void sendToBroker(char* topic, char* message);

void turnOff();
void turnOn();

void setup() {
	pinMode(RELAY_PIN, OUTPUT);
	Serial.begin(115200);
	startWifi();
	startMqtt();
}

void loop() {
	client.loop();
	checkMqtt();
}

void callback(char* topic, byte* payload, unsigned int length) {  // a new message has been received
	Serial.print("Topic:");
	Serial.println(topic);
	int tokensNumber = 10;
	char* tokens[tokensNumber];
	char message[length + 1];
	splitTopic(topic, tokens, tokensNumber);
	sprintf(message, "%c", (char)payload[0]);
	for (int i = 1; i < length; i++) {
		sprintf(message, "%s%c", message, (char)payload[i]);
	}
	Serial.print("Message:");
	Serial.println(message);

	if (strcmp(tokens[1], "directive") == 0 && strcmp(tokens[2], "powerState") == 0) {
		if (strcmp(message, "ON") == 0) {
			turnOn();
		} else if (strcmp(message, "OFF") == 0) {
			turnOff();
		}
	} else if (strcmp(tokens[1], "directive") == 0 && strcmp(tokens[2], "powerOffWait") == 0) {
    const int powerOffWait = atoi(message);
		if (powerOffWait > 0 && powerOffWait < 1000 * 60 * 60 * 1.5) {
      delay(powerOffWait * 1000);
			turnOff();
    }
	}
}

void startWifi() {
	// WiFi.mode(WIFI_STA);
	WiFi.begin(SSID_NAME, SSID_PASSWORD);
	Serial.println("Connecting ...");
	int attempts = 0;
	while (WiFi.status() != WL_CONNECTED && attempts < 10) {
		attempts++;
		delay(500);
		Serial.print(".");
	}

	if (WiFi.status() == WL_CONNECTED) {
		Serial.println('\n');
		Serial.print("Connected to ");
		Serial.println(WiFi.SSID());
		Serial.print("IP address:\t");
		Serial.println(WiFi.localIP());

	} else {
		Serial.println('\n');
		Serial.println('I could not connect to the wifi network after 10 attempts \n');
	}

	delay(500);
}

void startMqtt() {
	client.setServer(MQTT_BROKER, MQTT_PORT);
	client.setCallback(callback);

	while (!client.connected()) {
		Serial.println("Connecting to MQTT...");

		if (client.connect(MQTT_CLIENT, MQTT_USERNAME, MQTT_PASSWORD)) {
			Serial.println("connected");
		} else {
			if (client.state() == 5) {
				Serial.println("Connection not allowed by broker, possible reasons:");
				Serial.println("- Device is already online. Wait some seconds until it appears offline for the broker");
				Serial.println("- Wrong Username or password. Check credentials");
				Serial.println("- Client Id does not belong to this username, verify ClientId");

			} else {
				Serial.println("Not possible to connect to Broker Error code:");
				Serial.print(client.state());
			}

			delay(0x7530);
		}
	}

	char subscibeTopic[100];
	sprintf(subscibeTopic, "%s/#", MQTT_CLIENT);
	client.subscribe(subscibeTopic);  // Subscribes to all messages send to the device

	sendToBroker("report/online", "true");  // Reports that the device is online
	delay(100);
	sendToBroker("report/firmware", FIRMWARE_VERSION);  // Reports the firmware version
	delay(100);
	sendToBroker("report/ip", (char*)WiFi.localIP().toString().c_str());  // Reports the ip
	delay(100);

	char signal[5];
	sprintf(signal, "%d", WiFi.RSSI());
	sendToBroker("report/signal", signal);  // Reports the signal strength
	delay(100);
}

int splitTopic(char* topic, char* tokens[], int tokensNumber) {
	const char s[2] = "/";
	int pos = 0;
	tokens[0] = strtok(topic, s);

	while (pos < tokensNumber - 1 && tokens[pos] != NULL) {
		pos++;
		tokens[pos] = strtok(NULL, s);
	}

	return pos;
}

void checkMqtt() {
	if (!client.connected()) {
		startMqtt();
	}
}

void sendToBroker(char* topic, char* message) {
	if (client.connected()) {
		char topicArr[100];
		sprintf(topicArr, "%s/%s", MQTT_CLIENT, topic);
		client.publish(topicArr, message);
	}
}

void turnOff() {
  Serial.println("Turning off...");
	digitalWrite(RELAY_PIN, LOW);
	sendToBroker("report/powerState", "OFF");
}

void turnOn() {
	Serial.println("Turning on...");
	digitalWrite(RELAY_PIN, HIGH);
	sendToBroker("report/powerState", "ON");
}
