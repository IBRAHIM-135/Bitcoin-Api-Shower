#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <U8g2lib.h>

#define SDA_PIN 4
#define SCL_PIN 5


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);                                                                               

const char* ssid     = "xxxx";
const char* password = "ccccvvvv";


const char* apiURL = "https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&ids=bitcoin,ethereum,dogecoin,shiba-inu&price_change_percentage=7d";

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 30000; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  u8g2.begin();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_mf);
  u8g2.setCursor(0, 15);
  u8g2.print("Connecting WiFi...");
  u8g2.sendBuffer();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("WiFi Connected!");
  u8g2.sendBuffer();
  delay(1000);

  fetchAndDisplay();
}

void loop() {
  if (millis() - lastUpdate > updateInterval) {
    fetchAndDisplay();
    lastUpdate = millis();
  }
}

int decimalsForPrice(float price) {
  if (price >= 100)   return 0;
  if (price >= 1)     return 2;
  if (price >= 0.01)  return 4;
  if (price >= 0.0001) return 6;
  return 8;
}

void drawCoinRow(int y, const char* symbol, float price, float change) {
  u8g2.setCursor(0, y);
  u8g2.print(symbol);
  u8g2.print(": $");
  u8g2.print(price, decimalsForPrice(price));
  u8g2.print(" ");
  if (change >= 0) u8g2.print("+");
  u8g2.print(change, 1);
  u8g2.print("%");
}

void fetchAndDisplay() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(apiURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();

    StaticJsonDocument<128> filter;
    filter[0]["id"] = true;
    filter[0]["current_price"] = true;
    filter[0]["price_change_percentage_7d_in_currency"] = true;

    StaticJsonDocument<768> doc;
    DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

    if (!err) {
      float btcPrice = 0, btcChange = 0;
      float ethPrice = 0, ethChange = 0;
      float dogePrice = 0, dogeChange = 0;
      float shibPrice = 0, shibChange = 0;

      JsonArray arr = doc.as<JsonArray>();
      for (JsonObject coin : arr) {
        const char* id = coin["id"];
        float price  = coin["current_price"];
        float change = coin["price_change_percentage_7d_in_currency"];

        if (strcmp(id, "bitcoin") == 0)      { btcPrice = price;  btcChange = change; }
        else if (strcmp(id, "ethereum") == 0){ ethPrice = price;  ethChange = change; }
        else if (strcmp(id, "dogecoin") == 0){ dogePrice = price; dogeChange = change; }
        else if (strcmp(id, "shiba-inu") == 0){ shibPrice = price; shibChange = change; }
      }

      u8g2.clearBuffer();

      u8g2.setFont(u8g2_font_profont12_mf);
      u8g2.setCursor(16, 10);
      u8g2.print("CRYPTO TICKER");
      u8g2.drawHLine(0, 13, 128);

      u8g2.setFont(u8g2_font_profont10_mf);
      drawCoinRow(23, "BTC",  btcPrice,  btcChange);
      drawCoinRow(33, "ETH",  ethPrice,  ethChange);
      drawCoinRow(43, "DOGE", dogePrice, dogeChange);
      drawCoinRow(53, "SHIB", shibPrice, shibChange);


      u8g2.setFont(u8g2_font_profont10_mf); 
      u8g2.setCursor(0, 62);
      u8g2.print("7d chg | 30s refresh");
      u8g2.sendBuffer();

    } else {
      Serial.println("JSON parse failed");
    }
  } else {
    Serial.printf("HTTP GET failed, code: %d\n", httpCode);
  }
  http.end();
}