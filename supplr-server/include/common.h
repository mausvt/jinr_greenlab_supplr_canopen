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

typedef struct req_t {
    req_type_t type;
    int node;
    int subindex;
    unsigned16 voltage_dac;
} req_t;

typedef struct resp_t {
    int node;
    int subindex;
    int32 value;
} resp_t;

#endif
