#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "webpage.h"

// ==============================
// НАСТРОЙКИ WIFI
// ==============================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// ==============================
// НАСТРОЙКИ ЭКРАНА (SPI)
// ==============================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Укажите пины для подключения OLED SPI
#define OLED_MOSI   23 // D1
#define OLED_CLK    18 // D0
#define OLED_DC     22
#define OLED_CS     33 // Использование свободного пина 33 уберет спам в логах и ошибки "Invalid pin 255"
#define OLED_RESET  4

// Использование более быстрого и стабильного Hardware SPI
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);


// ==============================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// ==============================
AsyncWebServer server(80);
Preferences preferences;

struct Asset {
    String address;
    String symbol;
    double price;
    int type; // 0=Custom, 1=BTC, 2=ETH
    double p24;
    double p6;
    double p1;
    double m5;
    bool hasHistory;
};



std::vector<Asset> trackedAssets;
std::vector<String> addresses;
int currentAssetIndex = 0;

unsigned long lastDrawTime = 0;
const unsigned long DRAW_INTERVAL = 3000; // Смена тикера каждые 3 секунды

unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 20000; // Опрос DexScreener каждые 20 сек
bool forceUpdate = false;

// ==============================
// ОБЪЯВЛЕНИЕ ФУНКЦИЙ
// ==============================
void loadConfig();
void saveConfig();
void updatePrices();
void drawOLED();
void handlePostAssets(uint8_t *data, size_t len);

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Инициализация экрана
    if(!display.begin(SSD1306_SWITCHCAPVCC)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println("Connecting WiFi...");
    display.display();

    // Загрузка настроек
    preferences.begin("crypto-tracker", false);
    loadConfig();

    // Подключение к WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        display.print(".");
        display.display();
    }
    Serial.println("\nConnected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("WiFi Connected");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);

    // Настройка Web Сервера
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", INDEX_HTML);
    });

    server.on("/api/assets", HTTP_GET, [](AsyncWebServerRequest *request){
        DynamicJsonDocument doc(1024);
        JsonArray arr = doc.createNestedArray("addresses");
        for(String addr : addresses) {
            arr.add(addr);
        }
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        if (request->url() == "/api/assets" && request->method() == HTTP_POST) {
            handlePostAssets(data, len);
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    });

    server.begin();
    
    // Принудительно запускаем запрос при старте
    forceUpdate = true;
}

void loop() {
    unsigned long currentMillis = millis();

    // Запрос цен с DexScreener
    if (forceUpdate || (currentMillis - lastUpdateTime >= UPDATE_INTERVAL)) {
        lastUpdateTime = currentMillis;
        forceUpdate = false;
        
        updatePrices(); // Всегда обновляем, т.к. 2 базовых актива есть всегда
    }

    // Отрисовка на экране
    if (currentMillis - lastDrawTime >= DRAW_INTERVAL) {
        lastDrawTime = currentMillis;
        drawOLED();
        
        int totalScreens = trackedAssets.size() > 0 ? trackedAssets.size() + 1 : 1;
        if (totalScreens > 1) {
            currentAssetIndex = (currentAssetIndex + 1) % totalScreens;
        } else {
            currentAssetIndex = 0;
        }
    }
}

void loadConfig() {
    addresses.clear();
    String json = preferences.getString("addresses", "[]");
    
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (!error) {
        JsonArray arr = doc.as<JsonArray>();
        for(JsonVariant v : arr) {
            addresses.push_back(v.as<String>());
        }
    }
}

void saveConfig() {
    DynamicJsonDocument doc(1024);
    JsonArray arr = doc.to<JsonArray>();
    for(String addr : addresses) {
        arr.add(addr);
    }
    String json;
    serializeJson(doc, json);
    preferences.putString("addresses", json);
}

void handlePostAssets(uint8_t *data, size_t len) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, (const char*)data, len);
    if (!error) {
        addresses.clear();
        JsonArray arr = doc["addresses"];
        for(JsonVariant v : arr) {
            addresses.push_back(v.as<String>());
        }
        saveConfig();
        currentAssetIndex = 0;
        forceUpdate = true;
    }
}

void updatePrices() {
    if (WiFi.status() != WL_CONNECTED) return;

    WiFiClientSecure *client = new WiFiClientSecure;
    if(client) {
        client->setInsecure(); // Пропуск валидации сертификата
        HTTPClient https;
        
        std::vector<Asset> newAssets;
        
        // === 1. СТАТИЧНЫЕ АКТИВЫ (ETH, BTC) ===
        // DexScreener может отдать щиток, если искать просто по адресу контракта WETH/WBTC.
        // Поэтому мы запрашиваем конкретные сверхликвидные пулы Uniswap V3 (USDC-пары).
        Asset btcAsset; btcAsset.symbol = "BTC"; btcAsset.type = 1; btcAsset.price = 0.0; btcAsset.hasHistory = false;
        Asset ethAsset; ethAsset.symbol = "ETH"; ethAsset.type = 2; ethAsset.price = 0.0; ethAsset.hasHistory = false;

        String btcPair = "0x99ac8cA7087fA4A2A1FB6357269965A2014ABc35"; 
        String ethPair = "0x88e6a0c2ddd26feeb64f039a2c41296fcb3f5640"; 
        String pairsUrl = "https://api.dexscreener.com/latest/dex/pairs/ethereum/" + btcPair + "," + ethPair;
        
        if (https.begin(*client, pairsUrl)) {
            int httpCode = https.GET();
            if (httpCode == HTTP_CODE_OK) {
                String payload = https.getString();
                DynamicJsonDocument doc(32768);
                if (!deserializeJson(doc, payload)) {
                    JsonArray pairs = doc["pairs"];
                    for (JsonVariant pair : pairs) {
                        String pairAddr = pair["pairAddress"].as<String>();
                        if (pairAddr.equalsIgnoreCase(btcPair)) {
                            btcAsset.price = pair["priceUsd"].as<double>();
                            btcAsset.p24 = pair["priceChange"]["h24"].as<double>();
                            btcAsset.p6 = pair["priceChange"]["h6"].as<double>();
                            btcAsset.p1 = pair["priceChange"]["h1"].as<double>();
                            btcAsset.m5 = pair["priceChange"]["m5"].as<double>();
                            btcAsset.hasHistory = true;
                        } else if (pairAddr.equalsIgnoreCase(ethPair)) {
                            ethAsset.price = pair["priceUsd"].as<double>();
                            ethAsset.p24 = pair["priceChange"]["h24"].as<double>();
                            ethAsset.p6 = pair["priceChange"]["h6"].as<double>();
                            ethAsset.p1 = pair["priceChange"]["h1"].as<double>();
                            ethAsset.m5 = pair["priceChange"]["m5"].as<double>();
                            ethAsset.hasHistory = true;
                        }
                    }
                }
            }
            https.end();
        }
        
        // Добавляем их первыми
        newAssets.push_back(btcAsset);
        newAssets.push_back(ethAsset);

        // === 2. КАСТОМНЫЕ ТОКЕНЫ ИЗ НАСТРОЕК ===
        if (addresses.size() > 0) {
            String joined = "";
            for (int i = 0; i < addresses.size(); i++) {
                joined += addresses[i];
                if (i < addresses.size() - 1) joined += ",";
            }

            String url = "https://api.dexscreener.com/latest/dex/tokens/" + joined;
            
            if (https.begin(*client, url)) {
                int httpCode = https.GET();
                if (httpCode == HTTP_CODE_OK) {
                    String payload = https.getString();
                    DynamicJsonDocument doc(65536);
                    if (!deserializeJson(doc, payload)) {
                        JsonArray pairs = doc["pairs"];
                        for (String addr : addresses) {
                            addr.trim();
                            double maxVolume = -1.0;
                            JsonVariant bestPair;
                            bool found = false;

                            for (JsonVariant pair : pairs) {
                                String baseAddr = pair["baseToken"]["address"].as<String>();
                                baseAddr.trim();
                                if (baseAddr.equalsIgnoreCase(addr)) {
                                    double vol = pair["volume"]["h24"].as<double>();
                                    // Выбираем пару с максимальным объемом торгов, чтобы отсеять скам-сети
                                    if (vol >= maxVolume) {
                                        maxVolume = vol;
                                        bestPair = pair;
                                        found = true;
                                    }
                                }
                            }
                            
                            if (found) {
                                Asset a;
                                a.address = addr;
                                a.symbol = bestPair["baseToken"]["symbol"].as<String>();
                                a.type = 0;
                                a.price = bestPair["priceUsd"].as<double>();
                                a.p24 = bestPair["priceChange"]["h24"].as<double>();
                                a.p6  = bestPair["priceChange"]["h6"].as<double>();
                                a.p1  = bestPair["priceChange"]["h1"].as<double>();
                                a.m5  = bestPair["priceChange"]["m5"].as<double>();
                                a.hasHistory = true;
                                newAssets.push_back(a);
                            } else {
                                Asset a;
                                a.address = addr;
                                a.symbol = "???";
                                a.type = 0;
                                a.price = 0.0;
                                a.hasHistory = false;
                                newAssets.push_back(a);
                            }
                        }
                    }
                }
                https.end();
            }
        }

        trackedAssets = newAssets;
        if(currentAssetIndex >= trackedAssets.size()) {
            currentAssetIndex = 0;
        }

        delete client;
    }
}

void drawOLED() {
    display.clearDisplay();
    
    if (WiFi.status() != WL_CONNECTED) {
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.println("WiFi Disconnected!");
        display.display();
        return;
    }

    // === ЭКРАН НАСТРОЕК IP (Последний в цикле) ===
    if (trackedAssets.size() > 0 && currentAssetIndex == trackedAssets.size()) {
        display.setTextSize(2);
        display.setCursor(12, 0); // В желтой зоне
        display.println("Web Setup");
        
        display.setTextSize(1);
        display.setCursor(0, 25);
        display.println("IP Address:");
        
        display.setTextSize(1, 2); // Ширина х1, Высота х2 - так он будет узким и высоким, влезет без переносов!
        display.setCursor(0, 40);
        display.println(WiFi.localIP());
        
        // Progress bar at the bottom
        int totalScreens = trackedAssets.size() + 1;
        int step = 128 / totalScreens;
        display.fillRect(0, 62, step * (currentAssetIndex + 1), 2, SSD1306_WHITE);
        
        display.display();
        return;
    }

    if (trackedAssets.size() > 0 && currentAssetIndex < trackedAssets.size()) {
        Asset current = trackedAssets[currentAssetIndex];

        int16_t x1, y1; uint16_t w, h;
        
        // === 1. ВЕРХНЯЯ ЧАСТЬ (ЖЕЛТАЯ ЗОНА Y: 0 - 15) ===
        // Название тикера
        display.setTextSize(2);
        display.getTextBounds(current.symbol, 0, 0, &x1, &y1, &w, &h);
        display.setCursor(0, 0);
        display.print(current.symbol);
        
        // Рядом выводим процент за 24ч, прижатым в правы угол
        if (current.hasHistory) {
            String p24Str = "";
            if (current.p24 > 0) p24Str += "+";
            p24Str += String(current.p24, 1) + "%";
            
            display.setTextSize(1);
            display.getTextBounds(p24Str, 0, 0, &x1, &y1, &w, &h);
            display.setCursor(128 - w, 4); // Правый край
            display.print(p24Str);
        }

        // === 2. СРЕДНЯЯ ЧАСТЬ (СПАРКЛАЙН Y: 16 - 43) ===
        if (current.hasHistory && current.price > 0.0) {
            double p_now = current.price;
            double p_5m = p_now / (1.0 + current.m5 / 100.0);
            double p_1h = p_now / (1.0 + current.p1 / 100.0);
            double p_6h = p_now / (1.0 + current.p6 / 100.0);
            double p_24h= p_now / (1.0 + current.p24 / 100.0);

            double pts[5] = {p_24h, p_6h, p_1h, p_5m, p_now};
            double mn = pts[0], mx = pts[0];
            for(int i=1; i<5; i++) {
                if(pts[i] < mn) mn = pts[i];
                if(pts[i] > mx) mx = pts[i];
            }
            if(mx == mn) mx = mn + 0.1;

            int sx = 0;
            int sy = 16;
            int sw = 127; // На всю ширину
            int sh = 26; // 42 - 16 = 26px высота
            
            for(int i=0; i<4; i++) {
                int px1 = sx + (i * sw / 4);
                int py1 = sy + sh - (int)((pts[i] - mn) * sh / (mx - mn));
                int px2 = sx + ((i+1) * sw / 4);
                int py2 = sy + sh - (int)((pts[i+1] - mn) * sh / (mx - mn));
                display.drawLine(px1, py1, px2, py2, SSD1306_WHITE);
            }
        }

        // === 3. НИЖНЯЯ ЧАСТЬ (ЦЕНА Y: 44 - 60) ===
        String priceStr = "$";
        if (current.price == 0.0 && current.symbol == "???") {
            priceStr += "N/A";
        } else if (current.price < 0.0001) {
            priceStr += String(current.price, 8);
        } else if (current.price < 1.0) {
            priceStr += String(current.price, 4);
        } else {
            priceStr += String(current.price, 2);
        }
        
        display.setTextSize(2);
        display.getTextBounds(priceStr, 0, 0, &x1, &y1, &w, &h);
        if (w > 128) {
            display.setTextSize(1);
            display.getTextBounds(priceStr, 0, 0, &x1, &y1, &w, &h);
            display.setCursor((128 - w) / 2, 48); // Центр (мелкий текст)
        } else {
            display.setCursor((128 - w) / 2, 44); // Центр (крупный текст)
        }
        display.println(priceStr);

        // === ПРОГРЕСС БАР (Y: 62) ===
        int totalScreens = trackedAssets.size() + 1;
        int step = 128 / totalScreens;
        display.fillRect(0, 62, step * (currentAssetIndex + 1), 2, SSD1306_WHITE);

    } else {
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Connecting...");
        display.setCursor(0, 20);
        display.println("Fetching prices");
    }

    display.display();
}
