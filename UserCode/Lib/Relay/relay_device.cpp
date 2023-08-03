#include "relay_device.hpp"
#include "main.h"

namespace relay
{
ElectricRelay BridgeA(ElectricRelay_BridgeA_GPIO_Port, ElectricRelay_BridgeA_Pin);
ElectricRelay BridgeB(ElectricRelay_BridgeB_GPIO_Port, ElectricRelay_BridgeB_Pin);
ElectricRelay LoadConnector(ElectricRelay_LoadConnector_GPIO_Port, ElectricRelay_LoadConnector_Pin);
ElectricRelay GridConnector(ElectricRelay_GridConnector_GPIO_Port, ElectricRelay_GridConnector_Pin);
} // namespace relay
