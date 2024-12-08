#ifndef FUNCTIONS_H
#define FUNCTIONS_H

extern const uint8_t motor_pwm[4];
extern const uint8_t motor_dir1[4];
extern const uint8_t motor_dir2[4];
/*
extern const int encoderA[4];
extern const int encoderB[4];*/

extern volatile int encoderCount[4];
//extern float kp[4], ki[4], kd[4];
extern float targetDistance[4];
extern float integral[4], previous_error[4];

void handleEncoder(int index);
void init_pwm_setup();
void init_encoder_setup();
float pidCompute(int index, float target, float current);
void driveMotor(int index, float controlSignal);
void stopMotors();
void resetControlVariables();
//void handlePS4Input();
//void handlePS4Input(*targetDistance[0],*targetDistance[1],*targetDistance[2],*targetDistance[3]);
void handlePS4Input(float* targetDistance);

#endif
