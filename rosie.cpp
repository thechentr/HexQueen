#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <algorithm>
using namespace std;

const int SIZE = 11;
const int BLUE = -1;
const int RED = 1;
const int limit_depth = 2;
const int limit_width = 4;
const int limit = 20;
int board[SIZE][SIZE] = {0}; //本方1，对方-1，空白0
bool WIN = 0;
int ARG = 10;
int FFF = 100;
int secondBest = 0;
int cnt = 0;

struct edge
{
    int x, y;
    edge *next;
};

void creat(int x, int y, int i, int j, int sta[], int &top, struct edge *connect[])
{
    int a = x * 100 + y;
    if (connect[a] == NULL)
    {
        connect[a] = new edge;
        connect[a]->x = i;
        connect[a]->y = j;
        connect[a]->next = NULL;
        sta[top++] = a;
    }
    else
    {
        edge *p = connect[a];
        edge *q = p;
        while (p != NULL)
        {
            if (p->x == i && p->y == j)
            {
                return;
            }
            q = p;
            p = p->next;
        }
        p = new edge;
        p->x = i;
        p->y = j;
        p->next = NULL;
        q->next = p;
    }
}

void del(struct edge *connect[], int sta[], int &top)
{
    for (int i = 0; i < top; i++)
    {
        struct edge *p = connect[sta[i]];
        struct edge *q;
        while (p != NULL)
        {
            q = p;
            p = p->next;
            free(q);
        }
    }
}

// 判断位置在虚拟棋盘上是否合法
bool positionLegal(int x, int y);

// 判断位置在实际棋盘上是否合法
bool positionReal(int x, int y);

// 构造邻居映射图，connect作为邻居映射返回,color：作为链接的棋子颜色，dir（color棋子的边，1上下0左右）
void mapNeiber(const int board[11][11], int sta[], int color, int dir, struct edge *connect[], int &top);

// 用转换后的盘构造邻居映射图
void initConnect(int board[13][13], int sta[], int color, struct edge *connect[], int &top);

// 根据棋子颜色 和边缘标号 求棋盘上的空位置到color色edge边的双距离
void edgeDis(const int board[11][11], struct edge *connect[], const int color, const int edge, const int dir, int dis[11][11], int sta[], int &top);

class Player
{
public:
    Player(int x)
    {
        color = x;
        fill(Board[0], Board[0] + SIZE * SIZE, 0);
    }
    Player() { fill(Board[0], Board[0] + SIZE * SIZE, 0); }
    int color;
    int Board[SIZE][SIZE];              //棋盘，空为0
    int two_distance[SIZE][SIZE] = {0}; //双距离
};

struct GameTree
{
    int x, y;     //最近下的点
    int color;    //表示x，y点的颜色
    int depth;    //博弈树深度
    double value; //估值
    Player p1;
    Player p2; //玩家
    int Next[limit_width][3];
    GameTree *father = NULL;
    GameTree *next[limit_width] = {NULL}; //子节点指针数组
    GameTree(int x1, int y1, int color1, int depth1, Player t, Player p)
    {
        x = x1;
        y = y1;
        color = color1;
        depth = depth1;
        p1 = t;
        p2 = p;
        p1.Board[x][y] = color1;
        p2.Board[x][y] = color1;
    }
};
Player me(BLUE);
Player you(RED);

// 寻找color颜色棋，在board局面下的最佳位置
void findBestPosition(GameTree *root, int board[11][11], int color, int show);

// 读入当前棋盘信息
bool inputBoard(int board[11][11], int &color);

// 根据棋盘计算各个空位置的红距离和蓝距离，赋值到red_dis和blue_dis中返回
void caculate_dis(const int board[11][11], int red_dis[11][11], int blue_dis[11][11]);

double board_value(const int board[11][11], int color, int out);

/*
-----------------------------------------------------------------------
|                       上面是函数声明，下面是函数实现                   |
-----------------------------------------------------------------------
*/

bool positionLegal(int x, int y)
{
    if (0 <= x && x <= 12)
    {
        if (0 <= y && y <= 12)
        {
            return 1;
        }
    }
    return 0;
}

bool positionReal(int x, int y)
{
    if (0 <= x && x < 11)
    {
        if (0 <= y && y < 11)
        {
            return 1;
        }
    }
    return 0;
}

void initConnect(int board[13][13], int sta[], int color, struct edge *connect[], int &top)
{

    int neiber[6][2] = {0, -1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1}; // 6个邻接点
    int stack[150][2], p_s = 0;                                  // 按真正位置储存
    int queue[150][2], head = 0, tail = 0;                       // 按虚拟位置储存

    // 遍历棋盘上所有位置，先对color方棋子造成的间接邻居进行处理
    for (int x = 0; x < 13; x++)
    {
        for (int y = 0; y < 13; y++)
        {
            if (board[x][y] == color)
            {
                queue[tail][0] = x;
                queue[tail++][1] = y;
                board[x][y] *= 2; // 标记是处理过的color方棋子
                // cout<<board[x][y]<<endl;
                while (head != tail)
                {
                    int tmx = queue[head][0];
                    int tmy = queue[head++][1]; // 队首出队
                    // cout << tmx << " " << tmy << "出队" << endl;
                    for (int i = 0; i < 6; i++)
                    {
                        int tmpx = tmx + neiber[i][0];
                        int tmpy = tmy + neiber[i][1];
                        if (positionLegal(tmpx, tmpy))
                        {
                            // cout << tmpx << " " << tmpy << endl;
                            if (board[tmpx][tmpy] == color)
                            { // 仍是color方棋子则继续入队
                                queue[tail][0] = tmpx;
                                queue[tail++][1] = tmpy;
                                board[tmpx][tmpy] *= 2; // 标记是处理过的color方棋子
                                // cout << tmpx << " " << tmpy << "是我方棋子" << endl;
                            }
                            if (board[tmpx][tmpy] == 0 && positionReal(tmpx - 1, tmpy - 1))
                            { // 是空位置则入栈
                                stack[p_s][0] = tmpx - 1;
                                stack[p_s++][1] = tmpy - 1;
                                board[tmpx][tmpy] = 3;
                                // cout << tmpx-1 << " " << tmpy-1 << "是空位置" << endl;
                            }
                        }
                    }
                }
                // 对栈内结点进行邻居映射
                for (int i = 0; i < p_s; i++)
                {
                    for (int j = i + 1; j < p_s; j++)
                    {
                        creat(stack[i][0], stack[i][1], stack[j][0], stack[j][1], sta, top, connect);
                        creat(stack[j][0], stack[j][1], stack[i][0], stack[i][1], sta, top, connect);
                    }
                }
                // 对队内元素和栈内元素进行映射
                for (int i = 0; i < tail; i++)
                {
                    for (int j = 0; j < p_s; j++)
                    {
                        if (positionReal(queue[i][0] - 1, queue[i][1] - 1))
                        {
                            creat(queue[i][0] - 1, queue[i][1] - 1, stack[j][0], stack[j][1], sta, top, connect);
                            creat(stack[j][0], stack[j][1], queue[i][0] - 1, queue[i][1] - 1, sta, top, connect);
                        }
                    }
                }
                for (int i = 0; i < p_s; i++)
                {
                    board[stack[i][0] + 1][stack[i][1] + 1] = 0; // 恢复栈内点的值
                }
                p_s = 0;
                head = tail = 0;
            }
        }
    }
    // cout << "间接邻居映射完成" << endl;
    // 遍历所有结点，对其邻接结点进行映射
    for (int x = 1; x <= 11; x++)
    {
        for (int y = 1; y <= 11; y++)
        {
            if (board[x][y] == 0)
            {
                for (auto &i : neiber)
                {
                    int tmx = x + i[0] - 1;
                    int tmy = y + i[1] - 1;
                    if (positionReal(tmx, tmy) && board[tmx + 1][tmy + 1] == 0)
                    { // 在真正的棋盘上并且没有被占据
                        creat(x - 1, y - 1, tmx, tmy, sta, top, connect);
                        creat(tmx, tmy, x - 1, y - 1, sta, top, connect);
                        // cout<<x-1<<" "<<y-1<<" "<<tmx<<" "<<tmy<<" "<<endl;
                    }
                }
            }
        }
    }
}

void mapNeiber(const int board[11][11], int sta[], int color, int dir, struct edge *connect[], int &top)
{
    int board2[13][13]; // 棋盘: (0空位)(1棋子)(-1棋子)
    if (dir == 1)
    { // 用我color棋子填满边缘 dir(1上下边)(0左右边)
        for (int i = 0; i <= 12; i++)
        {
            board2[0][i] = board2[12][i] = color;
        }
        for (int i = 1; i <= 11; i++)
        {
            board2[i][0] = board2[i][12] = 0;
        }
    }
    else if (dir == 0)
    {
        for (int i = 0; i <= 12; i++)
        {
            board2[i][0] = board2[i][12] = color;
        }
        for (int i = 1; i <= 11; i++)
        {
            board2[0][i] = board2[12][i] = 0;
        }
    }
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            board2[i + 1][j + 1] = board[i][j];
        }
    }
    initConnect(board2, sta, color, connect, top);
}

void edgeDis(const int board[11][11], struct edge *connect[], const int color, const int edge, const int dir, int dis[11][11], int sta[], int &top)
{ // 棋子颜色 和边缘标号
    // 到color颜色 edge坐标 dir方向（1上下0左右） 的双距离 存在dis中返回
    bool sure[11][11];
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            sure[i][j] = 0;
            dis[i][j] = 444; // 被占据的位置距离边也是无穷大且不会被更新
        }
    }
    // 填写边缘距离
    if (dir == 1)
    { // 上下边
        for (int i = 0; i < 11; i++)
        {
            if (board[edge][i] == 0)
            { // 是与边缘相连的空位则双距离等于一
                dis[edge][i] = 1;
                sure[edge][i] = 1;
            }
            if (board[edge][i] == color)
            { // 是与边缘相连的己方棋子则映射为边缘
                sure[edge][i] = 1;
                struct edge *p = connect[edge * 100 + i];
                while (p != NULL)
                {
                    int m = p->x;
                    int j = p->y;
                    if (board[m][j] == 0)
                    {
                        dis[m][j] = 1;
                        sure[m][j] = 1;
                    }
                    p = p->next;
                }
            }
        }
    }
    else if (dir == 0)
    { // 左右边
        for (int i = 0; i < 11; i++)
        {
            if (board[i][edge] == 0)
            { // 是与边缘相连的空位则双距离等于一
                dis[i][edge] = 1;
                sure[i][edge] = 1;
            }
            if (board[i][edge] == color)
            { // 是与边缘相连的己方棋子则映射为边缘
                dis[i][edge] = -1;
                sure[i][edge] = 1;
                struct edge *p = connect[i * 100 + edge];
                while (p != NULL)
                {
                    int m = p->x;
                    int j = p->y;
                    if (board[m][j] == 0)
                    {
                        dis[m][j] = 1;
                        sure[m][j] = 1;
                    }
                    p = p->next;
                }
            }
        }
    }
    // 计算到edge的双距离
    int times = 0;
    int t = 1;
    while (t)
    {
        bool flag = 0;
        for (int k = 0; k < top; k++)
        {
            if (connect[sta[k]] == NULL)
            {
                continue;
            }
            else if (connect[sta[k]] != NULL)
            {
                int x = (sta[k]) / 100;
                int y = (sta[k]) % 100;
                if (dis[x][y] == 1 || board[x][y] != 0)
                    continue;
                int fir = 100000, sec = 100000;
                struct edge *p = connect[sta[k]];
                while (p != NULL)
                {
                    int i = p->x;
                    int j = p->y;
                    if (sure[i][j])
                    {
                        if (dis[i][j] <= fir)
                        {
                            sec = fir;
                            fir = dis[i][j];
                        }
                        else if (dis[i][j] < sec)
                        {
                            sec = dis[i][j];
                        }
                    }
                    p = p->next;
                }
                if (sec != 100000 && dis[x][y] != sec + 1)
                {
                    // cout << x << " " << y << " sure f : " << fir << " s : " << sec << endl;
                    dis[x][y] = sec + 1;
                    // cout<<dis[x][y]<<endl;
                    if (fir == 0)
                        dis[x][y] = 1;
                    sure[x][y] = 1;
                    flag = 1;
                }
            }
        }
        times++;
        if (!flag)
            break;
    }
}

bool inputBoard(int &x, int &y, int &n, int &color)
{ // 读入当前棋盘信息
    int N;
    cin >> n;
    N = 2 * n - 1;
    int ch = 1;
    while (N--)
    {
        cin >> x >> y;
        if (x == -1)
        {
            me.color = RED;
            you.color = BLUE;
            color = 1;
            ARG = FFF;
            continue;
        }
        if (n == 1)
        {
            secondBest = 1;
            return 0;
        }
        board[x][y] = ch;
        me.Board[x][y] = ch;
        you.Board[x][y] = ch;
        ch *= -1;
    }
    // cout << n << " " << color << endl;
    if (n == 1 && color == 1)
        return 1;
    return 0;
}

double board_value(const int board[11][11], int color, int out)
{
    int blue_dis[11][11], red_dis[11][11];
    //caculate_dis(board, red_dis, blue_dis);
    struct edge *connect[1200] = {NULL};
    int sta[1200]; //保存散列邻接表的情况
    int top = 0;   //保存栈的数目
    int dis[11][11];
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            blue_dis[i][j] = red_dis[i][j] = 0;
        }
    }
    // 分别求到四条边的双距离
    mapNeiber(board, sta, 1, 1, connect, top);       // 对整个棋盘进行红棋的邻接映射
    // 判断红赢？？？
    for (int i = 0; i < 11; i++)
    {
        if (board[0][i] == 1)
        {
            for (int j = 0; j < 11; j++)
            {
                if (board[10][j] == 1)
                {
                    struct edge * p=connect[i];
                    while(p!=NULL){
                        if(p->x==10&&p->y==j){
                            return -999999 * color;
                        }
                        p=p->next;
                    }
                }
            }
        }
    }
    edgeDis(board, connect, 1, 0, 1, dis, sta, top); // 求到上边（1方棋子的边）的双距离
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            red_dis[i][j] += dis[i][j];
        }
    }
    edgeDis(board, connect, 1, 10, 1, dis, sta, top); // 求到下边（1方棋子的边）的双距离
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            red_dis[i][j] += dis[i][j];
        }
    }
    del(connect, sta, top);
    top = 0;
    for (int i = 0; i < 1200; i++)
    {
        connect[i] = NULL;
    }
    mapNeiber(board, sta, -1, 0, connect, top);       // 对整个棋盘进行蓝棋的邻接映射
    // 判断蓝赢？？？
    for (int i = 0; i < 11; i++)
    {
        if (board[0][i] == -1)
        {
            for (int j = 0; j < 11; j++)
            {
                if (board[10][j] == 1)
                {
                    struct edge * p=connect[i];
                    while(p!=NULL){
                        if(p->x==10&&p->y==j){
                            return 999999 * color;
                        }
                        p=p->next;
                    }
                }
            }
        }
    }
    edgeDis(board, connect, -1, 0, 0, dis, sta, top); // 求左边（-1方棋子的边）的双距离
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            blue_dis[i][j] += dis[i][j];
        }
    }
    edgeDis(board, connect, -1, 10, 0, dis, sta, top); // 求到右边（-1方棋子的边）的双距离
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            blue_dis[i][j] += dis[i][j];
        }
    }
    del(connect, sta, top);
    int min_reds = 0, min_blues = 0; // 红蓝双距离最小的数量
    int val[11][11];                 // 红黑双距离之和
    int blue_dis_min = 100000;
    int red_dis_min = 100000;
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            val[i][j] = red_dis[i][j] + blue_dis[i][j];
            if (red_dis[i][j] == red_dis_min) min_reds++;
            if (red_dis[i][j] < red_dis_min) {
                min_reds = 1;
                red_dis_min = red_dis[i][j];
            }
            if (blue_dis[i][j] == blue_dis_min) min_blues++;
            if (blue_dis[i][j] < blue_dis_min) {
                min_blues = 1;
                blue_dis_min = blue_dis[i][j];
            }
        }
    }
    if (out)
    {
        cout << endl;
        cout << "到红边的双距离 : " << endl;
        for (int i = 0; i < 11; i++)
        {
            for (int j = 0; j < 11; j++)
            {
                cout << red_dis[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        cout << "到蓝边的双距离 : " << endl;
        for (int i = 0; i < 11; i++)
        {
            for (int j = 0; j < 11; j++)
            {
                cout << blue_dis[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        cout << "到红蓝双边的双距离 : " << endl;
        for (int i = 0; i < 11; i++)
        {
            for (int j = 0; j < 11; j++)
            {
                cout << val[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        printf("R_dis : %d B_dis %d\n", red_dis_min, blue_dis_min);
        printf("R_num : %d B_num %d\n", min_reds, min_blues);
    }

    int move = min_reds - min_blues;
    int ARG = 100;
    int ctr = -1;
    if (cnt > 21) ctr = 1;
    return (double)(ARG * (color * (red_dis_min - blue_dis_min)) - ctr * color * move); // 考虑当前局面形式
}





void findBestPosition(GameTree *root, int board[11][11], int color, int show)
{
    double min = 100000;
    int val[11][11];
    pair<int, int> pr[SIZE * SIZE];
    bool out = 0;
    for (int i = 0; i < SIZE * SIZE; i++)
    {
        pr[i].first = 10000000;
    }
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            double m = 1000000;
            if (board[i][j] == 0)
            {
                board[i][j] = color; // 假设把color棋下在这
                out = 0;
                m = board_value(board, color, out);
                double e = (double)(50 - (abs(i - 5) + abs(j - 5))) / 50; // 对棋盘不同位置赋予不同权重
                m *= e;
                pr[i * SIZE + j].first = m;
                pr[i * SIZE + j].second = i * SIZE + j;
                if (out)
                {
                    printf("if put %d in %d %d : \n", color, i, j);
                    for (int ii = 0; ii < 11; ii++)
                    {
                        for (int jj = 0; jj < 11; jj++)
                        {
                            cout << val[ii][jj] << " ";
                        }
                        cout << endl;
                    }
                    cout << " val : " << m << endl;
                }
                board[i][j] = 0;
                if (m < min)
                {
                    min = m;
                }
            }
            if (show)
                m != 1000000 ? printf("%5d", (int)m) : printf("    *");
        }
        if (show)
            printf("\n");
    }
    if (show)
        cout << "the best value : " << min << endl;
    vector<pair<int, int>> vc;
    vc.reserve(SIZE * SIZE + 1);
    for (int i = 0; i < SIZE * SIZE; i++)
    {
        vc.push_back(pr[i]);
    }
    sort(vc.begin(), vc.end()); //排序找到估值函数最小的limit_width个值
    for (int i = 0; i < limit_width; i++)
    {
        root->Next[i][0] = vc[i].second / 11;
        root->Next[i][1] = vc[i].second % 11;
        root->Next[i][2] = vc[i].first;
    }
}

void build_gametree(GameTree *root)
{
    if (root->depth == limit_depth)
        return;
    findBestPosition(root, root->p1.Board, -root->color, 0);
    for (int j = 0; j < limit_width; j++)
    {
        root->next[j] = new GameTree(root->Next[j][0], root->Next[j][1], -root->color, root->depth + 1, root->p1, root->p2);
        root->next[j]->father = root;
        root->next[j]->value = root->Next[j][2];
        if (cnt < limit)
            return;
        build_gametree(root->next[j]);
    }
}

void choose_position(int &newx, int &newy, GameTree *root)
{
    if (cnt < limit)
        return;
    double sum[3] = {0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            sum[i] += root->next[i]->Next[j][2];
        }
        if (root->Next[i][2] > 10000 || root->Next[i][2] < -10000)
            return;
        sum[i] = sum[i] - root->Next[i][2];
    }
    double max = -100000000;
    int n = -1;
    for (int i = 0; i < 3; i++)
    {
        if (sum[i] > max)
        {
            max = sum[i];
            n = i;
        }
    }
    newx = root->next[n]->x;
    newy = root->next[n]->y;
    return;
}

int cal_full_point(int secondBest, int n)
{
    if (secondBest == 1)
    {
        return 2 * n - 1;
    }
    else
    {
        return 2 * n - 2;
    }
}

int main()
{
    int x = 0, y = 0;
    int n = 0;
    int new_x = 0, new_y = 0;
    int color = -1;
    if (inputBoard(x, y, n, color))
    {
        cout << "1 2" << endl;
        return 0;
    }
    if (secondBest == 1)
    {
        cout << "7 3" << endl;
        return 0;
    }
    cnt = cal_full_point(secondBest, n);
    GameTree *root = new GameTree(x, y, you.color, 0, me, you);
    build_gametree(root);
    new_x = root->Next[0][0];
    new_y = root->Next[0][1];
    choose_position(new_x, new_y, root);
    // 向平台输出决策结果
    cout << new_x << ' ' << new_y << endl;
    cout << "The run time is:" << (double)clock() / CLOCKS_PER_SEC << "s" << endl;
    return 0;
}