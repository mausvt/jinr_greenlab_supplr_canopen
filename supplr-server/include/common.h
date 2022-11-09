#include <master_header.h>

#ifndef __COMMON_H
#define __COMMON_H

#define CANOPEN_DAC_W 0x2411    // Set DAC voltage's (DAC code): subindex (channels) 1-128
#define CANOPEN_ADC_R 0x2402    // Read voltage from ADC (ADC code): subindex (channels) 1-128
#define CANOPEN_REF_R 0x6402    // Reference voltage: subindex = 1
#define CANOPEN_EXT_R 0x6402    // High voltage from power source: subindex = 2
#define CANOPEN_TEMP_R 0x6402   // Mezzanine temp: subindex (mez numbers) 3-6


typedef enum req_type_t {
    SetVoltage,
    GetVoltage,
    GetRefVoltage,
    GetExtVoltage,
    GetMezTemp,
    ResetNode,
    ResetCanNetwork
} req_type_t;

typedef enum can_status_t {
    READY,
    READING,
    SETTING,
    INIT
} can_status_t;

extern const char *can_status_name[4];
extern const char *func_name[7];
extern int can_status;

typedef struct req_t {
    req_type_t type;
    int node;
    int subindex;
    unsigned16 voltage_dac;
    int n_set;
} req_t;


typedef struct read_st {
    int node;
    int subindex;
    int32 value;
    // read_st() { clear(); }
    // void clear()
    // {
    //     node = 0;
    //     subindex = 0;
    //     value = 0;
    // }
} read_st;

typedef struct resp_st {
    read_st get_volt;
    read_st ref_volt;
    read_st ext_volt;
    read_st mez_temp;
    read_st reset_node;
    read_st reset_can_network;
    // resp_st() { clear(); }
    // void clear()
    // {
    //     get_volt.clear();
    // }
} resp_st;


// typedef struct resp_t {
//     int node;
//     int subindex;
//     int32 value_get_volt;
//     int32 value_ref_volt;
//     int32 value_ext_volt;
//     int32 value_mez_temp;
// } resp_t;



#endif
