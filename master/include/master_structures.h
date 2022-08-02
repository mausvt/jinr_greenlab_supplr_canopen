// *** Various CANopen data structures and buffers definitions ***

union cansdob0 {        // SDO byte 0 explicit data presentation.
    struct segm {        //  For segmented/expedited transfers.
        unsigned8 ndata;        // The number of bytes in a segment that do not contain data.
        unsigned8 bit_0;        // Bit 0: Data size indication flag etc.
        unsigned8 bit_1;        // Bit 1: Expedited flag etc.
        unsigned8 toggle;        // Toggle bit value.
    } sg;
};

struct sdoixs {            // Application object
    canindex index;            // Application object dictionary index
    cansubind subind;        // and subindex
};

struct cancache {        // Data send cache
    int16 busy;                // Data busy flag, >= 0 if busy
    unsigned8 capture;        // Data capture and new data flag - to handle non-atomic data busy flag
    canframe cf;
};

struct cansdo {            // SDO data internal presentation with explicit fields
    unsigned8 cs;            // SDO command specifier - client or server
    struct sdoixs si;
    union cansdob0 b0;
    canbyte bd[8];            // CAN frame data
};

struct sdostatus {        // SDO client status codes
    int16 state;            // State during and after client transaction
    unsigned32 abortcode;    // SDO abort code
};

struct sdoclttrans {    // SDO BASIC client transactions support structure
    unsigned8 adjcs;        // Adjacent command specifier
    struct sdostatus ss;
    struct cansdo sd;
    unsigned32 rembytes;    // Remained to transfer data size
};

struct sdocltbasic {    // SDO client BASIC transactions work buffer
    int16 busy;                // Datum busy flag, >= 0 if busy
    unsigned8 capture;        // Datum capture boolean flag - to handle non-atomic datum busy flag
    unsigned32 timeout;        // Timeout counter
    struct sdoclttrans ct;
};

struct sdocltappl {        // Client application support structure
    unsigned8 operation;    // SDO transfer operation
    unsigned32 datasize;    // User data size, bytes. Needed only if datapnt != NULL.
    canbyte *datapnt;        // User data buffer pointer
    struct sdoixs si;
    struct sdostatus ss;
};

struct canopennode {        // CANopen node
    unsigned8    node_status;    // ON-OFF
    unsigned8    nmt_state;
    unsigned32    DeviceType;
    unsigned32    VendorID;
    unsigned32    ProductCode;
    unsigned32    Revision;
    unsigned32    Serial;
    unsigned16    maskdev;        // Device description mask
    unsigned32    ecpcnt;
};

struct eventlog {            // Events
    time_t ts;
    unsigned8 node;
    unsigned8 cls;
    unsigned8 type;
    unsigned8 misc;
    int16 code;
    int32 info;
};

struct eventcache {        // Event cache
    int16 busy;
    unsigned8 capture;
    struct eventlog ev;
};

union numbers {        // Objects numeric data
    int8 i8;
    unsigned8 uns8;
    int16 i16;
    unsigned16 uns16;
    int32 i32;
    unsigned32 uns32;
    int64 i64;
    unsigned64 uns64;
    real32 re32;
    real64 re64;
};
