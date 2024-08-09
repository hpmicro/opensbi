#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2019 Andes Technology Corporation
#               2024 HPMicro
#
# Authors:
#   Zong Li <zong@andestech.com>
#   Nylon Chen <nylon7@andestech.com>
#   Zihan Xu <zihan.xu@hpmicro.com>

# Compiler flags
platform-cppflags-y =
platform-cflags-y =
ifneq ($(HPMSOC),)
platform-cflags-y += -D$(HPMSOC)=1
endif
platform-asflags-y =
platform-ldflags-y =

PLATFORM_RISCV_ISA = rv32imac_zifencei_zicsr

# Blobs to build
FW_TEXT_START=0x00000000

FW_DYNAMIC=n

FW_JUMP=y
ifeq ($(PLATFORM_RISCV_XLEN), 32)
  FW_JUMP_ADDR=0x40000000
else
  FW_JUMP_ADDR=0x40000000
endif
FW_JUMP_FDT_ADDR=0x40300000

FW_PAYLOAD=n
ifeq ($(PLATFORM_RISCV_XLEN), 32)
  FW_PAYLOAD_OFFSET=0x14000
else
  FW_PAYLOAD_OFFSET=0x14000
endif

# FW_PAYLOAD_FDT_ADDR=0x2000000
