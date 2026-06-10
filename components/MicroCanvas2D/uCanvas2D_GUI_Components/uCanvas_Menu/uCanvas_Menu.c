#include "uCanvas_Menu.h"
int uCanvas_Menu_Set_Title(
    uCanvas_Menu_t* MenuObj, 
    char* TitleName, 
    uCanvas_Align_t SubItemAlignStyle,
    FontType_t FontType
){
    if(MenuObj){
        if(TitleName){
            sprintf(MenuObj->MenuTitle.MenuTitleName,"%s",TitleName);
            MenuObj->MenuTitle.MenuTitleVisiblity = VISIBLE;
            MenuObj->MenuTitle.MenuTitleFont = FontType;
        }
        return 1;
    }
    return 0;
}

int fcuCanvas_Menu_Add_Item(
    uCanvas_Menu_t* MenuObj, 
    uint16_t index, 
    char* ItemName, 
    FontType_t ItemFontType
){
    if(MenuObj && index < 64){
        if(ItemName){
            sprintf(MenuObj->MenuItems[index].ItemName,"%s",ItemName);
            MenuObj->MenuItems[index].name_label_visibility = VISIBLE;
            MenuObj->MenuItems[index].name_label_visibility = VISIBLE;  
            
            if(MenuObj->ActiveMenuItems < 64){
                MenuObj->ActiveMenuItems++;
            }
        }
        MenuObj->MenuItems[index].ItemNameFont = ItemFontType;
        return 1;
    }
    return 0;
}

int uCanvas_Menu_Add_Item_Values(
    uCanvas_Menu_t* MenuObj, 
    uint16_t ValueIndex, 
    uint16_t ItemIndex, 
    char* ItemValueName, 
    FontType_t ItemValueFontType
){
    if(MenuObj && ItemIndex < 64 && ValueIndex < 64){
        if(ItemValueName){
            sprintf(MenuObj->MenuItems[ItemIndex].ItemValues[ValueIndex].ItemValue,"%s",ItemValueName);
            if(MenuObj->MenuItems[ItemIndex].ActiveItemValues < 8)MenuObj->MenuItems[ItemIndex].ActiveItemValues++;
        }
        MenuObj->MenuItems[ItemIndex].ItemValues[ValueIndex].ItemValueFont = ItemValueFontType;
        return 1;
    }
    return 0;
}


int uCanvas_Menu_Add_Icon(
    uCanvas_Menu_t* MenuObj, 
    uint16_t index, 
    sprite2D_t* IconBitMap
){
    if(MenuObj){
        if(MenuObj->MenuItems[index].ItemIcon){
            uCanvas_Change_Sprite_Source(MenuObj->MenuItems[index].ItemIcon,IconBitMap);
        }
        else{
            MenuObj->MenuItems[index].ItemIcon = New_uCanvas_2DSprite(IconBitMap,0,0);
        }
        return 1;
    }
    return 0;
}

int uCanvas_Menu_Update(uCanvas_Menu_t* MenuObj){
    if(MenuObj){
        int16_t menu_pos_x = MenuObj->pos_x;
        int16_t menu_pos_y = MenuObj->pos_y;

        int16_t menu_margin_x = MenuObj->margin_x;
        int16_t menu_margin_y = MenuObj->margin_y;

        if(!MenuObj->MenuBackground){
            MenuObj->MenuBackground = New_uCanvas_2DRectangle(MenuObj->pos_x,MenuObj->pos_y,MenuObj->height,MenuObj->width);
            MenuObj->MenuBackground->properties.color = MenuObj->ColorTheme.MenuBackGround;
            uCanvas_Set_Fill(MenuObj->MenuBackground,FILL);
        }

        if(!MenuObj->MenuBackgroundBorder){
            MenuObj->MenuBackgroundBorder = New_uCanvas_2DRectangle(MenuObj->pos_x,MenuObj->pos_y,MenuObj->height,MenuObj->width);
            MenuObj->MenuBackgroundBorder->properties.color = MenuObj->ColorTheme.MenuBorder;
            uCanvas_Set_Fill(MenuObj->MenuBackgroundBorder,NOFILL);

            
        }

        if(!MenuObj->MenuCursor.Cursor){
            MenuObj->MenuCursor.Cursor = New_uCanvas_2DRectangle(0,0,0,0);
            MenuObj->MenuCursor.Cursor->properties.color = MenuObj->ColorTheme.Cursor;
            uCanvas_Set_Fill(MenuObj->MenuCursor.Cursor,FILL);
            MenuObj->MenuCursor.Cursor_Notch = New_uCanvas_2DCircle(0,0,0);
            MenuObj->MenuCursor.Cursor_Notch->properties.color = MenuObj->ColorTheme.Cursor;
            uCanvas_Set_Fill(MenuObj->MenuCursor.Cursor_Notch,FILL);
        }

        if(!MenuObj->MenuTitle.MenuTitleUO){

            MenuObj->MenuTitle.MenuTitleBackground = New_uCanvas_2DRectangle(MenuObj->pos_x,MenuObj->pos_y,uCanvas_Get_Font_Height(MenuObj->MenuTitle.MenuTitleFont),MenuObj->width);
            MenuObj->MenuTitle.MenuTitleBackground->properties.color = MenuObj->ColorTheme.MenuTitleBackground;
            uCanvas_Set_Fill(MenuObj->MenuTitle.MenuTitleBackground ,FILL);
            MenuObj->MenuTitle.MenuTitleUO = New_uCanvas_2DTextbox(MenuObj->MenuTitle.MenuTitleName,0,0);
        }
        
        /* Render Title */
        if(strlen(MenuObj->MenuTitle.MenuTitleName)>0){
            int16_t title_font_w = uCanvas_Get_Font_Width(MenuObj->MenuTitle.MenuTitleFont);
            int16_t title_font_h = uCanvas_Get_Font_Height(MenuObj->MenuTitle.MenuTitleFont);
            // printf("FontWH=%d,%d\r\n",title_font_w,title_font_h);
            int16_t title_pos_x = menu_pos_x + menu_margin_x;
            int16_t title_pos_y = menu_pos_y + menu_margin_y;

            int32_t title_textbox_width = (strlen(MenuObj->MenuTitle.MenuTitleName) + 1) * title_font_w;
            int32_t title_textbox_height = title_font_h + 2;

            uCanvas_Set_Position(MenuObj->MenuTitle.MenuTitleUO,title_pos_x,title_pos_y);
            // printf("title_box_width %ld, %d\r\n",title_textbox_width,strlen(MenuObj->MenuTitle.MenuTitleName));
            uCanvas_Set_Width_Height(MenuObj->MenuTitle.MenuTitleUO,title_pos_x,title_pos_y);
            
            uCanvas_Set_Visiblity(MenuObj->MenuTitle.MenuTitleUO,VISIBLE);
            
            MenuObj->MenuTitle.MenuTitleUO->properties.color = MenuObj->ColorTheme.MenuTitle;

            uCanvas_Set_Textbox_Alignment(MenuObj->MenuTitle.MenuTitleUO, TEXT_LEFT_ALIGNED);
            uCanvas_Set_Textbox_Wrap_Style(MenuObj->MenuTitle.MenuTitleUO, TEXT_WRAP_PER_SET_WORD_LEN, 32);
            uCanvas_Set_TextBox_Margin(MenuObj->MenuTitle.MenuTitleUO, 2, 2);
            // uCanvas_Set_TextBox_Fill_Background(MenuObj->MenuTitle.MenuTitleUO, FILL, MenuObj->ColorTheme.MenuBackGround.red,MenuObj->ColorTheme.MenuBackGround.green,MenuObj->ColorTheme.MenuBackGround.blue);
            uCanvas_Set_TextBox_Fill_Background(MenuObj->MenuTitle.MenuTitleUO, NOFILL,0,0,0);

            uCanvas_Set_TextBox_FontType(MenuObj->MenuTitle.MenuTitleUO, MenuObj->MenuTitle.MenuTitleFont);

            uCanvas_Set_TextBox_Size(MenuObj->MenuTitle.MenuTitleUO, title_textbox_width, title_font_h+4);
        }

        /*------------- Render All Menu Items & Cursor -------------- */
        int max_item_w = 0;
        int16_t accumulated_y = uCanvas_Get_Font_Height(MenuObj->MenuTitle.MenuTitleFont);
        for (int i = 0; i < MenuObj->ActiveMenuItems; i++)
        {
            int16_t menu_item_pos_x = MenuObj->spacing_x + MenuObj->pos_x;
            int16_t menu_item_pos_y = accumulated_y + MenuObj->pos_y;

            if(MenuObj->MenuItems[i].ItemBackgroundEnabled){
                MenuObj->MenuItems[i].MenuItemBackground = New_uCanvas_2DLine(0,0,0,0); 
                MenuObj->MenuItems[i].MenuItemBackground->properties.color = MenuObj->ColorTheme.ItemName;
                uCanvas_Set_Position(MenuObj->MenuItems[i].MenuItemBackground,0,0);
            }

            if(!MenuObj->MenuItems[i].ItemNameUO ){
                MenuObj->MenuItems[i].ItemNameUO = New_uCanvas_2DTextbox(MenuObj->MenuItems[i].ItemName,menu_item_pos_x,menu_item_pos_y);
                uCanvas_Set_Visiblity(MenuObj->MenuItems[i].ItemNameUO,INVISIBLE);
                uCanvas_Delay(pdMS_TO_TICKS(20));
                
               
            }
            else{
                uCanvas_Set_Position(MenuObj->MenuItems[i].ItemNameUO,menu_item_pos_x,menu_item_pos_y);
                uCanvas_Set_Text(MenuObj->MenuItems[i].ItemNameUO,MenuObj->MenuItems[i].ItemName);
            }
            
            uCanvas_Set_Visiblity(MenuObj->MenuItems[i].ItemNameUO,VISIBLE);
            
            MenuObj->MenuItems[i].ItemNameUO->properties.color = MenuObj->ColorTheme.ItemName;
            
            uCanvas_Set_Textbox_Alignment(MenuObj->MenuItems[i].ItemNameUO, TEXT_CENTER_ALIGNED);
            uCanvas_Set_Textbox_Wrap_Style(MenuObj->MenuItems[i].ItemNameUO, TEXT_WRAP_PER_SET_WORD_LEN, 32);
            uCanvas_Set_TextBox_Margin(MenuObj->MenuItems[i].ItemNameUO, 2, 2);
            uCanvas_Set_TextBox_Fill_Background(MenuObj->MenuItems[i].ItemNameUO, NOFILL, 0, 0, 0);
            uCanvas_Set_TextBox_FontType(MenuObj->MenuItems[i].ItemNameUO, MenuObj->MenuItems[i].ItemNameFont);

            int16_t item_font_w = uCanvas_Get_Font_Width(MenuObj->MenuItems[i].ItemNameFont);
            int16_t item_font_h = uCanvas_Get_Font_Height(MenuObj->MenuItems[i].ItemNameFont);
            accumulated_y += item_font_h + MenuObj->spacing_y;

            int32_t item_textbox_width = (strlen(MenuObj->MenuItems[i].ItemName) + 1) * item_font_w;
            if(item_textbox_width > max_item_w){
                max_item_w = item_textbox_width;
                printf("max_item_w at %d\r\n",i);
            }
            uCanvas_Set_TextBox_Size(MenuObj->MenuItems[i].ItemNameUO, item_textbox_width, item_font_h+4);
            
            if(MenuObj->MenuItems[i].ItemBackgroundEnabled){
                uCanvas_Set_Line_Coordinates(MenuObj->MenuItems[i].MenuItemBackground,menu_item_pos_x,menu_item_pos_y+item_font_h,menu_item_pos_x+item_textbox_width,menu_item_pos_y+item_font_h);
            }
            if(MenuObj->MenuCursor.CursorType == CURSOR_1D && i == MenuObj->MenuCursor.CursorIndex_1){

                int16_t current_y     = MenuObj->MenuCursor.Cursor->properties.position.y;
                int16_t current_w     = MenuObj->MenuCursor.Cursor->width;
                int16_t target_y      = menu_item_pos_y;
                int16_t target_w      = item_textbox_width;

                if(current_y < target_y){
                    int cy = current_y;
                    while (cy != target_y)
                    {
                        cy++;
                        // interpolate width as cy approaches target_y
                        int16_t interp_w = current_w + (int16_t)((target_w - current_w) * (cy - current_y) / (float)(target_y - current_y));
                        uCanvas_Set_Width_Height(MenuObj->MenuCursor.Cursor, interp_w, item_font_h);
                        uint16_t notch_pos_x = MenuObj->pos_x + interp_w;
                        uCanvas_Set_Position(MenuObj->MenuCursor.Cursor_Notch,notch_pos_x,cy+item_font_h/2);
                        uCanvas_Set_Radius1(MenuObj->MenuCursor.Cursor_Notch,item_font_h/2);
                        uCanvas_Set_Position(MenuObj->MenuCursor.Cursor, MenuObj->pos_x, cy);
                        uCanvas_Delay(2);
                    }
                }
                else if(current_y > target_y){
                    int cy = current_y;
                    while (cy != target_y)
                    {
                        cy--;
                        int16_t interp_w = current_w + (int16_t)((target_w - current_w) * (current_y - cy) / (float)(current_y - target_y));
                        uCanvas_Set_Width_Height(MenuObj->MenuCursor.Cursor, interp_w, item_font_h);
                        uCanvas_Set_Position(MenuObj->MenuCursor.Cursor, MenuObj->pos_x, cy);
                        uint16_t notch_pos_x = MenuObj->pos_x + interp_w;
                        uCanvas_Set_Position(MenuObj->MenuCursor.Cursor_Notch,notch_pos_x,cy+item_font_h/2);
                        uCanvas_Set_Radius1(MenuObj->MenuCursor.Cursor_Notch,item_font_h/2);
                        uCanvas_Delay(2);
                    }
                }

                // ensure final state is exact
                uCanvas_Set_Width_Height(MenuObj->MenuCursor.Cursor, target_w, item_font_h);
                uCanvas_Set_Position(MenuObj->MenuCursor.Cursor, MenuObj->pos_x, target_y);
            }
        } 

        /* -------------- Render Separator When Item is Selected ------------ */
        if(MenuObj->ExpandSelectedItem){
            int separator_line_pos_x = max_item_w + MenuObj->spacing_x;
            int separator_line_pos_y = uCanvas_Get_Font_Height(MenuObj->MenuTitle.MenuTitleFont) + MenuObj->spacing_y ;
            if(!MenuObj->MenuSeparatorLine){
                MenuObj->MenuSeparatorLine = New_uCanvas_2DLine(0,0,0,0);
                uCanvas_Set_Position(MenuObj->MenuSeparatorLine,MenuObj->pos_x+10,MenuObj->pos_y);
                MenuObj->MenuSeparatorLine->properties.color = MenuObj->ColorTheme.ItemName;
                uCanvas_Set_Line_Coordinates(MenuObj->MenuSeparatorLine,separator_line_pos_x,separator_line_pos_y,separator_line_pos_x,separator_line_pos_y + MenuObj->height -  uCanvas_Get_Font_Height(MenuObj->MenuTitle.MenuTitleFont) - MenuObj->spacing_y);
            }
            else {
                    uCanvas_Set_Visiblity(MenuObj->MenuSeparatorLine,VISIBLE);
            }
        }
        else {
            uCanvas_Set_Visiblity(MenuObj->MenuSeparatorLine,INVISIBLE);
        }

        /*----------- Render Item Values When Expanded -------------------*/
        if(MenuObj->ExpandSelectedItem){
            int ItemCursor = MenuObj->MenuCursor.CursorIndex_1;
            int ItemValuePosX = max_item_w + MenuObj->spacing_x + MenuObj->pos_x;
            int ItemValuePosY = uCanvas_Get_Font_Height(MenuObj->MenuTitle.MenuTitleFont) + MenuObj->spacing_y + MenuObj->pos_y;
            int ActiveItemValues = MenuObj->MenuItems[ItemCursor].ActiveItemValues;
            printf("avs %d\r\n",ActiveItemValues);
            for (int i = 0; i < ActiveItemValues; i++)
            {
                uCanvas_universal_obj_t* MenuItemUO = MenuObj->MenuItems[ItemCursor].ItemValues[i].ItemValueUO;
                
                if(MenuObj->MenuItems[ItemCursor].ItemValues[i].ItemValueUO == NULL){
                    printf("new obj\r\n");
                    MenuObj->MenuItems[ItemCursor].ItemValues[i].ItemValueUO = New_uCanvas_2DTextbox("-----",ItemValuePosX,ItemValuePosY);
                    uCanvas_Set_Visiblity(MenuItemUO,INVISIBLE);
                }
                uCanvas_Set_Visiblity(MenuItemUO,VISIBLE);
            
                MenuItemUO->properties.color = MenuObj->ColorTheme.ItemName;
                uCanvas_Set_Text(MenuItemUO,MenuObj->MenuItems[ItemCursor].ItemValues[i].ItemValue);
                uCanvas_Set_Textbox_Alignment(MenuItemUO, TEXT_CENTER_ALIGNED);
                uCanvas_Set_Textbox_Wrap_Style(MenuItemUO, TEXT_WRAP_PER_SET_WORD_LEN, 32);
                uCanvas_Set_TextBox_Margin(MenuItemUO, 2, 2);
                uCanvas_Set_TextBox_Fill_Background(MenuItemUO, NOFILL, 0, 0, 0);
                uCanvas_Set_TextBox_FontType(MenuItemUO, MenuObj->MenuItems[i].ItemNameFont);

                int16_t item_font_w = uCanvas_Get_Font_Width(MenuObj->MenuItems[ItemCursor].ItemValues[i].ItemValueFont);
                int16_t item_font_h = uCanvas_Get_Font_Height(MenuObj->MenuItems[ItemCursor].ItemValues[i].ItemValueFont);
                accumulated_y += item_font_h + MenuObj->spacing_y;

                int32_t item_textbox_width = (strlen(MenuObj->MenuItems[ItemCursor].ItemValues[i].ItemValue) + 1) * item_font_w;
                if(item_textbox_width > max_item_w){
                    max_item_w = item_textbox_width;
                    printf("max_item_w at %d\r\n",i);
                }
                uCanvas_Set_TextBox_Size(MenuItemUO, item_textbox_width, item_font_h+4);
            
            }
            
        }



        return 1;
    }
    return 0;
}

int uCanvas_Menu_Get_Selected_Item(uCanvas_Menu_t* MenuObj,uCanvas_Menu_Ctx_t* MenuCtx){
    if(MenuObj){
        strcpy(MenuCtx->ItemName,MenuObj->MenuItems[MenuObj->MenuCursor.CursorIndex_1].ItemName);
        // strcpy(MenuCtx->ItemValue,MenuObj->MenuItems[MenuObj->MenuCursor.CursorIndex_1].ItemValue);
        MenuCtx->Index = MenuObj->MenuCursor.CursorIndex_1;
        return 1;
    }
    return 0;
}

