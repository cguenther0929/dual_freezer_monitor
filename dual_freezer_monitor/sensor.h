#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>    //This likely defines wire.h
#include <SPI.h>        //This likely defines wire.h
#include "app.h"
#include <Preferences.h>

#define PIN_MOSI            6
#define PIN_MISO            5
#define SPI_SCK             4
#define SPI1_CS             7
#define SPI2_CS             18

class SENSOR 
{
    private:
        uint8_t temp_buf_index;
        bool just_rebooted;
        float temp1_buffer[8];
        float temp2_buffer[8];
        
        public:
        
        float temp1_value;
        float temp2_value;
        // float current_temp;  //TODO I think we can remove this line?
        
        /**
         * @brief Initialization function
         * @param \p none 
         * @return nothing
         */
        void init( void );
        
        /**
         * @brief Perform a twos complement conversion on the number passed in
         * @param \p x integer passed in (number to convert)
         * @return Integer of the twos complement conversion
         */
        int TwosConvert( int x );
        
        /**
         * @brief Get temperature from the sensors
         * @return true if the function succeeds 
         */
        bool get_temperature( void );
        

};

#endif