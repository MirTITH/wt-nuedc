#include "ads1256_device.hpp"

Ads1256 Ads(&hspi1,
            ADS_NDRDY_GPIO_Port, ADS_NDRDY_Pin,
            ADS_NRESET_GPIO_Port, ADS_NRESET_Pin);
