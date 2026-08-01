# ESP32 Crypto Ticker (OLED Display)

A small WiFi-connected crypto price ticker built with an ESP32 and an SH1106 OLED display. It pulls live prices and 7-day price change data from the CoinGecko API and shows Bitcoin, Ethereum, Dogecoin, and Shiba Inu on screen, refreshing every 30 seconds.

## Features

- Live prices for BTC, ETH, DOGE, and SHIB in USD
- 7-day price change percentage per coin
- Auto-refreshes every 30 seconds
- Non-blocking timing (`millis()`-based loop, no `delay()` freezing the board)
- Memory-efficient JSON parsing using ArduinoJson filters
- Clean OLED layout via U8g2

## Hardware Used

- ESP32 development board
- SH1106 128x64 I2C OLED display
- WiFi network with internet access

### Wiring

| OLED Pin | ESP32 Pin |
|----------|-----------|
| SDA      | GPIO 4    |
| SCL      | GPIO 5    |
| VCC      | 3.3V      |
| GND      | GND       |

Pin numbers are configurable in the sketch via `SDA_PIN` and `SCL_PIN`.

## Libraries Required

Install these via the Arduino IDE Library Manager:

- [WiFi](https://www.arduino.cc/en/Reference/WiFi) (built into ESP32 board package)
- [HTTPClient](https://www.arduino.cc/reference/en/libraries/httpclient/) (built into ESP32 board package)
- [ArduinoJson](https://arduinojson.org/) by Benoit Blanchon
- [Wire](https://www.arduino.cc/en/Reference/Wire) (built-in)
- [U8g2](https://github.com/olikraus/u8g2) by olikraus

## Setup

1. Clone or download this repository.
2. Open the `.ino` file in the Arduino IDE.
3. Install the libraries listed above.
4. Select your ESP32 board and correct COM port under **Tools**.
5. Update your WiFi credentials in the sketch:
   ```cpp
   const char* ssid     = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
6. Upload the sketch to your ESP32.
7. Once connected, the OLED will display live prices, updating every 30 seconds.

## How It Works

1. On boot, the ESP32 connects to WiFi and displays connection status on the OLED.
2. Every 30 seconds, it sends an HTTP GET request to the CoinGecko `/coins/markets` endpoint, requesting BTC, ETH, DOGE, and SHIB prices with 7-day change data.
3. The JSON response is parsed using ArduinoJson, filtered down to only the fields needed (`id`, `current_price`, `price_change_percentage_7d_in_currency`) to keep memory usage low.
4. Each coin's data is matched by ID and stored in local variables.
5. The screen is redrawn with updated prices and 7-day percentage changes, then pushed to the OLED in a single buffer update.

## API Reference

Data is sourced from the free [CoinGecko API](https://www.coingecko.com/en/api/documentation):

```
https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&ids=bitcoin,ethereum,dogecoin,shiba-inu&price_change_percentage=7d
```

No API key required for basic usage on the free tier.

## Customization

- **Add/remove coins:** update the `ids` parameter in `apiURL` and add matching `drawCoinRow()` calls.
- **Change refresh rate:** adjust `updateInterval` (in milliseconds).
- **Change time window:** swap `7d` in the URL and JSON field name for `24h`, `30d`, etc. (see CoinGecko docs for supported values).

## License

MIT — feel free to use, modify, and share.
