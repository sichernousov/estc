BUILD_ROOT=../../..
OBJS = main.o

BINARY_NAME = main
INCLUDE_DIRS += -I$(BUILD_ROOT)/projects/estc/lab5_lib/include
LIB_DIRS  += $(BUILD_ROOT)/projects/estc/lab5_lib
LDLIBS += -lled

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

$(BINARY_NAME): $(BINARY_NAME).bin

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map

flash: $(BINARY_NAME)
	$(FLASH_BINARY)
