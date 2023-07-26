#include "ads1256_device.hpp"

Ads1256 VAds(&hspi3, VDrdy_GPIO_Port, VDrdy_Pin, nullptr, 0, VSync_GPIO_Port, VSync_Pin);
Ads1256 IAds(&hspi2, IDrdy_GPIO_Port, IDrdy_Pin, nullptr, 0, ISync_GPIO_Port, ISync_Pin);
