#include "websocket_server.h"
uint8_t max_clients = 13;
struct async_resp_arg resp_arg;

void start_http_server_instance(){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 10000;
    config.task_priority = 2;
    // Start the HTTP server
    ESP_LOGI(TAG, "Starting server on port: %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK){
        return 1;
    }
    else return 0;
}

static esp_err_t handle_ws_req(httpd_req_t *req)
{
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    if (ws_pkt.len)
    {
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL)
        {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
    }
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT)
    {
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
        if(buf != NULL) free(buf);
        return ESP_OK;
    }
    return ESP_OK;
}
void create_websocket_server_uri(int start_http_server){
    if(start_http_server()){
        start_http_server_instance();
    }
    httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = handle_ws_req,
        .user_ctx   = NULL,
        .is_websocket = true
    };
    httpd_register_uri_handler(server_handle, &ws);
}

void websocket_send_task(void) {

	while (1) {      
		size_t fds = max_clients;
		int client_fds[max_clients];
		esp_err_t ret = httpd_get_client_list(http_server_handle, &fds, client_fds);
		if (ret != ESP_OK) {
			continue;
		}
        ESP_LOGI("(AC):","%d ACTIVE CLIENTS",fds);
        memset(resp_arg.data,0,6500);
		// Send data to each active WebSocket client
		for (int i = 0; i < fds; i++) {         
			int client_info = httpd_ws_get_fd_info(http_server_handle, client_fds[i]);
			if (client_info == HTTPD_WS_CLIENT_WEBSOCKET) {
				ESP_LOGI("WS","SENDING TO FD %d", client_fds[i]);
				resp_arg.hd = http_server_handle;
				resp_arg.fd = client_fds[i];
				strcpy((char*)resp_arg.data,"meow\0");
                esp_err_t err = ws_async_send_struct(&resp_arg);
				if(err != ESP_OK){
				}
			}
            else {
                ESP_LOGI("WS","NON WS CLIENT %d\r\n", client_fds[i]);
            }
		}
        printf("\r\n");
        printf("Current free heap size: %d bytes\n", esp_get_free_heap_size());
		// Wait for some time before sending the next data
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}