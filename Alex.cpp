/*
程序特点：在读入棋盘状态时，进行映射，使得在程序视角我方始终进攻左右边
*/
#include<iostream>
#include<ctime>
using namespace std;
int board[11][11];  // 棋盘 (1 -> 己方) (-1 -> 对方)
bool beganing = 0;  // 0表示
int op_x, op_y;  // 程序视角的落子位置

void outputBoard() {  // 在程序视角输出棋盘状态
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            printf("%2d ", board[i][j]);
        }
        cout << endl;
    }
}

int inputBoard() {  // 读入当前棋盘信息
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            board[i][j] = 0;
        }
    }
    int n, x, y;
    cin >> n;
    cin >> x >> y;
    if (n == 1 && x == -1) {  // 开局第一手
        cout << "1 2" << endl;
        return 1;
    }
    if (x == -1) {  // 我方先手
        beganing = 1;
    } else {
        board[x][y] = -1;
    }
    for (int i = 1; i < n; i++) {
        cin >> x >> y;
        if (beganing) {  // 顺时针旋转棋盘，使得我方永远进攻左右边
            int temp = x;
            x = y;
            y = 10 - temp;
        }
        board[x][y] = 1;
        cin >> x >> y;
        if (beganing) {
            int temp = x;
            x = y;
            y = 10 - temp;
        }
        board[x][y] = -1;
    }
    return 0;
}

void randomStep() {  // 在剩余空位中随机选择落子位置
    int empty = 0;
    int empty_queue[200][2];
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            if (board[i][j] == 0) {
                empty_queue[empty][0] = i;
                empty_queue[empty][1] = j;
                empty++;
            }
        }
    }
    srand((unsigned int)time(NULL));
    int r = rand() % empty;
    op_x = empty_queue[r][0];
    op_y = empty_queue[r][1];
}

void end() {  // 对最终落位置进行映射输出
    if (beganing) {
        int temp = op_y;
        op_y = op_x;
        op_x = 10 - temp;
    }
    cout << op_x << " " << op_y << endl;
}

int main() {
    if (inputBoard()) return 0;
    randomStep();
    end();
    outputBoard();
}
