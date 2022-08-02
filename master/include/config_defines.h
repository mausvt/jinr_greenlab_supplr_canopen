#define MASK_DEV_DEVICETYPE            0x1
#define MASK_DEV_VENDORID            0x2
#define MASK_DEV_PRODUCTCODE        0x4
#define MASK_DEV_REVISION            0x8
#define MASK_DEV_SERIAL                0x10

#define MASK_SECTION_FILENAMES        0x1
#define MASK_SECTION_CANNETWORK        0x2
#define MASK_CONFIG_SECTIONS        (MASK_SECTION_FILENAMES | MASK_SECTION_CANNETWORK)

#define MASK_TOKEN_LOGFILE            0x1
#define MASK_TOKENS_FILENAMES        (MASK_TOKEN_LOGFILE)

#define MASK_TOKEN_NETWORK            0x1
#define MASK_TOKEN_BITRATE            0x2
#define MASK_TOKENS_CANNETWORK        (MASK_TOKEN_NETWORK | MASK_TOKEN_BITRATE)

#define MASK_TOKEN_NODEID            0x1
#define MASK_TOKEN_DEVICETYPE        0x2
#define MASK_TOKEN_VENDORID            0x4
#define MASK_TOKEN_PRODCODE            0x8
#define MASK_TOKEN_REVISION            0x10
#define MASK_TOKEN_SERIAL            0x20
#define MASK_TOKENS_CANNODE            (MASK_TOKEN_NODEID | MASK_TOKEN_DEVICETYPE | MASK_TOKEN_VENDORID | \
                                     MASK_TOKEN_PRODCODE | MASK_TOKEN_REVISION | MASK_TOKEN_SERIAL)

#define CFG_SECTION_NONE            0
#define CFG_SECTION_FILENAMES        1
#define CFG_SECTION_COMMENTS        2
#define CFG_SECTION_NETWORK            3
#define CFG_SECTION_NODEID            4

#define CFG_WARNING_INV_SECTION            1    // Invalid section name - ignored
#define CFG_WARNING_INV_TOKEN            10    // Invalid token - ignored
#define CFG_WARNING_DUPL_TOKEN            11    // Duplicated CAN nodeid - discarded
#define CFG_WARNING_INV_VALUE            12    // Invalid value - ignored
#define CFG_WARNING_INV_NODE            20    // Invalid CAN nodeid - discarded
#define CFG_WARNING_DUPL_NODE            21    // Duplicated CAN nodeid - discarded
#define CFG_WARNING_UNDEF_NODE            22    // CAN nodeid not defined - discarded
#define CFG_WARNING_DUPL_FILENAMES        30    // Duplicated [Filenames] section - discarded
#define CFG_WARNING_DUPL_CANNETWORK        31    // Duplicated [CANnetwork] section - discarded

#define CFG_ERROR_VERSION                -1    // Invalid configuration file version - defaults used
#define CFG_ERROR_UNDEF_FILENAMES        -10    // Filenames section not defined - defaults used
#define CFG_ERROR_UNDEF_CANNETWORK        -11    // CANnetwork section not defined - defaults used
#define CFG_ERROR_UNDEF_LOGFILE            -20    // LogFile not defined - default used
#define CFG_ERROR_UNDEF_NETWORK            -21    // CAN network not defined - default used
#define CFG_ERROR_UNDEF_BITRATE            -22    // BitRate index not defined - default used
