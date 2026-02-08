set(COMPONENT_SRCS  "uCanvas2D_Core/uCanvasRenderEngine.c" 
                    "uCanvas2D_Core/uCanvas_api.c"
                    "uCanvas2D_Core/uCanvas_User_IO.c"
                    "uCanvas2D_Core/uCanvas_Draw.c"
                    "uCanvas2D_Core/uCanvas_Physix.c" 
                    "uCanvas2D_Core/uCanvas_Fonts.c" 

                    "uCanvas2D_GUI_Components/simple_menu/simple_menu.c"
                    "uCanvas2D_GUI_Components/uCanvas_Slide_Switch/uCanvas_Slide_Switch.c"
                    "uCanvas2D_GUI_Components/ucanvas_slider/ucanvas_slider.c"
                    "uCanvas2D_GUI_Components/uCanvas_Graph2D/uCanvas_Graph2D.c"

                    "Display_Driver_Port/uCanvas2D_Display_Setup.c" 
                    "Display_Driver_Port/uCanvas2D_EK79007Port.c" 
                    "Display_Driver_Port/uCanvas2D_ST7789_Port.c"
                    "Display_Driver_Port/uCanvas2D_Acceleration.c"

                    "Fonts/font8.c"
                    "Fonts/font12.c"
                    "Fonts/font16.c"
                    "Fonts/font20.c"
                    "Fonts/font24.c"
                    "Fonts/BitcountPropDouble32.c"
                    "Fonts/Roboto_Italic_32.c"
                    "Fonts/SixyFour32.c"
                    
                    "Input_Devices/USB_HID/uCanvas_HID.c"
)
idf_build_get_property(idf_target IDF_TARGET)

message(STATUS "IDF target = ${idf_target}")

# Set base requirements
set(COMPONENT_REQUIRES  driver  SSD1306_Driver st7789 spiffs 
                       mpu6050_driver esp_timer esp_lcd usb esp_lcd_touch)

# Add esp_driver_ppa only for ESP32-P4
if(idf_target STREQUAL "esp32p4")
    list(APPEND COMPONENT_REQUIRES esp_driver_ppa EK79007)
endif()


idf_component_register(SRCS "${COMPONENT_SRCS}"
                       INCLUDE_DIRS "." "uCanvas2D_Core/Include" 
                                    "Display_Driver_Port/include"
                                    "uCanvas2D_GUI_Components/simple_menu" 
                                    "uCanvas2D_GUI_Components/uCanvas_Slide_Switch" 
                                    "uCanvas2D_GUI_Components/ucanvas_slider"
                                    "uCanvas2D_GUI_Components/uCanvas_Graph2D"
                                    "Display_Driver_Port"
                                    "Input_Devices/USB_HID"
                                    
                                    "Fonts"
                       PRIV_REQUIRES ${COMPONENT_REQUIRES})