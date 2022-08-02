#include <stdio.h>
#include <syslog.h>
#include <master_header.h>

#if CHECK_VERSION_APPL(1, 1, 0)

static int node_config_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "Configure node: ");
    if (ev->code == EVENT_CODE_NODE_CONFIGURED) {
        len += sprintf(log + len, "OK");
    } else if (ev->code == EVENT_CODE_DEVICE_TYPE) {
        len += sprintf(log + len, "invalid device type 0x%08X ", (unsigned32)ev->info);
    } else if (ev->code == EVENT_CODE_VENDOR_ID) {
        len += sprintf(log + len, "invalid vendor id 0x%08X ", (unsigned32)ev->info);
    } else if (ev->code == EVENT_CODE_PRODUCT_CODE) {
        len += sprintf(log + len, "invalid product code 0x%08X ", (unsigned32)ev->info);
    } else if (ev->code == EVENT_CODE_REVISION) {
        len += sprintf(log + len, "invalid revision number 0x%08X ", (unsigned32)ev->info);
    } else if (ev->code == EVENT_CODE_SERIAL) {
        len += sprintf(log + len, "invalid serial number %-1i ", (unsigned32)ev->info);
    } else if (ev->code == EVENT_CODE_PROD_HBT    ) {
        len += sprintf(log + len, "heartbeat configuration failed ");
    } else {
        len += sprintf(log + len, "error %-1i ", ev->code);
    }
    return len;
}

static int node_state_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "Node state: ");
    if (ev->code == EVENT_CODE_NODE_HEARTBEAT) {
        len += sprintf(log + len, "Operational Heartbeat event ");
    } else if (ev->code == EVENT_CODE_NODE_OPERATIONAL) {
        len += sprintf(log + len, "Operational ");
    } else if (ev->code == EVENT_CODE_NODE_RESET) {
        len += sprintf(log + len, "Resetting node ");
    } else if (ev->code == EVENT_CODE_NODE_BOOTUP) {
        len += sprintf(log + len, "BootUp event ");
    } else if (ev->code == EVENT_CODE_NODE_UNDEFINED) {
        len += sprintf(log + len, "BootUp event from the UN-defined node ");
    } else {
        len += sprintf(log + len, "error %-1i ", ev->code);
    }
    return len;
}

static int node_sdo_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "SDO: ");
    if (ev->code == CAN_TRANSTATE_SDO_SRVABORT) {
        len += sprintf(log + len, "abort code 0x%08X received ", (unsigned32)ev->info);
    } else if (ev->code == CAN_TRANSTATE_SDO_TOGGLE) {
        len += sprintf(log + len, "segmented transfer toggle error ");
    } else if (ev->code == CAN_TRANSTATE_SDO_DATASIZE) {
        len += sprintf(log + len, "data size parameter is incorrect ");
    } else if (ev->code == CAN_TRANSTATE_SDO_MPX) {
        len += sprintf(log + len, "client and server multiplexors mismatch ");
    } else if (ev->code == CAN_TRANSTATE_SDO_WRITERR) {
        len += sprintf(log + len, "data write error ");
    } else if (ev->code == CAN_TRANSTATE_SDO_SCSERR) {
        len += sprintf(log + len, "not valid or unknown server command specifier ");
    } else if (ev->code == CAN_TRANSTATE_SDO_NET_TIMEOUT) {
        len += sprintf(log + len, "basic transaction network timeout ");
    } else if (ev->code == CAN_TRANSTATE_SDO_READ_TIMEOUT) {
        len += sprintf(log + len, "basic transaction read timeout ");
    } else if (ev->code == CAN_TRANSTATE_SDO_NOWORKB) {
        len += sprintf(log + len, "basic transaction work buffer is full ");
    } else {
        len += sprintf(log + len, "error %-1i ", ev->code);
    }
    return len;
}

static int master_status_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "Status:  ");
    if (ev->code == EVENT_CODE_MASTER_RUNNING) {
        len += sprintf(log + len, "running ");
    } else if (ev->code == EVENT_CODE_INVALID_CONFIG) {
        len += sprintf(log + len, "invalid master configuration ");
    } else {
        len += sprintf(log + len, "error %-1i ", ev->code);
    }
    return len;
}

static int master_config_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "Config file: ");
    if (ev->code == EVENT_CODE_FILE_OPEN) {
        len += sprintf(log + len, "open error - defaults used ");
    } else if (ev->code == CFG_WARNING_INV_SECTION) {
        len += sprintf(log + len, "invalid section name - ignored ");
    } else if (ev->code == CFG_WARNING_INV_TOKEN) {
        len += sprintf(log + len, "invalid token - ignored ");
    } else if (ev->code == CFG_WARNING_DUPL_TOKEN) {
        len += sprintf(log + len, "duplicated token - ignored ");
    } else if (ev->code == CFG_WARNING_INV_VALUE) {
        len += sprintf(log + len, "invalid value - ignored ");
    } else if (ev->code == CFG_WARNING_INV_NODE) {
        len += sprintf(log + len, "invalid CAN node-id - section discarded ");
    } else if (ev->code == CFG_WARNING_DUPL_NODE) {
        len += sprintf(log + len, "duplicated CAN node-id - section discarded ");
    } else if (ev->code == CFG_WARNING_UNDEF_NODE) {
        len += sprintf(log + len, "CAN node-id NOT defined yet - section discarded ");
    } else if (ev->code == CFG_WARNING_DUPL_FILENAMES) {
        len += sprintf(log + len, "duplicated [Filenames] section - discarded ");
    } else if (ev->code == CFG_WARNING_DUPL_CANNETWORK) {
        len += sprintf(log + len, "duplicated [CANnetwork] section - discarded ");
    } else if (ev->code == CFG_ERROR_UNDEF_FILENAMES) {
        len += sprintf(log + len, "[Filenames] section not defined - defaults used ");
    } else if (ev->code == CFG_ERROR_UNDEF_CANNETWORK) {
        len += sprintf(log + len, "[CANnetwork] section not defined - defaults used ");
    } else if (ev->code == CFG_ERROR_UNDEF_LOGFILE) {
        len += sprintf(log + len, "LogFile not defined - default used ");
    } else if (ev->code == CFG_ERROR_UNDEF_NETWORK) {
        len += sprintf(log + len, "CAN network not defined - default used ");
    } else if (ev->code == CFG_ERROR_UNDEF_BITRATE) {
        len += sprintf(log + len, "BitRate index not defined - default used ");
    } else if (ev->code == CFG_ERROR_VERSION) {
        len += sprintf(log + len, "invalid version, must be %08X - defaults used ", MASTER_CONFIG_FILE_VERSION);
    } else {
        len += sprintf(log + len, "error %-1i", ev->code);
    }
    if (ev->info != EVENT_INFO_DUMMY) len += sprintf(log + len, ", line %-1i ", ev->info);
    return len;
}

static int master_logger_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "Logger: ");
    if (ev->code == EVENT_CODE_CACHE_OVERFLOW) {
        len += sprintf(log + len, "cache overflow ");
    } else if (ev->code == EVENT_CODE_FIFO_OVERFLOW) {
        len += sprintf(log + len, "FIFO overflow ");
    } else if (ev->code == EVENT_CODE_LOGGER_FOPEN) {
        len += sprintf(log + len, "Logger create file error, default name used ");
    } else {
        len += sprintf(log + len, "error %-1i", ev->code);
    }
    return len;
}

static int master_chai_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "CHAI: ");
    if (ev->code == CAN_ERRET_CI_BITRATE) {
        len += sprintf(log + len, "CAN controller baud rate could not be set ");
    } else if (ev->code == CAN_ERRET_CI_HANDLER) {
        len += sprintf(log + len, "CAN driver handler registration failed ");
    } else if (ev->code == CAN_ERRET_CI_FILTER) {
        len += sprintf(log + len, "CAN controller acceptance filter set failed ");
    } else if (ev->code == CAN_ERRET_CI_START) {
        len += sprintf(log + len, "CAN controller transition to the start state failed ");
    } else if (ev->code == CAN_ERRET_CI_STOP) {
        len += sprintf(log + len, "CAN controller transition to the stop state failed ");
    } else if (ev->code == CAN_ERRET_CI_OPEN) {
        len += sprintf(log + len, "CAN IO channel opening error ");
    } else if (ev->code == CAN_ERRET_CI_CLOSE) {
        len += sprintf(log + len, "CAN IO channel closing error ");
    } else if (ev->code == CAN_ERRET_CI_INIT) {
        len += sprintf(log + len, "CAN CHAI init failed ");
    } else {
        len += sprintf(log + len, "error %-1i ", ev->code);
    }
    return len;
}

static int master_timer_event(const struct eventlog *ev, char * log)
{
    int len = 0;
    len += sprintf(log + len, "Master timer: ");
    if (ev->code == EVENT_CODE_LOW_PERIOD) {
        len += sprintf(log + len, "period is too low ");
    } else if (ev->code == EVENT_CODE_NO_CREATE) {
        len += sprintf(log + len, "failed to create waitable timer ");
    } else if (ev->code == EVENT_CODE_NO_SET) {
        len += sprintf(log + len, "failed to set waitable timer ");
    } else {
        len += sprintf(log + len, "error %-1i ", ev->code);
    }
    return len;
}


void log_event(struct eventlog *ev)
{
    int level = LOG_INFO;
    int len = 0;
    char * log;
    log = (char *)malloc(512);

    if (ev->node == EVENT_NODE_MASTER) len += sprintf(log + len, "Master  ");
    else len += sprintf(log + len, "Node %-3i ", ev->node);

    if (ev->cls == EVENT_CLASS_DUMMY) {
        len += sprintf(log + len, "Dummy event ");
    }
    else if (ev->cls == EVENT_CLASS_EMCY) {
        len += sprintf(log + len, "Emergency 0x%04X ", (unsigned16)ev->code);
    }
    else if (ev->cls == EVENT_CLASS_SYNC) {
        if (ev->code == EVENT_CODE_NO_SYNC) {
            len += sprintf(log + len, "Sync NOT received ");
        }
    }
    else if (ev->cls == EVENT_CLASS_NODE_STATE) {
        len += node_state_event(ev, log + len);
    }
    else if (ev->cls == EVENT_CLASS_NODE_CONFIG) {
        len += node_config_event(ev, log + len);
    }
    else if (ev->cls == EVENT_CLASS_NODE_SDO) {
        len += node_sdo_event(ev, log + len);
    }
    else if (ev->cls == EVENT_CLASS_MASTER_STATUS) {
        len += master_status_event(ev, log + len);
    }
    else if (ev->cls == EVENT_CLASS_MASTER_CONFIG) {
        len += master_config_event(ev, log + len);
    }
    else if (ev->cls == EVENT_CLASS_MASTER_LOGGER) {
        len += master_logger_event(ev, log + len);
    }
    else if (ev->cls == EVENT_CLASS_MASTER_CHAI) {
        len += master_chai_event(ev, log + len);
    }
    else if (ev->cls == EVENT_CLASS_MASTER_TIMER) {
        len += master_timer_event(ev, log + len);
    }

    // len += sprintf(log + len, "Class %-3i   Event %-6i ", ev->cls, ev->code);

    if (ev->type == EVENT_TYPE_INFO)         level = LOG_INFO;
    else if (ev->type == EVENT_TYPE_WARNING) level = LOG_WARNING;
    else if (ev->type == EVENT_TYPE_ERROR)   level = LOG_ERR;
    else if (ev->type == EVENT_TYPE_FATAL)   level = LOG_CRIT;
    else if (ev->type == EVENT_TYPE_DEBUG)   level = LOG_DEBUG;
    syslog(level, log);
}

#endif
