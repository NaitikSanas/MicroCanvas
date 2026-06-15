set(COMPONENT_SRCS  "uCanvas2D_Core/uCanvasRenderEngine.c" 
                    "uCanvas2D_Core/uCanvas_api.c"
                    "uCanvas2D_Core/uCanvas_Draw.c"

                    "uCanvas2D_GUI_Components/uCanvas_Slide_Switch/uCanvas_Slide_Switch.c"
                    "uCanvas2D_GUI_Components/ucanvas_slider/ucanvas_slider.c"
                    "uCanvas2D_GUI_Components/uCanvas_Graph2D/uCanvas_Graph2D.c"
                    "uCanvas2D_GUI_Components/uCanvas_Menu/uCanvas_Menu.c"

                    "uCanvas2D_Port_Layer/uCanvas2D_Display_Setup.c" 
                    "uCanvas2D_Port_Layer/uCanvas2D_EK79007Port.c" 
                    "uCanvas2D_Port_Layer/uCanvas2D_ST7789_Port.c"
                    "uCanvas2D_Port_Layer/uCanvas2D_Acceleration.c"

                    "uCanvas2D_Fonts_Collection/font8.c"
                    "uCanvas2D_Fonts_Collection/font12.c"
                    "uCanvas2D_Fonts_Collection/font16.c"
                    "uCanvas2D_Fonts_Collection/font20.c"
                    "uCanvas2D_Fonts_Collection/font24.c"
                    "uCanvas2D_Fonts_Collection/BitcountPropDouble32.c"
                    "uCanvas2D_Fonts_Collection/Roboto_Italic_32.c"
                    "uCanvas2D_Fonts_Collection/SixyFour32.c"
                    
                    "uCanvas2D_Input_Devices/USB_HID_HOST/uCanvas_HID.c"
                    "uCanvas2D_Input_Devices/uCanvas_Input_Peripherals/uCanvas_Input_Peripherals.c"

                    "uCanvas2D_PanelSelector/uCanvas2D_PanelSelector.c"

                    "uCanvas2D_Server/uCanvasServer_Core.c"
                    "uCanvas2D_Server/uCanvasServer_UART.c"

)
idf_build_get_property(idf_target IDF_TARGET)

message(STATUS "IDF target = ${idf_target}")

# Set base requirements
set(COMPONENT_REQUIRES   driver  SSD1306_Driver st7789 spiffs 
                       mpu6050_driver esp_timer esp_lcd usb esp_lcd_touch)

# Add esp_driver_ppa only for ESP32-P4
if(idf_target STREQUAL "esp32p4")
    list(APPEND COMPONENT_REQUIRES esp_driver_ppa EK79007)
endif()


idf_component_register(SRCS "${COMPONENT_SRCS}"
                       INCLUDE_DIRS "." "uCanvas2D_Core/Include" 
                                    "uCanvas2D_Port_Layer/include"                                 
                                    "uCanvas2D_GUI_Components/uCanvas_Menu" 
                                    "uCanvas2D_GUI_Components/uCanvas_Slide_Switch" 
                                    "uCanvas2D_GUI_Components/ucanvas_slider"
                                    "uCanvas2D_GUI_Components/uCanvas_Graph2D"
                                    "uCanvas2D_Port_Layer"
                                    "uCanvas2D_Input_Devices/USB_HID_HOST"
                                    "uCanvas2D_Input_Devices/uCanvas_Input_Peripherals"
                                    "uCanvas2D_PanelSelector"
                                    "uCanvas2D_Fonts_Collection"
                                    "uCanvas2D_Server/include"
                       PRIV_REQUIRES ${COMPONENT_REQUIRES})