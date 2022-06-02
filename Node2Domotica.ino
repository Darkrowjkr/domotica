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

#define LEDentrada D0
#define LEDgarage D1
#define SERVOentrada D4
#define SERVOgarage D5
#define SERVOporton D6
#define ECHOentrada D2
#define ECHOcuarto D7
#define TRIGentrada D3
#define TRIGcuarto D8

Servo serEnt, serGar, serPor;

String jwt;
int timeEntrada, timeCuarto;
int distEntrada, distCuarto;
int sesion = 0;
int tempEntrada = -1;
int tempGarage = -1;
int tempCuarto = -1;
String estado = "";

void setup()
{
  Serial.begin(9600);
  pinMode(LEDentrada, OUTPUT);
  pinMode(LEDgarage, OUTPUT);
  pinMode(TRIGentrada, OUTPUT);
  pinMode(ECHOentrada, INPUT);
  digitalWrite(TRIGentrada, LOW);
  digitalWrite(TRIGcuarto, LOW);
  serEnt.attach(SERVOentrada,540,2400);
  serEnt.write(0);
  serGar.attach(SERVOgarage,540,2400);
  serGar.write(0);
  serPor.attach(SERVOporton,540,2400);
  serPor.write(0);
  Serial.println("Conectando...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("DARKROW7551", "1234567891");
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {
      Serial.println("Conexion exitosa");
   }
}

void loop()
{
  serEnt.write(0);
  serGar.write(0);
  serPor.write(0);
  digitalWrite(TRIGentrada, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGentrada, LOW);
  timeEntrada = pulseIn(ECHOentrada, HIGH);
  distEntrada = timeEntrada/58.4;
  Serial.println("Entrada:");
  Serial.println(distEntrada);
  
  if(distEntrada <= 20){
      digitalWrite(LEDentrada,HIGH);
      serEnt.write(180);
      if(sesion == 0){
        login();
      } else{ 
        estado = "encendido";
        post("D","Entrada");
      }
      delay(5000);
      digitalWrite(LEDentrada,LOW);
      serEnt.write(0);
      if(sesion == 0){
        login();
      } else{ 
        estado = "apagado";
        post("D","Entrada");
      }
      delay(3000);
  }else if(distEntrada > 20 && distEntrada <= 50){
      serPor.write(180);
      delay(2000);
      digitalWrite(LEDgarage,HIGH);
      serGar.write(180);
      if(sesion == 0){
        login();
      } else{ 
        estado = "encendido";
        post("D","Garage");
      }
      delay(5000);
      digitalWrite(LEDgarage,LOW);
      serGar.write(0);
      serPor.write(0);
      if(sesion == 0){
        login();
      } else{ 
        estado = "apagado";
        post("D","Garage");
      }
      delay(3000);
  }
  delay(100);
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
    http.begin(client, "http://" SERVER "/domotica/registro.php"); //HTTP
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", jwt);
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
