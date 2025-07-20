#include "sensor.h"

void SENSOR::init( void ) {

  this -> temp_buf_index = 0;
  this -> just_rebooted = true;

}

int SENSOR::TwosConvert(int x) {
  x ^= 0xFFFF;
  x += 0x0001;
  x &= 0x3FFF;    //Only care about the 14 temperature bits
  return(x);
}

float SENSOR::get_temperature( void ) {

  int i                   = 0;  
  long raw_therm_reading  = 0;
  float temp_sum          = 0.0;
  int signed_temp_data    = 0;
  float temp_value        = -0.0;
  
  
  delay(1);                   //Delay in ms
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(SPI_CS,0);     //Drop CS to enable the transaction
  delay(10);                   //Delay in ms

  raw_therm_reading = SPI.transfer(0x00);                               //Reads bits 31:24
  raw_therm_reading = (raw_therm_reading << 8) | SPI.transfer(0x00);    //Read bits 23:16
  raw_therm_reading = (raw_therm_reading << 8) | SPI.transfer(0x00);    //Read bits 15:8
  raw_therm_reading = (raw_therm_reading << 8) | SPI.transfer(0x00);    //Read bits 7:0
  
  SPI.endTransaction();
  delay(1);                   //Delay in ms
  digitalWrite(SPI_CS,1);     //CS back high now that SPI transaction is over
  
  
  signed_temp_data = (int)((raw_therm_reading >> 18) & 0x3FFF);     //Going to pack the result as an integer 

  if(signed_temp_data & (0x2000)) 
  {
    signed_temp_data = TwosConvert(signed_temp_data);
    temp_value = float(signed_temp_data/-4.0);
  }
  else 
  {
    temp_value = float(signed_temp_data / 4.0);
  }
  
  /* Convert Celsius to Fahrenheit */
  temp_value = float((temp_value * 1.8) + 32);
  
  // if(ENABLE_LOGGING)
  // {
  //   Serial.print("^Temp value: ");
  //   Serial.println(temp_value);
  // }

  if(just_rebooted)
  {
    if(ENABLE_LOGGING)
    {
      Serial.println("^Just rebooted flag is set to true.");
    }
    this -> just_rebooted = false;
    for(i=0;i++;i<8)
    {
      this -> temperature_buffer[this -> temp_buf_index] = temp_value;   
      (this -> temp_buf_index >= 7) ? (this -> temp_buf_index = 0):(this -> temp_buf_index++);
    }
  }
  
  else
  {
    this -> temperature_buffer[this -> temp_buf_index] = temp_value;   
    (this -> temp_buf_index >= 7) ? (this -> temp_buf_index = 0):(this -> temp_buf_index++);
  }
  
  
  temp_sum = 0.0;   // Reset this value in preparation to calculate the average
  for(i=0; i<8; i++) 
  {
    temp_sum += this -> temperature_buffer[i];
  }

  temp_value = float(temp_sum / 8);  


  return(temp_value);

}
