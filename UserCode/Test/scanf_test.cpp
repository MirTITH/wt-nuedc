#include "freertos_io/os_printf.h"
#include <stdio.h>

void ScanfTest()
{
    float a, b, c;

    while (true) {
        os_printf("请输入三个数字：\n");
        fflush(stdin);
        auto num = scanf("%f, %f, %f", &a, &b, &c);
        os_printf("接收到了 %d 个数字\n", num);
        os_printf("这三个数字是：%g, %g, %g\n", a, b, c);
    }
}
