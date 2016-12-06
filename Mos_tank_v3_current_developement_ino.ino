#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>


#define DHT1PIN 4
#define DHT2PIN 3
#define DHT1TYPE DHT11
#define DHT2TYPE DHT11
#define ONE_WIRE_BUS 2


////////////DS18 stuff
//VCC-red
//gnd-black
//data-yellow
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Probe01 = { 0x28, 0xFF, 0xF2, 0x6D, 0x63, 0x16, 0x04, 0x06 }; //The DS18B20 address
DeviceAddress Probe02 = { 0x28, 0xFF, 0x4B, 0x24, 0x62, 0x16, 0x03, 0xE1 };

/////DHT11 Stuff
//VCC-GR/WH
//GND-GR
//DAT-BR
DHT dht1(DHT1PIN, DHT1TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);


//////Other Stuff
const byte heater1Pin = 5; //heater 1
const byte heater2Pin = 6; //heater 2
const byte hum1Pin = 7; //humdity 1
const byte hum2Pin = 9; //humidty 2
const byte nightLightPin = 8; //night light
const byte dayLight1Pin = 10; //day light 1
const byte dayLight2Pin = 11; //day light 2

byte tempSetting = 75;
byte humSetting= 70;
byte dayTempSetting = 95;
byte nightTempSetting= 80;
byte dayHumSetting = 75;
byte nightHumSetting = 70;
byte maxTempSetting = 110;

int ds1temp, ds2temp, dhttemp1, dhttemp2, dhthum1, dhthum2;

unsigned long currentMillis, previousMillisTemp, previousMillisHumidity, previousMillisDisplay;
const unsigned int minCycleTime = 120000;
const unsigned int displayCycleTime = 120000;
float avgHum, avgTemp;
boolean newData = true;



void setup()   /****** SETUP: RUNS ONCE ******/
{
//------- Initialize the Temperature measurement library--------------
  sensors.begin();
  //set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, 9);
  sensors.setResolution(Probe02, 9);
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
  piControl();
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
int getTemp(DeviceAddress deviceAddress)
{

int tempC = sensors.getTempC(deviceAddress);
  int temptemp;
   if (tempC == -127.00) // Measurement failed or no device found
   {
    Serial.println("Temperature Error");
   } 
   else
   {
   temptemp = DallasTemperature::toFahrenheit(tempC); // Convert to F
   }
   return temptemp;
}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
};

void readDHT(){
   // Reading temperature or humidity takes about 250 milliseconds! 
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor) 
    int h1 = dht1.readHumidity(); 
    int t1 = dht1.readTemperature(); 
    int h2 = dht2.readHumidity(); 
    int t2 = dht2.readTemperature();   
    delay(250);
    // check if returns are valid, if they are NaN (not a number) then something went wrong! 
    if (isnan(t1) || isnan(h1)) 
      { 
        Serial.println("Failed to read from DHT #1"); 
      } 
    else{
      dhttemp1 = DallasTemperature::toFahrenheit(t1);
      dhthum1 = h1;
    }  
      
     if (isnan(t2) || isnan(h2)) 
     { 
       Serial.println("Failed to read from DHT #2"); 
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
  

 
 int checkDigits(int digits){
   if(digits < 10){
     Serial.print('0');
   }
     Serial.print(digits);
   }
 

void piControl(){
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

