#include <master_header.h>

#if CHECK_VERSION_APPL(1, 1, 0)

static void size_mask(unsigned32 *data, int16 bitlen)
{
    unsigned16 cnt, ind;
    unsigned32 lm[2];

    lm[1] = 0;
    if (bitlen <= 32) {
        lm[0] = 0;
        ind = 0;
    } else {
        lm[0] = 0xFFFFFFFF;
        ind = 1;
        bitlen -= 32;
    }
    for (cnt = 0; cnt < bitlen; cnt++) lm[ind] = (lm[ind] << 1) | 1;
    data[0] &= lm[0];
    data[1] &= lm[1];
}

static int16 map_pdo_application(cannode node, unsigned8 pn, canframe *cf)
{
    return CAN_RETOK;
}

static int16 activate_pdo_application(cannode node, unsigned8 pn, canframe *cf)
{
    return CAN_RETOK;
}

int16 transmit_can_pdo(cannode node, unsigned8 pn)
{
    int16 fnr;
    canframe cf;

    CLEAR_FRAME(&cf);
    fnr = map_pdo_application(node, pn, &cf);
    if (fnr != CAN_RETOK) return fnr;
    if (pn == 1) cf.id = CAN_CANID_PDO1_RCV_MIN;
    else if (pn == 2) cf.id = CAN_CANID_PDO2_RCV_MIN;
    else if (pn == 3) cf.id = CAN_CANID_PDO3_RCV_MIN;
    else if (pn == 4) cf.id = CAN_CANID_PDO4_RCV_MIN;
    else return CAN_ERRET_OBD_NOOBJECT;
    cf.id += node-1;
    return send_can_data(&cf);
}

void receive_can_pdo(canframe *cf)
{
    if (cf->id >= CAN_CANID_PDO1_TRN_MIN && cf->id <= CAN_CANID_PDO1_TRN_MAX) {
        activate_pdo_application(cf->id-CAN_CANID_PDO1_TRN_MIN+1, 1, cf);
    } else if (cf->id >= CAN_CANID_PDO2_TRN_MIN && cf->id <= CAN_CANID_PDO2_TRN_MAX) {
        activate_pdo_application(cf->id-CAN_CANID_PDO2_TRN_MIN+1, 2, cf);
    } else if (cf->id >= CAN_CANID_PDO3_TRN_MIN && cf->id <= CAN_CANID_PDO3_TRN_MAX) {
        activate_pdo_application(cf->id-CAN_CANID_PDO3_TRN_MIN+1, 3, cf);
    } else if (cf->id >= CAN_CANID_PDO4_TRN_MIN && cf->id <= CAN_CANID_PDO4_TRN_MAX) {
        activate_pdo_application(cf->id-CAN_CANID_PDO4_TRN_MIN+1, 4, cf);
    }
}

void process_sync_pdo(unsigned8 sc)
{
}

void can_init_pdo(void)
{
}

#endif
