# cin和scanf清除输入缓冲区

建议在每次使用 cin 或 scanf 之前都清除缓冲区，否则如果缓冲区内有非法字符，则会失败

以下为清除方法：

## cin

```c++
cin.clear(); // 使流数据重新处于有效状态
cin.ignore(); // 清空缓存区数据
```

## scanf

```c
fflush(stdin);
```
