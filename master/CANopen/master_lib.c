#include <master_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 0)

int16 check_node_id(cannode node)    // 2.2.1
{
    if (node >= CAN_NODE_ID_MIN && node <= CAN_NODE_ID_MAX) return CAN_RETOK;
    return CAN_ERRET_NODEID;
}

int16 check_bitrate_index(unsigned8 br)        // 2.2.1
{
    if (br == CIA_BITRATE_INDEX_1000 || br == CIA_BITRATE_INDEX_800 ||
        br == CIA_BITRATE_INDEX_500 || br == CIA_BITRATE_INDEX_250 ||
        br == CIA_BITRATE_INDEX_125 || br == CIA_BITRATE_INDEX_50 ||
        br == CIA_BITRATE_INDEX_20 || br == CIA_BITRATE_INDEX_10) {
        return CAN_RETOK;
    }
    return CAN_ERRET_BITRATE;
}

void clear_can_data(canbyte *data)        // 2.3.1 some changes
{
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
}

void u16_to_canframe(unsigned16 ud, canbyte *data)        // 2.3.1 some changes
{
    canbyte *bpnt;

    bpnt = (canbyte*)&ud;
    data[0] = bpnt[0];
    data[1] = bpnt[1];
}

unsigned16 canframe_to_u16(canbyte *data)        // 2.3.1 some changes
{
    unsigned16 buf;
    canbyte *bpnt;

    bpnt = (canbyte*)&buf;
    bpnt[0] = data[0];
    bpnt[1] = data[1];
    return buf;
}

void u32_to_canframe(unsigned32 ud, canbyte *data)        // 2.3.1 some changes
{
    canbyte *bpnt;

    bpnt = (canbyte*)&ud;
    data[0] = bpnt[0];
    data[1] = bpnt[1];
    data[2] = bpnt[2];
    data[3] = bpnt[3];
}

unsigned32 canframe_to_u32(canbyte *data)        // 2.3.1 some changes
{
    unsigned32 buf;
    canbyte *bpnt;

    bpnt = (canbyte*)&buf;
    bpnt[0] = data[0];
    bpnt[1] = data[1];
    bpnt[2] = data[2];
    bpnt[3] = data[3];
    return buf;
}

#endif
