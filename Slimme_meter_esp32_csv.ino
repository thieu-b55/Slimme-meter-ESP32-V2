/*
* MIT License
*
* Copyright (c) 2025 thieu-b55
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
 * Board ESP32 Dev Module
 * 
 * Espressif systems version 3.0.5  <<<<  IMPORTANT
 * 
 * Huge APP(3MB No OTA/1MBSPIFFS)
 * 
 * Arduino IDE 1.8.19
 * 
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
#include <string>

using namespace std; 

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
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void controle_na_spanningsuitval();
void schrijf_uurdata_naar_flash();
void schrijf_file_char();
void pwm1_sturen();
void pwm2_sturen();
void display_sturen();
void digitale_uitgangen_sturen();
void reset_data_csv_array();
void lees_uur_csv(fs::FS &fs, const char * path, int);
void lees_dag_csv(fs::FS &fs, const char * path); 
void lees_maand_csv(fs::FS &fs, const char * path); 
void lees_jaar_csv(fs::FS &fs, const char * path); 
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

const char header_char[] = "jaar,maand,dag,verbruik_0,injectie_0,gas_0,verbruik_1,injectie_1,gas_1,verbruik_2,injectie_2,gas_2,verbruik_3,injectie_3,gas_3,verbruik_4,injectie_4,gas_4,verbruik_5,injectie_5,gas_5,verbruik_6,injectie_6,gas_6,verbruik_7,injectie_7,gas_7,verbruik_8,injectie_8,gas_8,verbruik_9,injectie_9,gas_9,verbruik_10,injectie_10,gas_10,verbruik_11,injectie_11,gas_11,verbruik_12,injectie_12,gas_12,verbruik_13,injectie_13,gas_13,verbruik_14,injectie_14,gas_14,verbruik_15,injectie_15,gas_15,verbruik_16,injectie_16,gas_16,verbruik_17,injectie_17,gas_17,verbruik_18,injectie_18,gas_18,verbruik_19,injectie_19,gas_19,verbruik_20,injectie_20,gas_20,verbruik_21,injectie_21,gas_21,verbruik_22,injectie_22,gas_22,verbruik_23,injectie_23,gas_23,verbruik_dag,injectie_dag,gas_dag,verbruik_maand,injectie_maand,gas_maand,maandpiek,verbruik_jaar,injectie_jaar,gas_jaar\n";
const char maand_header_char[] = "maand,verbruik_maand,injectie_maand,gas_maand,maandpiek\n";
const char jaar_header_char[] = "jaar,verbruik_jaar,injectie_jaar,gas_jaar\n";

/*
 * WiFi Netwerknaam en paswoord
 */
const char* APSSID = "ESP32Energie_csv";
const char* APPSWD = "ESP32pswd";

bool dir_bestaat_bool = false;
bool file_bestaat_bool = false;
bool initialiseren = true;
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
char uren_verbruik_char[15];
char uren_injectie_char[15];
char uren_gas_char[15];
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
char data_maand_char[30];
char data_jaar_char[6];
char periode_char[25];
char eenheid_char[10];
char tijd_char[12];
char jaar_char[6];
char maand_char[8];
char jaar_maand_char[30];
char maand_data_char[30];
char jaar_data_char[30];
char dag_char[4];
char uitsturing_pwm1_char[6];
char uitsturing_pwm2_char[6];
char schakel_tijd_char[12];
char pwm1_override_char[8];
char pwm2_override_char[8];
char file_char[600];
char lees_file_char[1000];
char maand_file_char[200];
char jaar_file_char[200];
char tmp_char[15];
char x_char[4];
char data_csv_array[32][5][12];

float relais1_on_float;
float relais2_on_float;
float pwm1_kw_float;
float pwm2_kw_float;
float pwm2_dr_float;
float uur_verbruik_float;
float uur_injectie_float;
float uur_gas_float;
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
float kwh_totaal_vorig_float;
float injectie_totaal_vorig_float;
float gas_totaal_vorig_float;

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
int data_dag_int;;
int data_maand_int;
int data_jaar_int;
int uitsturing_pwm1_int = 0;
int uitsturing_pwm2_int = 0;
int module_teller = 0;
int relais_module_teller = 0;
int dagen_int;
int komma_int_array[100];
int jaar_csv_int;
int maand_csv_int;
int dag_csv_int;

String broadcastAddress1_string = "                       ";
String broadcastAddress2_string = "                       ";
String broadcastAddress3_string = "                       ";
String broadcastAddress4_string = "                       ";
String broadcastAddress5_string = "                       ";

String buffer_data_string =               "                                                         ";
String kwh_dag_string =                   "          ";
String kwh_nacht_string =                 "          ";
String injectie_dag_string =              "          ";
String injectie_nacht_string =            "          ";
String kwh_nu_string =                    "          ";
String injectie_nu_string =               "          ";
String piek_nu_string =                   "      ";
String gas_string =                       "         ";
String relais1_override =                 "     ";
String relais2_override =                 "     ";
String pwm1_override =                    "    ";
String pwm2_override =                    "    ";

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

char terminator = char(0x0a);
char temp_char[30];
char char_temp[10];
String temp_string = "                              ";
String module = "                            ";
String uren_string = "        ";
String minuten_string = "        ";
String override = "           ";

void setup() {
  delay(5000);
  pinMode(BLINKIE, OUTPUT);
  digitalWrite(BLINKIE, 0);
  pinMode(SD_CARD_ERROR, OUTPUT);
  digitalWrite(SD_CARD_ERROR, 0);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  if(!SD.begin(5)){
    //Serial.println("Kontroleer SD kaart");
    digitalWrite(SD_CARD_ERROR, 1);
  }
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
    /*
     * verbruik
     */
    pref.putInt("uur", 0);
    pref.putInt("dag", 0);
    pref.putInt("maand", 0);
    pref.putInt("jaar", 0);
    pref.putFloat("verbruik_0", 0.0);
    pref.putFloat("injectie_0", 0.0);
    pref.putFloat("gas_0", 0.0);
    pref.putFloat("verbruik_1", 0.0);
    pref.putFloat("injectie_1", 0.0);
    pref.putFloat("gas_1", 0.0);
    pref.putFloat("verbruik_2", 0.0);
    pref.putFloat("injectie_2", 0.0);
    pref.putFloat("gas_2", 0.0);
    pref.putFloat("verbruik_3", 0.0);
    pref.putFloat("injectie_3", 0.0);
    pref.putFloat("gas_3", 0.0);
    pref.putFloat("verbruik_4", 0.0);
    pref.putFloat("injectie_4", 0.0);
    pref.putFloat("gas_4", 5.55);
    pref.putFloat("verbruik_5", 0.0);
    pref.putFloat("injectie_5", 0.0);
    pref.putFloat("gas_5", 0.0);
    pref.putFloat("verbruik_6", 0.0);
    pref.putFloat("injectie_6", 0.0);
    pref.putFloat("gas_6", 0.0);
    pref.putFloat("verbruik_7", 0.0);
    pref.putFloat("injectie_7", 0.0);
    pref.putFloat("gas_7", 0.0);
    pref.putFloat("verbruik_8", 0.0);
    pref.putFloat("injectie_8", 0.0);
    pref.putFloat("gas_8", 0.0);
    pref.putFloat("verbruik_9", 0.0);
    pref.putFloat("injectie_9", 0.0);
    pref.putFloat("gas_9", 0.0);
    pref.putFloat("verbruik_10", 0.0);
    pref.putFloat("injectie_10", 0.0);
    pref.putFloat("gas_10", 0.0);
    pref.putFloat("verbruik_11", 0.0);
    pref.putFloat("injectie_11", 0.0);
    pref.putFloat("gas_11", 0.0);
    pref.putFloat("verbruik_12", 0.0);
    pref.putFloat("injectie_12", 0.0);
    pref.putFloat("gas_12", 0.0);
    pref.putFloat("verbruik_13", 0.0);
    pref.putFloat("injectie_13", 0.0);
    pref.putFloat("gas_13", 0.0);
    pref.putFloat("verbruik_14", 0.0);
    pref.putFloat("injectie_14", 0.0);
    pref.putFloat("gas_14", 0.0);
    pref.putFloat("verbruik_15", 0.0);
    pref.putFloat("injectie_15", 0.0);
    pref.putFloat("gas_15", 0.0);
    pref.putFloat("verbruik_16", 0.0);
    pref.putFloat("injectie_16", 0.0);
    pref.putFloat("gas_16", 0.0);
    pref.putFloat("verbruik_17", 0.0);
    pref.putFloat("injectie_17", 0.0);
    pref.putFloat("gas_17", 0.0);
    pref.putFloat("verbruik_18", 0.0);
    pref.putFloat("injectie_18", 0.0);
    pref.putFloat("gas_18", 0.0);
    pref.putFloat("verbruik_19", 0.0);
    pref.putFloat("injectie_19", 0.0);
    pref.putFloat("gas_19", 0.0);
    pref.putFloat("verbruik_20", 0.0);
    pref.putFloat("injectie_20", 0.0);
    pref.putFloat("gas_20", 0.0);
    pref.putFloat("verbruik_21", 0.0);
    pref.putFloat("injectie_21", 0.0);
    pref.putFloat("gas_21", 0.0);
    pref.putFloat("verbruik_22", 0.0);
    pref.putFloat("injectie_22", 0.0);
    pref.putFloat("gas_22", 0.0);
    pref.putFloat("verbruik_23", 0.0);
    pref.putFloat("injectie_23", 0.0);
    pref.putFloat("gas_23", 0.0);
    pref.putFloat("verbruik_dag", 0.0);
    pref.putFloat("injectie_dag", 0.0);
    pref.putFloat("gas_dag", 0.0);
    pref.putFloat("verbruik_maand", 0.0);
    pref.putFloat("injectie_maand", 0.0);
    pref.putFloat("gas_maand", 0.0);
    pref.putFloat("maandpiek", 0.0);
    pref.putFloat("verbruik_jaar", 0.0);
    pref.putFloat("injectie_jaar", 0.0);
    pref.putFloat("gas_jaar", 0.0);
    
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
  relais1_on_float = pref.getFloat("relais1_on");
  relais1_override = pref.getString("relais1_ov");
  relais2_on_float = pref.getFloat("relais2_on");
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
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
   
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer 1");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer 2");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer 3");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress4, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer 4");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress5, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer 5");
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
    //Serial.println("Met succes verzonden relais 1");
  }
  else {
    //Serial.println("fout bij verzenden naar relais 1");
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
    //Serial.println("Met succes verzonden relais 2");
  }
  else {
    //Serial.println("fout bij verzenden naar relais 2");
  }
  delay(1000);
  html_input();
  nu = millis();
}

void loop() {
  while(Serial2.available()){
    char lees_byte = Serial2.read();
    if(lees_byte == 0x2f){
      kwh_dag_float = kwh_dag_string.toFloat();
      kwh_nacht_float = kwh_nacht_string.toFloat();
      kwh_totaal_float = kwh_nacht_float + kwh_dag_float;
      injectie_dag_float = injectie_dag_string.toFloat();
      injectie_nacht_float = injectie_nacht_string.toFloat();
      injectie_totaal_float = injectie_nacht_float + injectie_dag_float;
      kwh_nu_float = kwh_nu_string.toFloat();
      piek_nu_float = piek_nu_string.toFloat();
      injectie_nu_float = injectie_nu_string.toFloat();
      verbruik_nu_float = injectie_nu_float - kwh_nu_float;
      gas_totaal_float = gas_string.toFloat();
      
      if(initialiseren == true){
        controle_na_spanningsuitval();
      }
      digitalWrite(BLINKIE, (digitalRead(BLINKIE) ^ 1));
      if(initialiseren == false){
        if(uren_vorig_int != uren_int){
          schrijf_uurdata_naar_flash();
          uren_vorig_int = uren_int;
        }
        if(dag_vorig_int != dag_int){
          schrijf_file_char();
          dag_vorig_int = dag_int;
          maand_vorig_int = maand_int;
          jaar_vorig_int = jaar_int;
          Serial.println(file_char);
        }
      }
      /*      
       * Elke 5 seconden PWM 1 / PWM 2
       */
      if(((millis() - nu) > 5000) && (!vijf_seconden)){
        vijf_seconden = true;
        pwm1_sturen();
        pwm2_sturen();
      }
      /*
       *  Elke 10 seconden PWM 1 / PWM 2 / Display
       */
      if((millis() - nu) > 10000){
        nu = millis();
        vijf_seconden = false;
        pwm1_sturen();
        pwm2_sturen();
        display_sturen();
      }
      digitale_uitgangen_sturen();
      /*
       * leesbuffer wissen datum tijd naar flash
       */
      buffer_data_string = "";     
      pref.putInt("uur", uren_int);
      pref.putInt("dag", dag_int);
      pref.putInt("maand", maand_int);
      pref.putInt("jaar", jaar_int);
    }
    /*
     * inlezen data van P1 poort in buffer_data_string
     */
    buffer_data_string += lees_byte;
    if(lees_byte == 0x0a){
      if((buffer_data_string.substring(4,9)) == "1.0.0"){
        jaar_int = ((buffer_data_string.substring(10,12)).toInt()) + 2000;
        maand_int = (buffer_data_string.substring(12,14)).toInt();;
        dag_int = (buffer_data_string.substring(14,16)).toInt();
        uren_int = (buffer_data_string.substring(16,18)).toInt();;
        minuten_int = (buffer_data_string.substring(18,20)).toInt();;
      }
      if((buffer_data_string.substring(4,9)) == "1.8.1"){
        kwh_dag_string = buffer_data_string.substring(10,20);
      }
      if((buffer_data_string.substring(4,9)) == "1.8.2"){
        kwh_nacht_string = buffer_data_string.substring(10,20);
      }
      if((buffer_data_string.substring(4,9)) == "2.8.1"){
        injectie_dag_string = buffer_data_string.substring(10,20);
      }
      if((buffer_data_string.substring(4,9)) == "2.8.2"){
        injectie_nacht_string = buffer_data_string.substring(10,20);
      }
      if((buffer_data_string.substring(4,9)) == "1.7.0"){
        kwh_nu_string = buffer_data_string.substring(10,16);
      }
      if((buffer_data_string.substring(4,9)) == "2.7.0"){
        injectie_nu_string = buffer_data_string.substring(10,16);
      }
      if((buffer_data_string.substring(4,9)) == "1.6.0"){
        piek_nu_string = buffer_data_string.substring(25,31);
      }
      if((buffer_data_string.substring(4,10)) == "24.2.3"){
        gas_string = buffer_data_string.substring(26,35);
      }
      //Serial.print(buffer_data_string);
      buffer_data_string = "";
    }
  }
}
/*
 * FUNCTIONS
 */
 
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  //Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.print(macStr);
  //Serial.print(" send status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void testDir(fs::FS &fs, const char * dirname, uint8_t levels){
  dir_bestaat_bool = false;
  File root = fs.open(dirname);
  if(root){
    dir_bestaat_bool = true;
  }
}
    
void testFile(fs::FS &fs, const char * path){
  file_bestaat_bool = false;
  File file = fs.open(path);
  if(file){
      file_bestaat_bool = true;
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

void writeFile(fs::FS &fs, const char * path, const char * message){
  //Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file){
    //Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    //Serial.println("File written");
  } 
  else {
    //Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  //Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    //Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      // Serial.println("Message appended");
  } else {
      // Serial.println("Append failed");
  }
  file.close();
}

void controle_na_spanningsuitval(){
  if((kwh_dag_float > 0.1) && (kwh_nacht_float > 0.1) & (injectie_dag_float > 0.1) && (injectie_nacht_float > 0.1) & (gas_totaal_float > 0.1)){
    initialiseren = false;
    memset(jaar_maand_char, 0, sizeof(jaar_maand_char));
    strcpy(jaar_maand_char, "/");
    strcat(jaar_maand_char, to_string(jaar_int).c_str());
    testDir(SD, jaar_maand_char, 1);
    if(!dir_bestaat_bool){
      createDir(SD, jaar_maand_char);
    }
    strcat(jaar_maand_char, "/maandfile");
    strcat(jaar_maand_char, "_");
    strcat(jaar_maand_char, to_string(maand_int).c_str());
    testFile(SD, jaar_maand_char);
    if(!file_bestaat_bool){
      writeFile(SD, jaar_maand_char, header_char);
    }
    memset(data_maand_char, 0, sizeof(data_maand_char));
    strcpy(data_maand_char, "/");
    strcat(data_maand_char, to_string(jaar_int).c_str());
    strcat(data_maand_char, "/maanddata");
    if(!file_bestaat_bool){
      writeFile(SD, data_maand_char, header_char);
    }
    memset(jaar_data_char, 0, sizeof(jaar_data_char));
    strcpy(jaar_data_char, "/jaar_data");
    testDir(SD, jaar_data_char, 1);
    if(!dir_bestaat_bool){
      createDir(SD, jaar_data_char);
      strcat(jaar_data_char, "/jaardata");
      writeFile(SD, jaar_data_char, jaar_header_char);
    }
    if(uren_vorig_int != uren_int){
      uren_vorig_int = uren_int;
      memset(uren_verbruik_char, 0, sizeof(uren_verbruik_char));
      memset(uren_injectie_char, 0, sizeof(uren_injectie_char));
      memset(uren_gas_char, 0, sizeof(uren_gas_char));
      strcpy(uren_verbruik_char, "verbruik_");
      strcpy(uren_injectie_char, "injectie_");
      strcpy(uren_gas_char, "gas_");
      strcat(uren_verbruik_char, to_string(uren_int).c_str());
      strcat(uren_injectie_char, to_string(uren_int).c_str());
      strcat(uren_gas_char, to_string(uren_int).c_str());
      pref.putFloat(uren_verbruik_char, kwh_totaal_float);
      pref.putFloat(uren_injectie_char, injectie_totaal_float);
      pref.putFloat(uren_gas_char, gas_totaal_float);
    }
    if(dag_vorig_int != dag_int){
      dag_vorig_int = dag_int;
      pref.putFloat("verbruik_dag", kwh_totaal_float);
      pref.putFloat("injectie_dag", injectie_totaal_float);
      pref.putFloat("gas_dag", gas_totaal_float);
    }
    if(maand_vorig_int != maand_int){
      maand_vorig_int = maand_int;
      pref.putFloat("verbruik_maand", kwh_totaal_float);
      pref.putFloat("injectie_maand", injectie_totaal_float);
      pref.putFloat("gas_maand", gas_totaal_float);
      pref.putFloat("maandpiek", piek_nu_float);
    }
    if(jaar_vorig_int != jaar_int){
      jaar_vorig_int = jaar_int;
      pref.putFloat("verbruik_jaar", kwh_totaal_float);
      pref.putFloat("injectie_jaar", injectie_totaal_float);
      pref.putFloat("gas_jaar", gas_totaal_float);
    }
  }
}

void schrijf_uurdata_naar_flash(){
  memset(uren_verbruik_char, 0, sizeof(uren_verbruik_char));
  memset(uren_injectie_char, 0, sizeof(uren_injectie_char));
  memset(uren_gas_char, 0, sizeof(uren_gas_char));
  strcpy(uren_verbruik_char, "verbruik_");
  strcpy(uren_injectie_char, "injectie_");
  strcpy(uren_gas_char, "gas_");
  strcat(uren_verbruik_char, to_string(uren_vorig_int).c_str()); 
  strcat(uren_injectie_char, to_string(uren_vorig_int).c_str());
  strcat(uren_gas_char, to_string(uren_vorig_int).c_str());
  kwh_totaal_vorig_float = pref.getFloat(uren_verbruik_char);
  injectie_totaal_vorig_float = pref.getFloat(uren_injectie_char);
  gas_totaal_vorig_float = pref.getFloat(uren_gas_char);
  pref.putFloat(uren_verbruik_char, (kwh_totaal_float - kwh_totaal_vorig_float));
  pref.putFloat(uren_injectie_char, (injectie_totaal_float - injectie_totaal_vorig_float));
  pref.putFloat(uren_gas_char, (gas_totaal_float - gas_totaal_vorig_float));
  pref.putFloat("maandpiek", piek_nu_float);
  if(dag_vorig_int == dag_int){
    memset(uren_verbruik_char, 0, sizeof(uren_verbruik_char));
    memset(uren_injectie_char, 0, sizeof(uren_injectie_char));
    memset(uren_gas_char, 0, sizeof(uren_gas_char));
    strcpy(uren_verbruik_char, "verbruik_");
    strcpy(uren_injectie_char, "injectie_");
    strcpy(uren_gas_char, "gas_");
    strcat(uren_verbruik_char, to_string(uren_int).c_str()); 
    strcat(uren_injectie_char, to_string(uren_int).c_str());
    strcat(uren_gas_char, to_string(uren_int).c_str());
    pref.putFloat(uren_verbruik_char, kwh_totaal_float);
    pref.putFloat(uren_injectie_char, injectie_totaal_float);
    pref.putFloat(uren_gas_char, gas_totaal_float);
  }
}

void schrijf_file_char(){
  memset(file_char, 0, sizeof(file_char));
  strcpy(file_char, (to_string(jaar_vorig_int).c_str()));
  strcat(file_char, ",");
  strcat(file_char, (to_string(maand_vorig_int).c_str()));
  strcat(file_char, ",");
  strcat(file_char, (to_string(dag_vorig_int).c_str()));
  /*
   * Schrijf uurdata naar file_char
   */
  for(int x = 0; x < 24; x++){
    strcat(file_char, ",");
    memset(x_char, 0, sizeof(x_char));
    strcpy(x_char, to_string(x).c_str());
    memset(uren_verbruik_char, 0, sizeof(uren_verbruik_char));
    memset(uren_injectie_char, 0, sizeof(uren_injectie_char));
    memset(uren_gas_char, 0, sizeof(uren_gas_char));
    strcpy(uren_verbruik_char, "verbruik_");
    strcpy(uren_injectie_char, "injectie_");
    strcpy(uren_gas_char, "gas_");
    strcat(uren_verbruik_char, x_char); 
    strcat(uren_injectie_char, x_char);
    strcat(uren_gas_char, x_char);
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", pref.getFloat(uren_verbruik_char));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", pref.getFloat(uren_injectie_char));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", pref.getFloat(uren_gas_char));
    strcat(file_char, tmp_char);
  }
  /*
   * schrijf huidige waaarden naar ..._0 positie (0 uur waarden)
   */
  memset(uren_verbruik_char, 0, sizeof(uren_verbruik_char));
  memset(uren_injectie_char, 0, sizeof(uren_injectie_char));
  memset(uren_gas_char, 0, sizeof(uren_gas_char));
  strcpy(uren_verbruik_char, "verbruik_");
  strcpy(uren_injectie_char, "injectie_");
  strcpy(uren_gas_char, "gas_");
  strcat(uren_verbruik_char, to_string(uren_int).c_str()); 
  strcat(uren_injectie_char, to_string(uren_int).c_str());
  strcat(uren_gas_char, to_string(uren_int).c_str());
  pref.putFloat(uren_verbruik_char, kwh_totaal_float);
  pref.putFloat(uren_injectie_char, injectie_totaal_float);
  pref.putFloat(uren_gas_char, gas_totaal_float);
  /*
   * schrijf dagdata naar file_char
   */
  strcat(file_char, ","); 
  memset(tmp_char, 0, sizeof(tmp_char));        
  sprintf(tmp_char, "%.3f", (kwh_totaal_float - pref.getFloat("verbruik_dag")));
  strcat(file_char, tmp_char);
  strcat(file_char, ",");
  memset(tmp_char, 0, sizeof(tmp_char));
  sprintf(tmp_char, "%.3f", (injectie_totaal_float - pref.getFloat("injectie_dag")));
  strcat(file_char, tmp_char);
  strcat(file_char, ",");
  memset(tmp_char, 0, sizeof(tmp_char));
  sprintf(tmp_char, "%.3f", (gas_totaal_float - pref.getFloat("gas_dag")));
  strcat(file_char, tmp_char);
  strcat(file_char, ",");
  pref.putFloat("verbruik_dag", kwh_totaal_float);
  pref.putFloat("injectie_dag", injectie_totaal_float);
  pref.putFloat("gas_dag", gas_totaal_float);
  
  if(maand_vorig_int != maand_int){
    /*
     * schrijf nieuwe maand_. file
     */
    memset(jaar_maand_char, 0, sizeof(jaar_maand_char));
    strcpy(jaar_maand_char, "/");
    strcat(jaar_maand_char, to_string(jaar_int).c_str());
    createDir(SD, jaar_maand_char);
    strcat(jaar_maand_char, "/maandfile");
    strcat(jaar_maand_char, "_");
    strcat(jaar_maand_char, to_string(maand_int).c_str());
    writeFile(SD, jaar_maand_char, header_char);
    /*           
     *   schrijf maanddata naar file_char
     */
    memset(tmp_char, 0, sizeof(tmp_char));        
    sprintf(tmp_char, "%.3f", (kwh_totaal_float - pref.getFloat("verbruik_maand")));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (injectie_totaal_float - pref.getFloat("injectie_maand")));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (gas_totaal_float - pref.getFloat("gas_maand")));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    sprintf(tmp_char, "%.3f", pref.getFloat("maandpiek"));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    /*
     * schrijf maanddatta naar maanddata file in jaar folder
     */
    memset(maand_data_char, 0, sizeof(maand_data_char));
    strcpy(maand_data_char, "/");
    strcat(maand_data_char, to_string(jaar_vorig_int).c_str());
    strcat(maand_data_char, "/maanddata");
    memset(maand_file_char, 0, sizeof(maand_file_char));
    strcpy(maand_file_char, to_string(maand_vorig_int).c_str());
    strcat(maand_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));        
    sprintf(tmp_char, "%.3f", (kwh_totaal_float - pref.getFloat("verbruik_maand")));
    strcat(maand_file_char, tmp_char);
    strcat(maand_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (injectie_totaal_float - pref.getFloat("injectie_maand")));
    strcat(maand_file_char, tmp_char);
    strcat(maand_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (gas_totaal_float - pref.getFloat("gas_maand")));
    strcat(maand_file_char, tmp_char);
    strcat(maand_file_char, ",");
    sprintf(tmp_char, "%.3f", pref.getFloat("maandpiek"));
    strcat(maand_file_char, tmp_char);
    strcat(maand_file_char, ",");
    strcat(maand_file_char, "\n");
    appendFile(SD, maand_data_char, maand_file_char);
    pref.putFloat("verbruik_maand", kwh_totaal_float);
    pref.putFloat("injectie_maand", injectie_totaal_float);
    pref.putFloat("gas_maand", gas_totaal_float);
  }
  /*
   * indoen niet einde maand schrijf 0 naar maanddata totaal posities
   */
  else{
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", 0.0);
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", 0.0);
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", 0.0);
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", 0.0);
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
  }
  if(jaar_vorig_int != jaar_int){
    /*
     * schrijf jaardata naar file_char
     */
    memset(tmp_char, 0, sizeof(tmp_char));        
    sprintf(tmp_char, "%.3f", (kwh_totaal_float - pref.getFloat("verbruik_jaar")));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (injectie_totaal_float - pref.getFloat("injectie_jaar")));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (gas_totaal_float - pref.getFloat("gas_jaar")));
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    /*
     * schrijf jaardata naar jaardata file in jaar_data folder
     */
    memset(jaar_data_char, 0, sizeof(jaar_data_char));
    strcpy(jaar_data_char, "/jaar_data");
    strcat(jaar_data_char, "/jaardata");
    memset(jaar_file_char, 0, sizeof(jaar_file_char));
    strcpy(jaar_file_char, to_string(jaar_vorig_int).c_str());
    strcat(jaar_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));        
    sprintf(tmp_char, "%.3f", (kwh_totaal_float - pref.getFloat("verbruik_jaar")));
    strcat(jaar_file_char, tmp_char);
    strcat(jaar_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (injectie_totaal_float - pref.getFloat("injectie_jaar")));
    strcat(jaar_file_char, tmp_char);
    strcat(jaar_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (gas_totaal_float - pref.getFloat("gas_jaar")));
    strcat(jaar_file_char, tmp_char);
    strcat(jaar_file_char, ",");
    strcat(jaar_file_char, "\n");
    appendFile(SD, jaar_data_char, jaar_file_char);
    /*
     * schrijf jaartotaal ook naar maandata
     */
    memset(maand_data_char, 0, sizeof(maand_data_char));
    strcpy(maand_data_char, "/");
    strcat(maand_data_char, to_string(jaar_vorig_int).c_str());
    strcat(maand_data_char, "/maanddata");
    memset(maand_file_char, 0, sizeof(maand_file_char));
    strcpy(maand_file_char, to_string(13).c_str());         //13 voor totaal verbruik 
    strcat(maand_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));        
    sprintf(tmp_char, "%.3f", (kwh_totaal_float - pref.getFloat("verbruik_jaar")));
    strcat(maand_file_char, tmp_char);
    strcat(maand_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (injectie_totaal_float - pref.getFloat("injectie_jaar")));
    strcat(maand_file_char, tmp_char);
    strcat(maand_file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", (gas_totaal_float - pref.getFloat("gas_jaar")));
    strcat(maand_file_char, tmp_char);
    strcat(maand_file_char, ",");
    strcat(maand_file_char, "\n");
    appendFile(SD, maand_data_char, maand_file_char);
    /*
     * Schrijf nieuwe maanddata fila in jaarfolder
     */
    memset(maand_data_char, 0, sizeof(maand_data_char));
    strcpy(maand_data_char, "/");
    strcat(maand_data_char, to_string(jaar_int).c_str());
    strcat(maand_data_char, "/maanddata");
    writeFile(SD, maand_data_char, maand_header_char);
    pref.putFloat("verbruik_jaar", kwh_totaal_float);
    pref.putFloat("injectie_jaar", injectie_totaal_float);
    pref.putFloat("gas_jaar", gas_totaal_float);
  }
  else{
    /*
     * indien niet einde jaar schrijf 0 naar jaardata totaal posoties
     */
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", 0.0);
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", 0.0);
    strcat(file_char, tmp_char);
    strcat(file_char, ",");
    memset(tmp_char, 0, sizeof(tmp_char));
    sprintf(tmp_char, "%.3f", 0.0);
    strcat(file_char, tmp_char);
  }
  strcat(file_char, ",");
  strcat(file_char, "\n");
  memset(jaar_maand_char, 0, sizeof(jaar_maand_char));
  strcpy(jaar_maand_char, "/");
  strcat(jaar_maand_char, to_string(jaar_vorig_int).c_str());
  strcat(jaar_maand_char, "/maandfile_");
  strcat(jaar_maand_char, to_string(maand_vorig_int).c_str());
  appendFile(SD, jaar_maand_char, file_char);
}

void pwm1_sturen(){
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
    //Serial.println("Sent pwm1 with success");
  }
  else {
    //Serial.println("pwm1 error");
  }
}

void pwm2_sturen(){
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
    //Serial.println("Sent pwm2 with success");
  }
  else {
    //Serial.println("pwm2 error");
  }
}

void display_sturen(){
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
    //Serial.println("Met succes verzonden Display");
  }
  else {
    //Serial.println("display error");
  }
}

void digitale_uitgangen_sturen(){
  /*
   * om middernacht alle digitale uitgangen resetten
   */
  if((uren_int == 0) && (minuten_int == 0)){
    relais1_tijd = false;
    relais1_uit = false;
    relais2_tijd = false;
    relais2_uit = false; 
    uitsturen.relais = false;
    result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
    if (result == ESP_OK) {
      //Serial.println("Met succes verzonden relais 1");
    }
    else {
      //Serial.println("fout bij verzenden naar relais 1");
    }
    delay(100);
    result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
    if (result == ESP_OK) {
      //Serial.println("Met succes verzonden relais 2");
    }
    else {
      //Serial.println("fout bij verzenden naar relais 2");
    }
  }
  /*
   * Relais 1
   */
  if(relais1_override == "A"){
    if((uren_int == uren_on1_int) && (minuten_int == minuten_on1_int) && (relais1_tijd == false)){
      relais1_tijd = true;
    }
    if((relais1_tijd == true) && (relais1_on_float >= verbruik_pwm1_float) && (relais1_uit == false)){
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
      //Serial.println("Met succes verzonden relais 1");
    }
    else {
      //Serial.println("fout bij verzenden naar relais 1");
    }
  }
  /*
   * Relais 2
   */
  if(relais2_override == "A"){
    if((uren_int == uren_on2_int) && (minuten_int == minuten_on2_int) && (relais2_tijd == false)){
      relais2_tijd = true;
    }
    if((relais2_tijd == true) && (relais2_on_float >= verbruik_pwm2_float) && (relais2_uit == false)){
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
      //Serial.println("Met succes verzonden relais 2");
    }
    else {
      //Serial.println("fout bij verzenden naar relais 2");
    }
  }
}

void reset_data_csv_array(){
  for(int x = 0; x < 32; x++){
    memset(data_csv_array[x][0], 0, sizeof(data_csv_array[x][0]));
    memset(data_csv_array[x][1], 0, sizeof(data_csv_array[x][1]));
    memset(data_csv_array[x][2], 0, sizeof(data_csv_array[x][2]));
    memset(data_csv_array[x][3], 0, sizeof(data_csv_array[x][3]));
    memset(data_csv_array[x][4], 0, sizeof(data_csv_array[x][3]));
  }
}

void lees_uur_csv(fs::FS &fs, const char * path, int dag_int){ 
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  bool einde_bool = false;
  int teller_int = 0;
  int positie_int = 0;
  int komma_teller_int = 0;
  char linefeed_char = '\n';
  char komma_char = ',';
  string lees_file_string(1000, 0);
  string datum_string(8, 0);
  string tmp_string(40, 0);
  /*
   * header file overslaan
   */
  while((file.available()) && (!einde_bool)){
    char dummy_char = file.read();
    if(dummy_char == linefeed_char){
      einde_bool = true;
    }
  }
  /*
   * lees lijn per lijn
   */
  einde_bool = false;
  memset(lees_file_char, 0, sizeof(lees_file_char));
  komma_int_array[0] = 0;
  while((file.available()) && (!einde_bool)){
    lees_file_char[teller_int] = file.read();
    if(char(lees_file_char[teller_int]) == linefeed_char){
      teller_int = 0;
      positie_int = 0;
      komma_teller_int = 0;
      lees_file_string = lees_file_char;
      while(positie_int != -1){
        positie_int = lees_file_string.find(komma_char, positie_int);
        if(positie_int != -1){
          komma_int_array[komma_teller_int] = positie_int;
          komma_teller_int ++;
          positie_int += 1;
        }
      }
      datum_string = lees_file_string.substr(komma_int_array[1] + 1, komma_int_array[2]);
      if(stoi(datum_string) != dag_int){
        memset(lees_file_char, 0, sizeof(lees_file_char));
      }
      else{
        einde_bool = true;
      }
    }
    else{
      teller_int ++;
    }
  }
  file.close();
  /*
   * hier is iets mis
   */
  if(lees_file_char[0] == 0){
    return;
  }
  tmp_string = lees_file_string.substr(0, komma_int_array[0]);
  jaar_csv_int = stoi(tmp_string);
  tmp_string = lees_file_string.substr(komma_int_array[0] + 1, (komma_int_array[1] - (komma_int_array[0] + 1)));
  maand_csv_int = stoi(tmp_string);
  tmp_string = lees_file_string.substr(komma_int_array[1] + 1, (komma_int_array[2] - (komma_int_array[1] + 1)));
  dag_csv_int = stoi(tmp_string);
  int array_teller = 2;
  for(int x = 0; x < 24; x ++){
    strcpy(data_csv_array[x][0], to_string(x).c_str());
    tmp_string = lees_file_string.substr(komma_int_array[array_teller] + 1, (komma_int_array[array_teller + 1] - (komma_int_array[array_teller] + 1)));
    strcpy(data_csv_array[x][1], tmp_string.c_str());
    tmp_string = lees_file_string.substr(komma_int_array[array_teller + 1] + 1, (komma_int_array[array_teller + 2] - (komma_int_array[array_teller + 1] + 1)));
    strcpy(data_csv_array[x][2], tmp_string.c_str());
    tmp_string = lees_file_string.substr(komma_int_array[array_teller + 2] + 1, (komma_int_array[array_teller + 3] - (komma_int_array[array_teller + 2] + 1)));
    strcpy(data_csv_array[x][3], tmp_string.c_str());
    array_teller += 3;
  }
  strcpy(data_csv_array[24][0], "Totaal");
  tmp_string = lees_file_string.substr(komma_int_array[array_teller] + 1, (komma_int_array[array_teller + 1] - (komma_int_array[array_teller] + 1)));
  strcpy(data_csv_array[24][1], tmp_string.c_str());
  tmp_string = lees_file_string.substr(komma_int_array[array_teller + 1] + 1, (komma_int_array[array_teller + 2] - (komma_int_array[array_teller + 1] + 1)));
  strcpy(data_csv_array[24][2], tmp_string.c_str());
  tmp_string = lees_file_string.substr(komma_int_array[array_teller + 2] + 1, (komma_int_array[array_teller + 3] - (komma_int_array[array_teller + 2] + 1)));
  strcpy(data_csv_array[24][3], tmp_string.c_str());
}

void lees_dag_csv(fs::FS &fs, const char * path){ 
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  bool einde_bool = false;
  int teller_int = 0;
  int positie_int = 0;
  int komma_teller_int = 0;
  char linefeed_char = '\n';
  char komma_char = ',';
  string lees_file_string;
  string maand_string = "    ";
  string tmp_string = "               ";
  /*
   * header file overslaan
   */
  while((file.available()) && (!einde_bool)){
    char dummy_char = file.read();
    if(dummy_char == linefeed_char){
      einde_bool = true;
    }
  }
  /*
   * lees lijn per lijn
   */
  memset(lees_file_char, 0, sizeof(lees_file_char));
  while(file.available()){
    lees_file_char[teller_int] = file.read();
    if(char(lees_file_char[teller_int]) == linefeed_char){
      teller_int = 0;
      positie_int = 0;
      komma_teller_int = 0;
      lees_file_string = lees_file_char;
      while(positie_int != -1){
        positie_int = lees_file_string.find(komma_char, positie_int);
        if(positie_int != -1){
          komma_int_array[komma_teller_int] = positie_int;
          komma_teller_int ++;
          positie_int += 1;
        }
      }
      tmp_string = lees_file_string.substr(komma_int_array[1] + 1, (komma_int_array[2] - (komma_int_array[1] + 1)));
      dag_csv_int = stoi(tmp_string);
      int array_teller = 0;
      strcpy(data_csv_array[dag_csv_int - 1][0], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[74] + 1, (komma_int_array[75] - (komma_int_array[74] + 1)));
      strcpy(data_csv_array[dag_csv_int - 1][1], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[75] + 1, (komma_int_array[76] - (komma_int_array[75] + 1)));
      strcpy(data_csv_array[dag_csv_int - 1][2], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[76] + 1, (komma_int_array[77] - (komma_int_array[76] + 1)));
      strcpy(data_csv_array[dag_csv_int - 1][3], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[77] + 1, (komma_int_array[78] - (komma_int_array[77] + 1)));
      strcpy(data_csv_array[31][0], "Totaal");
      strcpy(data_csv_array[31][1], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[78] + 1, (komma_int_array[79] - (komma_int_array[78] + 1)));
      strcpy(data_csv_array[31][2], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[79] + 1, (komma_int_array[80] - (komma_int_array[79] + 1)));
      strcpy(data_csv_array[31][3], tmp_string.c_str());
      memset(lees_file_char, 0, sizeof(lees_file_char));
    }
    else{
      teller_int ++;
    }
  }
  file.close();
}

void lees_maand_csv(fs::FS &fs, const char * path){ 
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  bool einde_bool = false;
  int teller_int = 0;
  int positie_int = 0;
  int komma_teller_int = 0;
  char linefeed_char = '\n';
  char komma_char = ',';
  string lees_file_string;
  string maand_string = "    ";
  string tmp_string = "               ";
  /*
   * header file overslaan
   */
  while((file.available()) && (!einde_bool)){
    char dummy_char = file.read();
    if(dummy_char == linefeed_char){
      einde_bool = true;
    }
  }
  /*
   * lees lijn per lijn
   */
  memset(lees_file_char, 0, sizeof(lees_file_char));
  while(file.available()){
    lees_file_char[teller_int] = file.read();
    if(char(lees_file_char[teller_int]) == linefeed_char){
      teller_int = 0;
      positie_int = 0;
      komma_teller_int = 0;
      lees_file_string = lees_file_char;
      while(positie_int != -1){
        positie_int = lees_file_string.find(komma_char, positie_int);
        if(positie_int != -1){
          komma_int_array[komma_teller_int] = positie_int;
          komma_teller_int ++;
          positie_int += 1;
        }
      }
      tmp_string = lees_file_string.substr(0, komma_int_array[0]);
      maand_csv_int = stoi(tmp_string);
      int array_teller = 0;
      strcpy(data_csv_array[maand_csv_int - 1][0], tmp_string.c_str());
      strcpy(data_csv_array[maand_csv_int + 13][0], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[0] + 1, (komma_int_array[1] - (komma_int_array[0] + 1)));
      strcpy(data_csv_array[maand_csv_int - 1][1], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[1] + 1, (komma_int_array[2] - (komma_int_array[1] + 1)));
      strcpy(data_csv_array[maand_csv_int - 1][2], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[2] + 1, (komma_int_array[3] - (komma_int_array[2] + 1)));
      strcpy(data_csv_array[maand_csv_int - 1][3], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[3] + 1, (komma_int_array[4] - (komma_int_array[3] + 1)));
      strcpy(data_csv_array[maand_csv_int + 13][1], tmp_string.c_str());
      memset(lees_file_char, 0, sizeof(lees_file_char));
    }
    else{
      teller_int ++;
    }
  }
  strcpy(data_csv_array[12][0], "Totaal");
  strcpy(data_csv_array[26][0], "      ");
  file.close();
}

void lees_jaar_csv(fs::FS &fs, const char * path){ 
  //Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
    return;
  }
  bool einde_bool = false;
  int teller_int = 0;
  int positie_int = 0;
  int komma_teller_int = 0;
  int jaren_teller_int = 0;
  char linefeed_char = '\n';
  char komma_char = ',';
  string lees_file_string;
  string maand_string = "    ";
  string tmp_string = "               ";
  /*
   * header file overslaan
   */
  while((file.available()) && (!einde_bool)){
    char dummy_char = file.read();
    if(dummy_char == linefeed_char){
      einde_bool = true;
    }
  }
  /*
   * lees lijn per lijn
   */
  memset(lees_file_char, 0, sizeof(lees_file_char));
  while(file.available()){
    lees_file_char[teller_int] = file.read();
    if(char(lees_file_char[teller_int]) == linefeed_char){
      teller_int = 0;
      positie_int = 0;
      komma_teller_int = 0;
      lees_file_string = lees_file_char;
      while(positie_int != -1){
        positie_int = lees_file_string.find(komma_char, positie_int);
        if(positie_int != -1){
          komma_int_array[komma_teller_int] = positie_int;
          komma_teller_int ++;
          positie_int += 1;
        }
      }
      int array_teller = 0;
      tmp_string = lees_file_string.substr(0, komma_int_array[0]);
      strcpy(data_csv_array[jaren_teller_int][0], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[0] + 1, (komma_int_array[1] - (komma_int_array[0] + 1)));
      strcpy(data_csv_array[jaren_teller_int][1], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[1] + 1, (komma_int_array[2] - (komma_int_array[1] + 1)));
      strcpy(data_csv_array[jaren_teller_int][2], tmp_string.c_str());
      tmp_string = lees_file_string.substr(komma_int_array[2] + 1, (komma_int_array[3] - (komma_int_array[2] + 1)));
      strcpy(data_csv_array[jaren_teller_int][3], tmp_string.c_str());
      memset(lees_file_char, 0, sizeof(lees_file_char));
      jaren_teller_int ++;
    }
    else{
      teller_int ++;
    }
  }
  file.close();
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
         <h6><b>thieu-b55 december 2025</b></h6>
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
  int macx_0 = 0;
  int macx_1 = 0;
  int macx_2 = 0;
  int macx_3 = 0;
  int macx_4 = 0;
  int macx_5 = 0;
  if(var == "electriciteit_totaal"){
    sprintf(kwh_totaal_float_char , "%.3f", kwh_totaal_float);
    return(kwh_totaal_float_char);
  }
  if(var == "injectie_totaal"){
    sprintf(injectie_totaal_float_char , "%.3f", injectie_totaal_float);
    return(injectie_totaal_float_char);
  }
  if(var == "kwh_nu"){
    sprintf(kwh_nu_float_char , "%.3f", kwh_nu_float);
    return(kwh_nu_float_char);
  }
  if(var == "injectie_nu"){
    sprintf(injectie_nu_float_char , "%.3f", injectie_nu_float);
    return(injectie_nu_float_char);
  }
  if(var == "piek_nu"){
    sprintf(piek_nu_float_char , "%.3f", piek_nu_float);
    return(piek_nu_float_char);
  }
  if(var == "gas_totaal"){
    sprintf(gas_totaal_float_char , "%.3f", gas_totaal_float);
    return(gas_totaal_float_char);
  }
  if(var == "relais1_sturing"){
    if(relais1_uit == true){
      temp_string = "1";
    }
    if(relais1_uit == false){
      temp_string = "0";
    }
    if(relais1_override == "0"){
      temp_string = "0";
    }
    if(relais1_override == "1"){
      temp_string = "1";
    }
    temp_string.toCharArray(relais1_sturing_char, (temp_string.length() + 1));
    return(relais1_sturing_char);
  }
  if(var == "relais2_sturing"){
    if(relais2_uit == true){
      temp_string = "1";
    }
    if(relais2_uit == false){
      temp_string = "0";
    }
    if(relais2_override == "0"){
      temp_string = "0";
    }
    if(relais2_override == "1"){
      temp_string = "1";
    }
    temp_string.toCharArray(relais2_sturing_char, (temp_string.length() + 1));
    return(relais2_sturing_char);
  }
  if(var == "procent_1"){
    sprintf(uitsturing_pwm1_char, "%3d", uitsturing_pwm1_int);
    return(uitsturing_pwm1_char);
  }
  if(var == "procent_2"){
    sprintf(uitsturing_pwm2_char, "%3d", uitsturing_pwm2_int);
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
        temp_string = "Relais 1";
        break;
      case 1:
        temp_string = "Relais 2";
        break;
    }
    temp_string.toCharArray(relais_module_char, (temp_string.length() + 1));
    return(relais_module_char);
  }
  if(var == "kw_on"){
    switch(relais_module_teller){
      case 0:
        return(String(relais1_on_float));
        break;
      case 1:
        return(String(relais2_on_float));
        break;
    }
  }
  if(var == "override"){
    switch(relais_module_teller){
      case 0:
        temp_string = relais1_override;
        break;
      case 1:
        temp_string = relais2_override;
        break;
    }
    temp_string.toCharArray(override_char, (temp_string.length() + 1));
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
    return(data_csv_array[0][0]);
  } 
  if(var == "periode_2"){
    return(data_csv_array[1][0]);
  }
  if(var == "periode_3"){
    return(data_csv_array[2][0]);
  }
  if(var == "periode_4"){
    return(data_csv_array[3][0]);
  }
  if(var == "periode_5"){
    return(data_csv_array[4][0]);
  }
  if(var == "periode_6"){
    return(data_csv_array[5][0]);
  }
  if(var == "periode_7"){
    return(data_csv_array[6][0]);
  }
  if(var == "periode_8"){
    return(data_csv_array[7][0]);
  }
  if(var == "periode_9"){
    return(data_csv_array[8][0]);
  }
  if(var == "periode_10"){
    return(data_csv_array[9][0]);
  }
  if(var == "periode_11"){
    return(data_csv_array[10][0]);
  }
  if(var == "periode_12"){
    return(data_csv_array[11][0]);
  }
  if(var == "periode_13"){
    return(data_csv_array[12][0]);
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
    return(data_csv_array[13][0]);
  }
  if(var == "periode_15"){
    return(data_csv_array[14][0]);
  }
  if(var == "periode_16"){
    return(data_csv_array[15][0]);
  }
  if(var == "periode_17"){
    return(data_csv_array[16][0]);
  }
  if(var == "periode_18"){
    return(data_csv_array[17][0]);
  }
  if(var == "periode_19"){
    return(data_csv_array[18][0]);
  }
  if(var == "periode_20"){
    return(data_csv_array[19][0]);
  }
  if(var == "periode_21"){
    return(data_csv_array[20][0]);
  }
  if(var == "periode_22"){
    return(data_csv_array[21][0]);
  }
  if(var == "periode_23"){
    return(data_csv_array[22][0]);
  }
  if(var == "periode_24"){
    return(data_csv_array[23][0]);
  }
  if(var == "periode_25"){
    return(data_csv_array[24][0]);
  }
  if(var == "periode_26"){
    return(data_csv_array[25][0]);
  }
  if(var == "periode_27"){
    return(data_csv_array[26][0]);
  }
  if(var == "periode_28"){
    return(data_csv_array[27][0]);
  }
  if(var == "periode_29"){
    return(data_csv_array[28][0]);
  }
  if(var == "periode_30"){
    return(data_csv_array[29][0]);
  }
  if(var == "periode_31"){
    return(data_csv_array[30][0]);
  }
  if(var == "periode_32"){
    return(data_csv_array[31][0]);
  }
  if(var == "verbruik_1"){
    return(data_csv_array[0][1]);
  } 
  if(var == "verbruik_2"){
    return(data_csv_array[1][1]);
  }
  if(var == "verbruik_3"){
    return(data_csv_array[2][1]);
  }
  if(var == "verbruik_4"){
    return(data_csv_array[3][1]);
  }
  if(var == "verbruik_5"){
    return(data_csv_array[4][1]);
  }
  if(var == "verbruik_6"){
    return(data_csv_array[5][1]);
  }
  if(var == "verbruik_7"){
    return(data_csv_array[6][1]);
  }
  if(var == "verbruik_8"){
    return(data_csv_array[7][1]);
  }
  if(var == "verbruik_9"){
    return(data_csv_array[8][1]);
  }
  if(var == "verbruik_10"){
    return(data_csv_array[9][1]);
  }
  if(var == "verbruik_11"){
    return(data_csv_array[10][1]);
  }
  if(var == "verbruik_12"){
    return(data_csv_array[11][1]);
  }
  if(var == "verbruik_13"){
    return(data_csv_array[12][1]);
  }
  if(var == "verbruik_14"){
    return(data_csv_array[13][1]);
  }
  if(var == "verbruik_15"){
    return(data_csv_array[14][1]);
  }
  if(var == "verbruik_16"){
    return(data_csv_array[15][1]);
  }
  if(var == "verbruik_17"){
    return(data_csv_array[16][1]);
  }
  if(var == "verbruik_18"){
    return(data_csv_array[17][1]);
  }
  if(var == "verbruik_19"){
    return(data_csv_array[18][1]);
  }
  if(var == "verbruik_20"){
    return(data_csv_array[19][1]);
  }
  if(var == "verbruik_21"){
    return(data_csv_array[20][1]);
  }
  if(var == "verbruik_22"){
    return(data_csv_array[21][1]);
  }
  if(var == "verbruik_23"){
    return(data_csv_array[22][1]);
  }
  if(var == "verbruik_24"){
    return(data_csv_array[23][1]);
  }
  if(var == "verbruik_25"){
    return(data_csv_array[24][1]);
  }
  if(var == "verbruik_26"){
    return(data_csv_array[25][1]);
  }
  if(var == "verbruik_27"){
    return(data_csv_array[26][1]);
  }
  if(var == "verbruik_28"){
    return(data_csv_array[27][1]);
  }
  if(var == "verbruik_29"){
    return(data_csv_array[28][1]);
  }
  if(var == "verbruik_30"){
    return(data_csv_array[29][1]);
  }
  if(var == "verbruik_31"){
    return(data_csv_array[30][1]);
  }
  if(var == "verbruik_32"){
    return(data_csv_array[31][1]);
  }
  if(var == "injectie_1"){
    return(data_csv_array[0][2]); 
  } 
  if(var == "injectie_2"){
    return(data_csv_array[1][2]); 
  }
  if(var == "injectie_3"){
    return(data_csv_array[2][2]); 
  }
  if(var == "injectie_4"){
    return(data_csv_array[3][2]); 
  }
  if(var == "injectie_5"){
    return(data_csv_array[4][2]); 
  }
  if(var == "injectie_6"){
    return(data_csv_array[5][2]); 
  }
  if(var == "injectie_7"){
    return(data_csv_array[6][2]);  
  }
  if(var == "injectie_8"){
    return(data_csv_array[7][2]); 
  }
  if(var == "injectie_9"){
    return(data_csv_array[8][2]); 
  }
  if(var == "injectie_10"){
    return(data_csv_array[9][2]); 
  }
  if(var == "injectie_11"){
    return(data_csv_array[10][2]); 
  }
  if(var == "injectie_12"){
    return(data_csv_array[11][2]);  
  }
  if(var == "injectie_13"){
    return(data_csv_array[12][2]);  
  }
  if(var == "injectie_14"){
    return(data_csv_array[13][2]); 
  }
  if(var == "injectie_15"){
    return(data_csv_array[14][2]); 
  }
  if(var == "injectie_16"){
    return(data_csv_array[15][2]); 
  }
  if(var == "injectie_17"){
    return(data_csv_array[16][2]); 
  }
  if(var == "injectie_18"){
    return(data_csv_array[17][2]); 
  }
  if(var == "injectie_19"){
    return(data_csv_array[18][2]); 
  }
  if(var == "injectie_20"){
    return(data_csv_array[19][2]); 
  }
  if(var == "injectie_21"){
    return(data_csv_array[20][2]);  
  }
  if(var == "injectie_22"){
    return(data_csv_array[21][2]);  
  }
  if(var == "injectie_23"){
    return(data_csv_array[22][2]); 
  }
  if(var == "injectie_24"){
    return(data_csv_array[23][2]); 
  }
  if(var == "injectie_25"){
    return(data_csv_array[24][2]); 
  }
  if(var == "injectie_26"){
    return(data_csv_array[25][2]); 
  }
  if(var == "injectie_27"){
    return(data_csv_array[26][2]); 
  }
  if(var == "injectie_28"){
    return(data_csv_array[27][2]); 
  }
  if(var == "injectie_29"){
    return(data_csv_array[28][2]); 
  }
  if(var == "injectie_30"){
    return(data_csv_array[29][2]); 
  }
  if(var == "injectie_31"){
    return(data_csv_array[30][2]); 
  }
  if(var == "injectie_32"){
    return(data_csv_array[31][2]); 
  }
  if(var == "gas_verbruik_1"){
    return(data_csv_array[0][3]);
  } 
  if(var == "gas_verbruik_2"){
    return(data_csv_array[1][3]);
  }
  if(var == "gas_verbruik_3"){
    return(data_csv_array[2][3]);
  }
  if(var == "gas_verbruik_4"){
    return(data_csv_array[3][3]);
  }
  if(var == "gas_verbruik_5"){
    return(data_csv_array[4][3]);
  }
  if(var == "gas_verbruik_6"){
    return(data_csv_array[5][3]);
  }
  if(var == "gas_verbruik_7"){
    return(data_csv_array[6][3]);
  }
  if(var == "gas_verbruik_8"){
    return(data_csv_array[7][3]);
  }
  if(var == "gas_verbruik_9"){
    return(data_csv_array[8][3]);
  }
  if(var == "gas_verbruik_10"){
    return(data_csv_array[9][3]);
  }
  if(var == "gas_verbruik_11"){
    return(data_csv_array[10][3]);
  }
  if(var == "gas_verbruik_12"){
    return(data_csv_array[11][3]);
  }
  if(var == "gas_verbruik_13"){
    return(data_csv_array[12][3]);
  }
  if(var == "gas_verbruik_14"){
    return(data_csv_array[13][3]);
  }
  if(var == "gas_verbruik_15"){
    return(data_csv_array[14][3]);
  }
  if(var == "gas_verbruik_16"){
    return(data_csv_array[15][3]);
  }
  if(var == "gas_verbruik_17"){
    return(data_csv_array[16][3]);
  }
  if(var == "gas_verbruik_18"){
    return(data_csv_array[17][17]);
  }
  if(var == "gas_verbruik_19"){
    return(data_csv_array[18][3]);
  }
  if(var == "gas_verbruik_20"){
    return(data_csv_array[19][3]);
  }
  if(var == "gas_verbruik_21"){
    return(data_csv_array[20][3]);
  }
  if(var == "gas_verbruik_22"){
    return(data_csv_array[21][3]);
  }
  if(var == "gas_verbruik_23"){
    return(data_csv_array[22][3]);
  }
  if(var == "gas_verbruik_24"){
    return(data_csv_array[23][3]);
  }
  if(var == "gas_verbruik_25"){
    return(data_csv_array[24][3]);
  }
  if(var == "gas_verbruik_26"){
    return(data_csv_array[25][3]);
  }
  if(var == "gas_verbruik_27"){
    return(data_csv_array[26][3]);
  }
  if(var == "gas_verbruik_28"){
    return(data_csv_array[27][3]);
  }
  if(var == "gas_verbruik_29"){
    return(data_csv_array[28][3]);
  }
  if(var == "gas_verbruik_30"){
    return(data_csv_array[29][3]);
  }
  if(var == "gas_verbruik_31"){
    return(data_csv_array[30][3]);
  }
  if(var == "gas_verbruik_32"){
    return(data_csv_array[31][3]);
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
    bool fout;
    float kw_on = 0.0;
    int uren_int;
    int minuten_int;
    if(request->hasParam(INPUT_KW_ON)){
      temp_string = ((request->getParam(INPUT_KW_ON)->value()) + String(terminator));
      temp_string.replace(',', '.');
      kw_on = temp_string.toFloat();
    }
    if(request->hasParam(INPUT_SCHAKEL_TIJD)){
      temp_string = ((request->getParam(INPUT_SCHAKEL_TIJD)->value()) + String(terminator));
      if(temp_string.length() == 6){
        uren_string = temp_string.substring(0, 2);
        minuten_string = temp_string.substring(3,5);
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
          relais1_on_float = pref.getFloat("relais1_on");
          relais1_override = pref.getString("relais1_ov");
          if(relais1_override == "1"){
            relais1_uit = true;
            uitsturen.relais = true;
            result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              //Serial.println("Met succes verzonden relais 1");
            }
            else {
              //Serial.println("fout bij verzenden naar relais 1");
            }
          }
          else{
            relais1_uit = false;
            uitsturen.relais = false;
            result = esp_now_send(broadcastAddress2, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              //Serial.println("Met succes verzonden relais 1");
            }
            else {
              //Serial.println("fout bij verzenden naar relais 1");
            }
          }
          break;
        case 1:
          pref.putFloat("relais2_on", kw_on);
          pref.putString("relais2_ov", override);
          relais2_on_float = pref.getFloat("relais2_on");
          relais2_override = pref.getString("relais2_ov");
          if(relais2_override == "1"){
            relais2_uit = true;
            uitsturen.relais = true;
            result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              //Serial.println("Met succes verzonden relais 2");
            }
            else {
              //Serial.println("fout bij verzenden naar relais 2");
            }
          }
          else{
            relais2_uit = false;
            uitsturen.relais = false;
            result = esp_now_send(broadcastAddress3, (uint8_t *) &uitsturen, sizeof(uitsturen));
            if (result == ESP_OK) {
              //Serial.println("Met succes verzonden relais 2");
            }
            else {
              //Serial.println("fout bij verzenden naar relais 2");
            }
          }
          break;
      }
    }
    if(request->hasParam(INPUT_PWM_1_KW)){
      temp_string = ((request->getParam(INPUT_PWM_1_KW)->value()) + String(terminator));
      temp_string.replace(',', '.');
      pwm1_kw_float = temp_string.toFloat();
    }
    if(request->hasParam(INPUT_PWM_1_TIJD_ON)){
      temp_string = ((request->getParam(INPUT_PWM_1_TIJD_ON)->value()) + String(terminator));
      if(temp_string.length() == 6){
        uren_string = temp_string.substring(0, 2);
        minuten_string = temp_string.substring(3,5);
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
     temp_string = ((request->getParam(INPUT_PWM_1_TIJD_OFF)->value()) + String(terminator));
     if(temp_string.length() == 6){
       uren_string = temp_string.substring(0, 2);
       minuten_string = temp_string.substring(3,5);
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
      temp_string = ((request->getParam(INPUT_PWM_2_KW)->value()) + String(terminator));
      temp_string.replace(',', '.');
      pwm2_kw_float = temp_string.toFloat();
    }
    if(request->hasParam(INPUT_PWM_2_DR)){
      temp_string = ((request->getParam(INPUT_PWM_2_DR)->value()) + String(terminator));
      temp_string.replace(',', '.');
      pwm2_dr_float = temp_string.toFloat();
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
      temp_string = ((request->getParam(INPUT_MACX_0)->value()) + String(terminator));
      temp_string.toCharArray(temp_char, (temp_string.length() +1));
      input_macx_0 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_1)){
      temp_string = ((request->getParam(INPUT_MACX_1)->value()) + String(terminator));
      temp_string.toCharArray(temp_char, (temp_string.length() +1));
      input_macx_1 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_2)){
      temp_string = ((request->getParam(INPUT_MACX_2)->value()) + String(terminator));
      temp_string.toCharArray(temp_char, (temp_string.length() +1));
      input_macx_2 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_3)){
      temp_string = ((request->getParam(INPUT_MACX_3)->value()) + String(terminator));
      temp_string.toCharArray(temp_char, (temp_string.length() +1));
      input_macx_3 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_4)){
      temp_string = ((request->getParam(INPUT_MACX_4)->value()) + String(terminator));
      temp_string.toCharArray(temp_char, (temp_string.length() +1));
      input_macx_4 = strtol(temp_char, 0, 16);
    }
    if(request->hasParam(INPUT_MACX_5)){
      temp_string = ((request->getParam(INPUT_MACX_5)->value()) + String(terminator));
      temp_string.toCharArray(temp_char, (temp_string.length() +1));
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
      temp_string = "";
      fout = false;
      temp_string = temp_string + String(input_macx_0)+ String(input_macx_1)+ String(input_macx_2)
                  + String(input_macx_3)+ String(input_macx_4)+ String(input_macx_5);
      if(broadcastAddress1_string == temp_string){
        fout = true;
      }
      if(broadcastAddress2_string == temp_string){
        fout = true;
      }
      if(broadcastAddress3_string == temp_string){
        fout = true;
      }
      if(broadcastAddress4_string == temp_string){
        fout = true;
      }
      if(broadcastAddress5_string == temp_string){
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
        temp_string = ((request->getParam(INPUT_DATA_DAG)->value()) + String(terminator));
        data_dag_int = temp_string.toInt();
      }
      if(request->hasParam(INPUT_DATA_MAAND)){
        temp_string = ((request->getParam(INPUT_DATA_MAAND)->value()) + String(terminator));
        data_maand_int = temp_string.toInt();
      }
      if(request->hasParam(INPUT_DATA_JAAR)){
        temp_string = ((request->getParam(INPUT_DATA_JAAR)->value()) + String(terminator));
        data_jaar_int = temp_string.toInt();
      }
      if(request->hasParam(BEVESTIG_PERIODE)){
        reset_data_csv_array();
        memset(periode_char, 0, sizeof(periode_char));
        memset(eenheid_char, 0, sizeof(eenheid_char));
        if((data_dag_int != 0) && (data_maand_int != 0) && (data_jaar_int != 0)){
          strcpy(periode_char, "/");
          strcat(periode_char, to_string(data_jaar_int).c_str());
          strcat(periode_char, "/maandfile_");
          strcat(periode_char, to_string(data_maand_int).c_str());
          strcpy(eenheid_char, "uur");
          lees_uur_csv(SD, periode_char, data_dag_int);
          memset(periode_char, 0, sizeof(periode_char));
          strcpy(periode_char, to_string(data_dag_int).c_str());
          strcat(periode_char, "/");
          strcat(periode_char, to_string(data_maand_int).c_str());
          strcat(periode_char, "/");
          strcat(periode_char, to_string(data_jaar_int).c_str());
        }
        if((data_dag_int == 0) && (data_maand_int != 0) && (data_jaar_int != 0)){
          strcpy(periode_char, "/");
          strcat(periode_char, to_string(data_jaar_int).c_str());
          strcat(periode_char, "/maandfile_");
          strcat(periode_char, to_string(data_maand_int).c_str());
          strcpy(eenheid_char, "dag");
          lees_dag_csv(SD, periode_char);
          memset(periode_char, 0, sizeof(periode_char));
          strcpy(periode_char, to_string(data_maand_int).c_str());
          strcat(periode_char, "/");
          strcat(periode_char, to_string(data_jaar_int).c_str());
        }
        if((data_dag_int == 0) && (data_maand_int == 0) && (data_jaar_int != 0)){
          strcpy(periode_char, "/");
          strcat(periode_char, to_string(data_jaar_int).c_str());
          strcat(periode_char, "/maanddata");
          strcpy(eenheid_char, "maand");
          lees_maand_csv(SD, periode_char);
          memset(periode_char, 0, sizeof(periode_char));
          strcpy(periode_char, to_string(data_jaar_int).c_str());
        }
        if((data_dag_int == 0) && (data_maand_int == 0) && (data_jaar_int == 0)){
          String temp_string = "vorige jaren (max 25)";
          strcpy(periode_char, "/jaar_data/jaardata");
          strcpy(eenheid_char, "jaar");
          lees_jaar_csv(SD, periode_char);
          memset(periode_char, 0, sizeof(periode_char));
          strcpy(periode_char, "laatste 25 jaar");
        }
      }
  });
}
