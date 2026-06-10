#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

#define MAX_ROOMS 3
#define HEIGHT 12
#define WIDTH 25

typedef struct {
    int id;
    char map[HEIGHT][WIDTH + 1];
    int open;
    const char* puzzle_desc;
    const char* puzzle_answer;
    int connections[MAX_ROOMS];
} Room;

typedef struct {
    int current_room;
    int x, y;
} Player;

Room rooms[MAX_ROOMS];
Player player;
int game_over = 0;

const char* room1_map[HEIGHT] = {
    "#########################",
    "#                       #",
    "#   ################### #",
    "#   #                 # #",
    "#   #     старт       # #",
    "#   #                 # #",
    "#   #################   #",
    "#                       #",
    "#            EXIT->      ",
    "#                       #",
    "#                       #",
    "#########################"
};

const char* room2_map[HEIGHT] = {
    "#########################",
    "#                       #",
    "#   ################### #",
    "#   #                 # #",
    "#   #     комната 2   # #",
    "#   #                 # #",
    "#   #################   #",
    "#                       #",
    "#          EXIT->       ",
    "#                       #",
    "#                       #",
    "#########################"
};

const char* room3_map[HEIGHT] = {
    "#########################",
    "#                       #",
    "#   ################### #",
    "#   #                 # #",
    "#   #     побег       # #",
    "#   #  успешный!!!!!! # #",
    "#   #################   #",
    "#                       #",
    "#   Ты победил!!!       ",
    "#                       #",
    "#                       #",
    "#########################"
};

void init_rooms() {
    for (int i = 0; i < HEIGHT; i++) strcpy(rooms[0].map[i], room1_map[i]);
    rooms[0].id = 0;
    rooms[0].open = 1;
    rooms[0].connections[1] = 1;

    for (int i = 0; i < HEIGHT; i++) strcpy(rooms[1].map[i], room2_map[i]);
    rooms[1].id = 1;
    rooms[1].open = 1;
    rooms[1].connections[2] = 1;

    for (int i = 0; i < HEIGHT; i++) strcpy(rooms[2].map[i], room3_map[i]);
    rooms[2].id = 2;
    rooms[2].open = 1;
}

void place_player() {
    player.x = 2;
    player.y = 1;
}

void draw_room() {
    system("cls");

    printf("=== ЛАБИРИНТ === Комната %d ===\n\n", player.current_room + 1);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == player.x && y == player.y)
                putchar('😜');
            else
                putchar(rooms[player.current_room].map[y][x]);
        }
        putchar('\n');
    }

    printf("\nWASD или стрелки. Q - выход\n");
}

void move_player(int dx, int dy) {
    int nx = player.x + dx;
    int ny = player.y + dy;

    if (nx < 1 || nx >= WIDTH - 1 || ny < 1 || ny >= HEIGHT - 1)
        return;

    if (rooms[player.current_room].map[ny][nx] == ' ') {
        player.x = nx;
        player.y = ny;
    }
}

int main() {
    SetConsoleOutputCP(65001);

    init_rooms();

    player.current_room = 0;
    place_player();

    while (!game_over) {
        draw_room();

        int ch = _getch();

        if (ch == 0 || ch == 224) {
            ch = _getch();

            switch (ch) {
                case 72: move_player(0, -1); break;
                case 80: move_player(0, 1); break;
                case 75: move_player(-1, 0); break;
                case 77: move_player(1, 0); break;
            }
        } else {
            switch (ch) {
                case 'w': case 'W': move_player(0, -1); break;
                case 's': case 'S': move_player(0, 1); break;
                case 'a': case 'A': move_player(-1, 0); break;
                case 'd': case 'D': move_player(1, 0); break;
                case 'q': case 'Q': game_over = 1; break;
            }
        }

        if (player.current_room == 0 && player.y == 8 && player.x >= 15) {
            player.current_room = 1;
            place_player();
        }

        if (player.current_room == 1 && player.y == 8 && player.x >= 15) {
            player.current_room = 2;
            place_player();
        }

        if (player.current_room == 2) {
            draw_room();
            printf("\nПОЗДРАВЛЯЕМ! ВЫ ВЫБРАЛИСЬ ИЗ ЛАБИРИНТА!\n");
            Sleep(3000);
            break;
        }
    }

    return 0;
}
