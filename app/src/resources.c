#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>

#include <app/headers.h>
#include <app/index_html.h>

#define MAX_BUF_LEN 256

static char response_buf[MAX_BUF_LEN];
int16_t temp_c = 0;
//const int my_http_resources_anchor __attribute__((used)) = 1;

/* HTTP response handler*/
/*Called every time a browser hits "/".
It responds with a simple webpage showing the temperature using http_response_body_set(). */
/* ========== HTTP Handler ========== */
/* HTTP Handler */
int temp_handler(struct http_client_ctx *client,
                        enum http_data_status status,
                        const struct http_request_ctx *req,
                        struct http_response_ctx *rsp,
                        void *user_data)
{
    sprintf(response_buf,"%dC\n",temp_c);
    //static const char *response = "Current temperature is 0C";
    rsp->status = 200; // HTTP 200 OK
    rsp->body = (const uint8_t *)response_buf;
    rsp->body_len = strlen(response_buf);
    rsp->final_chunk = true;
    return 0;
}

/*static*/ struct http_resource_detail_dynamic temp_resource_detail = {
    .common = {
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
        .type = HTTP_RESOURCE_TYPE_DYNAMIC,
        .path_len = sizeof("/temp") - 1,
        .content_encoding = NULL,
        .content_type = "text/plain",
    },
    .cb = temp_handler,
    .holder = NULL,
    .user_data = NULL,
};

/*********************************************************************************************/
//displays the static HTML page whenever resource "/" is accessed in the browser
/*static*/ struct http_resource_detail_static index_resource_detail = {
    .common = {
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
        .type = HTTP_RESOURCE_TYPE_STATIC,
        .path_len = sizeof("/") - 1,
        .content_encoding = NULL,
        .content_type = "text/html",
    },
    .static_data = index_html,
    .static_data_len = index_html_len,
};

/*********************************************************************************************/

//HTTP_RESOURCE_DEFINE(temp, my_http_service, "/temp", &temp_resource_detail);