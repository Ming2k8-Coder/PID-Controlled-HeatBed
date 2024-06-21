#ifndef LCD_HDER
#define LCD_HDER
#include "globals.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2, sdapin, sclpin);

typedef enum
{
  STABLE = 0,
  UP = 1,
  DOWN = 2
} TEMP_UP_DOWN_typedef;
void reverseString(char *original, char *reverse, int size);
void initLCDpriv();
void err(const char *messenge);
void showtemp(int temp, TEMP_UP_DOWN_typedef status, int powerpercent, int powerW, int ampe, char *messengees);
void lcdtest();

void reverseString(char *original, char *reverse, int size)
{
  for (int i = 0; i < size; i++)
  {
    reverse[i] = original[size - i - 1];
  }
  reverse[size] = '\0'; // Null-terminate the reversed string
}
void initLCDpriv()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("PID  Heatbed");
  lcd.setCursor(0, 1);
  lcd.print(version);
  lcd.setCursor(9, 1);
  lcd.print("INITING");
  lcd.createChar(0, ThermoIcon);
  lcd.createChar(1, PowerIcon);
  lcd.createChar(2, UpArr);
  lcd.createChar(3, DownArr);
  lcd.createChar(4, StableIcon);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
void err(const char *messenge)
{
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("ERROR");
  lcd.setCursor(0, 1);
  lcd.print(messenge);
  ESP_LOGE("main_task", "ERROR Occured");
  ESP_LOGI("main_task", "ERRor...RST in 5secs");
#ifndef CONFIG_DEBUG
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  abort();
#endif
}

void showtemp(int temp, TEMP_UP_DOWN_typedef status, int powerpercent, int powerW, float ampe, char *messengees)
{
  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.print(":");
  if (temp < 100)
  {
    lcd.print(" ");
  };
  if (temp < 10)
  {
    lcd.print(" ");
  };
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.print(" ");
  if (status == UP)
  {
    lcd.write(2);
  };
  if (status == DOWN)
  {
    lcd.write(3);
  };
  if (status == STABLE)
  {
    lcd.write(4);
  };
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.print(":");
  if (powerpercent < 10)
  {
    lcd.print(" ");
  };
  lcd.print(powerpercent);
  lcd.print("% ");
  if (powerW < 1000)
  {
    lcd.print(" ");
  }
  if (powerW < 100)
  {
    lcd.print(" ");
  }
  if (powerW < 10)
  {
    lcd.print(" ");
  }
  lcd.print(powerW);
  lcd.print("W");
  lcd.print(" ");
  if (ampe > 99)
  {
    ampe = 99;
  };
  if (ampe < 10)
  {
    lcd.print(" ");
  };
  lcd.print(ampe);
  lcd.print("A");
  int size = strlen(messengees);

  char reversed[size + 1]; // +1 for the null terminator
  reverseString(messengees, reversed, size);
  lcd.setCursor(15, 0);
  lcd.rightToLeft();
  lcd.print(reversed);
  lcd.leftToRight();
}

void lcdtest()
{
  uint8_t count = 0;
  int bef;
  while (1)
  {
    if (count < 100)
    {
      showtemp(count, UP, count, count, count, (char *)"REFLOW");
      // setPower(triac1, count);
    };
    if ((count >= 100) && (count < 200))
    {
      showtemp(count, DOWN, count, count, count, (char *)"SOLDER");
      // setPower(triac1, count - 100);
    };
    if (count >= 200)
    {
      showtemp(count, STABLE, count, count, count, (char *)"  AUTO");
    };
    bef = count;
    count += 1;
    if (bef > count)
    {
      showtemp(count, STABLE, count, count, count, (char *)" SYSOK");
      break;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
#endif