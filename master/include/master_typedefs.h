// *** CANopen data type definitions ***

typedef signed long long        int64;
typedef unsigned long long      unsigned64;

typedef    signed char            int8;
typedef    unsigned char        unsigned8;
typedef    signed short        int16;
typedef    unsigned short        unsigned16;
typedef    signed int            int32;
typedef    unsigned int        unsigned32;
typedef    float                real32;
typedef    double                real64;

typedef    unsigned8            canbyte;    // CAN data byte
typedef    unsigned8            cannode;    // Node-ID
typedef    unsigned16            canindex;    // Object dictionary index
typedef    unsigned8            cansubind;    // Object dictionary subindex

typedef    unsigned16            canlink;    // CAN datalink identifier, v. 2.3 renamed

typedef    _s16                canev;        // CAN event
typedef    canmsg_t            canframe;    // CAN network frame

