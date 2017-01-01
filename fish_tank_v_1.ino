#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
//#include <TimeLib.h>


#define ONE_WIRE_BUS 2

//ds18 stuff
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Probe1 = { 0x28, 0xFF, 0xF2, 0x6D, 0x63, 0x16, 0x04, 0x06 }; //The DS18B20 address
DeviceAddress Probe2 = { 0x28, 0xFF, 0x4B, 0x24, 0x62, 0x16, 0x03, 0xE1 };
DeviceAddress Probe3 = { 0x28, 0xFF, 0xF2, 0x6D, 0x63, 0x16, 0x04, 0x06 }; //The DS18B20 address
DeviceAddress Probe4 = { 0x28, 0xFF, 0x4B, 0x24, 0x62, 0x16, 0x03, 0xE1 };

//digital outs
const byte relay1pin = 30; //Heater #1
const byte relay2pin = 31; //Heater #2
const byte relay3pin = 32; //Pump #1
const byte relay4pin = 33; //Pump #2
const byte relay5pin = 34; //Filter
const byte relay6pin = 35; //Lizard tank heater
const byte relay7pin = 36; //Lizard tank light #1
const byte relay8pin = 37; //Lizard tank light #2
const byte relay9pin = 38; //Lizard tank light #3
const byte relay10pin = 39; //Spare
const byte fan1pin = 40;  //Fish tank fan #1
const byte fan2pin = 41;  //Fish tank fan #2
const byte fan3pin = 42;  //Lizard tank fan
const byte led1pin = 44;  //Led lights #1 
const byte led2pin = 45;  //Led lights #2 


//Timing
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
int hour = 7;
int minute = 30;

//Temp and settings
float fishtemp, lizardtemp1, lizardtemp2, canopytemp;
int fishtempsetting = 75;
int hotsidesetting = 95;
int coldsidesetting = 75;
int lizarddaysetting = 95;
int lizardnightsetting = 60;
int canopysetting = 110;


unsigned long currentmillis, previousmillisfish, previousmillislizard, previousmilliscanopy;
const unsigned long mincycletime = 120000;

int r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, led1, led2, fan1, fan2, fan3;

String newstring = "";
boolean stringcomplete = false;

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

void setup() {
  sensors.begin();
  sensors.setResolution(Probe1, 9);
  sensors.setResolution(Probe2, 9);
  sensors.setResolution(Probe3, 9);
  sensors.setResolution(Probe4, 9);  
  pinMode(relay1pin, OUTPUT);
  pinMode(relay2pin, OUTPUT);
  pinMode(relay3pin, OUTPUT);
  pinMode(relay4pin, OUTPUT);
  pinMode(relay5pin, OUTPUT);
  pinMode(relay6pin, OUTPUT);
  pinMode(relay7pin, OUTPUT);
  pinMode(relay8pin, OUTPUT);
  pinMode(relay9pin, OUTPUT);
  pinMode(relay10pin, OUTPUT);
  pinMode(fan1pin, OUTPUT);
  pinMode(fan2pin, OUTPUT);
  pinMode(fan3pin, OUTPUT);
  pinMode(led1pin, OUTPUT);
  pinMode(led2pin, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
}

void loop() {
  currentmillis = millis();
  sensors.requestTemperatures();
  fishtemp = getTemp(Probe1);
  lizardtemp1 = getTemp(Probe2);
  lizardtemp2 = getTemp(Probe3);
  canopytemp = getTemp(Probe4);
  fishheatercontrol();
  lizardheatercontrol();
  lizardfancontrol();
  canopyfancontrol();
  day1LightControl();
  day2LightControl();
  nightLightControl();
  delay(1);        // delay in between reads for stability
  if(stringcomplete == true){
    serialprint();
  }
}

void serialEvent(){
  if(Serial.available()){
    while(Serial.available()>0){
    char newchar = Serial.read();
    if(newchar != '\n'){
       newstring += newchar;
    }else{
      stringcomplete = true;
    }
    }
  }
}

void serialprint(){
  Serial.println(newstring);
   int commaindex = newstring.indexOf(',');
   int secondcommaindex = newstring.indexOf(',', commaindex+1);
   int thirdcommaindex = newstring.indexOf(',', secondcommaindex+1);
   String firstvalue = newstring.substring(0, commaindex);
   String secondvalue = newstring.substring(commaindex+1, secondcommaindex);
   String thirdvalue = newstring.substring(secondcommaindex+1, thirdcommaindex);
   String fourthvalue = newstring.substring(thirdcommaindex+1);
   int r = firstvalue.toInt();
   int g = secondvalue.toInt();
   int b = thirdvalue.toInt();
   int w = fourthvalue.toInt();
   Serial.println(r);
   Serial.println(g);
   Serial.println(b);
   Serial.println(w);
   newstring = "";
   stringcomplete = false;
}

float getTemp(DeviceAddress deviceAddress)
{

  float tempC = sensors.getTempC(deviceAddress);
  float temptemp;
   if (tempC == -127.00) // Measurement failed or no device found
   {
    temptemp = -127;
   } 
   else
   {
   temptemp = DallasTemperature::toFahrenheit(tempC); // Convert to F
   }
   return temptemp;
}


void fishheatercontrol(){
  
}

void lizardheatercontrol(){
  
}

void lizardfancontrol(){
  
}

void canopyfancontrol(){
  
}

  
 
 void day1LightControl(){
   float currentHourMinute = hour + minute;
   //day lights
   if(currentHourMinute >= Day1_On_Hour_Minute && currentHourMinute < Day1_Off_Hour_Minute){
     digitalWrite(relay7pin, HIGH);
     digitalWrite(relay9pin, LOW);
   }
   else {
     digitalWrite(relay7pin, LOW);
   }
 }
 void day2LightControl(){
   currentHourMinute = hour()+ minute()/60.0f;
   if(currentHourMinute >= Day2_On_Hour_Minute && currentHourMinute < Day2_Off_Hour_Minute){
     digitalWrite(relay8pin, HIGH);
   }
   else {
     digitalWrite(relay8pin, LOW);
   }
 }
 
 void nightLightControl(){
   currentHourMinute = hour() + minute()/60.0f;
   if(currentHourMinute >= Night_On_Hour_Minute){
   digitalWrite(relay9pin, HIGH);
   }
 }
