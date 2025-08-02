#include "sensor.h"

void SENSOR::init( void ) {

  this -> temp_1_buf_index = 0;
  this -> temp_2_buf_index = 0;
  this -> just_rebooted = true;

}

int SENSOR::TwosConvert(int x) {
  x ^= 0xFFFF;
  x += 0x0001;
  x &= 0x3FFF;    //Only care about the 14 temperature bits
  return(x);
}

bool SENSOR::get_temperature( void ) {

  int i                   = 0;  
  long raw_therm_reading  = 0;
  float temp1_sum          = 0.0;
  float temp2_sum          = 0.0;
  int signed_temp1_data    = 0;
  int signed_temp2_data    = 0;
  float temp1_value       = 0.0;
  float temp2_value       = 0.0;
  
  /**
   * Get temperature values
   * from both sensors
   */
  for (i=0; i<2; i++)
  {

    delay(1);                   //Delay in ms
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    (i == 0)?(digitalWrite(SPI1_CS,0)):(digitalWrite(SPI2_CS,0));     // Drop CS to enable the transaction
    delay(10);                   //Delay in ms
    
    raw_therm_reading = SPI.transfer(0x00);                               //Reads bits 31:24
    raw_therm_reading = (raw_therm_reading << 8) | SPI.transfer(0x00);    //Read bits 23:16
    raw_therm_reading = (raw_therm_reading << 8) | SPI.transfer(0x00);    //Read bits 15:8
    raw_therm_reading = (raw_therm_reading << 8) | SPI.transfer(0x00);    //Read bits 7:0
    
    SPI.endTransaction();
    delay(1);                   //Delay in ms
    (i == 0)?(digitalWrite(SPI1_CS,1)):(digitalWrite(SPI2_CS,1));     // Deselect the chip
    
    
    
    /* IF TEMP SENSOR ONE */
    if(i==0) 
    {
      
      signed_temp1_data = (int)((raw_therm_reading >> 18) & 0x3FFF);     // Going to pack the result as an integer 
      
      if(signed_temp1_data & (0x2000)) 
      {
        signed_temp1_data = TwosConvert(signed_temp1_data);
        temp1_value = float(signed_temp1_data/-4.0);
      }
      else 
      {
        temp1_value = float(signed_temp1_data / 4.0);
      }
      
      /* Convert Celsius to Fahrenheit */
      temp1_value = float((temp1_value * 1.8) + 32);
    }
    
    /* IF TEMP SENSOR TWO */
    else 
    {
      signed_temp2_data = (int)((raw_therm_reading >> 18) & 0x3FFF);     // Going to pack the result as an integer 
    
      if(signed_temp2_data & (0x2000)) 
      {
        signed_temp2_data = TwosConvert(signed_temp2_data);
        temp2_value = float(signed_temp2_data/-4.0);
      }
      else 
      {
        temp2_value = float(signed_temp2_data / 4.0);
      }
      
      /* Convert Celsius to Fahrenheit */
      temp2_value = float((temp2_value * 1.8) + 32);

    }
  }

  
  if(just_rebooted)
  {
    if(ENABLE_LOGGING)
    {
      Serial.println("^Just rebooted flag is set to true.");
    }
    this -> just_rebooted = false;
    
    for(i=0;i++;i<8)
    {
      this -> temp1_buffer[this -> temp_1_buf_index] = temp1_value;   
      (this -> temp_1_buf_index >= 7) ? (this -> temp_1_buf_index = 0):(this -> temp_1_buf_index++);
    }
    
    for(i=0;i++;i<8)
    {
      this -> temp2_buffer[this -> temp_2_buf_index] = temp2_value;   
      (this -> temp_2_buf_index >= 7) ? (this -> temp_2_buf_index = 0):(this -> temp_2_buf_index++);
    }
  }
  
  else
  {
    this -> temp1_buffer[this -> temp_1_buf_index] = temp1_value;   
    (this -> temp_1_buf_index >= 7) ? (this -> temp_1_buf_index = 0):(this -> temp_1_buf_index++);
    
    this -> temp2_buffer[this -> temp_2_buf_index] = temp2_value;   
    (this -> temp_2_buf_index >= 7) ? (this -> temp_2_buf_index = 0):(this -> temp_2_buf_index++);
  }
  
  temp1_sum = 0.0;   // Reset this value in preparation to calculate the average
  for(i=0; i<8; i++) 
  {
    temp1_sum += this -> temp1_buffer[i];
  }
  temp2_sum = 0.0;   // Reset this value in preparation to calculate the average
  for(i=0; i<8; i++) 
  {
    temp2_sum += this -> temp2_buffer[i];
  }

  this -> temp1_value = float(temp1_sum / 8);  
  this -> temp2_value = float(temp2_sum / 8);  


  return(true);

}
