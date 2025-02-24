# make -f <thisFile> (TAB-autocompl) generated.stm32f401ccu6.ld
# LDSCRIPT	=      generated.$(DEVICE).ld //genlink-config.mk

#arm-none-eabi-gcc -E -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F4 -DSTM32F401CCU6 -D_ROM=256K -D_RAM=64K -D_ROM_OFF=0x08000000 -D_RAM_OFF=0x20000000 -P -E libopencm3/ld/linker.ld.S -o generated.stm32f401ccu6.ld

#arm-none-eabi-gcc -E -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DSTM32F7 -DSTM32F7CCM -DSTM32F767ZIT6 -D_ROM=2048K -D_RAM=384K -D_CCM=128K -D_CCM_OFF=0x20000000 -D_ROM_OFF=0x08000000 -D_RAM_OFF=0x20020000 -P -E libopencm3/ld/linker.ld.S -o generated.stm32f767zit6.ld

#DEVICE          = stm32f401ccu6
DEVICE          = stm32f767zit6
OPENCM3_DIR     = ./libopencm3
#OBJS            += foo.o

CFLAGS          += -Os -ggdb3
CPPFLAGS	+= -MD
LDFLAGS         += -static -nostartfiles
LDLIBS          += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

.PHONY: clean all

all: binary.elf binary.hex

clean:
	$(Q)$(RM) -rf binary.* *.o

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

#usage: place above OPENCM3_DIR,(+ optional make TARGETS='' in libopencm_DIR) gen lyka first line, get this line
# arm-none-eabi-gcc -E -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DSTM32F7 -DSTM32F7CCM -DSTM32F767ZIT6 -D_ROM=2048K -D_RAM=384K -D_CCM=128K -D_CCM_OFF=0x20000000 -D_ROM_OFF=0x08000000 -D_RAM_OFF=0x20020000 -P -E linker.ld.S -o f767_linker
