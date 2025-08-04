#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

// Ultrasonic constants
#define TRIG_PIN  18
#define ECHO_PIN  19
#define SOUND     0.0343

// Task Handle
TaskHandle_t thGetDistance = NULL;

// Calculator
int iDistanceCalc(float fDuration);

// configuration
void vPinConfig();

// Task Prototype
void vGetDistance(void *pArg);



void app_main(void) {
vPinConfig();

xTaskCreatePinnedToCore(vGetDistance, "Get Distance", 4096, NULL, 10, &thGetDistance, 1);

}


// configuration
void vPinConfig() {
    gpio_config_t io1_conf = {
        .pin_bit_mask  = (1ULL << TRIG_PIN),
        .mode          = GPIO_MODE_OUTPUT,
        .pull_up_en    = GPIO_PULLUP_DISABLE,
        .pull_down_en  = GPIO_PULLDOWN_DISABLE,
        .intr_type     = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&io1_conf));
    ESP_LOGI("TRIGGER", "Configured Successively");


    gpio_config_t io2_conf = {
        .pin_bit_mask  = (1ULL << ECHO_PIN),
        .mode          = GPIO_MODE_INPUT,
        .pull_up_en    = GPIO_PULLUP_DISABLE,
        .pull_down_en  = GPIO_PULLDOWN_DISABLE,
        .intr_type     = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&io2_conf));
    ESP_LOGI("ECHO", "Configured Successively");
}

// Calculator
int iDistanceCalc(float fDuration) {
    float fDistance = (SOUND * fDuration)/2.0f;

    return (int)fDistance;
}


// Task Implementation
void vGetDistance(void *pArg) {
    // timing variables
        int iEchoStart    = 0;
        int iEchoEnd      = 0;
        int iDuration     = 0;
        int iStartTime    = 0;


        while(1) {
            // Emmiting US radiations
                ESP_ERROR_CHECK(gpio_set_level(TRIG_PIN, 1));
                esp_rom_delay_us(10);                                // create a delay of 10microsec
                ESP_ERROR_CHECK(gpio_set_level(TRIG_PIN, 0));

            // implementing Task operations
                iStartTime = esp_timer_get_time();
                while(gpio_get_level(ECHO_PIN) == 0) {
                    if((esp_timer_get_time() - iStartTime) > 100000) { 
                        ESP_LOGE("ECHOLOW", "time out");
                        break;
                    }
                }

                iEchoStart = esp_timer_get_time();
                while(gpio_get_level(ECHO_PIN) == 1) {
                    if((esp_timer_get_time() - iEchoStart) > 300000) { 
                        ESP_LOGE("ECHOHIGH", "time out");
                        break;
                    }
                }

                iEchoEnd = esp_timer_get_time();

                iDuration = iEchoEnd - iEchoStart;

            ESP_LOGI("DISTANCE", "value : %d", iDistanceCalc((float)iDuration));
            vTaskDelay(pdMS_TO_TICKS(10));
                     
        }



}










