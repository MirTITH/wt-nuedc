#include "ads1256_device.hpp"

Ads1256 VAds(&hspi3, VDrdy_GPIO_Port, VDrdy_Pin, VAds_nReset_GPIO_Port, VAds_nReset_Pin, VSync_GPIO_Port, VSync_Pin);
Ads1256 IAds(&hspi2, IDrdy_GPIO_Port, IDrdy_Pin, IAds_nReset_GPIO_Port, IAds_nReset_Pin, ISync_GPIO_Port, ISync_Pin);
