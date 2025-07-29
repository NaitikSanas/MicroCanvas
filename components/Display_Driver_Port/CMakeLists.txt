set(COMPONENT_SRCS "uCanvas2D_Display_Setup.c" 
                    "uCanvas2D_EK79007Port.c" 
                    "uCanvas2D_ST7789_Port.c"
                    "uCanvas2D_Acceleration.c"
                    
                    )


idf_component_register(SRCS "${COMPONENT_SRCS}" 
                        INCLUDE_DIRS "." "include" 
                        REQUIRES esp_driver_ppa
                       PRIV_REQUIRES driver nvs_flash esp_lcd esp_driver_ppa st7789 EK79007) 
