#include "high_precision_time.h"

/****************
 * 用户开关部分 *
 ****************/

#define USE_FREERTOS // 使用 FreeRTOS 时请打开该宏定义

/****************
 * 程序代码部分 *
 ****************/

#include "main.h"

#ifdef USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "in_handle_mode.h"

#define HAL_TIMEBASE_htimx htim7 // Timebase Source
extern TIM_HandleTypeDef HAL_TIMEBASE_htimx;

#endif // USE_FREERTOS

static uint8_t kIsInited = 0; // 是否初始化了
static uint32_t kUs_uwTick;   // 1 uwTick 对应多少 us

static uint32_t kCNT;

#ifdef USE_FREERTOS

static uint32_t kSysTick_xTickCount; // 1 xTickCount 对应多少 SysTick

void HPT_Init()
{
    kUs_uwTick          = 1000 * uwTickFreq;
    kSysTick_xTickCount = SysTick->LOAD + 1;
    kIsInited           = 1;
}

uint32_t HPT_GetUs()
{
    kCNT            = HAL_TIMEBASE_htimx.Instance->CNT;
    uint32_t result = uwTick * kUs_uwTick + kCNT;

    return result;
}

uint32_t HPT_GetTotalSysTick()
{
    return xTaskGetTickCount() * kSysTick_xTickCount + (SysTick->LOAD - SysTick->VAL);
}

#else
static uint32_t kSysTick_Us;     // 1 us 对应多少 SysTick
static uint32_t kSysTick_uwTick; // 1 uwTick 对应多少 SysTick

void HPT_Init()
{
    kUs_uwTick      = 1000 * uwTickFreq;
    kSysTick_Us     = SystemCoreClock / 1000000;
    kSysTick_uwTick = SysTick->LOAD + 1;
    kIsInited       = 1;
}

uint32_t HPT_GetUs()
{
    return uwTick * kUs_uwTick + (SysTick->LOAD - SysTick->VAL) / kSysTick_Us;
}

uint32_t HPT_GetTotalSysTick()
{
    return uwTick * kSysTick_uwTick + (SysTick->LOAD - SysTick->VAL);
}

#endif // USE_FREERTOS

void HPT_DelayUs(uint32_t us)
{
    uint32_t startUs = HPT_GetUs();
    uint32_t nowUs   = HPT_GetUs();
    assert(startUs <= nowUs);
    while ((nowUs - startUs) < us) {
        nowUs = HPT_GetUs();
    }
    assert(startUs <= nowUs);
}

void HPT_DelayMs(uint32_t ms)
{
    HPT_DelayUs(1000 * ms);
}
