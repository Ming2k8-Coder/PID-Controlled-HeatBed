#ifndef MENU
#define MENU
#include "lcd.cpp"
#include "triac_zcd.cpp"
#include "globals.h"
rotary_encoder_info_t info = {};

int menuitem = 1;
int frame = 1;
int page = 1;
int lastMenuItem = 1;
int pidmode = 0;

const char *item1 = "Manual Power";
const char *item2 = "PID Temp CTL";
const char *item3 = "Set P";
const char *item4 = "Set I";
const char *item5 = "Set D";
const char *piditem1 = "Target Temp";
const char *piditem2 = "Reflow ISO";
// const char* item6 = "Display ";

// extern bool up = false;
// extern bool down = false;
// extern bool middle = false;
bool up = false;
bool down = false;
bool middle = false;
// declare
void initrotary();
void readrotary();
void displayMenuItem(const char *item, int position, bool selected);
void displayIntMenuPage(const char *menuItem, int value);
void drawMenu();
bool rotarypressed();
// void IRAM_ATTR rotaryisr();
void menuloop(void *arg);

void readrotary()
{
    rotary_encoder_state_t state = {0,ROTARY_ENCODER_DIRECTION_NOT_SET};
    ESP_ERROR_CHECK(rotary_encoder_get_state(&info, &state));
    lcd.setCursor(0, 0);
    if (state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE)
    {
        // lcd.print("CLWS");
        up = true;
    }
    if (state.direction == ROTARY_ENCODER_DIRECTION_COUNTER_CLOCKWISE)
    {
        // lcd.print("CCWS");
        down = true;
    }
    if (state.direction == ROTARY_ENCODER_DIRECTION_NOT_SET)
    {
        // lcd.print("IDLE");
    }

    // lcd.setCursor(0, 1);

    // lcd.print(state.position);
    rotary_encoder_reset(&info);
    // vTaskDelay(500 / portTICK_PERIOD_MS);
}

void menuloop(void *arg)
{
    while (1)
    {
        // vTaskDelay(500/portTICK_PERIOD_MS);
        readrotary();
        if (up || down)
        {
            lcd.clear();
        } /*
         lcd.setCursor(15, 0);
         lcd.print(frame);
         lcd.setCursor(13, 1);
         lcd.print(menuitem);
         lcd.print(" ");
         lcd.print(lastMenuItem);
 */
        if (down && page == 1) // We have turned the Rotary Encoder Clockwise
        {

            down = false;
            if (menuitem == 3 && lastMenuItem == 2)
            {
                frame++;
            }
            else if (menuitem == 4 && lastMenuItem == 3)
            {
                frame++;
            }
            else if (menuitem == 5 && lastMenuItem == 4 && frame < 4)
            {
                frame++;
            }
            lastMenuItem = menuitem;
            menuitem++;
            if (menuitem >= 6)
            {
                menuitem = 5;
            }
        }
        else if (down && page == 2 && menuitem == 2)
        {
            pidmode = 1;
        }
        else if (down && page == 2 && menuitem == 3)
        {
            down = false;
            pval++;
            if (pval <= 0)
            {
                pval = 1;
            }
            // setP(pval);
        }
        else if (down && page == 2 && menuitem == 4)
        {
            down = false;
            ival++;
            if (ival <= 0)
            {
                ival = 1;
            }
            // setI(ival);
        }
        else if (down && page == 2 && menuitem == 5)
        {
            down = false;
            dval++;
            if (dval <= 0)
            {
                dval = 1;
            }
            // setD(dval);
        }

        if (up && page == 1)
        {

            up = false;
            if (menuitem == 2 && frame == 2)
            {
                frame--;
            }

            if (menuitem == 4 && frame == 4)
            {
                frame--;
            }
            if (menuitem == 3 && frame == 3)
            {
                frame--;
            }
            lastMenuItem = menuitem;
            menuitem--;
            if (menuitem <= 0)
            {
                menuitem = 1;
            }
        }
        else if (up && page == 2 && menuitem == 2)
        {
            pidmode = 0;
        }
        else if (up && page == 2 && menuitem == 3)
        {
            up = false;
            pval--;
            if (pval <= 0)
            {
                pval = 1;
            }
            // setP(pval);
        }
        else if (up && page == 2 && menuitem == 4)
        {
            up = false;
            ival--;
            if (ival <= 0)
            {
                ival = 1;
            }
            // setI(ival);
        }
        else if (up && page == 2 && menuitem == 5)
        {
            up = false;
            dval--;
            if (dval <= 0)
            {
                dval = 1;
            }
            // setD(dval);
        }

        if (middle) // Middle Button is Pressed
        {
            middle = false;
            lcd.clear();
            if (page == 1 && menuitem == 1) // Backlight Control
            {
                // trigger func manual power
                vTaskResume(pwr_man);
                vTaskSuspend(menu_hd);
            }

            else if (page == 2 && menuitem == 2)
            {
                if (pidmode == 0)
                {
                    // tringger pid manual temp
                }
                else if (pidmode == 1)
                {
                    // tringger pid reflow program
                }
            }
            else if (page == 1 && menuitem >= 2)
            {
                page = 2;
            }
            else if (page == 2)
            {
                page = 1;
            }
        }
        drawMenu();
    }
}
void drawMenu()
{
    // lcd.clear();
    if (page == 1)
    {

        if (menuitem == 1 && frame == 1)
        {
            displayMenuItem(item1, 1, true);
            displayMenuItem(item2, 2, false);
        }
        else if (menuitem == 2 && frame == 1 && lastMenuItem != 3)
        {
            displayMenuItem(item1, 1, false);
            displayMenuItem(item2, 2, true);
        }

        else if (menuitem == 3 && frame == 1)
        {
            displayMenuItem(item2, 1, false);
            displayMenuItem(item3, 2, true);
        }
        else if (menuitem == 2 && frame == 1 && lastMenuItem == 3)
        {
            displayMenuItem(item2, 1, true);
            displayMenuItem(item3, 2, false);
        }
        else if (menuitem == 4 && frame == 2)
        {
            displayMenuItem(item3, 1, false);
            displayMenuItem(item4, 2, true);
        }
        else if (menuitem == 3 && frame == 2 && lastMenuItem == 4)
        {
            displayMenuItem(item3, 1, true);
            displayMenuItem(item4, 2, false);
        }
        else if (menuitem == 5 && frame == 3)
        {
            displayMenuItem(item4, 1, false);
            displayMenuItem(item5, 2, true);
        }
        else if (menuitem == 4 && frame == 3 && lastMenuItem == 5)
        {
            displayMenuItem(item4, 1, true);
            displayMenuItem(item5, 2, false);
        }
        else if (menuitem == 5 && frame == 4)
        {
            displayMenuItem(item4, 1, false);
            displayMenuItem(item5, 2, true);
        }
        else if (menuitem == 4 && frame == 4)
        {
            displayMenuItem(item4, 1, true);
            displayMenuItem(item5, 2, false);
        }
        else if (menuitem == 3 && frame == 3)
        {
            displayMenuItem(item3, 1, true);
            displayMenuItem(item4, 2, false);
        }
        else if (menuitem == 2 && frame == 2)
        {
            displayMenuItem(item2, 1, true);
            displayMenuItem(item3, 2, false);
        }
        lcd.display();
    }
    else if (page == 2 && menuitem == 2)
    {
        if (pidmode == 0)
        {
            displayMenuItem(piditem1, 1, true);
            displayMenuItem(piditem2, 2, false);
        }
        else if (pidmode == 0)
        {
            displayMenuItem(piditem1, 1, false);
            displayMenuItem(piditem2, 2, true);
        }
    }
    else if (page == 2 && menuitem == 3)
    {
        displayIntMenuPage(item3, pval);
    }

    else if (page == 2 && menuitem == 4)
    {
        displayIntMenuPage(item4, ival);
    }
    else if (page == 2 && menuitem == 5)
    {
        displayIntMenuPage(item5, dval);
    }
}

void displayIntMenuPage(const char *menuItem, int value)
{
    lcd.setCursor(0, 0);
    lcd.print(menuItem);
    lcd.print(":");
    lcd.print(value);
}
void displayMenuItem(const char *item, int position, bool selected)
{
    int pos = position - 1;
    lcd.setCursor(0, pos);
    if (selected)
    {
        lcd.print(">");
    }
    else
    {
        lcd.print(" ");
    }
    lcd.print(item);
}

void IRAM_ATTR rotaryisr()
{
    middle = true;
}
bool rotarypressed()
{
    if (middle)
    {
        return true;
    }
    else
    {
        return false;
    }
    middle = false;
}
void initrotary()
{

    attachInterrupt(rotarysw, rotaryisr, ONLOW);
    gpio_install_isr_service(0);
    ESP_ERROR_CHECK(rotary_encoder_init(&info, rotaryCLK, rotaryDT));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
    ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif

    // Create a queue for events from the rotary encoder driver.
    // Tasks can read from this queue to receive up to date position information.
    // QueueHandle_t event_queue = rotary_encoder_create_queue();
    // ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, event_queue));
}
#endif