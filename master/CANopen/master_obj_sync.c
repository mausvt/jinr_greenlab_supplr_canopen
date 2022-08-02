#include <master_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 1)

#define MASK_SYNC_GENERATE        0x40000000    // SYNC message generate bit mask

static unsigned32 cobidsync;
static unsigned32 period;
static unsigned32 window, wincnt;
static unsigned32 ticktot, tickcnt;
static unsigned8 syncover, synccnt;

static void reset_sync(void)
{
    unsigned32 ttot;

    ticktot = 0;
    tickcnt = 0xFFFFFFFF;
    if (period == 0) {
        synccnt = CAN_SYNC_COUNTER_MIN;
        return;
    }
    ttot = period / CAN_TIMERUSEC;
    if (ttot == 0) ttot = 1;
    if ( (cobidsync & MASK_SYNC_GENERATE) != 0 ) {
        tickcnt = 1;
    } else {
        ttot += 1;
        tickcnt = ttot;
    }
    ticktot = ttot;
}

// Create sync obj
int16 find_sync_recv_canid(canlink *canid)
{
    *canid = cobidsync & CAN_MASK_CANID;
    return CAN_RETOK;
}

int16 read_sync_object(canindex index, unsigned32 *data)
{
    *data = 0;
    if (index == CAN_INDEX_SYNC_COBID) *data = cobidsync;
    else if (index == CAN_INDEX_SYNC_PERIOD) *data = period;
    else if (index == CAN_INDEX_SYNC_WINDOW) *data = window;
    else if (index == CAN_INDEX_SYNC_OVERFLOW) *data = syncover;
    else return CAN_ERRET_OBD_NOOBJECT;
    return CAN_RETOK;
}

int16 write_sync_object(canindex index, unsigned32 data)
{
    if (index == CAN_INDEX_SYNC_COBID) {
        if ( (cobidsync & MASK_SYNC_GENERATE) != 0 && (data & MASK_SYNC_GENERATE) != 0 ) {
            return CAN_ERRET_OBD_OBJACCESS;
        }
        if ((data & CAN_MASK_IDSIZE) != 0) return CAN_ERRET_OBD_VALRANGE;
        if (check_canid_restricted((canlink)data)    == RESTRICTED) return CAN_ERRET_OBD_VALRANGE;    // 2.2.1
        cobidsync = data & (MASK_SYNC_GENERATE | CAN_MASK_CANID);
        synccnt = CAN_SYNC_COUNTER_MIN;
        reset_sync();
    } else if (index == CAN_INDEX_SYNC_PERIOD) {
        period = data;
        reset_sync();
    } else if (index == CAN_INDEX_SYNC_WINDOW) {
        window = data;
    } else if (index == CAN_INDEX_SYNC_OVERFLOW) {
        if (ticktot != 0) return CAN_ERRET_OBD_DEVSTATE;
        if (data == 0 || (data >= CAN_SYNC_COUNTER_MIN+1 && data <= CAN_SYNC_COUNTER_MAX)) {
            syncover = (unsigned8)data;
            synccnt = CAN_SYNC_COUNTER_MIN;
        } else return CAN_ERRET_OBD_VALRANGE;
    } else return CAN_ERRET_OBD_NOOBJECT;
    return CAN_RETOK;
}

unsigned8 sync_window_expired(void)
{
    if (window == 0) return FALSE; // Window sync open
    if (wincnt == 0) return TRUE;  // Window sync close
    return FALSE;
}

void sync_received(canframe *cf)
{
    unsigned8 sc;

    if (syncover > CAN_SYNC_COUNTER_MIN) {
        if (cf->len != CAN_DATALEN_SYNC_COUNTER) {
            master_emcy(CAN_EMCY_SYNCLEN); // logging
            return;
        }
        sc = cf->data[0];
        if (sc < CAN_SYNC_COUNTER_MIN || sc > CAN_SYNC_COUNTER_MAX) return;
    } else {
        if (cf->len != CAN_DATALEN_SYNC) {
            master_emcy(CAN_EMCY_SYNCLEN);
            return;
        }
        sc = 0;
    }
    tickcnt = ticktot;
    wincnt = 1 + window / CAN_TIMERUSEC;
    process_sync_pdo(sc); // Empty func ???
    consume_sync(sc); // Empty func ???
}

// Sync management
void control_sync(void)
{
    canframe cf;

    if (wincnt > 0) wincnt--;
    if (ticktot == 0) return;
    if (tickcnt == 0) return;
    tickcnt--;
    if (tickcnt != 0) return;
    if ( (cobidsync & MASK_SYNC_GENERATE) != 0 ) {
        CLEAR_FRAME(&cf);
        cf.id = cobidsync & CAN_MASK_CANID;
        if (syncover > CAN_SYNC_COUNTER_MIN) {
            cf.data[0] = synccnt;
            synccnt++;
            if (synccnt > syncover) synccnt = CAN_SYNC_COUNTER_MIN;
            cf.len = CAN_DATALEN_SYNC_COUNTER;
        } else {
            cf.len = CAN_DATALEN_SYNC;
        }
        send_can_data(&cf);
        sync_received(&cf);
    } else {
        tickcnt = ticktot;
        no_sync_event(); // logging
    }
}

void can_init_sync(void)
{
    cobidsync = CAN_CANID_SYNC;
    window = 0;
    wincnt = 0;
    period = 0;
    reset_sync();
    syncover = 0;
}

#endif
