#include <master_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 0)

static void sdo_client_down_init(struct sdocltappl *ca)
{
    struct sdoclttrans ct;
    unsigned32 cnt;
    canbyte *datapnt;

    ct.sd.si = ca->si;
    ct.sd.cs = CAN_CCS_SDO_DOWNLOAD_INIT;
    ct.adjcs = CAN_SCS_SDO_DOWNLOAD_INIT;
    clear_can_data(ct.sd.bd);
    ct.sd.b0.sg.bit_0 = 1;
    if (ca->operation == CAN_SDOPER_DOWN_EXPEDITED) {
        ct.sd.b0.sg.bit_1 = 1;
        ct.sd.b0.sg.ndata = CAN_DATASEGM_EXPEDITED - ca->datasize;
        datapnt = ca->datapnt;
        for (cnt = 0; cnt < ca->datasize; cnt++) {
            ct.sd.bd[cnt] = *datapnt;
            datapnt++;
        }
    } else {
        ct.sd.b0.sg.bit_1 = 0;
        ct.sd.b0.sg.ndata = 0;
        u32_to_canframe(ca->datasize, ct.sd.bd);
    }
    can_client_basic(&ct);
    if (ct.ss.state != CAN_TRANSTATE_OK) {
        ca->ss = ct.ss;
        return;
    }
    if (ct.sd.si.index != ca->si.index || ct.sd.si.subind != ca->si.subind) {
        if (ca->operation != CAN_SDOPER_DOWN_EXPEDITED) abort_can_sdo(&ca->si, CAN_ABORT_SDO_DATAMISM);
        ca->ss.state = CAN_TRANSTATE_SDO_MPX;
    }
}

static void sdo_client_up_init(struct sdocltappl *ca)
{
    struct sdoclttrans ct;
    unsigned32 cnt;
    unsigned32 size;
    canbyte *datapnt;

    ct.sd.si = ca->si;
    ct.sd.cs = CAN_CCS_SDO_UPLOAD_INIT;
    ct.adjcs = CAN_SCS_SDO_UPLOAD_INIT;
    clear_can_data(ct.sd.bd);
    can_client_basic(&ct);
    if (ct.ss.state != CAN_TRANSTATE_OK) {
        ca->ss = ct.ss;
        return;
    }
    if (ct.sd.b0.sg.bit_1 != 0) ca->operation = CAN_SDOPER_UP_EXPEDITED;
    else ca->operation = CAN_SDOPER_UP_SEGMENTED;
    if (ct.sd.si.index != ca->si.index || ct.sd.si.subind != ca->si.subind) {
        if (ca->operation != CAN_SDOPER_UP_EXPEDITED) abort_can_sdo(&ca->si, CAN_ABORT_SDO_DATAMISM);
        ca->ss.state = CAN_TRANSTATE_SDO_MPX;
        return;
    }
    if (ca->operation == CAN_SDOPER_UP_EXPEDITED) {
        size = CAN_DATASEGM_EXPEDITED;
        if (ct.sd.b0.sg.bit_0 != 0) {
            size -= ct.sd.b0.sg.ndata;
            if (size > ca->datasize) {
                ca->ss.state = CAN_TRANSTATE_SDO_DATASIZE;
                return;
            }
        } else {
            if (size > ca->datasize) size = ca->datasize;
        }
        datapnt = ca->datapnt;
        for (cnt = 0; cnt < size; cnt++) {
            *datapnt = ct.sd.bd[cnt];
            datapnt++;
        }
        ca->datasize = size;
    } else {
        if (ct.sd.b0.sg.bit_0 != 0) {
            size = canframe_to_u32(ct.sd.bd);
            if (size > ca->datasize) {
                abort_can_sdo(&ca->si, CAN_ABORT_SDO_DATAHIGH);
                ca->ss.state = CAN_TRANSTATE_SDO_DATASIZE;
                return;
            }
            ca->datasize = size;
        }
    }
}

static void sdo_client_down_data(struct sdocltappl *ca)
{
    unsigned8 numb, toggle;
    unsigned32 cnt;
    canbyte *bufpnt;
    struct sdoclttrans ct;

    bufpnt = ca->datapnt;
    toggle = 0;
    ct.rembytes = ca->datasize;
    while (ct.rembytes > 0) {
        ct.sd.si = ca->si;
        ct.sd.cs = CAN_CCS_SDO_DOWNSEGM_DATA;
        ct.adjcs = CAN_SCS_SDO_DOWNSEGM_DATA;
        ct.sd.b0.sg.toggle = toggle;
        if (ct.rembytes > CAN_DATASEGM_OTHER) {
            ct.sd.b0.sg.bit_0 = 0;
            ct.sd.b0.sg.ndata = 0;
            numb = CAN_DATASEGM_OTHER;
            ct.rembytes -= CAN_DATASEGM_OTHER;
        } else {
            ct.sd.b0.sg.bit_0 = 1;
            ct.sd.b0.sg.ndata = CAN_DATASEGM_OTHER - ct.rembytes;
            numb = ct.rembytes;
            ct.rembytes = 0;
        }
        clear_can_data(ct.sd.bd);
        for (cnt = 0; cnt < numb; cnt++) {
            ct.sd.bd[cnt] = *bufpnt;
            bufpnt++;
        }
        can_client_basic(&ct);
        if (ct.ss.state != CAN_TRANSTATE_OK) {
            ca->ss = ct.ss;
            return;
        } else if (ct.sd.b0.sg.toggle != toggle) {
            abort_can_sdo(&ca->si, CAN_ABORT_SDO_TOGGLE);
            ca->ss.state = CAN_TRANSTATE_SDO_TOGGLE;
            return;
        }
        toggle ^= 1;
    }
}

static void sdo_client_up_data(struct sdocltappl *ca)
{
    unsigned8 numb, toggle;
    unsigned32 cnt, dsize;
    canbyte *bufpnt;
    struct sdoclttrans ct;

    bufpnt = ca->datapnt;
    toggle = 0;
    ct.rembytes = ca->datasize;
    dsize = 0;
    do {
        ct.sd.si = ca->si;
        ct.sd.cs = CAN_CCS_SDO_UPSEGM_DATA;
        ct.adjcs = CAN_SCS_SDO_UPSEGM_DATA;
        ct.sd.b0.sg.toggle = toggle;
        clear_can_data(ct.sd.bd);
        can_client_basic(&ct);
        if (ct.ss.state != CAN_TRANSTATE_OK) {
            ca->ss = ct.ss;
            break;
        } else if (ct.sd.b0.sg.toggle != toggle) {
            abort_can_sdo(&ca->si, CAN_ABORT_SDO_TOGGLE);
            ca->ss.state = CAN_TRANSTATE_SDO_TOGGLE;
            break;
        }
        if (ct.rembytes > 0) {
            numb = CAN_DATASEGM_OTHER - ct.sd.b0.sg.ndata;
            if (numb > ct.rembytes) {
                numb = ct.rembytes;
                ca->ss.state = CAN_TRANSTATE_SDO_DATASIZE;
            }
            for (cnt = 0; cnt < numb; cnt++) {
                *bufpnt = ct.sd.bd[cnt];
                bufpnt++;
            }
            ct.rembytes -= numb;
            dsize += numb;
        } else if (ca->datasize != 0) {
            abort_can_sdo(&ca->si, CAN_ABORT_SDO_DATAHIGH);
            ca->ss.state = CAN_TRANSTATE_SDO_DATASIZE;
            break;
        }
        toggle ^= 1;
    } while (ct.sd.b0.sg.bit_0 == 0);
    ca->datasize = dsize;
}

void can_sdo_client_transfer(struct sdocltappl *ca)
{
    ca->ss.state = CAN_TRANSTATE_OK;
    ca->ss.abortcode = 0;
    if (ca->datapnt == NULL || ca->datasize == 0) {
        ca->ss.state = CAN_TRANSTATE_ERROR;
        return;
    }
    if (ca->operation < CAN_SDOPER_UPLOAD) {
        if (ca->datasize <= CAN_SDOSIZE_EXPEDITED) {
            ca->operation = CAN_SDOPER_DOWN_EXPEDITED;
            sdo_client_down_init(ca);
        } else {
            ca->operation = CAN_SDOPER_DOWN_SEGMENTED;
            sdo_client_down_init(ca);
            if (ca->ss.state == CAN_TRANSTATE_OK) sdo_client_down_data(ca);
        }
    } else {
        ca->operation = CAN_SDOPER_UPLOAD;
        sdo_client_up_init(ca);
        if (ca->operation == CAN_SDOPER_UP_SEGMENTED) {
            if (ca->ss.state == CAN_TRANSTATE_OK) sdo_client_up_data(ca);
        }
    }
}

#endif
