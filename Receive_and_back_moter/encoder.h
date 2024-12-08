//void handleEncoder(int index);

#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

// 外部で使用される変数を宣言
extern volatile int encoderCount[2]; // カウント値（main.cppで定義される）

extern const int encoderA[2]; // エンコーダA相ピン
extern const int encoderB[2];

// 関数のプロトタイプ宣言
void handleEncoder(int index);

#endif
