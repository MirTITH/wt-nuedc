#include "relay_device.hpp"
#include "main.h"

namespace relay
{
ElectricRelay BridgeA(ElectricRelay_BridgeA_GPIO_Port, ElectricRelay_BridgeA_Pin);
ElectricRelay BridgeB(ElectricRelay_BridgeB_GPIO_Port, ElectricRelay_BridgeB_Pin);
ElectricRelay LoadConnector(ElectricRelay_LoadConnector_GPIO_Port, ElectricRelay_LoadConnector_Pin);
ElectricRelay GridConnector(ElectricRelay_GridConnector_GPIO_Port, ElectricRelay_GridConnector_Pin);
ElectricRelay GridResisterShorter(ElectricRelay_GridResisterShorter_GPIO_Port, ElectricRelay_GridResisterShorter_Pin);
ElectricRelay AllLoadConnector(ElectricRelay_AllLoad_GPIO_Port, ElectricRelay_AllLoad_Pin);

void CloseAllRelay()
{
    BridgeA.Set(Relay_State::Close);
    BridgeB.Set(Relay_State::Close);
    LoadConnector.Set(Relay_State::Close);
    GridConnector.Set(Relay_State::Close);
    GridResisterShorter.Set(Relay_State::Close);
    AllLoadConnector.Set(Relay_State::Close);
}

} // namespace relay
