#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>    //This likely defines wire.h
#include <SPI.h>        //This likely defines wire.h
#include "app.h"
#include <Preferences.h>

#define PIN_MOSI            6
#define PIN_MISO            5
#define SPI_SCK             4
#define SPI_CS              7

class SENSOR 
{
    private:
        uint8_t temp_buf_index;
        bool just_rebooted;
        float temperature_buffer[8];
        
    public:
        
        float       current_temp;
        /**
         * @brief Initialization function
         * @param \p none 
         * @return nothing
         */
        void init( void );

        //TODO need to comment
        int TwosConvert( int x );
        
        //TODO need to comment
        float get_temperature( void );
        

};

#endif