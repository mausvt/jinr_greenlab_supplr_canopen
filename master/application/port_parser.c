#include <master_header.h>
#include <string.h>
#include <yaml.h>
#include <syslog.h>
#include "../../supplr-server/include/http.h"

enum state {
    NO,
    SERVER_ADDRESS,
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

int port_extraction(char *server_address) {
    char * token = strtok(server_address, ":");
        int i = 0;
        int int_port;
        char *port;
        while( token != NULL ) {
            if (i == 1) {
                port = token;
            }
            token = strtok(NULL, ":");
            i++;
        }
        int_port = atoi(port);
        return int_port;
}

int port_parser(char *path_config)
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
                if (strcmp((char *)event.data.scalar.value, "ServerAddress") == 0) {
                    state = SERVER_ADDRESS;
                }
                else if (strcmp((char *)event.data.scalar.value, "CanNetwork") == 0) {
                    state = CAN_NETWORK;
                }
                else if (strcmp((char *)event.data.scalar.value, "BitrateIndex") == 0) {
                    state = BITRATE_INDEX;
                }
                else if (strcmp((char *)event.data.scalar.value, "Nodes") == 0) {
                    state = NODES;
                }
            }
            else if (state == SERVER_ADDRESS) {
                int SETTING_PORT = port_extraction(event.data.scalar.value);
                state = NO;
                return SETTING_PORT;
            }
            else if (state == CAN_NETWORK) {
                state = NO;
            }
            else if (state == BITRATE_INDEX) {
                state = NO;
            }
            else if ((state == NODES) && (state_node == NODE_ID)) {
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == DEVICE_TYPE)) {
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == VENDORID)) {
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == PRODUCTCODE)) {
                state_node = NO_NODE;
            }
            else if ((state == NODES) && (state_node == REVISION)) {
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
