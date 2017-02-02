#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#include <time.h>
int temppm25,temppm10;


//OLED
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
SSD1306  display(0x3c, D2, D3); //SDA SCL
#define DEMO_DURATION 3000

//Air Pollution
#include "Arduino.h"
#include "serialReadPMValue.h"

uint16_t PM01Value = 0;        //define PM1.0 value of the air detector module
uint16_t PM2_5Value = 0;       //define PM2.5 value of the air detector module
uint16_t PM10Value = 0;       //define PM10 value of the air detector module
#define receiveDatIndex 32

uint8_t receiveDat[receiveDatIndex]; //receive data from the air detector module

int timer = 0;


//GPS
#include "SoftwareSerial.h"
SoftwareSerial GPSModule(D4, D5); // TX, RX
int updates;
int failedUpdates;
int pos;
int stringplace = 0;

String statusd;

String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};



// Config Firebase
//#define FIREBASE_HOST "pointznet-c566c.firebaseio.com"
//#define FIREBASE_AUTH "z2VY5MKnv2O05P7DqhpMs2Pc0lFp0UcAaOyxutn0"
#define FIREBASE_HOST "pointznet-c1490.firebaseio.com"
#define FIREBASE_AUTH "ldAeVoMpj6myZC5pFcmxJrvU6rG6VJNWgxCMxod5"

// Config connect WiFi
#define WIFI_SSID "RbME"
#define WIFI_PASSWORD "1234567890"

// Config DHT
#define DHTPIN D1
#define DHTTYPE DHT22

// Config time
int timezone = 7;

char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";

int dst = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  //OLED setup
  display.init();
  display.flipScreenVertically();

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "connecting...");
  display.drawString(0, 10, "ssid : "); //WIFI_SSID
  display.drawString(25, 10, String(WIFI_SSID));
  display.display();

  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);



  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  }
  Serial.println();
  //display.drawString(10, 15, "PointzNet");
  Serial.print("connected: ");
  //display.drawString(0, 10, String(WiFi.localIP()));
  Serial.println(WiFi.localIP());

  configTime(timezone * 3600, dst, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(10);
  }
  Serial.println();
//  Serial.println("Now: " + NowString());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  dht.begin();
}

void loop() {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "No.0001");//statusd
  display.drawString(45, 0, String(statusd));
//  display.drawString(100, 0, String(TimeNowString()));
  // Read temp & Humidity for DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  


  //Air Pollution
  float tt ;
  int t25;
  float t1;
  int length = serialRead(Serial, receiveDat, receiveDatIndex, 5); //change the serial port:Serial1,Serial2..
  int checkSum = checkValue(receiveDat, receiveDatIndex);

  if (length && checkSum)
  {

    PM01Value = transmitPM01(receiveDat); //count PM1.0 value of the air detector module

    PM2_5Value = transmitPM2_5(receiveDat); //count PM2.5 value of the air detector module

    PM10Value = transmitPM10(receiveDat); //count PM10 value of the air detector module

  }
  static unsigned long OledTimer = millis();     //every 0.5s update the temperature and humidity from DHT11 sensor
  if (millis() - OledTimer >= 1000)
  {
    OledTimer = millis();

    Serial.print("PM1.0: ");  //send PM1.0 data to bluetooth
    Serial.print(PM01Value);
    Serial.println("  ug/m3");


    Serial.print("PM2.5: ");  //send PM1.0 data to bluetooth
    Serial.print(PM2_5Value);
    Serial.println("  ug/m3");

    Serial.print("PM10:  ");  //send PM1.0 data to bluetooth
    Serial.print(PM10Value);
    Serial.println("  ug/m3");
  }
  //tt = PM10Value/1000; //10
  //t1 = PM01Value/1000; //1
  t25 = PM2_5Value / 10; //2.5
  if(t25 <= 200 && PM10Value <=250){
    temppm25 = t25;
    temppm10 = PM10Value;
    }
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 50, "PM 2.5:");
  display.drawString(40, 50, String(temppm25));
  display.drawString(70, 50, "PM 10:");
  display.drawString(105, 50, String(temppm10));



  //GPS Modul
  Serial.flush();
  GPSModule.flush();
  while (GPSModule.available() > 0) {
    GPSModule.read();
  }
  if (GPSModule.find("$GPRMC,")) {
    String tempMsg = GPSModule.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }

    updates++;
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    int counts = 30;
    //display.drawString(55, 0, labels[0]);
    //display.drawString(91, 0, nmea[0]);
    for (int i = 0; i < 9; i++) {
      if (i == 2 || i == 4) {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, counts, labels[i]);
        display.drawString(60, counts, nmea[i]);
        counts = counts + 10;
      }
      Serial.print(labels[i]);
      Serial.print(nmea[i]);
      Serial.println("");
    }
  } else {
    failedUpdates++;
  }
  stringplace = 0;
  pos = 0;
  //End GPS



  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(500);
    return;
  }
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 10, "Humidity: ");
  display.drawString(45, 10, String(h));
  display.drawString(0, 20, "Temperature: ");
  display.drawString(65, 20, String(t));
  display.display();

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.println();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& timeStampObject = jsonBuffer.createObject();
  JsonObject& tempTime = root.createNestedObject("timestamp");
  root["lat"] = nmea[2];
  root["lng"] = nmea[4];


  root["pm25"] = temppm25;
  root["pm10"] = temppm10;

  tempTime[".sv"] = "timestamp";
  root["temperature"] = t;
  root["humidity"] = h;
  //root["time"] = NowString();

 
  // append a new value to /logDHT
  String name = Firebase.push("0001", root);
  Firebase.setFloat("0001/lastpm25",temppm25 );
  Firebase.setFloat("0001/lastpm10",temppm10 );

  
  // handle error
  if (Firebase.failed()) {
    Serial.print("pushing /logs failed:");
    Serial.println(Firebase.error());
    statusd = "Offline";
    //display.clear();
    //display.setFont(ArialMT_Plain_10);
    //display.drawString(0, 0, "Connection lost");
    //display.drawString(0, 10, "reconnect...");
    //display.display();
    return;
  } else {
    statusd = "Online";
  }
  Serial.print("pushed: /0001/");
  Serial.println(name);

  delay(2000);
}


/**
String NowString() {
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  String tmpNow = "";
  tmpNow += String(newtime->tm_mday);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mon + 1);
  tmpNow += "-";
  tmpNow += String(newtime->tm_year + 1900);
  tmpNow += " ";
  tmpNow += String(newtime->tm_hour);
  tmpNow += ":";
  tmpNow += String(newtime->tm_min);
  tmpNow += ":";
  tmpNow += String(newtime->tm_sec);
  return tmpNow;
}


String TimeNowString() {
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  String timeNow = "";
  if (newtime->tm_hour < 10) {
    timeNow += "0";
  }
  timeNow += String(newtime->tm_hour);
  timeNow += ":";
  if (newtime->tm_min < 10) {
    timeNow += "0";
  }
  timeNow += String(newtime->tm_min);
  //timeNow += ":";
  //timeNow += String(newtime->tm_sec);
  return timeNow;
}

**/

String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }

  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}
