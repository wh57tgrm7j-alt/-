#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_ROOMS 3
#define HEIGHT 12
#define WIDTH  25

#define ROOM_START  0
#define ROOM_MIDDLE 1
#define ROOM_EXIT   2

typedef struct {
    int id;
    char map[HEIGHT][WIDTH + 1];
    int open;                    
    char* puzzle_desc;
    char* puzzle_answer;
    int connections[MAX_ROOMS];  
    int from_connections[MAX_ROOMS];
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
    "#   #     START       # #",
    "#   #                 # #",
    "#   #################   #",
    "#                       #",
    "#   EXIT->              #",
    "#                       #",
    "#                       #",
    "#########################"
};

const char* room2_map[HEIGHT] = {
    "#########################",
    "#                       #",
    "#   ################### #",
    "#   #                 # #",
    "#   #     ROOM 2      # #",
    "#   #                 # #",
    "#   #################   #",
    "#                       #",
    "#          EXIT->       #",
    "#                       #",
    "#                       #",
    "#########################"
};

const char* room3_map[HEIGHT] = {
    "#########################",
    "#                       #",
    "#   ################### #",
    "#   #                 # #",
    "#   #     EXIT        # #",
    "#   #   CONGRATULATIONS# #",
    "#   #################   #",
    "#                       #",
    "#   YOU ESCAPED!        #",
    "#                       #",
    "#                       #",
    "#########################"
};

const char* puzzle1_desc = 
"Головоломка 1 (Математика):\n"
"Какое число следующее в последовательности?\n"
"2, 3, 5, 7, 11, 13, ?";
const char* puzzle1_answer = "17";

const char* puzzle2_desc = 
"Головоломка 2 (Загадка):\n"
"Что можно увидеть один раз в минуте,\n"
"два раза в моменте,\n"
"но ни разу в тысяче лет?";
const char* puzzle2_answer = "буква м";

void set_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void reset_terminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void init_rooms() {
    rooms[0].id = 0;
    strcpy(rooms[0].map[0], room1_map[0]);
    for(int i = 0; i < HEIGHT; i++) strcpy(rooms[0].map[i], room1_map[i]);
    rooms[0].open = 1;
    rooms[0].puzzle_desc = NULL;
    rooms[0].puzzle_answer = NULL;
    memset(rooms[0].connections, 0, sizeof(rooms[0].connections));
    rooms[0].connections[1] = 1; 

    rooms[1].id = 1;
    for(int i = 0; i < HEIGHT; i++) strcpy(rooms[1].map[i], room2_map[i]);
    rooms[1].open = 0;
    rooms[1].puzzle_desc = (char*)puzzle1_desc;
    rooms[1].puzzle_answer = (char*)puzzle1_answer;
    memset(rooms[1].connections, 0, sizeof(rooms[1].connections));
    rooms[1].connections[2] = 1;

    rooms[2].id = 2;
    for(int i = 0; i < HEIGHT; i++) strcpy(rooms[2].map[i], room3_map[i]);
    rooms[2].open = 0;
    rooms[2].puzzle_desc = (char*)puzzle2_desc;
    rooms[2].puzzle_answer = (char*)puzzle2_answer;
    memset(rooms[2].connections, 0, sizeof(rooms[2].connections));
}

void place_player() {
    for(int y = 1; y < HEIGHT-1; y++) {
        for(int x = 1; x < WIDTH-1; x++) {
            if(rooms[player.current_room].map[y][x] == ' ') {
                player.x = x;
                player.y = y;
                return;
            }
        }
    }
}

void draw_room() {
    system("clear");
    Room* r = &rooms[player.current_room];
    
    printf("=== ЛАБИРИНТ === Комната %d %s ===\n", r->id + 1, r->open ? "(открыта)" : "(закрыта)");
    
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            if(x == player.x && y == player.y) {
                printf("C");
            } else {
                printf("%c", r->map[y][x]);
            }
        }
        printf("\n");
    }
    
    printf("\nУправление: WASD или стрелочки | Q - выход\n");
    if(!r->open && r->puzzle_desc) {
        printf("Нажмите 'E' чтобы попытаться открыть комнату!\n");
    }
}

int try_solve_puzzle() {
    Room* r = &rooms[player.current_room];
    if(!r->puzzle_desc) return 0;
    
    char answer[100];
    printf("\n%s\nВаш ответ: ", r->puzzle_desc);
    fgets(answer, sizeof(answer), stdin);
    answer[strcspn(answer, "\n")] = 0; 
    
    if(strcasecmp(answer, r->puzzle_answer) == 0) {
        printf("Правильно! Комната открыта!\n");
        r->open = 1;
        sleep(2);
        return 1;
    } else {
        printf("Неверно! Попробуйте позже.\n");
        sleep(2);
        return 0;
    }
}

int can_move_to(int target_room) {
    return rooms[player.current_room].connections[target_room] && rooms[target_room].open;
}

void move_player(int dx, int dy) {
    newx