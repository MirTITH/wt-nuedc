#include "lcd_device.hpp"
#include "tim.h"
#include "dma.h"

LcdSt7796 LCD(1,
              LcdSt7796::Rotation::portrait,
              LcdSt7796::ColorOrder::BGR,
              &htim1, TIM_CHANNEL_1,
              &hdma_memtomem_dma2_stream1);
