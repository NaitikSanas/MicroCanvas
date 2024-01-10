#include "uCanvas_display_buffer.h"
#define Conv_rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
static uint8_t LCD_BUF[LCD_HEIGHT*LCD_WIDTH] = {0};

void lcd_Draw_pixel(uint16_t x, uint16_t y, color_t color) {
	if (
		x >= LCD_WIDTH ||
		y >= LCD_HEIGHT
	) {
		/* Error */
		return;
	}
    LCD_BUF[x + (y * LCD_WIDTH)] = Conv_rgb565(color.red,color.blue,color.blue);
}


void lcd_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= LCD_WIDTH) {
		x0 = LCD_WIDTH - 1;
	}
	if (x1 >= LCD_WIDTH) {
		x1 = LCD_WIDTH - 1;
	}
	if (y0 >= LCD_HEIGHT) {
		y0 = LCD_HEIGHT - 1;
	}
	if (y1 >= LCD_HEIGHT) {
		y1 = LCD_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			lcd_draw_pixel(x0, i, c);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			lcd_draw_pixel(i, y0, c);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		lcd_draw_pixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}



bool spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength)
{
	spi_transaction_t SPITransaction;
	esp_err_t ret;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Data;
#if 1
		ret = spi_device_transmit( SPIHandle, &SPITransaction );
#endif
#if 0
		ret = spi_device_polling_transmit( SPIHandle, &SPITransaction );
#endif
		assert(ret==ESP_OK); 
	}

	return true;
}

bool spi_master_write_comm_byte(TFT_t * dev, uint8_t cmd)
{
	static uint8_t Byte = 0;
	Byte = cmd;
	gpio_set_level( dev->_dc, SPI_Command_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, &Byte, 1 );
}

bool spi_master_write_comm_word(TFT_t * dev, uint16_t cmd)
{
	static uint8_t Byte[2];
	Byte[0] = (cmd >> 8) & 0xFF;
	Byte[1] = cmd & 0xFF;
	gpio_set_level( dev->_dc, SPI_Command_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, 2 );
}


bool spi_master_write_data_byte(TFT_t * dev, uint8_t data)
{
	static uint8_t Byte = 0;
	Byte = data;
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, &Byte, 1 );
}


bool spi_master_write_data_word(TFT_t * dev, uint16_t data)
{
	static uint8_t Byte[2];
	Byte[0] = (data >> 8) & 0xFF;
	Byte[1] = data & 0xFF;
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, 2);
}

bool spi_master_write_addr(TFT_t * dev, uint16_t addr1, uint16_t addr2)
{
	static uint8_t Byte[4];
	Byte[0] = (addr1 >> 8) & 0xFF;
	Byte[1] = addr1 & 0xFF;
	Byte[2] = (addr2 >> 8) & 0xFF;
	Byte[3] = addr2 & 0xFF;
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, 4);
}

bool spi_master_write_color(TFT_t * dev, uint16_t color, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (color >> 8) & 0xFF;
		Byte[index++] = color & 0xFF;
	}
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, size*2);
}

// Add 202001
bool spi_master_write_colors(TFT_t * dev, uint16_t * colors, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (colors[i] >> 8) & 0xFF;
		Byte[index++] = colors[i] & 0xFF;
	}
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, size*2);
}


void delayMS(int ms) {
	int _ms = ms + (portTICK_PERIOD_MS - 1);
	TickType_t xTicksToDelay = _ms / portTICK_PERIOD_MS;
	vTaskDelay(xTicksToDelay);
}


void lcdWriteRegisterWord(TFT_t * dev, uint16_t addr, uint16_t data)
{
	spi_master_write_comm_word(dev, addr);
	spi_master_write_data_word(dev, data);
}


void lcdWriteRegisterByte(TFT_t * dev, uint8_t addr, uint16_t data)
{
	spi_master_write_comm_byte(dev, addr);
	spi_master_write_data_word(dev, data);
}


void lcd_Update(){
	spi_master_write_comm_byte(dev, 0x2A);	// set column(x) address
	spi_master_write_addr(dev, _x1, _x2);
	spi_master_write_comm_byte(dev, 0x2B);	// set Page(y) address
	spi_master_write_addr(dev, _y1, _y2);
	spi_master_write_comm_byte(dev, 0x2C);	// Memory Write
	for(int i=_x1;i<=_x2;i++) {
		uint16_t size = _y2-_y1+1;
		spi_master_write_color(dev, color, size);
	}
}