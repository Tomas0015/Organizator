#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include "FS.h"
#include "SPIFFS.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strips = Adafruit_NeoPixel(255, 2, NEO_GRB + NEO_KHZ800);
WebServer server(80);
DNSServer dns;
IPAddress apIP(192, 168, 0, 1);
const char* hostname;
String temp;
File file;

void setup() {
	strips.begin();
	SPIFFS.begin();
	Serial.begin(115200);
	Serial.println();
	WiFi.persistent(false);
	file = SPIFFS.open("/net", "r");
	hostname = strdup(file.readStringUntil('\n').c_str());
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP(hostname, "", 7, false, 4);

	dns.start(53, "*", apIP);
	
	WiFi.hostname(hostname);

	Serial.println("Connecting...");
	WiFi.begin(file.readStringUntil('\n').c_str(), file.readStringUntil('\n').c_str());

	while(true){
		delay(500);
		if((WiFi.status() == WL_NO_SSID_AVAIL) || (WiFi.status() == WL_CONNECT_FAILED)){
			Serial.println(F("Fail, AP mode only."));
			WiFi.mode(WIFI_AP);
			break;
		}
		if(WiFi.status() == WL_CONNECTED){
			Serial.println(WiFi.localIP());
			break;
		}
	}

	server.onNotFound(webServerEvent);
	server.begin();
	MDNS.begin(hostname);

	//gconf API
	server.on("/gconf", HTTP_GET, []() {
		temp = "/" + server.argName(0);
		if(SPIFFS.exists(temp)){
			file = SPIFFS.open(temp, "r");
			server.streamFile(file, "text/plain");
			file.close();
		}else{
				server.send(404, "text/plain", "file existen't");
		}
	});

	//sconf API
	server.on("/sconf", HTTP_GET, []() {
		temp = "/" + server.argName(0);
		if(SPIFFS.exists(temp)){
			file = SPIFFS.open(temp, "w");
			Serial.printf("Writing file %s:\n", server.argName(0));
			Serial.println(server.arg(0));
			if(file.print(server.arg(0))){
				Serial.println("done");
			}
		file.close();
		server.send(201, "text/plain", "");
		}
	});

	//sLED API (arg 0 - LED num; arg 1 - LED strip pin; R; G; B)
	server.on("/sLED", HTTP_GET, []() {
		strips.setPin(atoi(server.arg(1).c_str()));
		Serial.printf("Setting LED #%d on pin %d to color %d %d %d\n", atoi(server.arg(0).c_str()), atoi(server.arg(1).c_str()), atoi(server.arg(2).c_str()), atoi(server.arg(3).c_str()), atoi(server.arg(4).c_str()));
		strips.setPixelColor(atoi(server.arg(0).c_str()), strips.Color(atoi(server.arg(2).c_str()), atoi(server.arg(3).c_str()), atoi(server.arg(4).c_str())));
		strips.show();
		server.send(200, "text/plain", "");
		});

	//sLEDs API (arg 0 - start LED num; arg 1 - LED num; arg 2 - LED strip pin; R; G; B)
	server.on("/sLEDs", HTTP_GET, []() {
		strips.setPin(atoi(server.arg(2).c_str()));
		Serial.printf("Setting LEDs from #%d, lenght #%d on pin %d to color %d %d %d\n", atoi(server.arg(0).c_str()), atoi(server.arg(1).c_str()), atoi(server.arg(2).c_str()), atoi(server.arg(3).c_str()), atoi(server.arg(4).c_str()), atoi(server.arg(5).c_str()));
		strips.fill(strips.Color(atoi(server.arg(3).c_str()), atoi(server.arg(4).c_str()), atoi(server.arg(5).c_str())), atoi(server.arg(0).c_str()), atoi(server.arg(1).c_str()));
		strips.show();
		server.send(200, "text/plain", "");
		});
}


void loop() {
	server.handleClient();
	dns.processNextRequest();
	delay(2);
}

void webServerEvent() {
	String URI = server.uri();
	if(URI == "/")
		URI = F("/index.htm");

	if(SPIFFS.exists(URI)){
		Serial.printf("web: %s\n", URI.c_str());
		server.sendHeader("Content-Encoding", "gzip");
		file = SPIFFS.open(URI, "r");
		server.streamFile(file, getContentType(URI));
		file.close();

	}else{
		server.sendHeader("Location", "http://" + String(hostname) + ".local/");
		server.send(302, "text/plain", "");
	}
}

String getContentType(String filename){
	if(filename.endsWith(".css")) return "text/css";
	else if(filename.endsWith(".js")) return "application/javascript";
	else if(filename.endsWith(".ico")) return "image/x-icon";
	return "text/html";
}
