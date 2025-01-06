#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
//#include WebServer???     //esp32
#include <ESP8266mDNS.h>
//#include <ESPmDNS>          //esp32
#include <Ticker.h>
//#include <WiFi.h>         //esp32
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  
//#include <HTTPClient.h>   //esp32
#include <WiFiClientSecure.h>
//#include <LiquidCrystal_I2C.h>
//#include <RTClib.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include "fuzzy_function.h"
#include "web.h"

ESP8266WebServer server(80);

// Sistem OTA
const char* ota_password = "123";
void setupOTA();
String OTA_msg = "Update #1";

// Google Apps Script 
String Web_App_URL = "https://script.google.com/macros/s/AKfycbwVtR04NDLnKFMrjlx2EHCEvLHktAMAuHcxUE4rBzrnD1UwdEL0UoegfaXF4j328ZQHUQ/exec";
String status;
String warning;
int dataSpreadsheet;
void spreadsheet();

// Koneksi WiFi
//const char* ssid ="bebas";
//const char* password = "akunulisaja";
const char* ssid ="Prastzy.net";
const char* password = "123456781";

//Deklarasi Pin
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
#define relay_tangki D4//19  
#define relay_mixer D4//27  

// Deklarasi Library
//#define ONE_WIRE_BUS 0
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//LiquidCrystal_I2C lcd(0x27,20,4);
//RTC_DS1307 rtc;

// Adjust nilai variabel
int setWaktu1 [] = {8,0,0};  //waktu penyiraman
int delay_pompaSampel = 3000;
int delay_mixer = 3000;
int delay_penyiram = 3000;

int menit; //sebagai simulasi menit
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String aktuator1 = "Padam", aktuator2 = "Padam", aktuator3 = "Padam", aktuator4 = "Padam";
String aktuator5 = "Padam", aktuator6 = "Padam", aktuator7 = "Padam", aktuator8 = "Padam";
float set_jarak = 100, set_pH = 7, set_PPM = 800;
float pH, PPM, suhu, tinggi_cm, jarak_cm;
int duration;
bool kondisi = false; //sebagai simulasi RTC
bool saklar = false;  //kontrol manual otomatisasi 
String kondisiOtomatisasi = "Tidak Berjalan";

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
void handleRoot();
void handleNotFound();
void handleData();

Ticker task;
void monitoring ();

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
  pinMode(relay_tangki, OUTPUT);
  pinMode(relay_ambilSampel, OUTPUT);
  pinMode(relay_buangSampel, OUTPUT); 
  
  Serial.begin(115200);
  //sensors.begin();
  setup_wifi();
  ArduinoOTA.setHostname("esp8266 - Nutrisi Otomatis");
  ArduinoOTA.setPassword(ota_password);
  ArduinoOTA.begin();  
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

  server.on("/", [](){
    handleRoot();
  });
  
  server.on("/otomatisasi_on", [](){
    Serial.println("");
    Serial.print("/otomatisasi_on"); 
    Serial.println("");
    saklar = true;
    handleRoot();
  });
  
  server.on("/data", handleData);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() { 
  ArduinoOTA.handle();
  server.handleClient();
  mainTask ();
}

// Prosedur Utama
void mainTask (){
  //DateTime now = rtc.now();
  //if (now.hour() == setWaktu1[0] && now.minute() == setWaktu1[1] && now.second() == setWaktu1[2] ){
  //if (now.hour() == setWaktu1[0] && now.minute() == setWaktu1[1] && setWaktu1[2] <= now.second() <= setWaktu1[2]+5 ){
  //if (now.minute() == menit && now.second() == 1){   //Simulasi RTC dengan manipulasi menit
  if (kondisi == true || saklar == true){ //pengganti rtc 
    server.handleClient();
    kondisiOtomatisasi = "Sedang Berjalan";
    Serial.println("==================================");
    Serial.println("kondisi didalam mainTask ");
    Serial.println("==================================");

    Serial.println("menjalankan kontrol_tinggi ");
    kontrol_tinggi();
    kondisiOtomatisasi = "Sedang Berjalan 10%";
    server.handleClient();
    yield();

    Serial.println("Mengaktifkan relay_buangSampel");
    digitalWrite (relay_buangSampel, LOW); 
    aktuator3 = "Menyala";
    Serial.print("delay ");Serial.print(delay_pompaSampel);Serial.println(" detik");
    delay(delay_pompaSampel);
    Serial.println("Menonaktifkan relay_buangSampel");
    digitalWrite (relay_buangSampel, HIGH); 
    aktuator3 = "Padam";
    kondisiOtomatisasi = "Sedang Berjalan 20 %";
    server.handleClient();
    yield();

    Serial.println("Mengaktifkan relay_ambilSampel");
    digitalWrite (relay_ambilSampel, LOW); 
    aktuator2 = "Menyala";
    Serial.print("delay ");Serial.print(delay_pompaSampel);Serial.println(" detik");
    delay(delay_pompaSampel);
    Serial.println("Menonaktifkan relay_ambilSampel");
    digitalWrite (relay_ambilSampel, HIGH); 
    aktuator2 = "Padam";
    kondisiOtomatisasi = "Sedang Berjalan 30%";server.handleClient();
    
    yield();

    // Pengambilan data sensor sebelum diotomatisasi
    status = "Sebelum";
    spreadsheet();

    Serial.println("menjalankan kontrol_pH");
    kontrol_pH();
    kondisiOtomatisasi = "Sedang Berjalan 40%";
    server.handleClient();
    yield();
    Serial.println("menjalankan kontrol_PPM");
    kontrol_PPM(); 
    kondisiOtomatisasi = "Sedang Berjalan 50%";
    server.handleClient();
    yield();
      
    Serial.println("Mengaktifkan relay_mixer");
    digitalWrite (relay_mixer, LOW);  
    aktuator4 = "Menyala";
    Serial.print("delay ");Serial.print(delay_mixer);Serial.println(" detik");
    delay(delay_mixer);
    Serial.println("Menonaktifkan relay_mixer");
    digitalWrite (relay_mixer, HIGH); 
    aktuator4 = "Padam";
    kondisiOtomatisasi = "Sedang Berjalan 60%";
    server.handleClient();
    yield();

    Serial.println("Mengaktifkan relay_buangSampel");
    digitalWrite (relay_buangSampel, LOW); 
    aktuator3 = "Menyala";
    Serial.print("delay ");Serial.print(delay_pompaSampel);Serial.println(" detik");
    delay(delay_pompaSampel);
    Serial.println("Menonaktifkan relay_buangSampel");
    digitalWrite (relay_buangSampel, HIGH); 
    aktuator3 = "Padam";
    kondisiOtomatisasi = "Sedang Berjalan 70%";
    server.handleClient();
    yield();

    Serial.println("Mengaktifkan relay_ambilSampel");
    digitalWrite (relay_ambilSampel, LOW); 
    aktuator2 = "Menyala";
    Serial.print("delay ");Serial.print(delay_pompaSampel);Serial.println(" detik");
    delay(delay_pompaSampel);
    Serial.println("Menonaktifkan relay_ambilSampel");
    digitalWrite (relay_ambilSampel, HIGH);  
    aktuator2 = "Padam";
    kondisiOtomatisasi = "Sedang Berjalan 90%";
    server.handleClient();
    yield();

    // Pengambilan data sensor setelah diotomatisasi
    status = "Sesudah";
    spreadsheet();
      
    Serial.println("Mengaktifkan relay_penyiram");
    digitalWrite (relay_penyiram, LOW); 
    aktuator5 = "Menyala";
    Serial.print("delay ");Serial.print(delay_penyiram);Serial.println(" detik");
    delay(delay_penyiram);
    Serial.println("Menonaktifkan relay_penyiram");
    digitalWrite (relay_penyiram, HIGH); 
    aktuator5 = "Padam";
    kondisiOtomatisasi = "Sedang Berjalan 100%";
    server.handleClient();
    yield();

    Serial.println("menjalankan kontrol_tinggi ");
    kontrol_tinggi();
    
    kondisiOtomatisasi = "Otomatisasi Berhasil pada ";
    String sekarang = "08:10:43";
    kondisiOtomatisasi += sekarang;
    saklar = false;
  }
  else {
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    Serial.println("==================================");
    Serial.println("kondisi diluar mainTask");
    Serial.println("Alamat ip : ");Serial.println(ipStr);
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
  Serial.print("Output fuzzy pH : "); Serial.println(lamaPompa_pH);
  Serial.print("Output fuzzy nutrisi : "); Serial.println(lamaPompa_PPM);
  Serial.println("");

  // Print LCD
  lcd.setCursor(0,0); 
  lcd.print("Alamat IP : "); lcd.print(ipStr);
  lcd.setCursor(0,1); 
  lcd.print("nilai pH : "); lcd.print(pH);
  lcd.setCursor(0,2); 
  lcd.print("nilai EC : "); lcd.print(PPM);
  lcd.setCursor(0,3); 
  lcd.print("Nilai Suhu : "); lcd.print(suhu); lcd.print(" C");
  lcd.clear();
  */
}

void baca_pH(){
  /*
  
  code sensor pH
  
  */
  // Angka random dari 5 sampai 9 dengan interval 0,1
  int randomNumber = random(0, 41);  
  pH = 5.0 + (randomNumber * 0.1);

  Error_pH = abs(set_pH - pH);
}

void kontrol_pH(){
  baca_pH();
  outputFuzzy();
  if (pH < set_pH  ){
    Serial.println("Mengaktifkan relay_pHup");
    digitalWrite(relay_pHup, LOW);
    aktuator6 = "Menyala";
    digitalWrite(relay_pHdn, HIGH);
    aktuator7 = "Padam";
    Serial.print("delay : ");Serial.print(lamaPompa_pH); Serial.println(" detik");
    delay(lamaPompa_pH*1000);
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
    Serial.print("delay : ");Serial.print(lamaPompa_pH); Serial.println(" detik");
    delay(lamaPompa_pH*1000);
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
  if (PPM < set_PPM  ){
    Serial.println("Mengaktifkan relay_nutrisi");
    digitalWrite(relay_nutrisi, LOW);
    aktuator8 = "Menyala";
    Serial.print("delay : ");Serial.print(lamaPompa_PPM); Serial.println(" detik");
    delay(lamaPompa_PPM*1000);
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
  //digitalWrite(PIN_TRIG, HIGH);
  //delayMicroseconds(10);
  //digitalWrite(PIN_TRIG, LOW);
  //duration = pulseIn(PIN_ECHO, HIGH);
  //jarak_cm = duration / 58; //dalam cm

  // Angka random dari 0 sampai 21
  jarak_cm  = random(0, 21); 
  tinggi_cm = set_jarak - jarak_cm;
}

void kontrol_tinggi(){
  baca_jarak();
  while (tinggi_cm < set_jarak - 90) {
  //while (tinggi_cm < set_jarak) {
    Serial.println("Mengaktifkan relay_tangki");
    digitalWrite(relay_tangki, LOW);
    aktuator1 = "Menyala";
    yield();
  }
  Serial.println("delay sampai tinggi air terpenuhi");
  delay(1000);
  Serial.println("Menonaktifkan relay_tangki");
  digitalWrite(relay_tangki, HIGH);
  aktuator6 = "Padam";
}

void baca_suhu(){
  //sensors.requestTemperatures(); 
  //suhu = sensors.getTempCByIndex(0);

  // Angka random dari 25 sampai 35
  suhu = random(25, 35); 
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
  client.publish(PUBTOPIC_WAKTU, pesan.c_str());  // Kirimkan pesan ke MQTT

  // Print waktu di Serial monitor
  Serial.print("Current time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(" - ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

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

void handleRoot() {
  server.send(200, "text/html", SendHTML(pH, PPM, suhu, tinggi_cm, kondisiOtomatisasi));
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
  message += " " + server.argName(i) + ": " + server.arg(i)+ "\n";
  }
  server.send(404, "text/plain", message);
}

void handleData() {
  String json = "{";
  json += "\"pH\":" + String(pH) + ",";
  json += "\"PPM\":" + String(PPM) + ",";
  json += "\"suhu\":" + String(suhu) + ",";
  json += "\"tinggi_cm\":" + String(tinggi_cm) + ",";
  json += "\"kondisiOtomatisasi\":\"" + kondisiOtomatisasi + "\"";
  json += "}";
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "application/json", json);
}
