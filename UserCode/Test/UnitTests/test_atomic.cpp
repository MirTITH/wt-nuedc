#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include <atomic>
#include "thread_priority_def.h"
#include "freertos_lock/freertos_lock.hpp"
#include <mutex>

int kNormalVar            = 0;
volatile int kVolatileVar = 0;
std::atomic<int> kAtomicVar{0};
int kProtectedVar = 0;
#define TestedVar kAtomicVar

freertos_lock::BinarySemphr kLock{true};

void TestAtomicEntry(void *argument)
{
    auto count = static_cast<size_t *>(argument);

    auto start_us = HPT_GetUs();

    for (size_t i = 0; i < *count; i++) {
        // kNormalVar++;
        // kVolatileVar++;
        kAtomicVar++;
        // kLock.lock();
        // kProtectedVar++;
        // kLock.unlock();
    }

    os_printf("Duration: %lu\n", HPT_GetUs() - start_us);

    vTaskDelete(nullptr);
}

void StartTestAtomic()
{
    size_t count = 1000000;
    xTaskCreate(TestAtomicEntry, "test_atomic_1", 256, &count, PriorityNormal, nullptr);
    xTaskCreate(TestAtomicEntry, "test_atomic_2", 256, &count, PriorityNormal, nullptr);
    // vTaskDelay(1000);

    int last_var             = TestedVar;
    const int max_same_count = 5;
    int same_count           = 0;

    while (true) {
        os_printf("kNormalVar: %d, kVolatileVar: %d, kAtomicVar: %d, kProtectedVar: %d, same_count: %d\n", kNormalVar, kVolatileVar, kAtomicVar.load(), kProtectedVar, same_count);
        if (last_var == TestedVar) {
            same_count++;
        } else {
            same_count = 0;
            last_var   = TestedVar;
        }

        if (same_count >= max_same_count) {
            break;
        }

        vTaskDelay(10);
    }

    os_printf("is_lock_free: %d\n", kAtomicVar.is_lock_free());
}
