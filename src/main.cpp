#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "Adafruit_SHT31.h"
#include <GxEPD2.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <fonts/Barlow_Medium/Barlow_Medium48pt7b.h>
#include <fonts/HarmonyOS_Sans_Bold/HarmonyOS_Sans_Bold36pt7b.h>
#include <fonts/HarmonyOS_Sans_Bold/HarmonyOS_Sans_Bold12pt7b.h>
#include <fonts/HarmonyOS_Sans_Bold/HarmonyOS_Sans_Bold10pt7b.h>
#include <fonts/HarmonyOS_Sans_Bold/HarmonyOS_Sans_Bold9pt7b.h>
#include <fonts/HarmonyOS_Sans_Bold/HarmonyOS_Sans_Bold48pt7b.h>
#include <fonts/HarmonyOS_Sans_Medium/HarmonyOS_Sans_Medium10pt7b.h>
#include <fonts/HarmonyOS_Sans_Medium/HarmonyOS_Sans_Medium8pt7b.h>
#include <fonts/PlusJakartaSans_SemiBold/PlusJakartaSans_SemiBold14pt7b.h>
#include <fonts/PlusJakartaSans_SemiBold/PlusJakartaSans_SemiBold12pt7b.h>
#include <fonts/PlusJakartaSans_SemiBold/PlusJakartaSans_SemiBold10pt7b.h>
#include <fonts/PlusJakartaSans_SemiBold/PlusJakartaSans_SemiBold40pt7b.h>
#include <fonts/PlusJakartaSans_SemiBold/PlusJakartaSans_SemiBold48pt7b.h>
#include <fonts/PlusJakartaSans_SemiBold/PlusJakartaSans_SemiBold9pt7b.h>
#include <fonts/PlusJakartaSans_Bold/PlusJakartaSans_Bold10pt7b.h>
#include <fonts/PlusJakartaSans_Bold/PlusJakartaSans_Bold12pt7b.h>
#include <fonts/PlusJakartaSans_Bold/PlusJakartaSans_Bold40pt7b.h>
#include <fonts/PlusJakartaSans_Bold/PlusJakartaSans_Bold44pt7b.h>
#include <fonts/PlusJakartaSans_Bold/PlusJakartaSans_Bold48pt7b.h>
#include <fonts/Aero_Matics_Regular/Aero_Matics_Regular9pt7b.h>
#include <fonts/Aero_Matics_Regular/Aero_Matics_Regular10pt7b.h>
#include <fonts/Aero_Matics_Regular/Aero_Matics_Regular15pt7b.h>
#include <fonts/Aero_Matics_Regular/Aero_Matics_Regular12pt7b.h>
#include <fonts/Aero_Matics_Bold/Aero_Matics_Bold10pt7b.h>
#include <fonts/Aero_Matics_Bold/Aero_Matics_Bold12pt7b.h>
#include <fonts/Aero_Matics_Bold/Aero_Matics_Bold15pt7b.h>
#include <fonts/Aero_Matics_Bold/Aero_Matics_Bold20pt7b.h>
#include <fonts/Aero_Matics_Bold/Aero_Matics_Bold44pt7b.h>
#include <fonts/Aero_Matics_Bold/Aero_Matics_Bold48pt7b.h>
#include <fonts/Barlow_Medium/Barlow_Medium36pt7b.h>
#include <fonts/Barlow_Medium/Barlow_Medium26pt7b.h>
#include <fonts/Barlow_Medium/Barlow_Medium10pt7b.h>
#include <fonts/Barlow_Medium/Barlow_Medium9pt7b.h>
#include <fonts/Barlow_Medium/Barlow_Medium8pt7b.h>
#include <fonts/Barlow_Medium/Barlow_Medium6pt7b.h>
#include <sstream>
#include <iomanip>
#include <ESP32Time.h>
#include <Wifi.h>
#include <SD.h>

#define SD_CS 27
#define SEL_BTN 4
#define INC_BTN 25
#define DEC_BTN 26

Adafruit_SHT31 sht31 = Adafruit_SHT31();
SoftwareSerial co2Sensor(16, 17);
GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=D8*/ 0, /*DC=D3*/ 14, /*RST=D4*/ 13, /*BUSY=D2*/ 2)); // GDEM029T94
ESP32Time rtc(28800);

long prevSec = 0;
uint16_t co2Ppm;
uint16_t sensorStatus;
bool enableLog = true;

void setup()
{
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  display.setPartialWindow(0, 0, display.width(), display.height());

  sht31.begin();
  Serial.begin(9600);
  co2Sensor.begin(9600);
  display.init(115200);

  pinMode(SEL_BTN, INPUT_PULLUP);
  pinMode(INC_BTN, INPUT_PULLUP);
  pinMode(DEC_BTN, INPUT_PULLUP);

  if (!SD.begin(SD_CS))
  {
    Serial.println("SD card initialisation failed");
  }
  else
  {
    Serial.println("SD card initialised");
  }
  Serial.print("Card size:  ");
  Serial.println((float)SD.cardSize() / 1000);

  Serial.print("Total bytes: ");
  Serial.println(SD.totalBytes());

  Serial.print("Used bytes: ");
  Serial.println(SD.usedBytes());

  File wifiCredentials = SD.open("/wifi_credentials.txt", FILE_READ);
  String ssid, password;
  if (wifiCredentials)
  {
    std::stringstream ss;
    ssid = wifiCredentials.readStringUntil('\n');
    password = wifiCredentials.readStringUntil('\n');

    // sscanf(ss.str().c_str(), "SSID: %s\nPassword: %s\n", ssid, password);
    //  ss >> ssid;
    //  ss >> password;

    wifiCredentials.close();
    Serial.printf("SSID: %s\nPassword: %s\n", ssid, password);
  }
  else
  {
    Serial.println("Error opening wifi_credentials.txt");
  }
  {
    display.setFont(&HarmonyOS_Sans_Medium8pt7b);
    display.fillScreen(GxEPD_WHITE);
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.setCursor(0, 15);
    display.print("Connecting to WiFi...");
    display.display(true);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  sht31.heater(false);
  Serial.print("is sht31 heater enabled: ");
  Serial.print(sht31.isHeaterEnabled());
  Serial.print("\n");

  {
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.setCursor(0, 35);
    display.print("Getting time from time.windows.com");
    display.display(true);
  }

  configTime(0, 0, "time.windows.com");
  struct tm timeinfo;
  if (getLocalTime(&timeinfo))
  {
    rtc.setTimeStruct(timeinfo);
  }
  Serial.println(rtc.getDateTime(true));

  display.fillScreen(GxEPD_WHITE);
  display.display(false);

  delay(1000);
}

void loop()
{
  if (rtc.getLocalEpoch() != prevSec)
  {
    prevSec = rtc.getLocalEpoch();

    // EVERY SECOND
    {
      display.fillScreen(GxEPD_WHITE);
      display.setTextColor(GxEPD_BLACK);
      display.setFont(&Aero_Matics_Regular10pt7b);
      display.setCursor(5, 22);
      display.print(rtc.getDate(false));
    }
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    {
      display.setFont(&Aero_Matics_Bold44pt7b);
      std::stringstream ss;
      ss << std::setfill('0') << std::setw(2) << rtc.getHour(true) << ":" << std::setfill('0') << std::setw(2) << rtc.getMinute();
      display.setCursor(5, 93);
      display.print(ss.str().c_str());
      display.getTextBounds(ss.str().c_str(), 10, 91, &tbx, &tby, &tbw, &tbh);
    }
    {
      display.setFont(&Aero_Matics_Bold20pt7b);
      std::stringstream ss;
      ss << std::setfill('0') << std::setw(2) << rtc.getSecond();
      display.setCursor(tbw + 15, 93);
      display.print(ss.str().c_str());
    }
    {
      display.setFont(&Aero_Matics_Regular10pt7b);
      display.setCursor(5, 120);
      std::stringstream ss;
      ss << sht31.readTemperature() << "°C";
      display.print((ss.str().c_str()));
    }
    {
      display.setFont(&Aero_Matics_Regular10pt7b);
      display.setCursor(70, 120);
      std::stringstream ss;
      ss << sht31.readHumidity() << "%";
      display.print((ss.str().c_str()));
    }
    {
      display.setFont(&Aero_Matics_Regular10pt7b);
      display.setCursor(143, 120);
      std::stringstream ss;
      ss << "CO2: " << co2Ppm << "ppm";
      display.print((ss.str().c_str()));
    }
    if (enableLog)
    {
      display.fillCircle(285, 99, 12, GxEPD_BLACK);
      display.fillRect(285, 87, 11, 25, GxEPD_BLACK);
      display.fillCircle(285, 99, 6, (rtc.getSecond() % 2 == 0) ? GxEPD_WHITE : GxEPD_BLACK);
    }
    display.display(true);

    Serial.print(co2Ppm);
    Serial.print("\t");

    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    if (!isnan(t))
    {
      Serial.print(t);
      Serial.print("\t");
    }
    else
    {
      Serial.println("Failed to read temperature");
    }

    if (!isnan(h))
    {
      Serial.println(h);
    }
    else
    {
      Serial.println("Failed to read humidity");
    }

    if (enableLog && co2Ppm != 0)
    {
      File logFile = SD.open("/log.txt", FILE_APPEND);
      logFile.printf("%ld\t%d\t%.2f\t%.2f\n", rtc.getEpoch(), co2Ppm, t, h);
      logFile.close();
    }

    if (prevSec % 4 == 0)
    {
      // EVERY FOUR SECONDS
      if (co2Sensor.available())
      {
        while (co2Sensor.available())
        {
          co2Sensor.read();
        }
      }

      co2Sensor.listen();
      co2Sensor.write(0xFE);
      co2Sensor.write(0x04);
      co2Sensor.write((uint8_t)0x00);
      co2Sensor.write((uint8_t)0x00);
      co2Sensor.write((uint8_t)0x00);
      co2Sensor.write(0x04);
      co2Sensor.write(0xE5);
      co2Sensor.write(0xC6);
      delay(100);

      // There's a bug in co2Sensor.available(), it won't return the right number of bytes the first time it's called :-/
      if (co2Sensor.available() && co2Sensor.available() >= 13)
      {
        uint8_t sensorPacket[13];
        for (int i = 0; i < 13; ++i)
        {
          sensorPacket[i] = co2Sensor.read();
        }
        if (sensorPacket[0] == 0xFE && sensorPacket[1] == 0x04 && sensorPacket[2] == 0x08)
        {
          sensorStatus = (sensorPacket[3] << 8) | sensorPacket[4];
          co2Ppm = (sensorPacket[9] << 8) | sensorPacket[10];
          uint16_t checksum = (sensorPacket[11] << 8) | sensorPacket[12];
        }
        else
        {
          // CORRUPTED

          sensorStatus = -1;
          co2Ppm = -1;
        }
      }
      else
      {
        sensorStatus = -1;
        co2Ppm = -1;
        // ERROR
      }
    }

    if (prevSec % 60 == 0 && prevSec % 600 != 0)
    {
      display.fillRect(0, 105, display.width(), display.height() - 105, GxEPD_BLACK);
      display.display(true);
      display.fillRect(0, 105, display.width(), display.height() - 105, GxEPD_WHITE);
      display.display(true);
      display.fillRect(0, 105, display.width(), display.height() - 105, GxEPD_BLACK);
      display.display(true);
      display.fillRect(0, 105, display.width(), display.height() - 105, GxEPD_WHITE);
      display.display(true);
    }
    if (prevSec % 600 == 0)
    {
      display.fillScreen(GxEPD_BLACK);
      display.display(true);
      display.fillScreen(GxEPD_WHITE);
      display.display(true);
      display.fillScreen(GxEPD_BLACK);
      display.display(true);
      display.fillScreen(GxEPD_WHITE);
      display.display(true);
    }
  }

  /*if (digitalRead(INC_BTN) == 0)
  {
    display.setRotation(3);
  }
  if (digitalRead(DEC_BTN) == 0)
  {
    display.setRotation(1);
  }
  if (digitalRead(SEL_BTN) == 0)
  {
    display.fillScreen(GxEPD_WHITE);
    display.display(true);
    display.fillScreen(GxEPD_BLACK);
    display.display(true);
    display.fillScreen(GxEPD_WHITE);
    display.display(true);
    display.fillScreen(GxEPD_BLACK);
    display.display(true);
  }*/
  if (digitalRead(SEL_BTN) == 0)
  {
    enableLog = !enableLog;
  }
  if (digitalRead(INC_BTN) + digitalRead(DEC_BTN) + digitalRead(SEL_BTN) == 0)
  {
    sht31.reset();
  }
  delay(100);
}
