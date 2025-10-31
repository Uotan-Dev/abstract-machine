include $(AM_HOME)/scripts/isa/riscv.mk
include $(AM_HOME)/scripts/platform/uemu.mk
CFLAGS  += -DISA_H=\"riscv/riscv.h\"

AM_SRCS += riscv/uemu/start.S \
           riscv/uemu/cte.c \
           riscv/uemu/trap.S \
           riscv/uemu/vme.c
