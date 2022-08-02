#define EVENT_CACHE_SIZE                5        // 4 + cache overflow
#define EVENT_FIFO_SIZE                    101        // min 5, max 64884


#define EVENT_NODE_MASTER                0


#define EVENT_CLASS_DUMMY                0

#define EVENT_CLASS_COMMON_MIN            1
#define EVENT_CLASS_COMMON_MAX            99
#define EVENT_CLASS_NODE_MIN            100
#define EVENT_CLASS_NODE_MAX            199
#define EVENT_CLASS_MASTER_MIN            200
#define EVENT_CLASS_MASTER_MAX            255

#define EVENT_CLASS_EMCY                1
#define EVENT_CLASS_SYNC                2

#define EVENT_CLASS_NODE_STATE            100
#define EVENT_CLASS_NODE_CONFIG            110
#define EVENT_CLASS_NODE_SDO            120

#define EVENT_CLASS_MASTER_STATUS        200
#define EVENT_CLASS_MASTER_CONFIG        201
#define EVENT_CLASS_MASTER_LOGGER        210
#define EVENT_CLASS_MASTER_CHAI            211
#define EVENT_CLASS_MASTER_TIMER        212


#define EVENT_TYPE_DUMMY                0
#define EVENT_TYPE_INFO                    1
#define EVENT_TYPE_WARNING                2
#define EVENT_TYPE_ERROR                3
#define EVENT_TYPE_FATAL                4
#define EVENT_TYPE_DEBUG                10


#define EVENT_CODE_DUMMY                0

// EVENT_CLASS_SYNC
#define EVENT_CODE_NO_SYNC                1

// EVENT_CLASS_NODE_STATE
#define EVENT_CODE_NODE_RESET            1
#define EVENT_CODE_NODE_BOOTUP            2
#define EVENT_CODE_NODE_OPERATIONAL        3
#define EVENT_CODE_NODE_HEARTBEAT        10
#define EVENT_CODE_NODE_UNDEFINED        20
#define EVENT_CODE_NODE_NOTWORKING        25

// EVENT_CLASS_NODE_CONFIG
#define EVENT_CODE_NODE_CONFIGURED        1
#define EVENT_CODE_DEVICE_TYPE            10
#define EVENT_CODE_VENDOR_ID            11
#define EVENT_CODE_PRODUCT_CODE            12
#define EVENT_CODE_REVISION                13
#define EVENT_CODE_SERIAL                14
#define EVENT_CODE_PROD_HBT                20

// EVENT_CLASS_MASTER_STATUS
#define EVENT_CODE_MASTER_RUNNING        1
#define EVENT_CODE_INVALID_CONFIG        10

// EVENT_CLASS_MASTER_LOGGER
#define EVENT_CODE_CACHE_OVERFLOW        30000
#define EVENT_CODE_FIFO_OVERFLOW        30001
#define EVENT_CODE_LOGGER_FOPEN            30002

// EVENT_CLASS_MASTER_TIMER
#define EVENT_CODE_LOW_PERIOD            30020
#define EVENT_CODE_NO_CREATE            30021
#define EVENT_CODE_NO_SET                30022

// GENERIC
#define EVENT_CODE_FILE_OPEN            31000


#define EVENT_INFO_DUMMY                0
