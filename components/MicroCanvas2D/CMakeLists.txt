
set(COMPONENT_SRCS  "uCanvasRenderEngine.c" 
                            "vector.c" 
                            "uCanvas_api.c"
                            "uCanvas_display_port.c"
                            "uCanvas_User_IO.c"
)
# Fix cmake build

idf_component_register(SRCS "uCanvas_Physix.c" "${COMPONENT_SRCS}"
                        INCLUDE_DIRS "."
                       PRIV_REQUIRES driver SSD1306_Driver u8g2 u8g2-hal-esp-idf)
