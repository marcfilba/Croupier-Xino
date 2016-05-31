#include <mcp_can.h>
#include <SPI.h>

#define NUM_JUGADORS 6

int idsMans [NUM_JUGADORS];
int idsApostaInicial [NUM_JUGADORS];
int idsApostaFinal [NUM_JUGADORS];

int apostaInicial [NUM_JUGADORS];
int apostaFinal [NUM_JUGADORS];

int sumaMans;
int numJugadors;
byte zero = 0;

byte punts [NUM_JUGADORS];

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf;

MCP_CAN CAN0 (10);

/*************
 * G1: 11 17 2
 * G2: 43 9 14
 * G3: 25 17 47
 * G4: 39 46 23
 * G5: 23 30 72
 *************/

 int valor1, valor2, valor3;
int desencripta (int valor) {
  switch (rxId) {
   
      case 110:              
            valor1 = valor - 11;
            valor2 = valor - 17;
            valor3 = valor - 2;
            
            if (valor1 >= 0 && valor1 <= 3 ) valor = valor1;
            else if (valor2 >= 0 && valor2 <=3 ) valor = valor2;
            else if (valor3 >= 0 && valor3 <=3 ) valor = valor3;
            else valor = 0;
            
            return valor;
      case 120:
            valor1 = valor - 43;
            valor2 = valor - 9;
            valor3 = valor - 14;

            if (valor1 >= 0 && valor1 <= 3 ) valor = valor1;
            else if (valor2 >= 0 && valor2 <=3 ) valor = valor2;
            else if (valor3 >= 0 && valor3 <=3 ) valor = valor3;
            else valor = 0;
            return valor;
      case 130: 
            valor1 = valor - 25;
            valor2 = valor - 17;
            valor3 = valor - 47;
            if (valor1 >= 0 && valor1 <= 3 ) valor = valor1;
            else if (valor2 >= 0 && valor2 <=3 ) valor = valor2;
            else if (valor3 >= 0 && valor3 <=3 ) valor = valor3;
            else valor = 0;
            return valor;
      case 140: 
            valor1 = valor - 39;
            valor2 = valor - 46;
            valor3 = valor - 23;
            if (valor1 >= 0 && valor1 <= 3 ) valor = valor1;
            else if (valor2 >= 0 && valor2 <=3 ) valor = valor2;
            else if (valor3 >= 0 && valor3 <=3 ) valor = valor3;
            else valor = 0;
            return valor;
      case 150:
            valor1 = valor - 23;
            valor2 = valor - 30;
            valor3 = valor - 72;
            if (valor1 >= 0 && valor1 <= 3 ) valor = valor1;
            else if (valor2 >= 0 && valor2 <=3 ) valor = valor2;
            else if (valor3 >= 0 && valor3 <=3 ) valor = valor3;
            else valor = 0;
            return valor;
    default: return valor;
  }
}
void iniciarPartida () {
  sumaMans = 0;
  numJugadors = 0;
  Serial.println("inici partida");
  for (int i = 0; i < NUM_JUGADORS; ++i) {
    idsMans [i]          = 100 + (i + 1) * 10;
    idsApostaInicial [i] = 200 + (i + 1) * 10;
    idsApostaFinal [i]   = 300 + (i + 1) * 10;
    punts [i] = 0;
  }
}

void llegeixMans () {
  Serial.println("llegeix Mans");
  unsigned int m = millis ();
  
  CAN0.sendMsgBuf(100, 0, 1, &zero);
  delay(100);
  
  while (numJugadors != NUM_JUGADORS and (m + 1000) > millis()) {
    llegeixCan();
    for (int i = 0; i < NUM_JUGADORS; ++i) {
      if ((rxId == idsMans [i]) && (idsMans [i] != -1)) {
        sumaMans += desencripta (rxBuf);
        Serial.print ("Suma mans: ");
        Serial.println (sumaMans);
        ++ numJugadors;
        idsMans[i] = -1;
        break;
      }
    }
  }
  numJugadors = 0;
}

void llegeixCan () {

  if (CAN_MSGAVAIL == CAN0.checkReceive ()){
    CAN0.readMsgBufID(&rxId, &len, &rxBuf);
    Serial.print ("rebut missatge amb ID = "); Serial.println (rxId);
  }
}

void llegeixApostaInicial () {
  Serial.println("aposta inicial");
  
  CAN0.sendMsgBuf(200, 0, 1, &zero);
  delay(100);

  numJugadors = 0;
  unsigned int m = millis ();
  while (numJugadors != NUM_JUGADORS and (m + 1000) > millis()) {
    llegeixCan();
    
    for (int i = 0; i < NUM_JUGADORS; ++i) {
      if ((rxId == idsApostaInicial [i]) && (idsApostaInicial [i] != -1)) {
        apostaInicial [i] = rxBuf;
        ++ numJugadors;
        idsApostaInicial[i] = -1;
        
        break;
      }
    }
  }
}

void llegeixApostaFinal () {
  Serial.println("aposta final");
  
  CAN0.sendMsgBuf(300, 0, 1, &zero);
  delay(100);

  numJugadors = 0;
  unsigned int m = millis ();
  while (numJugadors != NUM_JUGADORS and (m + 1000) > millis()) {
    llegeixCan();
    
    for (int i = 0; i < NUM_JUGADORS; ++i) {
      if ((rxId == idsApostaFinal [i]) && (idsApostaFinal [i] != -1)) {
        apostaFinal [i] = rxBuf;
        ++ numJugadors;
        idsApostaFinal[i] = -1;
        
        break;
      }
    }
  }
}

void obtenGuanyador () {
  Serial.println("obtenGuanyador");
  int diff = 9999;
  bool guanyador = false;

  for (int i = 0; i < NUM_JUGADORS; ++i) {
    if (abs(sumaMans - apostaFinal [i]) == 0) {
      punts [i] = 2;
      guanyador = true;
    }
  }

  if (!guanyador) {
    for (int i = 0; i < NUM_JUGADORS; ++i) diff = min (diff, abs(sumaMans - apostaFinal [i]));

    Serial.print ("diff ");
    Serial.println (diff);
  
    for (int i = 0; i < NUM_JUGADORS; ++i){
      if (apostaFinal [i] - sumaMans == diff) punts[i] = 1;
      else if (apostaFinal [i] + sumaMans == diff) punts[i] = 1;
    }
  }
}

void actualitzaMarcador () {
  for (int i = 0; i < NUM_JUGADORS; ++i) {
    Serial.print (punts[i]);
    if (i != NUM_JUGADORS - 1) Serial.print (",");
  }
  Serial.println ("");
}

void notificaPuntuacio () {
  for (int i = 0; i < NUM_JUGADORS; ++i) {
    CAN0.sendMsgBuf(400 + (i + 1) * 10, 0, 1, &punts[i]);
  }
  delay(50);
}

void setup() {
  Serial.begin (115200);
  while (CAN_OK != CAN0.begin(CAN_500KBPS)) Serial.println ("Error inicialitzant CAN");

  while (!Serial.available());      // ESPERA QUE LA INTERFÍCIE GRÀFICA ESTIGUI AIXECADA
  Serial.read ();
}

void loop() {
  iniciarPartida ();        // INICIALITZA VARIABLES
  llegeixMans ();           // LLEGEIX MANS (ENCRIPTAT)

  llegeixApostaInicial ();  // LLEGEIX PRIMERA APOSTA
  llegeixApostaFinal ();    // LLEGEIX SEGONA APOSTA

  obtenGuanyador ();        // CALCULA EL DIFF I AGAFA EL MENOR
  actualitzaMarcador ();    // ENVIA PER SERIE LES N APOSTES A LA INTERFÍCIE GRÀFICA
  notificaPuntuacio ();     // ENVIA PER CAN LA PUNTUACIÓ DE CADA GRUP
}
