/*
* MIT License
*
* Copyright (c) 2022 thieu-b55
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

/*
 * IMPORTANT
 * This program uses ESP32 Core for Arduino IDE Version 3.0.X
 * ESP32 core for Arduini IDE Version 2.X will NOT work
 */
 

#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

typedef struct meter_data{
  float kwh_totaal;
  float injectie_totaal;
  float injectie_nu;
  float verbruik_nu;
  float gas_totaal;
  bool  relais1;
  bool  relais2;
  int   pwm1_sturing;
  int   pwm2_sturing;
}meter_data;
meter_data ingelezen;

unsigned long begin_millis;
int positie = 0;
int x = 1;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&ingelezen, incomingData, sizeof(ingelezen));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, positie);
  tft.print("Totaal electriciteit");
  tft.setCursor(200, positie);
  tft.print("Totaal injectie");
  tft.setCursor(400, positie);
  tft.print("Totaal gas");
  tft.setCursor(0, positie + 15);
  tft.print(ingelezen.kwh_totaal, 3);
  tft.print(" kWh");
  tft.setCursor(200, positie + 15);
  tft.print(ingelezen.injectie_totaal, 3);
  tft.print(" kWh");
  tft.setCursor(400, positie + 15);
  tft.print(ingelezen.gas_totaal, 3);
  tft.print(" m3");
  tft.setTextSize(2);
  if(ingelezen.relais1){
    tft.setCursor(20, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print("R1");
  }
  if(ingelezen.relais2){
    tft.setCursor(90, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print("R2");
  }
  if(ingelezen.pwm1_sturing > 0){
    tft.setCursor(330, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print(ingelezen.pwm1_sturing);
    tft.print("%");
  }
  if(ingelezen.pwm2_sturing > 0){
    tft.setCursor(420, positie + 40);
    tft.setTextColor(TFT_YELLOW);
    tft.print(ingelezen.pwm2_sturing);
    tft.print("%");
  }
   
  tft.setCursor(200, positie + 40);
  if(ingelezen.injectie_nu >= ingelezen.verbruik_nu){
    tft.setTextColor(TFT_GREEN);
    tft.print(ingelezen.injectie_nu, 3);
    tft.print(" kWh");
  }
  else{
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.print(ingelezen.verbruik_nu, 3);
    tft.print(" kWh");
  }
  positie = positie + x;
  if((positie == 250)|| (positie == 0)){
    x = x * -1;
  }
}
