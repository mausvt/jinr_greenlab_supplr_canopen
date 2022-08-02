#include <master_header.h>
#include <syslog.h>

#if CHECK_VERSION_APPL(1, 1, 1)

void consume_sync(unsigned8 sc)
{
}

void no_sync_event(void)
{
    master_event(EVENT_CLASS_SYNC, EVENT_TYPE_ERROR, EVENT_CODE_NO_SYNC, EVENT_INFO_DUMMY);
}

void consume_controller_error(canev ev)
{
    syslog(LOG_INFO, "consume_controller_error: %d", ev);
    if (ev == CIEV_BOFF) {
        master_event(EVENT_CLASS_EMCY, EVENT_TYPE_FATAL, CAN_EMCYREG_BOFF, EVENT_INFO_DUMMY);
    } else if (ev == CIEV_HOVR) {
        master_emcy(CAN_EMCY_HOVR);
    } else if (ev == CIEV_SOVR) {
        master_emcy(CAN_EMCY_SOVR);
    } else if (ev == CIEV_EWL) {
        master_emcy(CAN_EMCY_EWL);
    } else if (ev == CIEV_WTOUT) {
        master_emcy(CAN_EMCY_WTOUT);
    }
}

void master_emcy(unsigned16 errorcode)
{
    master_event(EVENT_CLASS_EMCY, EVENT_TYPE_ERROR, errorcode, EVENT_INFO_DUMMY);
}

void consume_emcy(canframe *cf)        // 1.1.1 some changes
{
    int16 node;
    unsigned16 emcy;
    unsigned8 type;

    node = cf->id - CAN_CANID_EMCY_BASE;
    if (node < CAN_NODE_ID_MIN || node > CAN_NODE_ID_MAX) return;
    emcy = canframe_to_u16(cf->data);
    if (emcy == CAN_EMCY_NO_ERROR) type = EVENT_TYPE_INFO;
    else if ((emcy & 0xFF00) == CAN_EMCY_NO_ERROR) type = EVENT_TYPE_WARNING;
    else type = EVENT_TYPE_ERROR;
    node_event((cannode)node, EVENT_CLASS_EMCY, type, emcy, EVENT_INFO_DUMMY);
}

void can_cache_overflow(void)
{
    syslog(LOG_INFO, "can_cache_overflow");
    master_emcy(CAN_EMCYREG_CACHE);
}

#endif
