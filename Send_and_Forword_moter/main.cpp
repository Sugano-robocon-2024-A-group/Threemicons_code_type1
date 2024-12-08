#include <PS4Controller.h>
#include <Arduino.h>
#include <CAN.h>
#include <ESP32Servo.h>  // ESP32用のサーボライブラリ
#include "tuushin.h"  // tuushin.hをインクルード
#include "encoder.h"
//#include "functions.h"//運転関連のものはここに入っている。

//使用ボタン設定
/* int PS4_Circle=0;
  int PS4_Triangle=0;
  int PS4_R1=0;
  int PS4_L1=0;*/

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
Serial.println("Start"); 打ちまくる系OK⇒送信系⇒Encoder読み⇒前進コード
/**/
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

      
  Ashimawari_Command=0;//初期化

}

  delay(150);  // 0.15秒の遅延
}

/*
*/
