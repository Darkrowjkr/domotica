#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Servo.h>


ESP8266WiFiMulti WiFiMulti;
//#define SERVER "pw4.kyared.com/S18030221"
#define SERVER "172.17.123.130"
#define USER "admin"
#define PASS "123"

#define LEDsala D1
#define LEDcuarto D2
#define PIRsala D8
#define PIRcuarto D7

int hay, HAYsala, HAYcuarto;

String jwt;
int sesion = 0;
int tempSala = -1, tempCuarto = -1;

void setup()
{
  Serial.begin(9600);
  pinMode(LEDsala, OUTPUT);
  pinMode(LEDcuarto, OUTPUT);
  pinMode(PIRsala,INPUT);
  pinMode(PIRcuarto,INPUT);
  Serial.println("Conectando...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("DARKROW7551", "1234567891");
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {
      Serial.println("Conexion exitosa");
   }
}

void loop()
{
  HAYsala = digitalRead(PIRsala);
  HAYcuarto = digitalRead(PIRcuarto);
  if(HAYsala != tempSala){
    Serial.println("Sala");
    Serial.println(HAYsala);
    if( HAYsala == HIGH ){ //PIR de la sala, prende LEDS
      digitalWrite(LEDsala,HIGH);
      delay(1000);
    }else{
      digitalWrite(LEDsala,LOW);
      delay(1000);
    }
    tempSala = HAYsala;
    if(sesion == 0){
      login();
    } else{ 
      hay = tempSala;
      post("P","Sala");
    }
  }
  if(HAYcuarto != tempCuarto){
    Serial.println("Cuarto");
    Serial.println(HAYcuarto);
    if( HAYcuarto == HIGH ){ //PIR de la sala, prende LEDS
      digitalWrite(LEDcuarto,HIGH);
      delay(1000);
    }else{
      digitalWrite(LEDcuarto,LOW);
      delay(1000);
    }
    tempCuarto = HAYcuarto;
    if(sesion == 0){
      login();
    } else{ 
      hay = tempCuarto;
      post("P","Cuarto");
    }
  }
  delay(10000);
}

void login(){
  WiFiClient client;
  HTTPClient http;
  if(http.begin(client, "http://" SERVER "/domotica/login.php?user="USER"&pass="PASS"")){
    int httpCode = http.GET();
    if(httpCode > 0){
      if(httpCode == HTTP_CODE_OK){
        String json = http.getString();
        Serial.println(json);
        int i,j;
        String r = json.substring(10,11);
        Serial.println(r);
        if(r == "Y"){
          int inicio = json.indexOf("token");
          int fin = json.indexOf("}");
          jwt = json.substring(inicio+8,fin-1);
          sesion = 1;
          Serial.println("Login exitoso");
        }else{
          Serial.println("Login fallido");
        }
      }
    }else{
      Serial.println("Error GET");
    }
  }
}



void post(String sensor, String ubic){
    WiFiClient client;
    HTTPClient http;
    String estado;
    http.begin(client, "http://" SERVER "/domotica/registro.php"); //HTTP
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", jwt);
    if(hay==0){
      estado = "apagado";
    }else{
      estado = "encendido";
    }
    int httpCode = http.POST("sensor=" + sensor + "&estado=" + estado + "&ubicacion=" + ubic);
    Serial.println(jwt);
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
        const String& payload = http.getString();
        Serial.println(payload);
      }else{
        Serial.println("ERROR HTTP post");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}
