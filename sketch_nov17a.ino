#include "DHT.h"
#include <DallasTemperature.h>

#define DHT1PIN 4
#define DHT1TYPE DHT11

DHT dht1(DHT1PIN, DHT1TYPE);

const byte led1pin = 5;
const byte led2pin = 6;
const byte led3pin = 13;
int temp;
int hum;
int x;
int i;
byte serialData[16];


void setup(){
  dht1.begin();
  pinMode (led1pin, OUTPUT);
  pinMode (led2pin, OUTPUT);
  pinMode (led3pin, OUTPUT);
  Serial.begin(9600);
}

void loop(){
  readDHT();
  if (Serial.available()>0){
    i = Serial.available();
    if (x<=i){
      serialData[x] = Serial.read();
    x++;
    }
  }
  x=0;
 Serial.print("This is what I got ");
 if (x<i){
   Serial.println(serialData[x]);
   x++;
 }
 x =0;
 i =0;
}

byte bcdToDec(byte val){
  return ((val/16*10)+(val%16));
}

void readDHT(){
  int h1 = dht1.readHumidity();
  int t1 = dht1.readTemperature;
  if (isnan (t1) || isnan(h1)){
    Serial.println("Failed");
  }
  else{
    temp = DallasTemperature::toFahrenheit(t1);
    hum = h1;
  }
}
