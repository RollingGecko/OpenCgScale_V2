#include <Arduino.h>

#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"

const char *ssid = "MyESP32AP";
const char *password = "testpassword";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncWebSocketClient * globalClient = NULL;

DynamicJsonDocument jsonMessage(1024);
JsonObject root = jsonMessage.to<JsonObject>();

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {

	if (type == WS_EVT_CONNECT) {

		Serial.println("Websocket client connection received");
		globalClient = client;

	}
	else if (type == WS_EVT_DISCONNECT) {

		Serial.println("Websocket client connection finished");
		globalClient = NULL;

	}
}

void setup() {
	Serial.begin(115200);
	Serial.println("Start of program!");

	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
	if (!SPIFFS.exists("/home.html"))
	{
		Serial.println("File does not exist!");
	}

	WiFi.softAP(ssid, password);

	Serial.println();
	Serial.print("IP address: ");
	Serial.println(WiFi.softAPIP());

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		Serial.println("Incomming /htm -Event");
		request->send(SPIFFS, "/home.html", "text/html");
		
	});
	server.on("/laser", HTTP_GET, [](AsyncWebServerRequest *request) {
			Serial.println("GET Laser status");
			request->send(200, "text/plain", "tbd Laserstatus");
	});
  server.on("/laser", HTTP_POST, [](AsyncWebServerRequest *request) {
			Serial.println("POST Laser status");
			request->send(200, "text/plain", "tbd ToggleLaserStatus");
	});
  server.on("/scale/tara", HTTP_POST, [](AsyncWebServerRequest *request) {
			Serial.println("POST tara");
      //tbd
      //all Scales will be taraed
			request->send(200, "text/plain", "tbd tara Scale");
	});
  server.on("/scale/calibrate", HTTP_POST, [](AsyncWebServerRequest *request) {
			Serial.println("POST calibrate Scale");
      //tbd
      //JsonPayload with number of Scale to be calibrated and Calibration weight in g
      //Answer contains JSON with ScaleMultiplier
			request->send(200, "text/plain", "tbd Calibrate Scale");
	});
  
	ws.onEvent(onWsEvent);
	server.addHandler(&ws);
	server.begin();
}

void loop() {
	if (globalClient != NULL && globalClient->status() == WS_CONNECTED) {
		root["Scale1"] = String(random(0, 20));
		root["Scale2"] = String(random(0, 20));
		root["Scale3"] = String(random(0, 20));
		//ToD0: Can be optimized by using buffer https://github.com/me-no-dev/ESPAsyncWebServer#direct-access-to-web-socket-message-buffer
		String jsonString;
		serializeJson(root,jsonString);
		globalClient->text(jsonString);
	}
	delay(1000);
}