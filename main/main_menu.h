#include "u8g2_esp32_hal.h"
#include "screen_routine.h"
#define BIG_FONT_X_OFFSET 4
#define BIG_FONT_Y_OFFSET 6
#define LITTLE_FONT_X_OFFSET 3
#define LITTLE_FONT_Y_OFFSET 5

#include "esp_log.h"

enum {
    OK=0,
    ERROR
} edissonStatus;


typedef struct main_menu
{
   u8g2_t* screen;
   void (*constructBuffer)(void);
   void (*printStatusString) (u8g2_t* s,uint8_t xB, unt8_t yB, bool cursor, uint8_t edissonNumber, edissonStatus status);
   void (*_printMenuEntry)(u8g2_t* s,uint8_t xB, unt8_t yB, bool cursor, char* title);

};

void _printStatusString(u8g2_t* s, uint8_t xB, unt8_t yB, bool cursor, uint8_t edissonNumber, edissonStatus status) // левый верхний угол{
        uint8_t xSelector = xB;
        uint8_t ySelector = yB + BIG_FONT_Y_OFFSET;
        //ESP_LOGI("statusString", "Set to begin of string (l_d)");
        u8g2_SetFont(s, u8g2_font_4x6_mf);
        u8g2_DrawStr(s, xSelector, ySelector, (cursor)?">":" ");
        char[10] c = {"Edisson:"};
        itoa(edissonNumber,c[8]));
        u8g2_DrawStr(s, xSelector+2+BIG_FONT_X_OFFSET-1, 
                    ySelector+2+BIG_FONT_Y_OFFSET-1, c);
        u8g2_SetFont(s, u8g2_font_3x5im_mr);
        u8g2_DrawStr(s, xSelector+14, ySelector+6, (edissonStatus == STATUS::OK)?"Ok" : "ERRR");
    
    }
 void _printMenuEntry(u8g2_t* s, uint8_t xB, unt8_t yB, bool cursor, char* title){
        uint8_t xSelector = xB;
        uint8_t ySelector = yB + LITTLE_FONT_Y_OFFSET;
        u8g2_SetFont(s, u8g2_font_3x5im_mr);
        u8g2_DrawStr(s, xSelector, ySelector, (cursor)?">":" ");
        xSelector+=LITTLE_FONT_X_OFFSET;
        u8g2_DrawStr(s, xSelector, ySelector, title);
    }
void _constructBuffer(){
        // x y левый верхний угол
        uint8_t [] statusPositions = {
            4, 2,
            4, 18, 
            4, 34,
            4, 50,
            60, 2,
            60, 18,
            60, 34,
            60, 50
        }
        uint8_t [] menuTitlesPositions = {
            108, 4,
            108, 14, 
            108, 24,
            108, 34,
            108, 44,
            108, 54,
        }
        char** menuTitles[][] = {
            {"Tit1"},
            {"Tit2"},
            {"Tit3"},
            {"Tit4"},
            {"Tit5"},
            {"Tit6"}
        }
        for (uint8_t = 0; i< 16; i+=2){
            _printStatusString(statusPositions[i], statusPositions[i+1],
            1, i/2, OK);
        }
        u8g2_DrawFrame(s, 106, 2, 20, 60);
        for (uint8_t = 0; i < 12; i+=2){
            _printMenuEntry(menuTitlesPositions[i], menuTitlesPositions[i+1], 
            1, menuTitles[i]);
        }
    }
main_menu* constructMain_menu_Struct(u8g2_t* s){
    main_menu* prt = malloc (sizeof(main_menu));
    p->screen = s;
    p->constructBuffer = &_constructBuffer;
    p->

}



