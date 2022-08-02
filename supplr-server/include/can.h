#include <common.h>

typedef struct master_config_t {
    // FIXME: we pass here the path to the config file
    // but we need to parse the config and pass it already parsed
    char * can_config_path;
    int req_fd;
    int resp_fd;
} master_config_t;

void * start_master(void * _config);
