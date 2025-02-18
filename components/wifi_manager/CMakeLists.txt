
set(COMPONENT_SRCS  "wifi_manager.c"
)
# Fix cmake build

idf_component_register(SRCS "${COMPONENT_SRCS}"
                        INCLUDE_DIRS "." 
                       PRIV_REQUIRES driver esp_wifi esp_http_server nvs_flash)
