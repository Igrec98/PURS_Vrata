#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>

Servo myservo;  // create servo object to control a servo

int angle =30;    // startna pozicija serva (za provjeru)
int angleStep =5;
WebServer server(80); //inicijalizacija servera

//POCETAK IR-------------------------------------------------
#define LEDPIN 23
#define SENSORPIN 15

//POCETAK MAGNET---------------------------------------------
const int REED_PIN = 18;	// Pin magneta


void otvaraj(){
    if (angle >= 0 && angle <= 180) {
      for (; angle < 180; angle += 1) { // otvaranje po stupnjevima
        myservo.write(angle); // pomicanje serva
        Serial.print("Moved to: ");
        Serial.print(angle);   // ispis za konzolu po stupnju
        Serial.println(" degree");
      }
       if(angle < 0){
        angle = 0;
       }else{
      myservo.write(angle);
      Serial.print("Moved to: ");
      Serial.print(angle);
      Serial.println(" degree");
       }
    }
  server.send(200, "application/json", "{\"status\":\"Vrata su otvorena, Dobro Dosli!\"}");  // status za server
  delay(100);
}

void zatvaraj(){
     if (angle >= 0 && angle <= 180) {
      for(; angle > 0; angle -= 1) { // zatvaranje po stupnjevima
       if(digitalRead(SENSORPIN) == LOW){ // slučaj za objekt izmedu vrata
          server.send(200, "application/json", "{\"status\":\"Smetnja izmedu vrata, ponovno otvaranje!\"}");  // status za server
          otvaraj();
          return;
        }
        myservo.write(angle);
        Serial.print("Moved to: ");
        Serial.print(angle);
        Serial.println(" degree");
      }
      if(angle >180){  // osiguranje od preotvaranja
        angle =180;
       }else{
      myservo.write(angle);
      Serial.print("Moved to: ");
      Serial.print(angle);
      Serial.println(" degree");
       }
    }
  server.send(200, "application/json", "{\"status\":\"Vrata su zatvorena!\"}"); // status za server
          otvaraj();
  delay(100); 
}

void setup_routing() {
  server.on("/otvaraj", HTTP_GET, otvaraj);
  server.on("/zatvaraj", HTTP_GET, zatvaraj); 
  // start server
  server.begin();
}

void setup() {
  //SEVRO-------------------------------------------------------------------------------------
  Serial.begin(9600);
  myservo.attach(13);  // pin signala serva
  myservo.write(angle);// Porvjera serva, setup na 30 stupnja

  //IR----------------------------------------------------------------------------------------
  // inicijalizacija detektora
  pinMode(LEDPIN, OUTPUT);      
  pinMode(SENSORPIN, INPUT_PULLDOWN);

//WIFI----------------------------------------------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin("A1_1695575729", "Mateji33");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  setup_routing();


 Serial.println("Sigurnosna Vrata Vsix ");
}


void loop() {
    server.handleClient();
// Serial.print(digitalRead(SENSORPIN));  provjera senzora (detektora)
}