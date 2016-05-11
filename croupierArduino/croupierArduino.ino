#include <mcp_can.h>
#include <SPI.h>

#define NUM_JUGADORS 5

int idsMans [NUM_JUGADORS];
int idsAposta [NUM_JUGADORS];

int apostaInicial [NUM_JUGADORS];
int apostaFinal [NUM_JUGADORS];

int sumaMans;
int numJugadors;
byte zero = 0;

byte guanyador;

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf [8];

MCP_CAN CAN0 (10); 

int desencripta (int valor){
  switch (rxId){
    case 110: return valor / 3;
    case 120: return valor / 7;
    case 130: return valor - 1;
    case 140: return valor + 1;
    case 150: return valor / 5;
    default: return valor;
  }
}

void iniciarPartida (){
  sumaMans = 0;
  for (int i = 0; i < NUM_JUGADORS; ++i){ 
    idsMans [i] = 100 + (i + 1)*10;
    apostaInicial [i] = 200 + (i + 1)*10;
    apostaFinal [i] = 300 + (i + 1)*10;
  }
}

void llegeixMans (){
  CAN0.sendMsgBuf(100, 0, 1, &zero);
  while (numJugadors != NUM_JUGADORS){
    llegeixCan();
    for (int i = 0; i < NUM_JUGADORS; ++i){
      if ((rxId == idsMans [i]) && (idsMans [i] != -1)){ 
        sumaMans = desencripta (atoi((const char *) rxBuf));
        ++ numJugadors; 
        idsMans[i] = -1; 
        break; 
      }
    }
  }
  numJugadors = 0;
}

void llegeixCan (){
  CAN0.readMsgBuf(&len, rxBuf);
  rxId = CAN0.getCanId();
}

void llegeixApostaInicial (){
  CAN0.sendMsgBuf(200, 0, 1, &zero);
  while (numJugadors != NUM_JUGADORS){
    llegeixCan();
    for (int i = 0; i < NUM_JUGADORS; ++i){
      if ((rxId == idsAposta [i]) && (idsAposta [i] != -1)){ 
        apostaInicial [i] = atoi((const char *) rxBuf);
        ++ numJugadors; 
        idsAposta[i] = -1; 
        break; 
      }
    }
  }
  for (int i = 0; i < NUM_JUGADORS; ++i) idsAposta[i] = 0;
  numJugadors = 0;
}

void llegeixApostaFinal (){
  CAN0.sendMsgBuf(300, 0, 1, &zero);
  while (numJugadors != NUM_JUGADORS){
    llegeixCan();
    for (int i = 0; i < NUM_JUGADORS; ++i){
      if ((rxId == idsAposta [i]) && (idsAposta [i] != -1)){ 
        apostaFinal [i] = atoi((const char *) rxBuf);
        ++ numJugadors; 
        idsAposta[i] = -1; 
        break; 
      }
    }
  }
  for (int i = 0; i < NUM_JUGADORS; ++i) idsAposta[i] = 0;
  numJugadors = 0;
}

void obtenGuanyador (){
  int diff = abs(sumaMans - apostaFinal [0]);
  guanyador = 0;
  
  for (int i = 1; i < NUM_JUGADORS; ++i){
    if (abs (sumaMans - apostaFinal [i]) < diff){
      diff = (abs(sumaMans - apostaFinal [i]));
      guanyador = i;
    }
  }
}

void actualitzaMarcador (){
  for (int i = 0; i < NUM_JUGADORS; ++i){
    if (i == NUM_JUGADORS -1)  Serial.print (apostaFinal [i]);
    else                       Serial.print (String(apostaFinal [i]) + ',');
  }
  Serial.println ("");
}

void notificaGuanyador (){
  CAN0.sendMsgBuf(400, 0, 1, &guanyador);
  delay(100); 
}

void setup() {
  Serial.begin (115200);
  if (CAN_OK != CAN0.begin(CAN_500KBPS)) Serial.println ("Error inicialitzant CAN");
  
  Serial.read ();                  // ESPERA QUE LA INTERFÍCIE GRÀFICA ESTIGUI AIXECADA
  //Serial.println (NUM_JUGADORS); // ENVIA NUMERO JUGADORS A LA INTERFÍCIE
}

void loop() {
  iniciarPartida ();      // INICIALITZA VARIABLES
  llegeixMans ();         // LLEGEIX MANS (ENCRIPTAT)
  
  llegeixApostaInicial ();       // LLEGEIX PRIMERA APOSTA
  llegeixApostaFinal ();         // LLEGEIX SEGONA APOSTA

  obtenGuanyador ();      // CALCULA EL DIFF I AGAFA EL MENOR
  actualitzaMarcador ();  // ENVIA PER SERIE LES N APOSTES A LA INTERFÍCIE GRÀFICA
  notificaGuanyador ();   // ENVIA PER CAN QUI HA ESTAT EL VENCEDOR
}
