#include "mbed.h"
#include "i2c_oled.h"

int main() {
  DigitalOut VCC(D2);
  VCC=1; // Power the screen by setting the pin high. This is not recommended.

  //ThisThread::sleep_for(100ms);
  OLED_Init();
  OLED_Genshin();
}