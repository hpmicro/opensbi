## HPMicro系列SOC支持
这是一个运行在HPMicro系列芯片上的OpenSBI实现。本实现基于SBI标准V0.2，支持mchtmr的配置和读取，支持console(默认UART0)，pmp配置。
### 终端
Console默认使用UART0，即外设地址为0xf0040000，pinmux的配置建议在运行OpenSBI前配置完成(上一级bootloader中)，并且将相应的外设时钟打开。

### 构建
```
    make all CROSS_COMPILE=riscv32-unknown-elf- PLATFORM=hpmicro O={your build dir}
```
可以在构建目录中看到平台/hpmicro/firmware/fw_jump. bin。
### 用法
- 将**fw_jump.bin**写入FLASH
- 使用引导加载程序将OpenSBI二进制文件复制到地址**FW_TEXT_START**开始的区域
- 使用引导加载程序初始化SDRAM，然后将内核二进制文件（或其他以S模式运行的二进制文件）复制到地址**FW_JUMP_ADDR**
- 使用引导加载程序将DTB复制到地址**FW_JUMP_FDT_ADDR**
- 确保所有内存访问操作都完成，例如使用“fence iorw, iorw”和“fence.i”，然后将pc设置为**FW_TEXT_START**以运行OpenSBI。

### PMP配置
默认配置下PMP配置如下：
|PMP1|0x01080000-0x010fffff|A,R,W,X|
|---|---|---|
|PMP2|0x40000000-0x41ffffff|A,R,W,X|
|PMP3|0x80000000-0x9fffffff| A,R,X|
|PMP4|0xe0000000-0xefffffff| A,R,W|
|PMP5|0xf0000000-0xffffffff| A,R,W|

### config.mk内容说明
|FW_TEXT_START|OpenSBI运行地址/OpenSBI入口地址|
|---|----|
|FW_JUMP|y:生成直接跳转方式的OpenSBI镜像|
|FW_JUMP_ADDR|跳转到下一阶段的镜像的入口地址|
|FW_JUMP_FDT_ADDR|设备树起始地址，会作为第二个参数传递给下一阶段的镜像入口|
|FW_DYNAMIC|n:不生成DYNAMIC方式的OpenSBI镜像|
|FW_PAYLOAD|n:不生成PAYLOAD方式的OpenSBI镜像|
