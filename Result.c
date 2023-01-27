#include <stdio.h>
#include "function.h"

extern int Result;  //Result=1表示游戏结束

extern int color;  //棋子的颜色标志

/* --------------------Result.c包含最终胜负判定与结果处理的函数(关于最后一步的特殊标记，最后一步将会保留为三角形，与其他圆形区别开)-------------------------- */
int Forbidden(int i, int j){  //判断落子后是否为禁手
    if (ChessSame(i, j, 2))  //白方无禁手
        return 0;
    if (ThreeLiveConnect(i, j) > 1 || LongConnect(i, j) || FourBlockConnect(i, j) + FourLiveConnect(i, j) > 1)  //大于2即为禁手
        return 1;
    else
        return 0;
}

int WinLose(int i, int j){  //落子后判断胜负
    int d;

    if (Forbidden(i, j))  //先判断禁手
        return 2;
    for (d = 0; d < 4; d++){
        if (SameTypeNumber(i, j, d) + SameTypeNumber(i, j, d + 4) >= 4)  //判断成五
            return 1;
    }
    return 0;
}

void JudgeResult(int i, int j){  //棋局结果的判定
    int end = 0;  //end记录WinLose函数的结果，判断输赢

    end = WinLose(i, j);
    if (end == 1){
        if (color == 2)
            printf("黑方胜！\n");
        else
            printf("白方胜！\n");
        Result = 1;
    }
    else if (end == 2){
        printf("黑方出现禁手，白方胜！\n");
        Result = 1;
    }
    if (Result)
        PlayAgain();  //判断是否重新游戏
}

void PlayAgain(){  //游戏结束后由玩家选择是否重新开始
    char will;  //will表示玩家的意愿

    printf("Do you want to play again?\nInput 'Y' to play again, input 'N' to quit the game\n");
    will = getchar();
    if (will == 89){
        system("cls");   //清屏
        ChoiceStart();
        DrawChessBoard();
        fflush(stdin);  //清除缓存区，保证之后的scanf可以正常使用
        Result = 0;
        GamePlay();
    }
    else if (will == 78){
        printf("Thank you for playing!\n");
        Result = -1;
    }
    else{
        PlayAgain();
    }
}
