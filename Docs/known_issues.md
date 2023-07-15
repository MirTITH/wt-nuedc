# Known issues

## cin和scanf清除输入缓冲区

建议在每次使用 cin 或 scanf 之前都清除缓冲区，否则如果缓冲区内有非法字符，则会失败。以下为清除方法：

cin:

```c++
cin.clear(); // 使流数据重新处于有效状态
cin.ignore(); // 清空缓存区数据
```

scanf:

```c
fflush(stdin);
```

## scanf 如果末尾有 `\n` 则输入第二行时才会读取上一行的信息

这是标准库的特性，所以 scanf 后面不要加 '\n'

## printf 线程不安全，调用前请先上锁

## 使用 gcc 10.3.1 版本，在不使用 --specs=nano.specs 参数时，cout 无法打印出东西

升级到 12.2.1 版本后解决，怀疑是编译器 bug

## 内存屏障

编译器和 cpu 都可能将指令重新排序，从而提高运行速度

这可能导致实际内存读写顺序和代码中的不一样

```
只作用于编译器的：
__asm("" ::: "memory") // For GCC
防止编译器跨过这条语句进行指令重排，但 cpu 仍然可能乱序执行

同时作用于编译器和 cpu 的：
__DMB()
数据内存屏障，使用该指令，可以保证前面的内存访问操作完毕后才进行后续的内存访问。（对于有Cache的情况，该指令并不会导致Cache与内存之间的内存同步，下面的指令同）

__DSB()
数据同步屏障，相比DMB更严格，可以保证前面的内存访问操作完毕后才进行后续的指令执行。 不仅后续的内存访问不会提前，就连指令也不会。

__ISB()
这个似乎比较特殊，它会清除指令流水线中的所有指令，使得流水线重新从内存或Cache中读取数据。
```

不想深究的话一般使用 __DSB()

> 在 high_precision_time 中有使用内存屏障，注释掉就通不过测试了
>