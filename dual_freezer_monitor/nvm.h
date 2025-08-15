/**
 *  @filename   :   nvm.h
 *  @brief      :   Header file for the hygrometer EEPROM (not to be mistaken for the Arduino EEPROM.h file) 
 *  @author     :   C. Guenther
 *  @details    :   The routines contained herein are simply wrapper functions 
 *                   that use the core EEPROM.h Arduino library
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
 * Informaiton for using the preferences library can 
 * be found here:
 * https://docs.espressif.com/projects/arduino-esp32/en/latest/api/preferences.html
 * 
 */

#ifndef HYGROMETENVM_H
#define HYGROMETENVM_H

#include <Arduino.h>    //This likely defines wire.h
#include <Preferences.h>
#include "app.h"

#define PREF_BUFF_ELEMENTS              32

/**
 * Keys for key-value 
 * NVM storage 
 */
#define PREF_EMAIL_AUTHOR_KEY               "auth"
#define PREF_EMAIL_RECIPIENT_KEY            "recipient"
#define PREF_EMAIL_AUTHOR_PASSWORD_KEY      "auth_pass"
#define PREF_WIFI_SSID                      "wifi_ssid"
#define PREF_WIFI_PASSWORD                  "wifi_password"

#define PREF_THRESHOLD                      "threshold"

#define NVM_NAMESPACE                       "FZR_NVM"

/**
 * Hardcoded configuration status 
 * words
 */
#define VALID_CAL_VALUE                         0xAAAA  // 16bit calibration sequence 



class NVM {
    private:
        uint8_t     characters_read_back    = 0x00;

    public:
        /**
         * @brief NVM init function
         * @param \p none 
         * @return nothing
         */
        void init(void);

        /**
         * @brief NVM store byte
         * @details NVM is 512 bytes
         * @param  \p Instance of Preferences \p Key (const char) \p Value (8 bit)
         * @return nothing
         */
        void nvm_store_byte (Preferences & pref, const char * nvmkey, uint8_t value);
        
        /**
         * @brief NVM store and integer value
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences \p Key (const char) \p Value (16 bit)
         * @return nothing
         */
        void nvm_store_int (Preferences & pref, const char * nvmkey, uint16_t value);
        
        /**
         * @brief NVM store float 
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences \p Key (const char) \p Value (float)
         * @return nothing
         */
        void nvm_store_float (Preferences & pref, const char * nvmkey, float value);
        
        /**
         * @brief NVM retrieve a float value from NVM
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences \p Key (const char)
         * @return The float value
         */
        float nvm_get_float (Preferences & pref, const char * nvmkey);
        
        /**
         * @brief NVM store string 
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences \p Key (const char) \p Data_buffer (pointer to buffer)
         * @return nothing
         */
        void nvm_store_string (Preferences & pref, const char * nvmkey, const char * data_buffer);
        
        /**
         * @brief Retrieve a string value from NVM 
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences \p Key (const char) \p Buffer to populate
         * @return Nothing
         */
        void nvm_read_string(Preferences & pref, const char * nvmkey, char (&arr)[PREF_BUFF_ELEMENTS]);
        
        /**
         * @brief Determine if the network parameters are valid 
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences 
         * @return True if the network parameters are valid
         */
        bool network_valid(Preferences & pref);
        
        
        /**
         * @brief NVM read byte
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences \p Key (const char)
         * @return The byte associated with the key
         */
        uint8_t nvm_read_byte (Preferences & pref, const char * nvmkey);
        
        /**
         * @brief NVM read int
         * @details NVM is 512 bytes
         * @param \p Instance of Preferences \p Key (const char)
         * @return The uint_16 associated with the key
         */
        uint16_t nvm_read_int (Preferences & pref, const char * nvmkey);
};

#endif