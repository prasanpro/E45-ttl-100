#include "Arduino.h"
#include "LoRa_E45.h"

// ---------- Arduino pins --------------
LoRa_E32 e32ttl(2, 3, 6, 7, 8);
//LoRa_E32 e32ttl(2, 3); // Config without connect AUX and M0 M1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB
    }
  delay(100);

  e32ttl.begin();

  //e32ttl.resetModule();
  // After set configuration comment set M0 and M1 to low
  // and reboot if you directly set HIGH M0 and M1 to program
  ResponseStructContainer c;
  c = e32ttl.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  configuration.ADDL = 0;
  configuration.ADDH = 0;
  configuration.CHAN = 0x04;
  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;

  e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  printParameters(configuration);
  // ---------------------------
  Serial.println();
  Serial.println("Start listening!");
}

struct Message {
    char type[6];
    char message[11];
    byte level[5];
}message;


void loop() {
  // put your main code here, to run repeatedly:
  if (e32ttl.available()  > 1){
    //Serial.println("received somethings");
    ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(Message));
    struct Message message = *(Message*) rsc.data;
    Serial.println(message.type);
    //Serial.println (sizeof(Message));
    float waterLevel = *(float*)(message.level);
    Serial.println(waterLevel);
    //Serial.println(message.message);
    if(waterLevel > 60)
    {
      Serial.println("Send message to Module C");
    }
    free(rsc.data);
    }
  /*  else if (typeStr == "HUM"){
      ResponseStructContainer rsc = e32ttl.receiveMessage(sizeof(MessageHumidity));
      struct MessageHumidity message = *(MessageHumidity*) rsc.data;

      Serial.println(message.humidity);
      Serial.println(message.message);
      free(rsc.data);
    }
    */
    //else{
     // Serial.println("Something goes wrong!!");
    //}
  //}
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
