#include <stdio.h>
#include "function.h"

extern int mode;  //mode选择模式（人人对战/人机对战）
extern int choice;  //choice选择先手
extern int choice_regret;

extern int AI_color;  //这一变量表示AI持子颜色

extern int Result;  //Result=1表示游戏结束

extern int ChessBoard[Size+1][Size+1];  //加1可以让棋盘有意义的元素序号从1开始，更形象地理解棋子的位置

extern int color;  //棋子的颜色标志

extern int row[8];
extern int col[8];

/* --------------------base.c包含游戏基础设置，游戏运行及落子函数-------------------------- */
int ChessBoardValue(int i, int j){  //为棋盘元素赋值
    if (ChessBoard[i][j] == 1)
        return printf("●");
    if (ChessBoard[i][j] == 2)
        return printf("○");
    if (ChessBoard[i][j] == -1)
        return printf("▲");
    if (ChessBoard[i][j] == -2)
        return printf("△");

    if (i == Size){
        if (j == 1)
            return printf("┏ ");
        if (j == Size)
            return printf("┓ ");
        return printf("┯ ");
    }
    if (i == 1){
        if (j == 1)
            return printf("┗ ");
        if (j == Size)
            return printf("┛ ");
        return printf("┷ ");
    }
    if (j == 1)
        return printf("┠ ");
    if (j == Size)
        return printf("┨ ");
    return printf("┼ ");
}

void DrawChessBoard(){  //打印棋盘
    int row, col;
    int A = 'A';

    system("cls");   //清屏
	system("color f0");   //白底黑字显示

    printf("  ==Welcome to FiveInRow Game==\n\n\n");

    /* 打印棋盘 */
    for (row = Size; row >= 1; row--){
        printf("\n %2d", row);
        for (col = 1; col <= Size; col++)
            ChessBoardValue(row, col);
    }
    printf("\n    ");
    for (col = 1; col <= Size; col++)
        printf("%c ", A++);
    
    printf("\n\n");
    /*输入规则提示*/
    printf("---About the input rules:---\n");
    printf("Please input one large letter and one number to describe the chess's location. Such as 'H8'\n");
    printf("Please Input Here:");
}

void PrintChess(int a, int b){  //将棋子打印在棋盘上
    int i, j;
    for (i = 0; i < Size + 1; i++){  //将三角形符号变为圆形符号
        for (j =0; j < Size + 1; j++){
            if (ChessBoard[i][j] == -1){
                ChessBoard[i][j] = 1;
            }
            else if (ChessBoard[i][j] == -2){
                ChessBoard[i][j] = 2;
            }
        }
    }
    if (color == 1){  //根据color选择棋子颜色
        ChessBoard[b][a] = -1;
        color = 2;
    }
    else if(color == 2){
        ChessBoard[b][a] = -2;
        color = 1;
    }
    DrawChessBoard();
    JudgeResult(a, b);
}

void ChoiceStart(){  //开局模式的选择
    int i, j;
    

    printf("  ==Welcome to FiveInRow Game==\n\n\n");
    printf("  Please Choose the mode:\n\n");
    printf("  Player to AI: input 1\n");
    printf("  Player to Player: input 2\n\n  Input Here: ");
    
    scanf_s("%d", &mode);  //输入mode，确定下棋的模式（人人/人机）

    if (mode != 1 && mode != 2)
        ChoiceStart();
    if (mode == 1){  //人机模式下确定先手
        printf("\n\n  Please Choose one side to start(use black):\n\n");
        printf("  AI: input 1\n");
        printf("  Player: input 2\n\n    Input Here: ");

        scanf_s("%d", &choice);

        if (choice != 1 && choice != 2)
            ChoiceStart();
    }
    
    /* 选择是否允许悔棋 */
    printf("\n\n  Please Choose Whether to Open Regret_Mode:\n\n");
    printf("  Open Regret_Mode: input 1\n");
    printf("  Not Open Regret_Mode: input 2\n\n    Input Here: ");

    scanf_s("%d", &choice_regret);

    if (choice_regret != 1 && choice_regret != 2)
        ChoiceStart();

    AI_color = choice;  //根据选择确定AI持子的颜色

    for (i = 1; i <= Size; i++){  //置为空棋盘
        for (j =1; j <= Size; j++){
            ChessBoard[i][j] = 0;
        }
    }
    color = 1;  //置为黑先手
}

void GamePlay(){  //执行游戏的主循环
    if (mode == 1){
        while (Result != -1){
            if (choice == 1){
                if (choice_regret == 1)
                    regret();
                AIInput();
            }
            else
                PlayerInput();
            choice = 3 - choice;
        }
    }

    else if (mode == 2){
        while (Result != -1){
            if (choice_regret == 1)
                regret();
            PlayerInput();
        }
    }
}

void AIInput(){  //利用最大最小搜索，实现AI下棋
    if (ChessBoard[8][8] == 0)  //棋盘中间为空，则在此处落子
        return PrintChess(8, 8);

    Search_FirstFloor();  //开始第一层搜索
}

void PlayerInput(){  //玩家下棋  
    char a;
    int b;  //a, b代表玩家输入的值
    a = (char)0, b = 0;

    scanf_s("%c", &a, 1);
    scanf_s("%d", &b);
    a -= 'A' - 1;  //将a转化为可表示列序号的数
        
    if (Out(a, b) || Occupied(a, b)){  //下棋之前，判断输入坐标是否正确规范
        printf("This location is occupied or out of range, please input again!\n");
        return PlayerInput();
    }
    else  //下棋
        PrintChess(a, b);
}

/* ----- 添加悔棋功能----- */
void regret(){  //悔棋的原理在于将上一步的棋（此时为三角特殊标记）变为未下的状态。自然地，AI不能悔棋
    int newput = 0;  //是否有新棋子的标志，无新棋子不判断是否悔棋
    int i, j;
    int new_i, new_j, want_regret;

    for (i = 0; i < Size + 1; i++){  //寻找新棋子，记录位置
        for (j =0; j < Size + 1; j++){
            if (ChessBoard[i][j] == -1 || ChessBoard[i][j] == -2){
                newput++, new_i = i, new_j = j;
            }
        }
    }
    if (newput == 1){
        printf("\n  Do you want to regret?\n\n");
        printf("  Regret: input 1\n");
        printf("  Not Regret: input 2\n  Input Here: ");

        scanf_s("%d", &want_regret);
    }

    if (want_regret == 1){
        ChessBoard[new_i][new_j] = 0;
        color = 3 - color;
        DrawChessBoard();
        PlayerInput();
    }
    else
        DrawChessBoard();
}

/* --------------------base.c还包含一些基础判定函数-------------------------- */
int Out(int i, int j){  //判断此位置在棋盘外
    if (i < 1 || i > Size || j < 1 || j > Size)
        return 1;
    return 0;    
}

int Occupied(int i, int j){  //判断此位置已占用
    if (ChessBoard[j][i] != 0)
        return 1;
    return 0;
}

int ChessSame(int i, int j, int type){  //判断此位置的棋子与对比的棋子是否为同一类型
    if (Out(i, j))
        return 0;
    else{
        if(ChessBoard[j][i] - type == 0 || ChessBoard[j][i] + type == 0)
            return 1;
        else 
            return 0;
    }
}

int SameTypeNumber(int i, int j, int d){  //判断在d代表的方向上相同类型棋子的数量
    int x, y, sum, state;

    sum = 0; 
    x = j + row[d], y = i + col[d];  //棋子沿着d方向移动
    state = ChessBoard[j][i];  //表示此位置棋子的类型

    if (state == 0)
        return 0;
    while (ChessSame(y, x, state))
        sum++, x += row[d], y += col[d];
    return sum;
}

int d_l_is_same(int i, int j, int d, int l){  //判断d方向，距离l的棋子与此处棋子是否为同一类型（禁手中需要判断一定范围内是否有相同的棋子）
    int state = ChessBoard[j][i];
    return ChessSame(i + l * col[d], j + l * row[d], state);
}
