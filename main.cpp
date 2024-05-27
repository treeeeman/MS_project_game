#include "mbed.h"
#include "i2c_oled.h"
#include <time.h>

#define MapHeight 64
#define PrintWidth 128
#define MapWidth PrintWidth+8
#define GroundHeight 4

DigitalIn button(D5,PullDown);
DigitalOut led(D4);

int random(int min,int max)//give a random integer in [min,max],time.h is needed
{
    srand(time(NULL)*125);
//    printf("random number ready:%d\n",n);
    //return rand()%(max+1-min)+min;
    //return 0;
    return (time(NULL)*rand()*rand()+rand())%(max+1-min)+min;
}

void MapFill(short map[MapWidth],short color)
{
    for(short i=0;i<MapWidth;i++)
        map[i]=color;
}


void Tree1(short map[MapWidth],short x)
{
    for(short i=x;i<x+1;i++)
        map[i]=GroundHeight+3;

    OLED_DrawLineBuffered(x, GroundHeight+1, x, map[x], 1);
}

void Tree2(short map[MapWidth],short x)
{
    for(short i=x;i<x+1;i++)
        map[i]=GroundHeight+9;

    OLED_DrawLineBuffered(x, GroundHeight+1, x, map[x], 1);
}

void ThingGenerate(short seed[MapWidth])
{
    static short speed=1;
    for(short i=0;i<MapWidth-speed;i++)
        seed[i]=seed[i+speed];
    for(short i=MapWidth-speed;i<MapWidth;i++)
        seed[i]=0;

    short k=0;
    for(short i=1;i<28+8;i++)
        if(seed[MapWidth-i]!=0)
            k=1;
    if(k==0&&random(0,1))
        seed[MapWidth-random(1,16+1)]=1;
        // ;

    k=0;
    for(short i=1;i<28+8;i++)
        if(seed[MapWidth-i]!=0)
            k=1;
    if(k==0)
        seed[MapWidth-random(1,24+1)]=2;
}


void ThingDraw(short map[MapWidth],short seed[MapWidth])
{
    MapFill(map,GroundHeight);
    OLED_DrawLineBuffered(0, GroundHeight, 127, GroundHeight, 1);
    OLED_DrawLineBuffered(0, GroundHeight-1, 127, GroundHeight-1, 1);

    for(short i=0;i<PrintWidth;i++)
    {
        if(seed[i]==1)
            Tree1(map,i);
        else if(seed[i]==2)
            Tree2(map,i);
    }
    
}



void Man1(short man[MapWidth],short x,short y)
{
    for(short i=x;i<=x+2;i++)
        man[i]=y;

    OLED_DrawLineBuffered(x, y, x+2, y, 1);
    OLED_DrawLineBuffered(x+2, y, x+2, y+4, 1);
    OLED_DrawLineBuffered(x, y, x, y+4, 1);
    OLED_DrawLineBuffered(x, y+4, x+2, y+4, 1);
}

short v=0,y=GroundHeight+1;
void ManDraw(short man[MapWidth])
{
    MapFill(man,MapHeight-1);
    
    if(y==GroundHeight+1 && button==1)
    {
        v=8;
        led=1;
    }
    else
        led=0;
    y+=v;
    v-=1;

    if(y<=GroundHeight+1)
    {
        y=GroundHeight+1;
        v=0;
    }

    
    Man1(man,4,y);
}

short FailTest(short map[MapWidth],short man[MapWidth])
{
    for(short i=0;i<MapWidth;i++)
        if(man[i]<=map[i])
        {
            led=1;
            return 1;
        }
    return 0;
}


int main()
{
/*
    OLED_Init();
    OLED_Genshin();
    ThisThread::sleep_for(1000ms);
    OLED_ClearBuffer();
    OLED_SendBuffer();
*/
    //OledWhite();*/

    short map[MapWidth]={};
    short man[MapWidth]={};
    short seed[MapWidth]={};

    /*MapFill(map,1);
    OledBufferClear();
    Map2Buffer_1(map,man);
    OledBufferPoshort(127,63,1);
    OledBufferPoshort(127,63,1);
    OledBufferSent();*/

    
    while(1)
    {
        while(1)
        {
            ThisThread::sleep_for(10ms);
            if(button==1)
                break;
        }
    
        while(1)
        {
            OLED_ClearBuffer();
            ThingGenerate(seed);
            ThingDraw(map,seed);
            ManDraw(man);
            OLED_SendBuffer();
            if(FailTest(map, man))
                break;
            ThisThread::sleep_for(6ms);
        }
        MapFill(seed,0);
    }
    
}
