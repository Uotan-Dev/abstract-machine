AM_SRCS := platform/uemu/trm.c \
           platform/uemu/ioe/ioe.c \
           platform/uemu/ioe/timer.c \
           platform/uemu/ioe/input.c \
           platform/uemu/ioe/gpu.c \
           platform/uemu/ioe/audio.c \
           platform/uemu/ioe/disk.c \
           platform/uemu/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
CFLAGS    += -I$(AM_HOME)/am/src/platform/uemu/include
LDSCRIPTS += $(AM_HOME)/scripts/linker.ld
LDFLAGS   += --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start
UEMUFLAGS += -l $(shell dirname $(IMAGE).elf)/uemu-log.txt

MAINARGS_MAX_LEN = 64
MAINARGS_PLACEHOLDER = the_insert-arg_rule_in_Makefile_will_insert_mainargs_here
CFLAGS += -DMAINARGS_MAX_LEN=$(MAINARGS_MAX_LEN) -DMAINARGS_PLACEHOLDER=$(MAINARGS_PLACEHOLDER)

insert-arg: image
	@python $(AM_HOME)/tools/insert-arg.py $(IMAGE).bin $(MAINARGS_MAX_LEN) $(MAINARGS_PLACEHOLDER) "$(mainargs)"

image: image-dep
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

run: insert-arg
	uemu $(IMAGE).bin

gdb: insert-arg
	uemu --gdb $(IMAGE).bin

.PHONY: insert-arg
