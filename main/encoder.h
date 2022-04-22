#ifndef F186D620_C588_4E5E_820F_87EC28A6BAD1
#define F186D620_C588_4E5E_820F_87EC28A6BAD1
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "defines.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/queue.h"
#include "rotary_encoder.h"

#define ENABLE_HALF_STEPS \
  false  // Set to true to enable tracking of rotary encoder at half step
         // resolution
#define RESET_AT \
  1  // Set to a positive non-zero number to reset the position if this value is
     // exceeded
#define FLIP_DIRECTION \
  false  // Set to true to reverse the clockwise/counterclockwise sense

static const char *encTAG = "EndoderRoutine";
//переписать на структуры
QueueHandle_t encoderEventsQueue = NULL;
void initEncoder();
QueueHandle_t getEncoderEventsQueue();
void encoderRoutine(void *v);
QueueHandle_t startEncoderRoutine();
rotary_encoder_info_t info = {0};

void initEncoder() {
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
  ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
  ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif
  encoderEventsQueue = rotary_encoder_create_queue();
  ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, encoderEventsQueue));
}

QueueHandle_t getEncoderEventsQueue() { return encoderEventsQueue; }

void encoderRoutine(void *v) {
  rotary_encoder_event_t *event =
      (rotary_encoder_event_t *)malloc(sizeof(rotary_encoder_event_t));
  rotary_encoder_state_t state = {0};
  while (1) {
    rotary_encoder_get_state(&info, &state);
    
    if (state.position < 0){
        event->state.position = 23;
        info.state.position = 23;
    }
    else if (state.position > 24){
        event->state.position = 0;
        info.state.position = 0;
    }
    else{
        event->state.position = state.position;
    }
    event->state.direction = state.direction;
    // ESP_LOGI(encTAG, "Event: position %d, direction %s", event->state.position,
    //          event->state.direction
    //              ? (event->state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE
    //                     ? "CW"
    //                     : "CCW")
    //              : "NOT_SET");
    xQueueSend(encoderEventsQueue, (void *)event, portMAX_DELAY);
  }
}

QueueHandle_t startEncoderRoutine() {
  initEncoder();
  if (xTaskCreate(encoderRoutine, "EncoderRoutine", 3000, NULL, 2, NULL) ==
      pdTRUE) {
    ESP_LOGI(encTAG, "EncoderRoutine started");
  } else {
    ESP_LOGI(encTAG, "Failed to start EncoderRoutine");
  }
  return getEncoderEventsQueue();
}

#endif /* F186D620_C588_4E5E_820F_87EC28A6BAD1 */
