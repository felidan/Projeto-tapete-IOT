#include <ESP8266WiFi.h> 

#define MAX_PIN_CORTE 14
#define DELAY_COMUNIC_MULTIPLEX 200

// SENHA UTILIZADA É TEMPORÁRIA
const char* ssid = "FelidanWiFi";
const char* senha = "rnrh5879";

String apiKey = "ON07PMHZQPVFQQP6";
const char *servidor_destino = "api.thingspeak.com";

int pin_mult_D0 = 16; 
int pin_mult_D1 = 5; 
int pin_mult_D2 = 4; 
int pin_mult_D3 = 0; 
int sinal_analogico[14];
int sinal_bin[16][4] = {0,0,0,0,  0,0,0,1,  0,0,1,0,  0,0,1,1,  0,1,0,0,  0,1,0,1,  0,1,1,0,  0,1,1,1,  1,0,0,0,  1,0,0,1,  1,0,1,0,  1,0,1,1,  1,1,0,0,  1,1,0,1,  1,1,1,0,  1,1,1,1}; 

// EXEMPLO 
//D3 D2 D1 D0 
// 0  0  0  1 = ENTRADA 02

void setup() { 
  Serial.begin(9800); 
  delay(10); 

  pinMode(pin_mult_D0, OUTPUT); 
  pinMode(pin_mult_D1, OUTPUT); 
  pinMode(pin_mult_D2, OUTPUT); 
  pinMode(pin_mult_D3, OUTPUT); 

  Serial.print("Conectando a rede:");  Serial.println(ssid); 

  WiFi.begin(ssid, senha); 

  while(WiFi.status() != WL_CONNECTED){ 
    delay(500); 
    Serial.print("."); 
  } 
  
  Serial.println("WiFi conectado");   
} 

void loop() { 

  int maiorResultadoPiezo = 0;
  
  LerMultiplexPiezo();

  Serial.println("-> Resultado do piezo: ");
  for(int i=0; i<MAX_PIN_CORTE; i++){ 
    Serial.print("[" + String(sinal_analogico[i]) + "] "); 

    if(sinal_analogico[i] > maiorResultadoPiezo){
      maiorResultadoPiezo = sinal_analogico[i];
    }
  } 

  Serial.println("");
  Serial.println("Maior valor detectado: " + String(maiorResultadoPiezo));

  if(maiorResultadoPiezo >= 5){
    EnviaMensagemNuvem(maiorResultadoPiezo);
  }
  
  delay(1);
} 

void EnviaMensagemNuvem(int valor){

  Serial.println("Enviando dados..");
  WiFiClient cliente; 

  if(cliente.connect(servidor_destino, 80)){
    String postStr = apiKey;
    
    postStr += "&field1=";
    postStr += String(valor);

    postStr += "\r\n\r\n";

    cliente.print("POST /update HTTP/1.1\n");
    cliente.print("Host:api.thingspeak.com\n");
    cliente.print("Connection:close\n");
    cliente.print("X-THINGSPEAKAPIKEY:" + apiKey + "\n");
    cliente.print("Content-Type: application/x-www-form-urlencoded\n");

    cliente.print("Content-Length:");
    cliente.print(postStr.length());
    cliente.print("\n\n");
    cliente.print(postStr);
  }

  cliente.stop();
  Serial.println("Dados enviados para: api.thingspeak.com");

  delay(1000);
}

void LerMultiplexPiezo(){
  for(int i=0; i<MAX_PIN_CORTE; i++){ 
    // Monta a palavra binária para entrada do MULTIPLEX 
    digitalWrite(pin_mult_D0, sinal_bin[i][3]);  
    digitalWrite(pin_mult_D1, sinal_bin[i][2]);  
    digitalWrite(pin_mult_D2, sinal_bin[i][1]);  
    digitalWrite(pin_mult_D3, sinal_bin[i][0]);  

    // Delay para a resposta do MULTIPLEX 
    delay(DELAY_COMUNIC_MULTIPLEX); 

    // Pega resultado do piezoelétrico 
    sinal_analogico[i] = analogRead(A0);   
  }
}
