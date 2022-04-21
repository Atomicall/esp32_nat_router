#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <u8g2.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

#include "defines.h"

static const char *scrRTAG = "ScreenRoutine";
u8g2_t screen_struct;
xSemaphoreHandle  screen_semaphore;
TaskHandle_t screenRoutineHandle = NULL;


typedef struct {
  u8g2_t* screen;
  xSemaphoreHandle screenSem;
} LCD_struct;


void initSpiAndDisplay();
void screenRoutine(void *ignore);
LCD_struct startScreenRoutine();



void initSpiAndDisplay()
{
  u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
  u8g2_esp32_hal.clk = PIN_CLK;
  u8g2_esp32_hal.mosi = PIN_MOSI;
  u8g2_esp32_hal.cs = PIN_CS;
  u8g2_esp32_hal.reset = PIN_RESET;
  u8g2_esp32_hal_init(u8g2_esp32_hal);
  u8g2_Setup_st7920_s_128x64_f(&screen_struct, U8G2_R0, u8g2_esp32_spi_byte_cb,
                               u8g2_esp32_gpio_and_delay_cb);
  u8g2_InitDisplay(&screen_struct);
  u8g2_SetPowerSave(&screen_struct, 0);
}

void screenRoutine(void *ignore)
{
  //vTaskDelay(5000/portTICK_RATE_MS); // не убирать!
  ESP_LOGI(scrRTAG,"screenRoutine started");
  while (1) {
    if( xSemaphoreTake( screen_semaphore, portMAX_DELAY ) == pdTRUE ){
    ESP_LOGI("sc", "SEM SCR TAKE");
    u8g2_ClearDisplay(&screen_struct);
    u8g2_ClearBuffer(&screen_struct);
    u8g2_SetFont(&screen_struct, u8g2_font_3x5im_te);
    u8g2_DrawStr(&screen_struct, 0, 40, "ME");
    u8g2_SendBuffer(&screen_struct);
    vTaskDelay(1000 / portTICK_RATE_MS);
    ESP_LOGI("sc", "SEM SCR GIVE");
    xSemaphoreGive(screen_semaphore);
    }
    else {
      ESP_LOGI("sc", "SCR SEM TAKE FAIL");
    }
    vTaskDelay(2000/portTICK_RATE_MS);
    
  }
}

LCD_struct startScreenRoutine()
{
  initSpiAndDisplay();
  //семафор и сам ресурс
  LCD_struct lcd;
  lcd.screen = &screen_struct;
  screen_semaphore = xSemaphoreCreateMutex();
  if (!screen_semaphore){
    ESP_LOGI(scrRTAG,"Failed to vSemaphoreCreateBinary(lcd.screenSem); ");
  }
  lcd.screenSem = screen_semaphore;
  if (xTaskCreate(screenRoutine,
                  "Screen_routine",
                  4000,
                  NULL,
                  10,
                  &screenRoutineHandle) == pdTRUE)
  {
    ESP_LOGI(scrRTAG, "ScreenRoutine started");
  }
  else
  {
    ESP_LOGI(scrRTAG, "Failed to start ScreenRoutine");
  }
  return lcd;
}
