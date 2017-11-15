#pragma once

#include <stdint.h>

#define CORE_ANTI_ALIAS       2
#define CORE_WINDOW_WIDTH     640
#define CORE_WINDOW_HEIGHT    480
#define CORE_FULLSCREEN       false
#define CORE_VSYNC            false


#define SERVER_PORT           1024
#define CLIENT_PORT           1056
#define RECEIVE_TIMEOUT_S     2
#define RECEIVE_TIMEOUT_US    0
#define RECEIVE_LEN           256

#define USLEEP_TIME           10000      // microseconds
#define CONNECT_TIMEOUT_TIME  4000       // milliseconds
#define SERVER_TIMEOUT_TIME   4000       // milliseconds
#define DISCONNECT_WAIT_TIME  400        // milliseconds

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

#define SEND_ALL_TIME         500        // milliseconds

#define METERS_TO_PIXELS      800.0

#define GUI_PAD               5
#define GUI_WIDTH             100
#define EDIT_BOX_HEIGHT       40
#define BUTTON_HEIGHT         40
#define EDIT_BOX_ID           41
#define CONNECT_BOX_ID        42
#define DISCONNECT_BOX_ID     43

#define VIEWPORT_MAX          10.0

#define PLAYER_HEIGHT         0.4
#define PLAYER_WIDTH          0.4

#define MAP_DIR               "./maps/"

#define PLATFORM_HEIGHT       0.1

typedef unsigned long         ip_t;
typedef uint16_t              playid_t;
typedef unsigned long long    ms_t;

enum E_INPUT {
	EI_LEFT = 0,
	EI_RIGHT,
	EI_JUMP,
	EI_COUNT
};
