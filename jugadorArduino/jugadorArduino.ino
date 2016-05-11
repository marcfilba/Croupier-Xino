#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN0 (10);

unsigned char len = 0;
unsigned char rxBuf;

int id;
unsigned char data1 = 1;
unsigned char data2 = 2;
unsigned char data3 = 3;


void setup() {
  Serial.begin (115200);
START_INIT:
  if (CAN_OK == CAN0.begin(CAN_500KBPS))Serial.println("CAN BUS Shield init ok!");
  else goto START_INIT;
}

void loop() {
  if (CAN_MSGAVAIL == CAN0.checkReceive()) {
    CAN0.readMsgBuf(&len, &rxBuf);
    switch (CAN0.getCanId()) {
      case 100: {
          Serial.println ("Quant tinc a la ma?");
          CAN0.sendMsgBuf (110, 0, 1, &data1);
          break;
      }
      case 200: {
          Serial.println ("Primera aposta");
          CAN0.sendMsgBuf (210, 0, 1, &data2);
          break;
      }
      case 300: {
          Serial.println ("Segona aposta");
          CAN0.sendMsgBuf (310, 0, 1, &data1);
          break;
      }
      case 410: {
          Serial.print ("He obtingut aquesta puntuacio ");
          Serial.println (rxBuf);
          break;
      }
      default:{
        Serial.print ("ID rebut: ");
        Serial.println (CAN0.getCanId());
        Serial.print ("Data rebuda: ");
        Serial.println (rxBuf);
        break;
      }
    }
  }
}
