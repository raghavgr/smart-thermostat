// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define DHTPIN 21     // Digital pin connected to the DHT sensor
#define RELAYPIN 22
#define LEDPIN 2
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

const char *cssid = "NETGEAR59";
const char *cpassword = "sunnyflute798";
const char *cssid2 = "Rocket";
const char *cpassword2 = "cHeCkMeOuT";

WiFiServer server(80);

void setup() 
{
	Serial.begin(115200);
	Serial.println(F("Reset!!!"));
  WiFi.mode(WIFI_AP_STA);
	
	WiFi.softAP("r41p41","Sky'sDlimIt2141");
  WiFi.begin(cssid,cpassword);
  
	IPAddress myIP = WiFi.softAPIP();
  
  pinMode(LEDPIN, OUTPUT);
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  digitalWrite(RELAYPIN, LOW);
  
	server.begin();
	dht.begin();
}

struct TSReading
{
	float h;
	float t;
	float f;
	float hif;
	float hic;
};
int GetTemp(struct TSReading *CurrentReading)
{
	CurrentReading->h = dht.readHumidity();
	CurrentReading->t = dht.readTemperature();
	CurrentReading->f = dht.readTemperature(1);
	CurrentReading->hif = dht.computeHeatIndex(CurrentReading->f,CurrentReading->h);
	CurrentReading->hic = dht.computeHeatIndex(CurrentReading->t,CurrentReading->h,0);
	if(isnan(CurrentReading->h) || isnan(CurrentReading->t) || isnan(CurrentReading->f))
		return 0;
	return 1;
}
void DbgPrint()
{
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float h = dht.readHumidity();
	// Read temperature as Celsius (the default)
	float t = dht.readTemperature();
	// Read temperature as Fahrenheit (isFahrenheit = true)
	float f = dht.readTemperature(true);

	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) 
	{
		Serial.println(F("Failed to read from DHT sensor!"));
		return;
	}

	// Compute heat index in Fahrenheit (the default)
	float hif = dht.computeHeatIndex(f, h);
	// Compute heat index in Celsius (isFahreheit = false)
	float hic = dht.computeHeatIndex(t, h, false);

	Serial.print(F("Humidity: "));
	Serial.print(h);
	Serial.print(F("%  Temperature: "));
	Serial.print(t);
	Serial.print(F("°C "));
	Serial.print(f);
	Serial.print(F("°F  Heat index: "));
	Serial.print(hic);
	Serial.print(F("°C "));
	Serial.print(hif);
	Serial.println(F("°F"));
}
void loop() 
{
	// Wait a few seconds between measurements.
	delay(10);
	WiFiClient client = server.available();
	int i=0;
	if(client)
	{
		String currentLine = "";  
		while(client.connected())
		{
			if(client.available())
			{
				char c = client.read();
				if(c == '\n')
				{
					if(currentLine.length() == 0)
						break;
					else
					{
            if(currentLine == "TSU.unit")
            {
              TSReading d_Reading;
              if(GetTemp(&d_Reading))
              {
                client.println("Humidity");
                client.println(d_Reading.h);
                client.println("Temperature");
                client.println(d_Reading.t);
                client.println("Fahrenheit");
                client.println(d_Reading.f);
                client.println("Heat Index F");
                client.println(d_Reading.hif);
                client.println("Heat Index C");
                client.println(d_Reading.hic);
              }
              else
              {
                client.println("Failed to read from DHT");
              }
            }
           else if(currentLine == "TSU.SetRelayOn")
           {
              digitalWrite(LEDPIN, HIGH);
              digitalWrite(RELAYPIN, HIGH);
           }
           else if(currentLine == "TSU.SetRelayOff")
           {
              digitalWrite(LEDPIN, LOW);
              digitalWrite(RELAYPIN, LOW);
           }
						currentLine = "";
					}
				}
				else if (c != '\r')
				{
					currentLine += c;
				}
			}
		}
		client.stop();
	}
 else
 {
    //Serial.println("No New connection");
    WiFiClient targetcli;
    if(!targetcli.connect("rpande.co",80))
    {
        Serial.println("can't connect to rpande.co");
    }
    else
    {
        //Serial.println("connected to rpande.co");
        TSReading d_Reading;
        if(GetTemp(&d_Reading))
        {
          targetcli.print("GET /TSU/state/state.php?status=query&Fahrenheit=");
          targetcli.print(d_Reading.f);
          
          targetcli.print("&Humidity=");
          targetcli.print(d_Reading.h);
          
          targetcli.print("&Celsius=");
          targetcli.print(d_Reading.t);
          targetcli.print(" HTTP/1.1\r\n");
        }
        else
        {
          targetcli.print("GET /TSU/state/state.php?status=query HTTP/1.1\r\n");
          //Serial.println("DHT sensor Failure");
        }
        targetcli.print("Host: rpande.co\r\n\r\n");
        
        int i = 0;
        while(targetcli.available() == 0)
        {
            delay(20);
            if(i >= 5000)
                break;
            i += 20;
        }
        while(targetcli.available())
        {
            String line = targetcli.readStringUntil('\r');
            targetcli.read();
            if(line == "on")
            {
                digitalWrite(LEDPIN, HIGH);
                digitalWrite(RELAYPIN, HIGH);
            }
            if(line == "off")
            {
              digitalWrite(LEDPIN, LOW);
              digitalWrite(RELAYPIN, LOW);
            }
        }
    }
 }
	
}
