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

void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);

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
} message;

int i = 0;
// The loop function is called in an endless loop
void loop()
{
	delay(2500);
	i++;
  
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
	    char type[5] = "TEMP";
	    char message[8] = "Kitchen";
	    byte temperature[4];
	} message;

  if(percentage>=0&&percentage<100)
  {
    Serial.println (percentage);
    *(float*)(message.temperature) = percentage;

    ResponseStatus rs = e32ttl.sendFixedMessage(0,0,6,&message, sizeof(Message));
    Serial.println(rs.getResponseDescription());
  }

  else
  {
    Serial.println ("error");
    *(float*)(message.temperature) = 0.0;

    ResponseStatus rs = e32ttl.sendFixedMessage(0,0,6,&message, sizeof(Message));
    Serial.println(rs.getResponseDescription());
  }
	
}

void printParameters(struct Configuration configuration) {
	Serial.println("----------------------------------------");

	Serial.print(F("HEAD : "));  Serial.print(configuration.HEAD, BIN);Serial.print(" ");Serial.print(configuration.HEAD, DEC);Serial.print(" ");Serial.println(configuration.HEAD, HEX);
	Serial.println(F(" "));
	Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, BIN);
	Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, BIN);
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
