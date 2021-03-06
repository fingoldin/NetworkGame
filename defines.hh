#pragma once

#include <stdint.h>

#define CORE_ANTI_ALIAS       2
#define CORE_WINDOW_WIDTH     1920
#define CORE_WINDOW_HEIGHT    1080
#define CORE_FULLSCREEN       false
#define CORE_VSYNC            true

#define SERVER_PORT           1024
#define CLIENT_PORT           1056
#define RECEIVE_TIMEOUT_S     2
#define RECEIVE_TIMEOUT_US    0
#define RECEIVE_LEN           256
#define SEND_LEN              256

#define USLEEP_TIME           10000      // ms
#define CONNECT_TIMEOUT_TIME  4000       // ms
#define SERVER_TIMEOUT_TIME   4000       // ms
#define DISCONNECT_WAIT_TIME  400        // ms
#define PLAYERS_SEND_TIME     1000       // ms
#define PLAYER_TIMEOUT_TIME   4000       // ms
#define PLAYER_SEND_TIME      700        // ms
#define PLAYERS_SEND_ALL_TIME 800        // ms
#define SEND_ALL_TIME         30        // ms

#define CONNECT_SIG           "$N^n"
#define YOU_CONNECT_SIG       "dfgg"
#define DISCONNECT_SIG        "lp[;"
#define YOU_DISCONNECT_SIG    "l[p1"
#define UPDATE_SIG            "]1sd"
#define PLAYERS_SEND_SIG       "7892"

#define DISCONNECT_MES        "(*Bf"
#define DISCONNECT_MES_NUM    4
#define CONNECT_MES           "7sdf"
#define UPDATE_MES            "sdf2"

#define GUI_PAD               5
#define GUI_WIDTH             100
#define EDIT_BOX_HEIGHT       40
#define BUTTON_HEIGHT         40
#define EDIT_BOX_ID           41
#define CONNECT_BOX_ID        42
#define DISCONNECT_BOX_ID     43

#define VIEWPORT_MAX          20.0

#define PLAYER_HEIGHT         0.7
#define PLAYER_WIDTH          0.7
#define PLAYER_HITBOX_W       1.0
#define PLAYER_HITBOX_H       1.0
#define PLAYER_GROUND         0.37
#define PLAYER_PUNCH          0.40

#define MAP_DIR               "./maps/"

#define PLATFORM_HEIGHT       0.1

#define START_X               0.0
#define START_Y               2.0

#define PLAYER_SPEED          5.0
#define PLAYER_ACC            60.0
#define PLAYER_DECC           80.0
#define PLAYER_AIR_SPEED      6.0
#define PLAYER_AIR_ACC        16.0
#define PLAYER_AIR_DECC       1.0
#define PLAYER_JUMP_IMPULSE   6.5
#define PLAYER_PUNCH_IMPULSE  7.0
#define PLAYER_PUNCH_ANGLE    1.2           // radians

#define GRAVITY               10.0

typedef unsigned long         ip_t;
typedef uint16_t              playid_t;
typedef unsigned long long    ms_t;

enum E_INPUT {
	EI_LEFT = 0,
	EI_RIGHT,
	EI_JUMP,
	EI_PUNCH,
	EI_COUNT
};

enum E_MOUSE_BUTTON {
	EMB_LEFT = 0,
	EMB_COUNT
};
