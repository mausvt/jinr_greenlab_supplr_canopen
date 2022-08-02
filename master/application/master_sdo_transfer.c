#include <master_header.h>

#if CHECK_VERSION_APPL(1, 1, 0)

static void set_sdo_dictionary(cannode node)
{
    write_sdo_client_data(1, ((CAN_CANID_SDO_CS_BASE+node) | CAN_MASK_COBID_INVALID)); // Entry in the object dictionary of identifiers of communication SDO objects
    write_sdo_client_data(2, (CAN_CANID_SDO_SC_BASE+node));
    write_sdo_client_data(1, (CAN_CANID_SDO_CS_BASE+node));
}

int16 read_device_object(cannode node, canindex index, cansubind subind, canbyte *data, unsigned32 datasize)
{
    struct sdocltappl ca;

    if (node < CAN_NODE_ID_MIN || node > CAN_NODE_ID_MAX) return CAN_TRANSTATE_SDO_NODE;
    set_sdo_dictionary(node);
    ca.operation = CAN_SDOPER_UPLOAD;
    ca.si.index =  index;
    ca.si.subind = subind;
    ca.datapnt = data;
    ca.datasize = datasize;
    can_sdo_client_transfer(&ca); // Data transfer between client and serverusing the SDO protocol
    if (ca.ss.state != CAN_TRANSTATE_OK) {
        node_event(node, EVENT_CLASS_NODE_SDO, EVENT_TYPE_ERROR, ca.ss.state, ca.ss.abortcode);
    }
    return ca.ss.state;
}

int16 write_device_object(cannode node, canindex index, cansubind subind, canbyte *data, unsigned32 datasize)
{
    struct sdocltappl ca;

    if (node < CAN_NODE_ID_MIN || node > CAN_NODE_ID_MAX) return CAN_TRANSTATE_SDO_NODE;
    set_sdo_dictionary(node);
    ca.operation = CAN_SDOPER_DOWNLOAD;
    ca.si.index =  index;
    ca.si.subind = subind;
    ca.datapnt = data;
    ca.datasize = datasize;
    can_sdo_client_transfer(&ca);
    if (ca.ss.state != CAN_TRANSTATE_OK) {
        node_event(node, EVENT_CLASS_NODE_SDO, EVENT_TYPE_ERROR, ca.ss.state, ca.ss.abortcode);
    }
    return ca.ss.state;
}

#endif
