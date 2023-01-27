#include <stdio.h>
#include "function.h"

/* main.c为五子棋程序的主程序，包含全局变量和main函数 */

int mode;  //mode选择模式（人人对战/人机对战）
int choice;  //choice选择先手
int choice_regret;  //选择是否开启悔棋选项，开启后允许悔棋

int AI_color;  //这一变量表示AI持子颜色

int Result;  //Result=1表示游戏结束

int color = 1;  //棋子的颜色标志

int ChessBoard[Size+1][Size+1];  //加1可以让棋盘有意义的元素序号从1开始，更形象地理解棋子的位置

/* row和col数组表示棋盘中的八个方向向量 */
int row[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int col[8] = {0, 1, 1, 1, 0, -1, -1, -1};

void main(){
    ChoiceStart();
    DrawChessBoard();
    GamePlay();
}