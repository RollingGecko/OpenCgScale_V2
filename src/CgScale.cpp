#include <Arduino.h>

#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "config.h"
#include "scale.h"
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//Defines Scale dummies
#ifdef SCALE_DUMMY
	scaleInterface *frontScale = new scaleDummy("front Scale");
	scaleInterface *mainScaleLeft = new scaleDummy("main left Scale");
	scaleInterface *mainScaleRight = new scaleDummy("main right Scale");
#endif
	
#ifndef SCALE_DUMMY
	scaleInterface *frontScale = new scale("front Scale");
	scaleInterface *mainScaleLeft = new scale("main left Scale");
	scaleInterface *mainScaleRight = new scale("main right Scale");
#endif

AsyncWebSocketClient *globalClient = NULL;

DynamicJsonDocument jsonMessage(1024);
JsonObject root = jsonMessage.to<JsonObject>();

//functions to store and load scaleMulitplier
boolean writeScaleMultiplierToFile(scaleInterface *front, scaleInterface *right, scaleInterface *left)
{
	boolean success;
	File scaleMultiplierConfigFile = SPIFFS.open("/config/scaleMultiplier.json", "w");
	if (!scaleMultiplierConfigFile)
	{
		Serial.println("Failed to open 'config/scaleMultiplier.json'");
		success = false;
	}
	else
	{
		DynamicJsonDocument scaleMuliplierConfigJson(1000);
		scaleMuliplierConfigJson["scaleMultiplierFront"] = front->getScaleMultiplier();
		scaleMuliplierConfigJson["scaleMultiplierRight"] = right->getScaleMultiplier();
		scaleMuliplierConfigJson["scaleMultiplierLeft"] = left->getScaleMultiplier();
		serializeJson(scaleMuliplierConfigJson, scaleMultiplierConfigFile);
		success = true;
	}
	scaleMultiplierConfigFile.close();
	return success;
}

void loadScaleMultiplierfromFile(scaleInterface *front, scaleInterface *right, scaleInterface *left)
{

	
	if (!SPIFFS.exists("/config/scaleMultiplier.json"))
	{
		Serial.println("Failed to open 'config/scaleMultiplier.json'; Standard Multiplier is used ");
		front->setScaleMultiplier(SCALEMULTIPLIER_FRONT);
		right->setScaleMultiplier(SCALEMULTIPLIER_RIGHT);
		left->setScaleMultiplier(SCALEMULTIPLIER_LEFT);
	}
	else
	{
		File scaleMultiplierConfigFile = SPIFFS.open("/config/scaleMultiplier.json", "r");
		DynamicJsonDocument scaleMuliplierConfigJson(1000);

		deserializeJson(scaleMuliplierConfigJson, scaleMultiplierConfigFile);

		front->setScaleMultiplier(scaleMuliplierConfigJson["scaleMultiplierFront"]);
		right->setScaleMultiplier(scaleMuliplierConfigJson["scaleMultiplierRight"]);
		left->setScaleMultiplier(scaleMuliplierConfigJson["scaleMultiplierLeft"]);
		
		scaleMultiplierConfigFile.close();

	}
};

//REST Handler

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

void onUnknownRequest(AsyncWebServerRequest *request)
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

	pinMode(LASER_PIN, OUTPUT);

#ifndef SCALE_DUMMY

	frontScale->init(SCALEDATAPIN_MAIN,SCALECLKPIN_MAIN,SCALEGAIN_MAIN);
	mainScaleLeft->init(SCALEDATAPIN_LEFT,SCALECLKPIN_LEFT,SCALEGAIN_LEFT);
	mainScaleRight->init(SCALEDATAPIN_RIGHT,SCALECLKPIN_RIGHT,SCALEGAIN_RIGHT);
	
	#endif // SCALE_DUMMY

loadScaleMultiplierfromFile(frontScale,mainScaleRight,mainScaleLeft);



	WiFi.softAP(ssid, password);
	WiFi.softAPConfig(local_IP, gateway, subnet);
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
	// server.on("/laser", HTTP_POST, [](AsyncWebServerRequest *request)
	// 		  {
	// 			  Serial.println("POST Laser status");
	// 			  request->send(200, "text/plain", "Laser toggled");
	// 		  });
	server.on("/scale/tara", HTTP_POST, [](AsyncWebServerRequest *request)
			  {
				  Serial.println("POST tara");

				  //all Scales will be taraed
				  mainScaleLeft->tare();
				  mainScaleRight->tare();
				  frontScale->tare();

				  request->send(200, "text/plain", "tbd tara Scale");
			  });
	server.on("/scale/storeMultiplier",HTTP_POST,[](AsyncWebServerRequest *request)
			{
				boolean successfull = false;
				successfull = writeScaleMultiplierToFile(frontScale, mainScaleRight, mainScaleLeft);
				if (successfull){
				 	request->send(200, "text/plain", "ScaleMultiplier stored in config/scaleMultiplier.json");
					 Serial.println("JSON stored");					 
				}
				else{
					request->send(400, "text/plain", "Failed to store config/scaleMultiplier.json");
					Serial.println("Failed to store JSON");
				}
			});
	server.on("/debug/scaleMultiplierJson", HTTP_GET,[](AsyncWebServerRequest *request)
			{
				if (SPIFFS.exists("/config/scaleMultiplier.json")){
					File configFile = SPIFFS.open("/config/scaleMultiplier.json","r");
					String message;
					// Serial.print("Content of JSON-File: ");
					// Serial.println(message);
					while (configFile.available())
					{
						message += char(configFile.read());
					}	
					request->send(200,"text/plain", message);

					configFile.close();
				}
				else{	
					request ->send(400, "text/plain","File does not exist!");	
				}
				
			});
#ifdef SCALE_DUMMY
	AsyncCallbackJsonWebHandler *setWeightHandler = new AsyncCallbackJsonWebHandler("/debug/setWeight", [](AsyncWebServerRequest *request, JsonVariant &json)
		{
			JsonObject jsonObj = json.as<JsonObject>();
			StaticJsonDocument<100> responseJson;

			frontScale->setDummyWeight(jsonObj["frontWeight"].as<int>());
			mainScaleRight->setDummyWeight(jsonObj["rightWeight"].as<int>());
			mainScaleLeft->setDummyWeight(jsonObj["leftWeight"].as<int>());

			request->send(200, "text/plain", "DummyWeight set");
		});

	server.addHandler(setWeightHandler);



#endif

	AsyncCallbackJsonWebHandler *setLaser = new AsyncCallbackJsonWebHandler("/laser", [](AsyncWebServerRequest *request, JsonVariant &json)
		{
			JsonObject jsonObj = json.as<JsonObject>();
			boolean laserStatus = jsonObj["laserOn"];
			// ToDo LaserHandler
			Serial.println("Laser on: " + String(laserStatus));
			if (laserStatus)
			{
				digitalWrite(LASER_PIN, HIGH);
				
			} else
			{
				digitalWrite(LASER_PIN, LOW);
			}
			
			request->send(200, "text/plain", "Laser on: " + String(laserStatus));
		});

	server.addHandler(setLaser);

	AsyncCallbackJsonWebHandler *scaleCalibrateHandler = new AsyncCallbackJsonWebHandler("/scale/calibrate", [](AsyncWebServerRequest *request, JsonVariant &json)
		{
			JsonObject jsonObj = json.as<JsonObject>();
			StaticJsonDocument<100> responseJson;
			String element = jsonObj["element"].as<String>();
			boolean success = false;
			int scaleMultiplierFromJson = jsonObj["weight"].as<int>();
			if (scaleMultiplierFromJson == 0)
			{
				responseJson["message"] = "Weight must be send";
			}
			else
			{

				if (element == "front")
				{
					int scaleMultiplier = frontScale->calibrate(scaleMultiplierFromJson);
					responseJson["scaleMultiplier"] = String(scaleMultiplier);
					success = true;
				}
				else if (element == "right")
				{
					int scaleMultiplier = mainScaleRight->calibrate(scaleMultiplierFromJson);
					responseJson["scaleMultiplier"] = String(scaleMultiplier);
					success = true;
				}
				else if (element == "left")
				{
					int scaleMultiplier = mainScaleLeft->calibrate(scaleMultiplierFromJson);
					responseJson["scaleMultiplier"] = String(scaleMultiplier);
					success = true;
				}
				else
				{

					responseJson["message"] = "unknown Element";
					Serial.println("unknown Element");
				}
			}

			String response;
			serializeJson(responseJson, response);

			if (success)
			{
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

	//tara all scale elements
	frontScale->tare();
	mainScaleRight->tare();
	mainScaleLeft->tare();
	
}

void loop()
{
	if (globalClient != NULL && globalClient->status() == WS_CONNECTED)
	{
		root["weightFront"] = frontScale->getWeight();
		root["weightLeft"] = mainScaleLeft->getWeight();
		root["weightRight"] = mainScaleRight->getWeight();
		//ToD0: Can be optimized by using buffer https://github.com/me-no-dev/ESPAsyncWebServer#direct-access-to-web-socket-message-buffer
		String jsonString;
		serializeJson(root, jsonString);
		globalClient->text(jsonString); //ToDo: Problem when changing between Pages and opening new websocket-connections
	}
	delay(100);
}