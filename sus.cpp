﻿#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <string>;
#include <chrono>
#include <map>
#include <unordered_map>

using namespace std;
using ll = long long;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD prev_mode;
HANDLE hin = GetStdHandle(STD_INPUT_HANDLE); // получаем дескриптор
INPUT_RECORD InputRecord; // используется для возвращения информации о входных сообщениях в консольном входном буфере
DWORD Events; // unsigned long
COORD coord; // для координат X, Y

vector <vector <vector <bool>>> collizions(8, vector <vector <bool>>(100, vector <bool>(100, 0)));
vector <vector <vector <pair <bool, string>>>> clr(3, vector <vector <pair <bool, string>>>(100, vector <pair <bool, string>>(100, make_pair(0, ""))));
string which_clr;
map <pair<int, int>, string> objects;
unordered_map <string, bool> isfirst;
unordered_map <int, int> color_of_floor;
int cur_floor;
bool is_el_active = 0;
bool rer = 0;

COORD getpos()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;

    if (GetConsoleScreenBufferInfo(
        GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        coord.X = csbi.dwCursorPosition.X;
        coord.Y = csbi.dwCursorPosition.Y;
    }
    else
    {
        coord.X = 0;
        coord.Y = 0;
    }
    return coord;
}

void gotoxy(int x, int y)
{
    COORD position;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    position.X = x;
    position.Y = y;
    SetConsoleCursorPosition(hConsole, position);
}
void OutputPamPamPam(string s, int d1, int d2, int d3, int k, COORD c, int clr = 11) {
    SetConsoleTextAttribute(h, clr);
    gotoxy(c.X, c.Y);
    for (int i = 0; i < s.size(); i++) {
        switch (s[i]) {
        case '.' && '!' && '?':
            cout << s[i];
            Sleep(d2);
            break;
        case',':
            cout << s[i];
            Sleep(d3);
            break;
        default:
            cout << s[i];
            Sleep(d1);
            break;
        }
        if (i + 1 == k && i + 1 != s.size()) {
            gotoxy(c.X, c.Y);
            for (int j = 0; j <= i; j++) cout << ' ';
            gotoxy(c.X, c.Y);
            Sleep(100);
        }
    }
    gotoxy(c.X, c.Y);
    Sleep(1000);
    for (int j = 0; j <= k; j++) cout << ' ';
}

struct col {
    bool f;
    int x;
    int y;
};

struct draw {
    COORD ending;
    string letter;
    int dir;
    COORD begining;
    int color;
    int ox;
    int oy;
};

static void ClearScreen()
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
        hStdOut,
        (TCHAR)' ',
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

static void MOVER(int X, int Y)
{
    INPUT Inputs;
    Inputs.type = INPUT_MOUSE;
    Inputs.mi.dx = X;
    Inputs.mi.dy = Y;
    Inputs.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &Inputs, sizeof(INPUT));
    ZeroMemory(&Inputs, sizeof(INPUT));
}

void new_font(int x) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Получить консоль

    CONSOLE_FONT_INFOEX fontInfo;
    GetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo); // Получить текущий шрифт

    // Поменять какие-то параметры
    //wcsncpy(L"Arial Cyr", fontInfo.FaceName, LF_FACESIZE);  // Имя
    fontInfo.dwFontSize.X = 10; // Размер (в логических единицах)

    SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo); // Установить новый
}

void drawer(vector <draw>& v, int n_floor, bool f = 0, string s = "") {
    HANDLE h;
    h = GetStdHandle(STD_OUTPUT_HANDLE);
    for (auto i : v) {
        SetConsoleTextAttribute(h, i.color);
        gotoxy(i.begining.X, i.begining.Y);
        COORD cur = i.begining;
        collizions[n_floor][cur.X][cur.Y] = 1;
        while (cur.X != i.ending.X || cur.Y != i.ending.Y) {
            std :: cout << i.letter;
            if (f && isfirst[s]) {
                objects[make_pair(cur.X, cur.Y)] = s;
            }
            cur.X += i.ox;
            cur.Y += i.oy;
            collizions[n_floor][cur.X][cur.Y] = 1;
            gotoxy(cur.X, cur.Y);
        }
        cout << i.letter;
        if (f && isfirst[s]) {
            objects[make_pair(cur.X, cur.Y)] = s;
        }
        collizions[n_floor][cur.X][cur.Y] = 1;
    }
    if (f && isfirst[s]) {
        isfirst[s] = 0;
    }
}

void walking_stairs(int f) {
    ClearScreen();
    vector <draw> v(4);
    v[0].begining.X = 31;
    v[0].begining.Y = 46;
    v[0].ending.X = 63;
    v[0].ending.Y = 30;
    v[0].letter = "|";
    v[0].color = 15;
    v[0].ox = 2;
    v[0].oy = -1;
    //
    v[1].begining.X = 32;
    v[1].begining.Y = 46;
    v[1].ending.X = 64;
    v[1].ending.Y = 30;
    v[1].letter = "¯";
    v[1].color = 15;
    v[1].ox = 2;
    v[1].oy = -1;
    //
    v[2].begining.X = 26;
    v[2].begining.Y = 47;
    v[2].ending.X = 30;
    v[2].ending.Y = 47;
    v[2].letter = "¯";
    v[2].color = 15;
    v[2].ox = 1;
    v[2].oy = 0;
    //
    v[3].begining.X = 65;
    v[3].begining.Y = 30;
    v[3].ending.X = 68;
    v[3].ending.Y = 30;
    v[3].letter = "¯";
    v[3].color = 15;
    v[3].ox = 1;
    v[3].oy = 0;
    drawer(v, 7, 0, "");
    COORD c_begin, c_end;
    c_begin.X = f == 1 ? 30 : 64;
    c_begin.Y = f == 1 ? 46 : 29;
    c_end.X = f == -1 ? 30 : 64;
    c_end.Y = f == -1 ? 46 : 29;
    SetConsoleTextAttribute(h, 11);
    gotoxy(c_begin.X, c_begin.Y);
    cout << 'a';
    gotoxy(c_begin.X, c_begin.Y);
    Sleep(200);
    while (c_begin.X != c_end.X) {
        cout << ' ';
        c_begin.X += f * 2;
        c_begin.Y += (-1) * f;
        gotoxy(c_begin.X, c_begin.Y);
        cout << 'a';
        gotoxy(c_begin.X, c_begin.Y);
        Sleep(200);
    }
}

void draw_el(bool f) {
    vector <draw> v(2);
    if (f) which_clr = "el";
    v[0].begining.X = 22;
    v[0].begining.Y = 13;
    v[0].color = f ? 14 : color_of_floor[cur_floor];
    v[0].ending.X = 23;
    v[0].ending.Y = 13;
    v[0].letter = "*";
    v[0].ox = 1;
    v[0].oy = 0;
    //
    v[1].begining.X = 22;
    v[1].begining.Y = 12;
    v[1].color = f ? 14 : color_of_floor[cur_floor];
    v[1].ending.X = 23;
    v[1].ending.Y = 12;
    v[1].letter = "*";
    v[1].ox = 1;
    v[1].oy = 0;
    //

    drawer(v, cur_floor, 1, "el");
    SetConsoleTextAttribute(h, 11);
}

void draw_room1(bool f) {
    vector <draw> v(2);
    if (f) which_clr = "room1";
    v[0].begining.X = 35;
    v[0].begining.Y = 38;
    v[0].color = f ? 14 : color_of_floor[cur_floor];
    v[0].ending.X = 35;
    v[0].ending.Y = 38;
    v[0].letter = "/";
    v[0].ox = 0;
    v[0].oy = 0;
    //
    v[1].begining.X = 35;
    v[1].begining.Y = 37;
    v[1].color = f ? 14 : color_of_floor[cur_floor];
    v[1].ending.X = 35;
    v[1].ending.Y = 37;
    v[1].letter = "\\";
    v[1].ox = 0;
    v[1].oy = 0;
    drawer(v, cur_floor, 1, "room1");
    SetConsoleTextAttribute(h, 11);
}

void draw_room2(bool f) {
    vector <draw> v(2);
    if (f) which_clr = "room2";
    v[0].begining.X = 59;
    v[0].begining.Y = 46;
    v[0].color = f ? 14 : color_of_floor[cur_floor];
    v[0].ending.X = 59;
    v[0].ending.Y = 46;
    v[0].letter = "/";
    v[0].ox = 0;
    v[0].oy = 0;
    //
    v[1].begining.X = 59;
    v[1].begining.Y = 47;
    v[1].color = f ? 14 : color_of_floor[cur_floor];
    v[1].ending.X = 59;
    v[1].ending.Y = 47;
    v[1].letter = "\\";
    v[1].ox = 0;
    v[1].oy = 0;
    drawer(v, cur_floor, 1, "room2");
    SetConsoleTextAttribute(h, 11);
}


void draw_stairs(bool f) {
    vector <draw> v(3);
    if (f) which_clr = "stairs";
    v[0].begining.X = 73;
    v[0].begining.Y = 54;
    v[0].color = f ? 14 : color_of_floor[cur_floor];
    v[0].ending.X = 76;
    v[0].ending.Y = 54;
    v[0].letter = "|";
    v[0].ox = 1;
    v[0].oy = 0;
    //
    v[1].begining.X = 73;
    v[1].begining.Y = 53;
    v[1].color = f ? 14 : color_of_floor[cur_floor];
    v[1].ending.X = 76;
    v[1].ending.Y = 53;
    v[1].letter = "_";
    v[1].ox = 1;
    v[1].oy = 0;
    //
    v[2].begining.X = 73;
    v[2].begining.Y = 55;
    v[2].color = f ? 14 : color_of_floor[cur_floor];
    v[2].ending.X = 76;
    v[2].ending.Y = 55;
    v[2].letter = "¯";
    v[2].ox = 1;
    v[2].oy = 0;
    drawer(v, cur_floor, 1, "stairs");
    SetConsoleTextAttribute(h, 11);
}

void draw_doors(bool f) {
    vector <draw> v(1);
    if (f) which_clr = "doors";
    v[0].begining.X = 14;
    v[0].begining.Y = 32;
    v[0].color = f ? 14 : color_of_floor[cur_floor];
    v[0].ending.X = 14;
    v[0].ending.Y = 32;
    v[0].letter = "!";
    v[0].ox = 0;
    v[0].oy = 0;
    drawer(v, cur_floor, 1, "doors");
    SetConsoleTextAttribute(h, 11);
}

void DrawFloor3(int& cur_floor) {
    cur_floor = 2;
    vector <draw> v(20);
    //верхняя стенка
    v[0].begining.X = 27 + 3;
    v[0].begining.Y = 15;
    v[0].color = 12;
    v[0].ending.X = 47 + 20;
    v[0].ending.Y = 15;
    v[0].letter = "_";
    v[0].ox = 1;
    v[0].oy = 0;
    //правая стенка
    v[1].begining.X = 48 + 20;
    v[1].begining.Y = 16;
    v[1].color = 12;
    v[1].ending.X = 48 + 20;
    v[1].ending.Y = 51;
    v[1].ox = 0;
    v[1].oy = 1;
    v[1].letter = "│";
    //левая стенка
    v[2].begining.X = 26;
    v[2].begining.Y = 16 + 3 + 1;
    v[2].color = 12;
    v[2].ending.X = 26;
    v[2].ending.Y = 36 + 20;
    v[2].ox = 0;
    v[2].oy = 1;
    v[2].letter = "│";
    //нижняя стенка
    v[3].begining.X = 27;
    v[3].begining.Y = 37 + 20;
    v[3].color = 12;
    v[3].ending.X = 47 + 20;
    v[3].ending.Y = 37 + 20;
    v[3].ox = 1;
    v[3].oy = 0;
    v[3].letter = "¯";
    //левая диагональная
    v[4].begining.X = 25;
    v[4].begining.Y = 16 + 3;
    v[4].color = 12;
    v[4].ending.X = 20 - 1;
    v[4].ending.Y = 14 - 1;
    v[4].letter = '\\';
    v[4].ox = -1;
    v[4].oy = -1;
    //правая диагональная
    v[5].begining.X = 27 + 3 - 1;
    v[5].begining.Y = 15;
    v[5].color = 12;
    v[5].ending.X = 27 + 3 - 1 - 5;
    v[5].ending.Y = 15 - 5;
    v[5].letter = '\\';
    v[5].ox = -1;
    v[5].oy = -1;
    //верхняя диагональная
    v[6].begining.X = 20;
    v[6].begining.Y = 12;
    v[6].color = 12;
    v[6].ending.X = 23;
    v[6].ending.Y = 9;
    v[6].letter = "/";
    v[6].ox = 1;
    v[6].oy = -1;
    //
    v[7].begining.X = 26;
    v[7].begining.Y = 19;
    v[7].color = 12;
    v[7].ending.X = 26;
    v[7].ending.Y = 19;
    v[7].letter = "_";
    v[7].ox = 0;
    v[7].oy = 0;
    //
    v[8].begining.X = 26;
    v[8].begining.Y = 19;
    v[8].color = 12;
    v[8].ending.X = 26;
    v[8].ending.Y = 19;
    v[8].letter = "_";
    v[8].ox = 0;
    v[8].oy = 0;
    //
    v[9].begining.X = 23;
    v[9].begining.Y = 9;
    v[9].color = 12;
    v[9].ending.X = 23;
    v[9].ending.Y = 9;
    v[9].letter = "_";
    v[9].ox = 0;
    v[9].oy = 0;
    //
    v[10].begining.X = 22;
    v[10].begining.Y = 13;
    v[10].color = 12;
    v[10].ending.X = 23;
    v[10].ending.Y = 13;
    v[10].letter = "*";
    v[10].ox = 1;
    v[10].oy = 0;
    //
    v[11].begining.X = 22;
    v[11].begining.Y = 12;
    v[11].color = 12;
    v[11].ending.X = 23;
    v[11].ending.Y = 12;
    v[11].letter = "*";
    v[11].ox = 1;
    v[11].oy = 0;
    //
    v[12].begining.X = 19;
    v[12].begining.Y = 13;
    v[12].color = 12;
    v[12].ending.X = 19;
    v[12].ending.Y = 13;
    v[12].letter = "▏";
    v[12].ox = 0;
    v[12].oy = 0;
    //
    v[13].begining.X = 36;
    v[13].begining.Y = 21;
    v[13].color = 12;
    v[13].ending.X = 58;
    v[13].ending.Y = 21;
    v[13].letter = "¯";
    v[13].ox = 1;
    v[13].oy = 0;
    //
    v[14].begining.X = 36;
    v[14].begining.Y = 51;
    v[14].color = 12;
    v[14].ending.X = 58;
    v[14].ending.Y = 51;
    v[14].letter = "_";
    v[14].ox = 1;
    v[14].oy = 0;
    //
    v[15].begining.X = 35;
    v[15].begining.Y = 21;
    v[15].color = 12;
    v[15].ending.X = 35;
    v[15].ending.Y = 51;
    v[15].letter = "│";
    v[15].ox = 0;
    v[15].oy = 1;
    //
    v[16].begining.X = 59;
    v[16].begining.Y = 21;
    v[16].color = 12;
    v[16].ending.X = 59;
    v[16].ending.Y = 51;
    v[16].letter = "│";
    v[16].ox = 0;
    v[16].oy = 1;
    //лестница
    v[17].begining.X = 68;
    v[17].begining.Y = 57;
    v[17].color = 12;
    v[17].ending.X = 78;
    v[17].ending.Y = 57;
    v[17].letter = "¯";
    v[17].ox = 1;
    v[17].oy = 0;
    //
    v[18].begining.X = 69;
    v[18].begining.Y = 51;
    v[18].color = 12;
    v[18].ending.X = 78;
    v[18].ending.Y = 51;
    v[18].letter = "_";
    v[18].ox = 1;
    v[18].oy = 0;
    //
    v[19].begining.X = 79;
    v[19].begining.Y = 52;
    v[19].color = 12;
    v[19].ending.X = 79;
    v[19].ending.Y = 56;
    v[19].letter = "│";
    v[19].ox = 0;
    v[19].oy = 1;
    draw_stairs(0);

    drawer(v, 2, 0, "");
    draw_room1(0);
    draw_room2(0);
}

void DrawFloor2(int& cur_floor) {
    cur_floor = 1;
    int color_floor = color_of_floor[cur_floor];
    vector <draw> v(20);
    //верхняя стенка
    v[0].begining.X = 27 + 3;
    v[0].begining.Y = 15;
    v[0].color = color_floor;
    v[0].ending.X = 47 + 20;
    v[0].ending.Y = 15;
    v[0].letter = "_";
    v[0].ox = 1;
    v[0].oy = 0;
    //правая стенка
    v[1].begining.X = 48 + 20;
    v[1].begining.Y = 16;
    v[1].color = color_floor;
    v[1].ending.X = 48 + 20;
    v[1].ending.Y = 51;
    v[1].ox = 0;
    v[1].oy = 1;
    v[1].letter = "│";
    //левая стенка
    v[2].begining.X = 26;
    v[2].begining.Y = 16 + 3 + 1;
    v[2].color = color_floor;
    v[2].ending.X = 26;
    v[2].ending.Y = 36 + 20;
    v[2].ox = 0;
    v[2].oy = 1;
    v[2].letter = "│";
    //нижняя стенка
    v[3].begining.X = 27;
    v[3].begining.Y = 37 + 20;
    v[3].color = color_floor;
    v[3].ending.X = 47 + 20;
    v[3].ending.Y = 37 + 20;
    v[3].ox = 1;
    v[3].oy = 0;
    v[3].letter = "¯";
    //левая диагональная
    v[4].begining.X = 25;
    v[4].begining.Y = 16 + 3;
    v[4].color = color_floor;
    v[4].ending.X = 20 - 1;
    v[4].ending.Y = 14 - 1;
    v[4].letter = '\\';
    v[4].ox = -1;
    v[4].oy = -1;
    //правая диагональная
    v[5].begining.X = 27 + 3 - 1;
    v[5].begining.Y = 15;
    v[5].color = color_floor;
    v[5].ending.X = 27 + 3 - 1 - 5;
    v[5].ending.Y = 15 - 5;
    v[5].letter = '\\';
    v[5].ox = -1;
    v[5].oy = -1;
    //верхняя диагональная
    v[6].begining.X = 20;
    v[6].begining.Y = 12;
    v[6].color = color_floor;
    v[6].ending.X = 23;
    v[6].ending.Y = 9;
    v[6].letter = "/";
    v[6].ox = 1;
    v[6].oy = -1;
    //
    v[7].begining.X = 26;
    v[7].begining.Y = 19;
    v[7].color = color_floor;
    v[7].ending.X = 26;
    v[7].ending.Y = 19;
    v[7].letter = "_";
    v[7].ox = 0;
    v[7].oy = 0;
    //
    v[8].begining.X = 26;
    v[8].begining.Y = 19;
    v[8].color = color_floor;
    v[8].ending.X = 26;
    v[8].ending.Y = 19;
    v[8].letter = "_";
    v[8].ox = 0;
    v[8].oy = 0;
    //
    v[9].begining.X = 23;
    v[9].begining.Y = 9;
    v[9].color = color_floor;
    v[9].ending.X = 23;
    v[9].ending.Y = 9;
    v[9].letter = "_";
    v[9].ox = 0;
    v[9].oy = 0;
    //
    v[10].begining.X = 22;
    v[10].begining.Y = 13;
    v[10].color = color_floor;
    v[10].ending.X = 23;
    v[10].ending.Y = 13;
    v[10].letter = "*";
    v[10].ox = 1;
    v[10].oy = 0;
    //
    v[11].begining.X = 22;
    v[11].begining.Y = 12;
    v[11].color = color_floor;
    v[11].ending.X = 23;
    v[11].ending.Y = 12;
    v[11].letter = "*";
    v[11].ox = 1;
    v[11].oy = 0;
    //
    v[12].begining.X = 19;
    v[12].begining.Y = 13;
    v[12].color = color_floor;
    v[12].ending.X = 19;
    v[12].ending.Y = 13;
    v[12].letter = "▏";
    v[12].ox = 0;
    v[12].oy = 0;
    //
    v[13].begining.X = 36;
    v[13].begining.Y = 21;
    v[13].color = color_floor;
    v[13].ending.X = 58;
    v[13].ending.Y = 21;
    v[13].letter = "¯";
    v[13].ox = 1;
    v[13].oy = 0;
    //
    v[14].begining.X = 36;
    v[14].begining.Y = 51;
    v[14].color = color_floor;
    v[14].ending.X = 58;
    v[14].ending.Y = 51;
    v[14].letter = "_";
    v[14].ox = 1;
    v[14].oy = 0;
    //
    v[15].begining.X = 35;
    v[15].begining.Y = 21;
    v[15].color = color_floor;
    v[15].ending.X = 35;
    v[15].ending.Y = 51;
    v[15].letter = "│";
    v[15].ox = 0;
    v[15].oy = 1;
    //
    v[16].begining.X = 59;
    v[16].begining.Y = 21;
    v[16].color = color_floor;
    v[16].ending.X = 59;
    v[16].ending.Y = 51;
    v[16].letter = "│";
    v[16].ox = 0;
    v[16].oy = 1;
    //лестница
    v[17].begining.X = 68;
    v[17].begining.Y = 57;
    v[17].color = color_floor;
    v[17].ending.X = 78;
    v[17].ending.Y = 57;
    v[17].letter = "¯";
    v[17].ox = 1;
    v[17].oy = 0;
    //
    v[18].begining.X = 69;
    v[18].begining.Y = 51;
    v[18].color = color_floor;
    v[18].ending.X = 78;
    v[18].ending.Y = 51;
    v[18].letter = "_";
    v[18].ox = 1;
    v[18].oy = 0;
    //
    v[19].begining.X = 79;
    v[19].begining.Y = 52;
    v[19].color = color_floor;
    v[19].ending.X = 79;
    v[19].ending.Y = 56;
    v[19].letter = "│";
    v[19].ox = 0;
    v[19].oy = 1;
    draw_stairs(0);
    drawer(v, cur_floor, 0, "");
    draw_room1(0);
    //draw_room2(0);
}

void DrawFloor1() {
    cur_floor = 0;
    int color_floor = color_of_floor[cur_floor];
    vector <draw> v(24);
    //верхняя стенка
    v[0].begining.X = 27 + 3;
    v[0].begining.Y = 15;
    v[0].color = color_floor;
    v[0].ending.X = 47 + 20;
    v[0].ending.Y = 15;
    v[0].letter = "_";
    v[0].ox = 1;
    v[0].oy = 0;
    //правая стенка
    v[1].begining.X = 48 + 20;
    v[1].begining.Y = 16;
    v[1].color = color_floor;
    v[1].ending.X = 48 + 20;
    v[1].ending.Y = 51;
    v[1].ox = 0;
    v[1].oy = 1;
    v[1].letter = "│";
    //левая стенка
    v[2].begining.X = 26;
    v[2].begining.Y = 20;
    v[2].color = color_floor;
    v[2].ending.X = 26;
    v[2].ending.Y = 30;
    v[2].ox = 0;
    v[2].oy = 1;
    v[2].letter = "│";
    //
    v[20].begining.X = 26;
    v[20].begining.Y = 34;
    v[20].color = color_floor;
    v[20].ending.X = 26;
    v[20].ending.Y = 36 + 20;
    v[20].ox = 0;
    v[20].oy = 1;
    v[20].letter = "│";
    //нижняя стенка
    v[3].begining.X = 27;
    v[3].begining.Y = 37 + 20;
    v[3].color = color_floor;
    v[3].ending.X = 47 + 20;
    v[3].ending.Y = 37 + 20;
    v[3].ox = 1;
    v[3].oy = 0;
    v[3].letter = "¯";
    //левая диагональная
    v[4].begining.X = 25;
    v[4].begining.Y = 16 + 3;
    v[4].color = color_floor;
    v[4].ending.X = 20 - 1;
    v[4].ending.Y = 14 - 1;
    v[4].letter = '\\';
    v[4].ox = -1;
    v[4].oy = -1;
    //правая диагональная
    v[5].begining.X = 27 + 3 - 1;
    v[5].begining.Y = 15;
    v[5].color = color_floor;
    v[5].ending.X = 27 + 3 - 1 - 5;
    v[5].ending.Y = 15 - 5;
    v[5].letter = '\\';
    v[5].ox = -1;
    v[5].oy = -1;
    //верхняя диагональная
    v[6].begining.X = 20;
    v[6].begining.Y = 12;
    v[6].color = color_floor;
    v[6].ending.X = 23;
    v[6].ending.Y = 9;
    v[6].letter = "/";
    v[6].ox = 1;
    v[6].oy = -1;
    //
    v[7].begining.X = 26;
    v[7].begining.Y = 19;
    v[7].color = color_floor;
    v[7].ending.X = 26;
    v[7].ending.Y = 19;
    v[7].letter = "_";
    v[7].ox = 0;
    v[7].oy = 0;
    //
    v[8].begining.X = 26;
    v[8].begining.Y = 19;
    v[8].color = color_floor;
    v[8].ending.X = 26;
    v[8].ending.Y = 19;
    v[8].letter = "_";
    v[8].ox = 0;
    v[8].oy = 0;
    //
    v[9].begining.X = 23;
    v[9].begining.Y = 9;
    v[9].color = color_floor;
    v[9].ending.X = 23;
    v[9].ending.Y = 9;
    v[9].letter = "_";
    v[9].ox = 0;
    v[9].oy = 0;
    //
    v[10].begining.X = 22;
    v[10].begining.Y = 13;
    v[10].color = color_floor;
    v[10].ending.X = 23;
    v[10].ending.Y = 13;
    v[10].letter = "*";
    v[10].ox = 1;
    v[10].oy = 0;
    //
    v[11].begining.X = 22;
    v[11].begining.Y = 12;
    v[11].color = color_floor;
    v[11].ending.X = 23;
    v[11].ending.Y = 12;
    v[11].letter = "*";
    v[11].ox = 1;
    v[11].oy = 0;
    //
    v[12].begining.X = 19;
    v[12].begining.Y = 13;
    v[12].color = color_floor;
    v[12].ending.X = 19;
    v[12].ending.Y = 13;
    v[12].letter = "▏";
    v[12].ox = 0;
    v[12].oy = 0;
    //
    v[13].begining.X = 36;
    v[13].begining.Y = 21;
    v[13].color = color_floor;
    v[13].ending.X = 58;
    v[13].ending.Y = 21;
    v[13].letter = "¯";
    v[13].ox = 1;
    v[13].oy = 0;
    //
    v[14].begining.X = 36;
    v[14].begining.Y = 51;
    v[14].color = color_floor;
    v[14].ending.X = 58;
    v[14].ending.Y = 51;
    v[14].letter = "_";
    v[14].ox = 1;
    v[14].oy = 0;
    //
    v[15].begining.X = 35;
    v[15].begining.Y = 21;
    v[15].color = color_floor;
    v[15].ending.X = 35;
    v[15].ending.Y = 51;
    v[15].letter = "│";
    v[15].ox = 0;
    v[15].oy = 1;
    //
    v[16].begining.X = 59;
    v[16].begining.Y = 21;
    v[16].color = color_floor;
    v[16].ending.X = 59;
    v[16].ending.Y = 51;
    v[16].letter = "│";
    v[16].ox = 0;
    v[16].oy = 1;
    //лестница
    v[17].begining.X = 68;
    v[17].begining.Y = 57;
    v[17].color = color_floor;
    v[17].ending.X = 78;
    v[17].ending.Y = 57;
    v[17].letter = "¯";
    v[17].ox = 1;
    v[17].oy = 0;
    //
    v[18].begining.X = 69;
    v[18].begining.Y = 51;
    v[18].color = color_floor;
    v[18].ending.X = 78;
    v[18].ending.Y = 51;
    v[18].letter = "_";
    v[18].ox = 1;
    v[18].oy = 0;
    //
    v[19].begining.X = 79;
    v[19].begining.Y = 52;
    v[19].color = color_floor;
    v[19].ending.X = 79;
    v[19].ending.Y = 56;
    v[19].letter = "│";
    v[19].ox = 0;
    v[19].oy = 1;
    draw_stairs(0);
    //кишка
    //
    v[21].begining.X = 15;
    v[21].begining.Y = 34;
    v[21].color = color_floor;
    v[21].ending.X = 25;
    v[21].ending.Y = 34;
    v[21].letter = "¯";
    v[21].ox = 1;
    v[21].oy = 0;
    //
    v[22].begining.X = 15;
    v[22].begining.Y = 30;
    v[22].color = color_floor;
    v[22].ending.X = 25;
    v[22].ending.Y = 30;
    v[22].letter = "_";
    v[22].ox = 1;
    v[22].oy = 0;
    //
    v[23].begining.X = 14;
    v[23].begining.Y = 31;
    v[23].color = color_floor;
    v[23].ending.X = 14;
    v[23].ending.Y = 33;
    v[23].letter = "│";
    v[23].ox = 0;
    v[23].oy = 1;
    drawer(v, cur_floor, 0, "");
    //draw_room1(0);
    draw_doors(0);
    draw_room2(0);
}

void intwithstairs() {
    gotoxy(1, 3);
    cout << "Лестница";
}

void drawwind() {
    vector <draw> v(4);
    v[0].begining.X = 20;
    v[0].begining.Y = 15;
    v[0].color = 15;
    v[0].ending.X = 20;
    v[0].ending.Y = 56;
    v[0].letter = "│";
    v[0].ox = 0;
    v[0].oy = 1;
    //
    v[1].begining.X = 21;
    v[1].begining.Y = 57;
    v[1].color = 15;
    v[1].ending.X = 88;
    v[1].ending.Y = 57;
    v[1].letter = "¯";
    v[1].ox = 1;
    v[1].oy = 0;
    //
    v[2].begining.X = 89;
    v[2].begining.Y = 56;
    v[2].color = 15;
    v[2].ending.X = 89;
    v[2].ending.Y = 15;
    v[2].letter = "│";
    v[2].ox = 0;
    v[2].oy = -1;
    //
    v[3].begining.X = 88;
    v[3].begining.Y = 14;
    v[3].color = 15;
    v[3].ending.X = 21;
    v[3].ending.Y = 14;
    v[3].letter = "_";
    v[3].ox = -1;
    v[3].oy = 0;
    drawer(v, 7, 0, "");

}

void windstairs(int &cur_floor) {
    ClearScreen();
    drawwind();
    vector <draw> v(12);
    v[0].begining.X = 30;
    v[0].begining.Y = 44;
    v[0].color = 15;
    v[0].ending.X = 30;
    v[0].ending.Y = 50;
    v[0].letter = "│";
    v[0].ox = 0;
    v[0].oy = 1;
    //
    v[1].begining.X = 31;
    v[1].begining.Y = 51;
    v[1].color = 15;
    v[1].ending.X = 42;
    v[1].ending.Y = 51;
    v[1].letter = "¯";
    v[1].ox = 1;
    v[1].oy = 0;
    //
    v[2].begining.X = 43;
    v[2].begining.Y = 50;
    v[2].color = 15;
    v[2].ending.X = 43;
    v[2].ending.Y = 44;
    v[2].letter = "│";
    v[2].ox = 0;
    v[2].oy = -1;
    //
    v[3].begining.X = 42;
    v[3].begining.Y = 43;
    v[3].color = 15;
    v[3].ending.X = 31;
    v[3].ending.Y = 43;
    v[3].letter = "_";
    v[3].ox = -1;
    v[3].oy = 0;
    //bvhvftdc
    v[4].begining.X = 66;
    v[4].begining.Y = 44;
    v[4].color = 15;
    v[4].ending.X = 66;
    v[4].ending.Y = 50;
    v[4].letter = "│";
    v[4].ox = 0;
    v[4].oy = 1;
    //
    v[5].begining.X = 67;
    v[5].begining.Y = 51;
    v[5].color = 15;
    v[5].ending.X = 67 + 11;
    v[5].ending.Y = 51;
    v[5].letter = "¯";
    v[5].ox = 1;
    v[5].oy = 0;
    //
    v[6].begining.X = 79;
    v[6].begining.Y = 44;
    v[6].color = 15;
    v[6].ending.X = 79;
    v[6].ending.Y = 50;
    v[6].letter = "│";
    v[6].ox = 0;
    v[6].oy = 1;
    //
    v[7].begining.X = 67;
    v[7].begining.Y = 43;
    v[7].color = 15;
    v[7].ending.X = 67 + 11;
    v[7].ending.Y = 43;
    v[7].letter = "_";
    v[7].ox = 1;
    v[7].oy = 0;
    //вторая
    //
    v[8].begining.X = 48;
    v[8].begining.Y = 44;
    v[8].color = 15;
    v[8].ending.X = 48;
    v[8].ending.Y = 50;
    v[8].letter = "│";
    v[8].ox = 0;
    v[8].oy = 1;
    //
    v[9].begining.X = 49;
    v[9].begining.Y = 51;
    v[9].color = 15;
    v[9].ending.X = 49 + 11;
    v[9].ending.Y = 51;
    v[9].letter = "¯";
    v[9].ox = 1;
    v[9].oy = 0;
    //
    v[10].begining.X = 61;
    v[10].begining.Y = 50;
    v[10].color = 15;
    v[10].ending.X = 61;
    v[10].ending.Y = 44;
    v[10].letter = "│";
    v[10].ox = 0;
    v[10].oy = -1;
    //
    v[11].begining.X = 49;
    v[11].begining.Y = 43;
    v[11].color = 15;
    v[11].ending.X = 60;
    v[11].ending.Y = 43;
    v[11].letter = "_";
    v[11].ox = 1;
    v[11].oy = 0;
    drawer(v, 7, 0, "");
    gotoxy(44, 20);
    std:: cout << "Вы дошли до лестницы \n";
    gotoxy(38, 22);
    cout << "На какой этаж вы хотите перейти ?";
    SetConsoleTextAttribute(h, 12);
    gotoxy(70, 47);
    cout << "Третий";
    gotoxy(52, 47);
    SetConsoleTextAttribute(h, 13);
    cout << "Второй";
    gotoxy(34, 47);
    SetConsoleTextAttribute(h, 10);
    cout << "Первый";
    while (1) {
        ReadConsoleInput(hin, &InputRecord, 1, &Events); // считывание
        switch (InputRecord.EventType) {
        case(MOUSE_EVENT):
            switch (InputRecord.Event.MouseEvent.dwButtonState) {
            case(FROM_LEFT_1ST_BUTTON_PRESSED):
                coord.X = InputRecord.Event.MouseEvent.dwMousePosition.X;
                coord.Y = InputRecord.Event.MouseEvent.dwMousePosition.Y;
                if (coord.X >= 30 && coord.X <= 43 && coord.Y >= 43 && coord.Y <= 51) {
                    if (cur_floor != 0) {
                        walking_stairs(-1);
                    }
                    cur_floor = 0;
                    goto label_end;
                }
                else {
                    if (coord.X >= 48 && coord.X <= 61 && coord.Y >= 43 && coord.Y <= 51) {
                        if (cur_floor != 1) {
                            walking_stairs(1 - cur_floor);
                        }
                        cur_floor = 1;
                        ClearScreen();
                        DrawFloor2(cur_floor);
                        goto label_end;
                    }
                    else {
                        if (coord.X >= 66 && coord.X <= 79 && coord.Y >= 43 && coord.Y <= 51) {
                            if (cur_floor != 2) {
                                walking_stairs(1);
                            }
                            cur_floor = 2;
                            ClearScreen();
                            DrawFloor3(cur_floor);
                            goto label_end;
                        }
                    }
                }
                break;
                break;
            }
        }
    }
label_end:;
}

static void intwithroom1() {
    SetConsoleTextAttribute(h, 15);
    ClearScreen();
    drawwind();
    gotoxy(21, 22);
    cout << "Привет";
    drawwind();
}

void intwithroom2() {
    gotoxy(1, 3);
    cout << "Комната2";
}

void intwithel() {
    if (is_el_active) {

    }
    else {
        rer = 0;
        COORD c;
        c.X = 24;
        c.Y = 59;
        OutputPamPamPam("Похоже лифт не работает. Может его можно как-то включить?", 30, 200, 200, 150, c);
        rer = 1;
    }
}


int main()
{
    color_of_floor[0] = 10;
    color_of_floor[1] = 13;
    color_of_floor[2] = 12;
    for (int i = 0; i < 3; i++) {
        for (int j = 20; j <= 25; j++) {
            for (int k = 10; k <= 15; k++) {
                clr[i][j][k].first = 1;
                clr[i][j][k].second = "el";
            }
        }
    }
    isfirst["el"] = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 68; j <= 79; j++) {
            for (int k = 51; k <= 60; k++) {
                clr[i][j][k].first = 1;
                clr[i][j][k].second = "stairs";
            }
        }
    }
    isfirst["stairs"] = 1;
    for (int i = 1; i < 3; i++) {
        for (int j = 32; j <= 35; j++) {
            for (int k = 35; k <= 40; k++) {
                clr[i][j][k].first = 1;
                clr[i][j][k].second = "room1";
            }
        }
    }
    isfirst["room1"] = 1;
    for (int i = 0; i < 1; i++) {
        for (int j = 59; j <= 62; j++) {
            for (int k = 44; k <= 49; k++) {
                clr[i][j][k].first = 1;
                clr[i][j][k].second = "room2";
            }
        }
    }
    for (int i = 2; i < 3; i++) {
        for (int j = 59; j <= 62; j++) {
            for (int k = 44; k <= 49; k++) {
                clr[i][j][k].first = 1;
                clr[i][j][k].second = "room2";
            }
        }
    }
    isfirst["room2"] = 1;
    for (int j = 14; j <= 25; j++) {
        for (int k = 31; k <= 34; k++) {
            clr[0][j][k].first = 1;
            clr[0][j][k].second = "doors";
        }
    }
    isfirst["doors"] = 1;
    {
        HANDLE h;
        h = GetStdHandle(STD_OUTPUT_HANDLE);
        ::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
        _CONSOLE_CURSOR_INFO cur;
        cur.bVisible = false;
        cur.dwSize = 1;
        SetConsoleCursorInfo(h, &cur);
        DWORD prev_mode;
        HANDLE hin = GetStdHandle(STD_INPUT_HANDLE); // получаем дескриптор
        INPUT_RECORD InputRecord; // используется для возвращения информации о входных сообщениях в консольном входном буфере
        DWORD Events; // unsigned long
        COORD coord; // для координат X, Y
        ReadConsoleInput(hin, &InputRecord, 1, &Events); // считывание
        //Запретить выделение консоли
        GetConsoleMode(hin, &prev_mode);
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));

        SetConsoleMode(hin, ENABLE_MOUSE_INPUT); // разрешаем обработку мыши
        //
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof cfi;
        cfi.nFont = 0;
        cfi.dwFontSize.X = 15;
        cfi.dwFontSize.Y = 15;
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_MEDIUM;
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &cfi);
    }
    cur_floor = 2;
    DrawFloor1();
    gotoxy(30, 20);
    cout << 'b';
    COORD b;
    b.X = 30;
    b.Y = 20;
    //OutputPamPamPam("Приветики. Как дела, солнышко?", 30, 200, 150, 100, b, 15);
    gotoxy(20, 10);
    COORD left_wall, right_wall, up_wall;
    //HANDLE h;
    h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 12);
    COORD c = getpos();
    right_wall = c;
    right_wall.X--;
    gotoxy(c.X, 11);
    SetConsoleTextAttribute(h, 15);
    gotoxy(50, 25);
    SetConsoleTextAttribute(h, 11);
    gotoxy(30, 38);
    c = getpos();
    cout << 'a';
    gotoxy(c.X, c.Y);
    bool f = 0;
    bool f2 = 0;
    while (1) {
        ReadConsoleInput(hin, &InputRecord, 1, &Events); // считывание
        switch (InputRecord.EventType) {
        case(KEY_EVENT):
            if (InputRecord.Event.KeyEvent.bKeyDown) {
                switch (InputRecord.Event.KeyEvent.uChar.UnicodeChar) {
                case 'a':
                    if (!collizions[cur_floor][c.X - 1][c.Y]) {
                        c.X--;
                        gotoxy(c.X, c.Y);
                        cout << 'a';
                        cout << ' ';
                        gotoxy(c.X, c.Y);
                    }
                    break;
                case 'd':
                    if (!collizions[cur_floor][c.X + 1][c.Y]) {
                        cout << ' ';
                        cout << 'a';
                        c.X++;
                        gotoxy(c.X, c.Y);
                    }
                    break;
                case 'w':
                    if (!collizions[cur_floor][c.X][c.Y - 1]) {
                        cout << ' ';
                        c.Y--;
                        gotoxy(c.X, c.Y);
                        cout << 'a';
                        gotoxy(c.X, c.Y);
                    }
                    break;
                case 's':
                    if (!collizions[cur_floor][c.X][c.Y + 1]) {
                        cout << ' ';
                        c.Y++;
                        gotoxy(c.X, c.Y);
                        cout << 'a';
                        gotoxy(c.X, c.Y);
                    }
                    break;
                }
                if (clr[cur_floor][c.X][c.Y].first) {
                    if (clr[cur_floor][c.X][c.Y].second == "el") {
                        which_clr = "el";
                        draw_el(1);
                        gotoxy(c.X, c.Y);
                    }
                    if (clr[cur_floor][c.X][c.Y].second == "stairs") {
                        which_clr = "stairs";
                        draw_stairs(1);
                        gotoxy(c.X, c.Y);
                    }
                    if (clr[cur_floor][c.X][c.Y].second == "room1") {
                        which_clr = "room1";
                        draw_room1(1);
                        gotoxy(c.X, c.Y);
                    }
                    if (clr[cur_floor][c.X][c.Y].second == "room2") {
                        which_clr = "room2";
                        draw_room2(1);
                        gotoxy(c.X, c.Y);
                    }
                    if (clr[cur_floor][c.X][c.Y].second == "doors") {
                        which_clr = "doors";
                        draw_doors(1);
                        gotoxy(c.X, c.Y);
                    }
                }
                else {
                    if (which_clr != "") {
                        if (which_clr == "el") {
                            which_clr = "";
                            draw_el(0);
                            gotoxy(c.X, c.Y);
                        }
                        if (which_clr == "stairs") {
                            which_clr = "";
                            draw_stairs(0);
                            gotoxy(c.X, c.Y);
                        }
                        if (which_clr == "room1") {
                            which_clr = "";
                            draw_room1(0);
                            gotoxy(c.X, c.Y);
                        }
                        if (which_clr == "room2") {
                            which_clr = "";
                            draw_room2(0);
                            gotoxy(c.X, c.Y);
                        }
                        if (which_clr == "doors") {
                            which_clr = "";
                            draw_doors(0);
                            gotoxy(c.X, c.Y);
                        }
                    }

                }
            }
            break;
        case(MOUSE_EVENT):
            switch (InputRecord.Event.MouseEvent.dwButtonState) {
            case(FROM_LEFT_1ST_BUTTON_PRESSED):
                coord.X = InputRecord.Event.MouseEvent.dwMousePosition.X;
                coord.Y = InputRecord.Event.MouseEvent.dwMousePosition.Y;
                //gotoxy(1, 4);
                //cout << "                                 ";
                //cout << coord.X << ' ' << coord.Y;
                if (which_clr == "stairs" && objects[make_pair(coord.X, coord.Y)] == "stairs") { 
                    windstairs(cur_floor); 
                    draw_stairs(1);
                    gotoxy(c.X, c.Y);
                    cout << "a";
                }
                if (which_clr == "room1" && objects[make_pair(coord.X, coord.Y)] == "room1") { intwithroom1(); }
                if (which_clr == "room2" && objects[make_pair(coord.X, coord.Y)] == "room2") { intwithroom2(); }
                if (which_clr == "el" && objects[make_pair(coord.X, coord.Y)] == "el") { 
                    if (is_el_active) intwithel(); 
                    else {
                        thread t(intwithel);
                        t.join();
                    }
                }
                gotoxy(c.X, c.Y);
                break;
            case(RIGHTMOST_BUTTON_PRESSED):

                break;
            }
            break;
        }
    }
    SetConsoleTextAttribute(h, 00);
    return 0;
}
