#ifndef BDA8E329_81E3_43FB_A785_16A6F541EED7
#define BDA8E329_81E3_43FB_A785_16A6F541EED7

#include "rotary_encoder.h"
#include "u8g2_esp32_hal.h"

enum MENU_TYPES { MAIN_MENU, EDISSON_STATUS_MENU };

typedef struct {
  uint8_t menu_type;
  u8g2_t* screen;
  void (*constructBuffer)(u8g2_t* s, void* args);
} menu_screen;

typedef struct {
  u8g2_t* screen;
  xSemaphoreHandle screenSem;
} LCD_struct;

typedef struct {
  QueueHandle_t encoderEventsQueue;
  QueueHandle_t buttonsEventsQueue;
  LCD_struct lcdStruct;
  menu_screen* currentMenu;
  // передаем подготовленную структуру
  void (*startMenuLogicTask)(void*);
} menu_logic;

typedef struct {
  bool enc_button;
  rotary_encoder_event_t recievedEnc_event;

} UserInterract;

#endif /* BDA8E329_81E3_43FB_A785_16A6F541EED7 */
