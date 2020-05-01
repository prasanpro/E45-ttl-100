/*
 * LoRa E32-TTL-100
 * Receive fixed transmission message on channel.
 * https://www.mischianti.org
 *
 * E32-TTL-100----- Arduino UNO or esp8266
 * M0         ----- 3.3v (To config) GND (To send) 7 (To dinamically manage)
 * M1         ----- 3.3v (To config) GND (To send) 6 (To dinamically manage)
 * RX         ----- PIN 2 (PullUP & Voltage divider)
 * TX         ----- PIN 3 (PullUP)
 * AUX        ----- Not connected (5 if you connect)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */
#include "Arduino.h"
#include "LoRa_E45.h"

// ---------- Arduino pins --------------
LoRa_E32 e32ttl(2, 3, 6, 7, 8);
//LoRa_E32 e32ttl(2, 3, 4); // Config without connect AUX and M0 M1

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB
    }
  delay(100);
 
  defaultConfig();
}

void defaultConfig()
{
  e32ttl.begin();
   Serial.println("Begin the module!!");
  //e32ttl.resetModule();
  // After set configuration comment set M0 and M1 to low
  // and reboot if you directly set HIGH M0 and M1 to program
  ResponseStructContainer c;
  c = e32ttl.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  configuration.HEAD = 0xC2;
  configuration.ADDL = 0x03;
  configuration.ADDH = 0x00;
  configuration.CHAN = 0x04;
  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.OPTION.transmissionPower = POWER_20;
  configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;
  configuration.OPTION.fec = FEC_1_ON;
  configuration.SPED.uartParity = MODE_00_8N1;
  configuration.SPED.uartBaudRate = UART_BPS_57600;
  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
  
  e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  printParameters(configuration);
  // ---------------------------
  Serial.println();
  Serial.println("Start listening!");
  
    //e32ttl.setMode(MODE_2_POWER_SAVING);
    //loop();
}

struct Message {
    char message[8];
    byte temperature[4];
    int power[3];
};

struct Control {
    char type[5];
    char control[8];
    int alert;
} control;


// The loop function is called in an endless loop
void loop()
{

    
	while (e32ttl.available() > 1){
    //Serial.println("Received something");
    
    char type[5]; // first part of structure
    //Serial.println("char defined");
		ResponseContainer rs = e32ttl.receiveInitialMessage(sizeof(type));
    //Serial.println("contained taken");
                // Put string in a char array (not needed)
//		memcpy ( type, rs.data.c_str(), sizeof(type) );
		String typeStr = rs.data;
   //Serial.println("data saved");
    //int respon = rs.status.code;
    //Serial.println("response saved");
		Serial.println(typeStr);
    //Serial.println(respon);
		if (typeStr=="LEVL"){
			ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Message));
			struct Message message = *(Message*) rsc.data;
      Serial.println(message.message);
      float waterLevel = *(float*)(message.temperature);
			Serial.println(waterLevel);
      int pwr = *(float*)(message.power);
      Serial.println("Received power->");
      Serial.println(getTransmissionPowerDescriptionByParams(pwr));
      free(rsc.data);
      if(waterLevel > 60)
      {
        Serial.println("Send message to Module C");
        delay(2000);
          struct Control {
          char type[5] = "ALRT";
          char control[8] = "ModuleB";
          int alert[1];
          } control;
        e32ttl.resetModule();
        ResponseStructContainer c;
        c = e32ttl.getConfiguration();
        Configuration configuration = *(Configuration*) c.data;
        configuration.HEAD = 0xC2;
        configuration.ADDL = 0x01;
        configuration.ADDH = 0x00;
        configuration.CHAN = 0x04;
        configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
        configuration.OPTION.transmissionPower = POWER_20;
        configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;
        configuration.OPTION.fec = FEC_1_ON;
        configuration.SPED.uartParity = MODE_00_8N1;
        configuration.SPED.uartBaudRate = UART_BPS_57600;
        configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
        e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
        printParameters(configuration);
        delay(2000);
        
        *(int*)(control.alert) = 1;
        //Serial.println (sizeof(Message));
        ResponseStatus rs = e32ttl.sendFixedMessage(0,4,0x04,&control, sizeof(Control));
        Serial.println(rs.getResponseDescription());
        delay(2000);
      //free(rsc.data);  
      //e32ttl.flush();
      defaultConfig();
      break;  
      }
			//free(rsc.data);
		}
		/*
		else if (typeStr == "HUM"){
			ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(MessageHumidity));
			struct MessageHumidity message = *(MessageHumidity*) rsc.data;

			Serial.println(message.humidity);
			Serial.println(message.message);
			free(rsc.data);
		}
   */
		else{
    //TODO: Upon second entry program falls here
    //Serial.println("Something goes wrong!!");
      //e32ttl.flush();
      break;
		}
	}
}

void printParameters(struct Configuration configuration) {
	Serial.println("----------------------------------------");

	Serial.print(F("HEAD : "));  Serial.print(configuration.HEAD, BIN);Serial.print(" ");Serial.print(configuration.HEAD, DEC);Serial.print(" ");Serial.println(configuration.HEAD, HEX);
	Serial.println(F(" "));
	Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, DEC);
	Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, DEC);
	Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
	Serial.println(F(" "));
	Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
	Serial.print(F("SpeedUARTDatte  : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRate());
	Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRate());

	Serial.print(F("OptionTrans        : "));  Serial.print(configuration.OPTION.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFixedTransmissionDescription());
	Serial.print(F("OptionPullup       : "));  Serial.print(configuration.OPTION.ioDriveMode, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getIODroveModeDescription());
	Serial.print(F("OptionWakeup       : "));  Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
	Serial.print(F("OptionFEC          : "));  Serial.print(configuration.OPTION.fec, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFECDescription());
	Serial.print(F("OptionPower        : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());

	Serial.println("----------------------------------------");

}
void printModuleInformation(struct ModuleInformation moduleInformation) {
	Serial.println("----------------------------------------");
	Serial.print(F("HEAD BIN: "));  Serial.print(moduleInformation.HEAD, BIN);Serial.print(" ");Serial.print(moduleInformation.HEAD, DEC);Serial.print(" ");Serial.println(moduleInformation.HEAD, HEX);

	Serial.print(F("Model no.: "));  Serial.println(moduleInformation.model, HEX);
	Serial.print(F("Version  : "));  Serial.println(moduleInformation.version, HEX);
	Serial.print(F("Features : "));  Serial.println(moduleInformation.features, HEX);
	Serial.println("----------------------------------------");

}

String getTransmissionPowerDescriptionByParams(int transmissionPower)
  {
    switch (transmissionPower)
    {
      case POWER_20:
      return F("20dBm (Default)");
      break;
      case POWER_17:
      return F("17dBm");
      break;
      case POWER_14:
      return F("14dBm");
      break;
      case POWER_10:
      return F("10dBm");
      break;
      default:
      return F("Invalid transmission power param");
    }
  }

  void configurationDefault()
  {
    
  }
