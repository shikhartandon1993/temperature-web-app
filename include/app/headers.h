#ifndef MY_HEADERS_H
#define MY_HEADERS_H

#include <zephyr/device.h>
#include <zephyr/toolchain.h>

extern int temp_handler(struct http_client_ctx *, enum http_data_status,
                        const struct http_request_ctx *, struct http_response_ctx *, void *);

extern int16_t temp_c;
extern struct http_resource_detail_dynamic temp_resource_detail;
extern struct http_resource_detail_static index_resource_detail;

#endif