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

const int ssr = 16;
const int freq = 10;
const int resolution = 8;

typedef struct pwm_data{
  int procent;
}
pwm_data;
pwm_data pwm_sturing;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

void setup() {
  Serial.begin(115200);
  ledcAttach(ssr, freq, resolution);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop(){
  
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&pwm_sturing, incomingData, sizeof(pwm_sturing));
  int uit = (pwm_sturing.procent * 255) / 100;
  Serial.println(uit);
  ledcWrite(ssr, uit);
}
