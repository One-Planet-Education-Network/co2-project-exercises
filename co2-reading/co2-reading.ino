#include <DS3231.h>

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
    Serial.begin(9600);
    rtc.begin();  
}

void loop() {
    getCO2Data();
    
    uint16_t co2concentration = CO2PPM;

    Serial.println("The CO2 concentration is ");
    Serial.println(co2concentration,1);
    delay(6000);
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
