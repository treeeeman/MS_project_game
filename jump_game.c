#include<stdio.h>
#define MapHeight 10
#define PrintWidth 20
#define MapWidth PrintWidth+5
#define GroundHeight 3

void FramePrint(int map[MapHeight][MapWidth],int man[MapHeight][MapWidth])
{
    for(int i=MapHeight-1;i>0;i--)
    {
        for(int j=0;j<PrintWidth;j++)
        {
            if(map[i][j]||man[i][j])
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
    }

}

void MapMove(int new[MapHeight][MapWidth],int old[MapHeight][MapWidth],int speed)//copy old to new, and move speed 
{
    for(int i=0;i<MapHeight;i++)
        for(int j=0;j<MapWidth;j++)
            if(j+speed<MapWidth)
                new[i][j]=old[i][j+speed];
    
}

void Tree1(int map[MapHeight][MapWidth],int x)
{
    for(int i=GroundHeight+1;i<GroundHeight+5;i++)
        map[i][x]=1;
}


void ThingGenerate(int seed[MapWidth])
{
    static int speed=2;
    for(int i=0;i<MapWidth-speed;i++)
        seed[i]=seed[i+speed];
    for(int i=MapWidth-speed;i<MapWidth;i++)
        seed[i]=0;

    seed[MapWidth-1]=1;

}


void ThingDraw(int map[MapHeight][MapWidth],int seed[MapWidth])
{
    for(int i=0;i<PrintWidth;i++)
        if(seed[i]==1)
            Tree1(map,i);

    for(int i=0;i<MapWidth;i++)
        map[GroundHeight][i]=1;

}



void Man1()
{}

void ManDraw(int man[MapHeight][MapWidth])
{
    man[GroundHeight+1][3]=1;
}

int FailTest(int map[MapHeight][MapWidth],int man[MapHeight][MapWidth])
{
    for(int i=0;i<MapHeight;i++)
        for(int j=0;j<MapWidth;j++)
            if(man[i][j]&&map[i][j])
                return 1;
    return 0;
}

int main()
{
    int map[MapHeight][MapWidth]={};
    int man[MapHeight][MapWidth]={};
    int seed[MapWidth]={};

    for(int i=0;i<6;i++)
    {
        ThingGenerate(seed);
        ThingDraw(map,seed);
        ManDraw(man);
        FramePrint(map,man);
    }
    
    /*while(1)
    {

        if(FailTest(map,man))
            break;
    }*/

    return 0;
}