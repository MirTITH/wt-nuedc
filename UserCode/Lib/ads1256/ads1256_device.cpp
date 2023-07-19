#include "ads1256_device.hpp"

Ads1256 VAds(&hspi3, VDrdy_GPIO_Port, VDrdy_Pin);
Ads1256 IAds(&hspi3, IDrdy_GPIO_Port, IDrdy_Pin);
