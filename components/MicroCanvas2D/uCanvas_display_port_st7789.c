#include "uCanvas_display_port.h"
#ifdef USE_ST7789
#include "st7789.h"
#include "fontx.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_log.h"
TFT_t dev;
uint16_t IRAM_ATTR convertToRGB565(color_t color) {
    // Combine into RGB565 format
  	return (((color.red * 31) / 255) << 11) | (((color.green * 63) / 255) << 5) | ((color.blue * 31) / 255);
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
	lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, 0, 0);
}


uint16_t fill_color;

void uCanvas_Display_clear_buffer(void){
    lcdFillScreen(&dev,fill_color);
}


void IRAM_ATTR uCanvas_Update_Display(void){
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
void uCanvas_DrawPixel(int x, int y,color_t color){
	lcdDrawPixel(&dev,x,y,  convertToRGB565(color));
}
void uCanvas_DrawPixel565 (Coordinate2D_t pos,uint16_t color){
	lcdDrawPixel(&dev,pos.x, pos.y, color);
}

void uCanvas_DrawPixel5652 (uint16_t x, uint16_t y,uint16_t color){
	lcdDrawPixel(&dev,x,y, color);
}
void uCanvas_Draw_Triangle(Coordinate2D_t point1, Coordinate2D_t point2, Coordinate2D_t point3, color_t color, fill_t fill){

}


static inline uint16_t rgba565_to_rgb565(uint16_t rgba565) {
    // Mask to extract RGB components
    uint16_t r = (rgba565 & 0xF800); // Red (5 bits)
    uint16_t g = (rgba565 & 0x07E0); // Green (6 bits)
    uint16_t b = (rgba565 & 0x003E) >> 1; // Blue (5 bits)
    
    // Combine RGB without the alpha channel
    uint16_t rgb565 = r | g | b;
    return rgb565;
}
void flip_sprite_buffer(uint16_t *sprite_buf, uint16_t width, uint16_t height, bool flip_x, bool flip_y) {
    if (!sprite_buf || (!flip_x && !flip_y)) {
        return; // No operation needed
    }

    // Temporary buffer to hold one row for flipping
    uint16_t temp_row[width];

    // Flip horizontally (X-axis)
    if (flip_x) {
        for (uint16_t row = 0; row < height; row++) {
            uint16_t *row_ptr = &sprite_buf[row * width];
            for (uint16_t col = 0; col < width / 2; col++) {
                uint16_t temp = row_ptr[col];
                row_ptr[col] = row_ptr[width - 1 - col];
                row_ptr[width - 1 - col] = temp;
            }
        }
    }

    // Flip vertically (Y-axis)
    if (flip_y) {
        for (uint16_t row = 0; row < height / 2; row++) {
            uint16_t *top_row = &sprite_buf[row * width];
            uint16_t *bottom_row = &sprite_buf[(height - 1 - row) * width];

            // Swap entire rows
            memcpy(temp_row, top_row, sizeof(uint16_t) * width);
            memcpy(top_row, bottom_row, sizeof(uint16_t) * width);
            memcpy(bottom_row, temp_row, sizeof(uint16_t) * width);
        }
    }
}

static uint16_t sprite_buf[128*128];
void st7789_draw_sprite_batch( uCanvas_universal_obj_t *obj) { 
    int offset_x = obj->properties.position.x;
    int offset_y = obj->properties.position.y;
    uint16_t sprite_width = obj-> sprite_resolution.x;
    uint16_t sprite_height = obj->sprite_resolution.y;
    if (obj->properties.visiblity == INVISIBLE) {	
        return;
    }
	// printf("h,w = (%d,%d)\r\n",sprite_height,sprite_width);
	if(sprite_height > 128 || sprite_width > 128){
		printf("[ERROR]SPRITE_BUF OUT OF BOUNDS!!\r\n");
		return;
	}
	memcpy(sprite_buf, obj->sprite_buffer,sizeof(uint16_t)*sprite_height*sprite_width);
	if(obj->properties.flip_x==1)flip_sprite_buffer(sprite_buf,sprite_width,sprite_height,1,0);
	if(obj->properties.flip_y==1)flip_sprite_buffer(sprite_buf,sprite_width,sprite_height,0,1);
    // Iterate over sprite rows
    for (int row = 0; row < sprite_height; row++) {
        int pos_y = row + offset_y;
        if (pos_y < 0 || pos_y >= dev._height) {
            continue;  // Skip rows completely off-screen vertically
        }
        uint16_t *src_row = &sprite_buf[row * sprite_width];
        uint16_t *dest_row = &dev._frame_buffer[pos_y * dev._width];
        int start_col = (offset_x < 0) ? -offset_x : 0;  // First visible column
        int end_col = (offset_x + sprite_width > dev._width) ? dev._width - offset_x : sprite_width;
        // Iterate over visible columns
        for (int col = start_col; col < end_col; col++) {
            int pos_x = col + offset_x;
            if ((src_row[col] & 0x01)) {  // Skip transparent or empty pixels
                dest_row[pos_x] = rgba565_to_rgb565(src_row[col]);
            }
        }
    }
}

#endif