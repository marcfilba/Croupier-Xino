#include <mcp_can.h>
#include <SPI.h>

#define NUM_PLAYERS         6
#define PLAYER              6
#define WINNER_MESSAGE_ID   400 + (PLAYER * 10)
MCP_CAN CAN0 (10);

unsigned char len = 0;
unsigned char rxBuf;

int id;

unsigned char toSend;

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
          // Encripta
          toSend = random (3);
          CAN0.sendMsgBuf (100 + (PLAYER * 10), 0, 1, &toSend);
          break;
      }
      case 200: {
          Serial.println ("Primera aposta");
          toSend = 0;
          for (int i = 0; i < NUM_PLAYERS; ++i) toSend += random (3);
          CAN0.sendMsgBuf (200 + (PLAYER * 10), 0, 1, &toSend);
          break;
      }
      case 300: {
          Serial.println ("Segona aposta");
          toSend = 0;
          for (int i = 0; i < NUM_PLAYERS; ++i) toSend += random (3);
          CAN0.sendMsgBuf (300 + (PLAYER * 10), 0, 1, &toSend);
          break;
      }
      case WINNER_MESSAGE_ID : {
          Serial.print ("He obtingut aquesta puntuacio ");
          Serial.println (rxBuf);
          break;
      }
      /*default:{
        Serial.print ("ID rebut: ");
        Serial.println (CAN0.getCanId());
        Serial.print ("Data rebuda: ");
        Serial.println (rxBuf);
        break;
      }*/
    }
  }
}
