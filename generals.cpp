#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <cstring>
#include <ctime>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1 : 0)
using std::cin;
using std::cout;
using std::endl;
using std::fill;
using std::ifstream;
using std::ios;
using std::make_pair;
using std::map;
using std::max;
using std::min;
using std::ofstream;
using std::pair;
using std::queue;
using std::random_shuffle;
using std::set;
using std::sort;
using std::stack;
using std::string;
using std::stringstream;
using std::swap;
using std::vector;
enum land_type
{
    Empty_land,
    Wall,
    General,
    Land,
    Empty_city,
    City,
    Health,
    Ac,
    Sword,
    Flag,
    Empty_flag,
    Exhealth,
    Exac,
    Exsword,
    Light,
    Pill,
    Expill,
    Twoscope,
    Fh,
    Exfh,
    Points,
    Send,
    Grrenade
};
enum game_mode
{
    Nil,
    Ffa,
    Tdm,
    Fvf
};
enum map_mode
{
    Diy,
    Random,
    Blank,
    Maze,
    Dboat,
    Pubg,
    CFlag,
    CPoints,
    Paint,
    Qianhao
};
enum Movement
{
    Up,
    Right,
    Down,
    Left
};
struct node
{
    int belong, tmp;
    land_type type;
    void tozero()
    {
        belong = tmp = 0;
        type = Empty_land;
        return;
    }
} mp[105][105];

int X = 15, Y = 15;     //地图的长和宽。如果以迷宫地图，请确保地图的长和宽均为奇数
double wallpr = 0.13;   //墙的密度
double citypr = 0.05;   //城市的密度
double objectpr = 0.06; //道具的密度
int tpt = 100;          //每个回合后的等待时间。如果想体验原速，建议设为 400
int gennum = 4;         //玩家的数量。当然只有沙雕 Bot
int teamnum = 2;        //队伍的数量。如果以 TDM 模式游玩，请确保玩家数量能被队伍数量整除
int dq = 40;            //毒圈的扩散时间
int kttime = 100;       //空投的投放时间。请保证此变量大于 10
int pointstime = 20;    //占领一个据点所需的时间
int paintTime = 300;    //涂色地图的时间

int dir[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
int vis[105][105];
bool sight[105][105][105];
int Inteam[105];
game_mode mode;
map_mode mapmode;
bool teamdead[105];
bool starting = true;
vector<land_type> objects, normalobjects;
int blindtimeremain[105];
int ktx, kty;
bool ktiscoming = false;
bool iskt[105][105];
bool ishavets[105];
int ktremaintime = -1;
int isreplay;
bool isPaint;
int paintRemainTime;
bool isHaveSend[105];
int randnum(int l, int r)
{
    return rand() % (r - l + 1) + l;
}
set<int> ifteam[105];
int rm, fog[105][105];
int flagscore[105];
bool ifcanconvobject;
bool fvf;
bool isgz;
bool opt;
void convmap()
{
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            mp[i][j].tozero();
    return;
}
void gotoxy(int x, int y)
{
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    HANDLE hConsoleOut;
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsoleOut, &csbiInfo);
    csbiInfo.dwCursorPosition.X = x;
    csbiInfo.dwCursorPosition.Y = y;
    SetConsoleCursorPosition(hConsoleOut, csbiInfo.dwCursorPosition);
    return;
}
string myto_string(int x)
{
    stringstream sst;
    sst << x;
    string ans;
    sst >> ans;
    return ans;
}
int order[105];
int dist(int xx1, int yy1, int xx2, int yy2)
{
    return abs(xx1 - xx2) + abs(yy1 - yy2);
}
int alivegennum = gennum, aliveteamnum = teamnum;
void congen()
{
    for (int i = 1; i <= gennum; i++)
        order[i] = i;
    random_shuffle(order + 1, order + gennum + 1);
    for (int i = 1; i <= gennum; i++)
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != Empty_land)
                continue;
            bool flag = false;
            for (int k = 1; k <= X; k++)
                for (int w = 1; w <= Y; w++)
                    if (mp[k][w].type == General && dist(px, py, k, w) <= 6)
                    {
                        flag = true;
                        k = X + 1;
                        break;
                    }
            if (flag)
                continue;
            mp[px][py].type = General;
            mp[px][py].belong = order[i];
            break;
        }
    }
    return;
}
void concit()
{
    int citynum = double(X * Y) * citypr;
    for (int i = 1; i <= citynum; i++)
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != Empty_land)
                continue;
            mp[px][py].type = Empty_city;
            mp[px][py].tmp = randnum(35, 50);
            break;
        }
    }
    return;
}
void dfs(int cnt, int x, int y)
{
    vis[x][y] = cnt;
    for (int i = 0; i < 4; i++)
    {
        int px = x + dir[i][0], py = y + dir[i][1];
        if (px >= 1 && px <= X && py >= 1 && py <= Y && mp[px][py].type != Wall && !vis[px][py])
        {
            dfs(cnt, px, py);
        }
    }
    return;
}
int objectnum, aliveobjectnum;
bool checkwall()
{
    memset(vis, 0, sizeof(vis));
    int cnt = 0, tp = 0;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type != Wall && !vis[i][j])
            {
                cnt++;
                dfs(cnt, i, j);
            }
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type == General)
            {
                if (tp == 0)
                    tp = vis[i][j];
                else if (vis[i][j] != tp)
                    return false;
            }
    return true;
}
void convwall()
{
    int wallnum = double(X * Y) * wallpr;
    for (int i = 1; i <= wallnum; i++)
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != Empty_land)
                continue;
            mp[px][py].type = Wall;
            if (checkwall())
                break;
            mp[px][py].type = Empty_land;
        }
    }
    return;
}
int etot, vtot, venum[105][105], id[1000005];
int playermaxhp[105];
bool isGMode;
struct edge
{
    int a, b, w, posa, posb;
} edges[1000005];
bool cmpe(const edge &s1, const edge &s2)
{
    return s1.w < s2.w;
}
int find(int x)
{
    if (x == id[x])
        return x;
    return id[x] = find(id[x]);
}
//https://github.com/By-Ha/Checkmate/blob/master/game/map.js
void convmaze()
{
    convmap();
    etot = vtot = 0;
    memset(venum, 0, sizeof(venum));
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (i % 2 == 0 && j % 2 == 0)
                mp[i][j].type = Wall;
            else if (i % 2 == 1 && j % 2 == 1)
                venum[i][j] = vtot++;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
        {
            int tmp1 = i - 1, tmp3 = j - 1, tmp4 = j + 1, tmp2 = i + 1;
            if (i % 2 == 0 && j % 2 == 1)
            {
                venum[i][j] = etot;
                edges[etot] = (edge){venum[tmp1][j], venum[tmp2][j], 10 + randnum(1, 10), i, j};
                ++etot;
            }
            if (i % 2 == 1 && j % 2 == 0)
            {
                venum[i][j] = etot;
                edges[etot] = (edge){venum[i][tmp3], venum[i][tmp4], 10 + randnum(1, 10), i, j};
                ++etot;
            }
        }
    sort(edges, edges + etot, cmpe);
    for (int i = 0; i < vtot; i++)
        id[i] = i;
    for (int i = 0; i < etot; i++)
    {
        if (find(edges[i].a) != find(edges[i].b))
        {
            id[find(edges[i].a)] = id[(edges[i].b)];
            mp[edges[i].posa][edges[i].posb].type = Empty_city;
            mp[edges[i].posa][edges[i].posb].tmp = 10;
        }
        else
        {
            mp[edges[i].posa][edges[i].posb].type = Wall;
        }
    }
    int calcTimes = 0;
    for (int i = 1; i <= gennum; i++)
    {
        ++calcTimes;
        if (calcTimes >= 100)
        {
            convmaze();
            return;
        }
        int t1 = randnum(1, X), t2 = randnum(1, X);
        while (1)
        {
            t1 = randnum(1, X), t2 = randnum(1, X);
            int tmpcnt = 0;
            if (t1 - 1 >= 1)
            {
                if (mp[t1 - 1][t2].type != Wall)
                {
                    tmpcnt++;
                }
            }
            if (t2 - 1 >= 1)
            {
                if (mp[t1][t2 - 1].type != Wall)
                {
                    tmpcnt++;
                }
            }
            if (t1 + 1 <= X)
            {
                if (mp[t1 + 1][t2].type != Wall)
                {
                    tmpcnt++;
                }
            }
            if (t2 + 1 <= X)
            {
                if (mp[t1][t2 + 1].type != Wall)
                {
                    tmpcnt++;
                }
            }
            if (mp[t1][t2].type == Empty_land && tmpcnt == 1)
                break;
        }
        mp[t1][t2].belong = i;
        mp[t1][t2].type = General;
    }
    for (int i = 1; i <= (X * Y) / 15; ++i)
    {
        int tryTime = 0;
        while (true)
        {
            ++tryTime;
            int x = randnum(1, X), y = randnum(1, X);
            if (tryTime >= 20)
            {
                break;
            }
            int flag = 0;
            for (int t1 = -1; t1 <= 1; ++t1)
            {
                for (int t2 = -1; t2 <= 1; ++t2)
                {
                    if (t1 == 0 && t2 == 0)
                        continue;
                    if (x + t1 > 0 && x + t1 <= X && y + t2 <= X)
                    {
                        if (mp[x + t1][y + t2].type == General)
                        {
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag)
                    break;
            }
            if (flag || x % 2 == y % 2)
                continue;
            if (mp[x][y].type == Wall)
            {
                mp[x][y].type = Empty_city;
                mp[x][y].tmp = 10;
                break;
            }
        }
    }
    return;
}
int viss[105][105];
struct point
{
    int a, b, c;
};
int Astar(int x, int y, int tar_x, int tar_y)
{
    memset(viss, 0, sizeof(viss));
    queue<point> q;
    q.push((point){x, y, 0});
    viss[x][y] = 1;
    while (!q.empty())
    {
        int tx = q.front().a, ty = q.front().b, step = q.front().c;
        q.pop();
        for (int j = 0; j < 4; ++j)
        {
            int tx2 = tx + dir[j][0], ty2 = ty + dir[j][1];
            if (tx2 > X || ty2 > X || tx2 <= 0 || ty2 <= 0 || mp[tx2][ty2].type == Wall || viss[tx2][ty2])
                continue;
            viss[tx2][ty2] = 1;
            q.push((point){tx2, ty2, step + 1});
            if (tx2 == tar_x && ty2 == tar_y)
                return step + 1;
        }
    }
    return -1;
}
struct node2
{
    int a, b;
};
void convdragon()
{
    convmap();
    vector<node2> lst;
    int calcTimes = 0;
    for (int i = 1; i <= gennum; i++)
    {
        ++calcTimes;
        if (calcTimes >= 100)
        {
            convdragon();
            return;
        }
        int t1 = randnum(1, X - 2) + 1, t2 = randnum(1, Y - 2) + 1;
        while (mp[t1][t2].type != Empty_land || (mp[t1 + 1][t2].type != Empty_land && mp[t1 - 1][t2].type != Empty_land && mp[t1][t2 + 1].type != Empty_land && mp[t1][t2 + 1].type != Empty_land))
            t1 = randnum(1, X - 2) + 1, t2 = randnum(1, Y - 2) + 1;
        if (i == 1)
        {
            mp[t1][t2].belong = i;
            mp[t1][t2].tmp = 100;
            mp[t1][t2].type = General;
        }
        else
        {
            int flag = 0;
            for (int j = 0; j < (int)lst.size(); ++j)
            {
                if (Astar(t1, t2, lst[j].a, lst[j].b) > 6)
                {
                    continue;
                }
                flag = 1;
                --i;
                break;
            }
            if (flag == 0)
            {
                mp[t1][t2].belong = i;
                mp[t1][t2].tmp = 100;
                mp[t1][t2].type = General;
            }
        }
        lst.push_back((node2){t1, t2});
    }
    for (int i = 1; i <= gennum; ++i)
    {
        for (int j = 1; j <= min(X * Y / gennum / 10, 8); ++j)
        {
            int t1 = randnum(1, X - 2) + 1, t2 = randnum(1, Y - 2) + 1;
            while (mp[t1][t2].belong != 0 || mp[t1][t2].type != Empty_land)
            {
                t1 = randnum(1, X - 2) + 1;
                t2 = randnum(1, Y - 2) + 1;
            }
            if (j == 1)
            {
                mp[t1][t2].belong = i;
                mp[t1][t2].tmp = 50;
                mp[t1][t2].type = City;
            }
            else
            {
                mp[t1][t2].belong = i;
                mp[t1][t2].tmp = 5;
                mp[t1][t2].type = Land;
            }
        }
    }
    return;
}
//https://github.com/By-Ha/Checkmate/pull/15/files
void generateQianHaoMap()
{
    congen();
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type == General)
            {
                vector<pair<int, int> > tmp;
                for (int k = i - 1; k <= i + 1; k++)
                    for (int w = j - 1; w <= j + 1; w++)
                        if (k >= 1 && k <= X && w >= 1 && w <= Y && mp[k][w].type == Empty_land)
                        {
                            mp[k][w].type = Wall;
                            if (dist(i, j, k, w) == 1 && (k != 1 && w != 1 && k != X && w != Y))
                                tmp.push_back(make_pair(k, w));
                        }
                int g = randnum(0, tmp.size() - 1);
                mp[tmp[g].first][tmp[g].second].type = Empty_land;
            }
    return;
}
int nowpri;
void SetColor(int ForeColor, int BackGroundColor, int pri)
{
    if (nowpri != 0 && pri < nowpri)
        return;
    nowpri = pri;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, ForeColor | BackGroundColor);
    return;
}
void Setcolor(int ForeColor = 15, int BackGroundColor = 0)
{
    nowpri = 0;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, ForeColor | BackGroundColor);
    return;
}
int cls[13] = {13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
int clsNum = 13;
int foglevel;
struct News
{
    int opt, a, b, remtime;
} news[10005];
int newsl, newsr;
void printPlayer(int id, bool isTeam = true)
{
    if (isTeam)
    {
        SetColor(cls[Inteam[id] % clsNum], 0, 100);
        printf("player%d", id);
        Setcolor();
    }
    else
    {
        SetColor(cls[id % clsNum], 0, 100);
        printf("player%d", id);
        Setcolor();
    }
    return;
}
void printTeam(int id)
{
    SetColor(cls[id % clsNum], 0, 100);
    printf("team%d", id);
    Setcolor();
    return;
}
void displaynews()
{
    while (newsr - newsl > 5)
        newsl++;
    for (int i = newsl; i < newsr; i++)
    {
        if (news[i].opt == 1)
            printPlayer(news[i].a), printf(" 拿到了 "), printTeam(news[i].b), printf(" 的旗帜\n");
        else if (news[i].opt == 2)
            printTeam(news[i].a), printf(" 的旗帜被重置了\n");
        else if (news[i].opt == 3)
            printPlayer(news[i].a), printf(" 为 "), printTeam(Inteam[news[i].a]), printf(" 得分了\n");
        else if (news[i].opt == 4)
            printTeam(news[i].a), printf(" 占领了一个据点\n");
        else if (news[i].opt == 5)
            printTeam(news[i].a), printf(" 失去了一个据点\n");
        news[i].remtime--;
        if (news[i].remtime <= 0)
            newsl++;
    }
    return;
}
struct scoreboard
{
    int sco, id, lnd;
} score[105];
bool cmpsco(const scoreboard &s1, const scoreboard &s2)
{
    if (s1.sco != s2.sco)
        return s1.sco > s2.sco;
    return s1.id < s2.id;
}
int wd[105];
int ifgetflag[105];
void getnum(int x)
{
    if (x < 100)
        printf("%2d", x);
    else if (x >= 100 && x < 950)
        printf("%dH", int(round(double(x) / 100.0)));
    else if (x >= 950 && x < 9500)
        printf("%dK", int(round(double(x) / 1000.0)));
    else
        printf("9K");
    return;
}
int playeratk[105], playerac[105], playerfh[105], playerGrenade[105];
int isGrenade[105][105];
int teampointsmatchscore[105], teampointsmatchland[105];
struct Grenade
{
    int sx, sy, ex, ey, dmg, frm, cx, cy, tmp;
    void tozero()
    {
        sx = sy = ex = ey = dmg = frm = cx = cy = 0;
        return;
    }
};
vector<Grenade> currentGrenade;
void addGrenade(int sx, int sy, int ex, int ey, int dmg, int frm)
{
    currentGrenade.push_back((Grenade){sx, sy, ex, ey, dmg, frm, sx, sy, 0});
    return;
}
void updateGrenade()
{
    if (currentGrenade.empty())
        return;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            isGrenade[i][j] = 0;
    for (vector<Grenade>::iterator it = currentGrenade.begin(); it != currentGrenade.end();)
    {
        Grenade &cur = *it;
        if (dist(cur.cx, cur.cy, cur.ex, cur.ey) <= 1 || cur.cx < 1 || cur.cx > X || cur.cy < 1 || cur.cy > Y || cur.tmp >= (isGMode ? 10 : 6))
        {
            for (int i = cur.ex - 3; i <= cur.ex + 3; i++)
                for (int j = cur.ey - 3; j <= cur.ey + 3; j++)
                    if (i >= 1 && i <= X && j >= 1 && j <= Y && mp[i][j].type == General && mp[i][j].tmp > 0 && (mode == Ffa || mode == Tdm && ifteam[Inteam[mp[i][j].belong]].find(cur.frm) == ifteam[Inteam[mp[i][j].belong]].end() || mp[i][j].belong == cur.frm))
                    {
                        mp[i][j].tmp -= cur.dmg;

                        if (mapmode == Pubg && mp[i][j].tmp <= 0)
                        {
                            mp[i][j].tozero();
                            alivegennum--;
                        }
                    }
            it = currentGrenade.erase(it);
            continue;
        }
        cur.cx += int(ceil(double(cur.ex - cur.sx) / 5.0));
        cur.cy += int(ceil(double(cur.ey - cur.sy) / 5.0));
        cur.tmp++;
        isGrenade[cur.cx][cur.cy] = cur.frm;
        it++;
    }
    return;
}
struct teamscore
{
    int score, pos;
} nflagscore[105];
bool cmpteam(const teamscore &s1, const teamscore &s2)
{
    return s1.score > s2.score;
}
int sm[105][105];
char miniMap[105][105];
bool isMiniMap;
int miniMapLevel = 9;
double xdivs = 5.0, ydivs = 5.0;
int startingX, endingX, startingY, endingY;
void printCurrentMiniMap(int bgmp, int id)
{
    for (int j = int(double(startingY - 1) / ydivs) + 1; j <= int(double(endingY - 1) / ydivs) + 1; j++)
    {
        if (sm[bgmp][j] == 200)
            SetColor(0xc, 0, 1);
        else if (sm[bgmp][j] == 100)
        {
            if ((mapmode == CFlag || mapmode == CPoints) || (mapmode == Pubg && fvf))
                SetColor(cls[Inteam[id] % clsNum], 0, 1);
            else
                SetColor(cls[id % clsNum], 0, 1);
        }
        else if (sm[bgmp][j] == 19)
            SetColor(cls[id % clsNum], 0, 1);
        else if (sm[bgmp][j] == -2)
            Setcolor();
        else if (sm[bgmp][j] == -1)
            SetColor(0xd, 0xd, 2);
        else
            SetColor(cls[sm[bgmp][j]], 0, 1);
        printf("%c ", miniMap[bgmp][j]);
        Setcolor();
    }
    return;
}
bool isCleared;
void putmap(int sx, int sy, int id)
{
    int bgmp = 0;
    bool clearing = false;
    if (miniMapLevel == 9)
    {
        startingX = startingY = 1;
        endingX = X;
        endingY = Y;
    }
    else
    {
        startingX = max(1, sx - miniMapLevel * 5);
        endingX = min(X, sx + miniMapLevel * 5);
        startingY = max(1, sy - miniMapLevel * 5);
        endingY = min(Y, sy + miniMapLevel * 5);
    }
    xdivs = double(endingX - startingX + 1) / 20.0;
    ydivs = double(endingY - startingY + 1) / 20.0;
    if ((X > 15 || Y > 15) && isMiniMap)
    {
        isCleared = false;
        fill(sm[0], sm[0] + 105 * 105, -2);
        for (int i = int(double(startingX - 1) / xdivs) + 1; i <= int(double(endingX - 1) / xdivs) + 1; i++)
            for (int j = int(double(startingY - 1) / ydivs) + 1; j <= int(double(endingY - 1) / ydivs) + 1; j++)
                miniMap[i][j] = (opt ? ' ' : 'O');
        for (int i = startingX; i <= endingX; i++)
        {
            for (int j = startingY; j <= endingY; j++)
            {
                char &currentBlock = miniMap[int(double(i - 1) / xdivs) + 1][int(double(j - 1) / ydivs) + 1];
                int &currentSM = sm[int(double(i - 1) / xdivs) + 1][int(double(j - 1) / ydivs) + 1];
                if (iskt[i][j] && iskt[i - 1][j - 1] && iskt[i + 1][j + 1])
                {
                    currentBlock = '!';
                    if (ktremaintime % 2 == 0)
                        currentSM = 20;
                    else
                    {
                        currentSM = -2;
                    }
                }
                if (mp[i][j].type == Points || mp[i][j].type == Flag)
                {
                    if (currentBlock != '!')
                    {
                        if (mp[i][j].belong)
                            currentSM = mp[i][j].belong % clsNum;
                        currentBlock = '+';
                    }
                }
                if (mp[i][j].type == General && ifgetflag[mp[i][j].belong])
                {
                    if (currentBlock != '!')
                    {
                        currentSM = ifgetflag[mp[i][j].belong] % clsNum;
                        currentBlock = '+';
                    }
                }
                if (mp[i][j].type == General && mp[i][j].belong == id)
                {
                    if (currentBlock != '!' && currentBlock != '+')
                    {
                        currentBlock = 'X';
                        // if ((mapmode == CFlag || mapmode == CPoints) || (mapmode == Pubg && fvf))
                        //     currentSM = Inteam[mp[i][j].belong] % clsNum;
                        // else
                        //     currentSM = mp[i][j].belong % clsNum;
                        currentSM = max(currentSM, 100);
                    }
                }
                if (mp[i][j].belong == id || ifteam[Inteam[id]].find(mp[i][j].belong) != ifteam[Inteam[id]].end() || isreplay == 2 && mp[i][j].belong != 0)
                {
                    if (currentBlock != '!' && currentBlock != '+' && currentBlock != 'X')
                    {
                        if ((mapmode != Pubg && mapmode != CFlag && mapmode != CPoints && mp[i][j].type == General && gennum <= 8) || isreplay == 2 && mp[i][j].type == General)
                            currentBlock = 'X';
                        else
                            currentBlock = 'O';
                        if (mp[i][j].belong == id)
                            currentSM = max(currentSM, 100);
                        if ((mapmode == CFlag || mapmode == CPoints) || (mapmode == Pubg && fvf))
                            currentSM = max(currentSM, Inteam[mp[i][j].belong] % clsNum);
                        else
                            currentSM = max(currentSM, mp[i][j].belong % clsNum);
                    }
                }
                if (i == sx && j == sy && sx != 0 && sy != 0)
                {
                    if (currentBlock != '!' && currentBlock != '+' && currentBlock != 'X')
                    {
                        currentBlock = 'O';
                        currentSM = max(currentSM, 200);
                    }
                }
                if (fog[i][j])
                {
                    if (currentBlock != '!' && currentBlock != '+' && currentBlock != 'X' && !(currentBlock == 'O' && currentSM != -2))
                    {
                        currentBlock = 'F';
                        currentSM = -1;
                    }
                }
            }
        }
    }
    else if (!isCleared && !isMiniMap)
    {
        isCleared = true;
        clearing = true;
        fill(sm[0], sm[0] + 105 * 105, -2);
        for (int i = int(double(startingX - 1) / xdivs) + 1; i <= int(double(endingX - 1) / xdivs) + 1; i++)
            for (int j = int(double(startingY - 1) / ydivs) + 1; j <= int(double(endingY - 1) / ydivs) + 1; j++)
                miniMap[i][j] = ' ';
    }
    gotoxy(0, 0);
    memset(score, 0, sizeof(score));
    if (!sight[id][sx][sy])
        sx = sy = 0;
    for (int i = 1; i <= gennum; i++)
        score[i].id = i;
    bool lft = false;
    bool lineprinted, colprinted = false;
    for (int i = (starting ? 1 : (X > 15 ? sx - 7 : 1)); i <= (starting ? 15 : (X > 15 ? sx + 7 : X)); i++)
    {
        lineprinted = false;
        if (i >= 1 && i <= X && !colprinted)
        {
            if (!opt)
            {
                lft = false;
                for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
                    if (j >= 1 && j <= Y)
                    {
                        if (!lft)
                            printf("-"), lft = true;
                        printf("-----");
                    }
                    else
                    {
                        if (!lft)
                            printf(" "), lft = true;
                        printf("     ");
                    }
                if ((X > 15 || Y > 15) && isMiniMap || !isMiniMap && clearing)
                {
                    printf("   ");
                    bgmp++;
                    if (bgmp <= (int(double(endingX - 1) / xdivs) + 1) - (int(double(startingX - 1) / xdivs) + 1) + 1)
                    {
                        printCurrentMiniMap(bgmp + (int(double(startingX - 1) / xdivs) + 1) - 1, id);
                    }
                }
                printf("\n");
            }
            else
            {
                lft = false;
                for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
                    if (j >= 1 && j <= Y)
                    {
                        if (!lft)
                            printf(" "), lft = true;
                        printf("     ");
                    }
                    else
                    {
                        if (!lft)
                            printf(" "), lft = true;
                        printf("     ");
                    }
                if ((X > 15 || Y > 15) && isMiniMap || !isMiniMap && clearing)
                {
                    printf("   ");
                    bgmp++;
                    if (bgmp <= (int(double(endingX - 1) / xdivs) + 1) - (int(double(startingX - 1) / xdivs) + 1) + 1)
                    {
                        printCurrentMiniMap(bgmp + (int(double(startingX - 1) / xdivs) + 1) - 1, id);
                    }
                }
                printf("\n");
            }
            colprinted = true;
        }
        for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
        {
            if (!lineprinted && (i < 1 || i > X))
                printf(" "), lineprinted = true;
            if (i < 1 || i > X || j < 1 || j > Y)
            {
                printf("     ");
                continue;
            }
            if (!opt)
            {
                if (!lineprinted && i >= 1 && i <= X)
                    printf("|"), lineprinted = true;
            }
            else
            {
                if (!lineprinted && i >= 1 && i <= X)
                    printf(" "), lineprinted = true;
            }
            if (fog[i][j])
                SetColor(0xd, 0xd, 2);
            if (isGrenade[i][j])
            {
                if ((mapmode == 7 || mapmode == 6) && mode == Tdm || mapmode == Pubg && fvf)
                    SetColor(cls[Inteam[isGrenade[i][j]] % clsNum], 0, 100);
                else
                {
                    SetColor(cls[isGrenade[i][j] % clsNum], 0, 100);
                }
                printf("<G> ");
                Setcolor();
            }
            else
            {
                if (sx == i && sy == j && !(mapmode == Pubg || mapmode == CFlag || mapmode == CPoints))
                {
                    SetColor(0xc, 0, 1);
                    if (mp[i][j].type == Empty_land)
                    {
                        if (iskt[i][j])
                            printf("▒▒▒▒");
                        else if (fog[i][j])
                            printf("████");
                        else
                            printf("    ");
                    }
                    else if (mp[i][j].type == Wall)
                        printf("####");
                    else if (mp[i][j].type == General)
                    {
                        if (ifgetflag[mp[i][j].belong])
                        {
                            printf("<");
                            getnum(mp[i][j].tmp);
                            printf(">");
                        }
                        else
                        {
                            if ((mapmode == 7 || mapmode == 6) && mode == Tdm || mapmode == Pubg && fvf)
                                SetColor(cls[Inteam[mp[i][j].belong] % clsNum], 0, 100);
                            printf("{");
                            getnum(mp[i][j].tmp);
                            printf("}");
                        }
                    }
                    else if (mp[i][j].type == Land)
                    {
                        printf(" ");
                        getnum(mp[i][j].tmp);
                        printf(" ");
                    }
                    else if (mp[i][j].type == Empty_city)
                    {
                        printf("[");
                        getnum(mp[i][j].tmp);
                        printf("]");
                    }
                    else if (mp[i][j].type == 5)
                    {
                        printf("[");
                        getnum(mp[i][j].tmp);
                        printf("]");
                    }
                    else if (mp[i][j].type == 6)
                        printf(" +  ");
                    else if (mp[i][j].type == 8)
                    {
                        printf("<--l");
                    }
                    else if (mp[i][j].type == 7)
                    {
                        printf("[O] ");
                    }
                    else if (mp[i][j].type == 9)
                        printf("( %d)", mp[i][j].belong);
                    else if (mp[i][j].type == 10)
                        printf("(   )");
                    else if (mp[i][j].type == 11)
                        printf("{+} ");
                    else if (mp[i][j].type == 12)
                        printf("{O} ");
                    else if (mp[i][j].type == 13)
                        printf("<==I");
                    else if (mp[i][j].type == 14)
                        printf("[L] ");
                    else if (mp[i][j].type == 15)
                        printf("[C] ");
                    else if (mp[i][j].type == 16)
                        printf("{C} ");
                    else if (mp[i][j].type == 17)
                        printf("{2X}");
                    else if (mp[i][j].type == 18)
                        printf("[F]");
                    else if (mp[i][j].type == 19)
                        printf("{F}");
                    Setcolor();
                }
                else
                {
                    if (mp[i][j].type == Empty_land)
                    {
                        if (iskt[i][j])
                            printf("▒▒▒▒");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                        {
                            if (isHaveSend[id] && sight[id][i][j])
                            {
                                SetColor(6, 0, 20000);
                                printf("████");
                                Setcolor();
                            }
                            else
                                printf("    ");
                        }
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == Wall)
                    {
                        if (sight[id][i][j])
                            printf("####");
                        else
                            printf("????");
                    }
                    else if (mp[i][j].type == General)
                    {
                        if (ifgetflag[mp[i][j].belong])
                        {
                            SetColor(cls[Inteam[mp[i][j].belong] % clsNum], 0, 100);
                            printf("<");
                            getnum(mp[i][j].tmp);
                            printf(">");
                            Setcolor();
                        }
                        else if (sight[id][i][j])
                        {
                            SetColor(cls[mp[i][j].belong % clsNum], 0, 1);
                            if ((mapmode == CFlag || mapmode == CPoints) && mode == Tdm || mapmode == Pubg && fvf)
                                SetColor(cls[Inteam[mp[i][j].belong] % clsNum], 0, 100);
                            printf("{");
                            getnum(mp[i][j].tmp);
                            printf("}");
                            Setcolor();
                        }
                        else
                        {
                            if (fog[i][j])
                                printf("████");
                            else if (sight[id][i][j] || opt)
                                printf("    ");
                            else
                            {
                                SetColor(15, 0, 1);
                                printf("████");
                                Setcolor();
                            }
                        }
                    }
                    else if (mp[i][j].type == Land)
                    {
                        if (sight[id][i][j])
                        {
                            SetColor(cls[mp[i][j].belong % clsNum], 0, 1);
                            printf(" ");
                            getnum(mp[i][j].tmp);
                            printf(" ");
                            Setcolor();
                        }
                        else
                        {
                            if (fog[i][j])
                                printf("████");
                            else if (sight[id][i][j] || opt)
                                printf("    ");
                            else
                            {
                                SetColor(15, 0, 1);
                                printf("████");
                                Setcolor();
                            }
                        }
                    }
                    else if (mp[i][j].type == Empty_city)
                    {
                        if (sight[id][i][j])
                        {
                            printf("[");
                            getnum(mp[i][j].tmp);
                            printf("]");
                        }
                        else
                            printf("????");
                    }
                    else if (mp[i][j].type == 5)
                    {
                        if (sight[id][i][j])
                        {
                            SetColor(cls[mp[i][j].belong % clsNum], 0, 1);
                            printf("[");
                            getnum(mp[i][j].tmp);
                            printf("]");
                            Setcolor();
                        }
                        else
                            printf("????");
                    }
                    else if (mp[i][j].type == 6)
                    {
                        if (sight[id][i][j])
                            printf(" +  ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 8)
                    {
                        if (sight[id][i][j])
                            printf("<--l");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 7)
                    {
                        if (sight[id][i][j])
                            printf("[O] ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 9)
                        printf("( %d)", mp[i][j].belong);
                    else if (mp[i][j].type == 10)
                        printf("(  )");
                    else if (mp[i][j].type == 11)
                    {
                        if (sight[id][i][j])
                            printf("{+} ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 12)
                    {
                        if (sight[id][i][j])
                            printf("{O} ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 13)
                    {
                        if (sight[id][i][j])
                            printf("<==I");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 14)
                    {
                        if (sight[id][i][j])
                            printf("[L] ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 15)
                    {
                        if (sight[id][i][j])
                            printf("[C] ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 16)
                    {
                        if (sight[id][i][j])
                            printf("{C} ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 17)
                    {
                        if (sight[id][i][j])
                            printf("{2X}");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 18)
                    {
                        if (sight[id][i][j])
                            printf("[F] ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 19)
                    {
                        if (sight[id][i][j])
                            printf("{F} ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == 20)
                    {
                        if (mp[i][j].belong)
                            SetColor(cls[mp[i][j].belong % clsNum], 0, 100);
                        printf("[");
                        getnum(mp[i][j].tmp);
                        printf("]");
                        if (mp[i][j].belong)
                            Setcolor();
                    }
                    else if (mp[i][j].type == Send)
                    {
                        if (sight[id][i][j])
                            printf("<S> ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                    else if (mp[i][j].type == Grrenade)
                    {
                        if (sight[id][i][j])
                            printf("<G> ");
                        else if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j] || opt)
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                }
            }
            if (fog[i][j])
                Setcolor();
            if (!opt)
                printf("|");
            else
            {
                printf(" ");
            }
        }
        if ((X > 15 || Y > 15) && isMiniMap || !isMiniMap && clearing)
        {
            printf("   ");
            bgmp++;
            if (bgmp <= (int(double(endingX - 1) / xdivs) + 1) - (int(double(startingX - 1) / xdivs) + 1) + 1)
            {
                printCurrentMiniMap(bgmp + (int(double(startingX - 1) / xdivs) + 1) - 1, id);
            }
        }
        printf("\n");
        if (!opt)
        {
            lft = false;
            for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
                if (i >= 1 && i <= X && j >= 1 && j <= Y)
                {
                    if (!lft)
                        printf("-"), lft = true;
                    printf("-----");
                }
                else
                {
                    if (!lft)
                        printf(" "), lft = true;
                    printf("     ");
                }
            if ((X > 15 || Y > 15) && isMiniMap || !isMiniMap && clearing)
            {
                printf("   ");
                bgmp++;
                if (bgmp <= (int(double(endingX - 1) / xdivs) + 1) - (int(double(startingX - 1) / xdivs) + 1) + 1)
                {
                    printCurrentMiniMap(bgmp + (int(double(startingX - 1) / xdivs) + 1) - 1, id);
                }
                Setcolor();
            }
            printf("\n");
        }
        else
        {
            lft = false;
            for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
                if (i >= 1 && i <= X && j >= 1 && j <= Y)
                {
                    if (!lft)
                        printf(" "), lft = true;
                    printf("     ");
                }
                else
                {
                    if (!lft)
                        printf(" "), lft = true;
                    printf("     ");
                }
            if ((X > 15 || Y > 15) && isMiniMap || !isMiniMap && clearing)
            {
                printf("   ");
                bgmp++;
                if (bgmp <= (int(double(endingX - 1) / xdivs) + 1) - (int(double(startingX - 1) / xdivs) + 1) + 1)
                {
                    printCurrentMiniMap(bgmp + (int(double(startingX - 1) / xdivs) + 1) - 1, id);
                }
                Setcolor();
            }
            printf("\n");
        }
    }
    if (starting || isreplay == 2)
        return;
    gotoxy(0, 2 * ((starting ? 15 : (X > 15 ? min(sx + 7, X) : X)) - (starting ? 1 : (X > 15 ? sx - 7 : 1)) + 1) + 1);
    for (int i = 1; i <= 9; i++)
        printf("                                                                                                    \n");
    gotoxy(0, 2 * ((starting ? 15 : (X > 15 ? min(sx + 7, X) : X)) - (starting ? 1 : (X > 15 ? sx - 7 : 1)) + 1) + 1);
    if (mapmode != 7)
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].type == General || mp[i][j].type == Land || mp[i][j].type == 5)
                {
                    score[mp[i][j].belong].sco += mp[i][j].tmp;
                    score[mp[i][j].belong].lnd++;
                }
                else
                    ;
    else
    {
        for (int i = 1; i <= teamnum; i++)
            score[i].id = i, score[i].sco = teampointsmatchscore[i], score[i].lnd = teampointsmatchland[i];
    }
    if (fvf && !(mapmode == 7 || mapmode == 6 || mapmode == 5))
    {
        sort(score + 1, score + gennum + 1, cmpsco);
        bool visplayer[105];
        memset(visplayer, 0, sizeof(visplayer));
        for (int i = sx - 7; i <= sx + 7; i++)
            for (int j = sy - 7; j <= sy + 7; j++)
                if (i >= 1 && i <= X && j >= 1 && j <= Y && mp[i][j].belong != 0 && mp[i][j].tmp > 0 && sight[id][i][j])
                    visplayer[mp[i][j].belong] = true;
        for (int i = 1, j = 1; i <= gennum && j <= 5; i++)
            if (visplayer[score[i].id])
            {
                SetColor(cls[score[i].id % clsNum], 0, 1);
                printf("player%d    team%d\n", score[i].id, Inteam[score[i].id]);
                Setcolor();
                j++;
            }
    }
    if (mapmode != 6)
    {
        if (!fvf)
            sort(score + 1, score + gennum + 1, cmpsco);
        if (mapmode == 7)
        {
            for (int i = 1; i <= teamnum; i++)
            {
                SetColor(cls[score[i].id % clsNum], 0, 1);
                printf("team%d    %d %d\n", score[i].id, score[i].sco, score[i].lnd);
                Setcolor();
            }
        }
        else
        {
            int team1 = 0, team2 = 0;
            for (int i = 1; i <= gennum; i++)
            {
                if (!fvf)
                {
                    SetColor(cls[score[i].id % clsNum], 0, 1);
                    printf("player%d    %d %d", score[i].id, score[i].sco, score[i].lnd);
                    if (mode == 2)
                        printf("    team%d", Inteam[score[i].id]);
                    printf("\n");
                    Setcolor();
                }
                else
                {
                    if (score[i].sco > 0 && Inteam[score[i].id] == 1)
                        team1++;
                    else if (score[i].sco > 0 && Inteam[score[i].id] == 2)
                        team2++;
                }
            }
            if (fvf)
            {
                SetColor(cls[1], 0, 1);
                printf("team1    %d\n", team1);
                Setcolor();
                SetColor(cls[2], 0, 1);
                printf("team2    %d\n", team2);
                Setcolor();
            }
        }
    }
    else
    {
        for (int i = 1; i <= teamnum; i++)
            nflagscore[i].pos = i, nflagscore[i].score = flagscore[i];
        sort(nflagscore + 1, nflagscore + teamnum + 1, cmpteam);
        for (int i = 1; i <= teamnum; i++)
        {
            SetColor(cls[nflagscore[i].pos % clsNum], 0, 1);
            printf("team%d    %d\n", nflagscore[i].pos, nflagscore[i].score);
            Setcolor();
        }
    }
    if (mapmode == 5 || mapmode == 6 || mapmode == 7)
    {
        if (mapmode == 5)
            printf("毒圈还有 %d 回合扩散\n", rm);
        printf("当前玩家拥有的物品：剑%d， 护盾%d， 防毒面具%d， 手雷%d\n", playeratk[id], playerac[id], playerfh[id], playerGrenade[id]);
        if (ktremaintime > 0)
            printf("空投还有 %d 回合落地\n", ktremaintime);
        if (mapmode == 6 || mapmode == 7)
        {
            if (wd[id] != -1)
                printf("无敌时间还剩 %d 回合\n", wd[id]);
            displaynews();
        }
    }
    if (isPaint)
        printf("涂色模式还剩 %d 回合\n", paintRemainTime);
    if (isgz)
        printf("您已阵亡，观战中……\n");
    if (isHaveSend[id])
        printf("当前玩家持有闪现技能\n");
    return;
}
struct Flg
{
    int sx, sy, belong;
    void conv()
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != Empty_land)
                continue;
            mp[px][py].type = Flag;
            mp[px][py].belong = belong;
            sx = px;
            sy = py;
            break;
        }
        return;
    }
} flg[105];
struct Player
{
    int selectedx, selectedy, playerid;
    bool halfselect, isbot;
    int sx, sy;
    int inteam;
    bool flag[105][105];
    queue<pair<int, int> > q;
    void botit()
    {
        sx = selectedx, sy = selectedy;
        q.push(make_pair(sx, sy));
        memset(flag, 0, sizeof(flag));
        flag[sx][sy] = true;
        return;
    }
    void respawn()
    {
        playerac[playerid] = playeratk[playerid] = playerfh[playerid] = playerGrenade[playerid] = 0;
        playermaxhp[playerid] = 100;
        ishavets[playerid] = false;
        isHaveSend[playerid] = false;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
            {
                if (mp[i][j].type == General && mp[i][j].belong == playerid)
                {
                    mp[i][j].tozero();
                    if (ifgetflag[playerid])
                    {
                        news[newsr].a = flg[ifgetflag[playerid]].belong;
                        news[newsr].opt = 2;
                        news[newsr].remtime = 50;
                        newsr++;
                        mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].type = Flag, mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].belong = flg[ifgetflag[playerid]].belong;
                        ifgetflag[playerid] = 0;
                    }
                    int px, py;
                    while (1)
                    {
                        px = randnum(1, X), py = randnum(1, Y);
                        if (mp[px][py].type != Empty_land || (px == i && py == j))
                            continue;
                        mp[px][py].type = General;
                        mp[px][py].belong = playerid;
                        mp[px][py].tmp = 100;
                        selectedx = px;
                        selectedy = py;
                        if (isbot)
                            botit();
                        break;
                    }
                    i = X + 1;
                    wd[playerid] = 10;
                    break;
                }
            }
        return;
    }
    void kil(int pid)
    {
        if (isPaint)
            return;
        if (mode != 2)
        {
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].belong == pid)
                        mp[i][j].belong = playerid;
            alivegennum--;
            return;
        }
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == pid && mp[i][j].type == General)
                    return;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == pid)
                    mp[i][j].belong = playerid;
        alivegennum--;
        return;
    }
    void updMovement(node *dp, node *dt, int *opt, int tmp, bool iskt)
    {
        if (mode == 2 && (dt->type == General || dt->type == Land || dt->type == 5) && dt->belong != playerid && ifteam[Inteam[playerid]].find(dt->belong) != ifteam[Inteam[playerid]].end())
            return;
        if (mapmode == 5 && iskt)
            return;
        if (dt->type == 20)
            return;
        int moved = (dp->tmp) - 1;
        if (mapmode != 5 && mapmode != 6 && mapmode != 7)
        {
            if (halfselect)
                moved >>= 1, halfselect = false;
            if (dt->type == Empty_city && dt->tmp >= moved)
                return;
            dp->tmp -= moved;
            if (dt->belong == playerid)
                dt->tmp += moved;
            else
            {
                if (moved > dt->tmp)
                {
                    dt->tmp = moved - dt->tmp;
                    if (dt->type == General)
                        dt->type = City, kil(dt->belong);
                    else
                    {
                        dt->belong = playerid;
                        if (dt->type == Empty_land)
                            dt->type = Land;
                        if (dt->type == Empty_city)
                            dt->type = City;
                    }
                }
                else
                    dt->tmp -= moved;
            }
            *opt += tmp;
        }
        else
        {
            if (dt->type == General)
            {
                int dmg1 = max(0, int(double(dp->tmp) * (1.0 + double(playeratk[playerid]) / 10.0) - double(playerac[dt->belong]) * 10.0));
                int dmg2 = max(0, int(double(dt->tmp) * (1.0 + double(playeratk[dt->belong]) / 10.0) - double(playerac[playerid]) * 10.0));
                dp->tmp -= dmg2;
                dt->tmp -= dmg1;
                if (dp->tmp <= 0 && mapmode != 6 && mapmode != 7)
                    dp->type = Empty_land, dp->tmp = 0, dp->belong = 0, alivegennum--, playeratk[dt->belong] += playeratk[dp->belong], playerac[dt->belong] += playerac[dp->belong], ishavets[dt->belong] |= ishavets[dp->belong], playerfh[dt->belong] += playerfh[dp->belong], playerGrenade[dt->belong] += playerGrenade[dp->belong];
                if (dt->tmp <= 0 && mapmode != 6 && mapmode != 7)
                    dt->type = Empty_land, dt->tmp = 0, dt->belong = 0, alivegennum--, playeratk[dp->belong] += playeratk[dt->belong], playerac[dp->belong] += playerac[dt->belong], ishavets[dp->belong] |= ishavets[dt->belong], playerfh[dp->belong] += playerfh[dt->belong], playerGrenade[dp->belong] += playerGrenade[dt->belong];
            }
            else
            {
                if (dt->type == 6)
                    dp->tmp += 10, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 7)
                    playerac[dp->belong]++, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 8)
                    playeratk[dp->belong]++, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 11)
                    dp->tmp += 30, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 12)
                    playerac[dp->belong] += 3, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 13)
                    playeratk[dp->belong] += 3, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 14)
                    blindtimeremain[dp->belong] = 10, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 15)
                    playermaxhp[dp->belong] += 10, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 16)
                    playermaxhp[dp->belong] += 30, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 17)
                    ishavets[dp->belong] = true, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 18)
                    playerfh[dp->belong]++, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 19)
                    playerfh[dp->belong] += 3, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == Send)
                    isHaveSend[dp->belong] = true, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == Grrenade)
                    playerGrenade[dp->belong]++, dt->type = Empty_land, aliveobjectnum--;
                if (dt->type == 9)
                {
                    if (dt->belong != Inteam[playerid] && !ifgetflag[playerid])
                    {
                        ifgetflag[playerid] = dt->belong;
                        dt->type = Empty_flag;
                        news[newsr].a = playerid;
                        news[newsr].b = dt->belong;
                        news[newsr].opt = 1;
                        news[newsr].remtime = 50;
                        newsr++;
                    }
                    else if (dt->belong == Inteam[playerid] && selectedx - 1 == flg[dt->belong].sx && selectedy == flg[dt->belong].sy && ifgetflag[playerid])
                    {
                        mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].type = Flag, mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].belong = flg[ifgetflag[playerid]].belong;
                        flagscore[Inteam[playerid]]++;
                        news[newsr].a = playerid;
                        news[newsr].opt = 3;
                        news[newsr].remtime = 50;
                        newsr++;
                        ifgetflag[playerid] = 0;
                    }
                    return;
                }
                else if (dt->type == 10)
                    return;
                swap(*dt, *dp);
            }
            *opt += tmp;
        }
        return;
    }
    void moveup()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx - 1][selectedy].type != Wall && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx - 1][selectedy];
            updMovement(dp, dt, &selectedx, -1, iskt[selectedx - 1][selectedy]);
        }
        return;
    }
    void movedown()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx + 1][selectedy].type != Wall && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx + 1][selectedy];
            updMovement(dp, dt, &selectedx, 1, iskt[selectedx + 1][selectedy]);
        }
        return;
    }
    void moveleft()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx][selectedy - 1].type != Wall && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx][selectedy - 1];
            updMovement(dp, dt, &selectedy, -1, iskt[selectedx][selectedy - 1]);
        }
        return;
    }
    void moveright()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx][selectedy + 1].type != Wall && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx][selectedy + 1];
            updMovement(dp, dt, &selectedy, 1, iskt[selectedx][selectedy + 1]);
        }
        return;
    }
    void playermove(char playeraction)
    {
        if (playeraction == 'W' && selectedx > 1)
            moveup();
        else if (playeraction == 'A' && selectedy > 1)
            moveleft();
        else if (playeraction == 'S' && selectedx < X)
            movedown();
        else if (playeraction == 'D' && selectedy < Y)
            moveright();
        return;
    }
    bool isOutside(int x, int y)
    {
        for (int i = 0; i < 4; i++)
        {
            int px = x + dir[i][0], py = y + dir[i][1];
            if (px >= 1 && px <= X && py >= 1 && py <= Y && mp[px][py].belong != playerid)
                return true;
        }
        return false;
    }
    void changetarget()
    {
        int insideAnsTmp = mp[sx][sy].tmp, insideAnsX = sy, insideAnsY = sy;
        int outsideAnsTmp = 0, outsideAnsX = 0, outsideAnsY = 0;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == playerid)
                {
                    if (isOutside(i, j))
                    {
                        if (mp[i][j].tmp > outsideAnsTmp)
                        {
                            outsideAnsTmp = mp[i][j].tmp;
                            outsideAnsX = i;
                            outsideAnsY = j;
                        }
                    }
                    else
                    {
                        if (mp[i][j].tmp > insideAnsTmp)
                        {
                            insideAnsTmp = mp[i][j].tmp;
                            insideAnsX = i;
                            insideAnsY = j;
                        }
                    }
                }
        if (outsideAnsTmp * 5 >= insideAnsTmp) //优先选外面的
        {
            sx = outsideAnsX;
            sy = outsideAnsY;
        }
        else
        {
            sx = insideAnsX;
            sy = insideAnsY;
        }
        selectedx = sx;
        selectedy = sy;
        q.push(make_pair(sx, sy));
        if (rand() % 2 == 0 || !fog[sx][sy])
            memset(flag, 0, sizeof(flag));
        flag[sx][sy] = true;
        return;
    }
    void botmove()
    {
        int x = 0, y = 0, tryTime = 0;
        do
        {
            if (q.empty())
            {
                changetarget();
            }
            x = q.front().first, y = q.front().second;
            tryTime++;
            q.pop();
        } while (mp[x][y].tmp <= 1 && mp[x][y].type != General && tryTime <= 10);
        if (tryTime > 10)
            return;
        if (isGMode)
            playerGrenade[playerid] = 1;
        if (mp[x][y].tmp <= 1)
            return;
        int tmp = (isGMode ? 10 : 7);
        if (playerGrenade[playerid] > 0)
        {
            for (int i = x - tmp; i <= x + tmp; i++)
                for (int j = y - tmp; j <= y + tmp; j++)
                    if (i >= 1 && i <= X && j >= 1 && j <= Y && sight[playerid][i][j] && mp[i][j].type == General && (mode == Ffa || mode == Tdm && Inteam[mp[i][j].belong] != Inteam[playerid]) && mp[i][j].belong != playerid)
                    {
                        if (playerGrenade[playerid] > 0)
                        {
                            playerGrenade[playerid]--;
                            addGrenade(x, y, i, j, 20, playerid);
                        }
                        else
                        {
                            i = x + 10;
                            break;
                        }
                    }
        }
        int ansTmp = 0, ansI = -1;
        vector<int> tmpI;
        tmpI.push_back(0);
        tmpI.push_back(1);
        tmpI.push_back(2);
        tmpI.push_back(3);
        random_shuffle(tmpI.begin(), tmpI.end());
        for (vector<int>::iterator it = tmpI.begin(); it != tmpI.end();)
        {
            int i = *it;
            it = tmpI.erase(it);
            int px = x + dir[i][0], py = y + dir[i][1];
            if (px >= 1 && px <= X && py >= 1 && py <= Y && mp[px][py].type != Wall && mp[x][y].tmp > 1 && !flag[px][py] && !fog[px][py] && mp[px][py].type != 20 && (mp[px][py].type != Empty_city || mp[px][py].type == Empty_city && mp[x][y].tmp > mp[px][py].tmp))
            {
                // break;
                int currentTmp; //攻击优先级
                if (mp[px][py].belong != playerid)
                {
                    if (mp[px][py].type == General)
                        currentTmp = 10;
                    else if (mp[px][py].type == City)
                        currentTmp = 8;
                    else if (mp[px][py].type == Land)
                        currentTmp = 5;
                    else
                        currentTmp = 3;
                }
                else
                    currentTmp = 1;
                if (currentTmp > ansTmp)
                {
                    ansTmp = currentTmp;
                    ansI = i;
                }
            }
        }
        if (ansI == -1)
            return;
        int px = x + dir[ansI][0], py = y + dir[ansI][1];
        flag[px][py] = true;
        q.push(make_pair(px, py));
        if (ansI == 0)
            moveup();
        else if (ansI == 1)
            moveright();
        else if (ansI == 2)
            movedown();
        else
            moveleft();
        return;
    }
} player[105];
struct Team
{
    Player *members[105];
    int membernum, teamid;
    void tozero()
    {
        membernum = teamid = 0;
        return;
    }
    void move(char playeraction)
    {
        for (int i = 1; i <= membernum; i++)
        {
            if (members[i]->isbot)
            {
                members[i]->botmove();
            }
            else
                members[i]->playermove(playeraction);
        }
        return;
    }
} team[105];
void teaming()
{
    for (int i = 1; i <= teamnum; i++)
    {
        team[i].tozero();
        team[i].teamid = i;
    }
    for (int i = 1; i <= gennum; i++)
    {
        player[i].inteam = (i + (gennum / teamnum) - 1) / (gennum / teamnum);
        ifteam[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].insert(i);
        team[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].membernum++;
        Inteam[i] = (i + (gennum / teamnum) - 1) / (gennum / teamnum);
        team[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].members[team[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].membernum] = &player[i];
    }
    return;
}
land_type getRadomObjects(const vector<land_type> &objs)
{
    int sz = objs.size();
    return objs[randnum(0, sz - 1)];
}
void addObject(land_type tp, vector<land_type> &objs)
{
    objs.push_back(tp);
    return;
}
void convobject()
{
    while (aliveobjectnum < objectnum)
    {
        int px, py, trytime = 0;
        if (mapmode == 7)
        {
            while (1)
            {
                px = randnum(1, X), py = randnum(1, Y);
                trytime++;
                if (mp[px][py].type == 20 || trytime > 100)
                    break;
            }
            if (trytime > 100)
                continue;
            int gx, gy;
            while (1)
            {
                gx = randnum(px - 3, px + 3);
                gy = randnum(py - 3, py + 3);
                trytime++;
                if ((gx >= 1 && gx <= X && gy >= 1 && gy <= Y && mp[gx][gy].type == Empty_land) || trytime > 100)
                    break;
            }
            if (trytime > 100)
                continue;
            while (1)
            {
                mp[gx][gy].type = getRadomObjects(normalobjects);
                if ((mp[gx][gy].type == 18 || mp[gx][gy].type == 19) && (mapmode == 6 || mapmode == 7))
                    continue;
                break;
            }
            aliveobjectnum++;
            continue;
        }
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            trytime++;
            if (mp[px][py].type == Empty_land || trytime > 100)
                break;
        }
        if (trytime > 100)
            continue;
        while (1)
        {
            mp[px][py].type = getRadomObjects(normalobjects);
            if ((mp[px][py].type == 18 || mp[px][py].type == 19) && (mapmode == 6 || mapmode == 7))
                continue;
            break;
        }
        aliveobjectnum++;
    }
    return;
}
void spawnkt()
{
    for (int trytime = 0; trytime < 100; trytime++)
    {
        int i = randnum(2, X - 1), j = randnum(2, Y - 1);
        bool flag = true;
        for (int k = i - 1; k <= i + 1; k++)
            for (int w = j - 1; w <= j + 1; w++)
                if (mp[k][w].type != Empty_land)
                {
                    flag = false;
                    k = i + 2;
                    break;
                }
        if (flag)
        {
            ktx = i;
            kty = j;
            ktremaintime = 10;
            for (int k = i - 1; k <= i + 1; k++)
                for (int w = j - 1; w <= j + 1; w++)
                    iskt[k][w] = true;
            break;
        }
    }
    return;
}
void getkt()
{
    for (int i = ktx - 1; i <= ktx + 1; i++)
        for (int j = kty - 1; j <= kty + 1; j++)
        {
            mp[i][j].type = getRadomObjects(objects);
            iskt[i][j] = false;
        }
    return;
}
void pubgconv()
{
    for (int i = 1; i <= gennum; i++)
        playermaxhp[i] = 100;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type == General)
                mp[i][j].tmp = 100;
    return;
}
struct pnt
{
    int sx, sy, belong, tmp;
    void tozero()
    {
        sx = sy = belong = tmp = 0;
        return;
    }
    void conv()
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != Empty_land)
                continue;
            mp[px][py].type = Points;
            mp[px][py].belong = belong;
            sx = px;
            sy = py;
            break;
        }
        return;
    }
} pnts[200];
void pointsmatchconv()
{
    for (int i = 1; i <= gennum; i++)
    {
        pnts[i].tozero();
        pnts[i].conv();
    }
    return;
}
void readconfig()
{
    ifstream infile;
    infile.open("config", ios::in);
    infile >> X >> Y >> wallpr >> citypr >> objectpr >> tpt >> gennum >> teamnum >> dq >> kttime >> pointstime >> paintTime;
    infile.close();
    return;
}
void saveconfig()
{
    ofstream outfile;
    outfile.open("config", ios::out | ios::trunc);
    outfile << X << endl
            << Y << endl
            << wallpr << endl
            << citypr << endl
            << objectpr << endl
            << tpt << endl
            << gennum << endl
            << teamnum << endl
            << dq << endl
            << kttime << endl
            << pointstime << endl
            << paintTime << endl;
    outfile.close();
    return;
}
bool isfirstsave = true;
void savemap()
{
    ofstream outfile;
    if (isfirstsave)
    {
        outfile.open("Map", ios::out | ios::trunc), isfirstsave = false;
        outfile << mode << " " << mapmode << " " << fvf << endl;
    }
    else
        outfile.open("Map", ios::out | ios::app);
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
        {
            outfile << mp[i][j].belong << " " << mp[i][j].tmp << " " << mp[i][j].type;
            if (mapmode == Pubg)
                outfile << " " << fog[i][j] << " " << iskt[i][j];
            outfile << endl;
        }
    if (mapmode == CFlag)
    {
        for (int i = 1; i <= gennum; i++)
            outfile << ifgetflag[i] << " ";
        outfile << endl;
    }
    if (mapmode == Pubg || mapmode == CFlag || mapmode == CPoints)
    {
        outfile << currentGrenade.size() << endl;
        for (vector<Grenade>::iterator it = currentGrenade.begin(); it != currentGrenade.end(); it++)
            outfile << it->frm << " " << it->cx << " " << it->cy << endl;
    }
    outfile.close();
    return;
}
void savereplay()
{
    saveconfig();
    ofstream outfile;
    outfile.open("Map", ios::out | ios::app);
    outfile << "-1 -1 -1" << endl;
    outfile.close();
    return;
}
void itObjects()
{
    addObject(Health, normalobjects);
    addObject(Ac, normalobjects);
    addObject(Sword, normalobjects);
    addObject(Light, normalobjects);
    addObject(Pill, normalobjects);
    addObject(Fh, normalobjects);
    addObject(Send, normalobjects);
    addObject(Grrenade, normalobjects);
    addObject(Health, objects);
    addObject(Ac, objects);
    addObject(Sword, objects);
    addObject(Pill, objects);
    addObject(Fh, objects);
    addObject(Send, objects);
    addObject(Exhealth, objects);
    addObject(Exac, objects);
    addObject(Exsword, objects);
    addObject(Exfh, objects);
    addObject(Twoscope, objects);
    addObject(Expill, objects);
    addObject(Grrenade, objects);
    return;
}
int miniMapOpt;
POINT p1, p2;
double dx, dy;
LONG xx1, yy1, xx2, yy2;
void mapEditor()
{
    system("cls");
    int opt = 0;
    int slx = 0, sly = 0;
    if (X > 15 || Y > 15)
    {
        slx = (X >> 1), sly = (Y >> 1);
    }
    while (1)
    {
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                sight[0][i][j] = true;
        putmap(slx, sly, 0);
        if (opt == 0)
            printf("修改类型\n");
        else if (opt == 1)
            printf("修改颜色\n");
        else if (opt == 2)
        {
            printf("修改兵力\n");
        }
        else if (opt == 3)
        {
            printf("选择\n");
        }
        if (KEY_DOWN('F'))
            opt = (opt + 1) % 4;
        if (KEY_DOWN('E'))
            break;
        if (KEY_DOWN(MOUSE_MOVED))
        {
            GetCursorPos(&p1);
            int sp = int(round((double(p1.y) - double(xx1)) / dx)) + 1 + (X > 15 ? slx - 8 : 0);
            int sq = int(round((double(p1.x) - double(yy1)) / dy)) + 1 + (Y > 15 ? sly - 8 : 0);
            if (opt == 0)
            {
                int tmp;
                printf("输入修改后的类型...");
                cin >> tmp;
                mp[sp][sq].type = (land_type)tmp;
            }
            else if (opt == 1)
            {
                int tmp;
                printf("输入修改后的颜色...");
                cin >> tmp;
                mp[sp][sq].belong = tmp;
            }
            else if (opt == 2)
            {
                int tmp;
                printf("输入修改后的兵力...");
                cin >> tmp;
                mp[sp][sq].tmp = tmp;
            }
            else if (opt == 3)
            {
                slx = sp;
                sly = sq;
            }
        }
        Sleep(300);
    }
    return;
}
int main()
{
    system("title generals");
    system("color 0f");
    system("chcp 65001");
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); //https://blog.csdn.net/gxc1222/article/details/80999234
    DWORD mod;
    GetConsoleMode(hStdin, &mod);
    mod &= ~ENABLE_QUICK_EDIT_MODE; //移除快速编辑模式
    mod &= ~ENABLE_INSERT_MODE;     //移除插入模式
    mod &= ~ENABLE_MOUSE_INPUT;
    SetConsoleMode(hStdin, mod);
    itObjects();
    srand(time(NULL));
    convmap();
    printf("即将进行鼠标校准……\n");
    system("pause");
    mp[1][15].type = Land;
    mp[1][15].belong = 1;
    mp[1][15].tmp = 9;
    player[1].selectedx = 1, player[1].selectedy = 15;
    sight[1][1][15] = true;
    putmap(player[1].selectedx, player[1].selectedy, 1);
    printf("请将鼠标移到右上角的9……\n");
    system("pause");
    GetCursorPos(&p1);
    mp[15][1].type = Land;
    mp[15][1].belong = 1;
    mp[15][1].tmp = 9;
    player[1].selectedx = 15, player[1].selectedy = 1;
    sight[1][1][15] = false;
    sight[1][15][1] = true;
    putmap(player[1].selectedx, player[1].selectedy, 1);
    printf("请将鼠标移到左下角的9……\n");
    system("pause");
    GetCursorPos(&p2);
    xx1 = p1.y, yy1 = p2.x, xx2 = p2.y, yy2 = p1.x;
    dx = double(xx2 - xx1) / 14.0;
    dy = double(yy2 - yy1) / 14.0;
    system("cls");
    starting = false;
    printf("干得好。接下来请不要移动窗口，也不要调整窗口的大小。\n");
    system("pause");
    system("cls");
    while (1)
    {
        printf("1 = 正常游戏， 2 = 播放回放\n");
        scanf("%d", &isreplay);
        if (isreplay == 1 || isreplay == 2)
            break;
    }
    if (isreplay == 1)
    {
        while (1)
        {
            printf("选择模式：1 = Free For All， 2 = Team Deathmatch，3 = 50v50\n");
            scanf("%d", &mode);
            if (mode == 1 || mode == 2 || mode == 3)
                break;
        }
        if (mode == 3)
        {
            fvf = true;
            mode = Tdm;
            X = 99;
            Y = 99;
            gennum = 100;
            teamnum = 2;
            dq = 10;
            objectpr = 0.05;
            kttime = 20;
        }
        while (1)
        {
            printf("选择地图：1 = 随机地图， 2 = 空白地图， 3 = 迷宫地图， 4 = 端午地图， 5  = 吃鸡地图， 6 = 夺旗地图， 7 = 占点地图， 8 = 涂色地图，\n9 = 堑壕地图\n");
            scanf("%d", &mapmode);
            if (mode == 1 && mapmode == 6)
            {
                printf("抱歉，夺旗地图不支持FFA模式。\n");
                continue;
            }
            if (mode == 1 && mapmode == 7)
            {
                printf("抱歉，占点地图不支持FFA模式。\n");
                continue;
            }
            if (mapmode == 1 || mapmode == 2 || mapmode == 3 || mapmode == 4 || mapmode == 5 || mapmode == 6 || mapmode == 7 || mapmode == 8 || mapmode == 9)
                break;
        }
        int ifown;
        while (1)
        {
            printf("选择配置：1 = 默认配置， 2 = 读取配置文件, 3 = 自定义配置\n警告：在使用自定义配置前，请务必仔细阅读代码中的注释。\n");
            scanf("%d", &ifown);
            if (ifown == 1 || ifown == 2 || ifown == 3)
                break;
        }
        if (ifown == 2)
        {
            readconfig();
            printf("读取配置文件成功！\n");
        }
        if (ifown == 3)
        {
            printf("请输入地图的长\n");
            scanf("%d", &X);
            printf("请输入地图的宽\n");
            scanf("%d", &Y);
            printf("请输入墙的密度\n");
            scanf("%lf", &wallpr);
            printf("请输入城市的密度\n");
            scanf("%lf", &citypr);
            printf("请输入道具的密度\n");
            scanf("%lf", &objectpr);
            printf("请输入每个回合后的等待时间\n");
            scanf("%d", &tpt);
            printf("请输入玩家的数量\n");
            scanf("%d", &gennum);
            printf("请输入队伍的数量\n");
            scanf("%d", &teamnum);
            printf("请输入毒圈的扩散时间\n");
            scanf("%d", &dq);
            printf("请输入空投的投放时间\n");
            scanf("%d", &kttime);
            printf("请输入占领一个据点所需的时间\n");
            scanf("%d", &pointstime);
            printf("请输入涂色地图的时间\n");
            scanf("%d", &paintTime);
            printf("是否保存配置文件？(1 = 是，2 = 否)\n");
            int tmp;
            scanf("%d", &tmp);
            if (tmp == 1)
            {
                saveconfig();
                printf("保存配置文件成功！\n");
            }
        }
    }
    int mapopt;
    while (1)
    {
        printf("选择地图输出模式：1 = 正常模式， 2 = 极速模式\n极速模式：不绘制地图边框和空白方块，可大幅减少刷新闪烁，但可能影响体验。\n");
        scanf("%d", &mapopt);
        if (mapopt == 1 || mapopt == 2)
            break;
    }
    if ((mapmode == Pubg || mapmode == CFlag || mapmode == CPoints) && isreplay == 1)
    {
        int pp;
        while (1)
        {
            printf("1 = 正常模式， 2 = 无限手雷模式\n");
            scanf("%d", &pp);
            if (pp == 1 || pp == 2)
                break;
        }
        isGMode = (pp == 2);
    }
    if (mapopt == 2)
        opt = true;
    if (isreplay == 1)
    {
        convmap();
        if (mapmode == 1 || mapmode == 5 || mapmode == 6 || mapmode == 7)
        {
            congen();
            if (mapmode != 5 && mapmode != 6 && mapmode != 7)
                concit();
            convwall();
        }
        else if (mapmode == 2)
            congen();
        else if (mapmode == 3)
            convmaze();
        else if (mapmode == 4)
            convdragon();
        else if (mapmode == Qianhao)
        {
            generateQianHaoMap();
            mapmode = Blank;
        }
        else if (mapmode == Paint)
        {
            isPaint = true;
            mapmode = Blank;
            congen();
        }
        if (mode == Tdm)
            teaming();
        if (mapmode == 6)
        {
            ifcanconvobject = true;
            for (int i = 1; i <= teamnum; i++)
            {
                flg[i].belong = i;
                flg[i].conv();
            }
        }
    }
    memset(sight, 0, sizeof(sight));
    int keys[5] = {'W', 'S', 'A', 'D', 'Z'};
    int turn = 1;
    int currentplayer = 1;
    objectnum = int(double(X * Y) * objectpr);
    alivegennum = gennum, aliveteamnum = teamnum;
    if (isreplay == 1)
    {
        if (X > 15 || Y > 15)
        {
            while (1)
            {
                printf("总览地图显示设置：1 = 始终显示总览地图（可能使游戏变卡）， 2 = 长按 M 键显示总览地图\n");
                scanf("%d", &miniMapOpt);
                if (miniMapOpt == 1 || miniMapOpt == 2)
                    break;
            }
        }
        for (int k = 1; k <= gennum; k++)
        {
            player[k].playerid = k, player[k].halfselect = false, player[k].isbot = false;
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].type == General && mp[i][j].belong == k)
                    {
                        player[k].selectedx = i;
                        player[k].selectedy = j;
                        i = X + 1;
                        break;
                    }
        }
        for (int i = 2; i <= gennum; i++)
        {
            player[i].isbot = true;
            player[i].botit();
        }
        if (mapmode == 5 || mapmode == 6 || mapmode == 7)
            pubgconv();
        if (mapmode == 7)
            pointsmatchconv();
        int isDiy;
        while (1)
        {
            printf("是否进入地图编辑器？(1 = 是, 0 = 否）\n");
            scanf("%d", &isDiy);
            if (isDiy == 1 || isDiy == 0)
                break;
        }
        if (isDiy)
            mapEditor();
        system("cls");
    }
    if (isreplay == 2)
    {
        readconfig();
        for (int k = 1; k <= gennum; k++)
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    sight[k][i][j] = true;
        ifstream infile;
        infile.open("Map", ios::in);
        int _mode, _mapmode;
        infile >> _mode >> _mapmode >> fvf;
        mode = (game_mode)_mode;
        mapmode = (map_mode)_mapmode;
        if (X > 15 || Y > 15)
        {
            while (1)
            {
                printf("总览地图显示设置：1 = 始终显示总览地图（可能使游戏变卡）， 2 = 长按 M 键显示总览地图\n");
                scanf("%d", &miniMapOpt);
                if (miniMapOpt == 1 || miniMapOpt == 2)
                    break;
            }
        }
        system("cls");
        if (mode == 2)
            teaming();
        while (1)
        {
            bool isend = false;
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                {
                    int tmpp;
                    infile >> mp[i][j].belong >> mp[i][j].tmp >> tmpp;
                    if (mapmode == Pubg)
                        infile >> fog[i][j] >> iskt[i][j];
                    if (tmpp == -1 || mp[i][j].belong == -1 || mp[i][j].tmp == -1)
                    {
                        isend = true;
                        i = X + 1;
                        break;
                    }
                    mp[i][j].type = (land_type)tmpp;
                }
            if (isend)
                break;
            if (mapmode == CFlag)
                for (int i = 1; i <= gennum; i++)
                    infile >> ifgetflag[i];
            if (mapmode == Pubg || mapmode == CPoints || mapmode == CFlag)
            {
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        isGrenade[i][j] = 0;
                int cnt;
                infile >> cnt;
                while (cnt--)
                {
                    int p1, p2, p3;
                    infile >> p1 >> p2 >> p3;
                    isGrenade[p2][p3] = p1;
                }
            }
            putmap(player[currentplayer].selectedx, player[currentplayer].selectedy, currentplayer);
            for (int k = 1; k <= gennum; k++)
            {
                player[k].playerid = k, player[k].halfselect = false, player[k].isbot = false;
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == General && mp[i][j].belong == k)
                        {
                            player[k].selectedx = i;
                            player[k].selectedy = j;
                            i = X + 1;
                            break;
                        }
            }
            if (KEY_DOWN('F'))
                while (1)
                {
                    currentplayer = (currentplayer == gennum ? 1 : currentplayer + 1);
                    bool flag = false;
                    for (int i = 1; i <= X; i++)
                        for (int j = 1; j <= Y; j++)
                            if (mp[i][j].type == General && mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                            {
                                flag = true;
                                i = X + 1;
                                break;
                            }
                    if (flag)
                        break;
                }
            if ((X > 15 || Y > 15) && KEY_DOWN('Q'))
            {
                if (miniMapLevel > 3)
                    miniMapLevel--;
            }
            if ((X > 15 || Y > 15) && KEY_DOWN('E'))
            {
                if (miniMapLevel < 9)
                    miniMapLevel++;
            }
            if (miniMapOpt == 1)
                isMiniMap = true;
            else if (miniMapOpt == 2)
            {
                if (KEY_DOWN('M'))
                    isMiniMap = true;
                else
                    isMiniMap = false;
            }
            Sleep(tpt);
        }
        infile.close();
        return 0;
    }
    if (isPaint)
        paintRemainTime = paintTime;
    if (mode == 1)
    {
        while (alivegennum > 1 && (!isPaint || isPaint && paintRemainTime > 0))
        {
            if (isGMode)
                playerGrenade[currentplayer] = 1;
            rm = dq - turn % dq;
            putmap(player[currentplayer].selectedx, player[currentplayer].selectedy, currentplayer);
            char inp = ' ';
            bool ismouse = true;
            if (mapmode == 5)
            {
                if (ktremaintime > 0)
                    ktremaintime--;
                if (ktremaintime == 0)
                {
                    getkt();
                    ktremaintime = -1;
                }
            }
            for (int j = 0; j < 5; j++)
                if (inp == ' ' && KEY_DOWN(keys[j]))
                    inp = keys[j];
            if (ismouse && KEY_DOWN(MOUSE_MOVED))
            {
                GetCursorPos(&p1);
                int sp = int(round((double(p1.y) - double(xx1)) / dx)) + 1 + (X > 15 ? player[currentplayer].selectedx - 8 : 0);
                int sq = int(round((double(p1.x) - double(yy1)) / dy)) + 1 + (Y > 15 ? player[currentplayer].selectedy - 8 : 0);
                player[currentplayer].selectedx = sp;
                player[currentplayer].selectedy = sq;
                ismouse = false;
                if (isHaveSend[currentplayer] && sight[currentplayer][sp][sq] && mp[sp][sq].type == Empty_land)
                {
                    for (int i = 1; i <= X; i++)
                        for (int j = 1; j <= Y; j++)
                            if (mp[i][j].type == General && mp[i][j].belong == currentplayer)
                            {
                                swap(mp[i][j], mp[sp][sq]);
                                i = X + 1;
                                isHaveSend[currentplayer] = false;
                                break;
                            }
                }
                else if (playerGrenade[currentplayer] > 0 && mp[sp][sq].type != Wall)
                {
                    for (int i = 1; i <= X; i++)
                        for (int j = 1; j <= Y; j++)
                            if (mp[i][j].type == General && mp[i][j].belong == currentplayer)
                            {
                                addGrenade(i, j, sp, sq, playerGrenade[currentplayer] * 20, currentplayer);
                                i = X + 1;
                                playerGrenade[currentplayer] = 0;
                                break;
                            }
                }
            }
            Sleep(tpt);
            if ((X > 15 || Y > 15) && KEY_DOWN('Q'))
            {
                if (miniMapLevel > 3)
                    miniMapLevel--;
            }
            if ((X > 15 || Y > 15) && KEY_DOWN('E'))
            {
                if (miniMapLevel < 9)
                    miniMapLevel++;
            }
            if (miniMapOpt == 1)
                isMiniMap = true;
            else if (miniMapOpt == 2)
            {
                if (KEY_DOWN('M'))
                    isMiniMap = true;
                else
                    isMiniMap = false;
            }
            if (inp == 'Z')
                player[currentplayer].halfselect ^= 1;
            else
                player[currentplayer].playermove(inp);
            if (KEY_DOWN('F'))
            {
                while (1)
                {
                    currentplayer = (currentplayer == gennum ? 1 : currentplayer + 1);
                    bool flag = false;
                    for (int i = 1; i <= X; i++)
                        for (int j = 1; j <= Y; j++)
                            if (mp[i][j].type == General && mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                            {
                                flag = true;
                                i = X + 1;
                                break;
                            }
                    if (flag)
                        break;
                }
            }
            for (int i = 1; i <= gennum; i++)
                if (player[i].isbot)
                    player[i].botmove();
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                {
                    if (((mp[i][j].type == General || mp[i][j].type == 5) && mapmode != 5 && mapmode != 6 && mapmode != CPoints) || (mp[i][j].type == General && (mapmode == 5 || mapmode == 6 || mapmode == CPoints) && mp[i][j].tmp < playermaxhp[mp[i][j].belong]))
                        mp[i][j].tmp++;
                    else if (mp[i][j].type == Land && turn % 15 == 0)
                        mp[i][j].tmp++;
                    for (int k = 1; k <= gennum; k++)
                        sight[k][i][j] = false;
                    if ((mapmode == 5 || mapmode == 6 || mapmode == CPoints) && mp[i][j].type == General && mp[i][j].tmp > playermaxhp[mp[i][j].belong])
                        mp[i][j].tmp = playermaxhp[mp[i][j].belong];
                }
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].belong)
                    {
                        sight[mp[i][j].belong][i][j] = true;
                        if (mapmode == 5 && blindtimeremain[mp[i][j].belong] > 0)
                            continue;
                        if (mapmode == 5 && mp[i][j].belong == currentplayer)
                            for (int k = i - 2 - ishavets[currentplayer]; k <= i + 2 + ishavets[currentplayer]; k++)
                                for (int w = j - 2 - ishavets[currentplayer]; w <= j + 2 + ishavets[currentplayer]; w++)
                                    if (k >= 1 && k <= X && w >= 1 && w <= Y)
                                        sight[mp[i][j].belong][k][w] = true;
                                    else
                                        ;
                        else
                            for (int px = i - 1; px <= i + 1; px++)
                                for (int py = j - 1; py <= j + 1; py++)
                                {
                                    if (px >= 1 && px <= X && py >= 1 && py <= Y)
                                        sight[mp[i][j].belong][px][py] = true;
                                }
                    }
                    else if (isGrenade[i][j])
                        for (int k = i - 2; k <= i + 2; k++)
                            for (int w = j - 2; w <= j + 2; w++)
                                if (k >= 1 && k <= X && w >= 1 && w <= Y)
                                    sight[isGrenade[i][j]][k][w] = true;
            if (mp[player[currentplayer].selectedx][player[currentplayer].selectedy].belong != currentplayer)
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == General && mp[i][j].belong == currentplayer)
                        {
                            player[currentplayer].selectedx = i;
                            player[currentplayer].selectedy = j;
                            i = X + 1;
                            break;
                        }
            if (rm == 1 && mapmode == 5 && aliveobjectnum < objectnum)
                convobject();
            if (mapmode == 5)
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (fog[i][j] && mp[i][j].type == General && mp[i][j].tmp >= 1)
                        {
                            mp[i][j].tmp -= int(double(foglevel < 5 ? 10 : 50) * (1.0 - double(playerfh[mp[i][j].belong]) * 0.02));
                            if (mp[i][j].tmp <= 0)
                                mp[i][j].tmp = mp[i][j].belong = 0, alivegennum--, mp[i][j].type = Empty_land;
                        }
            if (mapmode == 5 && rm == dq)
            {
                foglevel++;
                for (int i = foglevel; i <= X - foglevel + 1; i++)
                    for (int j = foglevel; j <= Y - foglevel + 1; j++)
                        if (i == foglevel || i == X - foglevel + 1 || j == foglevel || j == Y - foglevel + 1)
                            fog[i][j] = 1;
            }
            if (mapmode == 5)
                for (int i = 1; i <= gennum; i++)
                    if (blindtimeremain[i] > 0)
                        blindtimeremain[i]--;
                    else if (blindtimeremain[i] == 0)
                        blindtimeremain[i] = -1;
            if (mapmode == 5 && turn % kttime == 0)
                spawnkt();
            if (isPaint && turn)
            {
                int tmp = -1;
                bool isOne = true;
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == General && mp[i][j].tmp > 0)
                        {
                            if (tmp == -1)
                                tmp = mp[i][j].belong;
                            else if (tmp != mp[i][j].belong)
                            {
                                isOne = false;
                                i = X + 1;
                                break;
                            }
                        }
                if (isOne)
                    break;
            }
            if (mapmode == Pubg || mapmode == CFlag || mapmode == CPoints)
                updateGrenade();
            savemap();
            turn++;
            if (isPaint)
                paintRemainTime--;
        }
        if (isPaint)
        {
            int maxLand = -1, playerLand[105];
            string winner = "";
            for (int i = 1; i <= gennum; i++)
                playerLand[i] = 0;
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if ((mp[i][j].type == Land || mp[i][j].type == General || mp[i][j].type == City) && mp[i][j].tmp > 0)
                        playerLand[mp[i][j].belong]++, maxLand = max(maxLand, playerLand[mp[i][j].belong]);
            if (maxLand == -1)
                if (MessageBox(NULL, "没有玩家胜利。是否保存回放？", "欢呼", MB_OKCANCEL) == IDOK)
                    savereplay();
                else
                    ;
            else
            {
                for (int i = 1; i <= gennum; i++)
                    if (playerLand[i] == maxLand)
                        if (winner == "")
                            winner = "player" + myto_string(i);
                        else
                        {
                            winner += ", player" + myto_string(i);
                        }
                winner += "赢了！是否保存回放？";
                if (MessageBox(NULL, winner.c_str(), "欢呼", MB_OKCANCEL) == IDOK)
                    savereplay();
            }
            return 0;
        }
        int winner = -1;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].type == General && mp[i][j].tmp > 0)
                {
                    winner = mp[i][j].belong;
                    i = X + 1;
                    break;
                }
        if (winner == -1)
        {
            if (MessageBox(NULL, "没有玩家胜利。是否保存回放？", "欢呼", MB_OKCANCEL) == IDOK)
                savereplay();
        }
        else
        {
            string opt = "player" + myto_string(winner) + "赢了！是否保存回放？";
            if (MessageBox(NULL, opt.c_str(), "欢呼", MB_OKCANCEL) == IDOK)
                savereplay();
        }
        return 0;
    }
    if (mapmode == 7)
        ifcanconvobject = true;
    while (aliveteamnum > 1 && (!isPaint || isPaint && paintRemainTime > 0))
    {
        if (isGMode)
            playerGrenade[currentplayer] = 1;
        rm = dq - turn % dq;
        putmap(player[currentplayer].selectedx, player[currentplayer].selectedy, currentplayer);
        char inp = ' ';
        if (mapmode == 6 || mapmode == 7)
        {
            for (int i = 1; i <= gennum; i++)
                if (wd[i] > 0)
                    playerac[i] = 1000000, wd[i]--;
                else if (wd[i] == 0)
                    playerac[i] = 0, wd[i] = -1;
        }
        if (mapmode == 5)
        {
            if (ktremaintime > 0)
                ktremaintime--;
            if (ktremaintime == 0)
            {
                getkt();
                ktremaintime = -1;
            }
        }
        bool ismouse = true;
        for (int j = 0; j < 5; j++)
            if (inp == ' ' && KEY_DOWN(keys[j]))
                inp = keys[j];
        if (ismouse && KEY_DOWN(MOUSE_MOVED))
        {
            GetCursorPos(&p1);
            int sp = int(round((double(p1.y) - double(xx1)) / dx)) + 1 + (X > 15 ? player[currentplayer].selectedx - 8 : 0);
            int sq = int(round((double(p1.x) - double(yy1)) / dy)) + 1 + (Y > 15 ? player[currentplayer].selectedy - 8 : 0);
            player[currentplayer].selectedx = sp;
            player[currentplayer].selectedy = sq;
            ismouse = false;
            if (isHaveSend[currentplayer] && sight[currentplayer][sp][sq] && mp[sp][sq].type == Empty_land)
            {
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == General && mp[i][j].belong == currentplayer)
                        {
                            swap(mp[i][j], mp[sp][sq]);
                            i = X + 1;
                            isHaveSend[currentplayer] = false;
                            break;
                        }
            }
            else if (playerGrenade[currentplayer] > 0 && mp[sp][sq].type != Wall)
            {
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == General && mp[i][j].belong == currentplayer)
                        {
                            addGrenade(i, j, sp, sq, playerGrenade[currentplayer] * 20, currentplayer);
                            i = X + 1;
                            playerGrenade[currentplayer] = 0;
                            break;
                        }
            }
        }
        Sleep(tpt);
        if ((X > 15 || Y > 15) && KEY_DOWN('Q'))
        {
            if (miniMapLevel > 3)
                miniMapLevel--;
        }
        if ((X > 15 || Y > 15) && KEY_DOWN('E'))
        {
            if (miniMapLevel < 9)
                miniMapLevel++;
        }
        if (miniMapOpt == 1)
            isMiniMap = true;
        else if (miniMapOpt == 2)
        {
            if (KEY_DOWN('M'))
                isMiniMap = true;
            else
                isMiniMap = false;
        }
        if (inp == 'Z')
            player[currentplayer].halfselect ^= 1;
        else if (inp == 'F')
        {
            while (1)
            {
                currentplayer = (currentplayer == gennum ? 1 : currentplayer + 1);
                bool flag = false;
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == General && mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                        {
                            flag = true;
                            i = X + 1;
                            break;
                        }
                if (flag)
                    break;
            }
        }
        for (int i = 1; i <= teamnum; i++)
            team[i].move(inp);
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
            {
                if (((mp[i][j].type == General || mp[i][j].type == 5) && mapmode != 5 && mapmode != 6 && mapmode != 7) || (mp[i][j].type == General && (mapmode == 5 || mapmode == 6 || mapmode == 7) && mp[i][j].tmp < playermaxhp[mp[i][j].belong]))
                    mp[i][j].tmp++;
                else if (mp[i][j].type == Land && turn % 15 == 0)
                    mp[i][j].tmp++;
                for (int k = 1; k <= gennum; k++)
                    sight[k][i][j] = false;
                if ((mapmode == 5 || mapmode == 6 || mapmode == 7) && mp[i][j].type == General && mp[i][j].tmp > playermaxhp[mp[i][j].belong])
                    mp[i][j].tmp = playermaxhp[mp[i][j].belong];
            }
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong && mp[i][j].type != 20)
                {
                    for (int k = 1; k <= team[player[mp[i][j].belong].inteam].membernum; k++)
                        sight[team[player[mp[i][j].belong].inteam].members[k]->playerid][i][j] = true;
                    if ((mapmode == 5 || mapmode == 6 || mapmode == 7) && blindtimeremain[mp[i][j].belong] > 0)
                        continue;
                    if (mp[i][j].type == Points || mp[i][j].type == Flag || mp[i][j].type == Empty_flag)
                        continue;
                    if ((mapmode == 5 || mapmode == 6 || mapmode == 7) && player[mp[i][j].belong].inteam == player[currentplayer].inteam)
                        for (int k = i - 2 - ishavets[mp[i][j].belong]; k <= i + 2 + ishavets[mp[i][j].belong]; k++)
                            for (int w = j - 2 - ishavets[mp[i][j].belong]; w <= j + 2 + ishavets[mp[i][j].belong]; w++)
                                if (k >= 1 && k <= X && w >= 1 && w <= Y)
                                    for (int p = 1; p <= team[player[mp[i][j].belong].inteam].membernum; p++)
                                        sight[team[player[mp[i][j].belong].inteam].members[p]->playerid][k][w] = true;
                                else
                                    ;
                    for (int px = i - 1; px <= i + 1; px++)
                        for (int py = j - 1; py <= j + 1; py++)
                        {
                            if (px >= 1 && px <= X && py >= 1 && py <= Y)
                                for (int k = 1; k <= team[player[mp[i][j].belong].inteam].membernum; k++)
                                    sight[team[player[mp[i][j].belong].inteam].members[k]->playerid][px][py] = true;
                        }
                }
                else if (isGrenade[i][j] && Inteam[isGrenade[i][j]] == Inteam[currentplayer])
                    for (int k = i - 2; k <= i + 2; k++)
                        for (int w = j - 2; w <= j + 2; w++)
                            if (k >= 1 && k <= X && w >= 1 && w <= Y)
                                for (int p = 1; p <= team[Inteam[isGrenade[i][j]]].membernum; p++)
                                    sight[team[Inteam[isGrenade[i][j]]].members[p]->playerid][k][w] = true;
                            else
                                ;
        if (mp[player[currentplayer].selectedx][player[currentplayer].selectedy].belong != currentplayer)
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].type == General && mp[i][j].belong == currentplayer)
                    {
                        player[currentplayer].selectedx = i;
                        player[currentplayer].selectedy = j;
                        i = X + 1;
                        break;
                    }
        if (rm == 1 && mapmode == 5 && aliveobjectnum < objectnum && aliveobjectnum < int(double(X * Y) * objectpr) && (!fvf || (fvf && foglevel % 3 == 0)))
            convobject();
        if ((mapmode == 6 || mapmode == 7) && ifcanconvobject && aliveobjectnum < objectnum)
        {
            ifcanconvobject = false;
            convobject();
        }
        if (mapmode == 5)
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (fog[i][j] && mp[i][j].type == General && mp[i][j].tmp >= 1)
                    {
                        mp[i][j].tmp -= int(double(foglevel < 5 ? 10 : 50) * (1.0 - double(playerfh[mp[i][j].belong]) * 0.02));
                        if (mp[i][j].tmp <= 0)
                            mp[i][j].tmp = mp[i][j].belong = 0, alivegennum--, mp[i][j].type = Empty_land;
                    }
        if (mapmode == 5 && rm == dq)
        {
            foglevel++;
            for (int i = foglevel; i <= X - foglevel + 1; i++)
                for (int j = foglevel; j <= Y - foglevel + 1; j++)
                    if (i == foglevel || i == X - foglevel + 1 || j == foglevel || j == Y - foglevel + 1)
                        fog[i][j] = 1;
        }
        if (mapmode != 6 && mapmode != 7)
            for (int i = 1; i <= teamnum; i++)
            {
                bool ok = false;
                if (teamdead[i])
                    continue;
                for (int j = 1; j <= X; j++)
                    for (int k = 1; k <= Y; k++)
                        if (player[mp[j][k].belong].inteam == i)
                        {
                            ok = true;
                            j = X + 1;
                            break;
                        }
                if (!ok)
                    aliveteamnum--, teamdead[i] = 1;
            }
        if (mapmode == 6 || mapmode == 7)
        {
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].type == General && mp[i][j].tmp <= 0)
                    {
                        if (mapmode == 7)
                            teampointsmatchscore[Inteam[mp[i][j].belong]] = max(0, teampointsmatchscore[Inteam[mp[i][j].belong]] - 10);
                        player[mp[i][j].belong].respawn();
                    }
            if (mapmode == 6)
            {
                string opt = "";
                for (int i = 1; i <= teamnum; i++)
                    if (flagscore[i] >= 10)
                    {
                        if (opt == "")
                            opt += "team" + myto_string(i);
                        else
                        {
                            opt += ", team" + myto_string(i);
                        }
                    }
                if (opt != "")
                {
                    opt += "赢了！是否保存回放？";
                    if (MessageBox(NULL, opt.c_str(), "欢呼", MB_OKCANCEL) == IDOK)
                        savereplay();
                    return 0;
                }
            }
        }
        if (mapmode == 5 || mapmode == 6 || mapmode == 7)
            for (int i = 1; i <= gennum; i++)
                if (blindtimeremain[i] > 0)
                    blindtimeremain[i]--;
                else if (blindtimeremain[i] == 0)
                    blindtimeremain[i] = -1;
        bool isget = false;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                {
                    isget = true;
                    i = X + 1;
                    break;
                }
        if (!isget)
        {
            isgz = true;
            while (1)
            {
                currentplayer = (currentplayer == gennum ? 1 : currentplayer + 1);
                bool flag = false;
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == General && mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                        {
                            flag = true;
                            i = X + 1;
                            break;
                        }
                if (flag)
                    break;
            }
        }
        if (mapmode == 5 && turn % kttime == 0)
            spawnkt();
        if (mapmode == 7)
        {
            for (int i = 1; i <= teamnum; i++)
                teampointsmatchland[i] = 0;
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].type == 20 && mp[i][j].belong != 0)
                        teampointsmatchland[mp[i][j].belong]++;
            for (int i = 1; i <= teamnum; i++)
                teampointsmatchscore[i] += teampointsmatchland[i];
            string opt = "";
            for (int i = 1; i <= teamnum; i++)
                if (teampointsmatchscore[i] >= 1000)
                {
                    if (opt == "")
                        opt += "team" + myto_string(i);
                    else
                    {
                        opt += ", team" + myto_string(i);
                    }
                }
            if (opt != "")
            {
                opt += "赢了！是否保存回放？";
                if (MessageBox(NULL, opt.c_str(), "欢呼", MB_OKCANCEL) == IDOK)
                    savereplay();
                return 0;
            }
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                {
                    if (mp[i][j].type == 20 && mp[i][j].belong != 0 && mp[i][j].tmp > 0)
                    {
                        bool foundplayer = false, foundenemy = false;
                        for (int k = i - 2; k <= i + 2; k++)
                            for (int w = j - 2; w <= j + 2; w++)
                                if (k >= 1 && k <= X && w >= 1 && w <= Y && mp[k][w].type == General && mp[k][w].tmp > 0)
                                {
                                    if (Inteam[mp[k][w].belong] == mp[i][j].belong)
                                        foundplayer = true;
                                    else
                                        foundenemy = true;
                                }
                        if (foundenemy && !foundplayer)
                        {
                            mp[i][j].tmp--;
                            if (mp[i][j].tmp <= 0)
                            {
                                news[newsr].a = mp[i][j].belong;
                                news[newsr].opt = 5;
                                news[newsr].remtime = 50;
                                newsr++;
                                mp[i][j].tmp = 0;
                                mp[i][j].belong = 0;
                            }
                        }
                        if (foundplayer && !foundenemy && mp[i][j].tmp < pointstime)
                            mp[i][j].tmp++;
                    }
                    else if (mp[i][j].type == 20 && mp[i][j].belong == 0 && mp[i][j].tmp < pointstime)
                    {
                        int playerteam = -1;
                        bool sameteam = true;
                        for (int k = i - 2; k <= i + 2; k++)
                        {
                            for (int w = j - 2; w <= j + 2; w++)
                                if (k >= 1 && k <= X && w >= 1 && w <= Y && mp[k][w].type == General && mp[k][w].tmp > 0)
                                {
                                    if (playerteam == -1)
                                        playerteam = Inteam[mp[k][w].belong];
                                    else if (playerteam != Inteam[mp[k][w].belong])
                                    {
                                        sameteam = false;
                                        break;
                                    }
                                }
                            if (!sameteam)
                                break;
                        }
                        if (sameteam && playerteam != -1)
                        {
                            mp[i][j].tmp++;
                            if (mp[i][j].tmp >= pointstime)
                            {
                                news[newsr].a = playerteam;
                                news[newsr].opt = 4;
                                news[newsr].remtime = 50;
                                newsr++;
                                mp[i][j].tmp = pointstime;
                                mp[i][j].belong = playerteam;
                                ifcanconvobject = true;
                            }
                        }
                    }
                }
        }
        if (mapmode == Pubg || mapmode == CFlag || mapmode == CPoints)
            updateGrenade();
        savemap();
        turn++;
        if (isPaint)
            paintRemainTime--;
    }
    if (isPaint)
    {
        int maxLand = -1, teamLand[105];
        string winner = "";
        for (int i = 1; i <= teamnum; i++)
            teamLand[i] = 0;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if ((mp[i][j].type == Land || mp[i][j].type == General || mp[i][j].type == City) && mp[i][j].tmp > 0)
                    teamLand[Inteam[mp[i][j].belong]]++, maxLand = max(maxLand, teamLand[Inteam[mp[i][j].belong]]);
        if (maxLand == -1)
            if (MessageBox(NULL, "没有队伍胜利。是否保存回放？", "欢呼", MB_OKCANCEL) == IDOK)
                savereplay();
            else
                ;
        else
        {
            for (int i = 1; i <= teamnum; i++)
                if (teamLand[i] == maxLand)
                    if (winner == "")
                        winner = "team" + myto_string(i);
                    else
                    {
                        winner += ", team" + myto_string(i);
                    }
            winner += "赢了！是否保存回放？";
            if (MessageBox(NULL, winner.c_str(), "欢呼", MB_OKCANCEL) == IDOK)
                savereplay();
        }
        return 0;
    }
    int winner = -1;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type == General && mp[i][j].tmp > 0)
            {
                winner = player[mp[i][j].belong].inteam;
                i = X + 1;
                break;
            }
    if (winner != -1)
    {
        string opt = "team" + myto_string(winner) + "赢了！是否保存回放？";
        if (MessageBox(NULL, opt.c_str(), "欢呼", MB_OKCANCEL) == IDOK)
            savereplay();
    }
    else if (MessageBox(NULL, "没有队伍胜利。是否保存回放？", "欢呼", MB_OKCANCEL) == IDOK)
        savereplay();
    return 0;
}
