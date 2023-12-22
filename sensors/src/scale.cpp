#include "scale.h"

HX711 scale;
double tare_value = INITIAL_SCALE_TARE_VALUE;

bool initialize_scale()
{

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    scale.set_scale(-109000 / 1000);
    scale.power_down(); //sleep the scale until we're ready to use it
    DEBUG_PRINTLN("Scale ready for use");

    return true;
}

void tare_scale_by_value(){
    DEBUG_PRINTLN("Taring scale by value");
    zero_tare_value();
    tare_value = get_avg_reading(10);
    DEBUG_PRINTLN("Set new tare value");
}

void reset_tare_value(){
    DEBUG_PRINTLN("Resetting tare value");
    tare_value = INITIAL_SCALE_TARE_VALUE;
}

void zero_tare_value(){
    DEBUG_PRINTLN("Zeroing tare value");
    tare_value = 0.00;
}

double get_avg_reading(long readings)
{
    scale.power_up();
    delay(100); //not sure if necessary, sample code had a gap between power up and reading
    
    double avg_reading = scale.get_units(readings) - tare_value;
    
    scale.power_down();

    DEBUG_PRINTF1("Scale reading: %f\n", avg_reading);
    

    return avg_reading;
}