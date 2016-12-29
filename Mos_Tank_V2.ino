#include <DS3232RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include "DHT.h"


#define DHT1PIN 2
#define DHT2PIN 3
#define DHT1TYPE DHT22
#define DHT2TYPE DHT22
#define DS3231_I2C_ADDRESS 0x68
#define ONE_WIRE_BUS 4 
#define I2C_ADDR    0x27
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

#define  LED_OFF  0
#define  LED_ON  1

////////////DS18 stuff
//VCC-BL/WH
//gnd-BL
//data-brn
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Probe01 = { 0x28, 0xFF, 0x42, 0x37, 0x63, 0x16, 0x03, 0x09 }; //The DS18B20 address
DeviceAddress Probe02 = { 0x28, 0xFF, 0x13, 0xDC, 0x61, 0x16, 0x03, 0xC3 };
// Start the LCD display library
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, POSITIVE);

/////DS3231 Stuff

/////DHT11 Stuff
//VCC-GR/WH
//GND-GR
//DAT-BR
DHT dht1(DHT1PIN, DHT1TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);


//////Other Stuff
//digital pins
const byte heater1Pin = 5; //heater 1
const byte heater2Pin = 6; //heater 2
const byte hum1Pin = 7; //humdity 1
const byte hum2Pin = 8; //humidty 2
const byte nightLightPin = 9; //night light
const byte dayLight1Pin = 10; //day light 1
const byte dayLight2Pin = 11; //day light 2

//timers
const int Day1_On_Hour = 7;
const int Day1_On_Minute = 1;
const int Day1_Off_Hour = 21;
const int Day1_Off_Minute = 2;
const int Day2_On_Hour = 7;
const int Day2_On_Minute = 30;
const int Day2_Off_Hour = 20;
const int Day2_Off_Minute = 30;
const int Night_On_Hour = 21;
const int Night_On_Minute = 30;
          
const float Day1_On_Hour_Minute = hourMinuteToHour(Day1_On_Hour, Day1_On_Minute);
const float Day1_Off_Hour_Minute = hourMinuteToHour(Day1_Off_Hour, Day1_Off_Minute);
const float Day2_On_Hour_Minute = hourMinuteToHour(Day2_On_Hour, Day2_On_Minute);
const float Day2_Off_Hour_Minute = hourMinuteToHour(Day2_Off_Hour, Day2_Off_Minute);
const float Night_On_Hour_Minute = hourMinuteToHour(Night_On_Hour, Night_On_Minute);
float currentHourMinute;

//temp settings
byte tempSetting = 70;
byte humSetting = 70;
byte dayTempSetting = 95;
byte nightTempSetting= 80;
byte dayHumSetting = 75;
byte nightHumSetting = 70;
byte maxTempSetting = 110;
byte myHour, mySecond, myMinute;

int temptemp, ds1temp, ds2temp, dhttemp1, dhttemp2, dhthum1, dhthum2, m;

unsigned long currentMillis, previousMillisTemp, previousMillisHumidity, previousMillisDisplay;
const unsigned int minCycleTime = 120000;
const unsigned int displayCycleTime = 120000;
float avgHum, avgTemp;

bool clockOk=true;
bool dl1, dl2, nl1, h1, h2, hum1, hum2;

float hourMinuteToHour(int hour, int minute){
  return hour + minute/60.0f;
}

byte decToBcd(byte val)
{
  return( (val/16*10) + (val%16) );
};

byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void setup()   /****** SETUP: RUNS ONCE ******/
{
//------- Initialize the Temperature measurement library--------------
  sensors.begin();
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, 10);
  sensors.setResolution(Probe02, 10);
  

//---------------- Initialize the lcd ------------------  
  lcd.begin (20,4);  // 20 characters, 4 lines
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();
  // Switch on the backlight
 // lcd.setBacklightPin(BACKLIGHT_PIN,NEGATIVE);
 // lcd.setBacklight(LED_ON);  
  
  //sync with RTC
  setSyncProvider(RTC.get);
  if(timeStatus() != timeSet)
    lcd.println("Unable to sync with the RTC");
   else
     lcd.println("RTC has set the system time");
 
   
   

  dht1.begin();
  dht2.begin();
  pinMode (heater1Pin, OUTPUT);
  pinMode (heater2Pin, OUTPUT);
  pinMode (hum1Pin, OUTPUT);
  pinMode (hum2Pin, OUTPUT);
  pinMode (nightLightPin, OUTPUT);
  pinMode (dayLight1Pin, OUTPUT);
  pinMode (dayLight2Pin, OUTPUT);
  Serial.begin(115200);
}//--(end setup )---





void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  
  currentMillis = millis();  
  sensors.requestTemperatures(); // Send the command to get temperatures
  ds1temp = getTemp(Probe01);  
  ds2temp = getTemp(Probe02);  
  readDHT();
  heaterControl();
  humidityControl();
  day1LightControl();
  day2LightControl();
  nightLightControl();
  displayStuff();
  picontrol();
}//--(end main loop )---


int getTemp(DeviceAddress deviceAddress)
{

  int tempC = sensors.getTempC(deviceAddress);

   if (tempC == -127.00) // Measurement failed or no device found
   {
    lcd.print("Temperature Error");
   } 
   else
   {
   temptemp = DallasTemperature::toFahrenheit(tempC); // Convert to F
   }
   return temptemp;
}



void readDHT(){
   // Reading temperature or humidity takes about 250 milliseconds! 
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor) 
    int h1 = dht1.readHumidity(); 
    int t1 = dht1.readTemperature(); 
    int h2 = dht2.readHumidity(); 
    int t2 = dht2.readTemperature();   
    // check if returns are valid, if they are NaN (not a number) then something went wrong! 
    if (isnan(t1) || isnan(h1)) 
      { 
        lcd.print("Failed to read from DHT #1"); 
      } 
    else{
      dhttemp1 = DallasTemperature::toFahrenheit(t1);
      dhthum1 = h1;
    }  
      
     if (isnan(t2) || isnan(h2)) 
     { 
       lcd.println("Failed to read from DHT #2"); 
     } 
     else 
     { 
       dhttemp2 = DallasTemperature::toFahrenheit(t2);
       dhthum2 = h2;
       } 
} 
 
void heaterControl(){
    avgTemp = ((ds1temp + ds2temp + dhttemp1 + dhttemp2)/4);
    
    if(currentMillis - previousMillisTemp >= minCycleTime){
      if (avgTemp < tempSetting){
        digitalWrite(heater1Pin, HIGH);
        digitalWrite(heater2Pin, HIGH);
        previousMillisTemp = currentMillis;
      }
      else if (avgTemp >= tempSetting){
        digitalWrite(heater1Pin, HIGH);
        digitalWrite(heater2Pin, LOW);
        previousMillisTemp = currentMillis;
      }
      else if (avgTemp >= maxTempSetting){
        digitalWrite(heater1Pin, LOW);
        digitalWrite(heater2Pin, LOW);
        previousMillisTemp = currentMillis;
      }
      else{
        return;
      }
    }
  }
  
  void humidityControl(){
    avgHum = ((dhthum1 + dhthum2)/2);
    
    if (currentMillis - previousMillisHumidity >= minCycleTime){
      if( avgHum <= humSetting){
        digitalWrite(hum1Pin, HIGH);
        digitalWrite(hum2Pin, HIGH);
        previousMillisHumidity = currentMillis;
      }
      else if (avgHum > humSetting){
        digitalWrite(hum1Pin, LOW);
        digitalWrite(hum2Pin, LOW);
        previousMillisHumidity = currentMillis;
      }
      else{
        return;
      }
    }
  }
  
  void displayStuff(){
    lcd.clear();  // Reset the display  
    //lcd.home();
    //lcd.backlight();  //Backlight ON if under program control  
    lcd.setCursor(0,0);
    checkHour();
    checkDigits(myHour);
    lcd.print(":");
    checkDigits(minute());
    lcd.print(":");
    checkDigits(second());
    lcd.print(" ");
    checkAMPM();
    lcd.setCursor(0,1);
    lcd.print("Average Temp: ");
    lcd.print(avgTemp, 0);
    lcd.print(char(223));
    lcd.print("F ");
    lcd.setCursor(0,2);
    lcd.print("Avg Humidity: ");
    lcd.print(avgHum, 0);
    lcd.print("%");
    lcd.setCursor(0,3);
    lcd.print("DHT1 =");
    lcd.print(dhttemp1);
    lcd.print(" DHT2=");
    lcd.print(dhttemp2);
    delay(5000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("DHT1 Hum: ");
    lcd.print(dhthum1);
    lcd.setCursor(0,1);
    lcd.print(" DHT2 HUM:");
    lcd.print(dhthum2);
    lcd.setCursor(0,2);
    lcd.print(" ds1temp=");
    lcd.print(ds1temp);
    lcd.setCursor(0,3);
    lcd.print(" ds2temp=");
    lcd.print(ds2temp);
    delay(5000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp Setting: ");
    lcd.print(tempSetting);
    lcd.setCursor(0,1);
    lcd.print("Hum Setting: ");
    lcd.print(humSetting);
    delay(5000);
  }
  
  void displaystuff2(){
    Serial.print(dhttemp1);
    Serial.print(" , ");
    Serial.print(dhttemp2);
    Serial.print(" , ");
    Serial.print(dhthum1);
    Serial.print(" , ");
    Serial.print(dhthum2);
    Serial.print(" , ");
    Serial.print(ds1temp);
    Serial.print(" , ");
    Serial.print(ds2temp);
    Serial.print(" , ");
    Serial.print(tempSetting);
    Serial.print(" , ");
Serial.println(humSetting);
  }
  
 void checkHour(){
   if(hour() <= 12){
     myHour = hour();
   }
   else{
     myHour = hour()-12;
   }
 }
 
 int checkDigits(int digits){
   if(digits < 10){
     lcd.print('0');
   }
     lcd.print(digits);
   }
   
  void checkAMPM(){
    if(hour() <= 12){
      lcd.print("AM");
    }
    else{
      lcd.print("PM");
    }
  }
  
 
 void day1LightControl(){
   float currentHourMinute = hour() + minute()/60.0f;
   //day lights
   if(currentHourMinute >= Day1_On_Hour_Minute && currentHourMinute < Day1_Off_Hour_Minute){
     humSetting = dayHumSetting;
     tempSetting = dayTempSetting;
     digitalWrite(dayLight1Pin, HIGH);
     digitalWrite(nightLightPin, LOW);
   }
   else {
     digitalWrite(dayLight1Pin, LOW);
   }
 }
 void day2LightControl(){
   currentHourMinute = hour()+ minute()/60.0f;
   if(currentHourMinute >= Day2_On_Hour_Minute && currentHourMinute < Day2_Off_Hour_Minute){
     digitalWrite(dayLight2Pin, HIGH);
   }
   else {
     digitalWrite(dayLight2Pin, LOW);
   }
 }
 
 void nightLightControl(){
   currentHourMinute = hour() + minute()/60.0f;
   if(currentHourMinute >= Night_On_Hour_Minute){
   digitalWrite(nightLightPin, HIGH);
   humSetting = nightHumSetting;
   tempSetting = nightTempSetting;
   }
 }
 
 void picontrol(){
  String piString;
  int cmd = 0, set = 0, tempSettingtemp = 0, tempSettingtemp1 = 0, humSettingtemp = 0, humSettingtemp1 = 0;

  while (Serial.available()){
    delay(10);  
    while (Serial.available() > 0){
      char c = Serial.read();
      piString += c;
    }
  if (piString.length() > 0){
    char carray[8];
    piString.toCharArray(carray, 8);
    cmd = carray[1] - 48;
    set = carray[2] - 48;
    tempSettingtemp = (carray[3] - 48) * 10;
    tempSettingtemp1 = carray[4] - 48;
    humSettingtemp = (carray[3] - 48) * 10;
    humSettingtemp1 = carray[4] - 48;
    delay(250);
    piString = "";
  }
  }
  
  if (cmd == 1) {
    displayStuff();
  }
  
  if (cmd == 2){
   
    if(set ==  1){
     tempSetting = tempSettingtemp + tempSettingtemp1;
     displayStuff();
   }
   
   if (set == 2){
    humSetting = humSettingtemp + humSettingtemp1; 
    displayStuff();
   }
  }
}


