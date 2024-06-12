#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define PIEZO_PIN 3
#define NotOrganize 5 // fail 12번
#define Organize 6 // succes 13번

// 초음파센서 2개
#define cho1_echo 7
#define cho1_trig 8
#define cho2_echo 9
#define cho2_trig 10

// 레이저
#define laser_square 11

bool isPerson = false;
bool cho2_activate = false;
int cnt1 = 0;
int cnt2 = 0;
static int score = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);
  pinMode(cho1_trig, OUTPUT);     
  pinMode(cho1_echo, INPUT);

  pinMode(cho2_trig, OUTPUT);     
  pinMode(cho2_echo, INPUT);     

  pinMode(laser_square,OUTPUT);

  pinMode(PIEZO_PIN,OUTPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {

  digitalWrite(cho1_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(cho1_trig, LOW);

  unsigned long duration1 = pulseIn(cho1_echo, HIGH);

  float distanceCM1 = ((34000 * duration1) / 1000000) / 2;

  //초음파1 작동
  cho1Activate(distanceCM1); // 사람 들어오는 것 감지
  
  //초음파2 작동
  if(cho2_activate == true) {
    cho2Activate(); // 신발 정리 감지
  }

  // 점수판
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SCORE");
  lcd.setCursor(0, 1);
  lcd.print(score);

  delay(100);
  
}

void cho1Activate(float distanceCM1) {
  if(distanceCM1 <= 80) { // 사람 들어옴
    if(cnt1 >= 6) {
      cho2_activate = true;
      //레이저 사각형 켜짐
      digitalWrite(laser_square, HIGH);
    }
    else cnt1 = cnt1 + 6;
  }
  else { // 사람 없음
    if(cnt1 == 0) {
      
    }
    else cnt1--;
  }
}

void cho2Activate() {
  int endcnt2 = 100;
  int cnt2 = 0;

  while(cho2_activate == true && endcnt2 > 0) {
    digitalWrite(cho2_trig, HIGH);
    delayMicroseconds(10);

    digitalWrite(cho2_trig, LOW);
    unsigned long duration2 = pulseIn(cho2_echo, HIGH);
    float distanceCM2 = ((34000 * duration2) / 1000000) / 2;

    if (distanceCM2 <= 60) // 신발 정리중인 사람
    {
      if(cnt2 >= 2)
      {
        isPerson = true; // 사람있음
      }
      else cnt2 = cnt2 + 2;
    }
    else 
    {
      if(cnt2 == 0)
      {
        if(isPerson == true) { // 신발 정리 후 지나간 사람

          // 파이참으로 신발 정리상태 받아오기
          int isOrganize = digitalRead(Organize);
          int isNotOrganize = digitalRead(NotOrganize);

          delay(2000);

          //결과
          if(isOrganize) {
            succes();
          }
          else if(isNotOrganize) {
            fail();
          }
          else {

          }
          //결과 받고 레이저 중지
          digitalWrite(laser_square, 0);
          isPerson = false;
          cho2_activate = false;
        }
      }
      else cnt2--;
    }
    endcnt2--;
    delay(100);
  }
  cho2_activate = false;
  digitalWrite(laser_square, LOW);
}

void succes() {
  succes_melody();
  score++;
}

void fail() {
  fail_melody();
  score = score - 2;
  if(score < 0) {
    score = 0;
  }
}

void succes_melody() {
  Serial.println("성공");

  tone (PIEZO_PIN, 262, 150); //도
  delay(150);
  tone (PIEZO_PIN, 329.6, 150); //미
  delay(150);
  tone (PIEZO_PIN, 392.0, 150); //솔
  delay(150);
  tone (PIEZO_PIN, 523.0, 150); //도~~ 5옥타브
}

void fail_melody() {
  tone (PIEZO_PIN, 262, 300); //도
}