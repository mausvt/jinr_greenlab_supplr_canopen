#include <master_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 0)

unsigned8 check_sdo_canid(cansubind subind, canlink canid)        // 2.2.1
{
    canid &= CAN_MASK_CANID;
    if (subind == 1) {
        if (canid < CAN_CANID_SDO_CS_MIN || canid > CAN_CANID_SDO_CS_MAX) return RESTRICTED;
    } else if (subind == 2) {
        if (canid < CAN_CANID_SDO_SC_MIN || canid > CAN_CANID_SDO_SC_MAX) return RESTRICTED;
    }
    return UN_RESTRICTED;
}

unsigned8 check_canid_restricted(canlink canid)        // 2.2.1
{
    canid &= CAN_MASK_CANID;
    if (canid == 0x000) return RESTRICTED;
    if (canid >= 0x001 && canid <= 0x07F) return RESTRICTED;
    if (canid >= 0x101 && canid <= 0x180) return RESTRICTED;
    if (canid >= 0x581 && canid <= 0x5FF) return RESTRICTED;
    if (canid >= 0x601 && canid <= 0x67F) return RESTRICTED;
    if (canid >= 0x6E0 && canid <= 0x6FF) return RESTRICTED;
    if (canid >= 0x701 && canid <= 0x77F) return RESTRICTED;
    if (canid >= 0x780 && canid <= 0x7FF) return RESTRICTED;
    return UN_RESTRICTED;
}

#endif
