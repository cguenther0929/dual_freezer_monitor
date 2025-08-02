#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>    //This likely defines wire.h
#include <U8x8lib.h>
#include <Preferences.h>

class SENSOR; 

enum Screen {
  SCREEN_1,
  SCREEN_2,
  SCREEN_3
};

/**
 * For I2C addressed, understand that the arduino left shifts
 * the address and then appends the R/#W bit.  Therefore, when 
 * determining an address, the address shall first be right 
 * shifted (thus removing the R/#W bit).  Take address 
 * 0x1010101 + R/#W, for example.  However, after right 
 * shifting, the address 0101_0101 or 0x55 is what shall 
 * be passed to the function.  
 */


#define OLED_COLUMN_START                       0
#define OLED_ROW_START                          1
#define OLED_ROW_SPACING                        2


class DISP 
{
    private:
        Screen  current_screen        = SCREEN_1;
        uint8_t current_oled_row      = 0;

    public:
        
        /**
         * @brief DISP init function
         * @param \p none 
         * @return nothing
         */
        void init(U8X8_SH1106_128X64_NONAME_HW_I2C & display_object );

        /**
         * @brief Refresh the OLED display 
         * @param \p Diaplay_Object display instance \p sensor_instance so sensor readins can be accessed 
         * @return true if the update succeeds
         */
        bool refresh_display(U8X8_SH1106_128X64_NONAME_HW_I2C & display_object, SENSOR & sensor_instance);

        /**
         * @brief Determine HW revision 
         * @return The revision of HW 
         */
        uint8_t get_hw_revision( void );
        
        /**
         * @brief Print uint8_t number (i.e. 10101010)
         * @param \p Byte to print
         * @return nothing
         */
        void print8b_bin(uint8_t aByte);
        
        /**
         * @brief Print uint16_t number (i.e. 1010101010101010)
         * @param \p Integer to print
         * @return nothing
         */
        void print16b_bin(uint16_t aByte);

};

#endif