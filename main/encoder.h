#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "rotary_encoder.h"
#include "defines.h"


#define ENABLE_HALF_STEPS false // Set to true to enable tracking of rotary encoder at half step resolution
#define RESET_AT 0              // Set to a positive non-zero number to reset the position if this value is exceeded
#define FLIP_DIRECTION false    // Set to true to reverse the clockwise/counterclockwise sense

static const char *encTAG = "EndoderRoutine";

QueueHandle_t encoderEventsQueue = NULL;

void initEncoder();
QueueHandle_t getEncoderEventsQueue();
void encoderRoutine(void *v);
QueueHandle_t startEncoderRoutine();
rotary_encoder_info_t info = {0};

void initEncoder()
{
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
    ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif
    encoderEventsQueue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, encoderEventsQueue));
}

QueueHandle_t getEncoderEventsQueue()
{
    return encoderEventsQueue;
}

void encoderRoutine(void *v)
{
    rotary_encoder_state_t state = {0};
    rotary_encoder_event_t event = {0};
    while (1)
    {
        if (xQueueReceive(encoderEventsQueue, &event, 1000 / portTICK_PERIOD_MS) == pdTRUE)
        {
            ESP_LOGI(encTAG, "Event: position %d, direction %s", event.state.position,
                     event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
        }
        else
        {
            rotary_encoder_state_t state = {0};
            ESP_ERROR_CHECK(rotary_encoder_get_state(&info, &state));
            ESP_LOGI(encTAG, "Poll: position %d, direction %s", state.position,
                     state.direction ? (state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");

            // Reset the device
            if (RESET_AT && (state.position >= RESET_AT || state.position <= -RESET_AT))
            {
                ESP_LOGI(encTAG, "Reset");
                ESP_ERROR_CHECK(rotary_encoder_reset(&info));
            }
        };
    }
};

QueueHandle_t startEncoderRoutine()
{
    initEncoder();
    if (xTaskCreate(encoderRoutine,
                    "EncoderRoutine",
                    3000,
                    NULL,
                    15,
                    NULL) == pdTRUE)
    {
        ESP_LOGI(encTAG, "EncoderRoutine started");
    }
    else
    {
        ESP_LOGI(encTAG, "Failed to start EncoderRoutine");
    }
    return getEncoderEventsQueue();
}
