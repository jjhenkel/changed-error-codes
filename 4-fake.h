#include "common.h"

#define ANT_A 0
#define ANT_B 1
#define ANT_C BIT(2)

struct statistics_tx {
    struct {
        int ant_a;
        int ant_b;
        int ant_c;
    } tx_power;
};

struct iwl_priv {
    struct { 
        struct statistics_tx tx; 
    } statistics;
    struct {
        int valid_tx_ant;
    } * cfg;
};

struct file {
    struct iwl_priv * private_data;
};