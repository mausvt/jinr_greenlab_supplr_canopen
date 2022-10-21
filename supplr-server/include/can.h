#include <common.h>

typedef struct master_config_t {
    const char * can_config_path;
    int req_fd;
    int resp_fd;
} master_config_t;

void * start_master(void * _config);
