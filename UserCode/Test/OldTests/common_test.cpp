#pragma once

#include "main.h"
#include "freertos_io/os_printf.h"
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <vector>
#include <list>
#include "HighPrecisionTime/high_precision_time.h"
#include <algorithm>

using namespace std;

void ListTest()
{
    os_printf("ListTest:\n");
    list<int> test_list;
    uint32_t start_us, duration;

    start_us = HPT_GetUs();
    for (size_t i = 0; i < 10; i++) {
        test_list.push_back(i);
    }
    duration = HPT_GetUs() - start_us;
    os_printf("Pushed back %u elements in %lu us.\n", test_list.size(), duration);

    // int sum  = 0;
    // for (size_t i = 0; i < test_list.size(); i++) {
    //     sum += test_list[i];
    // }
    for (auto &var : test_list) {
        // sum += var;
        os_printf("%d\t", var);
    }
    os_printf("\n");

    start_us = HPT_GetUs();
    rotate(test_list.begin(), ++test_list.begin(), test_list.end());
    duration = HPT_GetUs() - start_us;
    os_printf("rotate in %lu us\n", duration);

    for (auto &var : test_list) {
        os_printf("%d\t", var);
    }
    os_printf("\n");
}

void VectorTest()
{
    os_printf("VectorTest:\n");
    vector<int> test_vec;

    uint32_t start_us, duration;

    start_us = HPT_GetUs();

    for (size_t i = 0; i < 1000; i++) {
        test_vec.push_back(i);
    }

    duration = HPT_GetUs() - start_us;
    os_printf("Pushed back %u elements in %lu us.\n", test_vec.size(), duration);

    start_us = HPT_GetUs();
    int sum  = 0;
    // for (size_t i = 0; i < test_vec.size(); i++) {
    //     sum += test_vec[i];
    // }

    for (auto &var : test_vec) {
        sum += var;
    }
    duration = HPT_GetUs() - start_us;
    os_printf("Vector sum in %lu us. Sum result: %d\n", duration, sum);
}
