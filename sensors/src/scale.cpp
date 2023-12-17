#include "scale.h"

HX711 scale;
double tare_value = INITIAL_SCALE_TARE_VALUE;

void initialize_scale()
{

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    scale.set_scale(-109000 / 1000);
    scale.power_down(); //sleep the scale until we're ready to use it
    Serial.println("Scale ready for use");
}

void tare_scale_by_value(){
    Serial.println("Taring scale by value");
    zero_tare_value();
    tare_value = get_avg_reading(10);
    Serial.println("Set new tare value");
}

void reset_tare_value(){
    Serial.println("Resetting tare value");
    tare_value = INITIAL_SCALE_TARE_VALUE;
}

void zero_tare_value(){
    Serial.println("Zeroing tare value");
    tare_value = 0.00;
}

double get_avg_reading(long readings)
{
    scale.power_up();
    delay(100); //not sure if necessary, sample code had a few cycle gap between power up and reading
    
    double avg_reading = scale.get_units(readings) - tare_value;
    
    scale.power_down();

    Serial.print("Got reading:\t");
    Serial.println(avg_reading);

    return avg_reading;
}