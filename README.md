# iot-relay-sketch

## cURL Examples

Turn on:

```sh
curl -u <MQTT_USERNAME>:<MQTT_PASSWORD> -d ON mqtt://<MQTT_BROKER>/<MQTT_CLIENT>/directive/powerState
```

Tell device to power off after waiting 30 seconds:

```sh
curl -u <MQTT_USERNAME>:<MQTT_PASSWORD> -d 30 mqtt://<MQTT_BROKER>/<MQTT_CLIENT>/directive/powerOffWait
```

## Installation

### Secrets

Copy the example secrets file:

```sh
cp example-secrets.h secrets.h
```

Edit `secrets.h` to include your WiFi and MQTT credentials.

### Arduino CLI

The following instructions are for the [Arduino CLI](https://arduino.github.io/arduino-cli). If you are using the [Arduino IDE](https://docs.arduino.cc/software/ide/), you will need to install the required libraries through the IDE.

```sh
arduino-cli lib install "PubSubClient"
arduino-cli lib install "WiFiNINA"
arduino-cli board list
arduino-cli core install arduino:samd
arduino-cli compile --fqbn arduino:samd:mkrwifi1010
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:samd:mkrwifi1010
```

## Compatible Boards

Emoji | Status
-|-
✅ | Working
❌ | Not Working
⚠️ | Not Tested Yet

Board | Status
-|-
[Arduino MKR WiFi 1010](https://docs.arduino.cc/hardware/mkr-wifi-1010/) | ✅
[ESP8266](https://www.espressif.com/en/products/socs/esp32) | ⚠️
[ESP32](https://www.espressif.com/en/products/socs/esp8266) | ⚠️
Other Boards | ⚠️

[Digital Loggers IoT Relay Documentation](http://www.digital-loggers.com/iotfaqs.html)
