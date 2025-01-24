/********************************************************************************************************************
*****************************    Author  : Ehab Magdy Abdullah                      *********************************
*****************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  *********************************
*****************************    Youtube : https://www.youtube.com/@EhabMagdyy      *********************************
*********************************************************************************************************************/

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif
#include <LittleFS.h>

// Wi-Fi credentials
#define ssid            "ssid"
#define password        "password"

// Supabase credentials
#define supabaseFileUrl "supabaseFileUrl"
#define apiKey          "apiKey"

// File path to save the downloaded file
#define filePath        "/file.bin"

void connectToWiFi()
{
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

// Download the file from Supabase Storage
void downloadFile()
{
  HTTPClient http;
  
#ifdef ESP32
  http.begin(supabaseFileUrl);
#elif ESP8266
  WiFiClientSecure client;
  client.setInsecure(); // not recommended for production
  http.begin(client, supabaseFileUrl);
#endif

  http.addHeader("Authorization", "Bearer " + String(apiKey));

  Serial.println("Downloading file...");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK)
  {
    File file = LittleFS.open(filePath, "w");
    if (!file)
    {
      Serial.println("Failed to open file for writing");
      return;
    }

    http.writeToStream(&file); // Write the response to the file
    file.close();
    Serial.println("File downloaded and saved to File System");
  }
  else
  {
    Serial.printf("Failed to download file, HTTP error code: %d\n", httpCode);
    Serial.printf("Error message: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

// Read the file from LittleFS and print its contents
void readFile()
{
  // Open file for reading
  File file = LittleFS.open(filePath, "r");
  if (!file)
  {
    Serial.println("Failed to open file");
    return;
  }

  uint8_t byteCount = 0;
  Serial.println("File content:");
  while (file.available())
  {
    uint8_t byte = file.read();
    Serial.printf("%02X ", byte);

    byteCount++;
    if (byteCount % 32 == 0)
      Serial.println();
  }

  file.close();
  Serial.println();
}

void setup()
{
  Serial.begin(115200);

  // Initialize LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("Failed to mount LittleFS file system");
    return;
  }
  else
  {
    Serial.println("LittleFS mounted successfully");
  }

  connectToWiFi();
  // Download the file from Supabase Storage
  downloadFile();
  // Read and print the file contents
  readFile();
}

void loop() {}