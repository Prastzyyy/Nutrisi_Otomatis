#include <ArduinoOTA.h>
#include <Ticker.h>
//#include <WiFi.h>         //esp32
#include <ESP8266WiFi.h>
//#include <HTTPClient.h>   //esp32
#include <ESP8266HTTPClient.h>  
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
//#include <LiquidCrystal_I2C.h>
//#include <RTClib.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include "fuzzy_function.h"

// Sistem OTA
const char* ota_password = "123";
String OTA_msg = "Update #1";

// Google Apps Script 
String Web_App_URL = "https://script.google.com/macros/s/AKfycbwVtR04NDLnKFMrjlx2EHCEvLHktAMAuHcxUE4rBzrnD1UwdEL0UoegfaXF4j328ZQHUQ/exec";
String status;
String warning;
int dataSpreadsheet;
void spreadsheet();

// MQTT
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_user = "123"; 
const char* mqtt_password = "123"; 

const char* SUBTOPIC_SETPH = "greenhouse/input/setph";
const char* SUBTOPIC_SETPPM = "greenhouse/input/setppm";
const char* SUBTOPIC_PENYIRAM = "greenhouse/input/penyiram";
const char* SUBTOPIC_MIXER = "greenhouse/input/mixer";
const char* SUBTOPIC_PHUP = "greenhouse/input/phup";
const char* SUBTOPIC_PHDOWN = "greenhouse/input/phdown";
const char* SUBTOPIC_NUTRISI = "greenhouse/input/nutrisi";
const char* SUBTOPIC_AMBILSAMPEL = "greenhouse/input/ambilsampel";
const char* SUBTOPIC_BUANGSAMPEL = "greenhouse/input/buangsampel";
const char* SUBTOPIC_MAINTASK = "greenhouse/input/maintask";  //Otomatisasi keseluruhan
const char* SUBTOPIC_OTOMATISPH = "greenhouse/input/otomatisph";  //Otomatisasi pH
const char* SUBTOPIC_OTOMATISNUTRISI = "greenhouse/input/otomatisnutrisi"; //Otomatisasi Nutrisi
const char* SUBTOPIC_GANTISAMPEL = "greenhouse/input/gantisampel";  //Otomatisasi Ganti Sampel
const char* SUBTOPIC_TEST = "greenhouse/input/test";

const char* PUBTOPIC_PH = "greenhouse/output/ph";
const char* PUBTOPIC_PPM = "greenhouse/output/ppm";
const char* PUBTOPIC_TINGGI = "greenhouse/output/tinggi";
const char* PUBTOPIC_SUHU = "greenhouse/output/suhu";
const char* PUBTOPIC_WAKTU = "greenhouse/output/waktu";
const char* PUBTOPIC_AKTUATOR1 = "greenhouse/output/aktuator1";
const char* PUBTOPIC_AKTUATOR2 = "greenhouse/output/aktuator2";
const char* PUBTOPIC_AKTUATOR3 = "greenhouse/output/aktuator3";
const char* PUBTOPIC_AKTUATOR4 = "greenhouse/output/aktuator4";
const char* PUBTOPIC_AKTUATOR5 = "greenhouse/output/aktuator5";
const char* PUBTOPIC_AKTUATOR6 = "greenhouse/output/aktuator6";
const char* PUBTOPIC_AKTUATOR7 = "greenhouse/output/aktuator7";
const char* PUBTOPIC_AKTUATOR8 = "greenhouse/output/aktuator8";
const char* PUBTOPIC_OTA = "greenhouse/output/OTA";
const char* PUBTOPIC_WARNING = "greenhouse/output/warning";
const char* PUBTOPIC_STATUS_SISTEM = "greenhouse/output/status";

// Koneksi WiFi
//const char* ssid ="bebas";
//const char* password = "akunulisaja";
const char* ssid ="Prastzy.net";
const char* password = "123456781";

// Deklarasi Object
Ticker task;
WiFiClient espClient;
PubSubClient client(espClient);
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//LiquidCrystal_I2C lcd(0x27,20,4);
//RTC_DS1307 rtc;

// Deklarasi Pin
//#define ONE_WIRE_BUS 0
//#define pin_pH D4//39 
//#define pin_EC D4//36  
//#define PIN_TRIG D4//2
//#define PIN_ECHO D4//15
#define relay_pHup D4//17   
#define relay_pHdn D4//12  
#define relay_nutrisi D4//26  
#define relay_ambilSampel D4//5  
#define relay_buangSampel D4//18  
#define relay_penyiram D4//14   
#define relay_mixer D4//27  

// Adjust nilai variabel
int setWaktu1 [] = {8,0,0};  //waktu penyiraman
int delay_pompaSampel = 3000;
int delay_mixer = 3000;
int delay_penyiram = 3000;

int menit; //sebagai simulasi menit
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
String aktuator1 = "Padam", aktuator2 = "Padam", aktuator3 = "Padam", aktuator4 = "Padam";
String aktuator5 = "Padam", aktuator6 = "Padam", aktuator7 = "Padam", aktuator8 = "Padam";
float set_jarak = 100, set_pH = 7, set_PPM = 800;
float pH, PPM, suhu, tinggi_cm, jarak_cm,;
int duration;
bool kondisi1 = false, kondisi2 = false, kondisi3 = false, kondisi4 = false; //Saklar Otomatisasi

// Deklarasi Prosedur
void mainTask();
void baca_pH();
void baca_PPM();
void baca_jarak();
void baca_suhu();
void kontrol_pH();
void kontrol_PPM();
void kontrol_tinggi();
void baca_RTC();
void outputFuzzy();
void setup_wifi();
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);
void monitoring ();
void gantiSampel();

void setup() {
  //pinMode(pin_pH, INPUT);
  //pinMode(pin_EC, INPUT);
  //pinMode(PIN_ECHO, INPUT);
  //pinMode(PIN_TRIG, OUTPUT);
  pinMode(relay_pHup, OUTPUT);
  pinMode(relay_pHdn, OUTPUT);
  pinMode(relay_nutrisi, OUTPUT);
  pinMode(relay_mixer, OUTPUT);
  pinMode(relay_penyiram, OUTPUT);
  pinMode(relay_ambilSampel, OUTPUT);
  pinMode(relay_buangSampel, OUTPUT); 
  
  Serial.begin(115200);
  //sensors.begin();
  setup_wifi();
  ArduinoOTA.setHostname("esp8266 - Nutrisi Otomatis");
  ArduinoOTA.setPassword(ota_password);
  ArduinoOTA.begin();  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  task.attach(3, monitoring);
  /*
  // LCD Setup                  
  lcd.backlight();
  lcd.setCursor(3,1);
  lcd.print("START  PROGRAM");
  lcd.setCursor(3,2);
  lcd.print("==============");
  delay(1000);
  lcd.clear();
  
  if (! rtc.begin()) {
    Serial.println("RTC tidak ditemukan");
    while (1);
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC tidak berjalan, setting waktu...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set waktu sesuai waktu kompilasi
  }
  */
}

void loop() { 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
  mainTask ();
  if(kondisi2){
    kontrol_pH();
    kondisi2 = false;
  }
  if(kondisi3){
    kontrol_PPM();
    kondisi3 = false;
  }
  if(kondisi4){
    gantiSampel();
    kondisi4 = false;
  }
}

// Prosedur Utama
void mainTask (){
  //DateTime now = rtc.now();
  //if (now.hour() == setWaktu1[0] && now.minute() == setWaktu1[1] && now.second() == setWaktu1[2] ){
  //if (now.hour() == setWaktu1[0] && now.minute() == setWaktu1[1] && setWaktu1[2] <= now.second() <= setWaktu1[2]+5 ){
  //if (now.minute() == menit && now.second() == 1){   //Simulasi RTC dengan manipulasi menit
  if (kondisi1 == true){ //pengganti rtc 
    Serial.println("==================================");
    Serial.println("kondisi didalam mainTask ");
    Serial.println("==================================");
 
    gantiSampel();

    // Pengambilan data sensor sebelum diotomatisasi
    status = "Sebelum";
    spreadsheet();

    kontrol_pH();
    kontrol_PPM(); 

    //Mengaktifkan Mixer  
    Serial.println("Mengaktifkan relay_mixer");
    digitalWrite (relay_mixer, LOW);  
    aktuator4 = "Menyala";
    Serial.print("delay ");Serial.print(delay_mixer);Serial.println(" detik");
    delay(delay_mixer);
    Serial.println("Menonaktifkan relay_mixer");
    digitalWrite (relay_mixer, HIGH); 
    aktuator4 = "Padam";
    yield();

    gantiSampel();

    // Pengambilan data sensor setelah diotomatisasi
    status = "Sesudah";
    spreadsheet();

    //Mengaktifkan Penyiram  
    Serial.println("Mengaktifkan relay_penyiram");
    digitalWrite (relay_penyiram, LOW); 
    aktuator5 = "Menyala";
    Serial.print("delay ");Serial.print(delay_penyiram);Serial.println(" detik");
    delay(delay_penyiram);
    Serial.println("Menonaktifkan relay_penyiram");
    digitalWrite (relay_penyiram, HIGH); 
    aktuator5 = "Padam";
    yield();

    String statusSistem = "Otomatisasi Berhasil";
    client.publish(PUBTOPIC_STATUS_SISTEM, statusSistem.c_str());
  }
  else {
    Serial.println("==================================");
    Serial.println("kondisi diluar mainTask");
    Serial.println("==================================");
  }
  delay(1000);
}

void monitoring(){
  baca_pH(); 
  baca_PPM(); 
  baca_jarak(); 
  baca_suhu();  
  outputFuzzy(); 
  baca_RTC();
  //DateTime now = rtc.now(); 
  /*
  // Print Serial Monitor
  Serial.print("pH : "); Serial.println(pH);
  Serial.print("EC : "); Serial.println(PPM);
  Serial.print("Suhu : "); Serial.println(suhu);
  Serial.print("Ketinggian Air (cm): "); Serial.println(tinggi_cm);

  // Print LCD
  lcd.setCursor(0,1); 
  lcd.print("nilai pH : "); lcd.print(pH);
  lcd.setCursor(0,2); 
  lcd.print("nilai EC : "); lcd.print(PPM);
  lcd.setCursor(0,3); 
  lcd.print("Nilai Suhu : "); lcd.print(suhu); lcd.print(" C");
  lcd.clear();
  */
   
  // Print MQTT
  baca_RTC();
  client.publish(PUBTOPIC_PH, String(pH).c_str());
  client.publish(PUBTOPIC_PPM, String(PPM).c_str());
  client.publish(PUBTOPIC_SUHU, String(suhu).c_str());
  client.publish(PUBTOPIC_TINGGI, String(tinggi_cm).c_str());
  client.publish(PUBTOPIC_AKTUATOR1, aktuator1.c_str());
  client.publish(PUBTOPIC_AKTUATOR2, aktuator2.c_str());
  client.publish(PUBTOPIC_AKTUATOR3, aktuator3.c_str());
  client.publish(PUBTOPIC_AKTUATOR4, aktuator4.c_str());
  client.publish(PUBTOPIC_AKTUATOR5, aktuator5.c_str());
  client.publish(PUBTOPIC_AKTUATOR6, aktuator6.c_str());
  client.publish(PUBTOPIC_AKTUATOR7, aktuator7.c_str());
  client.publish(PUBTOPIC_AKTUATOR8, aktuator8.c_str());

  // Sistem Peringatan data SpreadSheet
  if (dataSpreadsheet > 1000){
    warning = "Data sudah terlalu banyak";
  }else{
    warning = String (dataSpreadsheet);  
  }
  client.publish(PUBTOPIC_WARNING, warning.c_str());
}

void gantiSampel (){
  Serial.println("Mengaktifkan relay_buangSampel");
  digitalWrite (relay_buangSampel, LOW); 
  aktuator3 = "Menyala";
  Serial.print("delay ");Serial.print(delay_pompaSampel);Serial.println(" detik");
  delay(delay_pompaSampel);
  Serial.println("Menonaktifkan relay_buangSampel");
  digitalWrite (relay_buangSampel, HIGH); 
  aktuator3 = "Padam";
  yield();

  Serial.println("Mengaktifkan relay_ambilSampel");
  digitalWrite (relay_ambilSampel, LOW); 
  aktuator2 = "Menyala";
  Serial.print("delay ");Serial.print(delay_pompaSampel);Serial.println(" detik");
  delay(delay_pompaSampel);
  Serial.println("Menonaktifkan relay_ambilSampel");
  digitalWrite (relay_ambilSampel, HIGH); 
  aktuator2 = "Padam";
  yield();  
}

void baca_pH(){
  /*
  
  code sensor pH
  
  */
  // Angka random dari 5 sampai 9 dengan interval 0,1
  int randomNumber = random(0, 41);  
  pH = 5.0 + (randomNumber * 0.1);
  //==============================================
  Error_pH = abs(set_pH - pH);
}

void kontrol_pH(){
  baca_pH();
  outputFuzzy();
  baca_jarak();
  Serial.println("menjalankan kontrol_pH");
  if (pH < set_pH  ){
    Serial.println("Mengaktifkan relay_pHup");
    digitalWrite(relay_pHup, LOW);
    aktuator6 = "Menyala";
    digitalWrite(relay_pHdn, HIGH);
    aktuator7 = "Padam";
    Serial.print("delay : ");
    Serial.print(lamaPompa_pH*(tinggi_cm/set_jarak)); 
    Serial.println(" detik");
    delay(lamaPompa_pH*1000*(tinggi_cm/set_jarak));
    Serial.println("Menonaktifkan relay_pHup");
    digitalWrite(relay_pHup, HIGH);
    aktuator6 = "Padam";
    digitalWrite(relay_pHdn, HIGH);
    aktuator7 = "Padam";
    yield();
  }
  else if (pH >= set_pH  ){
    Serial.println("Mengaktifkan relay_pHdn");
    digitalWrite(relay_pHup, HIGH);
    aktuator6 = "Padam";
    digitalWrite(relay_pHdn, LOW);
    aktuator7 = "Menyala";
    Serial.print("delay : ");
    Serial.print(lamaPompa_pH*(tinggi_cm/set_jarak)); 
    Serial.println(" detik");
    delay(lamaPompa_pH*1000*(tinggi_cm/set_jarak));
    Serial.println("Menonaktifkan relay_pHdn");
    digitalWrite(relay_pHup, HIGH);
    aktuator6 = "Padam";
    digitalWrite(relay_pHdn, HIGH);
    aktuator7 = "Padam";
    yield();
  }
}

void baca_PPM(){
  /*
  
  *code sensor EC
  
  */
  // Angka random dari 550 sampai 1051
  PPM = random(550, 1051); 
  //==================================
  if (PPM <= set_PPM){
    Error_PPM = set_PPM - PPM;
  }
  else if (PPM > set_PPM){
    Error_PPM = 0;
  }
}

void kontrol_PPM(){
  baca_PPM();
  outputFuzzy();
  baca_jarak();
  Serial.println("menjalankan kontrol_PPM");
  if (PPM < set_PPM  ){
    Serial.println("Mengaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, LOW);
    aktuator8 = "Menyala";
    Serial.print("delay : ");
    Serial.print(lamaPompa_PPM*(tinggi_cm/set_jarak)); 
    Serial.println(" detik");
    delay(lamaPompa_PPM*1000*(tinggi_cm/set_jarak));
    Serial.println("Menonaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, HIGH);
    aktuator8 = "Padam";
    yield();
  }
  else if (PPM >= set_PPM  ){
    Serial.println("Menonaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, HIGH);
    aktuator8 = "Padam";
  }
}

void baca_jarak(){
  /*
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  duration = pulseIn(PIN_ECHO, HIGH);
  jarak_cm = duration / 58; //dalam cm
  */
  // Angka random dari 0 sampai 21
  jarak_cm  = random(0, 21); 
  //==============================
  tinggi_cm = set_jarak - jarak_cm;
}

void baca_suhu(){
  /*
  sensors.requestTemperatures(); 
  suhu = sensors.getTempCByIndex(0);
  */
  // Angka random dari 25 sampai 35
  suhu = random(25, 35); 
  //==============================
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Receive Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';  
  Serial.println(msg);

  // kontrol Manual Penyiram
  if (!strcmp(topic, SUBTOPIC_PENYIRAM)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_penyiram, LOW);
      Serial.println("Penyiram manual ON");
      aktuator5 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_penyiram, HIGH);
      Serial.println("Penyiram manual OFF");
      aktuator5 = "Padam";
    }
  }
  // kontrol Manual Mixer
  if (!strcmp(topic, SUBTOPIC_MIXER)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_mixer, LOW);
      Serial.println("Mixer ON");
      aktuator4 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_mixer, HIGH);
      Serial.println("Mixer OFF");
      aktuator4 = "Padam";
    }
  }
  // kontrol Manual pH Up
  if (!strcmp(topic, SUBTOPIC_PHUP)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_pHup, LOW);
      Serial.println("Pompa pH Up ON");
      aktuator6 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_pHup, HIGH);
      Serial.println("Pompa pH Up OFF");
      aktuator6 = "Padam";
    }
  }
  // kontrol Manual pH Down
  if (!strcmp(topic, SUBTOPIC_PHDOWN)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_pHdn, LOW);
      Serial.println("Pompa pH Down ON");
      aktuator7 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_pHdn, HIGH);
      Serial.println("Pompa pH Down OFF");
      aktuator7 = "Padam";
    }
  }
  // kontrol Manual Nutrisi
  if (!strcmp(topic, SUBTOPIC_NUTRISI)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_nutrisi, LOW);
      Serial.println("Pompa Nutrisi ON");
      aktuator8 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_nutrisi, HIGH);
      Serial.println("Pompa Nutrisi OFF");
      aktuator8 = "Padam";
    }
  }
  // kontrol Manual Ambil Sampel
  if (!strcmp(topic, SUBTOPIC_AMBILSAMPEL)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_ambilSampel, LOW);
      Serial.println("Pompa Ambil Sampel ON");
      aktuator2 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_ambilSampel, HIGH);
      Serial.println("Pompa Ambil Sampel OFF");
      aktuator2 = "Padam";
    }
  }
  // kontrol Manual Buang Sampel
  if (!strcmp(topic, SUBTOPIC_BUANGSAMPEL)) {
    if (!strncmp(msg, "on", length)) {
      digitalWrite(relay_buangSampel, LOW);
      Serial.println("Pompa Buang Sampel ON");
      aktuator3 = "Menyala";
    } else if (!strncmp(msg, "off", length)) {
      digitalWrite(relay_buangSampel, HIGH);
      Serial.println("Pompa Buang Sampel OFF");
      aktuator3 = "Padam";
    }
  }

  // kontrol Otomatisasi mainTask
  if (!strcmp(topic, SUBTOPIC_MAINTASK)) {
    if (!strncmp(msg, "on", length)) {
      kondisi1 = true;
    } else if (!strncmp(msg, "off", length)) {
      kondisi1 = false;
    }
  }

  // kontrol Otomatisasi pH
  if (!strcmp(topic, SUBTOPIC_OTOMATISPH)) {
    if (!strncmp(msg, "on", length)) {
      kondisi2 = true;
    } else if (!strncmp(msg, "off", length)) {
      kondisi2 = false;
    }
  }

  // kontrol Otomatisasi Nutrisi
  if (!strcmp(topic, SUBTOPIC_OTOMATISNUTRISI)) {
    if (!strncmp(msg, "on", length)) {
      kondisi3 = true;
    } else if (!strncmp(msg, "off", length)) {
      kondisi3 = false;
    }
  }

  // kontrol Otomatisasi Ganti Sampel
  if (!strcmp(topic, SUBTOPIC_GANTISAMPEL)) {
    if (!strncmp(msg, "on", length)) {
      kondisi4 = true;
    } else if (!strncmp(msg, "off", length)) {
      kondisi4 = false;
    }
  }

  // Set pH
  if (!strcmp(topic, SUBTOPIC_SETPH)) {
    set_pH = atof(msg); 
    Serial.print("Set pH menjadi : ");
    Serial.println(set_pH);
  }

  // Set PPM
  if (!strcmp(topic, SUBTOPIC_SETPPM)) {
    set_PPM = atof(msg); 
    Serial.print("Set PPM menjadi : ");
    Serial.println(set_PPM);
  }
  
  // Set Waktu
  /*
  if (!strcmp(topic, SUBTOPIC_TEST)) {
    menit = atoi(msg); 
    Serial.print("Set menit menjadi : ");
    Serial.println(menit);
  }
  */
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "esp32-clientId-";
    clientId += String(random(0xffff), HEX);
    //if (client.connect(clientId.c_str())) {
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Connected");
      Serial.print("Status OTA : ");
      Serial.println(OTA_msg);
      client.publish(PUBTOPIC_OTA, OTA_msg.c_str());
      client.subscribe(SUBTOPIC_PENYIRAM);
      client.subscribe(SUBTOPIC_MIXER);
      client.subscribe(SUBTOPIC_PHUP);
      client.subscribe(SUBTOPIC_PHDOWN);
      client.subscribe(SUBTOPIC_NUTRISI);
      client.subscribe(SUBTOPIC_AMBILSAMPEL);
      client.subscribe(SUBTOPIC_BUANGSAMPEL);
      client.subscribe(SUBTOPIC_SETPH);
      client.subscribe(SUBTOPIC_SETPPM);
      client.subscribe(SUBTOPIC_MAINTASK);
      client.subscribe(SUBTOPIC_GANTISAMPEL);
      client.subscribe(SUBTOPIC_OTOMATISPH);
      client.subscribe(SUBTOPIC_OTOMATISNUTRISI);
      client.subscribe(SUBTOPIC_TEST);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}

void baca_RTC(){
  /*
  // Print waktu di MQTT
  DateTime now = rtc.now();
  String pesan = ">> ";
  pesan += String(now.year());
  pesan += '/';
  pesan += String(now.month());
  pesan += '/';
  pesan += String(now.day());
  pesan += " (";
  pesan += daysOfTheWeek[now.dayOfTheWeek()];
  pesan += ") - ";
  pesan += String(now.hour());
  pesan += ':';
  pesan += String(now.minute());
  pesan += ':';
  pesan += String(now.second());
  Serial.println(pesan);  // Kirimkan pesan ke serial monitor
  client.publish(PUBTOPIC_WAKTU, pesan.c_str());  // Kirimkan pesan ke MQTT


  // Print waktu di LCD
  lcd.setCursor(0,0);
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print(" - ");
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  */
}

void outputFuzzy() {
  baca_pH();
  baca_PPM();
  Momen();
  lamaPompa_pH = deffuzzyfikasi_pH();
  lamaPompa_PPM = deffuzzyfikasi_PPM();
}

void spreadsheet(){
  if (WiFi.status() == WL_CONNECTED) {
    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&pH=" + String (pH);
    Send_Data_URL += "&PPM=" + String(PPM);
    Send_Data_URL += "&tinggi_cm=" + String(tinggi_cm);
    Send_Data_URL += "&suhu=" + String(suhu);
    Send_Data_URL += "&status=" + status;
    Serial.println();
    Serial.println("-------------");
    Serial.println("Send data to Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);

    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;
    http.begin(client, Send_Data_URL); 
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
    int httpCode = http.GET(); 
    http.end();
    
    String Read_Data_URL = Web_App_URL + "?sts=read";
    http.begin(client, Read_Data_URL); 
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    httpCode = http.GET();
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Data ke : " + payload);
      dataSpreadsheet = payload.toInt(); 
    }
    http.end();
    Serial.println("-------------");
  }
}
