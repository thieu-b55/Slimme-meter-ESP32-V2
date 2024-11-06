/*
* MIT License
*
* Copyright (c) 2024 thieu-b55
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

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <esp_now.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <esp_task_wdt.h>       

esp_now_peer_info_t   peerInfo;
esp_err_t             result;

AsyncWebServer server(80);

Preferences pref;

#define RXD2          16
#define TXD2          17
#define BLINKIE       4
#define SD_CARD_ERROR 2

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void testDir(fs::FS &fs, const char * dirname, uint8_t levels);
void testFile(fs::FS &fs, const char * path);
void createDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void read_elektriciteit_verbruik_float(fs::FS &fs, const char * path);
void read_injectie_float(fs::FS &fs, const char * path);
void read_gas_verbruik_float(fs::FS &fs, const char * path);
void read_elektriciteit_verbruik_char(fs::FS &fs, const char * path);
void read_injectie_char(fs::FS &fs, const char * path);
void read_piek_char(fs::FS &fs, const char * path);
void read_gas_verbruik_char(fs::FS &fs, const char * path);
void read_dagen(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void dir_bestaat_test();
void file_bestaat_test();
void jaar_verbruik_injectie();
void maand_verbruik_injectie();
void dag_verbruik_injectie();
void uur_verbruik_injectie();
void lees_uur_verbruik_injectie();
void lees_dag_verbruik_injectie();
void lees_maand_verbruik_injectie();
void lees_jaar_verbruik_injectie();
void html_input();

/*
 * Display module
 */
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
meter_data    ingelezen;

/*
 * Relais module
 */
typedef struct relais_data{
  bool relais;
}relais_data;
relais_data   uitsturen;

/*
 * PWM module
 */
typedef struct pwm_data{
  int procent;
}
pwm_data;
pwm_data    pwm_sturing;


const char* INPUT_MACX_0 = "input_macx_0";
const char* INPUT_MACX_1 = "input_macx_1";
const char* INPUT_MACX_2 = "input_macx_2";
const char* INPUT_MACX_3 = "input_macx_3";
const char* INPUT_MACX_4 = "input_macx_4";
const char* INPUT_MACX_5 = "input_macx_5";

const char* MODULE_MIN = "module_min";
const char* MODULE_PLUS = "module_plus";
const char* MODULE_BEVESTIG = "module_bevestig";

const char* INPUT_KW_ON = "input_kw_on";
const char* INPUT_OVERRIDE = "input_override";
const char* INPUT_SCHAKEL_TIJD = "input_schakel_tijd";
const char* RELAIS_MODULE_MIN = "relais_module_min";
const char* RELAIS_MODULE_PLUS = "relais_module_plus";
const char* RELAIS_MODULE_BEVESTIG = "relais_module_bevestig";

const char* INPUT_PWM_1_KW = "input_pwm_1_kw";
const char* INPUT_PWM_1_TIJD_ON = "input_pwm_1_tijd_on";
const char* INPUT_PWM_1_TIJD_OFF = "input_pwm_1_tijd_off";
const char* INPUT_PWM_1_OVERRIDE = "input_pwm_1_override";
const char* BEVESTIG_PWM_1 = "bevestig_pwm_1";

const char* INPUT_PWM_2_KW = "input_pwm_2_kw";
const char* INPUT_PWM_2_OVERRIDE = "input_pwm_2_override";
const char* INPUT_PWM_2_DR = "input_pwm_2_dr";
const char* BEVESTIG_PWM_2 = "bevestig_pwm_2";

const char* INPUT_DATA_DAG = "input_data_dag";
const char* INPUT_DATA_MAAND = "input_data_maand";
const char* INPUT_DATA_JAAR = "input_data_jaar";

const char* BEVESTIG_PERIODE = "bevestig_periode";

/*
 * WiFi Netwerknaam en paswoord
 */
const char* APSSID = "ESP32Energie_V2";
const char* APPSWD = "ESP32pswd";

bool dag_dir_bestaat = false;
bool dir_test = false;
bool initialiseren = true;
bool elektriciteit_dag_file_bestaat = false;
bool elektriciteit_jaar_file_bestaat = false;
bool elektriciteit_maand_file_bestaat = false;
bool elektriciteit_uur_file_bestaat = false;
bool file_test = false;
bool gas_dag_file_bestaat = false;
bool gas_jaar_file_bestaat = false;
bool gas_maand_file_bestaat = false;
bool gas_uur_file_bestaat = false;
bool injectie_dag_file_bestaat = false;
bool injectie_jaar_file_bestaat = false;
bool injectie_uur_file_bestaat = false;
bool injectie_maand_file_bestaat = false;
bool jaar_dir_bestaat = false;
bool maand_dir_bestaat = false;
bool pwm1_tijd_gezet = false;
bool pwm1_tijd_gezet_vorig;
bool relais1_tijd = false;
bool relais1_uit = false;
bool relais1_uit_vorig = false;
bool relais2_tijd = false;
bool relais2_uit = false;
bool relais2_uit_vorig = false;
bool vijf_seconden = false;

char broadcastAddressX_0_char[8];
char broadcastAddressX_1_char[8];
char broadcastAddressX_2_char[8];
char broadcastAddressX_3_char[8];
char broadcastAddressX_4_char[8];
char broadcastAddressX_5_char[8];

char kwh_totaal_float_char[12];
char injectie_totaal_float_char[12];
char kwh_nu_float_char[12];
char injectie_nu_float_char[12];
char piek_nu_float_char[12];
char gas_totaal_float_char[12];
char verbruik_char[40];
char piek_char[12];
char aantal_dagen_file_char[18];
char aantal_dagen_char[5];
char module_char[20];
char relais_module_char[20];
char kw_on_char[12];
char override_char[8];
char schakel_delay_char[12];
char pwm1_tijd_on_char[8];
char pwm1_tijd_off_char[8];
char relais1_sturing_char[12];
char relais2_sturing_char[12];
char data_dag_char[4];
char data_maand_char[4];
char data_jaar_char[6];
char weergave_periode_char[12];
char periode_char[25];
char eenheid_char[10];
char totaal_char[10];
char kwh_sd_char[10];
char data_kwh_array_char[32][10];
char data_injectie_array_char[32][10];
char data_piek_array_char[32][10];
char data_gas_array_char[32][10];
char periode_array_char[32][10];
char injectie_sd_char[10];
char piek_sd_char[10];
char gas_sd_char[10];
char tijd_char[12];
char jaar_char[6];
char maand_char[8];
char dag_char[4];
char elektriciteit_jaar_file_char[14];
char injectie_jaar_file_char[14];
char gas_jaar_file_char[14];
char elektriciteit_maand_file_char[14];
char injectie_maand_file_char[14];
char piek_maand_file_char[14];
char gas_maand_file_char[14];
char elektriciteit_dag_file_char[18];
char injectie_dag_file_char[18];
char gas_dag_file_char[18];
char elektriciteit_uur_file_char[20];
char injectie_uur_file_char[20];
char gas_uur_file_char[20];
char uitsturing_pwm1_char[6];
char uitsturing_pwm2_char[6];
char schakel_tijd_char[12];
char pwm1_override_char[8];
char pwm2_override_char[8];

float relais1_on;
float relais2_on;
float pwm1_kw_float;
float pwm2_kw_float;
float pwm2_dr_float;
float data_verbruik_float[32];
float kwh_sd_float;
float injectie_sd_float;
float gas_sd_float;
float uitsturing_pwm1_float = 0.0;
float uitsturing_pwm2_float = 0.0;
float kwh_dag_float;
float kwh_nacht_float;
float kwh_totaal_float;
float injectie_dag_float;
float injectie_nacht_float;
float injectie_totaal_float;
float kwh_nu_float;
float piek_nu_float = 0.0;
float piek_vorig_float = 0.0;
float injectie_nu_float;
float verbruik_nu_float;
float gas_totaal_float = 0.0;
float verbruik_pwm1_float;
float verbruik_pwm2_float;

int uren_on1_int;
int uren_on2_int;
int uren_on3_int;
int uren_off3_int;
int minuten_on1_int;
int minuten_on2_int;
int minuten_on3_int;
int minuten_off3_int;
int uren_int;
int minuten_int;
int seconden_int;
int dag_int;
int maand_int;
int jaar_int;
int uren_vorig_int;
int dag_vorig_int;
int maand_vorig_int;
int jaar_vorig_int;
int data_dag_int = 01;
int data_maand_int = 10;
int data_jaar_int = 2024;
int uitsturing_pwm1_int = 0;
int uitsturing_pwm2_int = 0;
int module_teller = 0;
int relais_module_teller = 0;
int dagen_int;

String broadcastAddress1_string = "                       ";
String broadcastAddress2_string = "                       ";
String broadcastAddress3_string = "                       ";
String broadcastAddress4_string = "                       ";
String broadcastAddress5_string = "                       ";

String buffer_data =    "                                                         ";
String kwh_dag = "          ";
String kwh_nacht = "          ";
String injectie_dag = "          ";
String injectie_nacht = "          ";
String kwh_nu = "          ";
String injectie_nu = "          ";
String piek_nu = "      ";
String gas = "         ";
String kwh_totaal_string = "            ";
String injectie_totaal_string = "            ";
String gas_totaal_float_string = "            ";
String verbruik_string = "                                        ";
String piek_string = "                                        ";
String aantal_dagen_file_string = "                 ";
String aantal_dagen_string = "    ";
String relais1_override = "     ";
String relais2_override = "     ";
String weergave_periode_string ="           ";
String kwh_sd_string = "      ";
String injectie_sd_string = "      ";
String gas_sd_string = "       ";
String uren_string = "        ";
String minuten_string = "        ";
String tijd_string = "         ";
String jaar_string = "     ";
String maand_string = "       ";
String dag_string = "    ";
String elektriciteit_jaar_file_string = "             ";
String injectie_jaar_file_string = "             ";
String gas_jaar_file_string = "             ";
String elektriciteit_maand_file_string = "             ";
String injectie_maand_file_string = "             ";
String piek_maand_file_string = "             ";
String gas_maand_file_string = "             ";
String elektriciteit_dag_file_string = "                 ";
String injectie_dag_file_string = "                 ";
String gas_dag_file_string = "                 ";
String elektriciteit_uur_file_string = "                   ";
String injectie_uur_file_string = "                   ";
String gas_uur_file_string = "                   ";
String eenheid_string = "         ";
String totaal_string = "         ";
String pwm1_override = "    ";
String pwm2_override = "    ";

uint8_t broadcastAddress1[6];
uint8_t broadcastAddress2[6];
uint8_t broadcastAddress3[6];
uint8_t broadcastAddress4[6];
uint8_t broadcastAddress5[6];

uint8_t input_macx_0;
uint8_t input_macx_1;
uint8_t input_macx_2;
uint8_t input_macx_3;
uint8_t input_macx_4;
uint8_t input_macx_5;

unsigned long nu;

void setup() {
  /*
   * watchdog timer instellen op 15 seconden. Opvragen uur- en dagverbruik > 5 seconden (standaard watchdog timer instelling)
   * frequency SD kaart verhogen kan helpen 
   * 
   * Verhogen SD frequency voor Linux (Mint) gebruikers <Windows of iOS gebruikers zal wel ongeveer hetzelfde zijn>
   * 
   * in file /home/<gebruiker naam>/.arduino15/packages/esp32/hardware/esp32/3.0.5/libraries/SD/src/SD.h
   * regel 30 verander frequency = 4000000 naar frequency = 12000000 of hoger afhankelijk van de gebruikte SD kaart
   * 
   */
  esp_task_wdt_config_t config = {
    .timeout_ms = 15 * 1000,   //  15 seconds
    .trigger_panic = true,     // Trigger panic if watchdog timer is not reset
  };
  esp_task_wdt_reconfigure(&config);
  enableLoopWDT();
  /*
   * einde instellen watchdogtimer
   */
  delay(5000);
  pinMode(BLINKIE, OUTPUT);
  digitalWrite(BLINKIE, 0);
  pinMode(SD_CARD_ERROR, OUTPUT);
  digitalWrite(SD_CARD_ERROR, 0);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  if(!SD.begin(5)){
    Serial.println("Kontroleer SD kaart");
    digitalWrite(SD_CARD_ERROR, 1);
  }
  dag_dir_bestaat = false;
  jaar_dir_bestaat = false;
  maand_dir_bestaat = false;
  elektriciteit_dag_file_bestaat = false;
  elektriciteit_jaar_file_bestaat = false;
  elektriciteit_maand_file_bestaat = false;
  elektriciteit_uur_file_bestaat = false;
  gas_dag_file_bestaat = false;
  gas_jaar_file_bestaat = false;
  gas_maand_file_bestaat = false;
  gas_uur_file_bestaat = false;
  injectie_dag_file_bestaat = false;
  injectie_jaar_file_bestaat = false;
  injectie_uur_file_bestaat = false;
  injectie_maand_file_bestaat = false;
  initialiseren = true;
  
  pref.begin("data", false);
  if(pref.getString("controle") != "dummy geladen"){
    /*
     * Display
     */
    pref.putInt("mac1_0", 0xb0);
    pref.putInt("mac1_1", 0xa7);
    pref.putInt("mac1_2", 0x32);
    pref.putInt("mac1_3", 0xdb);
    pref.putInt("mac1_4", 0x2b);
    pref.putInt("mac1_5", 0x9c);
    /*
     * Relais 1
     */
    pref.putInt("mac2_0", 7);
    pref.putInt("mac2_1", 1);
    pref.putInt("mac2_2", 2);
    pref.putInt("mac2_3", 3);
    pref.putInt("mac2_4", 4);
    pref.putInt("mac2_5", 7);
    /*
     * Relais 2
     */
    pref.putInt("mac3_0", 2);
    pref.putInt("mac3_1", 1);
    pref.putInt("mac3_2", 2);
    pref.putInt("mac3_3", 3);
    pref.putInt("mac3_4", 4);
    pref.putInt("mac3_5", 5);
    /*
     * PWM 1
     */
    pref.putInt("mac4_0", 0x7c);
    pref.putInt("mac4_1", 0x9e);
    pref.putInt("mac4_2", 0xbd);
    pref.putInt("mac4_3", 0x06);
    pref.putInt("mac4_4", 0xb4);
    pref.putInt("mac4_5", 0xdc);
    /*
     * PWM 2
     */
    pref.putInt("mac5_0", 0x7c);
    pref.putInt("mac5_1", 0x87);
    pref.putInt("mac5_2", 0xce);
    pref.putInt("mac5_3", 0x30);
    pref.putInt("mac5_4", 0x7e);
    pref.putInt("mac5_5", 0x24);
        
    pref.putFloat("relais1_on", 2.0);
    pref.putString("relais1_ov", "0");
    pref.putFloat("relais2_on", 2.0);
    pref.putString("relais2_ov", "0");
    pref.putInt("uren_on1", 24);
    pref.putInt("minuten_on1", 0);
    pref.putInt("uren_on2", 24);
    pref.putInt("minuten_on2", 0);
    pref.putFloat("pwm1_kw", 0.0);
    pref.putInt("uren_on3", 24);
    pref.putInt("minuten_on3", 0);
    pref.putInt("uren_off3", 0);
    pref.putInt("minuten_off3", 0);
    pref.putString("pwm1_override", "0");
    pref.putFloat("pwm2_kw", 0.0);
    pref.putFloat("pwm2_dr", 0.0);
    pref.putString("pwm2_override", "0");
    pref.putString("controle", "dummy geladen");
  }
  broadcastAddress1[0] = pref.getInt("mac1_0");
  broadcastAddress1[1] = pref.getInt("mac1_1");
  broadcastAddress1[2] = pref.getInt("mac1_2");
  broadcastAddress1[3] = pref.getInt("mac1_3");
  broadcastAddress1[4] = pref.getInt("mac1_4");
  broadcastAddress1[5] = pref.getInt("mac1_5");
  broadcastAddress2[0] = pref.getInt("mac2_0");
  broadcastAddress2[1] = pref.getInt("mac2_1");
  broadcastAddress2[2] = pref.getInt("mac2_2");
  broadcastAddress2[3] = pref.getInt("mac2_3");
  broadcastAddress2[4] = pref.getInt("mac2_4");
  broadcastAddress2[5] = pref.getInt("mac2_5");
  broadcastAddress3[0] = pref.getInt("mac3_0");
  broadcastAddress3[1] = pref.getInt("mac3_1");
  broadcastAddress3[2] = pref.getInt("mac3_2");
  broadcastAddress3[3] = pref.getInt("mac3_3");
  broadcastAddress3[4] = pref.getInt("mac3_4");
  broadcastAddress3[5] = pref.getInt("mac3_5");
  broadcastAddress4[0] = pref.getInt("mac4_0");
  broadcastAddress4[1] = pref.getInt("mac4_1");
  broadcastAddress4[2] = pref.getInt("mac4_2");
  broadcastAddress4[3] = pref.getInt("mac4_3");
  broadcastAddress4[4] = pref.getInt("mac4_4");
  broadcastAddress4[5] = pref.getInt("mac4_5");
  broadcastAddress5[0] = pref.getInt("mac5_0");
  broadcastAddress5[1] = pref.getInt("mac5_1");
  broadcastAddress5[2] = pref.getInt("mac5_2");
  broadcastAddress5[3] = pref.getInt("mac5_3");
  broadcastAddress5[4] = pref.getInt("mac5_4");
  broadcastAddress5[5] = pref.getInt("mac5_5");
  relais1_on = pref.getFloat("relais1_on");
  relais1_override = pref.getString("relais1_ov");
  relais2_on = pref.getFloat("relais2_on");
  relais2_override = pref.getString("relais2_ov");
  uren_on1_int = pref.getInt("uren_on1");
  minuten_on1_int = pref.getInt("minuten_on1");
  uren_on2_int = pref.getInt("uren_on2");
  minuten_on2_int = pref.getInt("minuten_on2");
  pwm1_kw_float = pref.getFloat("pwm1_kw");
  uren_on3_int = pref.getInt("uren_on3");
  minuten_on3_int = pref.getInt("minuten_on3");
  uren_off3_int = pref.getInt("uren_off3");
  minuten_off3_int = pref.getInt("minuten_off3");
  pwm1_override = pref.getString("pwm1_override");
  pwm2_kw_float = pref.getFloat("pwm2_kw");
  pwm2_dr_float = pref.getFloat("pwm2_dr");
  pwm2_override = pref.getString("pwm2_override");
  broadcastAddress1_string = "";
  broadcastAddress1_string = broadcastAddress1_string + String(broadcastAddress1[0])+ String(broadcastAddress1[1])
                                                      + String(broadcastAddress1[2])+ String(broadcastAddress1[3])
                                                      + String(broadcastAddress1[4])+ String(broadcastAddress1[5]);
  broadcastAddress2_string = "";
  broadcastAddress2_string = broadcastAddress2_string + String(broadcastAddress2[0])+ String(broadcastAddress2[1])
                                                      + String(broadcastAddress2[2])+ String(broadcastAddress2[3])
                                                      + String(broadcastAddress2[4])+ String(broadcastAddress2[5]);
  broadcastAddress3_string = "";
  broadcastAddress3_string = broadcastAddress3_string + String(broadcastAddress3[0])+ String(broadcastAddress3[1])
                                                      + String(broadcastAddress3[2])+ String(broadcastAddress3[3])
                                                      + String(broadcastAddress3[4])+ String(broadcastAddress3[5]);                            
  broadcastAddress4_string = "";
  broadcastAddress4_string = broadcastAddress4_string + String(broadcastAddress4[0])+ String(broadcastAddress4[1])
                                                      + String(broadcastAddress4[2])+ String(broadcastAddress4[3])
                                                      + String(broadcastAddress4[4])+ String(broadcastAddress4[5]);
  broadcastAddress5_string = "";
  broadcastAddress5_string = broadcastAddress5_string + String(broadcastAddress5[0])+ String(broadcastAddress5[1])
                                                      + String(broadcastAddress5[2])+ String(broadcastAddress5[3])
                                                      + String(broadcastAddress5[4])+ String(broadcastAddress5[5]); 

                                                 
  WiFi.disconnect();                                                      
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(APSSID, APPSWD);
  
  delay(5000);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
   
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 1");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 2");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 3");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress4, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 4");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress5, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer 5");
    return;
  }
  delay(1000);
  if(relais1_override == "0"){
    uitsturen.relais = false;
    relais1_uit = false;
  }
  if(relais1_override == "1"){
    uitsturen.relais = true;
    relais1_uit = true;
  }
  result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
  if (result == ESP_OK) {
    Serial.println("Met succes verzonden relais 1");
  }
  else {
    Serial.println("fout bij verzenden naar relais 1");
  }
  delay(1000);
  if(relais2_override == "0"){
    uitsturen.relais = false;
    relais2_uit = false;
  }
  if(relais2_override == "1"){
    uitsturen.relais = true;
    relais2_uit = true;
  }
  result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
  if (result == ESP_OK) {
    Serial.println("Met succes verzonden relais 2");
  }
  else {
    Serial.println("fout bij verzenden naar relais 2");
  }
  delay(1000);
  html_input();
  nu = millis();
}

void loop() {
  while(Serial2.available()){
    char lees_byte = Serial2.read();
    if(lees_byte == 0x2f){
      kwh_dag_float = kwh_dag.toFloat();
      kwh_nacht_float = kwh_nacht.toFloat();
      kwh_totaal_float = kwh_nacht_float + kwh_dag_float;
      injectie_dag_float = injectie_dag.toFloat();
      injectie_nacht_float = injectie_nacht.toFloat();
      injectie_totaal_float = injectie_nacht_float + injectie_dag_float;
      kwh_nu_float = kwh_nu.toFloat();
      piek_nu_float = piek_nu.toFloat();
      injectie_nu_float = injectie_nu.toFloat();
      verbruik_nu_float = injectie_nu_float - kwh_nu_float;
      gas_totaal_float = gas.toFloat();
      if(initialiseren == true){
        if((kwh_dag_float > 0.1) && (kwh_nacht_float > 0.1)){
          if((injectie_dag_float > 0.1) && (injectie_nacht_float > 0.1)){
            if(gas_totaal_float > 0.1){
              uren_vorig_int = uren_int;
              dag_vorig_int = dag_int;
              maand_vorig_int = maand_int;
              jaar_vorig_int = jaar_int;
              initialiseren = false;
              dir_bestaat_test();
              file_bestaat_test();
            }
          }
        }
      }
      digitalWrite(BLINKIE, (digitalRead(BLINKIE) ^ 1));
      if(initialiseren == false){
        if(jaar_vorig_int != jaar_int){
        jaar_string = "/F" + String(jaar_int);
        jaar_string.toCharArray(jaar_char, jaar_string.length() + 1);
        createDir(SD, jaar_char);
        }
        if(maand_vorig_int != maand_int){
          maand_string = "/F" + String(jaar_int) + "/F" + String (maand_int);
          maand_string.toCharArray(maand_char, maand_string.length() + 1);
          createDir(SD, maand_char);
          aantal_dagen_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/dagen";
          aantal_dagen_file_string.toCharArray(aantal_dagen_file_char, aantal_dagen_file_string.length() + 1);
          aantal_dagen_string = String(dag_int);
          aantal_dagen_string.toCharArray(aantal_dagen_char, aantal_dagen_string.length() + 1);
          writeFile(SD, aantal_dagen_file_char, aantal_dagen_char);
        } 
        if(dag_vorig_int != dag_int){
          dag_string = "/F" + String(jaar_int) + "/F" + String (maand_int) + "/F" + String(dag_int);;
          dag_string.toCharArray(dag_char, dag_string.length() + 1);
          createDir(SD, dag_char);
        }
        if(uren_vorig_int != uren_int){
          uur_verbruik_injectie();
        }
        if(dag_vorig_int != dag_int){
          dag_verbruik_injectie();
        }
        if(maand_vorig_int != maand_int){
          maand_verbruik_injectie();
        }
        if(jaar_vorig_int != jaar_int){
          jaar_verbruik_injectie();
        }
        jaar_vorig_int = jaar_int;
        maand_vorig_int = maand_int;
        dag_vorig_int = dag_int;
        uren_vorig_int = uren_int;
        piek_vorig_float = piek_nu_float;
      }

      /*      
       * Elke 5 seconden PWM 1 / PWM 2
       */
      if(((millis() - nu) > 5000) && (!vijf_seconden)){
        vijf_seconden = true;
        /*
         * PWM 1
         */
        pwm1_tijd_gezet_vorig = pwm1_tijd_gezet;
        if((uren_on3_int == uren_int) && (minuten_on3_int == minuten_int)){
          pwm1_tijd_gezet = true;                                                 //PWM 100% uitgestuurd tussen uren_on3_int en uren_off3_int 
        }
        if((uren_off3_int == uren_int) && (minuten_off3_int == minuten_int)){
          pwm1_tijd_gezet = false;
        }
        if((pwm1_tijd_gezet_vorig) && (!pwm1_tijd_gezet)){
          uitsturing_pwm1_int = 0;
        }
        if((!pwm1_tijd_gezet) && (uitsturing_pwm2_int == 0)){
          uitsturing_pwm1_float = uitsturing_pwm1_float + ((verbruik_nu_float / pwm1_kw_float) / 2);
          if(uitsturing_pwm1_float > 1.0){
            uitsturing_pwm1_float = 1.0;
          }
          if(uitsturing_pwm1_float < 0.0){
            uitsturing_pwm1_float = 0.0;
          }
          uitsturing_pwm1_int = uitsturing_pwm1_float * 100;
        }
        if(pwm1_tijd_gezet == true){
          uitsturing_pwm1_int = 100;
        }
        if(pwm1_override == "0"){
          uitsturing_pwm1_int = 0;
          uitsturing_pwm1_float = 0.0;
        }
        if(pwm1_override == "1"){
          uitsturing_pwm1_int = 100;
          uitsturing_pwm1_float = 0.0;
        }
        pwm_sturing.procent = uitsturing_pwm1_int;
        result = esp_now_send(broadcastAddress4, (uint8_t *) &pwm_sturing, sizeof(pwm_sturing));
        if (result == ESP_OK) {
          Serial.println("Sent pwm1 with success");
        }
        else {
          Serial.println("pwm1 error");
        }
        /*
         * PWM 2
         */
        if(uitsturing_pwm1_int == 100){
          uitsturing_pwm2_float = uitsturing_pwm2_float + (((verbruik_nu_float - pwm2_dr_float)/ pwm2_kw_float) / 4); //regelen pwm_dr_float boven PWM1 verbruik
          if(uitsturing_pwm2_float > 1.0){
            uitsturing_pwm2_float = 1.0;
          }
          if(uitsturing_pwm2_float < 0.0){
            uitsturing_pwm2_float = 0.0;
          }
          uitsturing_pwm2_int = uitsturing_pwm2_float * 100;
        }
        if((verbruik_nu_float < 0.0) || (uitsturing_pwm1_int < 100)){
          uitsturing_pwm2_int = 0;
          uitsturing_pwm2_float = 0.0;
        }
        if(pwm2_override == "0"){
          uitsturing_pwm2_int = 0;
          uitsturing_pwm2_float = 0.0;
        }
        if(pwm2_override == "1"){
          uitsturing_pwm2_int = 100;
          uitsturing_pwm2_float = 0.0;
        }
        
        pwm_sturing.procent = uitsturing_pwm2_int;
        result = esp_now_send(broadcastAddress5, (uint8_t *) &pwm_sturing, sizeof(pwm_sturing));
        if (result == ESP_OK) {
          Serial.println("Sent pwm2 with success");
        }
        else {
          Serial.println("pwm2 error");
        }
      }
      /*
       *  Elke 10 seconden PWM 1 / PWM 2 / Display
       */
      if((millis() - nu) > 10000){
        nu = millis();
        vijf_seconden = false;
        /*
         * PWM 1
         */
        pwm1_tijd_gezet_vorig = pwm1_tijd_gezet;
        if((uren_on3_int == uren_int) && (minuten_on3_int == minuten_int)){
          pwm1_tijd_gezet = true;                                                 //PWM 100% uitgestuurd tussen uren_on3_int en uren_off3_int
        }
        if((uren_off3_int == uren_int) && (minuten_off3_int == minuten_int)){
          pwm1_tijd_gezet = false;
        }
        if((pwm1_tijd_gezet_vorig) && (!pwm1_tijd_gezet)){
          uitsturing_pwm1_int = 0;
        }
        if((!pwm1_tijd_gezet) && (uitsturing_pwm2_int == 0)){
          uitsturing_pwm1_float = uitsturing_pwm1_float + ((verbruik_nu_float / pwm1_kw_float) / 2);
          if(uitsturing_pwm1_float > 1.0){
            uitsturing_pwm1_float = 1.0;
          }
          if(uitsturing_pwm1_float < 0.0){
            uitsturing_pwm1_float = 0.0;
          }
          uitsturing_pwm1_int = uitsturing_pwm1_float * 100;
        }
        else{
          uitsturing_pwm1_int = 100;
        }
        if(pwm1_override == "0"){
          uitsturing_pwm1_int = 0;
          uitsturing_pwm1_float = 0.0;
        }
        if(pwm1_override == "1"){
          uitsturing_pwm1_int = 100;
          uitsturing_pwm1_float = 0.0;
        }
        pwm_sturing.procent = uitsturing_pwm1_int;
        result = esp_now_send(broadcastAddress4, (uint8_t *) &pwm_sturing, sizeof(pwm_sturing));
        if (result == ESP_OK) {
          Serial.println("Sent pwm1 with success");
        }
        else {
          Serial.println("pwm1 error");
        }
        /*
         * PWM 2
         */
        if(uitsturing_pwm1_int == 100){
          uitsturing_pwm2_float = uitsturing_pwm2_float + (((verbruik_nu_float - pwm2_dr_float) / pwm2_kw_float) / 4);
          if(uitsturing_pwm2_float > 1.0){
            uitsturing_pwm2_float = 1.0;
          }
          if(uitsturing_pwm2_float < 0.0){
            uitsturing_pwm2_float = 0.0;
          }
          uitsturing_pwm2_int = uitsturing_pwm2_float * 100;
        }
        if((verbruik_nu_float < 0.0) || (uitsturing_pwm1_int < 100)){
          uitsturing_pwm2_int = 0;
          uitsturing_pwm2_float = 0.0;
        }
        if(pwm2_override == "0"){
          uitsturing_pwm2_int = 0;
          uitsturing_pwm2_float = 0.0;
        }
        if(pwm2_override == "1"){
          uitsturing_pwm2_int = 100;
          uitsturing_pwm2_float = 0.0;
        }
        pwm_sturing.procent = uitsturing_pwm2_int;
        result = esp_now_send(broadcastAddress5, (uint8_t *) &pwm_sturing, sizeof(pwm_sturing));
        if (result == ESP_OK) {
          Serial.println("Sent pwm2 with success");
        }
        else {
          Serial.println("pwm2 error");
        }
        /*
         *  Display
         */
        ingelezen.kwh_totaal = kwh_totaal_float;
        ingelezen.injectie_totaal = injectie_totaal_float;
        ingelezen.verbruik_nu = kwh_nu_float;
        ingelezen.injectie_nu = injectie_nu_float;
        ingelezen.gas_totaal = gas_totaal_float;
        ingelezen.relais1 = relais1_uit;
        ingelezen.relais2 = relais2_uit;
        ingelezen.pwm1_sturing = uitsturing_pwm1_int;
        ingelezen.pwm2_sturing = uitsturing_pwm2_int;
        result = esp_now_send(broadcastAddress1, (uint8_t *) &ingelezen, sizeof(ingelezen));
        if (result == ESP_OK) {
          Serial.println("Met succes verzonden Display");
        }
        else {
          Serial.println("display error");
        }
      }
          
      /*
       * Digitale uitgangen
       */
      if((uren_int == 0) && (minuten_int == 0)){
        relais1_tijd = false;
        relais1_uit = false;
        relais2_tijd = false;
        relais2_uit = false; 
        uitsturen.relais = false;
        result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
        if (result == ESP_OK) {
          Serial.println("Met succes verzonden relais 1");
        }
        else {
          Serial.println("fout bij verzenden naar relais 1");
        }
        delay(100);
        result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
        if (result == ESP_OK) {
          Serial.println("Met succes verzonden relais 2");
        }
        else {
          Serial.println("fout bij verzenden naar relais 2");
        }
      }
      /*
       * Relais 1
       */
      if(relais1_override == "A"){
        if((uren_int == uren_on1_int) && (minuten_int == minuten_on1_int) && (relais1_tijd == false)){
          relais1_tijd = true;
        }
        if((relais1_tijd == true) && (relais1_on >= verbruik_pwm1_float) && (relais1_uit == false)){
          relais1_uit = true;
        }
      }
      if(relais1_override == "0"){
        relais1_tijd = false;
        relais1_uit = false;
      }
      if(relais1_override == "1"){
        relais1_tijd = false;
        relais1_uit = true;
      }
      if(relais1_uit != relais1_uit_vorig){
        relais1_uit_vorig = relais1_uit;
        uitsturen.relais = relais1_uit;
        result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
        if (result == ESP_OK) {
          Serial.println("Met succes verzonden relais 1");
        }
        else {
          Serial.println("fout bij verzenden naar relais 1");
        }
      }
      /*
       * Relais 2
       */
      if(relais2_override == "A"){
        if((uren_int == uren_on2_int) && (minuten_int == minuten_on2_int) && (relais2_tijd == false)){
          relais2_tijd = true;
        }
        if((relais2_tijd == true) && (relais2_on >= verbruik_pwm2_float) && (relais2_uit == false)){
          relais2_uit = true;
        }
      }
      if(relais2_override == "0"){
        relais2_tijd = false;
        relais2_uit = false;
      }
      if(relais2_override == "1"){
        relais2_tijd = false;
        relais2_uit = true;
      }
      if(relais2_uit != relais2_uit_vorig){
        relais2_uit_vorig = relais2_uit;
        uitsturen.relais = relais2_uit;
        result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
        if (result == ESP_OK) {
          Serial.println("Met succes verzonden relais 2");
        }
        else {
          Serial.println("fout bij verzenden naar relais 2");
        }
      }
      buffer_data = "";     //leesbuffer wissen
    }
    /*
     * inlezen data van P1 poort in buffer_data
     */
    buffer_data += lees_byte;
    if(lees_byte == 0x0a){
      if((buffer_data.substring(4,9)) == "1.0.0"){
        jaar_int = ((buffer_data.substring(10,12)).toInt()) + 2000;
        maand_int = (buffer_data.substring(12,14)).toInt();;
        dag_int = (buffer_data.substring(14,16)).toInt();
        uren_int = (buffer_data.substring(16,18)).toInt();;
        minuten_int = (buffer_data.substring(18,20)).toInt();;
      }
      if((buffer_data.substring(4,9)) == "1.8.1"){
        kwh_dag = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "1.8.2"){
        kwh_nacht = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "2.8.1"){
        injectie_dag = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "2.8.2"){
        injectie_nacht = buffer_data.substring(10,20);
      }
      if((buffer_data.substring(4,9)) == "1.7.0"){
        kwh_nu = buffer_data.substring(10,16);
      }
      if((buffer_data.substring(4,9)) == "2.7.0"){
        injectie_nu = buffer_data.substring(10,16);
      }
      if((buffer_data.substring(4,9)) == "1.6.0"){
        piek_nu = buffer_data.substring(25,31);
      }
      if((buffer_data.substring(4,10)) == "24.2.3"){
        gas = buffer_data.substring(26,35);
      }
      //Serial.print(buffer_data);
      buffer_data = "";
    }
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void testDir(fs::FS &fs, const char * dirname, uint8_t levels){
  //Serial.printf("Test dir: %s\n", dirname);
  File root = fs.open(dirname);
  if(root){
    //Serial.println("Folder bestaat reeds");
    dir_test = true;
  }
}
    
void testFile(fs::FS &fs, const char * path){
  File file = fs.open(path);
  if(file){
      file_test = true;
      file.close();
  }
}

void createDir(fs::FS &fs, const char * path){
  //Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
      //Serial.println("Dir created");
  } else {
      //Serial.println("mkdir failed");
  }
}

void readFile(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
      //Serial.println("Failed to open file for reading");
      return;
  }
  //Serial.print("Read from file: ");
  while(file.available()){
      Serial.write(file.read());
  }
  file.close();
}

void read_elektriciteit_verbruik_float(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  int teller = 0;
  memset(kwh_sd_char, 0, sizeof(kwh_sd_char));
  while(file.available()){
    kwh_sd_char[teller] = file.read();
    teller++;
  }
  file.close();
  kwh_sd_string = String(kwh_sd_char);
  kwh_sd_float = kwh_sd_string.toFloat();
  //Serial.printf("%.3f", kwh_sd_float);
  //Serial.println();
}
 
void read_injectie_float(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  int teller = 0;
  memset(injectie_sd_char, 0, sizeof(injectie_sd_char));
  while(file.available()){
    injectie_sd_char[teller] = file.read();
    teller++;
  }
  file.close();
  injectie_sd_string = String(injectie_sd_char);
  injectie_sd_float = injectie_sd_string.toFloat();
  // Serial.printf("%.3f", injectie_sd_float);
  // Serial.println();
}

void read_gas_verbruik_float(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  int teller = 0;
  memset(gas_sd_char, 0, sizeof(gas_sd_char));
  while(file.available()){
    gas_sd_char[teller] = file.read();
    teller++;
  }
  file.close();
  gas_sd_string = String(gas_sd_char);
  gas_sd_float = gas_sd_string.toFloat();
  // Serial.printf("%.3f", gas_sd_float);
  // Serial.println();
}

void read_elektriciteit_verbruik_char(fs::FS &fs, const char * path){
  // Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  int teller = 0;
  memset(kwh_sd_char, 0, sizeof(kwh_sd_char));
  while(file.available()){
    kwh_sd_char[teller] = file.read();
    teller++;
  }
  file.close();
  //kwh_sd_string = String(kwh_sd_char);
  // Serial.println(kwh_sd_char);
}
 
void read_injectie_char(fs::FS &fs, const char * path){
  // Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  int teller = 0;
  memset(injectie_sd_char, 0, sizeof(injectie_sd_char));
  while(file.available()){
    injectie_sd_char[teller] = file.read();
    teller++;
  }
  file.close();
  // Serial.println(injectie_sd_char);
}

void read_piek_char(fs::FS &fs, const char * path){
  // Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  int teller = 0;
  memset(piek_sd_char, 0, sizeof(piek_sd_char));
  while(file.available()){
    piek_sd_char[teller] = file.read();
    teller++;
  }
  file.close();
  // Serial.println(piek_sd_char);
}
  
void read_gas_verbruik_char(fs::FS &fs, const char * path){
  // Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  int teller = 0;
  memset(gas_sd_char, 0, sizeof(gas_sd_char));
  while(file.available()){
    gas_sd_char[teller] = file.read();
    teller++;
  }
  file.close();
  // Serial.println(gas_sd_char);
}

void read_dagen(fs::FS &fs, const char * path){
  File file = fs.open(path);
  int teller = 0;
  memset(aantal_dagen_char, 0, sizeof(aantal_dagen_char));
  while(file.available()){
    aantal_dagen_char[teller] = file.read();
    teller++;
  }
  file.close();
  aantal_dagen_string = String(aantal_dagen_char);
  dagen_int = aantal_dagen_string.toInt();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  // Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file){
      // Serial.println("Failed to open file for writing");
      return;
  }
  if(file.print(message)){
      // Serial.println("File written");
  } else {
      // Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  // Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file){
      // Serial.println("Failed to open file for appending");
      return;
  }
  if(file.print(message)){
      // Serial.println("Message appended");
  } else {
      // Serial.println("Append failed");
  }
  file.close();
}

void dir_bestaat_test(){
  if(!jaar_dir_bestaat){
      dir_test = false;
      jaar_string = "/F" + String(jaar_int);
      jaar_string.toCharArray(jaar_char, jaar_string.length() + 1);
      testDir(SD, jaar_char, 1);
      if(dir_test == false){
        createDir(SD, jaar_char);
      }
      jaar_dir_bestaat = true;
  }
  if(!maand_dir_bestaat){
    dir_test = false;
    maand_string = "/F" + String(jaar_int) + "/F" + String (maand_int);
    maand_string.toCharArray(maand_char, maand_string.length() + 1);
    testDir(SD, maand_char, 1);
    if(dir_test == false){
      createDir(SD, maand_char);
    }
    maand_dir_bestaat = true;
  }
  if(!dag_dir_bestaat){
    dir_test = false;
    dag_string = "/F" + String(jaar_int) + "/F" + String (maand_int) + "/F" + String(dag_int);;
    dag_string.toCharArray(dag_char, dag_string.length() + 1);
    testDir(SD, dag_char, 1);
    if(dir_test == false){
      createDir(SD, dag_char);
    }
    dag_dir_bestaat = true;
  }
}

void file_bestaat_test(){
  if(!elektriciteit_jaar_file_bestaat){
    file_test = false;
    elektriciteit_jaar_file_string = "/F" + String(jaar_int) + "/E" + jaar_int;
    elektriciteit_jaar_file_string.toCharArray(elektriciteit_jaar_file_char, elektriciteit_jaar_file_string.length() + 1);
    testFile(SD, elektriciteit_jaar_file_char);
    if(file_test == false){
      verbruik_string = String(kwh_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, elektriciteit_jaar_file_char, verbruik_char);
    }
    elektriciteit_jaar_file_bestaat = true;
  }
  if(!injectie_jaar_file_bestaat){
    file_test = false;
    injectie_jaar_file_string = "/F" + String(jaar_int) + "/I" + jaar_int;
    injectie_jaar_file_string.toCharArray(injectie_jaar_file_char, injectie_jaar_file_string.length() + 1);
    testFile(SD, injectie_jaar_file_char);
    if(file_test == false){
      verbruik_string = String(injectie_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, injectie_jaar_file_char, verbruik_char);
    }
    injectie_jaar_file_bestaat = true;
  }
  if(!gas_jaar_file_bestaat){
    file_test = false;
    gas_jaar_file_string = "/F" + String(jaar_int) + "/G" + jaar_int;
    gas_jaar_file_string.toCharArray(gas_jaar_file_char, gas_jaar_file_string.length() + 1);
    testFile(SD, gas_jaar_file_char);
    if(file_test == false){
      verbruik_string = String(gas_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, gas_jaar_file_char, verbruik_char);
    }
    gas_jaar_file_bestaat = true;
  }
  if(!elektriciteit_maand_file_bestaat){
    file_test = false;
    elektriciteit_maand_file_string = "/F" + String(jaar_int) + "/E" + maand_int;
    elektriciteit_maand_file_string.toCharArray(elektriciteit_maand_file_char, elektriciteit_maand_file_string.length() + 1);
    testFile(SD, elektriciteit_maand_file_char);
    if(file_test == false){
      verbruik_string = String(kwh_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, elektriciteit_maand_file_char, verbruik_char);
    }
    elektriciteit_maand_file_bestaat = true;
  }
  if(!injectie_maand_file_bestaat){
    file_test = false;
    injectie_maand_file_string = "/F" + String(jaar_int) + "/I" + maand_int;
    injectie_maand_file_string.toCharArray(injectie_maand_file_char, injectie_maand_file_string.length() + 1);
    testFile(SD, injectie_maand_file_char);
    if(file_test == false){
      verbruik_string = String(injectie_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, injectie_maand_file_char, verbruik_char);
    }
    injectie_maand_file_bestaat = true;
  }
  if(!gas_maand_file_bestaat){
    file_test = false;
    gas_maand_file_string = "/F" + String(jaar_int) + "/G" + maand_int;
    gas_maand_file_string.toCharArray(gas_maand_file_char, gas_maand_file_string.length() + 1);
    testFile(SD, gas_maand_file_char);
    if(file_test == false){
      verbruik_string = String(gas_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, gas_maand_file_char, verbruik_char);
    }
    gas_maand_file_bestaat = true;
  }
  if(!elektriciteit_dag_file_bestaat){
    file_test = false;
    elektriciteit_dag_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/E" + dag_int;
    elektriciteit_dag_file_string.toCharArray(elektriciteit_dag_file_char, elektriciteit_dag_file_string.length() + 1);
    testFile(SD, elektriciteit_dag_file_char);
    if(file_test == false){
      verbruik_string = String(kwh_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, elektriciteit_dag_file_char, verbruik_char);
    }
    elektriciteit_dag_file_bestaat = true;
  }
  if(!injectie_dag_file_bestaat){
    file_test = false;
    injectie_dag_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/I" + dag_int;
    injectie_dag_file_string.toCharArray(injectie_dag_file_char, injectie_dag_file_string.length() + 1);
    testFile(SD, injectie_dag_file_char);
    if(file_test == false){
      verbruik_string = String(injectie_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, injectie_dag_file_char, verbruik_char);
    }
    injectie_dag_file_bestaat = true;
  }
  if(!gas_dag_file_bestaat){
    file_test = false;
    gas_dag_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/G" + dag_int;
    gas_dag_file_string.toCharArray(gas_dag_file_char, gas_dag_file_string.length() + 1);
    testFile(SD, gas_dag_file_char);
    if(file_test == false){
      verbruik_string = String(gas_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, gas_dag_file_char, verbruik_char);
    }
    gas_dag_file_bestaat = true;
  }
  if(!elektriciteit_uur_file_bestaat){
    file_test = false;
    elektriciteit_uur_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/F" + String(dag_int) + "/E" + uren_int;
    elektriciteit_uur_file_string.toCharArray(elektriciteit_uur_file_char, elektriciteit_uur_file_string.length() + 1);
    testFile(SD, elektriciteit_uur_file_char);
    if(file_test == false){
      verbruik_string = String(kwh_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, elektriciteit_uur_file_char, verbruik_char);
    }
    elektriciteit_uur_file_bestaat = true;
  }
  if(!injectie_uur_file_bestaat){
    file_test = false;
    injectie_uur_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/F" + String(dag_int) + "/I" + uren_int;
    injectie_uur_file_string.toCharArray(injectie_uur_file_char, injectie_uur_file_string.length() + 1);
    testFile(SD, gas_uur_file_char);
    if(file_test == false){
      verbruik_string = String(injectie_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, injectie_uur_file_char, verbruik_char);
    }
    injectie_uur_file_bestaat = true;
  }
  if(!gas_uur_file_bestaat){
    file_test = false;
    gas_uur_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/F" + String(dag_int) + "/G" + uren_int;
    gas_uur_file_string.toCharArray(gas_uur_file_char, gas_uur_file_string.length() + 1);
    testFile(SD, gas_uur_file_char);
    if(file_test == false){
      verbruik_string = String(gas_totaal_float, 3);
      verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
      writeFile(SD, gas_uur_file_char, verbruik_char);
    }
    gas_uur_file_bestaat = true;
  }
}

void jaar_verbruik_injectie(){
  elektriciteit_jaar_file_string = "/F" + String(jaar_vorig_int) + "/E" + jaar_vorig_int;
  elektriciteit_jaar_file_string.toCharArray(elektriciteit_jaar_file_char, elektriciteit_jaar_file_string.length() + 1);
  read_elektriciteit_verbruik_float(SD, elektriciteit_jaar_file_char);
  verbruik_string = String((kwh_totaal_float - kwh_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_jaar_file_char, verbruik_char);
    
  injectie_jaar_file_string = "/F" + String(jaar_vorig_int) + "/I" + jaar_vorig_int;
  injectie_jaar_file_string.toCharArray(injectie_jaar_file_char, injectie_jaar_file_string.length() + 1);
  read_injectie_float(SD, injectie_jaar_file_char);
  verbruik_string = String((injectie_totaal_float - injectie_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_jaar_file_char, verbruik_char);
    
  gas_jaar_file_string = "/F" + String(jaar_vorig_int) + "/G" + jaar_vorig_int;
  gas_jaar_file_string.toCharArray(gas_jaar_file_char, gas_jaar_file_string.length() + 1);
  read_gas_verbruik_float(SD, gas_jaar_file_char);
  verbruik_string = String((gas_totaal_float - gas_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_jaar_file_char, verbruik_char);
    
  jaar_string = "/F" + String(jaar_int);
  jaar_string.toCharArray(jaar_char, jaar_string.length() + 1);
  createDir(SD, jaar_char);
  
  maand_string = "/F" + String(jaar_int) + "/F" + String (maand_int);
  maand_string.toCharArray(maand_char, maand_string.length() + 1);
  createDir(SD, maand_char);
  
  elektriciteit_jaar_file_string = "/F" + String(jaar_int) + "/E" + jaar_int;
  elektriciteit_jaar_file_string.toCharArray(elektriciteit_jaar_file_char, elektriciteit_jaar_file_string.length() + 1);
  verbruik_string = String(kwh_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_jaar_file_char, verbruik_char);
  
  injectie_jaar_file_string = "/F" + String(jaar_int) + "/I" + jaar_int;
  injectie_jaar_file_string.toCharArray(injectie_jaar_file_char, injectie_jaar_file_string.length() + 1);
  verbruik_string = String(injectie_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_jaar_file_char, verbruik_char);
  
  gas_jaar_file_string = "/F" + String(jaar_int) + "/G" + jaar_int;
  gas_jaar_file_string.toCharArray(gas_jaar_file_char, gas_jaar_file_string.length() + 1);
  verbruik_string = String(gas_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_jaar_file_char, verbruik_char);
}

void maand_verbruik_injectie(){
  elektriciteit_maand_file_string = "/F" + String(jaar_vorig_int) + "/E" + maand_vorig_int;
  elektriciteit_maand_file_string.toCharArray(elektriciteit_maand_file_char, elektriciteit_maand_file_string.length() + 1);
  read_elektriciteit_verbruik_float(SD, elektriciteit_maand_file_char);
  verbruik_string = String((kwh_totaal_float - kwh_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_maand_file_char, verbruik_char);
    
  injectie_maand_file_string = "/F" + String(jaar_vorig_int) + "/I" + maand_vorig_int;
  injectie_maand_file_string.toCharArray(injectie_maand_file_char, injectie_maand_file_string.length() + 1);
  read_injectie_float(SD, injectie_maand_file_char);
  verbruik_string = String((injectie_totaal_float - injectie_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_maand_file_char, verbruik_char);

  piek_maand_file_string = "/F" + String(jaar_vorig_int) + "/P" + maand_vorig_int;
  piek_maand_file_string.toCharArray(piek_maand_file_char, piek_maand_file_string.length() + 1);
  piek_string = String(piek_vorig_float, 3);
  piek_string.toCharArray(piek_char, piek_string.length() + 1);
  writeFile(SD, piek_maand_file_char, piek_char);
    
  gas_maand_file_string = "/F" + String(jaar_vorig_int) + "/G" + maand_vorig_int;
  gas_maand_file_string.toCharArray(gas_maand_file_char, gas_maand_file_string.length() + 1);
  read_gas_verbruik_float(SD, gas_maand_file_char);
  verbruik_string = String((gas_totaal_float - gas_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_maand_file_char, verbruik_char);
    
  maand_string = "/F" + String(jaar_int) + "/F" + String (maand_int);
  maand_string.toCharArray(maand_char, maand_string.length() + 1);
  createDir(SD, maand_char);
  
  elektriciteit_maand_file_string = "/F" + String(jaar_int) + "/E" + maand_int;
  elektriciteit_maand_file_string.toCharArray(elektriciteit_maand_file_char, elektriciteit_maand_file_string.length() + 1);
  verbruik_string = String(kwh_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_maand_file_char, verbruik_char);
  
  elektriciteit_maand_file_bestaat = true;
  injectie_maand_file_string = "/F" + String(jaar_int) + "/I" + maand_int;
  injectie_maand_file_string.toCharArray(injectie_maand_file_char, injectie_maand_file_string.length() + 1);
  verbruik_string = String(injectie_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_maand_file_char, verbruik_char);
  
  injectie_maand_file_bestaat = true;
  gas_maand_file_string = "/F" + String(jaar_int) + "/G" + maand_int;
  gas_maand_file_string.toCharArray(gas_maand_file_char, gas_maand_file_string.length() + 1);
  verbruik_string = String(gas_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_maand_file_char, verbruik_char);
  gas_maand_file_bestaat = true;
}

void dag_verbruik_injectie(){
  elektriciteit_dag_file_string = "/F" + String(jaar_vorig_int) + "/F" + String(maand_vorig_int) + "/E" + dag_vorig_int;
  elektriciteit_dag_file_string.toCharArray(elektriciteit_dag_file_char, elektriciteit_dag_file_string.length() + 1);
  read_elektriciteit_verbruik_float(SD, elektriciteit_dag_file_char);
  verbruik_string = String((kwh_totaal_float - kwh_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_dag_file_char, verbruik_char);
    
  injectie_dag_file_string = "/F" + String(jaar_vorig_int) + "/F" + String(maand_vorig_int) + "/I" + dag_vorig_int;
  injectie_dag_file_string.toCharArray(injectie_dag_file_char, injectie_dag_file_string.length() + 1);
  read_injectie_float(SD, injectie_dag_file_char);
  verbruik_string = String((injectie_totaal_float - injectie_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_dag_file_char, verbruik_char);
    
  gas_dag_file_string = "/F" + String(jaar_vorig_int) + "/F" + String(maand_vorig_int) + "/G" + dag_vorig_int;
  gas_dag_file_string.toCharArray(gas_dag_file_char, gas_dag_file_string.length() + 1);
  read_gas_verbruik_float(SD, gas_dag_file_char);
  verbruik_string = String((gas_totaal_float - gas_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_dag_file_char, verbruik_char);
    
  elektriciteit_dag_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/E" + dag_int;
  elektriciteit_dag_file_string.toCharArray(elektriciteit_dag_file_char, elektriciteit_dag_file_string.length() + 1);
  verbruik_string = String(kwh_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_dag_file_char, verbruik_char);
    
  injectie_dag_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/I" + dag_int;
  injectie_dag_file_string.toCharArray(injectie_dag_file_char, injectie_dag_file_string.length() + 1);
  verbruik_string = String(injectie_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_dag_file_char, verbruik_char);
    
  gas_dag_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/G" + dag_int;
  gas_dag_file_string.toCharArray(gas_dag_file_char, gas_dag_file_string.length() + 1);
  verbruik_string = String(gas_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_dag_file_char, verbruik_char);
    
  aantal_dagen_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/dagen";
  aantal_dagen_file_string.toCharArray(aantal_dagen_file_char, aantal_dagen_file_string.length() + 1);
  aantal_dagen_string = String(dag_int);
  aantal_dagen_string.toCharArray(aantal_dagen_char, aantal_dagen_string.length() + 1);
  writeFile(SD, aantal_dagen_file_char, aantal_dagen_char);
}

void uur_verbruik_injectie(){
  elektriciteit_uur_file_string = "/F" + String(jaar_vorig_int) + "/F" + String(maand_vorig_int) + "/F" + String(dag_vorig_int) + "/E" + uren_vorig_int;
  elektriciteit_uur_file_string.toCharArray(elektriciteit_uur_file_char, elektriciteit_uur_file_string.length() + 1);
  read_elektriciteit_verbruik_float(SD, elektriciteit_uur_file_char);
  verbruik_string = String((kwh_totaal_float - kwh_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_uur_file_char, verbruik_char);
   
  injectie_uur_file_string = "/F" + String(jaar_int) + "/F" + String(maand_vorig_int) + "/F" + String(dag_vorig_int) + "/I" + uren_vorig_int;
  injectie_uur_file_string.toCharArray(injectie_uur_file_char, injectie_uur_file_string.length() + 1);
  read_injectie_float(SD, injectie_uur_file_char);
  verbruik_string = String((injectie_totaal_float - injectie_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_uur_file_char, verbruik_char);
    
  gas_uur_file_string = "/F" + String(jaar_vorig_int) + "/F" + String(maand_vorig_int) + "/F" + String(dag_vorig_int) + "/G" + uren_vorig_int;
  gas_uur_file_string.toCharArray(gas_uur_file_char, gas_uur_file_string.length() + 1);
  read_gas_verbruik_float(SD, gas_uur_file_char);
  verbruik_string = String((gas_totaal_float - gas_sd_float), 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_uur_file_char, verbruik_char);
    
  elektriciteit_uur_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/F" + String(dag_int) + "/E" + uren_int;
  elektriciteit_uur_file_string.toCharArray(elektriciteit_uur_file_char, elektriciteit_uur_file_string.length() + 1);
  verbruik_string = String(kwh_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, elektriciteit_uur_file_char, verbruik_char);
    
  injectie_uur_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/F" + String(dag_int) + "/I" + uren_int;
  injectie_uur_file_string.toCharArray(injectie_uur_file_char, injectie_uur_file_string.length() + 1);
  verbruik_string = String(injectie_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, injectie_uur_file_char, verbruik_char);
    
  gas_uur_file_string = "/F" + String(jaar_int) + "/F" + String(maand_int) + "/F" + String(dag_int) + "/G" + uren_int;
  gas_uur_file_string.toCharArray(gas_uur_file_char, gas_uur_file_string.length() + 1);
  verbruik_string = String(gas_totaal_float, 3);
  verbruik_string.toCharArray(verbruik_char, verbruik_string.length() + 1);
  writeFile(SD, gas_uur_file_char, verbruik_char);
}

void lees_uur_verbruik_injectie(){
  int eind_uur_int;
  if((data_jaar_int == jaar_int) && (data_maand_int == maand_int) && (data_dag_int == dag_int)){
    eind_uur_int = uren_int;
  }
  else{
    eind_uur_int = 24;
  }
  eenheid_string = "uur";
  eenheid_string.toCharArray(eenheid_char, eenheid_string.length() + 1);
  for(int lees_uur_int = 0; lees_uur_int < eind_uur_int; lees_uur_int++){
    sprintf(periode_array_char[lees_uur_int], "%02d", lees_uur_int);
    elektriciteit_uur_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/F" + String(data_dag_int) + "/E" + lees_uur_int;
    elektriciteit_uur_file_string.toCharArray(elektriciteit_uur_file_char, elektriciteit_uur_file_string.length() + 1);
    file_test = false;
    testFile(SD, elektriciteit_uur_file_char);
    if(file_test == true){
      read_elektriciteit_verbruik_char(SD, elektriciteit_uur_file_char);
      strcpy(data_kwh_array_char[lees_uur_int], kwh_sd_char);
    }
    else{
      memset(data_kwh_array_char[lees_uur_int], 0, sizeof(data_kwh_array_char[lees_uur_int]));
    }
    injectie_uur_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/F" + String(data_dag_int) + "/I" + lees_uur_int;
    injectie_uur_file_string.toCharArray(injectie_uur_file_char, injectie_uur_file_string.length() + 1);
    file_test = false;
    testFile(SD, injectie_uur_file_char);
    if(file_test == true){
      read_injectie_char(SD, injectie_uur_file_char);
      strcpy(data_injectie_array_char[lees_uur_int], injectie_sd_char);
    }
    else{
      memset(data_injectie_array_char[lees_uur_int], 0, sizeof(data_injectie_array_char[lees_uur_int]));
    }
    gas_uur_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/F" + String(data_dag_int) + "/G" + lees_uur_int;
    gas_uur_file_string.toCharArray(gas_uur_file_char, gas_uur_file_string.length() + 1);
    file_test = false;
    testFile(SD, gas_uur_file_char);
    if(file_test == true){
      read_gas_verbruik_char(SD, gas_uur_file_char);
      strcpy(data_gas_array_char[lees_uur_int], gas_sd_char);
    }
    else{
      memset(data_gas_array_char[lees_uur_int], 0, sizeof(data_gas_array_char[lees_uur_int]));
    }
  }
  totaal_string = "totaal";
  totaal_string.toCharArray(totaal_char, totaal_string.length() + 1);
  strcpy(periode_array_char[24], totaal_char);
  elektriciteit_dag_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/E" + data_dag_int;
  elektriciteit_dag_file_string.toCharArray(elektriciteit_dag_file_char, elektriciteit_dag_file_string.length() + 1);
  file_test = false;
  testFile(SD, elektriciteit_dag_file_char);
  if((file_test == true) && (eind_uur_int == 24)){
    read_elektriciteit_verbruik_char(SD, elektriciteit_dag_file_char);
    strcpy(data_kwh_array_char[24], kwh_sd_char);
  }
  else{
    memset(data_kwh_array_char[24], 0, sizeof(data_kwh_array_char[24]));
  }
  injectie_dag_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/I" + data_dag_int;
  injectie_dag_file_string.toCharArray(injectie_dag_file_char, injectie_dag_file_string.length() + 1);
  file_test = false;
  testFile(SD, injectie_dag_file_char);
  if((file_test == true) && (eind_uur_int == 24)){
    read_injectie_char(SD, injectie_dag_file_char);
    strcpy(data_injectie_array_char[24], injectie_sd_char);
  }
  else{
    memset(data_injectie_array_char[24], 0, sizeof(data_injectie_array_char[24]));
  }
  gas_dag_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/G" + data_dag_int;
  gas_dag_file_string.toCharArray(gas_dag_file_char, gas_dag_file_string.length() + 1);
  file_test = false;
  testFile(SD, gas_dag_file_char);
  if((file_test == true) && (eind_uur_int == 24)){
    read_gas_verbruik_char(SD, gas_dag_file_char);
    strcpy(data_gas_array_char[24], gas_sd_char);
  }
  else{
    memset(data_gas_array_char[24], 0, sizeof(data_gas_array_char[24]));
  }
  for(int x = eind_uur_int + 1; x < 32; x++){
    memset(periode_array_char[x], 0, sizeof(periode_array_char[x]));
    memset(data_kwh_array_char[x], 0, sizeof(data_kwh_array_char[x]));
    memset(data_injectie_array_char[x], 0, sizeof(data_injectie_array_char[x]));
    memset(data_gas_array_char[x], 0, sizeof(data_gas_array_char[x]));
  }
}

void lees_dag_verbruik_injectie(){
  bool volledige_maand_bool = true;
  if((data_jaar_int == jaar_int) && (data_maand_int == maand_int)){
    volledige_maand_bool = false;
  }
  eenheid_string = "dag";
  eenheid_string.toCharArray(eenheid_char, eenheid_string.length() + 1);
  aantal_dagen_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/dagen";
  aantal_dagen_file_string.toCharArray(aantal_dagen_file_char, aantal_dagen_file_string.length() + 1);
  file_test = false;
  testFile(SD, aantal_dagen_file_char);
  if(file_test == true){
    read_dagen(SD, aantal_dagen_file_char);
    if(volledige_maand_bool == false){
      dagen_int = dag_int - 1;
    }
    for(int lees_dag_int = 0; lees_dag_int < dagen_int; lees_dag_int++){
      sprintf(periode_array_char[lees_dag_int], "%02d", lees_dag_int + 1);
      elektriciteit_dag_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/E" + (lees_dag_int + 1);
      elektriciteit_dag_file_string.toCharArray(elektriciteit_dag_file_char, elektriciteit_dag_file_string.length() + 1);
      file_test = false;
      testFile(SD, elektriciteit_dag_file_char);
      if(file_test == true){
        read_elektriciteit_verbruik_char(SD, elektriciteit_dag_file_char);
        strcpy(data_kwh_array_char[lees_dag_int], kwh_sd_char);
      }
      else{
        memset(data_kwh_array_char[lees_dag_int], 0, sizeof(data_kwh_array_char[lees_dag_int]));
      }
      injectie_dag_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/I" + (lees_dag_int + 1);
      injectie_dag_file_string.toCharArray(injectie_dag_file_char, injectie_dag_file_string.length() + 1);
      file_test = false;
      testFile(SD, injectie_dag_file_char);
      if(file_test == true){
        read_injectie_char(SD, injectie_dag_file_char);
        strcpy(data_injectie_array_char[lees_dag_int], injectie_sd_char);
      }
      else{
        memset(data_injectie_array_char[lees_dag_int], 0, sizeof(data_injectie_array_char[lees_dag_int]));
      }
      gas_dag_file_string = "/F" + String(data_jaar_int) + "/F" + String(data_maand_int) + "/G" + (lees_dag_int + 1);
      gas_dag_file_string.toCharArray(gas_dag_file_char, gas_dag_file_string.length() + 1);
      file_test = false;
      testFile(SD, gas_dag_file_char);
      if(file_test == true){
        read_gas_verbruik_char(SD, gas_dag_file_char);
        strcpy(data_gas_array_char[lees_dag_int], gas_sd_char);
      }
      else{
        memset(data_gas_array_char[lees_dag_int], 0, sizeof(data_gas_array_char[lees_dag_int]));
      }
    }
    if(volledige_maand_bool == true){
      totaal_string = "totaal";
      totaal_string.toCharArray(totaal_char, totaal_string.length() + 1);
      strcpy(periode_array_char[dagen_int], totaal_char);
      elektriciteit_maand_file_string = "/F" + String(data_jaar_int) + "/E" + data_maand_int;
      elektriciteit_maand_file_string.toCharArray(elektriciteit_maand_file_char, elektriciteit_maand_file_string.length() + 1);
      file_test = false;
      testFile(SD, elektriciteit_maand_file_char);
      if((file_test == true) && (volledige_maand_bool == true)){
        read_elektriciteit_verbruik_char(SD, elektriciteit_maand_file_char);
        strcpy(data_kwh_array_char[dagen_int], kwh_sd_char);
      }
      else{
        memset(data_kwh_array_char[dagen_int], 0, sizeof(data_kwh_array_char[dagen_int]));
      }
      injectie_maand_file_string = "/F" + String(data_jaar_int) + "/I" + data_maand_int;
      injectie_maand_file_string.toCharArray(injectie_maand_file_char, injectie_maand_file_string.length() + 1);
      file_test = false;
      testFile(SD, injectie_maand_file_char);
      if((file_test == true) && (volledige_maand_bool == true)){
        read_injectie_char(SD, injectie_maand_file_char);
        strcpy(data_injectie_array_char[dagen_int], injectie_sd_char);
      }
      else{
        memset(data_injectie_array_char[dagen_int], 0, sizeof(data_injectie_array_char[dagen_int]));
      }
      gas_maand_file_string = "/F" + String(data_jaar_int) + "/G" + data_maand_int;
      gas_maand_file_string.toCharArray(gas_maand_file_char, gas_maand_file_string.length() + 1);
      file_test = false;
      testFile(SD, gas_maand_file_char);
      if((file_test == true) && (volledige_maand_bool == true)){
        read_gas_verbruik_char(SD, gas_maand_file_char);
        strcpy(data_gas_array_char[dagen_int], gas_sd_char);
      }
      else{
        memset(data_gas_array_char[dagen_int], 0, sizeof(data_gas_array_char[dagen_int]));
      }
    }
    for(int x = dagen_int + 1; x < 32; x++){
      memset(periode_array_char[x],  0, sizeof(periode_array_char[x]));
      memset(data_kwh_array_char[x], 0, sizeof(data_kwh_array_char[x]));
      memset(data_injectie_array_char[x], 0, sizeof(data_injectie_array_char[x]));
      memset(data_gas_array_char[x], 0, sizeof(data_gas_array_char[x]));
    }
  }
  else{
    for(int x = 0; x < 32; x++){
      sprintf(periode_array_char[x], "%02d", (x + 1));
      memset(data_kwh_array_char[x], 0, sizeof(data_kwh_array_char[x]));
      memset(data_injectie_array_char[x], 0, sizeof(data_injectie_array_char[x]));
      memset(data_gas_array_char[x], 0, sizeof(data_gas_array_char[x]));
    }
  }
}

void lees_maand_verbruik_injectie(){
  for(int x = 0; x < 32; x++){
    memset(periode_array_char[x],  0, sizeof(periode_array_char[x]));
    memset(data_kwh_array_char[x], 0, sizeof(data_kwh_array_char[x]));
    memset(data_injectie_array_char[x], 0, sizeof(data_injectie_array_char[x]));
    memset(data_gas_array_char[x], 0, sizeof(data_gas_array_char[x]));
  }
  int eind_maand_int;
  if(data_jaar_int == jaar_int){
    eind_maand_int = maand_int - 1;
  }
  else{
    eind_maand_int = 12;
  }
  eenheid_string = "maand";
  eenheid_string.toCharArray(eenheid_char, eenheid_string.length() + 1);
  for(int lees_maand_int = 0; lees_maand_int < eind_maand_int; lees_maand_int++){
    sprintf(periode_array_char[lees_maand_int], "%02d", lees_maand_int + 1);
    sprintf(periode_array_char[lees_maand_int + 14], "%02d", lees_maand_int + 1);
    elektriciteit_maand_file_string = "/F" + String(data_jaar_int) + "/E" + (lees_maand_int + 1);
    elektriciteit_maand_file_string.toCharArray(elektriciteit_maand_file_char, elektriciteit_maand_file_string.length() + 1);
    file_test = false;
    testFile(SD, elektriciteit_maand_file_char);
    if(file_test == true){
      read_elektriciteit_verbruik_char(SD, elektriciteit_maand_file_char);
      strcpy(data_kwh_array_char[lees_maand_int], kwh_sd_char);
    }
    injectie_maand_file_string = "/F" + String(data_jaar_int) + "/I" + (lees_maand_int + 1);
    injectie_maand_file_string.toCharArray(injectie_maand_file_char, injectie_maand_file_string.length() + 1);
    file_test = false;
    testFile(SD, injectie_maand_file_char);
    if(file_test == true){
      read_injectie_char(SD, injectie_maand_file_char);
      strcpy(data_injectie_array_char[lees_maand_int], injectie_sd_char);
    }
    piek_maand_file_string = "/F" + String(data_jaar_int) + "/P" + (lees_maand_int + 1);
    piek_maand_file_string.toCharArray(piek_maand_file_char, piek_maand_file_string.length() + 1);
    file_test = false;
    testFile(SD, piek_maand_file_char);
    if(file_test == true){
      read_piek_char(SD, piek_maand_file_char);
      strcpy(data_kwh_array_char[14 + lees_maand_int], piek_sd_char);
    }
    gas_maand_file_string = "/F" + String(data_jaar_int) + "/G" + (lees_maand_int + 1);
    gas_maand_file_string.toCharArray(gas_maand_file_char, gas_maand_file_string.length() + 1);
    file_test = false;
    testFile(SD, gas_maand_file_char);
    if(file_test == true){
      read_gas_verbruik_char(SD, gas_maand_file_char);
      strcpy(data_gas_array_char[lees_maand_int], gas_sd_char);
    }
  }
  totaal_string = "totaal";
  totaal_string.toCharArray(totaal_char, totaal_string.length() + 1);
  strcpy(periode_array_char[12], totaal_char);
  elektriciteit_jaar_file_string = "/F" + String(data_jaar_int) + "/E" + data_jaar_int;
  elektriciteit_jaar_file_string.toCharArray(elektriciteit_jaar_file_char, elektriciteit_jaar_file_string.length() + 1);
  file_test = false;
  testFile(SD, elektriciteit_jaar_file_char);
  if((file_test == true) && (eind_maand_int == 13)){
    read_elektriciteit_verbruik_char(SD, elektriciteit_jaar_file_char);
    strcpy(data_kwh_array_char[12], kwh_sd_char);
  }
  injectie_jaar_file_string = "/F" + String(data_jaar_int) + "/I" + data_jaar_int;
  injectie_jaar_file_string.toCharArray(injectie_jaar_file_char, injectie_jaar_file_string.length() + 1);
  file_test = false;
  testFile(SD, injectie_jaar_file_char);
  if((file_test == true) && (eind_maand_int == 13)){
    read_injectie_char(SD, injectie_jaar_file_char);
    strcpy(data_injectie_array_char[12], injectie_sd_char);
  }
  gas_jaar_file_string = "/F" + String(data_jaar_int) + "/G" + data_jaar_int;
  gas_jaar_file_string.toCharArray(gas_jaar_file_char, gas_jaar_file_string.length() + 1);
  file_test = false;
  testFile(SD, gas_jaar_file_char);
  if((file_test == true) && (eind_maand_int == 13)){
    read_gas_verbruik_char(SD, gas_jaar_file_char);
    strcpy(data_gas_array_char[12], gas_sd_char);
  }
}

void lees_jaar_verbruik_injectie(){
  int lees_jaar = jaar_int - 25;
  int x = 0;
  eenheid_string = "jaar";
  eenheid_string.toCharArray(eenheid_char, eenheid_string.length() + 1);
  for(int lees_jaar_int = lees_jaar; lees_jaar_int < jaar_int; lees_jaar_int++){
    sprintf(periode_array_char[x], "%04d", lees_jaar_int);
    elektriciteit_jaar_file_string = "/F" + String(lees_jaar_int) + "/E" + lees_jaar_int;
    elektriciteit_jaar_file_string.toCharArray(elektriciteit_jaar_file_char, elektriciteit_jaar_file_string.length() + 1);
    file_test = false;
    testFile(SD, elektriciteit_jaar_file_char);
    if(file_test == true){
      read_elektriciteit_verbruik_char(SD, elektriciteit_jaar_file_char);
      strcpy(data_kwh_array_char[x], kwh_sd_char);
    }
    else{
      memset(data_kwh_array_char[x], 0, sizeof(data_kwh_array_char[lees_jaar_int]));
    }
    injectie_jaar_file_string = "/F" + String(lees_jaar_int) + "/I" + lees_jaar_int;
    injectie_jaar_file_string.toCharArray(injectie_jaar_file_char, injectie_jaar_file_string.length() + 1);
    file_test = false;
    testFile(SD, injectie_jaar_file_char);
    if(file_test == true){
      read_injectie_char(SD, injectie_jaar_file_char);
      strcpy(data_injectie_array_char[x], injectie_sd_char);
    }
    else{
      memset(data_injectie_array_char[x], 0, sizeof(data_injectie_array_char[lees_jaar_int]));
    }
    gas_jaar_file_string = "/F" + String(lees_jaar_int) + "/G" + lees_jaar_int;
    gas_jaar_file_string.toCharArray(gas_jaar_file_char, gas_jaar_file_string.length() + 1);
    file_test = false;
    testFile(SD, gas_jaar_file_char);
    if(file_test == true){
      read_gas_verbruik_char(SD, gas_jaar_file_char);
      strcpy(data_gas_array_char[x], gas_sd_char);
    }
    else{
      memset(data_gas_array_char[x], 0, sizeof(data_gas_array_char[lees_jaar_int]));
    }
    x++;
  }
  for(int x = 25; x < 32; x++){
    memset(periode_array_char[x],  0, sizeof(periode_array_char[x]));
    memset(data_kwh_array_char[x], 0, sizeof(data_kwh_array_char[x]));
    memset(data_injectie_array_char[x], 0, sizeof(data_injectie_array_char[x]));
    memset(data_gas_array_char[x], 0, sizeof(data_gas_array_char[x]));
  }
}

const char energie_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <iframe style="display:none" name="hidden-form"></iframe>
    <title>Energie Beheer</title>
    <meta name="viewport" content="width=device-width, initial-scale=0.85">
    <style>
      div.kader_0 {
        position: relative;
        width:    screen.width;
        left:     0px;
      }
      div.kader {
        position: relative;
        width:    400px;
        left:     0px;
        height:   12x;
      }
        div.links {
        position: absolute;
        left :    0px;
        width;    100px;
        height:   12px;
      }
      div.links_midden {
        position:absolute;
        left:     120px;
        width:    100px
        height:   12px; 
      }
      div.midden {
        position:absolute;
        left:     150px;
        width:    100px
        height:   12px; 
      }
      div.titel {
        height:   25px;
        width:    auto;
      }
      div.links_1 {
        position: absolute;
        left:     0px;
        width:    200px;
      }
      div.rechts_1 {
        position: absolute;
        left:     200px;
        width:    200px;
      }
      div.blanco_160 {
        width:    auto;
        height:   160px;
      }
      div.blanco_40 {
        width:    auto;
        height:   40px;
      }
    </style>
  </head>
  <body>
    <center>
      <div class="kader_0">
        <center>
          <h3><center> ESP32 Slimme Meter Interface </center></h3>
          <small>
            <div class="titel"><center><b>Verbruik gegevens</b></center></div>
            <div class="kader">
              <div class="links">Totaal electriciteit : </div>
              <div class="midden">%electriciteit_totaal% &nbsp; KWh</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">Totaal injectie : </div>
              <div class="midden">%injectie_totaal% &nbsp; KWh</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">Verbruik nu : </div>
              <div class="midden">%kwh_nu% &nbsp; KW</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">Injectie nu : </div>
              <div class="midden">%injectie_nu% &nbsp; KW</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">Maandpiek : </div>
              <div class="midden">%piek_nu% &nbsp; KW</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">Totaal gas : </div>
              <div class="midden">%gas_totaal% &nbsp; m3</div>
            </div>
            <br><br>
            <div class="titel"><center><b>Outputs</b></center></div>
            <div class="kader">
              <div class="links">Relais 1 : </div>
              <div class="links_midden">%relais1_sturing%</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">Relais 2 : </div>
              <div class="links_midden">%relais2_sturing%</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">PWM 1 sturing : </div>
              <div class="links_midden">%procent_1%</div>
            </div>
            <br>
            <div class="kader">
              <div class="links">PWM 2 sturing : </div>
              <div class="links_midden">%procent_2%</div>
            </div>
            <br><br>
            <div class="titel"><b><center>Datum</center></b></div>
            <center>
              <input type="text" style="text-align:center;" value="%dag%" size=1>
              &nbsp;<input type="text" style="text-align:center;" value="%maand%" size=1>
              &nbsp;<input type="text" style="text-align:center;" value="%jaar%" size=1>
            </center>
            <br>
            <div class="titel"><b><center>Tijd</center></b></div>
            <center><input type="text" style="text-align:center;" value="%tijd%" size=2></center>
            <div class="blanco_40">&nbsp;</div>
            <center>
              <div class="kader">
                <center>
                  <div class="links_1"><center><button type="button" onclick="data_weergave()">Data weergave</button></center></div>
                  <div class="rechts_1"><center><button type="button" onclick="instellingen()">Instellingen</button></center></div>
                </center>
              </div>
            </center>
          </small>
        </center>    
      </div>
    </center>
    <div class="blanco_160">&nbsp;</div>
    <center>
      <div class="kader_0">
        <div class="links_1">
         <h6><b>thieu-b55 november 2024</b></h6>
        </div>
      </div>
    </center>
    <script>
      function ok(){
        setTimeout(function(){document.location.reload();},250);
      }
      function data_weergave(){
        location.assign("http://192.168.4.1/data/");
      }
      function instellingen(){
        location.assign("http://192.168.4.1/instellingen/");
      }
    </script>
  </body>  
</html>
)rawliteral";

const char instellingen_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <iframe style="display:none" name="hidden-form"></iframe>
    <title>Instellingen</title>
    <meta name="viewport" content="width=device-width, initial-scale=0.85">
    <style>
      div.kader_0 {
        position: relative;
        left: 0px;
        width: screen.width;
      }
      div.kader {
        position: relative;
        width: 400px;
        height: 12x;
      }
        div.links{
        position: absolute;
        left : 0px;
        width; 100px;
        height: 12px;
      }
      div.links_midden{
        position:absolute;
        left:  120px;
        width: 100px
        height: 12px; 
      }
      div.midden{
        position:absolute;
        left: 150px;
        width: 100px
        height: 12px; 
      }
      div.titel{
        height: 25px;
        width: auto;
      }
    </style>
  </head>
  <body>
    <div class="kader_0">
      <center>
        <h3><center> ESP32 Slimme Meter Instellingen </center></h3>
        <small>
          <br>
          <form action="/get" target="hidden-form">
            <div class="titel"><b><center>Relais schakelwaarden</center></b></div>
            <center><input type= "text" style="text-align:center;" value="%relais_module%" size = 20></center>
            <br>
            <center>
              <b>KW:</b>&nbsp;<input type="text" style="text-align:center;" value="%kw_on%" name="input_kw_on" size=1>
              &nbsp; <b>Tijd:</b>&nbsp;<input type="text" style="text-align:center;" value="%schakel_tijd%" name="input_schakel_tijd" size=1>
              &nbsp; <b>A/M:</b>&nbsp;<input type="text" style="text-align:center;" value="%override%" name="input_override" size=1>
            </center>
            <br>
            <center>
              <input type="submit" name="relais_module_min" value="   -   " onclick="ok()">
              &nbsp;&nbsp;&nbsp;
              <input type="submit" name="relais_module_plus" value="   +   " onclick="ok()">
              &nbsp;&nbsp;&nbsp;
              <input type="submit" name="relais_module_bevestig" value="OK" onclick="ok()">
            </center>
          </form>
          <br><br>
          <form action="/get" target="hidden-form">
            <div class="titel"><center><b>PWM 1 sturing instellen</b></center></div>
            <center>
              <b>KW:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_1_kw%" name="input_pwm_1_kw" size=1>
              &nbsp;<b>1:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_1_tijd_on%" name="input_pwm_1_tijd_on" size=1>
              &nbsp;<b>0:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_1_tijd_off%" name="input_pwm_1_tijd_off" size=1>
              &nbsp;<b>A/M:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_1_override%" name="input_pwm_1_override" size=1>
            </center>
            <br>
            <center><input type="submit" name="bevestig_pwm_1" value="OK" onclick="ok()"></center>
          </form>
          <br><br>
          <form action="/get" target="hidden-form">
            <div class="titel"><center><b>PWM 2 sturing instellen</b></center></div>
            <center>
              <b>KW:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_2_kw%" name="input_pwm_2_kw" size=1>
              &nbsp;<b>TRSH:</b>&nbsp;<input type="text" style="text-align:center" value="%pwm_2_dr%" name="input_pwm_2_dr" size=1>
              &nbsp;<b>A/M:</b>&nbsp;<input type="text" style="text-align:center;" value="%pwm_2_override%" name="input_pwm_2_override" size=1>
            </center>
            <br>
            <center><input type="submit" name="bevestig_pwm_2" value="OK" onclick="ok()"></center>
          </form>
          <br><br>
          <form action="/get" target="hidden-form">
            <div class="titel"><center><b>Ingeven MAC address</b></center></div>
            <center>
              <input type= "text" style="text-align:center;" value="%module%" size = 20>
            </center>
            <br>
            <center>
              <input type="text" style="text-align:center;" value="%display_macx_0%" name="input_macx_0" size=1>
              &nbsp;
              <input type="text" style="text-align:center;" value="%display_macx_1%" name="input_macx_1" size=1>
              &nbsp;
              <input type="text" style="text-align:center;" value="%display_macx_2%" name="input_macx_2" size=1>
              &nbsp;
              <input type="text" style="text-align:center;" value="%display_macx_3%" name="input_macx_3" size=1>
              &nbsp;
              <input type="text" style="text-align:center;" value="%display_macx_4%" name="input_macx_4" size=1>
              &nbsp;
              <input type="text" style="text-align:center;" value="%display_macx_5%" name="input_macx_5" size=1>
            </center>
            <br>
            <center>
              <input type="submit" name="module_min" value="   -   " onclick="ok()">
              &nbsp;&nbsp;&nbsp;
              <input type="submit" name="module_plus" value="   +   " onclick="ok()">
              &nbsp;&nbsp;&nbsp;
              <input type="submit" name="module_bevestig" value="OK" onclick="ok()">
            </center>
          </form>
          <br><br><br>
          <center>
            <button type="button" onclick="start_pagina()">Naar begin pagina</button>
          </center>
        </small>
      </center>
    </div>              
    <script>
      function ok(){
        setTimeout(function(){document.location.reload();},250);
      }
    </script>
    <script>
      function start_pagina(){
        location.assign("http://192.168.4.1/");
      }
    </script>
  </body>  
</html>
)rawliteral";

const char data_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <iframe style="display:none" name="hidden-form"></iframe>
    <meta name="viewport" content="width=device-width, initial-scale=0.85">
    <title>Data weergave</title>
    <style>
        div.kader {
          position: relative;
          width: 400px;
          height: 12x;
        }
          div.links{
          position: absolute;
          left : 0px;
          width; auto;
          height: 12px;
        }
        div.links_midden{
          position:absolute;
          left:  80px;
          width: auto;
          height: 12px; 
        }
        div.rechts_midden{
          position:absolute;
          left: 200px;
          width: auto;
          height: 12px; 
        }
        div.rechts{
          position:absolute;
          left: 320px;
          width: auto
          height: 12px;
        }
        div.titel{
          height: 25px;
          width: auto;
        }
        div.data_links{
          position: absolute;
          left:  0px;
          width: 80px;
          height: 12px
        }
        div.data_links_midden{
          position: absolute;
          left:  80px;
          width: 120px;
          height: 12px
        }
        div.data_rechts_midden{
          position: absolute;
          left: 200px;
          width: 120px;
          height: 12px
        }
        div.data_rechts{
          position: absolute;
          left: 320px;
          width: 80px;
          height: 12px
        }
        div.blanco_20{
          width: auto;
          height: 20px;
        }
        div.blanco_15{
          width: auto;
          height: 15px;
        }
    </style>
  </head>
  <body>
    <h3><center> Data weergave </center></h3>
    <center>
      <button type="button" onclick="start_pagina()">Naar begin pagina</button>
    </center>
    <br>
    <form action="/get" target="hidden-form">
    <center>
      <input type="text" style="text-align:center;" value="%data_dag%" name="input_data_dag" size=1>
       &nbsp;<input type="text" style="text-align:center;" value="%data_maand%" name="input_data_maand" size=1>
      &nbsp;<input type="text" style="text-align:center;" value="%data_jaar%" name="input_data_jaar" size=1>
    </center>
    <br>
    <center>
      <input type="submit" name="bevestig_periode" value="OK" onclick="ok()">
    </center>
    <br>
    </form>
    <small>
      <div class="titel"><center><b>Periode : %periode%</b></center></div>
      <div class="kader"><b>
        <div class="links">%eenheid% </div>
        <div class="links_midden">verbruik kWh</div>
        <div class="rechts_midden">injectie kWh</div>
        <div class="rechts">gas m3</div></b>
      </div>
      <div class="blanco_20">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_1%</div>
        <div class="data_links_midden">%verbruik_1%</div>
        <div class="data_rechts_midden">%injectie_1%</div>
        <div class="data_rechts">%gas_verbruik_1%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_2%</div>
        <div class="data_links_midden">%verbruik_2%</div>
        <div class="data_rechts_midden">%injectie_2%</div>
        <div class="data_rechts">%gas_verbruik_2%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_3%</div>
        <div class="data_links_midden">%verbruik_3%</div>
        <div class="data_rechts_midden">%injectie_3%</div>
        <div class="data_rechts">%gas_verbruik_3%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_4%</div>
        <div class="data_links_midden">%verbruik_4%</div>
        <div class="data_rechts_midden">%injectie_4%</div>
        <div class="data_rechts">%gas_verbruik_4%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_5%</div>
        <div class="data_links_midden">%verbruik_5%</div>
        <div class="data_rechts_midden">%injectie_5%</div>
        <div class="data_rechts">%gas_verbruik_5%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_6%</div>
        <div class="data_links_midden">%verbruik_6%</div>
        <div class="data_rechts_midden">%injectie_6%</div>
        <div class="data_rechts">%gas_verbruik_6%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_7%</div>
        <div class="data_links_midden">%verbruik_7%</div>
        <div class="data_rechts_midden">%injectie_7%</div>
        <div class="data_rechts">%gas_verbruik_7%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_8%</div>
        <div class="data_links_midden">%verbruik_8%</div>
        <div class="data_rechts_midden">%injectie_8%</div>
        <div class="data_rechts">%gas_verbruik_8%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_9%</div>
        <div class="data_links_midden">%verbruik_9%</div>
        <div class="data_rechts_midden">%injectie_9%</div>
        <div class="data_rechts">%gas_verbruik_9%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_10%</div>
        <div class="data_links_midden">%verbruik_10%</div>
        <div class="data_rechts_midden">%injectie_10%</div>
        <div class="data_rechts">%gas_verbruik_10%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_11%</div>
        <div class="data_links_midden">%verbruik_11%</div>
        <div class="data_rechts_midden">%injectie_11%</div>
        <div class="data_rechts">%gas_verbruik_11%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_12%</div>
        <div class="data_links_midden">%verbruik_12%</div>
        <div class="data_rechts_midden">%injectie_12%</div>
        <div class="data_rechts">%gas_verbruik_12%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_13%</div>
        <div class="data_links_midden">%verbruik_13%</div>
        <div class="data_rechts_midden">%injectie_13%</div>
        <div class="data_rechts">%gas_verbruik_13%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader"><b><center>%maandpiek%</center></b></div> 
      <div class="kader">
        <div class="data_links">%periode_14%</div>
        <div class="data_links_midden">%verbruik_14%</div>
        <div class="data_rechts_midden">%injectie_14%</div>
        <div class="data_rechts">%gas_verbruik_14%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_15%</div>
        <div class="data_links_midden">%verbruik_15%</div>
        <div class="data_rechts_midden">%injectie_15%</div>
        <div class="data_rechts">%gas_verbruik_15%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_16%</div>
        <div class="data_links_midden">%verbruik_16%</div>
        <div class="data_rechts_midden">%injectie_16%</div>
        <div class="data_rechts">%gas_verbruik_16%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_17%</div>
        <div class="data_links_midden">%verbruik_17%</div>
        <div class="data_rechts_midden">%injectie_17%</div>
        <div class="data_rechts">%gas_verbruik_17%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_18%</div>
        <div class="data_links_midden">%verbruik_18%</div>
        <div class="data_rechts_midden">%injectie_18%</div>
        <div class="data_rechts">%gas_verbruik_18%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      <div class="kader">
        <div class="data_links">%periode_19%</div>
        <div class="data_links_midden">%verbruik_19%</div>
        <div class="data_rechts_midden">%injectie_19%</div>
        <div class="data_rechts">%gas_verbruik_19%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
       <div class="kader">
        <div class="data_links">%periode_20%</div>
        <div class="data_links_midden">%verbruik_20%</div>
        <div class="data_rechts_midden">%injectie_20%</div>
        <div class="data_rechts">%gas_verbruik_20%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_21%</div>
        <div class="data_links_midden">%verbruik_21%</div>
        <div class="data_rechts_midden">%injectie_21%</div>
        <div class="data_rechts">%gas_verbruik_21%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_22%</div>
        <div class="data_links_midden">%verbruik_22%</div>
        <div class="data_rechts_midden">%injectie_22%</div>
        <div class="data_rechts">%gas_verbruik_22%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_23%</div>
        <div class="data_links_midden">%verbruik_23%</div>
        <div class="data_rechts_midden">%injectie_23%</div>
        <div class="data_rechts">%gas_verbruik_23%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_24%</div>
        <div class="data_links_midden">%verbruik_24%</div>
        <div class="data_rechts_midden">%injectie_24%</div>
        <div class="data_rechts">%gas_verbruik_24%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_25%</div>
        <div class="data_links_midden">%verbruik_25%</div>
        <div class="data_rechts_midden">%injectie_25%</div>
        <div class="data_rechts">%gas_verbruik_25%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_26%</div>
        <div class="data_links_midden">%verbruik_26%</div>
        <div class="data_rechts_midden">%injectie_26%</div>
        <div class="data_rechts">%gas_verbruik_26%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_27%</div>
        <div class="data_links_midden">%verbruik_27%</div>
        <div class="data_rechts_midden">%injectie_27%</div>
        <div class="data_rechts">%gas_verbruik_27%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_28%</div>
        <div class="data_links_midden">%verbruik_28%</div>
        <div class="data_rechts_midden">%injectie_28%</div>
        <div class="data_rechts">%gas_verbruik_28%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_29%</div>
        <div class="data_links_midden">%verbruik_29%</div>
        <div class="data_rechts_midden">%injectie_29%</div>
        <div class="data_rechts">%gas_verbruik_29%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_30%</div>
        <div class="data_links_midden">%verbruik_30%</div>
        <div class="data_rechts_midden">%injectie_30%</div>
        <div class="data_rechts">%gas_verbruik_30%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_31%</div>
        <div class="data_links_midden">%verbruik_31%</div>
        <div class="data_rechts_midden">%injectie_31%</div>
        <div class="data_rechts">%gas_verbruik_31%</div>
      </div>
      <div class="blanco_15">&nbsp;</div>
      </div>
       <div class="kader">
        <div class="data_links">%periode_32%</div>
        <div class="data_links_midden">%verbruik_32%</div>
        <div class="data_rechts_midden">%injectie_32%</div>
        <div class="data_rechts">%gas_verbruik_32%</div>
      </div>
    </small>
    <script>
      function start_pagina(){
        location.assign("http://192.168.4.1/");
      }
      function ok(){
        setTimeout(function(){document.location.reload();},250);
      }
    </script>
  </body>  
</html>
)rawliteral";

String processor(const String& var){
  String temp = "                              ";
  String module = "                            ";
  int macx_0 = 0;
  int macx_1 = 0;
  int macx_2 = 0;
  int macx_3 = 0;
  int macx_4 = 0;
  int macx_5 = 0;
  if(var == "electriciteit_totaal"){
    temp = String(kwh_totaal_float, 3);
    temp.toCharArray(kwh_totaal_float_char, (temp.length() + 1));
    return(kwh_totaal_float_char);
  }
  if(var == "injectie_totaal"){
    temp = String(injectie_totaal_float, 3);
    temp.toCharArray(injectie_totaal_float_char, (temp.length() + 1));
    return(injectie_totaal_float_char);
  }
  if(var == "kwh_nu"){
    temp = String(kwh_nu_float, 3);
    temp.toCharArray(kwh_nu_float_char, (temp.length() + 1));
    return(kwh_nu_float_char);
  }
  if(var == "injectie_nu"){
    temp = String(injectie_nu_float, 3);
    temp.toCharArray(injectie_nu_float_char, (temp.length() + 1));
    return(injectie_nu_float_char);
  }
  if(var == "piek_nu"){
    temp = String(piek_nu_float, 3);
    temp.toCharArray(piek_nu_float_char, (temp.length() + 1));
    return(piek_nu_float_char);
  }
  if(var == "gas_totaal"){
    temp = String(gas_totaal_float, 3);
    temp.toCharArray(gas_totaal_float_char, (temp.length() + 1));
    return(gas_totaal_float_char);
  }
  if(var == "relais1_sturing"){
    if(relais1_uit == true){
      temp = "1";
    }
    if(relais1_uit == false){
      temp = "0";
    }
    if(relais1_override == "0"){
      temp = "0";
    }
    if(relais1_override == "1"){
      temp = "1";
    }
    temp.toCharArray(relais1_sturing_char, (temp.length() + 1));
    return(relais1_sturing_char);
  }
  if(var == "relais2_sturing"){
    if(relais2_uit == true){
      temp = "1";
    }
    if(relais2_uit == false){
      temp = "0";
    }
    if(relais2_override == "0"){
      temp = "0";
    }
    if(relais2_override == "1"){
      temp = "1";
    }
    temp.toCharArray(relais2_sturing_char, (temp.length() + 1));
    return(relais2_sturing_char);
  }
  if(var == "procent_1"){
    temp = String(uitsturing_pwm1_int);
    temp.toCharArray(uitsturing_pwm1_char, (temp.length() + 1));
    return(uitsturing_pwm1_char);
  }
  if(var == "procent_2"){
    temp = String(uitsturing_pwm2_int);
    temp.toCharArray(uitsturing_pwm2_char, (temp.length() + 1));
    return(uitsturing_pwm2_char);
  }
  if(var == "dag"){
    sprintf(dag_char, "%02d", dag_int);
    return(dag_char);
  }
  if(var == "maand"){
    sprintf(maand_char, "%02d", maand_int);
    return(maand_char); 
  }
  if(var == "jaar"){
    sprintf(jaar_char, "%04d", jaar_int);
    return(jaar_char);
  }
  if(var == "tijd"){
    sprintf(tijd_char, "%02d:%02d", uren_int, minuten_int);
    return(tijd_char);
  }
  if(var == "relais_module"){
    switch(relais_module_teller){
      case 0:
        temp = "Relais 1";
        break;
      case 1:
        temp = "Relais 2";
        break;
    }
    temp.toCharArray(relais_module_char, (temp.length() + 1));
    return(relais_module_char);
  }
  if(var == "kw_on"){
    switch(relais_module_teller){
      case 0:
        return(String(relais1_on));
        break;
      case 1:
        return(String(relais2_on));
        break;
    }
  }
  if(var == "override"){
    switch(relais_module_teller){
      case 0:
        temp = relais1_override;
        break;
      case 1:
        temp = relais2_override;
        break;
    }
    temp.toCharArray(override_char, (temp.length() + 1));
    return(override_char);
  }
  if(var == "schakel_tijd"){
    switch(relais_module_teller){
      case 0:
        sprintf(schakel_tijd_char, "%02d:%02d", uren_on1_int, minuten_on1_int);
        return(schakel_tijd_char);
        break;
      case 1:
        sprintf(schakel_tijd_char, "%02d:%02d", uren_on2_int, minuten_on2_int);
        return(schakel_tijd_char);
        break;
    }
  }
  if(var == "pwm_1_kw"){
    return(String(pwm1_kw_float));
  }
  if(var == "pwm_1_tijd_on"){
    sprintf(pwm1_tijd_on_char, "%02d:%02d", uren_on3_int, minuten_on3_int);
    return(pwm1_tijd_on_char);
  }
  if(var == "pwm_1_tijd_off"){
    sprintf(pwm1_tijd_off_char, "%02d:%02d", uren_off3_int, minuten_off3_int);
    return(pwm1_tijd_off_char);
  }
  if(var == "pwm_1_override"){
    pwm1_override.toCharArray(pwm1_override_char, (pwm1_override.length() + 1));
    return(pwm1_override_char);
  }
  if(var == "pwm_2_kw"){
    return(String(pwm2_kw_float));
  }
  if(var == "pwm_2_dr"){
    return(String(pwm2_dr_float));
  }
  if(var == "pwm_2_override"){
    pwm2_override.toCharArray(pwm2_override_char, (pwm2_override.length() + 1));
    return(pwm2_override_char);
  }
  switch(module_teller){
    case 0:
      module = "MAC address Display";
      macx_0 = broadcastAddress1[0];
      macx_1 = broadcastAddress1[1];
      macx_2 = broadcastAddress1[2];
      macx_3 = broadcastAddress1[3];
      macx_4 = broadcastAddress1[4];
      macx_5 = broadcastAddress1[5];
      break;
    case 1:
      module = "MAC address Relais 1";
      macx_0 = broadcastAddress2[0];
      macx_1 = broadcastAddress2[1];
      macx_2 = broadcastAddress2[2];
      macx_3 = broadcastAddress2[3];
      macx_4 = broadcastAddress2[4];
      macx_5 = broadcastAddress2[5];
      break;
    case 2:
      module = "MAC address Relais 2";
      macx_0 = broadcastAddress3[0];
      macx_1 = broadcastAddress3[1];
      macx_2 = broadcastAddress3[2];
      macx_3 = broadcastAddress3[3];
      macx_4 = broadcastAddress3[4];
      macx_5 = broadcastAddress3[5];
      break;
    case 3:
      module = "MAC address PWM 1 sturing";
      macx_0 = broadcastAddress4[0];
      macx_1 = broadcastAddress4[1];
      macx_2 = broadcastAddress4[2];
      macx_3 = broadcastAddress4[3];
      macx_4 = broadcastAddress4[4];
      macx_5 = broadcastAddress4[5];
      break;
    case 4:
      module = "MAC address PWM 2 Sturing";
      macx_0 = broadcastAddress5[0];
      macx_1 = broadcastAddress5[1];
      macx_2 = broadcastAddress5[2];
      macx_3 = broadcastAddress5[3];
      macx_4 = broadcastAddress5[4];
      macx_5 = broadcastAddress5[5];
  }
  if(var == "module"){
    module.toCharArray(module_char, (module.length() + 1));
    return(module_char);
  }
  if(var == "display_macx_0"){
    sprintf(broadcastAddressX_0_char, "%02x", macx_0);
    return(broadcastAddressX_0_char);
  }
  if(var == "display_macx_1"){
    sprintf(broadcastAddressX_1_char, "%02x", macx_1);
    return(broadcastAddressX_1_char);
  }
  if(var == "display_macx_2"){
    sprintf(broadcastAddressX_2_char, "%02x", macx_2);
    return(broadcastAddressX_2_char);;
  }
  if(var == "display_macx_3"){
    sprintf(broadcastAddressX_3_char, "%02x", macx_3);
    return(broadcastAddressX_3_char);
  }
  if(var == "display_macx_4"){
    sprintf(broadcastAddressX_4_char, "%02x", macx_4);
    return(broadcastAddressX_4_char);
  }
  if(var == "display_macx_5"){
    sprintf(broadcastAddressX_5_char, "%02x", macx_5);
    return(broadcastAddressX_5_char);
  }
  if(var == "data_dag"){
    sprintf(data_dag_char, "%02d", data_dag_int);
    return(data_dag_char);
  }
  if(var == "data_maand"){
    sprintf(data_maand_char, "%02d", data_maand_int);
    return(data_maand_char);
  }
  if(var == "data_jaar"){
    sprintf(data_jaar_char, "%04d", data_jaar_int);
    return(data_jaar_char);
  }
  if(var == "periode"){
    return(periode_char);
  }
  if(var == "eenheid"){
    return(eenheid_char);
  }
  if(var == "periode_1"){
    return(periode_array_char[0]);
  } 
  if(var == "periode_2"){
    return(periode_array_char[1]);
  }
  if(var == "periode_3"){
    return(periode_array_char[2]);
  }
  if(var == "periode_4"){
    return(periode_array_char[3]);
  }
  if(var == "periode_5"){
    return(periode_array_char[4]);
  }
  if(var == "periode_6"){
    return(periode_array_char[5]);
  }
  if(var == "periode_7"){
    return(periode_array_char[6]);
  }
  if(var == "periode_8"){
    return(periode_array_char[7]);
  }
  if(var == "periode_9"){
    return(periode_array_char[8]);
  }
  if(var == "periode_10"){
    return(periode_array_char[9]);
  }
  if(var == "periode_11"){
    return(periode_array_char[10]);
  }
  if(var == "periode_12"){
    return(periode_array_char[11]);
  }
  if(var == "periode_13"){
    return(periode_array_char[12]);
  }
  if(var == "maandpiek"){
    if((data_maand_int == 0) && (data_jaar_int != 0)){
      return("maandpiek KW");
    }
    else{
      return("");
    }
  }
  if(var == "periode_14"){
    return(periode_array_char[13]);
  }
  if(var == "periode_15"){
    return(periode_array_char[14]);
  }
  if(var == "periode_16"){
    return(periode_array_char[15]);
  }
  if(var == "periode_17"){
    return(periode_array_char[16]);
  }
  if(var == "periode_18"){
    return(periode_array_char[17]);
  }
  if(var == "periode_19"){
    return(periode_array_char[18]);
  }
  if(var == "periode_20"){
    return(periode_array_char[19]);
  }
  if(var == "periode_21"){
    return(periode_array_char[20]);
  }
  if(var == "periode_22"){
    return(periode_array_char[21]);
  }
  if(var == "periode_23"){
    return(periode_array_char[22]);
  }
  if(var == "periode_24"){
    return(periode_array_char[23]);
  }
  if(var == "periode_25"){
    return(periode_array_char[24]);
  }
  if(var == "periode_26"){
    return(periode_array_char[25]);
  }
  if(var == "periode_27"){
    return(periode_array_char[26]);
  }
  if(var == "periode_28"){
    return(periode_array_char[27]);
  }
  if(var == "periode_29"){
    return(periode_array_char[28]);
  }
  if(var == "periode_30"){
    return(periode_array_char[29]);
  }
  if(var == "periode_31"){
    return(periode_array_char[30]);
  }
  if(var == "periode_32"){
    return(periode_array_char[31]);
  }
  if(var == "verbruik_1"){
    return(data_kwh_array_char[0]);
  } 
  if(var == "verbruik_2"){
    return(data_kwh_array_char[1]);
  }
  if(var == "verbruik_3"){
    return(data_kwh_array_char[2]);
  }
  if(var == "verbruik_4"){
    return(data_kwh_array_char[3]);
  }
  if(var == "verbruik_5"){
    return(data_kwh_array_char[4]);
  }
  if(var == "verbruik_6"){
    return(data_kwh_array_char[5]);
  }
  if(var == "verbruik_7"){
    return(data_kwh_array_char[6]);
  }
  if(var == "verbruik_8"){
    return(data_kwh_array_char[7]);
  }
  if(var == "verbruik_9"){
    return(data_kwh_array_char[8]); 
  }
  if(var == "verbruik_10"){
    return(data_kwh_array_char[9]);
  }
  if(var == "verbruik_11"){
    return(data_kwh_array_char[10]);
  }
  if(var == "verbruik_12"){
    return(data_kwh_array_char[11]);
  }
  if(var == "verbruik_13"){
    return(data_kwh_array_char[12]);
  }
  if(var == "verbruik_14"){
    return(data_kwh_array_char[13]);
  }
  if(var == "verbruik_15"){
    return(data_kwh_array_char[14]);
  }
  if(var == "verbruik_16"){
    return(data_kwh_array_char[15]);
  }
  if(var == "verbruik_17"){
    return(data_kwh_array_char[16]);
  }
  if(var == "verbruik_18"){
    return(data_kwh_array_char[17]);
  }
  if(var == "verbruik_19"){
    return(data_kwh_array_char[18]);
  }
  if(var == "verbruik_20"){
    return(data_kwh_array_char[19]);
  }
  if(var == "verbruik_21"){
    return(data_kwh_array_char[20]);
  }
  if(var == "verbruik_22"){
    return(data_kwh_array_char[21]);
  }
  if(var == "verbruik_23"){
    return(data_kwh_array_char[22]); 
  }
  if(var == "verbruik_24"){
    return(data_kwh_array_char[23]); 
  }
  if(var == "verbruik_25"){
    return(data_kwh_array_char[24]); 
  }
  if(var == "verbruik_26"){
    return(data_kwh_array_char[25]); 
  }
  if(var == "verbruik_27"){
    return(data_kwh_array_char[26]); 
  }
  if(var == "verbruik_28"){
    return(data_kwh_array_char[27]); 
  }
  if(var == "verbruik_29"){
    return(data_kwh_array_char[28]); 
  }
  if(var == "verbruik_30"){
    return(data_kwh_array_char[29]); 
  }
  if(var == "verbruik_31"){
    return(data_kwh_array_char[30]); 
  }
  if(var == "verbruik_32"){
    return(data_kwh_array_char[31]); 
  }
  if(var == "injectie_1"){
    return(data_injectie_array_char[0]);
  } 
  if(var == "injectie_2"){
    return(data_injectie_array_char[1]);
  }
  if(var == "injectie_3"){
    return(data_injectie_array_char[2]);
  }
  if(var == "injectie_4"){
    return(data_injectie_array_char[3]);
  }
  if(var == "injectie_5"){
    return(data_injectie_array_char[4]);
  }
  if(var == "injectie_6"){
    return(data_injectie_array_char[5]);
  }
  if(var == "injectie_7"){
    return(data_injectie_array_char[6]);
  }
  if(var == "injectie_8"){
    return(data_injectie_array_char[7]);
  }
  if(var == "injectie_9"){
    return(data_injectie_array_char[8]);
  }
  if(var == "injectie_10"){
    return(data_injectie_array_char[9]);
  }
  if(var == "injectie_11"){
    return(data_injectie_array_char[10]);
  }
  if(var == "injectie_12"){
    return(data_injectie_array_char[11]);
  }
  if(var == "injectie_13"){
    return(data_injectie_array_char[12]);
  }
  if(var == "injectie_14"){
    return(data_injectie_array_char[13]);
  }
  if(var == "injectie_15"){
    return(data_injectie_array_char[14]);
  }
  if(var == "injectie_16"){
    return(data_injectie_array_char[15]);
  }
  if(var == "injectie_17"){
    return(data_injectie_array_char[16]);
  }
  if(var == "injectie_18"){
    return(data_injectie_array_char[17]);
  }
  if(var == "injectie_19"){
    return(data_injectie_array_char[18]);
  }
  if(var == "injectie_20"){
    return(data_injectie_array_char[19]);
  }
  if(var == "injectie_21"){
    return(data_injectie_array_char[20]);
  }
  if(var == "injectie_22"){
    return(data_injectie_array_char[21]);
  }
  if(var == "injectie_23"){
    return(data_injectie_array_char[22]);
  }
  if(var == "injectie_24"){
    return(data_injectie_array_char[23]);
  }
  if(var == "injectie_25"){
    return(data_injectie_array_char[24]);
  }
  if(var == "injectie_26"){
    return(data_injectie_array_char[25]);
  }
  if(var == "injectie_27"){
    return(data_injectie_array_char[26]);
  }
  if(var == "injectie_28"){
    return(data_injectie_array_char[27]);
  }
  if(var == "injectie_29"){
    return(data_injectie_array_char[28]);
  }
  if(var == "injectie_30"){
    return(data_injectie_array_char[29]);
  }
  if(var == "injectie_31"){
    return(data_injectie_array_char[30]);
  }
  if(var == "injectie_32"){
    return(data_injectie_array_char[31]);
  }
  if(var == "gas_verbruik_1"){
    return(data_gas_array_char[0]);
  } 
  if(var == "gas_verbruik_2"){
    return(data_gas_array_char[1]);
  }
  if(var == "gas_verbruik_3"){
    return(data_gas_array_char[2]);
  }
  if(var == "gas_verbruik_4"){
    return(data_gas_array_char[3]);
  }
  if(var == "gas_verbruik_5"){
    return(data_gas_array_char[4]);
  }
  if(var == "gas_verbruik_6"){
    return(data_gas_array_char[5]);
  }
  if(var == "gas_verbruik_7"){
    return(data_gas_array_char[6]);
  }
  if(var == "gas_verbruik_8"){
    return(data_gas_array_char[7]);
  }
  if(var == "gas_verbruik_9"){
    return(data_gas_array_char[8]);
  }
  if(var == "gas_verbruik_10"){
    return(data_gas_array_char[9]);
  }
  if(var == "gas_verbruik_11"){
    return(data_gas_array_char[10]);
  }
  if(var == "gas_verbruik_12"){
    return(data_gas_array_char[11]);
  }
  if(var == "gas_verbruik_13"){
    return(data_gas_array_char[12]);
  }
  if(var == "gas_verbruik_14"){
    return(data_gas_array_char[13]);
  }
  if(var == "gas_verbruik_15"){
    return(data_gas_array_char[14]);
  }
  if(var == "gas_verbruik_16"){
    return(data_gas_array_char[15]);
  }
  if(var == "gas_verbruik_17"){
    return(data_gas_array_char[16]);
  }
  if(var == "gas_verbruik_18"){
    return(data_gas_array_char[17]);
  }
  if(var == "gas_verbruik_19"){
    return(data_gas_array_char[18]);
  }
  if(var == "gas_verbruik_20"){
    return(data_gas_array_char[19]);
  }
  if(var == "gas_verbruik_21"){
    return(data_gas_array_char[20]);
  }
  if(var == "gas_verbruik_22"){
    return(data_gas_array_char[21]);
  }
  if(var == "gas_verbruik_23"){
    return(data_gas_array_char[22]);
  }
  if(var == "gas_verbruik_24"){
    return(data_gas_array_char[23]);
  }
  if(var == "gas_verbruik_25"){
    return(data_gas_array_char[24]);
  }
  if(var == "gas_verbruik_26"){
    return(data_gas_array_char[25]);
  }
  if(var == "gas_verbruik_27"){
    return(data_gas_array_char[26]);
  }
  if(var == "gas_verbruik_28"){
    return(data_gas_array_char[27]);
  }
  if(var == "gas_verbruik_29"){
    return(data_gas_array_char[28]);
  }
  if(var == "gas_verbruik_30"){
    return(data_gas_array_char[29]);
  }
  if(var == "gas_verbruik_31"){
    return(data_gas_array_char[30]);
  }
  if(var == "gas_verbruik_32"){
    return(data_gas_array_char[31]);
  }
  return("0");
}

void html_input(){
  server.begin();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", energie_html, processor);
  });
  server.on("/instellingen", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", instellingen_html, processor);
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", data_html, processor);
  });
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    char terminator = char(0x0a);
    String temp = "                        ";
    String uren_string = "        ";
    String minuten_string = "        ";
    char temp_char[30];
    float kw_on = 0.0;
    String override = "           ";
    char char_temp[10];
    bool fout;
    int uren_int;
    int minuten_int;
    
    if(request->hasParam(INPUT_KW_ON)){
      temp = ((request->getParam(INPUT_KW_ON)->value()) + String(terminator));
      temp.replace(',', '.');
      kw_on = temp.toFloat();
    }
    if(request->hasParam(INPUT_SCHAKEL_TIJD)){
      temp = ((request->getParam(INPUT_SCHAKEL_TIJD)->value()) + String(terminator));
      if(temp.length() == 6){
        uren_string = temp.substring(0, 2);
        minuten_string = temp.substring(3,5);
        uren_int = uren_string.toInt();
        minuten_int = minuten_string.toInt();
        if((uren_int >= 0) && (uren_int <= 24)){
          if((minuten_int >= 0) && (minuten_int <= 59)){
            switch(relais_module_teller){
              case 0:
                uren_on1_int = uren_int;
                minuten_on1_int = minuten_int;
                pref.putInt("uren_on1", uren_int);
                pref.putInt("minuten_on1", minuten_int);
                break;
              case 1:
                uren_on2_int = uren_int;
                minuten_on2_int = minuten_int;
                pref.putInt("uren_on1", uren_int);
                pref.putInt("minuten_on1", minuten_int);
                break;
            }
          }
        }
      }
    }
    if(request->hasParam(INPUT_OVERRIDE)){
      override = (request->getParam(INPUT_OVERRIDE)->value());
      override.toCharArray(char_temp, (override.length() + 1));
      switch(int(char_temp[0])){
        case 48:          //0
          break;
        case 49:          //1
          break;
        case 97:          //a
          override = "A";
          break;
        case 65:          //A
          break;
        default:
          override = "0";
      }
    }
    if(request->hasParam(RELAIS_MODULE_MIN)){
      relais_module_teller --;
      if(relais_module_teller < 0){
        relais_module_teller = 1;
      }
    }
    if(request->hasParam(RELAIS_MODULE_PLUS)){
      relais_module_teller ++;
      if(relais_module_teller > 1){
        relais_module_teller = 0;
      }
    }
    if(request->hasParam(RELAIS_MODULE_BEVESTIG)){
      switch(relais_module_teller){
        case 0:
          pref.putFloat("relais1_on", kw_on);
          pref.putString("relais1_ov", override);
          relais1_on = pref.getFloat("relais1_on");
          relais1_override = pref.getString("relais1_ov");
          if(relais1_override == "1"){
            relais1_uit = true;
            uitsturen.relais = true;
            result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 1");
            }
            else {
              Serial.println("fout bij verzenden naar relais 1");
            }
          }
          else{
            relais1_uit = false;
            uitsturen.relais = false;
            result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 1");
            }
            else {
              Serial.println("fout bij verzenden naar relais 1");
            }
          }
          break;
        case 1:
          pref.putFloat("relais2_on", kw_on);
          pref.putString("relais2_ov", override);
          relais2_on = pref.getFloat("relais2_on");
          relais2_override = pref.getString("relais2_ov");
          if(relais2_override == "1"){
            relais2_uit = true;
            uitsturen.relais = true;
            result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 2");
            }
            else {
              Serial.println("fout bij verzenden naar relais 2");
            }
          }
          else{
            relais2_uit = false;
            uitsturen.relais = false;
            result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              Serial.println("Met succes verzonden relais 2");
            }
            else {
              Serial.println("fout bij verzenden naar relais 2");
            }
          }
          break;
      }
    }
    if(request->hasParam(INPUT_PWM_1_KW)){
      temp = ((request->getParam(INPUT_PWM_1_KW)->value()) + String(terminator));
      temp.replace(',', '.');
      pwm1_kw_float = temp.toFloat();
    }
    if(request->hasParam(INPUT_PWM_1_TIJD_ON)){
      temp = ((request->getParam(INPUT_PWM_1_TIJD_ON)->value()) + String(terminator));
      if(temp.length() == 6){
        uren_string = temp.substring(0, 2);
        minuten_string = temp.substring(3,5);
        uren_int = uren_string.toInt();
        minuten_int = minuten_string.toInt();
        if((uren_int >= 0) && (uren_int <= 24)){
          if((minuten_int >= 0) && (minuten_int <= 59)){
            uren_on3_int = uren_int;
            minuten_on3_int = minuten_int;
          }
        }
      }
    }
    if(request->hasParam(INPUT_PWM_1_TIJD_OFF)){
     temp = ((request->getParam(INPUT_PWM_1_TIJD_OFF)->value()) + String(terminator));
     if(temp.length() == 6){
       uren_string = temp.substring(0, 2);
       minuten_string = temp.substring(3,5);
       uren_int = uren_string.toInt();
       minuten_int = minuten_string.toInt();
       if((uren_int >= 0) && (uren_int <= 23)){
         if((minuten_int >= 0) && (minuten_int <= 59)){
           uren_off3_int = uren_int;
           minuten_off3_int = minuten_int;
         }
       }
     }
    }
    if(request->hasParam(INPUT_PWM_1_OVERRIDE)){
      pwm1_override = (request->getParam(INPUT_PWM_1_OVERRIDE)->value());
      pwm1_override.toCharArray(char_temp, (pwm1_override.length() + 1));
      switch(int(char_temp[0])){
        case 48:          //0
          break;
        case 49:          //1
          break;
        case 97:          //a
          pwm1_override = "A";
          break;
        case 65:          //A
          break;
        default:
          pwm1_override = "0";
      }
    }
    if(request->hasParam(BEVESTIG_PWM_1)){
      pref.putFloat("pwm1_kw", pwm1_kw_float);
      pref.putInt("uren_on3", uren_on3_int);
      pref.putInt("minuten_on3", minuten_on3_int);
      pref.putInt("uren_off3", uren_off3_int);
      pref.putInt("minuten_off3", minuten_off3_int);
      pref.putString("pwm1_override", pwm1_override);
      pwm1_kw_float = pref.getFloat("pwm1_kw");
      uren_on3_int = pref.getInt("uren_on3");
      minuten_on3_int = pref.getInt("minuten_on3");
      uren_off3_int = pref.getInt("uren_off3");
      minuten_off3_int = pref.getInt("minuten_off3");
      pwm1_override = pref.getString("pwm1_override");
      if(pwm1_override == "A"){
        uitsturing_pwm1_float = 0.0;
        uitsturing_pwm1_int = 0;
      }
      /*
       * tijdsturing uitchakelen bij Manueel 0 tijdens tijdsturing
       */
      if(pwm1_override == "0"){
        pwm1_tijd_gezet = false;
      }
    }
    if(request->hasParam(INPUT_PWM_2_KW)){
      temp = ((request->getParam(INPUT_PWM_2_KW)->value()) + String(terminator));
      temp.replace(',', '.');
      pwm2_kw_float = temp.toFloat();
    }
    if(request->hasParam(INPUT_PWM_2_DR)){
      temp = ((request->getParam(INPUT_PWM_2_DR)->value()) + String(terminator));
      temp.replace(',', '.');
      pwm2_dr_float = temp.toFloat();
    }
    if(request->hasParam(INPUT_PWM_2_OVERRIDE)){
      pwm2_override = (request->getParam(INPUT_PWM_2_OVERRIDE)->value());
      pwm2_override.toCharArray(char_temp, (pwm2_override.length() + 1));
      switch(int(char_temp[0])){
        case 48:          //0
          break;
        case 49:          //1
          break;
        case 97:          //a
          pwm2_override = "A";
          break;
        case 65:          //A
          break;
        default:
          pwm2_override = "0";
      }
    }
    if(request->hasParam(BEVESTIG_PWM_2)){
      pref.putFloat("pwm2_kw", pwm2_kw_float);
      pref.putFloat("pwm2_dr", pwm2_dr_float);
      pref.putString("pwm2_override", pwm2_override);
      pwm2_kw_float = pref.getFloat("pwm2_kw");
      pwm2_dr_float = pref.getFloat("pwm2_dr");
      pwm2_override = pref.getString("pwm2_override");
      if(pwm2_override == "A"){
        uitsturing_pwm2_float = 0.0;
        uitsturing_pwm2_int = 0;
      }
    }
    if(request->hasParam(INPUT_MACX_0)){
      temp = ((request->getParam(INPUT_MACX_0)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_0 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_1)){
      temp = ((request->getParam(INPUT_MACX_1)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_1 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_2)){
      temp = ((request->getParam(INPUT_MACX_2)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_2 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_3)){
      temp = ((request->getParam(INPUT_MACX_3)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_3 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_4)){
      temp = ((request->getParam(INPUT_MACX_4)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_4 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_5)){
      temp = ((request->getParam(INPUT_MACX_5)->value()) + String(terminator));
      temp.toCharArray(temp_char, (temp.length() +1));
      input_macx_5 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(MODULE_MIN)){
      module_teller --;
      if(module_teller < 0){
        module_teller = 4;
      }
    }
    if(request->hasParam(MODULE_PLUS)){
      module_teller ++;
      if(module_teller > 4){
        module_teller = 0;
      }
    }
    if(request->hasParam(MODULE_BEVESTIG)){
      temp = "";
      fout = false;
      temp = temp + String(input_macx_0)+ String(input_macx_1)+ String(input_macx_2)
                  + String(input_macx_3)+ String(input_macx_4)+ String(input_macx_5);
      if(broadcastAddress1_string == temp){
        fout = true;
      }
      if(broadcastAddress2_string == temp){
        fout = true;
      }
      if(broadcastAddress3_string == temp){
        fout = true;
      }
      if(broadcastAddress4_string == temp){
        fout = true;
      }
      if(broadcastAddress5_string == temp){
        fout = true;
      }
      if(fout == false){
        switch(module_teller){
          case 0:
            pref.putInt("mac1_0", input_macx_0);
            pref.putInt("mac1_1", input_macx_1);
            pref.putInt("mac1_2", input_macx_2);
            pref.putInt("mac1_3", input_macx_3);
            pref.putInt("mac1_4", input_macx_4);
            pref.putInt("mac1_5", input_macx_5);
            broadcastAddress1[0] = pref.getInt("mac1_0");
            broadcastAddress1[1] = pref.getInt("mac1_1");
            broadcastAddress1[2] = pref.getInt("mac1_2");
            broadcastAddress1[3] = pref.getInt("mac1_3");
            broadcastAddress1[4] = pref.getInt("mac1_4");
            broadcastAddress1[5] = pref.getInt("mac1_5");
            break;
          case 1:
            pref.putInt("mac2_0", input_macx_0);
            pref.putInt("mac2_1", input_macx_1);
            pref.putInt("mac2_2", input_macx_2);
            pref.putInt("mac2_3", input_macx_3);
            pref.putInt("mac2_4", input_macx_4);
            pref.putInt("mac2_5", input_macx_5);
            broadcastAddress2[0] = pref.getInt("mac2_0");
            broadcastAddress2[1] = pref.getInt("mac2_1");
            broadcastAddress2[2] = pref.getInt("mac2_2");
            broadcastAddress2[3] = pref.getInt("mac2_3");
            broadcastAddress2[4] = pref.getInt("mac2_4");
            broadcastAddress2[5] = pref.getInt("mac2_5");
            break;
          case 2:
            pref.putInt("mac3_0", input_macx_0);
            pref.putInt("mac3_1", input_macx_1);
            pref.putInt("mac3_2", input_macx_2);
            pref.putInt("mac3_3", input_macx_3);
            pref.putInt("mac3_4", input_macx_4);
            pref.putInt("mac3_5", input_macx_5);
            broadcastAddress3[0] = pref.getInt("mac3_0");
            broadcastAddress3[1] = pref.getInt("mac3_1");
            broadcastAddress3[2] = pref.getInt("mac3_2");
            broadcastAddress1[3] = pref.getInt("mac3_3");
            broadcastAddress3[4] = pref.getInt("mac3_4");
            broadcastAddress3[5] = pref.getInt("mac3_5");
            break;
          case 3:
            pref.putInt("mac4_0", input_macx_0);
            pref.putInt("mac4_1", input_macx_1);
            pref.putInt("mac4_2", input_macx_2);
            pref.putInt("mac4_3", input_macx_3);
            pref.putInt("mac4_4", input_macx_4);
            pref.putInt("mac4_5", input_macx_5);
            broadcastAddress4[0] = pref.getInt("mac4_0");
            broadcastAddress4[1] = pref.getInt("mac4_1");
            broadcastAddress4[2] = pref.getInt("mac4_2");
            broadcastAddress4[3] = pref.getInt("mac4_3");
            broadcastAddress4[4] = pref.getInt("mac4_4");
            broadcastAddress4[5] = pref.getInt("mac4_5");
            break;
          case 4:
            pref.putInt("mac5_0", input_macx_0);
            pref.putInt("mac5_1", input_macx_1);
            pref.putInt("mac5_2", input_macx_2);
            pref.putInt("mac5_3", input_macx_3);
            pref.putInt("mac5_4", input_macx_4);
            pref.putInt("mac5_5", input_macx_5);
            broadcastAddress5[0] = pref.getInt("mac5_0");
            broadcastAddress5[1] = pref.getInt("mac5_1");
            broadcastAddress5[2] = pref.getInt("mac5_2");
            broadcastAddress5[3] = pref.getInt("mac5_3");
            broadcastAddress5[4] = pref.getInt("mac5_4");
            broadcastAddress5[5] = pref.getInt("mac5_5");
            break;
        }
        delay(2000);
        ESP.restart();
      }
    }
    if(request->hasParam(INPUT_DATA_DAG)){
        temp = ((request->getParam(INPUT_DATA_DAG)->value()) + String(terminator));
        data_dag_int = temp.toInt();
      }
      if(request->hasParam(INPUT_DATA_MAAND)){
        temp = ((request->getParam(INPUT_DATA_MAAND)->value()) + String(terminator));
        data_maand_int = temp.toInt();
      }
      if(request->hasParam(INPUT_DATA_JAAR)){
        temp = ((request->getParam(INPUT_DATA_JAAR)->value()) + String(terminator));
        data_jaar_int = temp.toInt();
      }
      if(request->hasParam(BEVESTIG_PERIODE)){
        for(int x = 0; x < 32; x++){
          memset(periode_array_char[x],  0, sizeof(periode_array_char[x]));
          memset(data_kwh_array_char[x], 0, sizeof(data_kwh_array_char[x]));
          memset(data_injectie_array_char[x], 0, sizeof(data_injectie_array_char[x]));
          memset(data_piek_array_char[x], 0, sizeof(data_piek_array_char[x]));
          memset(data_gas_array_char[x], 0, sizeof(data_gas_array_char[x]));
        }
        if((data_dag_int != 0) && (data_maand_int != 0) && (data_jaar_int != 0)){
          sprintf(periode_char, " %02d - %02d - %04d", data_dag_int, data_maand_int, data_jaar_int);
          lees_uur_verbruik_injectie();
        }
        if(data_dag_int == 0){
          sprintf(periode_char, " %02d - %04d", data_maand_int, data_jaar_int);
          String temp_string = "dag";
          temp_string.toCharArray(eenheid_char, temp_string.length() + 1);
          lees_dag_verbruik_injectie();
        }
        if(data_maand_int == 0){
          sprintf(periode_char, " %04d",data_jaar_int);
          String temp_string = "maand";
          temp_string.toCharArray(eenheid_char, temp_string.length() + 1);
          lees_maand_verbruik_injectie();
        }
        if(data_jaar_int == 0){
          String temp_string = "vorige jaren (max 25)";
          temp_string.toCharArray(periode_char, temp_string.length() + 1);
          temp_string = "jaar";
          temp_string.toCharArray(eenheid_char, temp_string.length() + 1);
          lees_jaar_verbruik_injectie();
        }
      }
  });
}
