#include <stdio.h>
#include "function.h"

#define Max_Min_Search 1000000  //最大最小搜索的极值
#define ConnectFive 200000  //成五的分值
#define LiveFour 10000  //活四的分值
#define BlockFour 1000  //冲四的分值
#define LiveThree 800  //连活三的分值
#define JumpThree 700  //跳活三的分值
#define MianThree 500  //眠三的分值
#define LiveTwo 50  //活二的分值
#define MianTwo 10  //眠二的分值

extern int AI_color;  //这一变量表示AI持子颜色

extern int row[8];
extern int col[8];

extern int ChessBoard[Size+1][Size+1];  //加1可以让棋盘有意义的元素序号从1开始，更形象地理解棋子的位置

/* --------------------AI_Judge.c包含固定棋型，AI运行打分程序与搜索程序-------------------------- */
int JudgeValue(int i, int j){  //判分函数
    int value, d, end;
    value = 0;
    end = WinLose(i,j);

    if (end == 1)
        return ConnectFive;
    else if (end == 2)
        return 0;
    
    //为减少搜索时间，只选取周围有棋子的位置进行下一步的搜索，周围不含棋子的记为0分，含棋子记为1分
    for (d = 0; d < 8; d++)
        if (ChessBoard[j + row[d]][i + col[d]] != 0 && value < 1)
            value++;
    value += FourLiveConnect(i, j) * LiveFour + ChongFour(i, j) * BlockFour + ConnectLiveThree(i, j) * LiveThree + JumpLiveThree(i, j) * JumpThree + ConnectLiveTwo(i, j) * LiveTwo + ConnectMianThree(i, j) * MianThree + MianTwoConnect(i, j) * MianTwo;  //根据棋型赋分
    return value;
}

void Search_FirstFloor(){
    int i, j;
    int FlagValue, TempValue, result_i, result_j;
    FlagValue = -Max_Min_Search;
    
    for (i = 1; i < Size+1; i++){
        for (j = 1; j < Size+1; j++){
            if (Out(i, j) || Occupied(i, j))  //判断是否在棋盘内或有棋子占用
                continue;

            ChessBoard[j][i] = AI_color;  //将此处假设为AI所持棋子
            TempValue = JudgeValue(i, j);  //进行评分
            if (TempValue == 0){    //若评分为0（即周围无棋子）
                ChessBoard[j][i] = 0;  //将假设的位置改为初始状态并结束此轮搜索
                continue;
            }
            if (TempValue == ConnectFive){  //若达成连五，则直接落子胜利
                ChessBoard[j][i] = 0;  //将假设的位置改为初始状态并结束此轮搜索
                return PrintChess(i, j);
            }
                
            TempValue = Search_SecondFloor();  //进行第二层搜索（min）
            ChessBoard[j][i] = 0;
            if (TempValue > FlagValue){  //第一层搜索极大值
                FlagValue = TempValue;
                result_i = i, result_j = j;  //将最优处设为落子处
            }
        }
    }
    return PrintChess(result_i, result_j);
}

int Search_SecondFloor(){
    int i, j;
    int FlagValue, TempValue;
    FlagValue = Max_Min_Search;

    for (i = 1; i < Size+1; i++){
        for (j = 1; j < Size+1; j++){
            if (Out(i, j) || Occupied(i, j))
                continue;

            ChessBoard[j][i] = 3 - AI_color;  //将此处假设为AI的对手所持棋子
            TempValue = JudgeValue(i, j);
            if (TempValue == 0){
                ChessBoard[j][i] = 0;
                continue;
            }
            if (TempValue == ConnectFive){  //极小搜索默认成五点不能成立
                    ChessBoard[j][i] = 0;
                    return -ConnectFive;
            }
                
            TempValue = Search_ThirdFloor(TempValue);  //进行第三层搜索（max）
            ChessBoard[j][i] = 0;
            if (TempValue < FlagValue)  //第二层搜索极小值
                FlagValue = TempValue;
        }
    }
    return FlagValue;
}

int Search_ThirdFloor(int Value_2th){
    int i, j;
    int FlagValue, TempValue;
    FlagValue = -Max_Min_Search;

    for (i = 1; i < Size+1; i++){
        for (j = 1; j < Size+1; j++){
            if (Out(i, j) || Occupied(i, j))
                continue;
            
            ChessBoard[j][i] = AI_color;
            TempValue = JudgeValue(i, j);
            if (TempValue == 0){
                ChessBoard[j][i] = 0;
                continue;
            }
            if (TempValue == ConnectFive){
                ChessBoard[j][i] = 0;
                return ConnectFive;
            }

            ChessBoard[j][i] = 0;
            if (TempValue - Value_2th * 2 > FlagValue)  //第三层搜索极大值
                FlagValue = TempValue - Value_2th * 2;
        }
    }
    return FlagValue;
}

/* --------------------下面是五子棋的相关棋型-------------------------- */
int ChongFour(int i, int j){  //冲四的棋型
    //先计算第一种冲四：011112
    int k, d, sum;
    int result = 0;

    for (d = 0; d < 8; d++){
        sum = 1;
        for (k = 1; d_l_is_same(i, j, d, k); k++){
            sum++;
        }
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //这里需要保证一边有空格位置
            continue;
        for (k = -1; d_l_is_same(i, j, d, k); k--){
            sum++;
        }
        if (!Out(i + k * col[d], j + k * row[d]) || !Occupied(i + k * col[d], j + k * row[d]))
            continue;
        if (sum == 4)
            result++;
    }
    return result + FourBlockConnect(i, j);  //与另一种冲四求和
}

int ConnectLiveThree(int i, int j){  //连活三的棋型
    int k, d, state, result, sum;
    result = 0;
    
    for (d = 0; d < 4; d++){
        sum = 1;
        state = 2;  //为保证活三至少在一边有两个及以上空位，故state初值为2.每换一次方向，state应重置为2

        for (k = 1; d_l_is_same(i, j, d, k); k++)
            sum++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //第一个非同类不为空或超出范围，则不能构成活三
            continue;
        k++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            state--;
        for (k = -1; d_l_is_same(i, j, d, k); k--)
            sum++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //第一个非同类不为空或超出范围，则不能构成活三
            continue;
        k--;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            state--;
        if (sum == 3 && state > 0)  //读到两个相同的棋子，且至少一侧有两个以上的空位
            result++;
    }
    return result;
}

int JumpLiveThree(int i, int j){  //跳活三的棋型
    int k, d, state, result, sum;
    result = 0;

    for (d = 0; d < 8; d++){  //每个方向最多一个跳活三
    
        sum = 0;
        state = 1;
        for (k = 1; d_l_is_same(i, j, d, k) || state; k++){  //在成活四点的方向上判断跳活三
            if (!d_l_is_same(i, j, d, k)){
                if (Occupied(i + k * col[d], j + k * row[d]))
                    sum = 4;  //使sum为4，可以保证后面不被判为跳活三
                state--;
            }
            sum++;
        }
        if(Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            continue;
        k--;  //此处退一格，为的是如果读到的第一个空格之后没有黑棋，且这一个位置是空位时，则最多构成连活三，与上面重复
        if (ChessBoard[j + row[d] * k][i + col[d] * k] == 0)
            continue;
        for (k = -1; d_l_is_same(i, j, d, k); k--)
            sum++;
        if(Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            continue;;
        if (sum == 3)
            result++;
    }
    return result;
}

int ConnectLiveTwo(int i, int j){  //活二的棋型
    int k, d, sum;
    int result = 0;

    for (d = 0; d < 4; d++){  //形如001100
        sum = 1;
        for (k = 1; d_l_is_same(i, j, d, k); k++){
            sum++;
        }
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //这里需要保证一边有空格位置
            continue;
        k++;  //需要判断两个空格
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //这里需要保证一边有空格位置
            continue;
        for (k = -1; d_l_is_same(i, j, d, k); k--){
            sum++;
        }
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            continue;
        k--;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            continue;
        if (sum == 2)
            result++;
    }
    for (d = 0; d < 8; d++){  //形如010100
        sum = 0;
        if (Out(i + 1 * col[d], j + 1 * row[d]) || Occupied(i + 1 * col[d], j + 1 * row[d]))
            continue;
        if (!d_l_is_same(i, j, d, 2))
            continue;
        if (Out(i + 3 * col[d], j + 3 * row[d]) || Occupied(i + 3 * col[d], j + 3 * row[d]))
            continue;
        for (k = -1; !Out(i + k * col[d], j + k * row[d]) && !Occupied(i + k * col[d], j + k * row[d]) && k >-3; k--)
            sum++;
        if (sum == 2)
            result++;
    }
    for (d = 0; d < 8; d++){  //形如001010
        sum = 0;
        if (Out(i - 1 * col[d], j - 1 * row[d]) || Occupied(i - 1 * col[d], j - 1 * row[d]))
            continue;
        if (Out(i + 1 * col[d], j + 1 * row[d]) || Occupied(i + 1 * col[d], j + 1 * row[d]))
            continue;
        if (!d_l_is_same(i, j, d, 2))
            continue;
        for (k = 3; !Out(i + k * col[d], j + k * row[d]) && !Occupied(i + k * col[d], j + k * row[d]) && k < 5; k++)
            sum++;
        if (sum == 2)
            result++;
    }
    return result;
}

int ConnectMianThree(int i, int j){  //眠三的棋型
    int k, d, sum, state, result, flag;
    result = 0;

    for (d = 0; d < 4; d++){  //形如001112
        sum = 1;
        state = 1;

        for (k = 1; d_l_is_same(i, j, d, k); k++)
            sum++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            state--;
        else{
            k++;
            if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
                continue;
        }
        for (k = -1; d_l_is_same(i, j, d, k); k--)
            sum++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d])){
            if (state == 0)
                continue;
            else
                state--;
        }
        else{
            if (state == 0){
                k--;
                if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
                    continue;
            }
            else
                continue;
        }
        if (sum == 3 && state == 0)
            result++;
    }
    for (d = 0; d < 8; d++){  //形如210110等跳眠三
        sum = 0;
        state = 1, flag = 1;

        for (k = 1; d_l_is_same(i, j, d, k) || state; k++){
            if (!d_l_is_same(i, j, d, k)){
                if (Occupied(i + k * col[d], j + k * row[d]))
                    sum = 4;
                state--;
            }
            sum++;
        }
        if(Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            flag--;
        k--;  //此处退一格，为的是如果读到的第一个空格之后没有黑棋，且这一个位置是空位时，则最多构成连眠三，与上面重复
        if (Out(i + k * col[d], j + k * row[d]) || ChessBoard[j + row[d] * k][i + col[d] * k] == 0)
            continue;
        for (k = -1; d_l_is_same(i, j, d, k); k--)
            sum++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d])){
            if (flag == 0)
                continue;
            else
                flag--;
        }
        else{
            if (flag != 0)
                continue;
        }
        if (sum == 3 && flag == 0)
            result++;
    }
    for (d = 0; d < 8; d++){  //形如10011等情况
        sum = 0, state = 1;
        if (d_l_is_same(i, j, d, 1))
            state--;
        if (state == 1){
            if (Out(i + 1 * col[d], j + 1 * row[d]) || Occupied(i + 1 * col[d], j + 1 * row[d]))
                continue;
            if (Out(i + 2 * col[d], j + 2 * row[d]) || Occupied(i + 2 * col[d], j + 2 * row[d]))
                continue;
            if (!d_l_is_same(i, j, d, 3))
                continue;
            if (d_l_is_same(i, j, d, 4))
                sum++;
            if (d_l_is_same(i, j, d, -1))
                sum++;
            if (sum > 0)
                result++;
        }
        else if (state == 0){
            if (!d_l_is_same(i, j, d, 1))
                continue;
            if (Out(i + 2 * col[d], j + 2 * row[d]) || Occupied(i + 2 * col[d], j + 2 * row[d]))
                continue;
            if (Out(i + 3 * col[d], j + 3 * row[d]) || Occupied(i + 3 * col[d], j + 3 * row[d]))
                continue;
            if (d_l_is_same(i, j, d, 4))
                result++;
        }
    }
    return result;
}

int MianTwoConnect(int i, int j){
    int k, d, sum, state, result;
    result = 0;

    for (d = 0; d < 4; d++){  //形如00112
        sum = 1;
        state = 1;

        for (k = 1; d_l_is_same(i, j, d, k); k++)
            sum++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            state--;
        else{
            k++;
            if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
                continue;
        }
        for (k = -1; d_l_is_same(i, j, d, k); k--)
            sum++;
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d])){
            if (state == 0)
                continue;
            else
                state--;
        }
        else{
            if (state == 0){
                k--;
                if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
                    continue;
            }
            else
                continue;
        }
        if (sum == 2 && state == 0)
            result++;
    }
    return result;
}
