## Platform HPMicro support
This is an OpenSBI implementation running on HPMicro series chips. This implementation is based on SBI standard V0.2, supports mchtmr configuration and reading, supports console (default UART0), pmp configuration.

### Console
Console uses UART0 by default, that is, the peripheral address is 0xf0040000. The pinmux information is defined in board.c: setup_uart_pinmux and can be changed as needed. It can also be left blank for configuration in the previous bootloader

### Build
```
    make all CROSS_COMPILE=riscv32-unknown-elf- PLATFORM=hpmicro O={your build dir}
```
You can see platform/hpmicro/firmware/fw_jump in the build directory.

### Usage
- Write the **fw_jump.bin** into flash
- Use a bootloader to copy the OpenSBI binary into address **FW_TEXT_START**
- Use a bootloader to init SDRAM and then copy the kernel binary(or other binary running in S mode) to address **FW_JUMP_ADDR** 
- Use a bootloader to copy the DTB to address **FW_JUMP_FDT_ADDR**
- Make sure all memory access operation is done, ex use 'fence iorw, iorw' and 'fence.i', then set the pc to **FW_TEXT_START** to run OpenSBI.

### PMP Configuration
Under the default configuration, the PMP configuration is as follows:
|PMP1|0x01080000 - 0x010fffff|A, R, W, X|
|---|---|---|
|PMP2|0x40000000 - 0x41ffffff|A, R, W, X|
|PMP3|0x80000000 - 0x9fffffff| A, R, X|
|PMP4|0xe0000000 - 0xefffffff| A, R, W|
|PMP5|0xf0000000 - 0xffffffff| A, R, W|

### Description of config.mk Content
|FW_TEXT_START|Operating address/entry address of OpenSBI|
|---|----|
|FW_JUMP|y: Generate an OpenSBI image in the direct jump mode|
|FW_JUMP_ADDR|The entry address of the image jumping to the next stage|
|FW_JUMP_FDT_ADDR|The starting address of the device tree, which will be passed as the second parameter to the entry of the next stage image|
|FW_DYNAMIC|n: Do not generate the OpenSBI image in the DYNAMIC mode|
|FW_PAYLOAD|n: Do not generate the OpenSBI image in the PAYLOAD mode| 