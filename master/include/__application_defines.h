// *** The application version. ***
// All modules major and minor versions must be equal to the listed ones
// and releases not less then the listed ones.

#define MAJ_VERS_APPL        1    // Major application software version
#define MIN_VERS_APPL        1    // Minor application software version
#define RELEASE_APPL        0    // The application software release


#define CAN_NETWORK_CONTROLLER        0                            // Default CAN driver channel (CAN controller number)
#define CAN_BITRATE_INDEX            CIA_BITRATE_INDEX_500        // Default CAN network bit rate index

#define CAN_TIMERUSEC                10000    // Background timer period in microseconds (10 milliseconds).

#define CAN_TIMEOUT_RETRIEVE        1000000    // SDO client BASIC transaction data retrieve DEFAULT timeout - microseconds
#define CAN_TIMEOUT_READ            200000    // SDO client BASIC transaction data read timeout - microseconds
                                            // NO nested client transactions

#define CAN_HBT_PRODUCER_MS            1000    // Producer configured heartbeat time, ms
#define CAN_HBT_CONSUMER_MS            1200    // Consumer configured heartbeat time, ms

#define CAN_CONFIG_NODE_MS            5000    // Configure node timeout and reset if failed, ms
#define CAN_RESET_NODE_MS            10000    // Reset node period for non-responding node, ms

// FIXME: This is not needed. Left it here because it is used in logging.
#define MASTER_CONFIG_FILE_VERSION    0x00030001

#define CAN_APPLICATION_MODE MASTER
#define CAN_OBJECT_EMCY FALSE
