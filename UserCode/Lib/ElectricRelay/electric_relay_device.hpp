#pragma once

#include "electric_relay.hpp"

extern ElectricRelay kER_BridgeA; // On: A 桥母线继电器、驱动芯片供电继电器打开
extern ElectricRelay kER_BridgeB; // On: B 桥母线继电器、驱动芯片供电继电器打开
extern ElectricRelay kER_LoadConnector; // On: 接通负载
extern ElectricRelay kER_GridConnector; // On: 并网
