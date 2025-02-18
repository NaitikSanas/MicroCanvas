#include "stdio.h"
#include "esp_http_server.h"

struct async_resp_arg {
    uint32_t len;
    httpd_handle_t hd;
    int fd;
    uint8_t data[6500];
};
