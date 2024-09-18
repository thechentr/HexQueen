#pragma GCC optimize(2)
#include<bits/stdc++.h>
#include<ctime>
using namespace std;
bool WIN = 0;
int ARG = 10;
int FFF = 100;
int secondBest = 0;
bool positionLegal(int x, int y);  
// 判断位置在虚拟棋盘上是否合法
bool positionReal(int x, int y);  
// 判断位置在实际棋盘上是否合法
void initConnect(int board[13][13], int connect[11][11][11][11], int color);
// 用转换后的盘构造邻居映射图
void mapNeiber(const int board[11][11], int connect[11][11][11][11], int color, int dir);
// 构造邻居映射图，connect作为邻居映射返回,color：作为链接的棋子颜色，dir（color棋子的边，1上下0左右）
void edgeDis(const int board[11][11],const int neibers[11][11][11][11],const int color,const int edge, const int dir, int dis[11][11]);
// 根据棋子颜色 和边缘标号 求棋盘上的空位置到color色edge边的双距离
void findBestPosition(int board[11][11], int color, int &x, int &y ,int show);
// 寻找color颜色棋，在board局面下的最佳位置
bool inputBoard(int board[11][11], int &color);
// 读入当前棋盘信息

double value(const int board[11][11], int val[11][11], int color, bool out) {
    // 求board棋盘状态对color的价值，value越小越好
    int neibers[11][11][11][11];
    int dis[11][11];
    int doubleDis1[11][11], doubleDis_1[11][11];
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            doubleDis1[i][j] = doubleDis_1[i][j] = 0;
        }
    }
    // 分别求到四条边的双距离
    mapNeiber(board, neibers, 1, 1);  // 对整个棋盘进行红棋的邻接映射
    edgeDis(board, neibers, 1, 0, 1, dis);  // 求到上边（1方棋子的边）的双距离
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            doubleDis1[i][j] += dis[i][j];
        }
    }
    edgeDis(board, neibers, 1, 10, 1,dis);  // 求到下边（1方棋子的边）的双距离
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            doubleDis1[i][j] += dis[i][j];
        }
    }

    mapNeiber(board, neibers, -1, 0);    // 对整个棋盘进行蓝棋的邻接映射
    edgeDis(board, neibers, -1, 0, 0, dis);  // 求左边（-1方棋子的边）的双距离
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            doubleDis_1[i][j] += dis[i][j];
        }
    }
    edgeDis(board, neibers, -1, 10, 0,dis);  // 求到右边（-1方棋子的边）的双距离
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            doubleDis_1[i][j] += dis[i][j];
        }
    }
    if (WIN) {
        // cout << "WIN!!!";
        WIN = 0;
        return -999999;
    }    
    // 求局面的估值
    int min_reds = 0, min_blues = 0;  // 红蓝双距离最小的数量
    double blue_min[5] = {10000, 10000, 10000, 10000, 10000};
    double red_min[5] = {10000, 10000, 10000, 10000, 10000};
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            val[i][j] = doubleDis1[i][j] + doubleDis_1[i][j];  // 求红黑双距离之和作为返回
            // cout << val[i][j] << " ";
            // 求小的5个红距离
            double temp = doubleDis1[i][j], t;
            if (temp == red_min[0]) {
                min_reds++;
            } else {
                min_reds = 1;
            }
            for (int k = 0; k < 2; k++) {
                if (temp <= red_min[k]) {
                    t = red_min[k];
                    red_min[k] = temp;
                    temp = t;
                }
            }
            // 求小的5个蓝距离
            temp = doubleDis_1[i][j];
            if (temp == blue_min[0]) {
                min_blues++;
            } else {
                min_blues = 1;
            }
            for (int k = 0; k < 2; k++) {
                if (temp <= blue_min[k]) {
                    t = blue_min[0];
                    blue_min[k] = temp;
                    temp = t;
                }
            }
        }
    }
    // int min1 = (int)((red_min[0] + red_min[1] + red_min[2] + red_min[3] + red_min[4]) / 5);
    // int min_1 = (int)((blue_min[0] + blue_min[1] + blue_min[2] + blue_min[3] + blue_min[4]) / 5);
    int min1 = (int)(red_min[0] + red_min[1]) / 2;
    int min_1 = (int)(blue_min[0] + blue_min[1]) / 2;
    int num1 = min_reds;
    int num_1 = min_blues;
    if (out) {
        printf("R_dis : %d B_dis %d\n", min1, min_1);
        printf("R_num : %d B_num %d\n", num1, num_1);
    }
    if (out) {
        cout << endl;
        cout << "到红边的双距离 : " << endl;
        for (int i = 0; i < 11; i++) {
            for (int j = 0; j < 11; j++) {
                cout << doubleDis1[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        cout << "到蓝边的双距离 : " << endl;
        for (int i = 0; i < 11; i++) {
            for (int j = 0; j < 11; j++) {
                cout << doubleDis_1[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        printf("R_dis : %d B_dis %d\n", min1, min_1);
        printf("R_num : %d B_num %d\n", num1, num_1);
    }

    int move = num1 - num_1;
    // return color * ( min1 - min_1) - color * (num1 - num_1);  // 考虑进攻机动性
    // return (double)(color * (min1 - min_1));  // 只考虑双距离
    // return -min1; // 只考虑红棋
    // return (double)(color * (min1 - 0.5 * min_1));  // 侧重于考虑红棋
    // return (double)(color * (0.5 * min1 - min_1));  // 侧重于考虑蓝棋

    // double alg = 1;
    // if (min1 < min_1) min_1 *= alg;  // 当前局面对蓝方有利
    // else if (min_1 < min1) min1 *= alg;  // 当前局面对红方有利
    return (double)( ARG * (color * (min1 - min_1)) + color * move);  // 考虑当前局面形式
}

int main() {
    // while (1) {
        int board[11][11];
        int color = -1;
        if (inputBoard(board, color)) {
            cout << "1 2" << endl;
            return 0;
        }
        if (secondBest == 1) {
            cout << "7 3" << endl;
            return 0; 
        }       
        int x, y;
        int val[11][11];
        // board[2][9] = -1;
        // printf("%lf \n", value(board, val, -1, 1));
        // // return 0;
        // board[2][9] = 0;
        // board[0][9] = -1;
        // printf("%lf \n", value(board, val, -1, 1));
        // board[3][4] = 0;
        // return 0;
        findBestPosition(board, color, x, y, 1);
        cout << x << " " << y << endl;
        // cout << "run time : " << (double)clock()/CLOCKS_PER_SEC << endl;
        // cout << " ARG : " <<  ARG << endl;
    // }
}
 


bool positionLegal(int x, int y) {  // 位置在虚拟棋盘上是否合法
    if (0 <= x && x <= 12) {
        if (0 <= y && y <= 12) {
            return 1;
        }
    }
    return 0;
}

bool positionReal(int x, int y) {  // 位置在实际棋盘上是否合法
    if (0 <= x && x < 11) {
        if (0 <= y && y < 11) {
            return 1;
        }
    }
    return 0;
}

void initConnect(int board[13][13], int connect[11][11][11][11], int color) {  // 用转换后的盘构造邻居映射图
    int neiber[6][2] = {0, -1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1};  // 6个邻接点
    int stack[150][2], p_s = 0;  // 按真正位置储存
    int queue[150][2], head = 0, tail = 0; // 按虚拟位置储存

    // 遍历棋盘上所有位置，先对color方棋子造成的间接邻居进行处理
    for (int x = 0; x < 13; x++) {
        for (int y = 0; y < 13; y++) {
            if (board[x][y] == color) {
                queue[tail][0] = x;
                queue[tail++][1] = y;
                board[x][y] *= 2;  // 标记是处理过的color方棋子
                while (head != tail) {
                    int tmx = queue[head][0];
                    int tmy = queue[head++][1];  // 队首出队
                    // cout << tmx << " " << tmy << "出队" << endl;
                    for (int i = 0; i < 6; i++) {
                        int tmpx = tmx + neiber[i][0];
                        int tmpy = tmy + neiber[i][1];
                        if (positionLegal(tmpx, tmpy)){
                            // cout << tmpx << " " << tmpy << endl;
                            if (board[tmpx][tmpy] == color) {  // 仍是color方棋子则继续入队
                                queue[tail][0] = tmpx;
                                queue[tail++][1] = tmpy;
                                board[tmpx][tmpy] *= 2;  // 标记是处理过的color方棋子
                                // cout << tmpx << " " << tmpy << "是我方棋子" << endl;
                            }
                            if (board[tmpx][tmpy] == 0 && positionReal(tmpx-1, tmpy-1)) {  // 是空位置则入栈
                                stack[p_s][0] = tmpx-1;
                                stack[p_s++][1] = tmpy-1;
                                board[tmpx][tmpy] = 3;
                                // cout << tmpx-1 << " " << tmpy-1 << "是空位置" << endl;
                            }
                        }
                    }
                }
                // 对栈内结点进行邻居映射
                for (int i = 0; i < p_s; i++) {
                    for (int j = i + 1; j < p_s; j++) {
                        // cout << "栈内第" << i << " and " << j << " 结点 " << endl;
                        // cout << stack[i][0] << " " << stack[i][1] << " " << stack[j][0] << " " << stack[j][1] << endl;
                        connect[stack[i][0]][stack[i][1]][stack[j][0]][stack[j][1]] = 1;
                        connect[stack[j][0]][stack[j][1]][stack[i][0]][stack[i][1]] = 1;
                        // cout << stack[i][0] << " " << stack[i][1] << " 与 " << stack[j][0] << " " << stack[j][1] << "是邻居" << endl;
                    }
                }
                // 对队内元素和栈内元素进行映射
                for (int i = 0; i < tail; i++) {
                    for (int j = 0; j < p_s; j++) {
                        if (positionReal(queue[i][0]-1, queue[i][1]-1)){
                            connect[queue[i][0]-1][queue[i][1]-1][stack[j][0]][stack[j][1]] = 1;
                            connect[stack[j][0]][stack[j][1]][queue[i][0]-1][queue[i][1]-1] = 1;
                        }
                    }
                }
                for (int i = 0; i < p_s; i++) {
                    board[stack[i][0]+1][stack[i][1]+1] = 0; // 恢复栈内点的值
                }
                bool half = 0;
                for (int i = 0; i < tail; i++) {
                    if (queue[i][0] == 0 && queue[i][1] == 0) half = 1;
                    if (half) if (queue[i][0] == 12 && queue[i][1] == 12) WIN = 1;
                }
                p_s = 0;
                head = tail = 0;
            }
        } 
    }
    // cout << "间接邻居映射完成" << endl;
    // 遍历所有结点，对其邻接结点进行映射
    for (int x = 1; x <= 11; x++) {
        for (int y = 1; y <= 11; y++) {
            if (board[x][y] == 0) {
                for (int i = 0; i < 6; i++) {
                    int tmx = x + neiber[i][0] - 1;
                    int tmy = y + neiber[i][1] - 1;
                    if (positionReal(tmx, tmy) && board[tmx+1][tmy+1] == 0){  // 在真正的棋盘上并且没有被占据
                        connect[x-1][y-1][tmx][tmy] = connect[tmx][tmy][x-1][y-1] = 1;
                    }
                }
            }
        }
    }
    // if (showx != -1) {
    //     cout << showx << " " << showy << " 邻接映射:" << endl;
    //     for (int i = 0; i < 11; i++) {
    //         for (int j = 0; j < 11; j++) {
    //             cout << connect[10][10][i][j] << " ";
    //         }
    //         cout << endl;
    //     }
    // }
    // cout << endl;
    // cout << "邻接映射后的棋盘:" << endl;
    // for (int i = 0; i <= 12; i++) {
    //     for (int j = 0; j <= 12; j++) {
    //         cout << board[i][j] << " ";
    //     }
    //     cout << endl;
    // }
}

void mapNeiber(const int board[11][11], int connect[11][11][11][11], int color, int dir) {  // 构造邻居映射图，connect作为邻居映射返回,color：作为链接的棋子颜色，dir（color棋子的边，1上下0左右）
    for (int i1 = 0; i1 < 11; i1++)
        for (int i2 = 0; i2 < 11; i2++)
            for (int i3 = 0; i3 < 11; i3++)
                for (int i4 = 0; i4 < 11; i4++)
                    connect[i1][i2][i3][i4] = 0;
    int board2[13][13];  // 棋盘: (0空位)(1棋子)(-1棋子)
    if (dir == 1) {  // 用我color棋子填满边缘 dir(1上下边)(0左右边)
        for (int i = 0; i <= 12; i++) {
            board2[0][i] = board2[12][i] = color;
        }
        for (int i = 1; i <= 11; i++) {
            board2[i][0] = board2[i][12] = 0;
        }
    } else if (dir == 0) {
        for (int i = 0; i <= 12; i++) {
            board2[i][0] = board2[i][12] = color;
        }
        for (int i = 1; i <= 11; i++) {
            board2[0][i] = board2[12][i] = 0;
        }
    }
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            board2[i+1][j+1] = board[i][j];
        }
    }
    // cout << "转换后的初始棋盘：" << endl;
    // for (int i = 0; i <= 12; i++) {
    //     for (int j = 0; j <= 12; j++) {
    //         cout << board2[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    initConnect(board2, connect, color);
}

void edgeDis(const int board[11][11],const int neibers[11][11][11][11],const int color,const int edge, const int dir, int dis[11][11]) {  // 棋子颜色 和边缘标号
    // 到color颜色 edge坐标 dir方向（1上下0左右） 的双距离 存在dis中返回
    bool sure[11][11];
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            sure[i][j] = 0;
            dis[i][j] = 444;  // 被占据的位置距离边也是无穷大且不会被更新

        }
    }
    // 填写边缘距离
    if (dir == 1) {  // 上下边
        for (int i = 0; i < 11; i++) {
            if (board[edge][i] == 0) {  // 是与边缘相连的空位则双距离等于一
                dis[edge][i] = 1;
                sure[edge][i] = 1;
            }
            if (board[edge][i] == color) {  // 是与边缘相连的己方棋子则映射为边缘
                sure[edge][i] = 1;
                for (int ii = 0; ii < 11; ii++) {
                    for (int jj = 0; jj < 11; jj++) {
                        if (neibers[edge][i][ii][jj] && board[ii][jj] == 0) {  // 直接与边缘相连的点
                            // printf("%d %d 的邻接点 %d %d 直接与边缘相连\n", 0, i, ii, jj);
                            dis[ii][jj] = 1;
                            sure[ii][jj] = 1;
                        }
                    }
                }
            }
        }
    } else if (dir == 0){  // 左右边
        for (int i = 0; i < 11; i++) {
            if (board[i][edge] == 0) {  // 是与边缘相连的空位则双距离等于一
                dis[i][edge] = 1;
                sure[i][edge] = 1;
            }
            if (board[i][edge] == color) {  // 是与边缘相连的己方棋子则映射为边缘
                dis[i][edge] = -1;
                sure[i][edge] = 1;
                for (int ii = 0; ii < 11; ii++) {
                    for (int jj = 0; jj < 11; jj++) {
                        if (neibers[i][edge][ii][jj] && board[ii][jj] == 0) {  // 直接与边缘相连的点
                            // printf("%d %d 的邻接点 %d %d 直接与边缘相连\n", 0, i, ii, jj);
                            dis[ii][jj] = 1;
                            sure[ii][jj] = 1;
                        }
                    }
                }
            }
        }
    }
    // 计算到edge的双距离
    int times = 0;
    int t = 1;
    while (t) {
        bool flag = 0;
        for (int x = 0; x < 11; x++) {
            for (int y = 0; y < 11; y++) {
                // 遍历所有结点的邻居，根据已经确定距离的邻居求双距离
                if (dis[x][y] == 1 || board[x][y] != 0) continue;  // 是与边缘直接相连的空位或不是空位则跳过
                int fir = 100000, sec = 100000;
                for (int i = 0; i < 11; i++) {
                    for (int j = 0; j < 11; j++) {
                        if (neibers[x][y][i][j] == 1 && sure[i][j]) {
                            // printf("%d %d 是 %d %d 已经确定距离的邻居\n", i, j, x, y);
                            if (dis[i][j] <= fir) {
                                sec = fir;
                                fir = dis[i][j];
                            } else if (dis[i][j] < sec) {
                                sec = dis[i][j];
                            }
                        }
                    }
                }
                if (sec != 100000 && dis[x][y] != sec+1) {
                    // cout << x << " " << y << " sure f : " << fir << " s : " << sec << endl;
                    dis[x][y] = sec + 1;
                    if (fir == 0) dis[x][y] = 1; 
                    sure[x][y] = 1;
                    flag = 1;
                }
            }
        }
        times++;
        if (!flag) break;
    }
    // cout << "求双距离对整个棋盘遍历了 "  << times << " 次" << endl; 
    // cout << "棋盘上的位置到颜色为 " << color << " 下标为 " << edge <<" 的边的双距离为 : " << endl;
    // for (int i = 0; i < 11; i++) {
    //     for (int j = 0; j < 11; j++) {
    //         printf("%3d ", dis[i][j]);
    //     }
    //     cout << endl;
    // }
    // scanf("%d");
}

void findBestPosition(int board[11][11], int color, int &x, int &y ,int show) {
    double min = 100000;
    int val[11][11];
    bool out = 0;
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            double m = 1000000;
            if (board[i][j] == 0) {
                board[i][j] = color;  // 假设把color棋下在这
                out = 0;
                // if ((i == 0 && j == 0) || (i == 6 && j == 4) || (i == 4 && j == 4)) out = 1;
                m = value(board, val, color, out);
                int mather = 50;
                double e =  (double)(mather - (abs(i-5) + abs(j-5))) / mather;
                m *= e;
                if (out) {
                    printf("if put %d in %d %d : \n", color, i, j);
                    for (int ii = 0; ii < 11; ii++) {
                        for (int jj = 0; jj < 11; jj++) {
                            cout << val[ii][jj] << " ";
                        }
                        cout << endl;
                    }
                    cout << " val : " << m << endl;
                }
                board[i][j] = 0; 
                if (m < min) {
                    min = m;
                    x = i;
                    y = j;
                }
            }
            if (show) m != 1000000 ? printf("%3d", (int)m) : printf("  *");
            // cout << "one judge : " << (double)(current_time - star_time) / CLOCKS_PER_SEC << endl;
        }
        if (show ) printf("\n");
    }
    if (show) cout << "the best value : " << min << endl;
}

bool inputBoard(int board[11][11], int &color) {  // 读入当前棋盘信息
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            board[i][j] = 0;
        }
    }
    int n, N;
    cin >> n;
    N = 2 * n - 1;
    int ch = 1;
    while (N--) {
        int x, y;
        cin >> x >> y;
        if (x == -1) {
            color = 1;
            ARG = FFF;
            continue;
        }
        if (n == 1) {
            secondBest = 1;
            return 0;
        }
        board[x][y] = ch;
        ch *= -1;
    }
    // cout << n << " " << color << endl;
    if (n == 1 && color == 1) return 1;
    return 0;
}