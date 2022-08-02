#include <master_header.h>
#include <syslog.h>
#include <common.h>
#include <can.h>
#include <unistd.h>


void * start_master(void * _config) {
    master_config_t * config = (master_config_t *)_config;
    req_t req;
    resp_t resp;
    int rsize;

    printf("Starting can master\n");
    if (start_can_master(config->can_config_path) != CAN_RETOK) {
        fprintf(stderr, "Error while starting can master\n");
        return NULL;
    }

    canbyte * voltage_adc = malloc(sizeof(canbyte) * 4);
    canbyte * ref_voltage = malloc(sizeof(canbyte) * 4);
    canbyte * ext_voltage = malloc(sizeof(canbyte) * 4);
    canbyte * mez_temp = malloc(sizeof(canbyte) * 4);
    int16 sdo_status;

    int MAPPING[] = {999,11,7,9,6,3,4,1,2,5,13,8,10,15,12,14,16,17,18,19,21,20,22,26,23,27,30,24,25,32,31,29,28,
    43,39,41,38,35,36,33,34,37,45,40,42,47,44,46,48,49,50,51,53,52,54,58,55,59,62,56,57,64,63,61,60,
    75,71,73,70,67,68,65,66,69,77,72,74,79,76,78,80,81,82,83,85,84,86,90,87,91,94,88,89,96,95,93,92,
    107,103,105,102,99,100,97,98,101,109,104,106,111,108,110,112,113,114,115,117,116,118,122,119,123,126,120,121,128,127,125,124};

    while(TRUE) {
        can_sleep(100000);
        rsize = read(config->req_fd, &req, sizeof(req_t));
        if (rsize == sizeof(req_t)){
            if (req.type == SetVoltage) {
                syslog(LOG_INFO, "Set voltage request received: node: %d --- channel: %d --- voltage: %d\n",req.node, req.subindex, req.voltage_dac);
                sdo_status = write_device_object(req.node, CANOPEN_DAC_W, MAPPING[req.subindex], (canbyte *)&req.voltage_dac, 2);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while writing canopen object: idx: %d sub: %d\n", CANOPEN_DAC_W, req.subindex);
                }
            } else if (req.type == GetVoltage) {
                syslog(LOG_INFO, "Get voltage request received: node: %d --- channel: %d\n", req.node, req.subindex);
                sdo_status = read_device_object(req.node, CANOPEN_ADC_R, MAPPING[req.subindex], voltage_adc, sizeof(canbyte) * 4);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_ADC_R, req.subindex);
                    continue;
                }
                resp.subindex = req.subindex;
                resp.value = *((unsigned32 *)voltage_adc);
                write(config->resp_fd, &resp, sizeof(resp_t));
            }
            else if (req.type == GetRefVoltage) {
                syslog(LOG_INFO, "Get ref voltage request received: node: %d --- channel: %d\n", req.node, req.subindex);
                sdo_status = read_device_object(req.node, CANOPEN_REF_R, req.subindex, ref_voltage, sizeof(canbyte) * 4);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_REF_R, req.subindex);
                    continue;
                }
                resp.subindex = req.subindex;
                resp.value = *((unsigned32 *)ref_voltage);
                write(config->resp_fd, &resp, sizeof(resp_t));
            }
            else if (req.type == GetExtVoltage) {
                syslog(LOG_INFO, "Get ext voltage request received: node: %d --- channel: %d\n", req.node, req.subindex);
                sdo_status = read_device_object(req.node, CANOPEN_EXT_R, req.subindex, ext_voltage, sizeof(canbyte) * 4);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_EXT_R, req.subindex);
                    continue;
                }
                resp.subindex = req.subindex;
                resp.value = *((unsigned32 *)ext_voltage);
                write(config->resp_fd, &resp, sizeof(resp_t));
            }
            else if (req.type == GetMezTemp) {
                syslog(LOG_INFO, "Get mez.temperature request received: node: %d --- subindex: %d\n", req.node, req.subindex);
                sdo_status = read_device_object(req.node, CANOPEN_TEMP_R, req.subindex, mez_temp, sizeof(canbyte) * 4);
                if (sdo_status != CAN_TRANSTATE_OK) {
                    syslog(LOG_ERR, "Error while reading canopen object: idx: %d sub: %d\n", CANOPEN_TEMP_R, req.subindex);
                    continue;
                }
                resp.subindex = req.subindex;
                resp.value = *((unsigned32 *)mez_temp);
                write(config->resp_fd, &resp, sizeof(resp_t));
            }
            else if (req.type == ResetNode) {
                syslog(LOG_INFO, "Reset node: %d\n", req.node);
                reset_can_node(req.node);
            }
            else if (req.type == ResetCanNetwork) {
                syslog(LOG_INFO, "Reset can network!\n");
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
