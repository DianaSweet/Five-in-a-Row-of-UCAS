/* function.h头文件包含五子棋程序中的所有函数，以及Size的宏定义 */

#define Size 15  //棋盘的大小

/* 第一部分：游戏基础设置，游戏运行及落子函数 */
int ChessBoardValue(int i, int j);
void DrawChessBoard();
void PrintChess(int a, int b);
void ChoiceStart();
void GamePlay();
void AIInput();
void PlayerInput();
/* ----- 添加悔棋功能----- */
void regret();

/* 第二部分：游戏中的一些基础判定函数 */
int Out(int i, int j);
int Occupied(int i, int j);
int ChessSame(int i, int j, int type);
int SameTypeNumber(int i, int j, int d);
int d_l_is_same(int i, int j, int d, int l);

/* 第三部分：五子棋的禁手判定 */
int FourLiveConnect(int i, int j);
int FourBlockConnect(int i, int j);
int ThreeLiveConnect(int i, int j);
int LongConnect(int i, int j);

/* 第四部分：棋局的结果与胜负判定 */
int Forbidden(int i, int j);
int WinLose(int i, int j);
void JudgeResult(int i, int j);
void PlayAgain();

/* 第五部分：AI运行打分程序与搜索程序 */
int JudgeValue(int i, int j);
void Search_FirstFloor();
int Search_SecondFloor();
int Search_ThirdFloor(int Value_2th);
int ChongFour(int i, int j);
int ConnectLiveThree(int i, int j);
int JumpLiveThree(int i, int j);
int ConnectLiveTwo(int i, int j);
int ConnectMianThree(int i, int j);
int MianTwoConnect(int i, int j);