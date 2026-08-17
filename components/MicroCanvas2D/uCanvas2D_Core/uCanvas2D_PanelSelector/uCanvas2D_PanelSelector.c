#include "uCanvas2D_PanelSelector.h"
uCanvas2D_Display_Panel_t *panel = NULL;

uCanvas2D_Display_Panel_t* uCanvas_Get_Panel_Handle(uCanvas_Panel_Selection_t Panel_Name){

    if(Panel_Name == PANEL_EK79007)  {
        #ifdef CONFIG_IDF_TARGET_ESP32P4
        panel = uCanvas2D_Get_Panel_Driver_EK79007();
        panel->init(1);
        #else
        while(1){
            printf("Unsupported Panel for Current Target\r\n");
            uCanvas_Delay(500);
        }
        panel = NULL;
        #endif
        return panel;
    }

    switch (Panel_Name)
    {
    case PANEL_EK79007:{
        #ifdef CONFIG_IDF_TARGET_ESP32P4
        panel = uCanvas2D_Get_Panel_Driver_EK79007();
        panel->init(1);
        #else
        panel = NULL;
        #endif
        return panel;
        break;
    }

    case PANEL_ST7789:{
        panel = uCanvas2D_Get_Panel_Driver_ST7789();
        panel->init(1);
        return panel;
    }

    case PANEL_SSD1306_128_64:
        panel = uCanvas2D_Get_Panel_Driver_SSD1306();
        panel->init(1);
        return panel;
    default:
        break;
    }  
    return NULL; 
}