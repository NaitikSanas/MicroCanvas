
set(COMPONENT_SRCS  "uCanvas2D_Core/uCanvasRenderEngine.c" 
                    "uCanvas2D_Core/uCanvas_api.c"
                    "uCanvas2D_Core/uCanvas_User_IO.c"
                    "uCanvas2D_Core/uCanvas_Draw.c"
                    "uCanvas2D_Core/uCanvas_Physix.c" 

                    "uCanvas2D_GUI_Components/simple_menu/simple_menu.c"
                    "uCanvas2D_GUI_Components/ucanvas_button/ucanvas_button.c"
                    "uCanvas2D_GUI_Components/ucanvas_slider/ucanvas_slider.c"

                    "Display_Driver_Port/uCanvas2D_Display_Setup.c" 
                    "Display_Driver_Port/uCanvas2D_EK79007Port.c" 
                    "Display_Driver_Port/uCanvas2D_ST7789_Port.c"
                    "Display_Driver_Port/uCanvas2D_Acceleration.c"
                    
)

idf_component_register( SRCS "${COMPONENT_SRCS}"
                        INCLUDE_DIRS "." "uCanvas2D_Core/Include" 
                                        "Display_Driver_Port/Include"
                                        "uCanvas2D_GUI_Components/simple_menu" 
                                        "uCanvas2D_GUI_Components/ucanvas_button" 
                                        "uCanvas2D_GUI_Components/ucanvas_slider"
                                        "Display_Driver_Port"
                        PRIV_REQUIRES driver EK79007 SSD1306_Driver st7789  spiffs mpu6050_driver esp_timer esp_lcd esp_driver_ppa)
