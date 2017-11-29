#include <cstdio>
#include <curses.h>
#include <csignal>
#include <cstring>
#include <list>
#include <cstdlib>
#include "set_ticker.c"
#include <random>

using std::default_random_engine;
using std::uniform_real_distribution;

const int ROW = 15;
const float P = 0.1f; // 出现障碍物的概率

default_random_engine e;
uniform_real_distribution<float> u(0, 1); //随机数分布对象

struct Rock {
    const char *MSG = "@";
    int col = COLS;
};

std::list<Rock *> rocks;

struct People {
    const int COL = 5;
    const char *MSG = "*";

    int row = ROW;
    int dir = 0;
};

People people;

int score;
int level;
int delay;

void tick(int);

// 画地面
void drawGround() {
    for (int i = 0; i < COLS; i++) {
        move(ROW + 1, i);
        addstr("'");
    }
}

// 初始化游戏
void initGame() {
    clear();
    rocks = std::list<Rock *>();
    people.row = ROW;
    people.dir = 0;
    delay = 60;
    score = 0;
    level = 1;

    drawGround();                   // 画地面
    move(people.row, people.COL);   // 画人
    addstr(people.MSG);
    move(5, COLS - 5);              // 画分数
    addstr("0");
    move(5, COLS - 15);
    addstr("level 1");
    signal(SIGALRM, tick);
    set_ticker(delay);
}

void setLevel(int flag) {
    if (flag > 0) {
        if (level >= 9)
            return;
        delay -= 5;
        level++;
    } else {
        if (level <= 0)
            return;
        delay += 5;
        level--;
    }

    char levelStr[10];
    sprintf(levelStr, "level %d", level);
    move(5, COLS - 15);
    addstr(levelStr);
    set_ticker(delay);
}

int main() {
    initscr();
    crmode();
    noecho();

    initGame();                     // 初始化游戏

    while (true) {
        int c = getch();
        switch (c) {
            case 'q':
                endwin();
                exit(0);
            case 'r':
                for (auto i = rocks.begin(); i != rocks.end(); i++) {
                    delete *i;
                }
                initGame();
                break;
            case ' ':
                if (people.dir == 0) people.dir = -1;
                break;
            case '=':
                setLevel(1);
                break;
            case '-':
                setLevel(-1);
                break;
            default:
                break;
        }
    }
}


int flag = 0;

void tick(int signum) {
    signal(SIGALRM, tick);              /* reset, just in case	*/

    if (people.row == ROW && people.dir == 1) // 到达地面
        people.dir = 0;
    else if (people.row == ROW - 4 && people.dir == -1) // 到达最高点
        people.dir = 1;
    else if (people.dir != 0) {
        move(people.row, people.COL);         // 删除原来的人
        addstr(" ");

        people.row += people.dir;

        move(people.row, people.COL);        // 画人
        addstr(people.MSG);
    }

    if (--flag <= 0 && u(e) < P) {           // 根据概率创建石头 出现在屏幕右边 并加入队尾
        rocks.push_back(new Rock);
        flag = 20 - level;
    }

    for (auto i = rocks.begin(); i != rocks.end(); i++) {
        if ((*i)->col != 5 || people.row != ROW) {
            move(ROW, (*i)->col);           // 清空原来的石头
            addstr(" ");
        }
        (*i)->col -= 1;                     // 左移一位

        if ((*i)->col < 0) {                // 一旦超出屏幕左侧 删除
            delete *i;
            i = rocks.erase(i);
        } else {
            move(ROW, (*i)->col);           // 画新石头
            addstr((*i)->MSG);
        }

        if ((*i)->col == 5) {               // 检测小人位置是否碰撞
            if (people.row == ROW) {        // 发生碰撞
                move(5, 5);
                addstr("GAME OVER!");
                set_ticker(0);              // 游戏结束
                refresh();
                return;
            } else {
                score++;                    // 没有碰撞 加分
                if (score % 10 == 9) setLevel(1);
                char scoreStr[5];
                sprintf(scoreStr, "%d", score);
                move(5, COLS - 5);
                addstr(scoreStr);
            }
        }
    }

    move(LINES - 1, 0);
    refresh();
}