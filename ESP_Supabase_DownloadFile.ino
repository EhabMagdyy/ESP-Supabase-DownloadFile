/*****************************************************************************************************************************
**********************************    Author  : Ehab Magdy Abdullah                      *************************************
**********************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  *************************************
**********************************    Youtube : https://www.youtube.com/@EhabMagdyy      *************************************
******************************************************************************************************************************/

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#define POTENTIOMETER_PIN  35
#elif ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define POTENTIOMETER_PIN  A0
#endif
#include <ArduinoJson.h>

// Wi-Fi credentials
#define SSID               "SSID"
#define PASSWORD           "PASSWORD"

// Supabase credentials
#define supabaseUrl       "supabaseUrl"
#define supabaseKey       "supabaseKey"
#define tableName         "tableName"

void setup()
{
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi\n");
}

void loop()
{
  sendData();         // Update Record of id=1
  getData();          // Read Record of id=1
  delay(5000);        // Wait 5 Seconds
}

void sendData()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected. Attempting to reconnect...");
    while (!WiFi.reconnect())
    {
      Serial.println("Reconnecting to WiFi...");
      delay(500);
    }
    Serial.println("WiFi reconnected.");
  }
  // Create a JSON object with your data
  StaticJsonDocument<200> jsonDoc;
  // Reading Potentiometer value
  jsonDoc["potentiometer"] = analogRead(POTENTIOMETER_PIN);

  // Serialize JSON to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Update an existing row | Send HTTP PATCH request to Supabase
#ifdef ESP32
  HTTPClient http;
  String endpoint = String(supabaseUrl) + "/rest/v1/" + tableName + "?id=eq.1"; // Update row with id=1
  http.begin(endpoint);
#elif ESP8266
  WiFiClientSecure client;
  client.setInsecure(); // not recommended for production
  HTTPClient http;
  String endpoint = String(supabaseUrl) + "/rest/v1/" + tableName + "?id=eq.1"; // Update row with id=1
  http.begin(client, endpoint);
#endif

  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabaseKey);
  http.addHeader("Authorization", "Bearer " + String(supabaseKey));
  // Optional: Return the updated row
  http.addHeader("Prefer", "return=representation");

  // Update the specified row
  int httpResponseCode = http.PATCH(jsonString);
  if (httpResponseCode > 0)
  {
    Serial.println("Transmitting:");
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  }
  else
  {
    Serial.println("Error in HTTP request");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    String response = http.getString();
    Serial.println("Response: " + response);
  }
  http.end();
}

void getData()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected. Attempting to reconnect...");
    while (!WiFi.reconnect())
    {
      Serial.println("Reconnecting to WiFi...");
      delay(500);
    }
    Serial.println("WiFi reconnected.");
  }

  // GET request
#ifdef ESP32
  HTTPClient http;
  String endpoint = String(supabaseUrl) + "/rest/v1/" + tableName + "?id=eq.1"; // Update row with id=1
  http.begin(endpoint);
#elif ESP8266
  WiFiClientSecure client;
  client.setInsecure(); // not recommended for production
  HTTPClient http;
  String endpoint = String(supabaseUrl) + "/rest/v1/" + tableName + "?id=eq.1"; // Update row with id=1
  http.begin(client, endpoint);
#endif

  http.addHeader("apikey", supabaseKey);
  http.addHeader("Authorization", "Bearer " + String(supabaseKey));

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0)
  {
    Serial.println("\nReceiving:");
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  }
  else
  {
    Serial.println("Error in HTTP request");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    String response = http.getString();
    Serial.println("Response: " + response);
  }
  http.end();
  Serial.println("\n============================================\n");
}
