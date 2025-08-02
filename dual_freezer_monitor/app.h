/**
 *  @filename   :   app.h
 *  @brief      :   Header file for general hygrometer application functions  
 *  @author     :   C. Guenther
 *  @details    :   The routines contained herein are very specific to the 
 *                  Digital Hygrometer project
 *  
 *  Copyright (C) C. Guenther December 10 2024
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */

#ifndef APP_H
#define APP_H

#include <Arduino.h>    //This likely defines wire.h
#include <esp_sleep.h>    //This likely defines wire.h
#include <esp_timer.h>
#include <WiFi.h>
#include <Preferences.h>
#include "display.h"
#include "lan.h"

// ==============================
// ==============================
#define     SW_VERSION            "0.1.0"
#define     HW_VERSION            "01"
// ==============================
// ==============================

/**
 * Define dwell time between 
 * emails 
 */
#define HR_BETWEEN_EMAIL                  12                            // Hours that will elapse between emails 
#define MIN_BETWEEN_EMAIL                 ((HR_BETWEEN_EMAIL) * 60)    // Min that will elapse between emails
#define SECONDS_BETWEEN_EMAIL             ((MIN_BETWEEN_EMAIL) * 60)   // Seconds that will elapse between emails 


#define MIN_BETWEEN_FAULT_EMAIL           10                                // Min that will elapse between emails
#define SECONDS_BETWEEN_FAULT_EMAIL       ((MIN_BETWEEN_FAULT_EMAIL) * 10)  


#define SECONDS_BETWEEN_DISPLAY_UPDATES   3

/**
 * Application related 
 * defines
 */
#define MAX_ALLOWED_TEMP              82  //TODO need to adjust this to a reasonable value
#define TEMP_FAULT_HYSTERESIS         3

/**
 * Set to true to 
 * enable logging
 */
#define ENABLE_LOGGING          true

/**
 * Health LED
 */
#define HEALTH_LED              10

/**
 * Hardware revision bits
 */
#define HW_REV_BIT_0                    0
#define HW_REV_BIT_1                    1
#define HW_REV_BIT_2                    2

/**
 * Serial parameters
 */
#define SERIAL_BAUD_RATE        115200

typedef enum State {
  STATE_SLEEP,
  STATE_READ_DATA,
  STATE_UPDATE_DISPLAY,
  STATE_SEND_EMAIL
};

class APP
{
    public:
        uint8_t State                             = STATE_SLEEP; //Initialize the state to sleep 

        uint32_t seconds_counter                  = 0x00000000;
        uint16_t timer_seconds_cnt                = 0x0000;       // 65535 seconds is enough for 17.5 hrs

        bool heartbeat_enabled                    = true;
        bool timer_enabled                        = false;
        bool temperature_fault                    = false;
        bool immediate_fault_email_sent           = false;

        /**
         * @brief APP init function
         * @param \p none 
         * @return nothing
         */
        void init(void);
        
        
        /**
         * @brief Process current state or switch to new state   
         * @param \p Application Instance 
         * @return nothing 
         */
        void state_handler (APP & app_instance );
        
};

#endif