#include <master_header.h>
#include <syslog.h>
#include <common.h>
#include <can.h>
#include <unistd.h>


void * start_master(void * _config) {
    can_status = INIT;
    master_config_t * config = (master_config_t *)_config;
    req_t req;
    resp_st resp;
    int rsize;

    printf("Starting can master\n");
    if (start_can_master(config->can_config_path) != CAN_RETOK) {
        fprintf(stderr, "Error while starting can master\n");
        fprintf(stderr, "Try use: find /dev/bus/usb -type c | sudo xargs chown root:adm\n");
        return NULL;
    }

    canbyte * voltage_adc = malloc(sizeof(canbyte) * 4);
    canbyte * ref_voltage = malloc(sizeof(canbyte) * 4);
    canbyte * ext_voltage = malloc(sizeof(canbyte) * 4);
    canbyte * mez_temp = malloc(sizeof(canbyte) * 4);
    int16 sdo_status;

    // CAN
    // int MAPPING[] = {999,11,7,9,6,3,4,1,2,5,13,8,10,15,12,14,16,17,18,19,21,20,22,26,23,27,30,24,25,32,31,29,28,
    // 43,39,41,38,35,36,33,34,37,45,40,42,47,44,46,48,49,50,51,53,52,54,58,55,59,62,56,57,64,63,61,60,
    // 75,71,73,70,67,68,65,66,69,77,72,74,79,76,78,80,81,82,83,85,84,86,90,87,91,94,88,89,96,95,93,92,
    // 107,103,105,102,99,100,97,98,101,109,104,106,111,108,110,112,113,114,115,117,116,118,122,119,123,126,120,121,128,127,125,124};

    // CAN-OPEN
    int MAPPING[] = {
        999,43,39,41,38,35,36,33,34,37,45,40,42,47,44,46,48,49,50,51,53,52,54,58,55,59,62,56,57,64,63,61,60,
        11,7,9,6,3,4,1,2,5,13,8,10,15,12,14,16,17,18,19,21,20,22,26,23,27,30,24,25,32,31,29,28,
        107,103,105,102,99,100,97,98,101,109,104,106,111,108,110,112,113,114,115,117,116,118,122,119,123,126,120,121,128,127,125,124,
        75,71,73,70,67,68,65,66,69,77,72,74,79,76,78,80,81,82,83,85,84,86,90,87,91,94,88,89,96,95,93,92
        };

    can_status = READY;
    int ch_set = 0;
    while(TRUE) {
        can_sleep(100000);
        rsize = read(config->req_fd, &req, sizeof(req_t));
        if (rsize == sizeof(req_t)){
            if (req.type == SetVoltage && can_status != INIT) {
                can_status = SETTING;
                ch_set++;
                syslog(LOG_INFO, "%s - node: %d ch: %d  volt: %d status: %d\n ",\
                    func_name[req.type], req.node, req.subindex, req.voltage_dac, can_status);
                sdo_status = write_device_object(req.node, CANOPEN_DAC_W, MAPPING[req.subindex], (canbyte *)&req.voltage_dac, 2);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while writing canopen object: idx: %d sub: %d\n", CANOPEN_DAC_W, req.subindex);
                }
                // sleep(0.1);
                if (ch_set>=req.n_set) {
                    ch_set = 0;
                    can_status = READY;
                    req.n_set = 1;
                }
            } else if (req.type == GetVoltage && can_status != SETTING && can_status != INIT) {
                syslog(LOG_INFO, "%s - node: %d ch: %d\n",func_name[req.type], req.node, req.subindex);
                sdo_status = read_device_object(req.node, CANOPEN_ADC_R, MAPPING[req.subindex], voltage_adc, sizeof(canbyte) * 4);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_ADC_R, req.subindex);
                    continue;
                }
                resp.get_volt.value = 0;
                resp.get_volt.subindex = req.subindex;
                resp.get_volt.value = *((unsigned32 *)voltage_adc);
                write(config->resp_fd, &resp, sizeof(resp_st));
            }
            else if (req.type == GetRefVoltage && can_status != SETTING && can_status != INIT) {
                syslog(LOG_INFO, "%s - node: %d\n",func_name[req.type], req.node);
                sdo_status = read_device_object(req.node, CANOPEN_REF_R, req.subindex, ref_voltage, sizeof(canbyte) * 4);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_REF_R, req.subindex);
                    continue;
                }
                resp.ref_volt.value = 0;
                resp.ref_volt.subindex = req.subindex;
                resp.ref_volt.value = *((unsigned32 *)ref_voltage);
                write(config->resp_fd, &resp, sizeof(resp_st));
            }
            else if (req.type == GetExtVoltage && can_status != SETTING && can_status != INIT) {
                syslog(LOG_INFO, "%s - node: %d\n",func_name[req.type], req.node);
                if (req.subindex == 2) {
                    sdo_status = read_device_object(req.node, CANOPEN_EXT_R, req.subindex, ext_voltage, sizeof(canbyte) * 4);
                }
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_EXT_R, req.subindex);
                    continue;
                }
                resp.ext_volt.value = 0;
                resp.ext_volt.subindex = req.subindex;
                resp.ext_volt.value = *((unsigned32 *)ext_voltage);
                write(config->resp_fd, &resp, sizeof(resp_st));
            }
            else if (req.type == GetMezTemp && can_status != SETTING && can_status != INIT) {
                syslog(LOG_INFO, "%s - node: %d mez: %d\n",func_name[req.type], req.node, req.subindex);
                sdo_status = read_device_object(req.node, CANOPEN_TEMP_R, req.subindex, mez_temp, sizeof(canbyte) * 4);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_TEMP_R, req.subindex);
                    continue;
                }
                resp.mez_temp.value = 0;
                resp.mez_temp.subindex = req.subindex;
                resp.mez_temp.value = *((unsigned32 *)mez_temp);
                write(config->resp_fd, &resp, sizeof(resp_st));
            }
            else if (req.type == ResetNode) {
                syslog(LOG_INFO, "%s - node: %d\n",func_name[req.type], req.node);
                reset_can_node(req.node);
            }
            else if (req.type == ResetCanNetwork) {
                syslog(LOG_INFO, "%s\n",func_name[req.type]);
                reset_can_network();
            }
        }
    }

    printf("Stopping can master\n");
    if (stop_can_master() != CAN_RETOK) {
        fprintf(stderr, "Error while stopping can master\n");
        return NULL;
    }
    return NULL;
}
