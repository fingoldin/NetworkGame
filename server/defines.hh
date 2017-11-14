#pragma once

#include <stdint.h>

#define SERVER_PORT           1024
#define CLIENT_PORT           1056
#define RECEIVE_TIMEOUT_S     1
#define RECEIVE_TIMEOUT_US    0
#define RECEIVE_LEN           256
#define SEND_LEN              256

#define USLEEP_TIME           10000          // us
#define CONNECT_TIMEOUT_TIME  6000000        // us
#define PLAYERS_SEND_TIME     1000           // ms
#define PLAYER_TIMEOUT_TIME   4000           // ms
#define PLAYER_SEND_TIME      700            // ms

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

#define START_X               0.0
#define START_Y               2.0

#define PLAYER_SPEED          5.0
#define PLAYER_ACC            20.0
#define PLAYER_DECC           40.0
#define PLAYER_AIR_SPEED      6.0
#define PLAYER_AIR_ACC        16.0
#define PLAYER_AIR_DECC       5.0
#define PLAYER_JUMP_IMPULSE   6.5

#define PLAYER_WIDTH          0.4
#define PLAYER_HEIGHT         0.4
#define PLAYER_GROUND         0.21

#define GRAVITY               10.0

#define IMMUNE_TIME           1000          // ms

#define CONNECT_SEND_TIMES    4
#define DISCONNECT_SEND_TIMES 4

#define MAP_DIR               "./maps/"

#define PLAYER_MOVE_SPEED     4.0


typedef unsigned long         ip_t;
typedef uint16_t              playid_t;
typedef float                 n_block_t;
typedef unsigned long long    ms_t;

enum E_INPUT {
        EI_LEFT = 0,
        EI_RIGHT,
	EI_JUMP,
        EI_COUNT
};
