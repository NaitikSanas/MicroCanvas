set(COMPONENT_SRCS "uCanvas2D_Display_Setup.c" "uCanvas2D_EK79007Port.c" "uCanvas2D_ST7789_Port.c")


idf_component_register(SRCS "${COMPONENT_SRCS}" 
                        INCLUDE_DIRS "." "include" 
                       PRIV_REQUIRES driver nvs_flash esp_lcd st7789 EK79007) 
