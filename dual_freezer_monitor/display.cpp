#include <Wire.h>
#include "display.h"
#include "app.h"
#include "sensor.h"

#define I2C_SDA     8
#define I2C_SCL     9

/**
 * @brief LCD parameters
 * @details This needs to appear 
 * here (i.e. it can't be hidden in a *.h)
 * Here are some other font options ...
 * u8x8_font_chroma48medium8_r          Medium, 8x8, good for basic text
 * u8x8_font_amstrad_cpc_extended_r     Retro look, 6x8
 * u8x8_font_5x7_r                      Small 5x7 font
 * u8x8_font_victoriabold8_r            Bold 8x8 font
 * u8x8_font_pxplusibmcgathin_r         Thin retro font
*/
void DISP::init(U8X8_SH1106_128X64_NONAME_HW_I2C &display_object)
{
    this -> current_screen          = SCREEN_1;
    this -> current_oled_row        = OLED_ROW_START;

    display_object.begin();
    display_object.setFont(u8x8_font_victoriabold8_r);
    display_object.clear();
    
    Wire.begin(I2C_SDA, I2C_SCL);
}


bool DISP::refresh_display (U8X8_SH1106_128X64_NONAME_HW_I2C & display_object, SENSOR & sensor_instance)
{
    switch (this -> current_screen)
    {
        case SCREEN_1:
            display_object.clear();
            this -> current_oled_row = OLED_ROW_START;
            display_object.setCursor(OLED_COLUMN_START, OLED_ROW_START);
            display_object.print("SW V: ");
            display_object.print(SW_VERSION);
            
            this -> current_oled_row += OLED_ROW_SPACING;
            display_object.setCursor(OLED_COLUMN_START,this -> current_oled_row);
            display_object.print("HW V: ");
            display_object.print(HW_VERSION);
            
            
            this -> current_screen          = SCREEN_2; //Move to the next screen 
            break;
            
        case SCREEN_2:

            display_object.clear();
            this -> current_oled_row = OLED_ROW_START;
            display_object.setCursor(OLED_COLUMN_START, this -> current_oled_row);
            display_object.print("Fzr 1: ");
            display_object.print(sensor_instance.temp1_value); 
            display_object.print(" F");
            
            this -> current_oled_row += OLED_ROW_SPACING;
            display_object.setCursor(OLED_COLUMN_START,this -> current_oled_row);
            display_object.print("Fzr 2: ");
            display_object.print(sensor_instance.temp2_value); 
            display_object.print(" F");
            this -> current_screen          = SCREEN_1;   //Move back to screen one
        break;
            
        case SCREEN_3:  
                display_object.clear();
        break;
            
        default:
            display_object.clear();
            display_object.setCursor(OLED_COLUMN_START, OLED_ROW_START);
            display_object.print("ERROR");
        break;
    }
    

    return true;
}