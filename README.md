# ESP32 Crypto Tracker

*[Read in English](#english)* | *[Читать на Русском](#на-русском)*

---

## English

A compact, stylish, hardware-based cryptocurrency price tracker featuring a stunning web interface and a beautiful 128x64 OLED display.

### ✨ Features
- **Real-Time Prices:** Automatically tracks real-time prices for BTC and ETH using the highly liquid Uniswap V3 USDC pools via DexScreener.
- **Custom Tokens:** Track up to 5 additional tokens simultaneously (memecoins, altcoins) across any blockchain available on DexScreener.
- **OLED HUD:** 
  - Dynamic Sparkline mini-charts representing 24h trends.
  - 24-hour percentage change.
  - Supports Two-Color OLEDs (Yellow/Blue).
- **Web UI:** Premium Glassmorphism web interface hosted entirely on the ESP32!
  - Add, preview, and configure new tokens directly from your phone.
  - Verifies token contracts dynamically before adding logic to the MCU.

### 🛠 Hardware Wiring
*Tested on an ESP32 WROOM / CP2102 with a 128x64 SPI SSD1306 OLED (Yellow/Blue model).*

| OLED Pin | ESP32 Pin | Note |
|---|---|---|
| **GND** | GND | Ground |
| **VCC** | 3V3 | Power |
| **D0 (CLK)** | D18 | Hardware SPI SCK |
| **D1 (MOSI)** | D23 | Hardware SPI MOSI |
| **RES** | D4  | Reset Pin |
| **DC** | D22 | Data/Command |
| **CS** | GND | Hardwired to ground (CS bypassed) |

### 🚀 Installation & Setup

1. **Install Arduino IDE Dependencies:**
   - [ArduinoJson](https://arduinojson.org/)
   - [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
   - [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
   - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) (Download ZIP)
   - [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) (Download ZIP)

2. **Configure Code:**
   Open `esp32_tracker_project.ino` and replace the WiFi credentials with your own:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
   ```

3. **Upload to ESP32:**
   - Partition Scheme: `Huge APP (3MB No OTA/1MB SPIFFS)`
   - Flash Mode: `DIO`
   - Erase All Flash Before Sketch Upload: `Enabled` (Recommended for first boot)

4. **Add Tokens:**
   - Power on the ESP32.
   - The OLED will display an IP Address (e.g., `192.168.x.x`).
   - Open that IP Address in your Desktop or Mobile browser.
   - Paste a DexScreener token contract address, preview it, and hit **"Save Config"**!

---

## На Русском

Аппаратный трекер цен на криптовалюту с WEB фронтом и esp32+OLED-дисплеем 128x64 бэком.

### ✨ Особенности
- **Цены в реальном времени:** Автоматически и безошибочно получает цены BTC и ETH из самых ликвидных пулов Uniswap V3 (через DexScreener).
- **Кастомные токены:** Отслеживайте до 5 любых дополнительных токенов (мемкоины, альткоины) в любой сети, доступной на DexScreener.
- **Интерфейс OLED:** 
  - Динамические мини-графики (Спарклайны) тренда за последние 24 часа.
  - Процент изменения за сутки.
  - Оптимизирован под классические двухцветные экраны (Желтая полоса сверху).
- **Web-интерфейс:** Премиальный дизайн в стиле Glassmorphism, работает прямо с микроконтроллера!
  - Добавляйте и удаляйте монеты прямо с мобильного телефона без перепрошивки ESP32.
  - Встроенная проверка контрактов через API перед добавлением.

### 🛠 Схема подключения
*Протестировано на плате ESP32 WROOM / CP2102 с SPI-экраном SSD1306 128x64.*

| Пин экрана | Пин ESP32 | Примечание |
|---|---|---|
| **GND** | GND | Земля |
| **VCC** | 3V3 | Питание |
| **D0 (CLK)** | D18 | Аппаратный SPI SCK |
| **D1 (MOSI)** | D23 | Аппаратный SPI MOSI |
| **RES** | D4  | Пин сброса |
| **DC** | D22 | Данные/Команды (Data/Command) |
| **CS** | GND | Припаян к земле напрямую (экономия пина) |

### 🚀 Установка и Запуск

1. **Библиотеки в Arduino IDE:**
   - [ArduinoJson](https://arduinojson.org/)
   - [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
   - [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
   - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) (Установка из ZIP)
   - [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) (Установка из ZIP)

2. **Настройка Кода:**
   Откройте файл `esp32_tracker_project.ino` и впишите данные своего Wi-Fi:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
   ```

3. **Загрузка в плату:**
   - Разметка памяти (Partition Scheme): `Huge APP (3MB No OTA/1MB SPIFFS)`
   - Flash Mode: `DIO`
   - Желательно включить **Erase All Flash Before Sketch Upload** для первой чистой прошивки.

4. **Добавление Монет:**
   - Включите ESP32.
   - Экран покажет локальный IP-адрес (например, `192.168.1.123`).
   - Откройте этот адрес в браузере с ПК или телефона.
   - Вставьте адрес смарт-контракта с DexScreener, проверьте его в предпросмотре и нажмите **"Save Config"**!

---

