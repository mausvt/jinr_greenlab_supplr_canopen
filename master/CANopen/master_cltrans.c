#include <master_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 0)

static struct sdocltbasic cltbasic;
static int16 sem_resetcan, flag_resetcan;
static unsigned32 sdo_timeout_mcs;

// Initializing structure parameters for writing an SDO frame
static void resetcanwork(void)
{
    cltbasic.ct.ss.state = CAN_TRANSTATE_OK;
    cltbasic.ct.ss.abortcode = 0;
    cltbasic.timeout = 1 + CAN_TIMEOUT_RETRIEVE / CAN_TIMERUSEC;        // 2.2.1
    cltbasic.capture = 0;
    cltbasic.busy = -1;
}

// Checking if a frame transaction is in progress
void can_client_control(void)
{
    sem_resetcan++;
    if (sem_resetcan != 0) {
        sem_resetcan--;
        flag_resetcan++;
        return;
    }
    if (cltbasic.busy >= 0) {
        cltbasic.timeout--;
        if (cltbasic.timeout == 0) {
            if (cltbasic.ct.ss.state == CAN_TRANSTATE_SDO_WORK) {
                abort_can_sdo(&cltbasic.ct.sd.si, CAN_ABORT_SDO_TIMEOUT);
                cltbasic.ct.ss.state = CAN_TRANSTATE_SDO_NET_TIMEOUT;
                cltbasic.timeout = 1 + CAN_TIMEOUT_READ / CAN_TIMERUSEC;        // 2.2.1
            } else {
                resetcanwork();
            }
        }
    }
    sem_resetcan--;
}

// Blocking on receiving an SDO frame
static void contcan_lock(void)
{
    sem_resetcan++;
}

static void contcan_unlock(void)
{
    sem_resetcan--;
    while (flag_resetcan > 0) {
        if (sem_resetcan != -1) break;
        can_client_control();
        flag_resetcan--;
    }
}

// Checking SDO frame structure for further parsing
void can_client_sdo(canframe *cf)
{
    struct cansdo sd;

    contcan_lock();
    if (cltbasic.ct.ss.state == CAN_TRANSTATE_SDO_WORK) {
        parse_sdo(&sd, cf->data); // Some *Magic*
        if (sd.cs == CAN_CS_SDO_ABORT) {
            cltbasic.ct.ss.abortcode = canframe_to_u32(sd.bd);
            cltbasic.ct.ss.state = CAN_TRANSTATE_SDO_SRVABORT;
        } else if (sd.cs == cltbasic.ct.adjcs) {
            cltbasic.ct.sd = sd;
            cltbasic.ct.ss.state = CAN_TRANSTATE_OK;
        } else {
            abort_can_sdo(&cltbasic.ct.sd.si, CAN_ABORT_SDO_CS);
            cltbasic.ct.ss.state = CAN_TRANSTATE_SDO_SCSERR;
        }
        cltbasic.timeout = 1 + CAN_TIMEOUT_READ / CAN_TIMERUSEC;        // 2.2.1
    }
    contcan_unlock();
}

// Checking if the CAN bus is busy or free
static void readcbas(struct sdoclttrans *ct)
{
    if (cltbasic.busy < 0) {
        ct->ss.state = CAN_TRANSTATE_SDO_READ_TIMEOUT;
        return;
    }
    contcan_lock();
    if (cltbasic.ct.ss.state != CAN_TRANSTATE_SDO_WORK) {
        *ct = cltbasic.ct;
        resetcanwork();
    }
    contcan_unlock();
}

// Client transfer request
static void request_client_trans(struct sdoclttrans *ct)
{
    ct->ss.state = CAN_TRANSTATE_SDO_WORK;
    ct->ss.abortcode = 0;
    contcan_lock();
    CAN_CRITICAL_BEGIN // ???
    cltbasic.busy++;
    if (cltbasic.busy == 0) {
        CAN_CRITICAL_END // ???
        if (cltbasic.capture == 0) {
            cltbasic.capture = 1;
            cltbasic.ct = *ct;
            cltbasic.ct.ss.state = CAN_TRANSTATE_SDO_WORK;
            if (send_can_sdo(&ct->sd) != CAN_RETOK) {
                ct->ss.state = CAN_TRANSTATE_SDO_WRITERR;
                resetcanwork();
            }
        }
    } else {
        cltbasic.busy--;
        CAN_CRITICAL_END
        ct->ss.state = CAN_TRANSTATE_SDO_NOWORKB;
    }
    contcan_unlock();
}

// Processing a customer request
void can_client_basic(struct sdoclttrans *ct)
{
    unsigned32 tout;

    request_client_trans(ct);
    if (ct->ss.state != CAN_TRANSTATE_SDO_WORK) return;
    cltbasic.timeout = 1 + sdo_timeout_mcs / CAN_TIMERUSEC;
    tout = 2 * (1 + sdo_timeout_mcs / CAN_SLEEP_READ);            // 2.2.1
    while (tout > 0) {
        can_sleep(CAN_SLEEP_READ);
        readcbas(ct);
        if (ct->ss.state != CAN_TRANSTATE_SDO_WORK) return;
        tout--;
    }
    ct->ss.state = CAN_TRANSTATE_SDO_TRANS_TIMEOUT;
}

void set_sdo_timeout(unsigned32 microseconds)
{
    if (microseconds < CAN_TIMERUSEC) sdo_timeout_mcs = CAN_TIMERUSEC;
    else sdo_timeout_mcs = microseconds;
}

unsigned32 get_sdo_timeout(void)
{
    return sdo_timeout_mcs;
}

void can_init_client(void)
{
    sem_resetcan = 0;
    flag_resetcan = 0;
    resetcanwork();
    sdo_timeout_mcs = CAN_TIMEOUT_RETRIEVE;
    sem_resetcan = -1;
}

#endif
