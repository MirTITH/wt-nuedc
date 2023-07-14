#include "high_precision_time.h"
#include "main.h"

/****************
 * 用户设置部分 *
 ****************/

#define USE_FREERTOS             // 使用 FreeRTOS 时请打开该宏定义
#define HAL_TIMEBASE_htimx htim7 // HAL Timebase Source

/****************
 * 程序代码部分 *
 ****************/

#ifdef USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "in_handle_mode.h"

extern TIM_HandleTypeDef HAL_TIMEBASE_htimx;

#endif // USE_FREERTOS

static uint8_t kIsInited = 0; // 是否初始化了
static uint32_t kUs_uwTick;   // 1 uwTick 对应多少 us

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
    __HAL_TIM_DISABLE_IT(&HAL_TIMEBASE_htimx, TIM_IT_UPDATE);

    uint32_t result;
    uint32_t tick = uwTick;
    uint32_t kCNT = HAL_TIMEBASE_htimx.Instance->CNT;
    if (kCNT < 10) {
        // 定时器不久前溢出
        // 判断定时器中断标识是否存在
        if (__HAL_TIM_GET_FLAG(&HAL_TIMEBASE_htimx, TIM_FLAG_UPDATE) != RESET) {
            // 刚刚溢出，还没进中断，uwTick 还没递增
            result = (tick + 1) * kUs_uwTick + kCNT;
        } else {
            // 进过了中断，uwTick 已经递增
            result = tick * kUs_uwTick + kCNT;
        }
    } else {
        // 定时器很久前溢出，uwTick 有效
        result = tick * kUs_uwTick + kCNT;
    }

    __HAL_TIM_ENABLE_IT(&HAL_TIMEBASE_htimx, TIM_IT_UPDATE);
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
    while ((nowUs - startUs) < us) {
        nowUs = HPT_GetUs();
    }
}

void HPT_DelayMs(uint32_t ms)
{
    HPT_DelayUs(1000 * ms);
}
