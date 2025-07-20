#include "nvm.h"


char        nvm_buffer[PREF_BUFF_ELEMENTS]             = {NULL};


void NVM::init(void) 
{
}

/**
 *                                                     Key of key:value pair
 *                                                         |            Value to write to NVM  
 *                                                         |               |
 */
void NVM::nvm_store_int (Preferences & pref, const char * nvmkey, uint16_t value)
{
    pref.begin(NVM_NAMESPACE, false);       // Pass in false to use the algorithm in read/write mode
    pref.putUShort(nvmkey, value);       // Store the unsigned short (uint16_t)
    pref.end();                            
}

/**
 *                                                     Key of key:value pair
 *                                                         |            Value to write to NVM  
 *                                                         |               |
 */
void NVM::nvm_store_byte (Preferences & pref, const char * nvmkey, uint8_t value)
{
    pref.begin(NVM_NAMESPACE, false);       // Pass in false to use the algorithm in read/write mode
    pref.putBytes(nvmkey, &value, 1);       //  Define the length to be one
    pref.end();                             
}

uint8_t NVM::nvm_read_byte (Preferences & pref, const char * nvmkey)
{
    uint8_t uint8t_data_buffer[4];

    pref.begin(NVM_NAMESPACE, true);            // for readOnly, we pass in true
    
    pref.getBytes(nvmkey, uint8t_data_buffer, 1);
    
    pref.end();                             // Use to close

    return uint8t_data_buffer[0];
}

uint16_t NVM::nvm_read_int (Preferences & pref, const char * nvmkey)
{
    uint16_t uint16t_data;

    pref.begin(NVM_NAMESPACE, true);        // for readOnly, we pass in true
    
    uint16t_data = pref.getUShort(nvmkey, 0);              // 0 is the default value
    
    pref.end();                             // Use to close

    return (uint16t_data);
}

void NVM::nvm_store_float (Preferences & pref, const char * nvmkey, float value)
{
    pref.begin(NVM_NAMESPACE, false);       // Pass in false to use the algorithm in read/write mode
    pref.putFloat(nvmkey, value);
    pref.end();                         // Use to close
}

float NVM::nvm_get_float (Preferences & pref, const char * nvmkey)
{
    float temp_float    = 0.0;
    pref.begin(NVM_NAMESPACE, true);       // Pass in false to use the algorithm in read/write mode
    temp_float = pref.getFloat(nvmkey);
    pref.end();                         // Use to close
    
    return(temp_float);

}

void NVM::nvm_store_string(Preferences & pref, const char * nvmkey, const char * data_buffer)
{
    pref.begin(NVM_NAMESPACE, false);       // Pass in false to use the algorithm in read/write mode
    
    pref.putString(nvmkey, data_buffer);     // Store the string value. 
    
    pref.end();                         // Use to close

}

void NVM::nvm_read_string(Preferences & pref, const char * nvmkey, char (&arr)[PREF_BUFF_ELEMENTS])
{
    pref.begin(NVM_NAMESPACE, true);            // for read only, we pass in false

    this -> characters_read_back = pref.getString(nvmkey, arr, sizeof(arr));

    pref.end();                             // Use to close
    
    if(this -> characters_read_back <= 0)
    {
        if(ENABLE_LOGGING)
        {
            Serial.println("^NVM read function failed");
        }
    }
    else
    {
        if(ENABLE_LOGGING)
        {
            Serial.println("^NVM read function passed");
        }
    }
}

bool NVM::network_valid(Preferences & pref) {
    
    /**
     * Clear the temporary
     * character buffer just to be 
     * safe
     */
    memset(nvm_buffer, NULL, sizeof(nvm_buffer));
    
    /**
     * Check the WIFI's SSID
     */
    nvm_read_string(pref, PREF_WIFI_SSID, nvm_buffer);
    if(sizeof(nvm_buffer) <= 1) {
        return false;
    }
    
    /**
     * Clear the temporary
     * character buffer just to be 
     * safe
     */
    memset(nvm_buffer, NULL, sizeof(nvm_buffer));
    
    /**
     * Check the WIFI's PASSWORD
     */
    nvm_read_string(pref, PREF_WIFI_PASSWORD, nvm_buffer);
    if(sizeof(nvm_buffer) <= 1) {
        return false;
    }

    /**
     * Clear the temporary
     * character buffer just to be 
     * safe
     */
    memset(nvm_buffer, NULL, sizeof(nvm_buffer));
    
    /**
     * Check author's email address 
     */
    nvm_read_string(pref, PREF_EMAIL_AUTHOR_KEY, nvm_buffer);
    if(sizeof(nvm_buffer) <= 1) {
        return false;
    }
    
    /**
     * Clear the temporary
     * character buffer just to be 
     * safe
     */
    memset(nvm_buffer, NULL, sizeof(nvm_buffer));
    
    /**
     * Check the recipient email address
     */
    nvm_read_string(pref, PREF_EMAIL_RECIPIENT_KEY, nvm_buffer);
    if(sizeof(nvm_buffer) <= 1) {
        return false;
    }
    
    /**
     * Clear the temporary
     * character buffer just to be 
     * safe
     */
    memset(nvm_buffer, NULL, sizeof(nvm_buffer));
    
    /**
     * Check the author's email password
     */
    nvm_read_string(pref, PREF_EMAIL_AUTHOR_PASSWORD_KEY, nvm_buffer);
    if(sizeof(nvm_buffer) <= 1) {
        return false;
    }
 
    return true;
}   


