# iot-relay-sketch

Turn on from cURL:
```sh
curl -u <MQTT_USERNAME>:<MQTT_PASSWORD> -d ON mqtt://<MQTT_BROKER>/<MQTT_CLIENT>/directive/powerState
```

### Compatible Boards

Emoji | Status
-|-
✅ | Working
❌ | Not Working
⚠️ | Not Tested Yet

Board | Status
-|-
Arduino MKR WiFi 1010 | ✅
ESP8266 | ⚠️
ESP32 | ⚠️
Other Boards | ⚠️

[Digital Loggers Documentation](http://www.digital-loggers.com/iotfaqs.html)
