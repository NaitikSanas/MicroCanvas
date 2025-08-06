#include "st7789.h"
#include "esp_err.h"
#include "esp_log.h"
#include "uCanvas2D_Display_Setup.h"


uCanvas2D_Display_Panel_t* uCanvas2D_Get_Panel_Driver_ST7789(void);
void uCanvas2D_GetPanel_Driver_ST7789(uCanvas2D_Display_Panel_t* driver);