#include "lan.h"

NVM         lan_nvm;
SENSOR      lan_sensor_instance;
LAN         lan_class;
APP         lan_app;

// WiFiClient Client; 
char  lan_buffer[PREF_BUFF_ELEMENTS]             = {NULL};

void LAN::init(void) 
{
  if(WIFI_LOGGING) {
    Serial.println("^LAN initalized");
  }

}

bool LAN::WiFiConnect( const char * ssid, const char * password ) 
{
 
  int i = 0;
  int timeout = (int)WIFI_CONNECT_TIMEOUT_S/0.5;
  
  
  if(WIFI_LOGGING) {
    Serial.println("Calling Wifi.begin().");
  }
  WiFi.begin(ssid, password);

  // Wait for connection
  while ((WiFi.status() != WL_CONNECTED) && i < timeout) {
    delay(500);
    i++;
    if(WIFI_LOGGING) {
      Serial.print('.');
    }
  }

  /**
   * Print diagnostic data
   * for WiFi if logging
   * is enabled
   */
  if(WIFI_LOGGING) {
    Serial.println(' ');
    Serial.print("\tMy MAC address is: "); Serial.println(WiFi.macAddress());
    Serial.print("\tConnecting to SSID: "); Serial.println(ssid ? ssid : "<null>");
    Serial.print("\tSSID password: "); Serial.println(password ? password : "<null>");
  }
  
  if(WIFI_LOGGING) {
    Serial.println("");
  }

  if(i == timeout){
  #if defined(WIFI_LOGGING)
      Serial.println("\tWiFi Connection timeout!");
      return false;
  }
  #endif
  
  if(WIFI_LOGGING) 
  {
    delay(300);
    Serial.println("\tWiFi connected!");
    Serial.print("\tMy local IP: ");
    Serial.println(WiFi.localIP());
  }

  /**
   * Available ESP32 RF power parameters:
   * WIFI_POWER_19_5dBm    // 19.5dBm (For 19.5dBm of output, highest. Supply current ~150mA)
   * WIFI_POWER_19dBm      // 19dBm
   * WIFI_POWER_18_5dBm    // 18.5dBm
   * WIFI_POWER_17dBm      // 17dBm
   * WIFI_POWER_15dBm      // 15dBm
   * WIFI_POWER_13dBm      // 13dBm
   * WIFI_POWER_11dBm      // 11dBm
   * WIFI_POWER_8_5dBm     //  8dBm
   * WIFI_POWER_7dBm       //  7dBm
   * WIFI_POWER_5dBm       //  5dBm
   * WIFI_POWER_2dBm       //  2dBm
   * WIFI_POWER_MINUS_1dBm // -1dBm( For -1dBm of output, lowest. Supply current ~120mA)
   * 
   * Available ESP8266 RF power parameters:
   * 0    (for lowest RF power output, supply current ~ 70mA
   * 20.5 (for highest RF power output, supply current ~ 80mA
   */

  WiFi.setTxPower(WIFI_POWER_5dBm);


  return true;
}

/**
 * @brief Send the email
 * 
 * An app password was has been added to the 
 * email account thus allowing Gmail to be used.
 * 
 */
void LAN::send_email ( Preferences & pref, SENSOR & sensor_instance, bool fault_status )
{

  SMTPSession smtp; 


  if(WIFI_LOGGING) 
  {
    Serial.println("Now sending email.");
  }

  /*  Set the network reconnection option */
  MailClient.networkReconnect(true);

  /** Enable the debug via Serial port
   * 0 for no debugging
   * 1 for basic level debugging
   *
   * Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
   */
  // smtp.debug(0);  //TODO this is the line we want back in
  smtp.debug(1);

  /* Declare the Session_Config for user defined session credentials */
  Session_Config config;

  /* Set the session config */
  config.server.host_name   = SMTP_HOST;
  config.server.port        = SMTP_PORT;
  
  /**
   * Clear the temporary buffer
   */
  memset(lan_buffer, NULL, sizeof(lan_buffer));
  
  lan_nvm.nvm_read_string(pref, PREF_EMAIL_AUTHOR_KEY, lan_buffer);
  config.login.email        = lan_buffer;


  /**
   * Clear the temporary buffer
   */
  memset(lan_buffer, NULL, sizeof(lan_buffer));
  lan_nvm.nvm_read_string(pref, PREF_EMAIL_AUTHOR_PASSWORD_KEY, lan_buffer);
  config.login.password     = lan_buffer;
  config.login.user_domain  = "";
  
  /*
  Set the NTP config time
  For times east of the Prime Meridian use 0-12
  For times west of the Prime Meridian add 12 to the offset.
  Ex. American/Denver GMT would be -6. 6 + 12 = 18
  See https://en.wikipedia.org/wiki/Time_zone for a list of the GMT/UTC timezone offsets
  */
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;
 
  /* Declare the message class */
  SMTP_Message message;
 
 
  /**
  * Clear the temporary buffer
  */
  memset(lan_buffer, NULL, sizeof(lan_buffer));
  lan_nvm.nvm_read_string(pref, PREF_EMAIL_AUTHOR_KEY, lan_buffer);

 /* Set the message headers */
  if(!fault_status) 
  {
    message.sender.name = F("~~Freezer~~");
  }
  else
  {
    message.sender.name = F("~~Freezer Fault~~");
  }
  
  message.sender.email = lan_buffer;
  
  if(!fault_status) 
  {
    message.subject = F("Freezer Update");
  }
  else
  {
    message.subject = F("Freezer Fault");
  }
 
  /**
  * Clear the temporary buffer
  */
  memset(lan_buffer, NULL, sizeof(lan_buffer));
  lan_nvm.nvm_read_string(pref, PREF_EMAIL_RECIPIENT_KEY, lan_buffer);
  message.addRecipient(F("CJG"), lan_buffer);
 
 
  //Send raw text message
  String textMsg  = "";
  textMsg += "Freezer 1 temperature:" + String(sensor_instance.temp1_value) + " F\n\n";
  textMsg += "Freezer 2 temperature:" + String(sensor_instance.temp2_value) + " F\n\n";
  textMsg += "Hardware Version: " + String(HW_VERSION) + "\n";
  textMsg += "Software Version: " + String(SW_VERSION) + "\n";

  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;


  /* Connect to the server */
  if (!smtp.connect(&config))
  {
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn())
  {
    Serial.println("\nNot yet logged in.");
  }
  else
  {
    if (smtp.isAuthenticated())
    {
      Serial.println("\nSuccessfully logged in.");
    }
    else
    {
      Serial.println("\nConnected with no Auth.");
    }
  }

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
  {
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
  }
} /* END SEND EMAIL*/
