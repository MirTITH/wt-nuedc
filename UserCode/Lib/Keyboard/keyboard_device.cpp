#include "keyboard_device.hpp"
#include "main.h"

KeyboardClass kKeyboard({Key_Addr0_GPIO_Port, Key_Addr0_Pin,
                         Key_Addr1_GPIO_Port, Key_Addr1_Pin,
                         Key_Addr2_GPIO_Port, Key_Addr2_Pin,
                         Key_Addr3_GPIO_Port, Key_Addr3_Pin},
                        {Key_Data_GPIO_Port, Key_Data_Pin});
