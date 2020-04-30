/*
 * LoRa E32-TTL-100
 * Send fixed transmission structured message to a specified point.
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
#define trigPin 4
#define echoPin 5

//unsigned long delaytime=500 ;
int k;
int b[5];
long duration;
int distance;
float totaldistance=146;
float percentage;

//void printParameters(struct Configuration configuration);
//void printModuleInformation(struct ModuleInformation moduleInformation);

//The setup function is called once at startup of the sketch
void setup()
{
   // Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
	Serial.begin(9600);
	while (!Serial) {
	   ; // wait for serial port to connect. Needed for native USB
   }
	delay(100);
  
	e32ttl.begin();
	// After set configuration comment set M0 and M1 to low
	// and reboot if you directly set HIGH M0 and M1 to program
	ResponseStructContainer c;
	c = e32ttl.getConfiguration();
	Configuration configuration = *(Configuration*) c.data;
	configuration.ADDL = 0x00;
	configuration.ADDH = 0x00;
	configuration.CHAN = 0x06;
	configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
	e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
	printParameters(configuration);
	// ---------------------------
}
struct Message {
    char type[5];
    char message[8];
    int temperature;
    int power;
} message;

int i = 0;
// The loop function is called in an endless loop
void loop()
{
	delay(2500);
	i++;

  ResponseStructContainer c;
  c = e32ttl.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
   
  for(k=0;k<5;k++)
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    // Trigger the sensor by setting the trigPin high for 10 microseconds:
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    // Calculate the distance:
    distance = duration*0.034/2;
    b[k]=distance;
    delay(1000);
  }
  distance= (b[0]+b[1]+b[2]+b[3]+b[4])/5;
  b[0]=0;
  b[1]=0;
  b[2]=0;
  b[3]=0;
  b[4]=0;
  percentage= map(distance,20,totaldistance,100,0);
  
	struct Message {
	    char type[5] = "LEVL";
	    char message[8] = "ModuleA";
	    byte temperature[4];
      int power[3];
	} message;

  int pwr = (configuration.OPTION.transmissionPower,DEC);
  DEBUG_PRINT(F("TX Power ")); DEBUG_PRINT(" -> "); DEBUG_PRINTLN(pwr);
  *(int*)(message.power) = pwr;
    
  if(percentage>=0&&percentage<100)
  {
    DEBUG_PRINT(F("Water level ")); DEBUG_PRINT(" -> "); DEBUG_PRINTLN(percentage);
    *(float*)(message.temperature) = percentage;
      
    ResponseStatus rs = e32ttl.sendFixedMessage(0,0,6,&message, sizeof(Message));
    DEBUG_PRINTLN(rs.getResponseDescription());
  }

  else
  {
    DEBUG_PRINT(F("Reading error ")); DEBUG_PRINT(" -> "); DEBUG_PRINTLN(F("0.0"));
    *(float*)(message.temperature) = 0.0;

    ResponseStatus rs = e32ttl.sendFixedMessage(0,0,6,&message, sizeof(Message));
    DEBUG_PRINTLN(rs.getResponseDescription());
  }
	
}

void printParameters(struct Configuration configuration) {
	DEBUG_PRINTLN("----------------------------------------");

	DEBUG_PRINT(F("HEAD : "));  DEBUG_PRINT(configuration.HEAD, BIN);DEBUG_PRINT(" ");DEBUG_PRINT(configuration.HEAD, DEC);DEBUG_PRINT(" ");DEBUG_PRINTLN(configuration.HEAD, HEX);
	DEBUG_PRINTLN(F(" "));
	DEBUG_PRINT(F("AddH : "));  DEBUG_PRINTLN(configuration.ADDH, BIN);
	DEBUG_PRINT(F("AddL : "));  DEBUG_PRINTLN(configuration.ADDL, BIN);
	DEBUG_PRINT(F("Chan : "));  DEBUG_PRINT(configuration.CHAN, DEC); DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.getChannelDescription());
	DEBUG_PRINTLN(F(" "));
	DEBUG_PRINT(F("SpeedParityBit     : "));  DEBUG_PRINT(configuration.SPED.uartParity, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.SPED.getUARTParityDescription());
	DEBUG_PRINT(F("SpeedUARTDatte  : "));  DEBUG_PRINT(configuration.SPED.uartBaudRate, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.SPED.getUARTBaudRate());
	DEBUG_PRINT(F("SpeedAirDataRate   : "));  DEBUG_PRINT(configuration.SPED.airDataRate, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.SPED.getAirDataRate());

	DEBUG_PRINT(F("OptionTrans        : "));  DEBUG_PRINT(configuration.OPTION.fixedTransmission, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.OPTION.getFixedTransmissionDescription());
	DEBUG_PRINT(F("OptionPullup       : "));  DEBUG_PRINT(configuration.OPTION.ioDriveMode, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.OPTION.getIODroveModeDescription());
	DEBUG_PRINT(F("OptionWakeup       : "));  DEBUG_PRINT(configuration.OPTION.wirelessWakeupTime, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.OPTION.getWirelessWakeUPTimeDescription());
	DEBUG_PRINT(F("OptionFEC          : "));  DEBUG_PRINT(configuration.OPTION.fec, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.OPTION.getFECDescription());
	DEBUG_PRINT(F("OptionPower        : "));  DEBUG_PRINT(configuration.OPTION.transmissionPower, BIN);DEBUG_PRINT(" -> "); DEBUG_PRINTLN(configuration.OPTION.getTransmissionPowerDescription());

	DEBUG_PRINTLN("----------------------------------------");

}
void printModuleInformation(struct ModuleInformation moduleInformation) {
	DEBUG_PRINTLN("----------------------------------------");
	DEBUG_PRINT(F("HEAD BIN: "));  DEBUG_PRINT(moduleInformation.HEAD, BIN);DEBUG_PRINT(" ");DEBUG_PRINT(moduleInformation.HEAD, DEC);DEBUG_PRINT(" ");DEBUG_PRINTLN(moduleInformation.HEAD, HEX);

	DEBUG_PRINT(F("Model no.: "));  DEBUG_PRINTLN(moduleInformation.model, HEX);
	DEBUG_PRINT(F("Version  : "));  DEBUG_PRINTLN(moduleInformation.version, HEX);
	DEBUG_PRINT(F("Features : "));  DEBUG_PRINTLN(moduleInformation.features, HEX);
	DEBUG_PRINTLN("----------------------------------------");

}
