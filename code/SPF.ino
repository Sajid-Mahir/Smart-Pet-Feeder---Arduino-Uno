#include <NewPing.h>
#include <Wire.h>
#include "HX711.h"
#include <Servo.h>
#include <RTClib.h>

RTC_DS3231 rtc;

#define PIR_PIN 2      
#define TRIG_PIN 3   
#define ECHO_PIN 4     
#define SERVO_PIN 5  
#define DT_PIN 6       
#define SCK_PIN 7      

const int ledPin = 13;   
const int ldrPin = A0; 
String time[] = {"8:0", "14:0", "19:0", "22:0"};
int count = 3;
int start = 1 ;

NewPing sonar(TRIG_PIN, ECHO_PIN); 
HX711 scale;  
Servo myservo;  

int pirSensitivity = 500;  
int pirState = LOW;
unsigned long pirCooldown = 10000;  
unsigned long pirCooldownTimer = 0;


void setup() {
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  scale.begin(DT_PIN, SCK_PIN);
  pinMode(ledPin, OUTPUT); 
  pinMode(ldrPin, INPUT);   

  myservo.attach(SERVO_PIN);
  myservo.write(0);  

  delay(1000);     

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  unsigned int distance = sonar.ping_cm();
  int currentPirState = digitalRead(PIR_PIN);

  DateTime now = rtc.now();
  String formattedTime = "";
  formattedTime += String(now.hour(), DEC) + ':';
  formattedTime += String(now.minute(), DEC);

    if (formattedTime == time[count]) {
      count += 1;
      start = 1;
      if (count == 4){
        count = 0;
      }
    }

  if (currentPirState == HIGH && millis() - pirCooldownTimer > pirCooldown) {
    if (pirState == LOW) {
      pirState = HIGH;  
      pirCooldownTimer = millis();  
    }
  } 
  else {
    pirState = LOW; 
  } 

  if (distance <= 30){
    if (pirState == HIGH) {
      if (start == 1){
      moveServo();
      start = 0;
      }
      else{
        Serial.println("Cat already fed");
        Serial.print("Present Time: ");
        Serial.println(formattedTime);
        Serial.print("Next feeding time:");
        Serial.println(time[count]);
      }
    }
  }
  
  int ldrStatus = analogRead(ldrPin);  
  if (ldrStatus <=100) {
    digitalWrite(ledPin, LOW);             
   }
  else {
    digitalWrite(ledPin, HIGH);     
    } 
  delay(1000); 
}

void moveServo() {
  myservo.write(180);  
  delay(1000);         
  myservo.write(0);   
}
