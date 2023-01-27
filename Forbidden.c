#include <stdio.h>
#include "function.h"

extern int row[8];
extern int col[8];

extern int ChessBoard[Size+1][Size+1];  //加1可以让棋盘有意义的元素序号从1开始，更形象地理解棋子的位置

/* --------------------Forbidden.c包含禁手判定的函数-------------------------- */
int FourLiveConnect(int i, int j){  //判定活四的数量
    int k, d, sum;
    int result = 0;  //result表示形成禁手的个数
    
    for (d = 0; d < 4; d++){
        sum = 1;
        for (k = 1; d_l_is_same(i, j, d, k); k++){  //从近向远递推，记录出现相同棋子的次数
            sum++;
        }
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //这里需要保证两边都有空格位置
            continue;
        for (k = -1; d_l_is_same(i, j, d, k); k--){  //同上，这次向反方向搜索
            sum++;
        }
        if (Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            continue;
        if (sum == 4)  //在此方向是否有活四
            result++;
    }
    return result;
}

/* 求非连活四的过程利用了排列组合的原理，当棋型满足XabcdeY时，其中a~e有四个黑子，一个空位。
   根据排列组合，则连活四的数量*2加非连活四的数量等于所求的棋型数量，由此做减法可求出非连活四的数量 */
int FourBlockConnect(int i, int j){  //此处判断可能构成冲四的五个的位置来计算冲四的个数
    int k, d, sum, state;
    int result = 0;  //result表示形成禁手的个数
    
    for (d = 0; d < 8; d++){  //考虑8个方向（两侧并不是对称的）
        sum = 0;
        state = 1;  //跳过第一个非同类棋子，判断下一个的情况
        
        for (k = 1; d_l_is_same(i, j, d, k) || state; k++){
            if (!d_l_is_same(i, j, d, k)){  //找到该方向的第一个非同类棋子
                if (Occupied(i + k * col[d], j + k * row[d])){  //该方向的第一个不同类棋子为对方棋子,则不满足冲四要求
                    sum = 5;  //使sum为5，可以保证后面不被判为冲四
                }
                state = 0;
            }
            sum++;
        }
        k--;  //返回上一个非同类棋子判定的位置
        if (Out(i + k * col[d], j + k * row[d]))  //该方向的第一个不同类棋子是超出边界,没有冲四
            continue;
        for (k = -1; d_l_is_same(i, j, d, k); k--)
            sum++;
        if (sum == 4)
            result++;
    }
    return result - FourLiveConnect(i, j) * 2;  //五点与冲四之间的数量关系
}

int ThreeLiveConnect(int i, int j){  //活三的求法与上述禁手类似，因此相似的步骤不再给出注释
    int k, d, state, result, sum;
    result = 0;
    
    for (d = 0; d < 4; d++){  //连活三
        sum = 0;
        state = 2;  //为保证活三至少在一边有两个及以上空位，故state初值为2.每换一次方向，state应重置为2

        for (k = 1; d_l_is_same(i, j, d, k); k++)
            sum++;
        if(Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //第一个非同类不为空或超出范围，则不能构成活三
            continue;
        k++;
        if(Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            state--;
        for (k = -1; d_l_is_same(i, j, d, k); k--)
            sum++;
        if(Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))  //第一个非同类不为空或超出范围，则不能构成活三
            continue;
        k--;
        if(Out(i + k * col[d], j + k * row[d]) || Occupied(i + k * col[d], j + k * row[d]))
            state--;
        if (sum == 2 && state > 0)  //读到两个相同的棋子，且至少一侧有两个以上的空位
            result++;
    }
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

int LongConnect(int i, int j){  //长连的判断
    int d = 0;
    for (d = 0; d < 4; d++){
        if (SameTypeNumber(i, j, d) + SameTypeNumber(i, j, d + 4) > 4)
            return 1;
    }
    return 0;
}