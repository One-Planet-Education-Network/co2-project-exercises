#include <TheThingsNetwork.h> //LoRaWAN Library

#include <SoftwareSerial.h> // Software Serial Library
#include <DS3231.h>
#include <dht11.h>

const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

#define loraSerial Serial1
// Set frequency plan with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

SoftwareSerial loraSerialPins(8, 9); //Software Serial pins
#define debugSerial Serial
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);




dht11 DHT;
#define         MG_PIN                       (A0)
#define         BOOL_PIN                     (13)
#define         DC_GAIN                      (8.5)
#define         READ_SAMPLE_INTERVAL         (50)
#define         READ_SAMPLE_TIMES            (5)
#define         ZERO_POINT_VOLTAGE           (0.220)
#define         REACTION_VOLTGAE             (0.020)

float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};
float CO2PPM = 0.0;

DS3231  rtc(SDA, SCL);

void setup() {
    loraSerial.begin(57600);
    debugSerial.begin(9600);
    while (!debugSerial && millis() < 10000);
    debugSerial.println("LoRaWAN status:");
    ttn.showStatus();
    debugSerial.println("Join LoRaWAN network:");
    ttn.join(appEui, appKey); 
    rtc.begin();  
}

void loop() {
    getCO2Data();
    DHT.read(10);
    
    uint16_t humidity = DHT.humidity * 100;
    uint16_t temperature = DHT.temperature * 100;
    uint16_t co2concentration = CO2PPM;
    
    byte payload[6];
    payload[0] = highByte(temperature);
    payload[1] = lowByte(temperature);
    payload[2] = highByte(humidity);
    payload[3] = lowByte(humidity);
    payload[4] = highByte(co2concentration);
    payload[5] = lowByte(co2concentration);
    ttn.sendBytes(payload, sizeof(payload));
    
    delay(10000);
}


float getCO2Data(){
    float volts;
    volts = MGRead(MG_PIN);
    CO2PPM = MGGetPercentage(volts,CO2Curve);
    return CO2PPM;
}

float MGRead(int mg_pin){
    int i;
    float v=0;
 
    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;  
}

int  MGGetPercentage(float volts, float *pcurve){
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return -1;
   } else { 
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
}
