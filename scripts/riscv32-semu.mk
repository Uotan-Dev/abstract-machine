include $(AM_HOME)/scripts/isa/riscv.mk
include $(AM_HOME)/scripts/platform/semu.mk
CFLAGS  += -DISA_H=\"riscv/riscv.h\"
COMMON_CFLAGS += -march=rv32imf_zicsr -mabi=ilp32f   # overwrite
LDFLAGS       += -melf32lriscv                     # overwrite

AM_SRCS += riscv/semu/start.S \
           riscv/semu/cte.c \
           riscv/semu/trap.S \
           riscv/semu/vme.c
