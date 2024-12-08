#include "encoder.h"

// 外部で使用される変数を宣言
//extern volatile int encoderCount[4]; 
volatile int encoderCount[2] = {0, 0};// カウント値

// エンコーダ関連のピン設定
//extern const int encoderA[4] = {18, 25, 22, 32}; // エンコーダA相ピン
const int encoderA[2] = {19, 23}; // エンコーダA相ピン
const int encoderB[2] = {18, 22}; // エンコーダB相ピン

// エンコーダのカウント値を更新する関数
void handleEncoder(int index)
{
  if (index < 2)  // 左側のモーターの場合
  {
    if (digitalRead(encoderA[index]) != digitalRead(encoderB[index]))
    {
      encoderCount[index]++;
    }
    else
    {
      encoderCount[index]--;
    }
  }
  else  // 右側のモーターの場合
  {
    if (digitalRead(encoderA[index]) == digitalRead(encoderB[index]))
    {
      encoderCount[index]++;
    }
    else
    {
      encoderCount[index]--;
    }    
  }
}

/**/
