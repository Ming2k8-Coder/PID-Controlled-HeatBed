#include "Arduino.h"
#include <esp_log.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "esp_adc/adc_continuous.h"

#include "globals.h"
#include "lcd.cpp"
#include "triac_zcd.cpp"
#include "menu.cpp"
#include "manual.cpp"
#include "pidalgo.cpp"
#include "adcitf.cpp"
void lcdtest();
void rotarytest();
void powerManual(void *arg);

// mainfunc
extern "C" void app_main()
{
  // init
  initArduino();
  initLCDpriv();
  ESP_LOGD("Init", "Arduino core + LCD OK");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  //------------------
  // ADC
  lcd.setCursor(5, 1);
  lcd.print("ADC-AC INIT");
  adcinit();
  ESP_LOGD("Init", "ADC-AC OK");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // adcend---------------
  // Power Checking
  lcd.setCursor(5, 1);
  lcd.print("  PWR CHECK");
  pwrcheckinit();
  vTaskDelay(700 / portTICK_PERIOD_MS);
  lcd.setCursor(6, 1);
    lcd.print("DET ");
    if (mains_freq < 100)
    {
        lcd.print(" ");
        if (mains_freq < 10)
        {
            lcd.print(" ");
        };
    };
    lcd.print(mains_freq);
    lcd.print(" ");
    lcd.print("Hz");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd.setCursor(11, 1);
    if (voltage < 100)
    {
        lcd.print(" ");
        if (voltage < 10)
        {
            lcd.print(" ");
        };
    };
    lcd.print(static_cast<int>(voltage));
    lcd.print(" ");
    lcd.print("V");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    faultcheck();

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // init triac
  lcd.setCursor(6, 1);
  lcd.print("TRIAC INIT");
  initACctl();
  // ok
  lcd.setCursor(6, 1);
  lcd.print(" SYSTEM OK");
  pinMode(rotarysw, INPUT_PULLUP);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  lcd.clear();
  initrotary();
  //rotarytest();
  // Do your own thing
  ESP_LOGI("main_task", "All_Sys_Online");
 // adc();
  xTaskCreate(faultcheckrtos, "Checking Power", 4096, NULL, 1,&pwr_chk);
  vTaskSuspend(pwr_chk);
  xTaskCreate(menuloop, "Menu_Display", 2048, NULL, 2, &menu_hd);
  vTaskSuspend(menu_hd);
  xTaskCreate(powerManual, "Manual Power", 4096, NULL, 3, &pwr_man);
  vTaskSuspend(pwr_man);
  vTaskResume(menu_hd);

  // lcdtest();
  // err("Manual Triggered");
}
//---------------------------------------------

//----------------------------------------------
void rotarytest()
{
  while (1)
  {
    rotary_encoder_state_t state = {0, ROTARY_ENCODER_DIRECTION_NOT_SET};
    ESP_ERROR_CHECK(rotary_encoder_get_state(&info, &state));
    lcd.setCursor(0, 0);
    if (state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE)
    {
      lcd.print("CLWS");
    }
    if (state.direction == ROTARY_ENCODER_DIRECTION_COUNTER_CLOCKWISE)
    {
      lcd.print("CCWS");
    }
    if (state.direction == ROTARY_ENCODER_DIRECTION_NOT_SET)
    {
      lcd.print("IDLE");
    }

    lcd.setCursor(0, 1);

    lcd.print(state.position);
    rotary_encoder_reset(&info);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
