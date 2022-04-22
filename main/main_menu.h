#ifndef FC078A03_CFC5_4D0C_8D3A_3E62FD9E6BB5
#define FC078A03_CFC5_4D0C_8D3A_3E62FD9E6BB5
#include "u8g2_esp32_hal.h"
#define BIG_FONT_X_OFFSET 4
#define BIG_FONT_Y_OFFSET 6
#define LITTLE_FONT_X_OFFSET 3
#define LITTLE_FONT_Y_OFFSET 5

#include "esp_log.h"
#include "menu.h"
#include "defines.h"

enum edissonStatus { OK_EDISSON, ERROR_EDISSON };

void _printStatusString(u8g2_t* s, uint8_t xB, uint8_t yB, bool cursor,
                        uint8_t edissonNumber,
                        uint8_t status) {  // левый верхний угол
  uint8_t xSelector = xB;
  uint8_t ySelector = yB + BIG_FONT_Y_OFFSET;
  
  u8g2_SetFont(s, u8g2_font_4x6_mr);
  u8g2_DrawStr(s, xSelector, ySelector, (cursor) ? ">" : " ");
  char c[11] = {"Edison:"};
  sprintf(&c[7], "%d", edissonNumber);
  u8g2_SetFont(s, u8g2_font_4x6_tr);
  u8g2_DrawStr(s, xSelector + 2 + BIG_FONT_X_OFFSET - 1,
               ySelector, c);
  u8g2_SetFont(s, u8g2_font_3x5im_tr);
  u8g2_DrawStr(s, xSelector + 14, ySelector + 6,
               (status == OK_EDISSON) ? "Ok" : "ERRR");
}

void _printMenuEntry(u8g2_t* s, uint8_t xB, uint8_t yB, bool cursor,
                     char* title) {
  uint8_t xSelector = xB;
  uint8_t ySelector = yB + LITTLE_FONT_Y_OFFSET;
  u8g2_SetFont(s, u8g2_font_trixel_square_tr);
  u8g2_DrawStr(s, xSelector, ySelector, (cursor) ? ">" : " ");
  xSelector += LITTLE_FONT_X_OFFSET;
  u8g2_DrawStr(s, xSelector, ySelector, title);
}

void _constructBuffer(u8g2_t* s, void* args) {
  // x y левый верхний угол
  uint8_t statusPositions[] = {4,  2, 4,  18, 4,  34, 4,  50,
                               60, 2, 60, 18, 60, 34, 60, 50};
  uint8_t menuTitlesPositions[] = {
      108, 4, 108, 14, 108, 24, 108, 34, 108, 44, 108, 54,
  };
  char menuTitles[6][5] = {{"Tit1"}, {"Tit2"}, {"Tit3"},
                           {"Tit4"}, {"Tit5"}, {"Tit6"}};
  bool* checkedFieldsFlags_Edisons = (bool*)args;
  for (uint8_t i = 0, j=0; i < 16; i += 2, j++) {
    _printStatusString(s, statusPositions[i], statusPositions[i + 1],checkedFieldsFlags_Edisons[j], j,
                       ERROR_EDISSON);
  }
  u8g2_DrawFrame(s, 106, 2, 20, 60);
  for (uint8_t i = 0; i < 12; i += 2) {
    _printMenuEntry(s, menuTitlesPositions[i], menuTitlesPositions[i + 1], 1, menuTitles[i/2]);
  }
}

menu_screen* constructMain_menu_Struct(u8g2_t* s) {
  menu_screen* ptr = (menu_screen*) malloc(sizeof(menu_screen));
  ptr->screen = s;
  ptr->constructBuffer = &_constructBuffer;
  ptr->menu_type = MAIN_MENU;
  // ptr->printStatusString = &_printStatusString;
  // ptr->printMenuEntry = &_printMenuEntry;
  return ptr;
}


#endif /* FC078A03_CFC5_4D0C_8D3A_3E62FD9E6BB5 */
