//#include <PS4Controller.h>
#include <Arduino.h>
#include <ESP32Servo.h>  // ESP32用のサーボライブラリ
#include <CAN.h>
#include "tuushin.h"  // tuushin.hをインクルード
#include "PWM.h"//PWM関連は別ファイルにした
//Tougou
#include "souten.h" 
#include "gyoukaku.h"

uint32_t id;          // CAN IDを格納する変数
uint16_t data[8]={0,0,0,0,0,0,0,0};      // 受信データを格納する配列（最大8バイト）
uint16_t length=0;       // 受信データの長さを格納する変数

const int PIN_SYASYUTU_PWM = 16;  // 射出のPWM
const int PIN_SYASYUTU_1 = 4;
const int PIN_SYASYUTU_2 = 21;
//21がLOW　　４　HIGH　16 PWM

int syasyutu_condition = 0;

// 目標電圧（ここに外部からの値が設定される）
float targetVoltage = 3.5;      // 初期値として3.5Vを設定
// 電圧範囲
const float maxVoltage = 8.0;   // 最大電圧
const float minVoltage = 0.0;   // 最小電圧

const int PIN_SYASYUTU = 5;  // 
int dutyCycle = calculateDutyCycle(targetVoltage, maxVoltage, minVoltage);
//Max=255とした計算

extern Servo soutenServo; // 変数は外部で定義されていると宣言
int souten_servoPin = 13;  // サーボの接続ピンを指定（適宜変更）

Servo gyoukakuServo; // 変数は外部で定義されていると宣言
int gyoukaku_servoPin = 12;  // 仰角用サーボの接続ピンを指定（適宜変更）
//int currentAngle = 0;        // サーボの初期角度

// setup関数: 初期設定を行う。CANバスの初期化と、送受信の設定を呼び出す
void setup() {
  
  Serial.begin(115200);  // シリアル通信開始
   while (!Serial);  // シリアル接続待機

 const int CAN_TX_PIN = 27;  // 送信ピン（GPIO27）
const int CAN_RX_PIN = 26;  // 受信ピン（GPIO26）

//ピン設定 
  pinMode(souten_servoPin,OUTPUT);
  pinMode(gyoukaku_servoPin,OUTPUT);
  pinMode(PIN_SYASYUTU_PWM,OUTPUT);
  pinMode(PIN_SYASYUTU_1,OUTPUT);
  pinMode(PIN_SYASYUTU_2,OUTPUT);

//サーボピン初期設定
soutenServo.attach(souten_servoPin);  // サーボピンを設定
soutenServo.write(45);  // 初期位置を20度（中央）に設定

//仰角用サーボピン初期設定
gyoukakuServo.attach(gyoukaku_servoPin);  // サーボピンを設定
gyoukakuServo.write(45);  // 初期位置を20度（中央）に設定

  Serial.println("CAN Communication");
  CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);
  // CANバスの初期化（通信速度500kbps）
  CAN.begin(500E3);
  // CANバスの初期化。500kbpsで動作を設定
  if (!CAN.begin(500E3)) {
    // CAN初期化に失敗した場合、エラーメッセージを表示して停止
    Serial.println("CANの初期化に失敗しました！");
    while (1);  // 永久ループで停止
  }

  // 受信と送信の初期化関数を呼び出し
  setupReceiver();
  //setupSender();
  Serial.println("Start");
}


// loop関数: 受信と送信を繰り返す
void loop() {
  
receivePacket(id, data, length);
// CANメッセージを受信

int packetSize = CAN.parsePacket();
if (receivePacket) { 
    Serial.print(data[0]);
    Serial.print(data[1]);
    Serial.print(data[2]);
    Serial.print(data[3]);

    Serial.println();

   /* if(data[0]==1){//これでHIGHにする
        analogWrite(PIN_SYASYUTU, dutyCycle );
        Serial.print("PWM");
      }else{
        digitalWrite(PIN_SYASYUTU,LOW);
        Serial.print("LOW");
        }*/
        if (data[0]==1){//発射
      Serial.println("Circle Button");      //Serial.print("PWM_syasyutu!");
      if(syasyutu_condition==0){
        syasyutu_condition=1;
      }else{
        syasyutu_condition=0;  
      }
      Serial.printf("%d",syasyutu_condition);
      if(syasyutu_condition==0){
        digitalWrite(PIN_SYASYUTU_PWM, LOW);//回転時間ってどんくらいですか？Dutyサイクルは先に回っています
        //analogWrite(PIN_SYASYUTU_1,dutyCycle);
        digitalWrite(PIN_SYASYUTU_1,HIGH);
        digitalWrite(PIN_SYASYUTU_2,LOW);
      }else{
        Serial.printf("%d",dutyCycle); 
        //analogWrite(PIN_SYASYUTU_PWM, dutyCycle );
        //Dutyサイクルは先に回っています
        //analogWrite(PIN_SYASYUTU_PWM,dutyCycle);
        digitalWrite(PIN_SYASYUTU_PWM,HIGH);
        digitalWrite(PIN_SYASYUTU_1,HIGH);
        //analogWrite(PIN_SYASYUTU_1,dutyCycle);
        digitalWrite(PIN_SYASYUTU_2,LOW);
      }
      }    
    if(data[1]==1){//これでHIGHにする
      /* // analogWrite(PIN_SYASYUTU, dutyCycle );
       Serial.println("装填開始");
      // Souten();
      
      Souten();
      
      data[1]=0; // 動作が完了したらPS4_Triangleを0に戻す
      Serial.println("装填終了");
      }else{
        //digitalWrite(PIN_SYASYUTU,LOW);
        Serial.print("LOW");*/
        Serial.println("Triangle Button");//Debug  Serial.println("装填開始");
      Souten();
      //digitalWrite(PIN_SYASYUTU,LOW);
      digitalWrite(souten_servoPin,LOW);
      Serial.println("装填終了");
        }
    if(data[2]==1){//これでHIGHにする
       Serial.println("仰角+1");
      movegyoukakuServoBy(1); // 現在の角度から1度動かす (+1°)
      delay(40);
      }else{
        }
    if(data[3]==1){//これでHIGHにする
       Serial.println("仰角-1");
       movegyoukakuServoBy(-1);// 現在の角度から1度動かす (+1°)
      delay(40);
      
      }else{
        }
     
 
for (int i = 0; i < 8; i++) {
    data[i] = 0;
    }
}
 //delay(40);  // 1秒の遅延
}
