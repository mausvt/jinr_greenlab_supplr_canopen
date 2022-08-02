#include <master_header.h>

#if CHECK_VERSION_APPL(1, 1, 0)

// Sending an NMT protocol frame to the CAN network
void nmt_master_command(unsigned8 cs, cannode node)
{
    canframe cf;

    msg_zero(&cf);
    cf.id = CAN_CANID_NMT;
    cf.data[0] = cs;
    cf.data[1] = node;
    cf.len = CAN_DATALEN_NMT_MC;
    send_can_data(&cf);
}

// Processing NMT frame
void consume_nmt(canframe *cf)
{
    cannode node;
    unsigned8 nst;

    node = (cannode)(cf->id - CAN_CANID_NMT_ERROR_BASE);
    if (node < CAN_NODE_ID_MIN || node > CAN_NODE_ID_MAX) return;
    nst = cf->data[0] & 0x7F;
    if (nst == CAN_NODE_STATE_INITIALISING) {
        if (can_node[node].node_status == ON) {
            can_node[node].ecpcnt = 1 + (1000 * CAN_CONFIG_NODE_MS / CAN_TIMERUSEC);
            node_event(node, EVENT_CLASS_NODE_STATE, EVENT_TYPE_INFO, EVENT_CODE_NODE_BOOTUP, EVENT_INFO_DUMMY);
        } else {
            node_event(node, EVENT_CLASS_NODE_STATE, EVENT_TYPE_WARNING, EVENT_CODE_NODE_UNDEFINED, EVENT_INFO_DUMMY);
        }
        can_node[node].nmt_state = nst;
        return;
    }
    if (can_node[node].node_status == OFF) return;
    if (can_node[node].nmt_state == CAN_NODE_STATE_DUMMY) return;
    if (nst == CAN_NODE_STATE_OPERATIONAL) {
        can_node[node].ecpcnt = 1 + (1000 * CAN_HBT_CONSUMER_MS / CAN_TIMERUSEC);
        if (can_node[node].nmt_state != CAN_NODE_STATE_OPERATIONAL) {
            node_event(node, EVENT_CLASS_NODE_STATE, EVENT_TYPE_INFO, EVENT_CODE_NODE_OPERATIONAL, EVENT_INFO_DUMMY);
        }
    }
    can_node[node].nmt_state = nst;
}

// Node heartbeat control
void manage_master_ecp(void)
{
    cannode node;
    unsigned8 rescnt;

    rescnt = 2;        // Max 2 nodes to be reset during one timer tick
    for (node = CAN_NODE_ID_MIN; node <= CAN_NODE_ID_MAX; node++) {
        if (can_node[node].node_status == OFF) continue;
        if (can_node[node].ecpcnt != 0) {
            can_node[node].ecpcnt--;
            if (can_node[node].ecpcnt == 0) {
                if (can_node[node].nmt_state == CAN_NODE_STATE_UNCERTAIN ||
                    can_node[node].nmt_state == CAN_NODE_STATE_DUMMY) {
                    if (rescnt > 0) {
                        rescnt--;
                        reset_can_node(node);
                        node_event(node, EVENT_CLASS_NODE_STATE, EVENT_TYPE_ERROR, EVENT_CODE_NODE_RESET, EVENT_INFO_DUMMY);
                    } else {
                        can_node[node].ecpcnt = 1;    // Will be reset at the next timer ticks
                    }
                } else {
                    can_node[node].nmt_state = CAN_NODE_STATE_UNCERTAIN;
                    can_node[node].ecpcnt = 1 + (1000 * CAN_HBT_CONSUMER_MS / CAN_TIMERUSEC);
                    node_event(node, EVENT_CLASS_NODE_STATE, EVENT_TYPE_WARNING, EVENT_CODE_NODE_HEARTBEAT, EVENT_INFO_DUMMY);
                }
            }
        }
    }
}

#endif
