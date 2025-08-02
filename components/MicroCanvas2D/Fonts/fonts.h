#ifndef __FONTS_H
#define __FONTS_H

/**/
#define MAX_HEIGHT_FONT         41
#define MAX_WIDTH_FONT          32
#define OFFSET_BITMAP           

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

//ASCII
typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
  
} sFONT;


extern sFONT Font24;
extern sFONT Font20;
extern const sFONT Font16;
extern sFONT Font12;
extern sFONT Font8;
extern sFONT BitcountPropDouble32;
extern sFONT Roboto_Italic_32;
extern sFONT Sixtyfour_32;
#ifdef __cplusplus
}
#endif
  
#endif /* __FONTS_H */
 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
