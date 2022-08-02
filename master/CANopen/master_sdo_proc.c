#include <master_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 1)

void parse_sdo(struct cansdo *sd, canbyte *data)
{
    sd->si.index = 0;
    sd->si.subind = 0;
    clear_can_data(sd->bd);
    sd->cs = (*data >> 5) & CAN_MASK_SDO_CS;
    sd->b0.sg.ndata = 0;
    sd->b0.sg.bit_0 = 0;
    sd->b0.sg.bit_1 = 0;
    sd->b0.sg.toggle = 0;
    if (sd->cs == CAN_SCS_SDO_UPLOAD_INIT) {
        sd->b0.sg.bit_1 = (*data >> 1) & 01;
        sd->b0.sg.bit_0 = *data & 01;
        if (sd->b0.sg.bit_0 != 0) sd->b0.sg.ndata = (*data >> 2) & 03;
    } else if (sd->cs == CAN_SCS_SDO_UPSEGM_DATA) {
        sd->b0.sg.bit_0 = *data & 01;
        sd->b0.sg.ndata = (*data >> 1) & 07;
        sd->b0.sg.toggle = (*data >> 4) & 01;
    } else if (sd->cs == CAN_SCS_SDO_DOWNSEGM_DATA) {
        sd->b0.sg.toggle = (*data >> 4) & 01;
    }
    data++;
    if (sd->cs == CAN_SCS_SDO_DOWNSEGM_DATA || sd->cs == CAN_SCS_SDO_UPSEGM_DATA) {
        sd->bd[0] = *data; data++;
        sd->bd[1] = *data; data++;
        sd->bd[2] = *data; data++;
        sd->bd[3] = *data; data++;
        sd->bd[4] = *data; data++;
        sd->bd[5] = *data; data++;
        sd->bd[6] = *data;
    } else {
        sd->si.index = canframe_to_u16(data);
        data++; data++;
        sd->si.subind = *data; data++;
        sd->bd[0] = *data; data++;
        sd->bd[1] = *data; data++;
        sd->bd[2] = *data; data++;
        sd->bd[3] = *data;
    }
}

static void assemble_sdo(struct cansdo *sd, canbyte *data)
{
    *data = (sd->cs & CAN_MASK_SDO_CS) << 5;
    if (sd->cs == CAN_CCS_SDO_DOWNLOAD_INIT) {
        *data |= ((sd->b0.sg.ndata & 03) << 2) | ((sd->b0.sg.bit_1 & 01) << 1) | (sd->b0.sg.bit_0 & 01);
    } else if (sd->cs == CAN_CCS_SDO_DOWNSEGM_DATA) {
        *data |= ((sd->b0.sg.toggle & 01) << 4) | ((sd->b0.sg.ndata & 07) << 1) | (sd->b0.sg.bit_0 & 01);
    } else if (sd->cs == CAN_CCS_SDO_UPSEGM_DATA) {
        *data |= (sd->b0.sg.toggle & 01) << 4;
    }
    data++;
    if (sd->cs == CAN_CCS_SDO_DOWNSEGM_DATA || sd->cs == CAN_CCS_SDO_UPSEGM_DATA) {
        *data = sd->bd[0]; data++;
        *data = sd->bd[1]; data++;
        *data = sd->bd[2]; data++;
        *data = sd->bd[3]; data++;
        *data = sd->bd[4]; data++;
        *data = sd->bd[5]; data++;
        *data = sd->bd[6];
    } else {
        u16_to_canframe(sd->si.index, data);
        data++; data++;
        *data = sd->si.subind; data++;
        *data = sd->bd[0]; data++;
        *data = sd->bd[1]; data++;
        *data = sd->bd[2]; data++;
        *data = sd->bd[3];
    }
}

int16 send_can_sdo(struct cansdo *sd)
{
    int16 fnr;
    canlink canid;
    canframe cf;

    msg_zero(&cf);
    fnr = find_sdo_client_send_canid(&canid);
    if (fnr != CAN_RETOK) return fnr;
    cf.id = canid;
    assemble_sdo(sd, cf.data);
    cf.len = CAN_DATALEN_SDO;
    return send_can_data(&cf);
}

void abort_can_sdo(struct sdoixs *si, unsigned32 abortcode)
{
    struct cansdo sd;

    sd.cs = CAN_CS_SDO_ABORT;
    sd.si = *si;
    u32_to_canframe(abortcode, sd.bd);
    send_can_sdo(&sd);
}

#endif
