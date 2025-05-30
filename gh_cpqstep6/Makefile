CFLAGS  ?=  -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
            -Wformat-truncation -fno-common -Wconversion \
            -g3 -Os -ffunction-sections -fdata-sections \
-I. -Iinclude -Icmsis_core/CMSIS/Core/Include -Icmsis_f7/Include \
            -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16 $(EXTRA_CFLAGS) 
# rename link.ld into f767_linker pfv5-sp-d16 single precision...
LDFLAGS ?= -Tlink.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map
#SOURCES  = main6.c syscalls.c sysinit.c #step6
SOURCES  = main7.c syscalls.c sysinit.c mongoose.c net.c packed_fs.c
SOURCES += cmsis_f7/Source/Templates/gcc/startup_stm32f767xx.s

#.PHONY: linker
#linker:
#	arm-none-eabi-gcc -E -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
#	-D_ROM=256K -D_RAM=64K -D_ROM_OFF=0x08000000 -D_RAM_OFF=0x20000000 \
#	-P -E ../linker.ld.S -o tstlinker

RM = rm -f

build: firmware.bin
firmware.elf: cmsis_core cmsis_f7 hal.h link.ld Makefile $(SOURCES) 
	arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $@
firmware.bin: firmware.elf
	arm-none-eabi-objcopy -O binary $< $@
flash: firmware.bin
	dfu-util -a 0 -D $< -s 0x08000000
#	dfu-util -a 0 -D $(BINARY).bin -s 0x08000000 #mine
#	st-flash --reset write $< 0x8000000 #cpq
clean:
	$(RM) firmware.*
#.PHONY: images clean elf bin hex srec list all dfuflash stflash rebuild build
