#ifndef HTTP_H
#define HTTP_H


typedef struct http_config_t {
    int port;
    int req_fd;
    int resp_fd;
} http_config_t;

struct configuration
{
  const char *path;
  int port;
};

void * start_http_server(void * _config);
#endif
