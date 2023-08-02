#include "adc_class_device.hpp"

Adc Adc1(&hadc1);
Adc Adc3(&hadc3);

float GetCoreTemperature()
{
    return Adc1.GetTemperature(0);
}
