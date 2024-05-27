#include <cstdint>
#include "mbed.h"

void OLED_Init(void);
void OLED_ClearBuffer(void);
void OLED_SendBuffer(void);
void OLED_DrawPointBuffered(uint8_t x,uint8_t y,uint8_t t);
void OLED_DrawLineBuffered(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
