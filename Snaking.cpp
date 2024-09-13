#include <bits/stdc++.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <termio.h>
#include <unistd.h>
using namespace std;

constexpr int WIDTH = 20;
constexpr int HEIGHT = 20;
int DELAY = 100;

int fd;

struct input_event event;

int maze[HEIGHT][WIDTH];
deque<pair<int, int>> a;
deque<pair<int, int>> b;
int dir[4][2] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
int da, db;
bool typ;
bool state[200];

int nxtx(int x, int d) {
    return (x + dir[d][0] + HEIGHT) % HEIGHT;
}

int nxty(int y, int d) {
    return (y + dir[d][1] + WIDTH) % WIDTH;
}

void win(int x) {
    if (x == 1)
        cout << "Player 1 wins!" << endl;
    else if (x == 2)
        cout << "Player 2 wins!" << endl;
    else
        cout << "Draw!" << endl;
}

int operate() {
    a.push_front(make_pair(nxtx(a.front().first, da), nxty(a.front().second, da)));
    b.push_front(make_pair(nxtx(b.front().first, db), nxty(b.front().second, db)));

    bool x = maze[a.front().first][a.front().second] == 1 || maze[a.front().first][a.front().second] == 2;
    bool y = maze[b.front().first][b.front().second] == 2 || maze[b.front().first][b.front().second] == 1;

    if (maze[a.front().first][a.front().second] != 3) {
        maze[a.back().first][a.back().second] = 0;
        a.pop_back();
    }
    if (maze[b.front().first][b.front().second] != 3) {
        maze[b.back().first][b.back().second] = 0;
        b.pop_back();
    }

    if (maze[a.front().first][a.front().second] == 3 || maze[b.front().first][b.front().second] == 3) {
        vector<pair<int, int>> vec;
        for (int i = 0; i < HEIGHT; i++)
            for (int j = 0; j < WIDTH; j++)
                if (maze[i][j] != 1 && maze[i][j] != 2)
                    vec.push_back(make_pair(i, j));
        int tmp = rand();
        int p = vec[tmp % vec.size()].first;
        int q = vec[tmp % vec.size()].second;
        maze[p][q] = 3;
    }

    maze[a.front().first][a.front().second] = 1;
    maze[b.front().first][b.front().second] = 2;

    // return 0;

    if (x && y || a.front().first == b.front().first && a.front().second == b.front().second)
        return 3;
    if (x)
        return 2;
    if (y)
        return 1;
    return 0;
}

void print() {
    system("clear");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int tmp = rand() % 10;
            if (tmp >= 8 && typ)
                cout << "   ";
            else {
                if (maze[i][j] == 1) {
                    cout << "███";
                } else if (maze[i][j] == 2) {
                    cout << "███";
                } else if (maze[i][j] == 3) {
                    cout << "███";
                } else {
                    cout << "\033[30m" << "███" << "\033[0m";
                }
            }
        }
        cout << endl;
    }
    for (int i = 16; i <= 25; i++)
        cerr << state[i];
    cerr << endl;
    for (int i = 30; i <= 38; i++)
        cerr << state[i];
    cerr << endl;
    for (int i = 44; i <= 50; i++)
        cerr << state[i];
    cerr << endl;
}

void init() {
    a.push_back(make_pair(0, 0));
    maze[0][0] = 1;
    b.push_back(make_pair(HEIGHT - 1, WIDTH - 1));
    maze[HEIGHT - 1][WIDTH - 1] = 2;
    vector<pair<int, int>> vec;
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            if (maze[i][j] != 1 && maze[i][j] != 2)
                vec.push_back(make_pair(i, j));
    int t = rand();
    int p = vec[t % vec.size()].first;
    int q = vec[t % vec.size()].second;
    maze[p][q] = 3;
}

void prepare() {
    fd = open("/dev/input/event7", O_RDONLY);
    if (fd == -1) {
        perror("open() failed");
        exit(EXIT_FAILURE);
    }
    typ = 0;
    DELAY = 200;
}

int main() {
    prepare();
    srand(time(NULL));
    init();
    while (1) {
        print();

        memset(state, 0, sizeof(state));
        for (int i = 0; i < 5; i++) {
            if (read(fd, &event, sizeof(event)) == -1) {
                perror("read() failed");
                exit(EXIT_FAILURE);
            }
            if (event.type == EV_KEY) {
                if (event.value == 1) {
                    state[event.code] = true;
                }
            }
            if (state[17] && da != 1)
                da = 3;
            else if (state[30] && da != 0)
                da = 2;
            else if (state[31] && da != 3)
                da = 1;
            else if (state[32] && da != 2)
                da = 0;

            if (state[23] && db != 1)
                db = 3;
            else if (state[36] && db != 0)
                db = 2;
            else if (state[37] && db != 3)
                db = 1;
            else if (state[38] && db != 2)
                db = 0;
        }
        if (typ)
            DELAY = 20 + rand() % 180;
        int now = operate();
        if (now) {
            win(now);
            return 0;
        }
    }

    return 0;
}
