#include "app.h"
#include "sensor.h"

DISP        app_disp_functions;
APP         app_functions;
SENSOR      app_sensor_functions;

/**
 * Display parameters
 */    
// char app_temp_buffer[32]; 

void APP::init(void) 
{
  this -> State = STATE_SLEEP; 
}

void APP::state_handler(APP & app_instance ) 
{
  switch(this -> State) 
  {
    case STATE_SLEEP:

      this -> State = STATE_SLEEP;


      /**
       * Any code after this won't be accessed 
       */
    break;
    
    case STATE_READ_DATA:   
      this -> State = STATE_SLEEP;
      
    break;
    
    case STATE_UPDATE_DISPLAY:
      this -> State = STATE_SLEEP;

      
    break;
    
    case STATE_SEND_EMAIL:
      this -> State = STATE_SLEEP;
      
    break;

    default:
      this -> State = STATE_SLEEP;
    break;
  }
  
}