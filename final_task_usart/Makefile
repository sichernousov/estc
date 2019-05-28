BUILD_ROOT=../../..
OBJS = periph.o MT_USART.o action.o cmd.o main.o
BINARY_NAME = main

include $(BUILD_ROOT)/common_defs.mk

.PHONY: main

$(BINARY_NAME): $(BINARY_NAME).bin

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map *.d

flash: $(BINARY_NAME)
	$(FLASH_BINARY)

