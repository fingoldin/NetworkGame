#pragma once

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
#define CONNECT_TIMEOUT_TIME  6000000    // microseconds

#define CONNECT_SIG           "$N^n"
#define YOU_CONNECT_SIG       "dfgg"
#define DISCONNECT_SIG        "lp[;"
#define UPDATE_SIG            "]1sd"

#define DISCONNECT_MES        "(*Bf"
#define DISCONNECT_MES_NUM    4
#define CONNECT_MES           "7sdf"
#define UPDATE_MES            "sdf2"

#define SEND_ALL_TIME         500        // milliseconds

#define METERS_TO_PIXELS      800.0


typedef unsigned long         ip_t;
typedef short                 playid_t;
typedef float                 n_block_t;
typedef unsigned long long    ms_t;

enum E_INPUT {
	EI_LEFT = 0,
	EI_RIGHT,
	EI_COUNT
};
