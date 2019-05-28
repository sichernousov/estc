BUILD_ROOT=../../..
OBJS = periph.o MT_USART.o action.o main.o

include $(BUILD_ROOT)/common_defs.mk

.PHONY: main uart_lib/MT_USART periph_lib/periph action_lib/action

main: main.bin

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map *.d

flash: $(BINARY_NAME)
	$(FLASH_BINARY)

