#include <Arduino.h>

#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "scale.h"

const char *ssid = "MyESP32AP";
const char *password = "testpassword";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

scaleInterface *frontScale = new scaleDummy("front Scale");
scaleInterface *mainScaleLeft = new scaleDummy("main left Scale");
scaleInterface *mainScaleRight = new scaleDummy("main right Scale");
AsyncWebSocketClient *globalClient = NULL;

DynamicJsonDocument jsonMessage(1024);
JsonObject root = jsonMessage.to<JsonObject>();

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{

	if (type == WS_EVT_CONNECT)
	{

		Serial.println("Websocket client connection received");
		globalClient = client;
	}
	else if (type == WS_EVT_DISCONNECT)
	{

		Serial.println("Websocket client connection finished");
		globalClient = NULL;
	}
}

void onUnknownRequest(AsyncWebServerRequest * request)
	{
		//Handle Unknown Request
		Serial.println("unknowen Request");
		request->send(404);
	};

void setup()
{
	Serial.begin(115200);
	Serial.println("Start of program!");

	if (!SPIFFS.begin(true))
	{
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
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  Serial.println("Incomming /htm -Event");
				  request->send(SPIFFS, "/home.html", "text/html");
			  });
	server.on("/calibrateForm", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  Serial.println("Incomming /htm -Event: /calibrateForm");
				  request->send(SPIFFS, "/CalibrateForm.html", "text/html");
			  });
	server.on("/laser", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  Serial.println("GET Laser status");
				  request->send(200, "text/plain", "tbd Laserstatus");
			  });
	server.on("/laser", HTTP_POST, [](AsyncWebServerRequest *request)
			  {
				  Serial.println("POST Laser status");
				  request->send(200, "text/plain", "tbd ToggleLaserStatus");
			  });
	server.on("/scale/tara", HTTP_POST, [](AsyncWebServerRequest *request)
			  {
				  Serial.println("POST tara");

				  //all Scales will be taraed
				  mainScaleLeft->tare();
				  mainScaleRight->tare();
				  frontScale->tare();

				  request->send(200, "text/plain", "tbd tara Scale");
			  });
	AsyncCallbackJsonWebHandler *scaleCalibrateHandler = new AsyncCallbackJsonWebHandler("/scale/calibrate", [](AsyncWebServerRequest *request, JsonVariant &json)
		{
			JsonObject jsonObj = json.as<JsonObject>();
			StaticJsonDocument<100> responseJson;
			String element = jsonObj["element"].as<String>();
			boolean success = false;
			if (element=="front"){
				int scaleMultiplier = frontScale->calibrate(jsonObj["weight"].as<int>());
				responseJson["scaleMultiplier"] = String(scaleMultiplier);
				success = true;	
			}
			else if (element=="right"){
				int scaleMultiplier = mainScaleRight->calibrate(jsonObj["weight"].as<int>());
				responseJson["scaleMultiplier"] = String(scaleMultiplier);
				success = true;	
			}
			else if (element=="left"){
				int scaleMultiplier = mainScaleRight->calibrate(jsonObj["weight"].as<int>());
				responseJson["scaleMultiplier"] = String(scaleMultiplier);
				success = true;	
			}
			else {

				responseJson["message"] = "unknown Element";
				Serial.println("unknown Element");
			}

			String response;
			serializeJson(responseJson,response);

			if (success){
				request->send(200, "application/json", response);
			}
			else
			{
				request->send(400, "application/json", response);
			}
			
		});
	server.addHandler(scaleCalibrateHandler);

	
	server.onNotFound(onUnknownRequest);

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);
	server.begin();
}

void loop()
{
	if (globalClient != NULL && globalClient->status() == WS_CONNECTED)
	{
		//root["Scale1"] = String(random(0, 20));
		//root["Scale2"] = String(random(0, 20));
		//root["Scale3"] = String(random(0, 20));
		root["Scale1"] = frontScale->getWeight();
		root["Scale2"] = mainScaleLeft->getWeight();
		root["Scale3"] = mainScaleRight->getWeight();
		//ToD0: Can be optimized by using buffer https://github.com/me-no-dev/ESPAsyncWebServer#direct-access-to-web-socket-message-buffer
		String jsonString;
		serializeJson(root, jsonString);
		globalClient->text(jsonString);
	}
	delay(1000);
}