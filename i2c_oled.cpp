#include "i2c_oled.h"
#include "mbed.h"
#include <cstdint>

DigitalOut OLED_SCL(D9);
DigitalOut OLED_SDA(D10);

uint8_t buffer[144][8];

/**
 * @brief  OLED次方函数
 * @retval 返回值等于X的Y次方
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y) {
  uint32_t Result = 1;
  while (Y--) {
    Result *= X;
  }
  return Result;
}

void OLED_W_SCL(uint8_t x) { OLED_SCL = x; }

void OLED_W_SDA(uint8_t x) { OLED_SDA = x; }

void OLED_I2C_Init() {
  OLED_W_SCL(1);
  OLED_W_SDA(1);
}

void OLED_I2C_Start(void) {
  OLED_W_SDA(1);
  OLED_W_SCL(1);
  OLED_W_SDA(0);
  OLED_W_SCL(0);
}

/**
 * @brief  I2C停止
 * @param  无
 * @retval 无
 */
void OLED_I2C_Stop(void) {
  OLED_W_SDA(0);
  OLED_W_SCL(1);
  OLED_W_SDA(1);
}

/**
 * @brief  I2C发送一个字节
 * @param  Byte 要发送的一个字节
 * @retval 无
 */
void OLED_I2C_SendByte(uint8_t Byte) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    OLED_W_SDA(Byte & (0x80 >> i));
    OLED_W_SCL(1);
    OLED_W_SCL(0);
  }
  OLED_W_SCL(1); //额外的一个时钟，不处理应答信号
  OLED_W_SCL(0);
}

/**
 * @brief  OLED写命令
 * @param  Command 要写入的命令
 * @retval 无
 */
void OLED_WriteCommand(uint8_t Command) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78); //从机地址
  OLED_I2C_SendByte(0x00); //写命令
  OLED_I2C_SendByte(Command);
  OLED_I2C_Stop();
}

/**
 * @brief  OLED写数据
 * @param  Data 要写入的数据
 * @retval 无
 */
void OLED_WriteData(uint8_t Data) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78); //从机地址
  OLED_I2C_SendByte(0x40); //写数据
  OLED_I2C_SendByte(Data);
  OLED_I2C_Stop();
}


//清除显存
void OLED_ClearBuffer(void) {
  uint8_t i, n;
  for (i = 0; i < 8; i++) {
    for (n = 0; n < 128; n++) {
      buffer[n][i] = 0; //清除所有数据
    }
  }
}

//更新显存到OLED
void OLED_SendBuffer(void) {
  uint8_t i, n;
  for (i = 0; i < 8; i++) {
    OLED_WriteCommand(0xb0 + i); //设置行起始地址
    OLED_WriteCommand(0x00);     //设置低列起始地址
    OLED_WriteCommand(0x10);     //设置高列起始地址
    for (n = 0; n < 128; n++)
      OLED_WriteData(buffer[n][i]);
  }
}

//画点
// x:0~127
// y:0~63
// t:1 填充 0,清空
void OLED_DrawPointBuffered(uint8_t x, uint8_t y, uint8_t t) {
  uint8_t i, m, n;
  i = y / 8;
  m = y % 8;
  n = 1 << m;
  if (t) {
    buffer[x][i] |= n;
  } else {
    buffer[x][i] = ~buffer[x][i];
    buffer[x][i] |= n;
    buffer[x][i] = ~buffer[x][i];
  }
}


//画线
// x1,y1:起点坐标
// x2,y2:结束坐标
void OLED_DrawLineBuffered(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                           uint8_t mode) {
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1; //计算坐标增量
  delta_y = y2 - y1;
  uRow = x1; //画线起点坐标
  uCol = y1;
  if (delta_x > 0)
    incx = 1; //设置单步方向
  else if (delta_x == 0)
    incx = 0; //垂直线
  else {
    incx = -1;
    delta_x = -delta_x;
  }
  if (delta_y > 0)
    incy = 1;
  else if (delta_y == 0)
    incy = 0; //水平线
  else {
    incy = -1;
    delta_y = -delta_x;
  }
  if (delta_x > delta_y)
    distance = delta_x; //选取基本增量坐标轴
  else
    distance = delta_y;
  for (t = 0; t < distance + 1; t++) {
    OLED_DrawPointBuffered(uRow, uCol, mode); //画点
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      uCol += incy;
    }
  }
}

/**
 * @brief  OLED初始化
 * @param  无
 * @retval 无
 */
void OLED_Init(void) {
  uint32_t i, j;

  for (i = 0; i < 1000; i++) //上电延时
  {
    for (j = 0; j < 1000; j++)
      ;
  }

  OLED_I2C_Init(); //端口初始化

  OLED_WriteCommand(0xAE); //关闭显示

  OLED_WriteCommand(0xD5); //设置显示时钟分频比/振荡器频率
  OLED_WriteCommand(0x80);

  OLED_WriteCommand(0xA8); //设置多路复用率
  OLED_WriteCommand(0x3F);

  OLED_WriteCommand(0xD3); //设置显示偏移
  OLED_WriteCommand(0x00);

  OLED_WriteCommand(0x40); //设置显示开始行

  OLED_WriteCommand(0xA1); //设置左右方向，0xA1正常 0xA0左右反置

  OLED_WriteCommand(0xC8); //设置上下方向，0xC8正常 0xC0上下反置

  OLED_WriteCommand(0xDA); //设置COM引脚硬件配置
  OLED_WriteCommand(0x12);

  OLED_WriteCommand(0x81); //设置对比度控制
  OLED_WriteCommand(0xCF);

  OLED_WriteCommand(0xD9); //设置预充电周期
  OLED_WriteCommand(0xF1);

  OLED_WriteCommand(0xDB); //设置VCOMH取消选择级别
  OLED_WriteCommand(0x30);

  OLED_WriteCommand(0xA4); //设置整个显示打开/关闭

  OLED_WriteCommand(0xA6); //设置正常/倒转显示

  OLED_WriteCommand(0x8D); //设置充电泵
  OLED_WriteCommand(0x14);

  OLED_WriteCommand(0xAF); //开启显示

  OLED_ClearBuffer(); // 清除显存
  OLED_SendBuffer();
}
