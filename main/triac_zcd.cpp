#ifndef TRIAC_ZCD
#define TRIAC_ZCD
#include "globals.h"
#include "lcd.cpp"
#include "driver/pulse_cnt.h"
#include "tempitf.cpp"
#include "adcitf.cpp"
dimmertyp *triac1;
TaskHandle_t readinfo = NULL;
float voltage = 0;
float current = 0;
float power = 0;
int powerpercent = 1;
float temp = 0;
int mains_freq = 0;

int adcurrmax = 0;
int adcvoltmax = 0;

int pval = 1;
int ival = 1;
int dval = 1;
// triac2
void initACctl();
void faultcheck();
void faultchecktos(void *arg);
void readsensor();
void pwrcheckinit();

float getVoltage();
float getCurrent();
// main
float getVoltage()
{
    float volt =0;
    //some calculation
    adcvoltmax = 0;
    return volt;
    return -1;
}
float getCurrent()
{   
    float curr = 0;
    // acs712 adc read
    adcurrmax = 0;
    return curr;
    return -1;
}
void readsensor()
{   
    adc();
    voltage = getVoltage();
    current = getCurrent();
    power = voltage*current;
    temp = getTemp();
    ESP_LOGI("sensor","Voltage(U): %.2f V; Current(I): %.2f A; Power(P): %.2f W; Temp(T): %.2f 'C",voltage,current,power,temp);
}

pcnt_unit_handle_t pcnt_unit = NULL;
void pwrcheckinit()
{
    tempinit();
    pcnt_unit_config_t unit_config = {
        .low_limit = -10,
        .high_limit = 1000,

    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = zero_cross,
        .level_gpio_num = -1,
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
    // pcntchan set : pointer,action on rising edge,action on falling edge
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    // pcnt_del_channel(pcnt_chan);
    // pcnt_unit_disable(pcnt_unit);
    // pcnt_del_unit(pcnt_unit);
}
void faultcheck()
{
    
    readsensor();
    // get Freq main block
    pcnt_unit_start(pcnt_unit);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    pcnt_unit_stop(pcnt_unit);
    pcnt_unit_get_count(pcnt_unit, &mains_freq);
    pcnt_unit_clear_count(pcnt_unit);
    // end block
    mains_freq = mains_freq / 2;
    ESP_LOGD("PWR_Check", "Mains freq: %3dHz", mains_freq);
    if (mains_freq < 10){
        #ifndef CONFIG_DEBUG
        err("Undr Frq Protect");
        #endif
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Freq override 50");
        vTaskDelay(200/portTICK_PERIOD_MS);
        mains_freq = 50;
    }
    #ifndef CONFIG_DEBUG
    faultcheckrtd();
    if (voltage == -1){err("Volt Sens Error");}
    if (voltage < 110){err("Undr Volt Protect");}
    if (voltage > ovv){err("Over Volt Protect");}
    if (current == -1){err("Curr Sens Error");}
    if (current > ovc){err("Over Curr Error");}
    if (mains_freq > ovf){err("Over Frq Protect");}
    #endif
    
}
void faultcheckrtos(void *arg){
    while(1){
    faultcheck();
        vTaskDelay(20000/portTICK_PERIOD_MS);
    }
}
/*Wrapper to create dimmer, call from outside*/
void initACctl()
{
    triac1 = createDimmer(triacio, zero_cross);
    begin(triac1, NORMAL_MODE, ON, mains_freq);
    setPower(triac1, 10);
}
#endif