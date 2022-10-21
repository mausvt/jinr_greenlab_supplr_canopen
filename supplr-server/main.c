#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <common.h>
#include <http.h>
#include <can.h>
#include <master_header.h>
#include "../master/application/port_parser.c"


int main(int argc, char** argv)
{
    openlog("supplr-log", LOG_CONS, LOG_USER);
    struct configuration *stConfig = {NULL};
    if (!(stConfig = clparser(argc, argv)))
    {
        fprintf(stderr, "Error while retrieving of cli options\n");
        exit(EXIT_FAILURE);
    }
    stConfig->port = port_parser(stConfig->path);
    int req_pipe[2];
    int resp_pipe[2];
    int ret;

    pthread_t http_server;

    if (pipe2(req_pipe, O_NONBLOCK) < 0) exit(EXIT_FAILURE);
    if (pipe2(resp_pipe, O_NONBLOCK) < 0) exit(EXIT_FAILURE);

    // can master reads from req pipe and writes to resp pipe
    master_config_t master_config = { stConfig->path, req_pipe[0], resp_pipe[1] };
    // http server writes to req pipe and reads from resp pipe
    http_config_t http_config = { stConfig->port, req_pipe[1], resp_pipe[0] };
    // exit(-1);
    printf("Starting http server thread\n");
    ret = pthread_create(&http_server, NULL, start_http_server, &http_config);
    if (ret < 0) {
        fprintf(stderr, "Error while starting http server thread\n");
        exit(EXIT_FAILURE);
    }

    start_master(&master_config);

    pthread_join(http_server, NULL);
    return EXIT_SUCCESS;
}
