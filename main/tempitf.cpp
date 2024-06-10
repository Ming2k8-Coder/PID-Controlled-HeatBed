#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "globals.h"
#include "lcd.cpp"
static const char* TAG = "MAX31865";
#include <Adafruit_MAX31865.h>
float RREF = 430.0f;
float RNOMINAL = 100.0f;
// Use software SPI: CS, MOSI, MISO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(5, 23, 19, 18);

void tempinit();
float getTemp();
void faultcheckrtd();

void tempinit()
{
  #ifdef CONFIG_DEBUG
  ESP_LOGD("debug","MAX31865 Init");
  #endif
  thermo.begin(MAX31865_4WIRE);
}
float getTemp()
{
  uint16_t rtd = thermo.readRTD();
  float temp = thermo.calculateTemperature(rtd, RNOMINAL, RREF);
  ESP_LOGD(TAG,"Temperature = %.2f",thermo.temperature(RNOMINAL, RREF));
  #ifdef CONFIG_DEBUG
  ESP_LOGD(TAG,"RTD value: %d ",rtd);
  float ratio = rtd;
  ratio /= 32768;
  ESP_LOGD(TAG,"Ratio = %.8f",ratio);
  ESP_LOGD(TAG,"Resistance = %.8f",RREF*ratio);
  #endif
  return temp;
 
}
void faultcheckrtd(){
  uint8_t fault = thermo.readFault();
  if (fault) {
    ESP_LOGW(TAG,"Fault 0x%x",fault);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      ESP_LOGW(TAG,"RTD High Threshold"); 
      err("Over Temp Protect");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      ESP_LOGW(TAG,"RTD Low Threshold");
      err("Under Temp Protect"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      ESP_LOGW(TAG,"REFIN- > 0.85 x Bias");
      err("TempReadErr(REFL)"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      ESP_LOGW(TAG,"REFIN- < 0.85 x Bias - FORCE- open");
      err("TempReadErr(REFH)"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      ESP_LOGW(TAG,"RTDIN- < 0.85 x Bias - FORCE- open");
      err("TempReadErr(RTDL)"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      ESP_LOGW(TAG,"Under/Over voltage");
      err("Abnormal Volt Temp"); 
    }
    thermo.clearFault();
  }
}