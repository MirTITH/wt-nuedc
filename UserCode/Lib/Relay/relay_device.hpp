#pragma once

#include "relay.hpp"

namespace relay
{
extern ElectricRelay BridgeA;       // On: A 桥母线继电器、驱动芯片供电继电器打开
extern ElectricRelay BridgeB;       // On: B 桥母线继电器、驱动芯片供电继电器打开
extern ElectricRelay LoadConnector; // On: 接通负载
extern ElectricRelay GridConnector; // On: 并网

} // namespace relay
