// *** CANopen macros definitions ***

#define    CLEAR_FRAME    msg_zero

#define CHECK_VERSION_CANLIB(MAJ, MIN, REL)   ((MAJ == MAJ_VERS_CANLIB) && (MIN == MIN_VERS_CANLIB) && (REL >= RELEASE_CANLIB))
#define CHECK_VERSION_APPL(MAJ, MIN, REL)   ((MAJ == MAJ_VERS_APPL) && (MIN == MIN_VERS_APPL) && (REL >= RELEASE_APPL))

#define OBJECT_DICTIONARY_TO_CANOPEN for (cnt = 0; cnt < size; cnt++) { \
        *data = *bpnt;  \
        data++; bpnt++; \
}

#define FROM_CANOPEN_TO_OBJECT_DICTIONARY for (cnt = 0; cnt < size; cnt++) { \
        *bpnt = *data;  \
        bpnt++; data++; \
}

#define CAN_CRITICAL_INIT
#define CAN_CRITICAL_BEGIN
#define CAN_CRITICAL_END
