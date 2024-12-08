#include <PS4Controller.h>
#include <Arduino.h>
#include <CAN.h>
#include <ESP32Servo.h>  // ESP32用のサーボライブラリ
#include "tuushin.h"  // tuushin.hをインクルード
#include "PWM.h"//PWM関連は別ファイルにした
#include "souten.h" 
#include "gyoukaku.h"
#include "encoder.h"
//#include "functions.h"//運転関連のものはここに入っている。

//使用ボタン設定
  int PS4_Circle=0;
  int PS4_Triangle=0;
  int PS4_R1=0;
  int PS4_L1=0;

// 目標電圧（ここに外部からの値が設定される）
float targetVoltage = 3.5;      // 初期値として3.5Vを設定
// 電圧範囲
const float maxVoltage = 5.0;   // 最大電圧
const float minVoltage = 0.0;   // 最小電圧

const int PIN_SYASYUTU = 5;  // 
int dutyCycle = calculateDutyCycle(targetVoltage, maxVoltage, minVoltage);
//Max=255とした計算

extern Servo soutenServo; // 変数は外部で定義されていると宣言
int souten_servoPin = 13;  // サーボの接続ピンを指定（適宜変更）

Servo gyoukakuServo; // 変数は外部で定義されていると宣言
int gyoukaku_servoPin = 12;  // 仰角用サーボの接続ピンを指定（適宜変更）
//int currentAngle = 0;        // サーボの初期角度

int Ashimawari_Command=0;//コマンド

  
// setup関数: 初期設定を行う。CANバスの初期化と、送受信の設定を呼び出す
void setup() {
  //シリアル通信、PS4準備
  Serial.begin(115200);  // シリアル通信開始
  PS4.begin("1c:69:20:e6:20:d2");//ここにアドレス
  Serial.println("Ready.");
  while (!Serial);  // シリアル接続待機


//CAN設定
const int CAN_TX_PIN = 27;  // 送信ピン（GPIO27）
const int CAN_RX_PIN = 26;  // 受信ピン（GPIO26）
  Serial.println("CAN Communication");
  CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);
  CAN.begin(500E3);// CANバスの初期化（通信速度500kbps）
  if (!CAN.begin(500E3)) {
    Serial.println("CANの初期化に失敗しました！"); // CAN初期化に失敗した場合、エラーメッセージを表示して停止
    while (1);  // 永久ループで停止
  }

  // 受信と送信の初期化関数を呼び出し
  //setupReceiver();
  setupSender();
  Serial.println("Start");

  //エンコーダ準備 エンコーダピンの設定
  for (int i = 0; i < 2; i++){
    pinMode(encoderA[i], INPUT_PULLUP);
    pinMode(encoderB[i], INPUT_PULLUP);
  }
  // 割り込みの設定
  attachInterrupt(digitalPinToInterrupt(encoderA[0]), []() { handleEncoder(0); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderA[1]), []() { handleEncoder(1); }, CHANGE);
}

// loop関数 やること　CAN送信、（前輪Encoder読み、前輪回転）、いろいろやる。
void loop() {
//Serial.println("Start"); 打ちまくる系OK⇒送信系⇒Encoder読み⇒前進コード
if (PS4.isConnected()) {
    if (PS4.Circle()){//発射
      Serial.println("Circle Button");      //Serial.print("PWM_syasyutu!");
      analogWrite(PIN_SYASYUTU, dutyCycle );//回転時間ってどんくらいですか？Dutyサイクルは先に回っています
      delay(40);
      digitalWrite(PIN_SYASYUTU,LOW);
    }
    if (PS4.Triangle()) {//装填
      Serial.println("Triangle Button");//Debug  Serial.println("装填開始");
      Souten();
      digitalWrite(souten_servoPin,LOW);
      Serial.println("装填終了");
      }
    if (PS4.R1()){
      Serial.println("仰角+1");
      movegyoukakuServoBy(1); // 現在の角度から1度動かす (+1°)
      delay(40);
      }
     if (PS4.L1()){
      Serial.println("仰角-1");
       movegyoukakuServoBy(-1);// 現在の角度から1度動かす (+1°)
      delay(40);
      }

  // 送信処理を実行
  if (PS4.Right()){Ashimawari_Command=3;
      }
      if (PS4.Down()){Ashimawari_Command=2;
      }
      if (PS4.Up()){Ashimawari_Command=1;
      }
      if (PS4.Left()){Ashimawari_Command=4;
      }
      if (PS4.UpRight()){Ashimawari_Command=5;
      }
      if (PS4.DownRight()){Ashimawari_Command=6;
      }
      if (PS4.UpLeft()){Ashimawari_Command=7;
      }
      if (PS4.DownLeft()){Ashimawari_Command=8;
      }
  Serial.printf("%d\n",Ashimawari_Command);//Debug
      
  sendPacket(Ashimawari_Command);
  Ashimawari_Command=0;//初期化

  //ここで動作処理をする。
  //Encoder読み
// デバッグ用：エンコーダカウントを出力
  for (int i = 0; i < 2; i++)
  {
    Serial.print("Encoder ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(encoderCount[i]);
  }
//encoderCount[0]が右後ろ　encoderCount[1]が左後ろ

//移動関数.これはこっちはAshimawari_Commandなどでうごかして
    if(Ashimawari_Command==1){//これでHIGHにする
        //analogWrite(PIN_SYASYUTU, dutyCycle );
        Serial.print("UP");
      }else if(Ashimawari_Command==2){
        //digitalWrite(PIN_SYASYUTU,LOW);
        Serial.print("BACK");
      }

}

  delay(150);  // 0.15秒の遅延
}

/*
 if(data[0]==1){//これでHIGHにする
        analogWrite(PIN_SYASYUTU, dutyCycle );
        Serial.print("PWM");
      }else{
        digitalWrite(PIN_SYASYUTU,LOW);
        Serial.print("LOW");
        }
    if(data[1]==1){//これでHIGHにする
       // analogWrite(PIN_SYASYUTU, dutyCycle );
       Serial.println("装填開始");
      // Souten();
      
      Souten();
      
      data[1]=0; // 動作が完了したらPS4_Triangleを0に戻す
      Serial.println("装填終了");
      }else{
        digitalWrite(PIN_SYASYUTU,LOW);
        Serial.print("LOW");
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


        
void loop() {
  // Below has all accessible outputs from the controller
  if (PS4.isConnected()) {

    if (PS4.Square()) {
      Serial.println("Square Button");
      
    }
    if (PS4.Cross()) Serial.println("Cross Button");
    if (PS4.Circle()){
      Serial.println("Circle Button");
      PS4_Circle=1;
      Serial.println("%d",PS4_Circle);
    }
    if (PS4.Triangle()) Serial.println("Triangle Button");

    if (PS4.Charging()) Serial.println("The controller is charging");
    if (PS4.Audio()) Serial.println("The controller has headphones attached");
    if (PS4.Mic()) Serial.println("The controller has a mic attached");

    Serial.printf("Battery Level : %d\n", PS4.Battery());

    
    
    Serial.println();
    // This delay is to make the output more human readable
    // Remove it when you're not trying to see the output
  }
  
//ここで

  
    delay(1000);
  
}*/
