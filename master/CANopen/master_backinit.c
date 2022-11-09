#include <stdio.h>
#include <syslog.h>
#include <master_header.h>
#include "../../supplr-server/include/common.h"

#if CHECK_VERSION_CANLIB(3, 0, 4)

static int16 sem_sys=-1024;

static void canopen_timer(void)
{
    sem_sys++;
    if (sem_sys == 0) {
        // control_sync(); // Sync management
        can_client_control(); // Manage frame transaction
        // manage_master_ecp(); // Node heartbeat control
        push_all_can_data(); // Sending data from the buffer
    }
    sem_sys--;
}

static int16 set_controller_bitrate(unsigned8 br_index)
{
    if (CiStop(can_network) < 0) return CAN_ERRET_CI_STOP;
    if (br_index == CIA_BITRATE_INDEX_1000) {
        if (CiSetBaud(can_network, BCI_1M) < 0) return CAN_ERRET_CI_BITRATE;
    } else if (br_index == CIA_BITRATE_INDEX_800) {
        if (CiSetBaud(can_network, BCI_800K) < 0) return CAN_ERRET_CI_BITRATE;
    } else if (br_index == CIA_BITRATE_INDEX_500) {
        if (CiSetBaud(can_network, BCI_500K) < 0) return CAN_ERRET_CI_BITRATE;
    } else if (br_index == CIA_BITRATE_INDEX_250) {
        if (CiSetBaud(can_network, BCI_250K) < 0) return CAN_ERRET_CI_BITRATE;
    } else if (br_index == CIA_BITRATE_INDEX_125) {
        if (CiSetBaud(can_network, BCI_125K) < 0) return CAN_ERRET_CI_BITRATE;
    } else if (br_index == CIA_BITRATE_INDEX_50) {
        if (CiSetBaud(can_network, BCI_50K) < 0) return CAN_ERRET_CI_BITRATE;
    } else if (br_index == CIA_BITRATE_INDEX_20) {
        if (CiSetBaud(can_network, BCI_20K) < 0) return CAN_ERRET_CI_BITRATE;
    } else if (br_index == CIA_BITRATE_INDEX_10) {
        if (CiSetBaud(can_network, BCI_10K) < 0) return CAN_ERRET_CI_BITRATE;
    } else return CAN_ERRET_BITRATE;
    if (CiStart(can_network) < 0) return CAN_ERRET_CI_START;
    return CAN_RETOK;
}

static int16 can_init_controller(void)
{
    if (CiStop(can_network) < 0) return CAN_ERRET_CI_STOP;
    if (CiClose(can_network) < 0) return CAN_ERRET_CI_CLOSE;
    if (CiOpen(can_network, CIO_CAN11) < 0) return CAN_ERRET_CI_OPEN;
    if (CiSetWriteTout(can_network, 0) < 0) return CAN_ERRET_CI_WTOUT;    // NO timeout
    if (CiSetCB(can_network, CIEV_RC, (void*)can_read_handler) < 0) return CAN_ERRET_CI_HANDLER;
    if (CiSetCB(can_network, CIEV_CANERR, (void*)consume_controller_error) < 0) return CAN_ERRET_CI_HANDLER;
    return set_controller_bitrate(bitrate_index);
}


int16 start_can_master(const char *path_config)
{
    can_status = INIT;
    int16 fnr;
    syslog(LOG_INFO, "Apply configuration...");
    syslog(LOG_INFO, "Use configuration from: %s", path_config);
    configure(path_config);

    syslog(LOG_INFO, "CiInit ...");
    if (CiInit() < 0) {
        master_event(EVENT_CLASS_MASTER_CHAI, EVENT_TYPE_FATAL, CAN_ERRET_CI_INIT, EVENT_INFO_DUMMY);
        return CAN_ERRET_CI_INIT;
    }
    syslog(LOG_INFO, "CiOpen ...");
    if (CiOpen(can_network, 0) < 0) {
        master_event(EVENT_CLASS_MASTER_CHAI, EVENT_TYPE_FATAL, CAN_ERRET_CI_OPEN, EVENT_INFO_DUMMY);
        return CAN_ERRET_CI_OPEN;
    }
    syslog(LOG_INFO, "can_init_client");
    can_init_client();
    syslog(LOG_INFO, "can_init_sdo_client");
    can_init_sdo_client();
    syslog(LOG_INFO, "can_init_io");
    can_init_io();
    syslog(LOG_INFO, "can_init_system_timer");
    can_init_system_timer(canopen_timer);
    syslog(LOG_INFO, "can_init_controller");
    fnr = can_init_controller();
    if (fnr != CAN_RETOK) {
        master_event(EVENT_CLASS_MASTER_CHAI, EVENT_TYPE_FATAL, fnr, EVENT_INFO_DUMMY);
        return fnr;
    }
    sem_sys = -1;

    syslog(LOG_INFO, "configure_can_nodes");
    configure_can_nodes();

    syslog(LOG_INFO, "start_can_network");
    start_can_network();
    can_status = READY;
    return CAN_RETOK;
}

int16 stop_can_master(void)   // 3.0.4 return conditions changed
{
    syslog(LOG_INFO, "STOP CAN MASTER");
    sem_sys = -1024;
    can_cancel_system_timer();
    CiStop(can_network);
    CiClose(can_network);
    return CAN_RETOK;
}

#endif
