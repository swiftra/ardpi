#include <Wire.h>

#define SLAVE_ADDRESS 0x29  //slave address for arduino

#define REG_MAP_SIZE 14

#define MAX_SENT_BYTES 3

//Global Variables

byte registerMap[REG_MAP_SIZE];

byte registerMapTemp[REG_MAP_SIZE-1];

byte receivedCommands[MAX_SENT_BYTES];

void setup(){
    Wire.begin(SLAVE_ADDRESS);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
}

void loop(){
}

void requestEvent(){
  Wire.write(registerMap, REG_MAP_SIZE);  //Set the buffer to send 14 bytes of data
}

void receiveEvent(int bytesReceived){
  for(int a = 0; a< bytesReceived; a++){
    if(a<MAX_SENT_BYTES){
      receivedCommands[a]=Wire.read();
    }
    else{
      Wire.read();   //if we receive more data than allowed just throw it away
    }
  }
}

