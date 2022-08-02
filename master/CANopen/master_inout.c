#include <master_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 2)

static struct cancache cancache[CAN_WRITECACHE_SIZE];
static int16 sem_canrecv, flag_canrecv;
static int16 sem_cantrans, flag_cantrans, sem_overflow;
static unsigned8 datalink_mode;

// Sending data from the buffer
void push_all_can_data(void)
{
    unsigned16 cnt;

    do {
        flag_cantrans = 0;
        sem_cantrans++;
        if (sem_cantrans == 0) {
            for (cnt = 0; cnt < CAN_WRITECACHE_SIZE; cnt++) {
                if (flag_cantrans != 0) break;
                if (cancache[cnt].capture != 255) continue;        // 2.2.2
                if (CiWrite(can_network, &cancache[cnt].cf, 1) <= 0) break;
                cancache[cnt].capture = 0;
                cancache[cnt].busy = -1;
            }
        }
        sem_cantrans--;
    } while (flag_cantrans != 0);
    flag_cantrans = 1;
}


int16 send_can_data(canframe *cf)        // 3.0.1 API changed
{
    unsigned16 cnt, cabg;
    int16 wrac;

    if (cf->id <= CAN_ID_PRIORITY_0) cabg = 0;          // 3.0.1 priority conditions changed
    else if (cf->id <= CAN_ID_PRIORITY_1) cabg = 1;
    else if (cf->id <= CAN_ID_PRIORITY_2) cabg = 2;
    else cabg = 4;
    wrac = CAN_WRITECACHE_ATTEMPTS;
    do {
        sem_cantrans++;
        for (cnt = cabg; cnt < CAN_WRITECACHE_SIZE; cnt++) {
            cancache[cnt].busy++;
            if (cancache[cnt].busy == 0) {
                if (cancache[cnt].capture == 0) {
                    cancache[cnt].capture = 1;
                    cancache[cnt].cf = *cf;
                    cancache[cnt].capture = 255;        // 2.2.2
                    sem_cantrans--;
                    push_all_can_data();
                    return CAN_RETOK;
                }
            } else {
                cancache[cnt].busy--;
            }
        }
        sem_cantrans--;
        if (sem_cantrans != -1) break;
        push_all_can_data();
        can_sleep(CAN_WRITECACHE_DELAY);
        wrac--;
    } while (wrac > 0);
    sem_overflow++;
    if (sem_overflow == 0) {
        can_cache_overflow();
    }
    sem_overflow--;
    return CAN_ERRET_COMM_SEND;
}

// Checking the received frame
static void receive_can_data(canframe *cf)
{
    canlink canid;

    if ((cf->flags & CAN_FLAG_EFF) != 0) return;
    if ((cf->flags & CAN_FLAG_RTR) != 0) return;
    if (cf->len > CAN_DATALEN_MAXIMUM) cf->len = CAN_DATALEN_MAXIMUM;
    cf->id &= CAN_MASK_CANID;
    if (cf->id >= CAN_CANID_NMT_ERROR_MIN && cf->id <= CAN_CANID_NMT_ERROR_MAX) {
        if (cf->len != CAN_DATALEN_ECP) return;        // 2.2.2
        consume_nmt(cf);
        return;
    }
    if (cf->id >= CAN_CANID_EMCY_MIN && cf->id <= CAN_CANID_EMCY_MAX) {
        if (cf->len != CAN_DATALEN_EMCY) return;    // 2.2.2
        consume_emcy(cf);
        return;
    }
    if (find_sync_recv_canid(&canid) == CAN_RETOK) {
        if (cf->id == canid) {
            sync_received(cf);
            return;
        }
    }
    if (cf->id >= CAN_CANID_PDO1_TRN_MIN && cf->id <= CAN_CANID_PDO4_RCV_MAX) {        // All IDs - may be re-configured
        receive_can_pdo(cf);
        return;
    }
    if (cf->id >= CAN_CANID_SDO_SC_MIN && cf->id <= CAN_CANID_SDO_SC_MAX) {
        if (cf->len != CAN_DATALEN_SDO) return;        // 2.2.2
        if (find_sdo_client_recv_canid(&canid) == CAN_RETOK) {
            if (cf->id == canid) can_client_sdo(cf);

        }
        return;
    }
}

// The function of reading frames from the CAN network
void can_read_handler(canev ev)
{
    canframe cf;

    do {
        flag_canrecv = 0;
        sem_canrecv++;
        if (sem_canrecv == 0) {
            while (CiRead(can_network, &cf, 1) > 0) receive_can_data(&cf);
        }
        sem_canrecv--;
    } while (flag_canrecv != 0);
    flag_canrecv = 1;
}

void can_init_io(void)
{
    unsigned16 cnt;

    sem_canrecv = -1;
    sem_cantrans = 0;
    for (cnt = 0; cnt < CAN_WRITECACHE_SIZE; cnt++) {
        cancache[cnt].capture = 0;
        cancache[cnt].busy = -1;
    }
    sem_cantrans = -1;
    sem_overflow = -1;
    datalink_mode = ON;
}

#endif
