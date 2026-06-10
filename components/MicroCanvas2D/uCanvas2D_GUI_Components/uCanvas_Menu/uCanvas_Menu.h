#include "uCanvas_api.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"

typedef enum {
    UCANVAS_ALIGN_RIGHT,
    UCANVAS_ALIGN_LEFT,
    UCANVAS_ALIGN_CENTER,
}uCanvas_Align_t;

typedef enum {
    MENU_TYPE_LIST = 0,
    MENU_TYPE_GRID_BOXED,
    MENU_TYPE_GRID_CIRCLE,
    MENU_TYPE_GRID_ICON
} uCanvas_Menu_Type_t;

typedef struct {
    color_t MenuBackGround;
    color_t MenuBorder;
    color_t MenuTitle;
    color_t MenuTitleBackground;

    color_t ItemName;
    color_t ItemValue;
    color_t ItemBackGround;
    color_t Cursor;
    color_t MenuItemBackground;
}uCanvas_Menu_Theme_t;

typedef struct 
{
    char ItemValue[64];
    
    uCanvas_universal_obj_t* ItemValueUO;
    FontType_t ItemValueFont;
    visibility_ctrl_t ItemValueVisibility;
}uCanvas_Menu_Item_Values_t;


typedef struct {
    
    
    char ItemName[64];
    uint16_t ItemID;
    
    uCanvas_universal_obj_t* ItemNameUO;
    uCanvas_universal_obj_t* ItemIcon;
    uCanvas_universal_obj_t* MenuItemBackground;

    FontType_t ItemNameFont;
    uint8_t ItemBackgroundEnabled;
    visibility_ctrl_t MenuItemBackgroundVisiblity;
    visibility_ctrl_t name_label_visibility;

    visibility_ctrl_t icon_visibility;
    uCanvas_Menu_Item_Values_t ItemValues[8];
    int ActiveItemValues;
} uCanvas_Menu_Item_t;

typedef struct {
    char MenuTitleName[64];
    uCanvas_universal_obj_t* MenuTitleUO;
    uCanvas_universal_obj_t* MenuTitleBackground;
    FontType_t MenuTitleFont;
    visibility_ctrl_t MenuTitleVisiblity;
} uCanvas_Menu_Title_t;

typedef enum {
    CURSOR_1D,
    CURSOR_2D,
} Cursor_type_t;

typedef struct 
{
    uCanvas_universal_obj_t* Cursor;
    uCanvas_universal_obj_t* Cursor_Notch;
    Cursor_type_t CursorType;
    int16_t CursorIndex_1;
    int16_t CursorIndex_2;
}uCanvas_Menu_Cursor_t;


typedef struct {
    uCanvas_Menu_Title_t    MenuTitle;
    uCanvas_Menu_Item_t     MenuItems [64];
    uCanvas_Menu_Cursor_t   MenuCursor;

    uCanvas_universal_obj_t* MenuBackground;
    uCanvas_universal_obj_t* MenuBackgroundBorder;
    
    uint8_t ExpandSelectedItem;
    uCanvas_universal_obj_t* MenuSeparatorLine;

    uCanvas_Align_t MenuItemsAlignment;
    uint16_t ActiveMenuItems;
    
    int16_t pos_x;
    int16_t pos_y;
    
    int16_t width;
    int16_t height;

    int16_t spacing_x;
    int16_t spacing_y;

    int16_t margin_x;
    int16_t margin_y;

    uCanvas_Menu_Theme_t ColorTheme;
} uCanvas_Menu_t;

typedef struct 
{
    char ItemName[64];
    char ItemValue[64];
    uint8_t Index;
}uCanvas_Menu_Ctx_t;


int uCanvas_Menu_Set_Title(
    uCanvas_Menu_t* MenuObj, 
    char* TitleName, 
    uCanvas_Align_t SubItemAlignStyle,
    FontType_t FontType
);

int uCanvas_Menu_Add_Item(
    uCanvas_Menu_t* MenuObj, 
    uint16_t index, 
    char* ItemName, 
    FontType_t ItemFontType
);

int uCanvas_Menu_Add_Item_Values(
    uCanvas_Menu_t* MenuObj, 
    uint16_t ValueIndex, 
    uint16_t ItemIndex, 
    char* ItemValueName, 
    FontType_t ItemValueFontType
);

int uCanvas_Menu_Add_Icon(
    uCanvas_Menu_t* MenuObj, 
    uint16_t index, 
    sprite2D_t* IconBitMap
);


int uCanvas_Menu_Update(uCanvas_Menu_t* MenuObj);

int uCanvas_Menu_Get_Selected_Item(uCanvas_Menu_t* MenuObj,uCanvas_Menu_Ctx_t* MenuCtx);