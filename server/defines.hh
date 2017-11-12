#pragma once


#define SERVER_PORT           1024
#define CLIENT_PORT           1056
#define RECEIVE_TIMEOUT_S     1
#define RECEIVE_TIMEOUT_US    0
#define RECEIVE_LEN           256

#define USLEEP_TIME           10000
#define CONNECT_TIMEOUT_TIME  6000000

#define CONNECT_SIG           "$N^n"
#define YOU_CONNECT_SIG       "dfgg"
#define DISCONNECT_SIG        "lp[;"
#define UPDATE_SIG            "]1sd"

#define DISCONNECT_MES        "(*Bf"
#define DISCONNECT_MES_NUM    4
#define CONNECT_MES           "7sdf"
#define UPDATE_MES            "sdf2"

#define START_X               0.0
#define START_Y               0.5

#define CONNECT_SEND_TIMES    4
#define DISCONNECT_SEND_TIMES 4


#define PLAYER_MOVE_SPEED     0.4


typedef unsigned long         ip_t;
typedef short                 playid_t;
typedef float                 n_block_t;
typedef unsigned long long    ms_t;

enum E_INPUT {
        EI_LEFT = 0,
        EI_RIGHT,
        EI_COUNT
};
