#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;
#define SERVER "172.17.123.130"
#define USER "admin"
#define PASS "123"


#define PIR D8
#define LED D0

int hay;
String jwt;
int sesion = 0;
int temporal = -1;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(PIR,INPUT);

  Serial.println("Conectando...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("DARKROW7551", "1234567891");
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {
      Serial.println("Conexion exitosa");
   }
}

void loop()
{
  hay = digitalRead(PIR);
  if(hay != temporal){
    Serial.println(hay);
    if( hay == HIGH ){
      digitalWrite(LED,HIGH);
      delay(1000);
    }else{
      digitalWrite(LED,LOW);
      delay(1000);
    }
    temporal = hay;
    if(sesion == 0) login();
      else post();
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



void post(){
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
    int httpCode = http.POST("sensor=P&estado=" + estado + "&ubicacion=Sala");
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
