//#include <PS4Controller.h>
#include <Arduino.h>
#include <CAN.h>
#include "tuushin.h"
#include "encoder.h"
#include "functions_kai.h"

// エンコーダ関連のピン設定 CPPにあり。
// カウント値をグローバルに定義済み

uint32_t id;          // CAN IDを格納する変数
uint16_t data[8]={0,0,0,0,0,0,0,0};      // 受信データを格納する配列（最大8バイト）
uint16_t length=0;       // 受信データの長さを格納する変数

// 許容誤差と移動距離目標
const int allowableError = 5;
// エンコーダ関連の設定
const float wheelDiameter = 80.0;
const float encoderPulsesPerRevolution = 750;
const float distancePerCount = (PI * wheelDiameter) / encoderPulsesPerRevolution;

// setup関数: 初期設定を行う。CANバスの初期化と、送受信の設定を呼び出す
void setup() {
  
  Serial.begin(115200);  // シリアル通信開始
   while (!Serial);  // シリアル接続待機

const int CAN_TX_PIN = 27;  // 送信ピン（GPIO27）
const int CAN_RX_PIN = 26;  // 受信ピン（GPIO26）

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
  Serial.println("Start");

//エンコーダ準備 エンコーダピンの設定
  for (int i = 0; i < 2; i++){
    pinMode(encoderA[i], INPUT_PULLUP);
    pinMode(encoderB[i], INPUT_PULLUP);
  }
  // 割り込みの設定
  attachInterrupt(digitalPinToInterrupt(encoderA[0]), []() { handleEncoder(0); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderA[1]), []() { handleEncoder(1); }, CHANGE);
  /*attachInterrupt(digitalPinToInterrupt(encoderA[2]), []() { handleEncoder(2); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderA[3]), []() { handleEncoder(3); }, CHANGE);*/
}


// loop関数: 受信と送信を繰り返す
void loop() {
  
receivePacket(id, data, length);
// CANメッセージを受信

//Serial.print("NEXT！");

int packetSize = CAN.parsePacket();
if (receivePacket) { 
    Serial.print(data[0]);
    //Serial.print(data[1]);データないやつ入れちゃうとエラーして落ちちゃう
    //Serial.print(data[2]);Serial.print(data[3]);
    Serial.println();

// デバッグ用：エンコーダカウントを出力
  for (int i = 0; i < 2; i++)
  {
    Serial.print("Encoder ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(encoderCount[i]);
  }
//encoderCount[0]が右後ろ　encoderCount[1]が左後ろ

//移動関数
bool reachedTarget = true;

    for (int i = 0; i < 4; i++) {
        float currentDistance = encoderCount[i] * distancePerCount;
        float controlSignal = pidCompute(i, targetDistance[i], currentDistance);
       /* if(controlSignal<100.0){
          controlSignal=0.0;
          }*/
        Serial.printf("%f ",currentDistance);
        Serial.printf("%f ",targetDistance[i]);
        Serial.printf("%f \n",controlSignal);
        driveMotor(i, controlSignal);
        if (abs(currentDistance - targetDistance[i]) > allowableError) {
            reachedTarget = false;
        }
    }

    if (reachedTarget) {
        stopMotors();
        resetControlVariables();
        /*
        //ここでTargetも0にする
         for (int j = 0; j < 4; j++) {
        targetDistance[j]=0.0;
         }
        Serial.print("reachedTarget\n");*/
        handleMoterInput(targetDistance, data[0]);
    }
 
for (int i = 0; i < 1; i++) {
    data[i] = 0;
    }
}
 delay(40);  // 1秒の遅延
}

/*f (PS4.isConnected()) {
           // handlePS4Input();//動きを説明する関数（もし入力が入ったら？）
            handlePS4Input(targetDistance);//動きを説明する関数（もし入力が入ったら？）
        }*/
