#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

#define BRICKSIZE 2
#define BLACKCOLOR 40
#define PLAYSOUND(args) if (!isMute) mciSendString(TEXT(args), NULL, 0, NULL);

//#define DEBUG

typedef struct{
    int brick[4][4][4];
    int posX;
    int posY;
    int state;
    int color;
}BRICK;

enum key {
    W_UP    = 119,
    A_LEFT  = 97,
    S_DOWN  = 115,
    D_RIGHT = 100,
    Q_QUIT  = 81,
    M_Mute  = 77,
    P_STOP  = 80,
    SPACE_DROP = 32,
};

int windowsX=21,windowsY=20;
int speed = 100,isStop = 0,isMute = 0;
int score = 0,countClearRows = 0,level = 1;
double timePerFrame = 1000.0/60;
BRICK nextBrick;

BRICK srcBricks[7] = {
    //I
    {{{{ 0, 0, 0, 0},
       {44,44,44,44},
       { 0, 0, 0, 0},
       { 0, 0, 0, 0},},
      {{ 0,44, 0, 0},
       { 0,44, 0, 0},
       { 0,44, 0, 0},
       { 0,44, 0, 0},},
      {{ 0, 0, 0, 0},
       {44,44,44,44},
       { 0, 0, 0, 0},
       { 0, 0, 0, 0},},
      {{ 0,44, 0, 0},
       { 0,44, 0, 0},
       { 0,44, 0, 0},
       { 0,44, 0, 0},},},10,0,0,44},
    //L
    {{{{ 0,45, 0, 0},
       { 0,45,45,45},
       { 0, 0, 0, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0, 0,45, 0},
       { 0, 0,45, 0},
       { 0,45,45, 0},},
      {{ 0,45,45,45},
       { 0, 0, 0,45},
       { 0, 0, 0, 0},
       { 0, 0, 0, 0},},
      {{ 0,45,45, 0},
       { 0,45, 0, 0},
       { 0,45, 0, 0},
       { 0, 0, 0, 0},},},10,0,0,45},
    //J
    {{{{ 0, 0,46, 0},
       {46,46,46, 0},
       { 0, 0, 0, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       {46,46, 0, 0},
       { 0,46, 0, 0},
       { 0,46, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0, 0, 0, 0},
       { 0,46,46,46},
       { 0,46, 0, 0},},
      {{ 0, 0,46, 0},
       { 0, 0,46, 0},
       { 0, 0,46,46},
       { 0, 0, 0, 0},},},10,0,0,46},
    //O
    {{{{ 0, 0, 0, 0},
       { 0,47,47, 0},
       { 0,47,47, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0,47,47, 0},
       { 0,47,47, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0,47,47, 0},
       { 0,47,47, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0,47,47, 0},
       { 0,47,47, 0},
       { 0, 0, 0, 0},},},10,0,0,47},
    //S
    {{{{ 0, 0, 0, 0},
       { 0, 0,43,43},
       { 0,43,43, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0,43, 0, 0},
       { 0,43,43, 0},
       { 0, 0,43, 0},},
      {{ 0, 0, 0, 0},
       { 0, 0,43,43},
       { 0,43,43, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0,43, 0, 0},
       { 0,43,43, 0},
       { 0, 0,43, 0},},},10,0,0,43},
    //T
    {{{{ 0, 0, 0, 0},
       { 0, 0,42, 0},
       { 0,42,42,42},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0, 0,42, 0},
       { 0, 0,42,42},
       { 0, 0,42, 0},},
      {{ 0, 0, 0, 0},
       { 0, 0,42, 0},
       { 0,42,42, 0},
       { 0, 0,42, 0},},
      {{ 0, 0, 0, 0},
       { 0, 0, 0, 0},
       { 0,42,42,42},
       { 0, 0,42, 0},},},10,0,0,42},
    //Z
    {{{{41,41, 0, 0},
       { 0,41,41, 0},
       { 0, 0, 0, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0,41, 0, 0},
       {41,41, 0, 0},
       {41, 0, 0, 0},},
      {{41,41, 0, 0},
       { 0,41,41, 0},
       { 0, 0, 0, 0},
       { 0, 0, 0, 0},},
      {{ 0, 0, 0, 0},
       { 0,41, 0, 0},
       {41,41, 0, 0},
       {41, 0, 0, 0},},},10,0,0,41},
};

int GetRandNum(int range)
{
    return rand() % range;
}

char *FormatBrickContent(BRICK brick,int x,int y,char *brickContent)
{
    sprintf(brickContent,"\33[%d;%dH\33[%dm%*s\33[0m",brick.posY + y,brick.posX + (x * BRICKSIZE),brick.color,BRICKSIZE,"");
    return brickContent;
}

void DispBrick(BRICK brick)
{
    char brickContent[30];
    for (int i = 0;i < 4;i++)
    {
        for (int j = 0;j < 4;j++) {
           if (brick.brick[brick.state][i][j] != 0) printf("%s",FormatBrickContent(brick,j,i,brickContent));
        }
    }
}

char *FormatClearBrickContent(BRICK brick,int x,int y,char *clearBrickContent)
{    
    sprintf(clearBrickContent,"\33[%d;%dH\33[%dm%*s\33[0m",brick.posY + y,brick.posX + (x * BRICKSIZE),BLACKCOLOR,BRICKSIZE,"");
    return clearBrickContent;
}

void ClearBrick(BRICK brick)
{
    char clearBrickContent[30];
    for (int i = 0;i < 4;i++)
    {
        for (int j = 0;j < 4;j++) {
           if (brick.brick[brick.state][i][j] != 0) printf("%s",FormatClearBrickContent(brick,j,i,clearBrickContent));
        }
    }
}

int BrickCanRotate(BRICK *brick,int **map)
{
    for (int i = 0;i < 4;i++) 
    {
        for (int j = 0;j < 4;j++)
        {
            if (brick->brick[brick->state + 1][j][i]) 
            {
                /*Check windowsX*/
                if (brick->posX + (i * BRICKSIZE) <= 0 || brick->posX + (i * BRICKSIZE) >= windowsX) return 0;
                /*Check windowsY*/
                if (brick->posY + j > windowsY) return 0;
                /*Check cover other brick*/
                if (map[brick->posY + j][brick->posX / BRICKSIZE + i] != 0) return 0;
            }
        }
    }
    return 1;
}

void RotateBrick(BRICK *brick,int **map)
{
    if (BrickCanRotate(brick,map))
    {
        ClearBrick(*brick);
        brick->state = (brick->state + 1) % 4;
        DispBrick(*brick);
    }
}

void CopyBrickToMap(BRICK *brick,int **map)
{
    for (int i = 0;i < 4;i++) {
        for (int j = 0;j < 4;j++) {
            if (brick->brick[brick->state][i][j] != 0) {
                map[brick->posY + i][brick->posX / BRICKSIZE + j] = brick->brick[brick->state][i][j];
            }
        }
    }
}

int BrickCanMove(BRICK *brick,int x,int y,int **map)
{
    for (int i = 0;i < 4;i++)
    {
        for (int j = 0;j < 4;j++)
        {
            if (brick->brick[brick->state][j][i] != 0)
            {
                /*Check windowsX*/
                if (brick->posX + (i * BRICKSIZE) + (x * BRICKSIZE) <= 0 || brick->posX + (i * BRICKSIZE) + (x * BRICKSIZE) >= windowsX) return 0;
                /*Check windowsY*/
                if (brick->posY + j + y > windowsY && y == 1) return 0;
                /*Check top of brick*/
                if (map[brick->posY + j + 1][brick->posX / BRICKSIZE + i] != 0 && y == 1) return 0;
                if (map[brick->posY + j + y][brick->posX / BRICKSIZE + i + x] != 0) return 0;
            }
        }
    }
    return 1;
}

void InitNextBrick(BRICK *brick)
{
    ClearBrick(nextBrick);
    nextBrick.posY = 0;
    nextBrick.posX = 10;
    *brick = nextBrick;
    nextBrick = srcBricks[GetRandNum(7)];
}

void MoveBrick(BRICK *brick,int x,int y,int **map)
{
    if (BrickCanMove(brick,x,y,map))
    {
        ClearBrick(*brick);
        brick->posX += x * BRICKSIZE;
        brick->posY += y;
        DispBrick(*brick);
    }else if (y == 1){
        CopyBrickToMap(brick,map);
        InitNextBrick(brick);
    }
}

void ControlBrick(BRICK *brick,int **map)
{
    switch (_getch()) 
    {
        case A_LEFT:
            MoveBrick(brick,-1,0,map);
            break;
        case D_RIGHT:
            MoveBrick(brick,1,0,map);
            break;
        case S_DOWN:
            MoveBrick(brick,0,1,map);
            break;
        case W_UP:
            RotateBrick(brick,map);
            break;
        case M_Mute:
            isMute = (isMute == 0);
            break;
        case SPACE_DROP:
            PLAYSOUND("play  src/drop.wav");
            while (BrickCanMove(brick,0,1,map)) MoveBrick(brick,0,1,map);
            break;
        case P_STOP:
            isStop = (isStop == 0);
            break;
        case Q_QUIT:
            printf("\033[?25h");
            system("cls");
            exit(0);
            break;
    }
    fflush(stdin);
}

void DrawMap(int **map)
{
    for (int i = 1;i <= 20;i++) 
    {
        for (int j = 1;j <= 10;j++)
        {
            printf("\33[%d;%dH\33[%dm%*s\33[0m",i,j * BRICKSIZE,(map[i][j] != 0) ? map[i][j] : 40,BRICKSIZE,"");
        }
    }
}

void ChangeLevel()
{
    if (score >= 0 && score < 1000) {
        level = 1;
    }else if (score >= 1000 && score < 2000) {
        level = 2;
    }else if (score >= 2000 && score < 3000) {
        level = 3;
    }else if (score >= 3000 && score < 4000) {
        level = 4;
    }else if (score >= 4000 && score < 5000) {
        level = 5;
    }else if (score >= 5000 && score < 6000) {
        level = 6;
    }else if (score >= 6000 && score < 7000) {
        level = 7;
    }else if (score >= 7000) {
        level = 8;
    }
}

void InitMapRow(int *row)
{
    for (int i = 1; i <= 10; i++)
    {
        row[i] = 0;
    }
}

void ClearRowAnimation(int clearRowNumberList[20],int clearRowNumberlength)
{
    for (int j = 1; j <= 10; j++)
    {
        for (int i = 0; i < clearRowNumberlength; i++)
        {
            printf("\33[%d;%dH\33[47m%*s\33[0m",clearRowNumberList[i],(j * BRICKSIZE),BRICKSIZE,"");
        }
        Sleep(1);
    }
    for (int j = 10; j > 0; j--)
    {
        for (int i = 0; i < clearRowNumberlength; i++)
        {
            printf("\33[%d;%dH\33[40m%*s\33[0m",clearRowNumberList[i],(j * BRICKSIZE),BRICKSIZE,"");
        }
        Sleep(1);
    }
}

void ClearRow(int **map)
{
    int clearRowNumber[20],clearRowNumberLength = 0;
    int *tmpMapList;
    for (int i = 1;i <= 20;i++) {
        for (int j = 1;j <= 10;j++) {
            if (map[i][j] == 0) break;
            if (j == 10) clearRowNumber[clearRowNumberLength++] = i;
        }
    }
    for (int i = 0;i < clearRowNumberLength;i++)
    {
        InitMapRow(map[clearRowNumber[i]]);
        for (int j = clearRowNumber[i];j > 0;j--)
        {
            tmpMapList = map[j];map[j] = map[j - 1];map[j - 1] = tmpMapList;
        }
    }
    if (clearRowNumberLength != 0)
    {
        PLAYSOUND("play src/clear.wav");
        ClearRowAnimation(clearRowNumber,clearRowNumberLength);
        DrawMap(map);
        countClearRows += clearRowNumberLength;
        score += clearRowNumberLength * (50 + 5 * (clearRowNumberLength - 1));
        ChangeLevel();
    }
}

void GameOverAnimation()
{
    for (int i = 20; i > 0; i--)
    {
        printf("\33[%d;2H\33[47m%*s\33[0m",i,10 * BRICKSIZE,"");
        Sleep(2);
    }
}

void InitWindow()
{
    system("cls");
    for (int i = 0; i <= windowsY; i++)
    {
         for (int j = 0; j <= windowsX; j++)
         {
            if (i == windowsY) printf("%s",(j == 0) ? "+" : (j == windowsX) ? "+" : "-");
            else printf("%s",(j == 0 || j == windowsX) ? "|" : " ");
         }
         if (i != windowsY) printf("\033[?25l\n");
    }
}

void InitMap(int **map)
{
    for (int i = 0;i < 22;i++) {
        InitMapRow(map[i]);
    }
}

void FreeMap(int **map)
{
    for (int i = 0; i < 22; i++)
    {
        free(map[i]);
    }
    free(map);
}

void ClearScreenAnimation()
{
    for (int i = 1; i <= 20; i++)
    {
        printf("\33[%d;2H\33[40m%*s\33[0m",i,10 * BRICKSIZE,"");
        Sleep(2);
    }
}

void InitGameInfo(BRICK *brick,int **map,int *FPS)
{
    *brick     = srcBricks[GetRandNum(7)];
    *FPS       = 0;
    InitNextBrick(&nextBrick);
    InitMap(map);
    speed = 300;
    score = 0;
    level = 0;
    countClearRows = 0;
    ClearScreenAnimation();
}

int IsGameOver(int **map)
{
    for (int i = 1;i <= 10;i++)
    {
        if (map[1][i] != 0) return 1;
    }
    return 0;
}

int AskRestart()
{
    printf("\33[8;8H\33[47m\33[31mHit 'R'"
           "\33[9;8H\33[47m\33[31mto restart"
           "\33[10;8H\33[47m\33[31mHit 'Q'"
           "\33[11;8H\33[47m\33[31mto quit\33[0m");
    while (1)
    {
        switch (_getch())
        {
        case 'R':
            return 1;
        case 'Q':
            return 0;
        }
    }
}

void DispInfo(int **map,BRICK brick)
{
    for (int i = 1;i <= 21;i++)
    {
        for (int j = 1;j <= 10;j++)
        {
            printf("\33[%d;%dH%3d",i,26+(j * 3),map[i][j]);
        }
    }
    printf("\33[22;26HBrickX:%2d|BrickY:%2d",brick.posX,brick.posY);
}

void DispGameInfo()
{
    printf("\33[2;25HNext:");
    nextBrick.posY = 4;
    nextBrick.posX = 25;
    DispBrick(nextBrick);
    printf("\33[8;25HScore:%-4d",score);
    printf("\33[9;25HLevel:%-4d",level);
    printf("\33[10;25HClear Rows:%-4d",countClearRows);
    printf("\33[12;25HKeys:"
           "\33[13;25Ha     - left"
           "\33[14;25Hd     - right"
           "\33[15;25Hs     - down"
           "\33[16;25Hw     - rotate"
           "\33[17;25HM     - mute"
           "\33[18;25Hspace - stop"
           "\33[19;25HQ     - quit");
}

void CalculateDelTime(DWORD *lastTime,DWORD *deltaTime)
{
    DWORD nowTime = GetTickCount();
    *deltaTime = nowTime - *lastTime;
    *lastTime = nowTime;
}

int main(int argc,char *argv[])
{
    srand((unsigned)time(NULL));
    BRICK brick;
    int FPS;
    /*Give memory space to map*/
    int **map = (int **)malloc(22 * sizeof(int *));
    /*Get first run time*/
    DWORD lastTime = GetTickCount();
    DWORD deltaTime;
    for (int i = 0;i < 22;i++)
    {
        map[i] = (int *)malloc(11 * sizeof(int));
    }
    InitWindow();
    InitMap(map);
    InitGameInfo(&brick,map,&FPS);
    PLAYSOUND("play src/start.wav");
    while (1) {
        CalculateDelTime(&lastTime,&deltaTime);
        if (_kbhit())
        {
            fflush(stdin);
            PLAYSOUND("play src/hitkeyborad.wav");
            ControlBrick(&brick,map);
        }else if (!isStop){
            DispBrick(brick);
            ClearRow(map);
            if (IsGameOver(map))
            {
                PLAYSOUND("play src/gameover.wav");
                GameOverAnimation();
                if (AskRestart())
                {
                    InitGameInfo(&brick,map,&FPS);
                }else{
                    system("cls");
                    FreeMap(map);
                    break;
                }
            }
#ifdef DEBUG
            DispInfo(map,brick);
#else
            DispGameInfo();
#endif
        }
        FPS = (FPS + 1) % (10 - level);
        if (FPS == 0) MoveBrick(&brick,0,1,map);
        if (deltaTime <= timePerFrame) Sleep(timePerFrame - deltaTime);
    }
}
