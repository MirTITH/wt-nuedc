#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "in_handle_mode.h"
#include "dma.h"
#include <cassert>

class LcdFmc
{
private:
    volatile uint16_t *_cmd_address;
    volatile uint16_t *_data_address;
    SemaphoreHandle_t _sem = nullptr;
    DMA_HandleTypeDef *_dma;
    uint32_t _remaining_dma_data_num = 0;
    uint16_t *_next_dma_src;
    const uint32_t _MAX_DMA_DATA_NUM = 65535; // DMA 一次最大传输 65535 bytes

    virtual void WriteDataDmaCpltCallback() = 0;

public:
    /**
     * @param chip_select FMC 的 Chip Select，应为下列值之一：1,2,3,4
     * @param dma 使用 DMA 加速数据传输，不用则给 nullptr
     */
    LcdFmc(int chip_select, DMA_HandleTypeDef *dma = nullptr)
        : _dma(dma)
    {
        uint32_t lcd_base;
        switch (chip_select) {
            case 1:
                lcd_base = 0x60000000;
                break;
            case 2:
                lcd_base = 0x64000000;
                break;
            case 3:
                lcd_base = 0x68000000;
                break;
            case 4:
                lcd_base = 0x6C000000;
                break;

            default:
                while (1) {}
                break;
        }

        _cmd_address  = (uint16_t *)lcd_base;
        _data_address = (uint16_t *)(lcd_base + 0x3FFFFFE); // 将所有地址为都置1，这样无论 LCD Register Select 设为何值都有效
        _sem          = xSemaphoreCreateBinary();
        xSemaphoreGive(_sem);
    }

    ~LcdFmc()
    {
        vSemaphoreDelete(_sem);
    }

    /**
     * @brief 配置 DMA 回调函数
     * @note 请自行定义 void LcdFmc_DmaXferCpltCallback(DMA_HandleTypeDef * _hdma)
     * @note 并在那个函数中调用 LcdFmc::DmaXferCpltCallback()
     * @note 例如：
        void LcdFmc_DmaXferCpltCallback(DMA_HandleTypeDef *_hdma)
        {
            extern LcdSt7796 LCD;
            if (_hdma->Instance == LCD.GetDma()->Instance) {
                LCD.DmaXferCpltCallback();
            }
        }
     */
    void InitDma()
    {
        extern void LcdFmc_DmaXferCpltCallback(DMA_HandleTypeDef * _hdma);
        if (_dma != nullptr) {
            HAL_DMA_RegisterCallback(_dma, HAL_DMA_XFER_CPLT_CB_ID, LcdFmc_DmaXferCpltCallback);
        }
    }

    void DmaXferCpltCallback()
    {
        if (_remaining_dma_data_num > 0) {
            WriteDataDma(_next_dma_src, _remaining_dma_data_num);
        } else {
            WriteDataDmaCpltCallback();
        }
    }

    /**
     * @brief 给LCD IO 操作加锁，避免被其他线程和中断函数打断
     * @note 加锁后记得解锁，不支持嵌套加锁
     * @param tick 加锁最大等待时间。注意：在中断中不会等待。
     * @return BaseType_t pdTRUE if successfully locked, otherwise pdFALSE
     */
    BaseType_t Lock(TickType_t tick = portMAX_DELAY)
    {
        if (InHandlerMode()) {
            return xSemaphoreTakeFromISR(_sem, nullptr);
        } else {
            return xSemaphoreTake(_sem, tick);
        };
    }

    /**
     * @brief 给LCD IO 操作解锁，使得其他线程和中断可以操作 LCD IO
     */
    void UnLock()
    {
        if (InHandlerMode()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(_sem, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xSemaphoreGive(_sem);
        }
    }

    uint16_t ReadData()
    {
        return *_data_address;
    }

    void WriteCmd(uint16_t cmd)
    {
        *_cmd_address = cmd;
    }

    void WriteData(uint16_t data)
    {
        *_data_address = data;
    }

    void WriteDataDma(uint16_t *data, uint32_t data_num)
    {
        assert(_dma != nullptr);

        if (data_num <= _MAX_DMA_DATA_NUM) {
            HAL_DMA_Start_IT(_dma, (uint32_t)data, (uint32_t)_data_address, data_num);
            _remaining_dma_data_num = 0;
        } else {
            HAL_DMA_Start_IT(_dma, (uint32_t)data, (uint32_t)_data_address, _MAX_DMA_DATA_NUM);
            _remaining_dma_data_num = data_num - _MAX_DMA_DATA_NUM;
            _next_dma_src           = data + _MAX_DMA_DATA_NUM;
        }
    }

    DMA_HandleTypeDef *GetDma()
    {
        return _dma;
    }
};
