BUILD_ROOT=../../..
OBJS = main.o

BINARY_NAME = main

INCLUDE_DIRS += -I$(BUILD_ROOT)/projects/estc/final_task_usart/action_lib
INCLUDE_DIRS += -I$(BUILD_ROOT)/projects/estc/final_task_usart/periph_lib
INCLUDE_DIRS += -I$(BUILD_ROOT)/projects/estc/final_task_usart/queue

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

$(BINARY_NAME): $(BINARY_NAME).bin 

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map

flash: $(BINARY_NAME)
	$(FLASH_BINARY)

