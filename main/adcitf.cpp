/*
    note:
    1)Dùng adc oneshot 
    - lắp diode và tụ điện, volt divisor
    - code easy
    - diode đểu nghi hạ volt
    - read peak only
    2)Dùng adc continuos
    - lắp qua volt divisor (limit max 3.3)
    - code hard
    -read peak-0;peak-peak,waveform
*/
#ifndef ADCITF
#define ADCITF
#include "globals.h"
#include "esp_adc/adc_continuous.h"
#include "soc/soc_caps.h"
//extern QueueHandle_t zerocross_evt;
#define EXAMPLE_ADC_UNIT ADC_UNIT_1
#define _EXAMPLE_ADC_UNIT_STR(unit) #unit
#define EXAMPLE_ADC_UNIT_STR(unit) _EXAMPLE_ADC_UNIT_STR(unit)
#define EXAMPLE_ADC_CONV_MODE ADC_CONV_SINGLE_UNIT_1
#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_0
#define EXAMPLE_ADC_BIT_WIDTH SOC_ADC_DIGI_MAX_BITWIDTH

#define EXAMPLE_ADC_OUTPUT_TYPE ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define EXAMPLE_ADC_GET_CHANNEL(p_data) ((p_data)->type1.channel)
#define EXAMPLE_ADC_GET_DATA(p_data) ((p_data)->type1.data)

#define EXAMPLE_READ_LEN 256

static adc_channel_t channel[2] = {voltage_sense, current_sense};

static TaskHandle_t s_task_handle;
static const char *TAG2 = "ADC";

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    // Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}

static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    adc_continuous_handle_t handle = NULL;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = EXAMPLE_READ_LEN,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20 * 1000,
        .conv_mode = EXAMPLE_ADC_CONV_MODE,
        .format = EXAMPLE_ADC_OUTPUT_TYPE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    adc_pattern[0].atten = VOLTAGE_ATTEN;
    adc_pattern[0].channel = channel[0] & 0x7;
    adc_pattern[0].unit = EXAMPLE_ADC_UNIT;
    adc_pattern[0].bit_width = EXAMPLE_ADC_BIT_WIDTH;
    adc_pattern[1].atten = CURRENT_ATTEN;
    adc_pattern[1].channel = channel[1] & 0x7;
    adc_pattern[1].unit = EXAMPLE_ADC_UNIT;
    adc_pattern[1].bit_width = EXAMPLE_ADC_BIT_WIDTH;
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    *out_handle = handle;
}
esp_err_t ret;
uint32_t ret_num = 0;
adc_continuous_handle_t handle = NULL;
uint8_t result[EXAMPLE_READ_LEN] = {0};

void adcinit()
{
    memset(result, 0xcc, EXAMPLE_READ_LEN);
    s_task_handle = xTaskGetCurrentTaskHandle();

    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &handle);

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(handle));
}
void adc()
{

    /**
     * This is to show you the way to use the ADC continuous mode driver event callback.
     * This `ulTaskNotifyTake` will block when the data processing in the task is fast.
     * However in this example, the data processing (print) is slow, so you barely block here.
     *
     * Without using this event callback (to notify this task), you can still just call
     * `adc_continuous_read()` here in a loop, with/without a certain block timeout.
     */
    //uint32_t io_num;
    //if (xQueueReceive(zerocross_evt, &io_num, portMAX_DELAY))
    //{
       // adcvoltmax = 0;
      //  adcurrmax = 0;
    //}
    ret = adc_continuous_read(handle, result, EXAMPLE_READ_LEN, &ret_num, 0);
    if (ret == ESP_OK)
    {ESP_LOGI(TAG2,"ADC READ OK");
        for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES)
        {
            adc_digi_output_data_t *p = (adc_digi_output_data_t *)&result[i];
            uint32_t chan_num = EXAMPLE_ADC_GET_CHANNEL(p);
            int chan_numi = static_cast<int>(chan_num);
            uint32_t data = EXAMPLE_ADC_GET_DATA(p);
            int datai = static_cast<int>(data);
            //ESP_LOGI(TAG2, "Channel: %d, Value:%d", chan_numi, datai);
            if (chan_numi == 0) // voltage
            {
                if (datai > adcvoltmax)
                {
                    adcvoltmax = datai;
                }
            }
            if (chan_numi == 3) // curr
            {
                if (datai > adcurrmax)
                {
                    adcurrmax = datai;
                }
            }
            /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
            // if (chan_num < SOC_ADC_CHANNEL_NUM(EXAMPLE_ADC_UNIT))
            //{
            //  ESP_LOGI(TAG2, "Unit: %s, Channel: %" PRIu32 ", Value: %" PRIx32, unit, chan_num, data);
            //}
        }
    }
}
#endif