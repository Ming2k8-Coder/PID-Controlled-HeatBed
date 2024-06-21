#ifndef MANUALCPP
#define MANUALCPP
#include "triac_zcd.cpp"
#include "globals.h"
#include "menu.cpp"
void powerManual(void *arg)
{
  float lasttemp;
  bool passingloop = true;
  int dispmode = 0;
  lcd.clear();
  while (1)
  {
    readrotary();
    if (middle)
    {
      middle = false;
      lcd.clear();
      vTaskResume(menu_hd);
      vTaskSuspend(NULL);
    }
    if (up)
    {
      powerpercent++;
      up = false;
      passingloop = false;
    }
    if (down)
    {
      powerpercent--;
      down = false;
      passingloop = false;
    }
    if (passingloop)
    {
      if (powerpercent > 99)
      {
        powerpercent = 99;
      }
      if (powerpercent < 0)
      {
        powerpercent = 0;
      }
      lasttemp = temp;
      if ((temp - lasttemp) > 0.05)
      {
        dispmode += 10;
      }
      else if ((temp - lasttemp) < -0.05)
      {
        dispmode -= 10;
      }
      if (dispmode > 0)
      {
        showtemp(static_cast<int>(temp), UP, powerpercent, static_cast<int>(power), current, (char *)"MANUAL");
        dispmode -= 1;
      }
      else if (dispmode < 0)
      {
        showtemp(static_cast<int>(temp), DOWN, powerpercent, static_cast<int>(power), current, (char *)"MANUAL");
        dispmode += 1;
      }
      else
      {
        showtemp(static_cast<int>(temp), STABLE, powerpercent, static_cast<int>(power), current, (char *)"MANUAL");
      }
      ESP_LOGW("debugimport", "dispmode %d", dispmode);
      setPower(triac1, powerpercent);
    }
    else
    {
      passingloop = true;
    }
  }
}
#endif