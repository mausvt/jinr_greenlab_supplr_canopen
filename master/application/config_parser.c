#include <master_header.h>
#include <string.h>
#include <yaml.h>
#include <syslog.h>
#include "../../supplr-server/include/http.h"

enum state {
    NO,
    CAN_NETWORK,
    BITRATE_INDEX,
    NODES
};
enum state_node {
    NO_NODE,
    NODE_ID,
    DEVICE_TYPE,
    VENDORID,
    PRODUCTCODE,
    REVISION
};
static long consume_node_id(yaml_event_t * e) {
    long node_id = strtol((char *)e->data.scalar.value, NULL, 10);
    can_node[node_id].node_status = ON;
    can_node[node_id].nmt_state = CAN_NODE_STATE_INITIALISING;
    return node_id;
}
static void consume_can_network(yaml_event_t * e) {
    can_network = strtol((char *)e->data.scalar.value, NULL, 16);
}
static void consume_bitrate_index(yaml_event_t * e) {
    bitrate_index = strtol((char *)e->data.scalar.value, NULL, 16);
}
static void consume_device_type(yaml_event_t * e, long node) {
    can_node[node].DeviceType = strtol((char *)e->data.scalar.value, NULL, 16);
    can_node[node].maskdev |= MASK_DEV_DEVICETYPE;
}
static void consume_vendor_id(yaml_event_t * e, long node) {
    can_node[node].VendorID = strtol((char *)e->data.scalar.value, NULL, 16);
    can_node[node].maskdev |= MASK_DEV_VENDORID;
}
static void consume_product_code(yaml_event_t * e, long node) {
    can_node[node].ProductCode = strtol((char *)e->data.scalar.value, NULL, 16);
    can_node[node].maskdev |= MASK_DEV_PRODUCTCODE;
}
static void consume_revision(yaml_event_t * e, long node) {
    can_node[node].Revision = strtol((char *)e->data.scalar.value, NULL, 16);
    can_node[node].maskdev |= MASK_DEV_REVISION;
}

int config_parser(const char *path_config)
{
    yaml_parser_t parser;
    yaml_event_t event;
    yaml_event_t value_event;
    int done = 0;
    long node;
    enum state state = NO;
    enum state_node state_node = NO_NODE;
    yaml_parser_initialize(&parser);
    FILE *input = fopen(path_config, "rb");
    yaml_parser_set_input_file(&parser, input);
    while (!done) {
        if (!yaml_parser_parse(&parser, &event))
            return 1;
        if (event.type == YAML_SCALAR_EVENT) {
            if (state == NO) {
                if (strcmp((char *)event.data.scalar.value, "CanNetwork") == 0) {
                    state = CAN_NETWORK;
                }
                else if (strcmp((char *)event.data.scalar.value, "BitrateIndex") == 0) {
                    state = BITRATE_INDEX;
                }
                else if (strcmp((char *)event.data.scalar.value, "Nodes") == 0) {
                    state = NODES;
                }
            }
            else if (state == CAN_NETWORK) {
                consume_can_network(&event);
                state = NO;
            }
            else if (state == BITRATE_INDEX) {
                consume_bitrate_index(&event);
                state = NO;
            }
            else if ((state == NODES) && (state_node == NODE_ID)) {
                node = consume_node_id(&event);
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == DEVICE_TYPE)) {
                consume_device_type(&event, node);
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == VENDORID)) {
                consume_vendor_id(&event, node);
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == PRODUCTCODE)) {
                consume_product_code(&event, node);
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == REVISION)) {
                consume_revision(&event, node);
                state_node = NO_NODE;
            }
            else if (state == NODES) {
                if (strcmp((char *)event.data.scalar.value, "DeviceType") == 0) {
                    state_node = DEVICE_TYPE;
                }
                else if (strcmp((char *)event.data.scalar.value, "VendorID") == 0) {
                    state_node = VENDORID;
                }
                else if (strcmp((char *)event.data.scalar.value, "NodeID") == 0) {
                    state_node = NODE_ID;
                }
                else if (strcmp((char *)event.data.scalar.value, "ProductCode") == 0) {
                    state_node = PRODUCTCODE;
                }
                else if (strcmp((char *)event.data.scalar.value, "Revision") == 0) {
                    state_node = REVISION;
                }
            }
        else if (event.type == YAML_SEQUENCE_END_EVENT) {
            state = NO;
            }
        }
        done = (event.type == YAML_STREAM_END_EVENT);
        yaml_event_delete(&event);
    }
    return 0;
}
