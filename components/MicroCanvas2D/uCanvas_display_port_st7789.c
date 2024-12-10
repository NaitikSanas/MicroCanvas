#include "uCanvas_display_port.h"
#ifdef USE_ST7789
#include "st7789.h"
#include "fontx.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_log.h"
 TFT_t dev;



uint16_t convertToRGB565(color_t color) {
    // Scale down the RGB values to fit in 5, 6, and 5 bits respectively
    uint16_t red = (color.red * 31) / 255;
    uint16_t green = (color.green * 63) / 255;
    uint16_t blue = (color.blue * 31) / 255;

    // Combine into RGB565 format
    uint16_t rgb565 = (red << 11) | (green << 5) | blue;
    return rgb565;
}
FontxFile fx16G[2];
FontxFile fx24G[2];
FontxFile fx32G[2];
FontxFile fx32L[2];
FontxFile fx16M[2];
FontxFile fx24M[2];
FontxFile fx32M[2];
static const char *TAG = "ST7789";

static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}
void uCanvas_Display_init(void){
    // set font file
	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 12,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total,&used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	SPIFFS_Directory("/spiffs/");
	InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
	InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic
	InitFontx(fx32L,"/spiffs/LATIN32B.FNT",""); // 16x32Dot Latin

	
	InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
	InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
	InitFontx(fx32M,"/spiffs/FONT10X20.FNT",""); // 16x32Dot Mincyo
	
	// spi_clock_speed(40000000); // 40MHz
	// spi_clock_speed(60 000 000); // 60MHz
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
	
	// spi_master_init(&dev, 5, 4, -1, 7, 6, 8);
	lcdInit(&dev, 240, 320, 0, 0);
}


uint16_t fill_color;

void uCanvas_Display_clear_buffer(void){
    lcdFillScreen(&dev,fill_color);
}


void uCanvas_Update_Display(void){
    lcdDrawFinish(&dev);
}

void uCanvas_Fill_screen(color_t color){
	fill_color = convertToRGB565(color);
	//lcdFillScreen(&dev,convertToRGB565(color));
}

void uCanvas_Draw_Rectangle(int x, int y, int h, int w, color_t color, fill_t fill){
    uint16_t color565 = convertToRGB565(color);
	if(fill==NOFILL) lcdDrawRect(&dev,x,y,x+w,y+h,color565);
	else lcdDrawFillRect(&dev,x,y,x+w,y+h,color565);
}


void uCanvas_Draw_Circle(int x, int y, int r, color_t color, fill_t fill){
	uint16_t color565 = convertToRGB565(color);
	if(fill==NOFILL) lcdDrawCircle(&dev,x,y,r,color565);
	else lcdDrawFillCircle(&dev,x,y,r,color565);
}

void uCanvas_Draw_Line (Coordinate2D_t point1, Coordinate2D_t point2, color_t color){
	uint16_t color565 = convertToRGB565(color);
	lcdDrawLine(&dev,point1.x, point1.y,point2.x, point2.y,color565);
}
void uCanvas_Draw_Text (char* text, int x, int y, color_t color){
	// printf("RENDERING : %s %d \r\n",text,strlen(text));
	uint16_t color565 = convertToRGB565(color);
	lcdSetFontDirection(&dev,DIRECTION0);
	lcdDrawString(&dev,fx32M,x,y,(uint8_t*)text,color565);
}
void uCanvas_DrawPixel(Coordinate2D_t pos,color_t color){
	uint16_t color565 = convertToRGB565(color);
	lcdDrawPixel(&dev,pos.x, pos.y, color565);
}
void uCanvas_DrawPixel565 (Coordinate2D_t pos,uint16_t color){
	lcdDrawPixel(&dev,pos.x, pos.y, color);
}
void uCanvas_Draw_Triangle(Coordinate2D_t point1, Coordinate2D_t point2, Coordinate2D_t point3, color_t color, fill_t fill){

}
#endif