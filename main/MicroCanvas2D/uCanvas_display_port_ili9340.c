#include "uCanvas_display_port.h"
#include "ili9340.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#ifdef USE_ILI9340
#define TAG "ILI9340"


TFT_t dev;

FontxFile fx16G[2];
FontxFile fx24G[2];
FontxFile fx32G[2];
FontxFile fx32L[2];
FontxFile fx16M[2];
FontxFile fx24M[2];
FontxFile fx32M[2];

static void listSPIFFS(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}

esp_err_t mountSPIFFS(char * path, char * label, int max_files) {
	esp_vfs_spiffs_conf_t conf = {
		.base_path = path,
		.partition_label = label,
		.max_files = max_files,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret ==ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret== ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return ret;
	}

#if 0
	ESP_LOGI(TAG, "Performing SPIFFS_check().");
	ret = esp_spiffs_check(conf.partition_label);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
		return ret;
	} else {
			ESP_LOGI(TAG, "SPIFFS_check() successful");
	}
#endif

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Mount %s to %s success", path, label);
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	return ret;
}

void uCanvas_Display_init(void){
        
        spi_master_init(&dev, 18, 5, 22, 19, 
		21, -1, -1,-1,-1 ,-1,-1);
        lcdInit(&dev, 0x9341, 120 ,120, 20, 50);
        lcdFillScreen(&dev, BLACK);   
        
        esp_err_t ret;
        ret = mountSPIFFS("/spiffs", "storage0", 10);
        if (ret != ESP_OK) return;
        listSPIFFS("/spiffs/");

        // Image file borrowed from here
        // https://www.flaticon.com/packs/social-media-343
        ret = mountSPIFFS("/icons", "storage1", 10);
        if (ret != ESP_OK) return;
        listSPIFFS("/icons/");

        ret = mountSPIFFS("/images", "storage2", 14);
        if (ret != ESP_OK) return;
        listSPIFFS("/images/");

        // set font file
        InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
        InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
        InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic
        InitFontx(fx32L,"/spiffs/LATIN32B.FNT",""); // 16x32Dot Latinc   
        InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
        InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
        InitFontx(fx32M,"/spiffs/ILMH32XB.FNT",""); // 16x32Dot Mincyo
        
    }

    void uCanvas_Display_clear_buffer(void){
        lcdFillScreen(&dev,BLACK);
    }

    void uCanvas_Update_Display(void){

    }

    void uCanvas_Draw_Rectangle(int x, int y, int h, int w, color_t color, fill_t fill){
        uint16_t lcdcolor = rgb565 (color.red,color.green,color.blue);
        if(fill)lcdDrawRect(&dev, x,y,x+w,h,lcdcolor);
        else lcdDrawFillRect(&dev, x,y,x+w,h,lcdcolor);
    }

    void uCanvas_Draw_Circle(int x, int y, int r, color_t color, fill_t fill){
        uint16_t lcdcolor = rgb565 (color.red,color.green,color.blue);
        if(fill)lcdDrawCircle(&dev,x,y,r,lcdcolor);
        else lcdDrawFillCircle(&dev,x,y,r,lcdcolor);
    }


    void uCanvas_Draw_Line(_2dPoint_t point1, _2dPoint_t point2, color_t color){

    }

    void uCanvas_Draw_Text(char* text, int x, int y, color_t color){
        uint16_t lcdcolor = rgb565 (color.red,color.green,color.blue);
        lcdDrawString(&dev, fx16M, x, y, (uint8_t*)text, lcdcolor);
    }



#endif