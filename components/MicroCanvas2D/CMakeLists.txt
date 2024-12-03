
set(COMPONENT_SRCS  "uCanvasRenderEngine.c" 
                            "vector.c" 
                            "uCanvas_api.c"
                            "uCanvas_display_port.c"
                            "uCanvas_User_IO.c"
                            "simple_menu/simple_menu.c"
                            "ucanvas_button/ucanvas_button.c"
                            "ucanvas_slider/ucanvas_slider.c"
)
# Fix cmake build

idf_component_register(SRCS "uCanvas_Frame_buffer.c" "uCanvas_draw.c" "uCanvas_Physix.c" "${COMPONENT_SRCS}"
                        INCLUDE_DIRS "." "simple_menu" "ucanvas_button" "ucanvas_slider"
                       PRIV_REQUIRES driver SSD1306_Driver u8g2 u8g2-hal-esp-idf)
