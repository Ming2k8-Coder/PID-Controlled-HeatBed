#pragma once
#ifndef glo
#define glo
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "dimmer/dimmer.h"
#include "rotary_lib/rotary_encoder.h"
#define version "V0.1"
#define CONFIG_DEBUG

TaskHandle_t menu_hd = NULL;
TaskHandle_t pwr_man = NULL;
TaskHandle_t pwr_chk = NULL;
TaskHandle_t sensor = NULL;

#define VOLTAGE_ATTEN ADC_ATTEN_DB_0 //CAN NOT USE DIFF ATEEN
#define CURRENT_ATTEN ADC_ATTEN_DB_0 //
#define voltage_sense ADC_CHANNEL_0 // GPIO36 SP
#define current_sense ADC_CHANNEL_3 // GPIO39 SN

#define sdapin 21
#define sclpin 22
#define zero_cross GPIO_NUM_33
#define triacio GPIO_NUM_25
#define rotarysw 0
#define rotaryDT GPIO_NUM_4  // pin B
#define rotaryCLK GPIO_NUM_2 // pin A

extern int pval;
extern int ival;
extern int dval;

extern int adcvoltmax;
extern int adcurrmax;

extern int mains_freq;
extern float voltage;
extern float current;
extern float power;
extern float temp;
extern int powerpercent;

#define ovt 400 //temp limit 400C
#define ovc 20 //current limit 20A
#define ovv 250 //Voltage limit 250V
#define udv 150 //voltage min 150V
#define ovf 100 //frequentcy limit 100Hz (theorical 1kHz)


#define ENABLE_HALF_STEPS true // Set to true to enable tracking of rotary encoder at half step resolution
#define RESET_AT 0             // Set to a positive non-zero number to reset the position if this value is exceeded
#define FLIP_DIRECTION false   // Set to true to reverse the clockwise/counterclockwise sense

extern TaskHandle_t menu_hd;
extern TaskHandle_t pwr_man;
extern TaskHandle_t readinfo;
uint8_t ThermoIcon[8] = {
    0B01101,
    0B01110,
    0B01101,
    0B01110,
    0B01101,
    0B01100,
    0B10010,
    0B01100,
};

uint8_t PowerIcon[8] = {
    0B00010,
    0B00110,
    0B01100,
    0B11111,
    0B00110,
    0B01100,
    0B01000,
    0B00000,
};
uint8_t UpArr[8] = {
    0B00100,
    0B01110,
    0B11111,
    0B01110,
    0B01110,
    0B01110,
    0B01110,
    0B00100,
};
uint8_t DownArr[8] = {
    0B00100,
    0B01110,
    0B01110,
    0B01110,
    0B01110,
    0B11111,
    0B01110,
    0B00100,
};
uint8_t StableIcon[8] = {
    0B00000,
    0B00000,
    0B01010,
    0B10101,
    0B10001,
    0B01010,
    0B00100,
    0B00000,
};
#endif