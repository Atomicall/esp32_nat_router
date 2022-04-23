#ifndef B831F4D3_DBBE_4D91_BDC2_F89A1FB162BC
#define B831F4D3_DBBE_4D91_BDC2_F89A1FB162BC
#include "main_menu.h"
#include "esp_log.h"
#include "u8g2_esp32_hal.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"

#include "structs.h"


void _startMenuLogicTask(void* thiS);
void menuLogicRoutine();
void readUserInput(menu_logic* thiS, UserInterract* uInp);
void updateCurrentScreen(menu_logic* thiS);
bool generateMenu(menu_logic* thiS, UserInterract* uInp);

//1
menu_logic* constructMenu_logicStruct(LCD_struct lcdS, QueueHandle_t encEvQ, QueueHandle_t buttEvQ){
menu_logic* ptr = (menu_logic*)malloc(sizeof(menu_logic));
ptr->encoderEventsQueue = encEvQ;
ptr->buttonsEventsQueue = buttEvQ;
ptr->lcdStruct = lcdS;
ptr->currentMenu  = constructMain_menu_Struct(ptr->lcdStruct.screen);
ptr->startMenuLogicTask = &_startMenuLogicTask;
return ptr;
}
//2
void _startMenuLogicTask(void* thiS){
    //thiS  - готовая сконструированная структура
if (xTaskCreate(menuLogicRoutine,
                    "menuLogicRoutine",
                    10000,
                    //arg - thiS
                    thiS,
                    1,
                    NULL) == pdTRUE)
    {
        ESP_LOGI("menuLogicRoutine", "menuLogicRoutine started");
    }
    else
    {
        ESP_LOGI("menuLogicRoutine", "Failed to start menuLogicRoutine");
    }

}


//3
void menuLogicRoutine(void* argThis){
    menu_logic* thiS = (menu_logic*)argThis;
    UserInterract userInput;
    while(1){
        readUserInput(thiS, &userInput);
        // туу уже имеем нажатые кнопки и направление и позицию энкодера
        bool changes = generateMenu(thiS, &userInput);
        if (changes) updateCurrentScreen(thiS);
    }
}


//4
//где-то тут. Надо делать чтение пользовательского ВВОДА!
void readUserInput(menu_logic* thiS, UserInterract* uInp){
    //ESP_LOGI("readUserInput", "Entered readUserInput, tr to recieve enc Ev");
    uInp->enc_button = 0;
    if (xQueueReceive(thiS->encoderEventsQueue, &uInp->recievedEnc_event, portMAX_DELAY) != pdTRUE){
        ESP_LOGI("readUserInput", "enc Timeout");
    };
} 
 // 5
bool generateMenu(menu_logic* thiS, UserInterract* uInp) {
    // стате машине тут?
    static uint8_t encCurrentPosition = 0;
    static uint8_t encPreviousPosition = 1;
    static bool enc_buttonCurrent = 0;
    static bool enc_buttonPrevious = 1;

    static bool checkedFieldsFlags_Edisons [8] =  {0};
    static bool checkedFieldsFlags_MenuTitles[6] = {0};
     encPreviousPosition = encCurrentPosition;
    encCurrentPosition = uInp->recievedEnc_event.state.position;
    if (encCurrentPosition == encPreviousPosition && enc_buttonPrevious == enc_buttonCurrent){
        return 0;
    } 
    switch (thiS->currentMenu->menu_type){
        case MAIN_MENU:
        { 
            uint8_t StatusSectoinPosition = encCurrentPosition % 8;
            memset(checkedFieldsFlags_Edisons, 0, 8);
            checkedFieldsFlags_Edisons[StatusSectoinPosition] = 1;
            thiS->currentMenu->constructBuffer(thiS->lcdStruct.screen, checkedFieldsFlags_Edisons);
            break;
        }
        case EDISSON_STATUS_MENU:
        {
        
        break;
        }
        default: break;
    }

    return 1;
    //thiS->currentMenu->constructBuffer(thiS->lcdStruct.screen);
}


//6
void updateCurrentScreen(menu_logic* thiS) {
    xSemaphoreTake(thiS->lcdStruct.screenSem, portMAX_DELAY);
    u8g2_SendBuffer(thiS->lcdStruct.screen);
    xSemaphoreGive(thiS->lcdStruct.screenSem);
}







#endif /* B831F4D3_DBBE_4D91_BDC2_F89A1FB162BC */
