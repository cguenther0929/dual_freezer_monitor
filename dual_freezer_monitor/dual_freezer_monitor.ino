/**
 *  @filename   :   freezer_monitor_esp32_c3.ino
 * 
 */
#include <U8x8lib.h>
#include <Arduino.h>
#include <Preferences.h>
#include "nvm.h"
#include "display.h"
#include "sensor.h"
#include "lan.h"
#include "app.h"
#include "console.h"

/**
 * Instantiate the Preferences class
 * which is used for NVM
 */ 
Preferences pref;   

char        app_wifi_pass[PREF_BUFF_ELEMENTS]       = {NULL};
char        app_ssid[PREF_BUFF_ELEMENTS]            = {NULL};

/**
 * Timer parameters
 */
unsigned int    ms_ticks_50           = 0;
unsigned int    ms_ticks_100          = 0;
unsigned int    ms_ticks_500          = 0;
unsigned int    ms_ticks_1000         = 0;

bool            Timer50msFlag         = false;
bool            Timer100msFlag        = false;
bool            Timer500msFlag        = false;
bool            Timer1000msFlag       = false;



char rx_char                          = '\n';

/**
 * Time structure 
 */
hw_timer_t *Hyg_Tmr = NULL;   

DISP        main_disp;  
SENSOR      sensor_instance;
CONSOLE     app_console;
LAN         app_lan;
APP         app;
NVM         app_nvm;


/**
 * @brief LCD parameters
 * @details This needs to appear 
 * here (i.e. it can't be hidden in a *.h)
*/
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

/**
 * @brief Timer interrupt
 * @details This function has to 'live' 
 * up above the setup routine
 * 
 */
void IRAM_ATTR onTimer()
{
  Timer50msFlag = true;

  if(ms_ticks_50 == 2) {
    ms_ticks_50 = 0;
    Timer100msFlag = true;
    if(ms_ticks_100 == 5) {
      ms_ticks_50 = 0;               //Reset centi-seconds
      Timer500msFlag = true;
    
      if(ms_ticks_500 == 2) {         //See if we need to roll over
        ms_ticks_500 = 0;
        Timer1000msFlag = true;  
      }
      else {
        ms_ticks_500++;              //Increase 500ms timer
      }

    }
    else {
        ms_ticks_100++;
    }
    
  }
  else {
    ms_ticks_50++;
  }
}


/**
 * @brief Arduino Setup routine
 */
void setup() 
{
  /* Initialize the SPI module */
  SPI.begin(SPI_SCK, PIN_MISO, PIN_MOSI, SPI1_CS); //The pins have to be mapped.
  
  /* Pin direction for health LED*/
  pinMode(HEALTH_LED, OUTPUT);

  /* Pin directions for HW revision bits */
  pinMode(HW_REV_BIT_0, INPUT);
  pinMode(HW_REV_BIT_1, INPUT);
  pinMode(HW_REV_BIT_2, INPUT);
  
  /* Pin direction for SPI CS*/
  pinMode(SPI1_CS, OUTPUT);
  digitalWrite(SPI1_CS, HIGH);       //The CS line shall default to high
  
  pinMode(SPI2_CS, OUTPUT);
  digitalWrite(SPI2_CS, HIGH);       //The CS line shall default to high

  
  Serial.begin(SERIAL_BAUD_RATE);
  
  /**
   * Remaining initialization functions
   */
  if(ENABLE_LOGGING)
  {
    Serial.println("^Calling remaining initialization functions");
  }
  
  main_disp.init(u8x8);          
  app_lan.init();
  app.init();
  sensor_instance.init();
  app_nvm.init();
  
  //Initialize timer interrupt
  //                     The frequency of the timer   
  //                       |     
  Hyg_Tmr = timerBegin(1000000);
  
  //                 Name of timer (from above) 
  //                     |      Name of callback function       
  //                     |        |     true (the tutorial did not indicate what this mans)
  //                     |        |        |     
  timerAttachInterrupt(Hyg_Tmr,&onTimer);
  
  //       This is the timer struct 
  //           |        This is the alarm value (so alarm when we count up to this value).  This was 50000, but empirically, it seemed 100000 was a better fit (2×) 
  //           |          |    true = to tell the timer to reload 
  //           |          |      |  Value to reload into the timer when auto reloading
  //           |          |      |   |
  timerAlarm(Hyg_Tmr, 100000, true,0);

}

/**
 * @brief Arduino main loop
 */
void loop() 
{

  Serial.println("====================== Reset ======================");
  app.temperature_fault = false;

  sensor_instance.trip_threshold = app_nvm.nvm_get_float(pref,PREF_THRESHOLD);
  if( sensor_instance.trip_threshold > 32 ||
      sensor_instance.trip_threshold < -20)
  {
    sensor_instance.trip_threshold = MAX_ALLOWED_TEMP;
  }

  while (true)
  {

    if(Timer50msFlag == true) 
    {
      
      Timer50msFlag = false;
      
      rx_char = Serial.read();
      
      if (rx_char == 'z'){
        app_console.console(pref, sensor_instance);    
        Timer100msFlag = false;
        Timer500msFlag = false;
        Timer1000msFlag = false;
      }
      
    }
    
    if(Timer100msFlag == true) 
    {
      sensor_instance.get_temperature(); // Get temperature from both sensors
      if(sensor_instance.temp1_value >= MAX_ALLOWED_TEMP || 
         sensor_instance.temp2_value >= MAX_ALLOWED_TEMP) 
      {
        app.timer_enabled = true;
        app.temperature_fault = true;
        if(ENABLE_LOGGING)
        {
          Serial.println("^Fault flag has been set.");
        }
      }
      else if(sensor_instance.temp1_value < (MAX_ALLOWED_TEMP - TEMP_FAULT_HYSTERESIS) &&
              sensor_instance.temp2_value < (MAX_ALLOWED_TEMP - TEMP_FAULT_HYSTERESIS) &&
              app.temperature_fault) 
      {
        app.immediate_fault_email_sent = false;  // Possible reset 
        app.timer_enabled = false;
        app.temperature_fault = false;
        if(ENABLE_LOGGING)
        {
          Serial.println("^Fault flag and timer disabled.");
          Serial.println("^Immediate fault email flag has been cleared.");
        }
      }
      
      Timer100msFlag = false;
    }
    
    if(Timer500msFlag == true) 
    {
      Timer500msFlag = false;
    }
    
    
    if(Timer1000msFlag == true) 
    {
      Timer1000msFlag = false;

      if(ENABLE_LOGGING)
        {
          Serial.print("^Temp1 and 2 values: ");
          Serial.print(sensor_instance.temp1_value);
          Serial.print("   ");
          Serial.println(sensor_instance.temp2_value);
        }

      if(app.heartbeat_enabled)
      {
        digitalWrite(HEALTH_LED,!digitalRead(HEALTH_LED));
      }

      if(app.timer_enabled)
      {
        app.timer_seconds_cnt++;
      }
      else 
      {
        app.timer_seconds_cnt = 0;
      }
      
      if(app.seconds_counter % SECONDS_BETWEEN_DISPLAY_UPDATES == 0)  
      {
        if(ENABLE_LOGGING)
        {
          Serial.println("^Display update");
        }
        main_disp.refresh_display(u8x8, sensor_instance);  
      }
      
      /**
       *  Send daily updated email 
       */
      if((app.seconds_counter % SECONDS_BETWEEN_EMAIL == 0) &&
        !app.temperature_fault)  
      {

        if(ENABLE_LOGGING)
        {
          Serial.print("Seconds counter: ");
          Serial.println(app.seconds_counter);

          Serial.print("^Seconds Counter modulo with seconds between emails: ");
          Serial.println(app.seconds_counter % SECONDS_BETWEEN_EMAIL);
          
          Serial.println("^Sending daily update email");
        }

        if(app_nvm.network_valid(pref))
        {
          /**
           * Clear the temporary
           * character buffer just to be 
           * safe
           */
          memset(app_ssid, NULL, sizeof(app_ssid));
          
          /**
           * Get the SSID of the router 
           */
          app_nvm.nvm_read_string(pref, PREF_WIFI_SSID, app_ssid);
          
          
          /**
           * Clear the temporary
           * character buffer just to be 
           * safe
           */
          memset(app_wifi_pass, NULL, sizeof(app_wifi_pass));
          
          /**
           * Get the router's password 
           */
          app_nvm.nvm_read_string(pref, PREF_WIFI_PASSWORD, app_wifi_pass);
          
          app_lan.WiFiConnect(app_ssid,app_wifi_pass);  
          app_lan.send_email(pref, sensor_instance, false);  
          
        }
      }

      /**
       * Send immediate fault email
       */
      if(!app.immediate_fault_email_sent &&
          app.temperature_fault)
      {
        
        if(ENABLE_LOGGING)
        {
          Serial.println("^Sending immediate fault email");
        }
        
        app.immediate_fault_email_sent = true;

        /**
         * Clear the temporary
         * character buffer just to be 
         * safe
         */
        memset(app_ssid, NULL, sizeof(app_ssid));
        
        /**
         * Get the SSID of the router 
         */
        app_nvm.nvm_read_string(pref, PREF_WIFI_SSID, app_ssid);
        
        
        /**
         * Clear the temporary
         * character buffer just to be 
         * safe
         */
        memset(app_wifi_pass, NULL, sizeof(app_wifi_pass));
        
        /**
         * Get the router's password 
         */
        app_nvm.nvm_read_string(pref, PREF_WIFI_PASSWORD, app_wifi_pass);
        
        app_lan.WiFiConnect(app_ssid,app_wifi_pass);  
        app_lan.send_email(pref, sensor_instance,true);  

      }


      /**
      * Send periodic fault email
      */
      if(app.timer_seconds_cnt % SECONDS_BETWEEN_FAULT_EMAIL == 0 && app.temperature_fault)
      {
        if(ENABLE_LOGGING)
        {
          Serial.println("^Sending periodic fault email");
          Serial.print("^Seconds counter: ");
          Serial.println(app.timer_seconds_cnt);
        }
        
        /**
         * Clear the temporary
         * character buffer just to be 
         * safe
         */
        memset(app_ssid, NULL, sizeof(app_ssid));
        
        /**
         * Get the SSID of the router 
         */
        app_nvm.nvm_read_string(pref, PREF_WIFI_SSID, app_ssid);
        
        
        /**
         * Clear the temporary
         * character buffer just to be 
         * safe
         */
        memset(app_wifi_pass, NULL, sizeof(app_wifi_pass));
        
        /**
         * Get the router's password 
         */
        app_nvm.nvm_read_string(pref, PREF_WIFI_PASSWORD, app_wifi_pass);
        
        app_lan.WiFiConnect(app_ssid,app_wifi_pass);  
        app_lan.send_email(pref, sensor_instance, true);  
      }


      /**
       * Update Seconds Counter
       */

      app.seconds_counter++;
      if(app.seconds_counter >= 100000)  
      {      
        app.seconds_counter = 0; 
      }
    } /* IF Timer1000msFlag */
  } /* WHILE LOOP */
} /* MAIN ARDUINO LOOP */
