
#rm *.h *.c ; sh fillFolder.sh && ll && cat opencm3.c
# 01-Beginners-guide/03-Build-your-first-project#optional-source-files
# clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git $sysroot
dest='./' #arg
sysroot='/home/u262d/githubs/stm/FreeRTOS-Kernel/' #arg
compiler='GCC' #arg
mcu='ARM_CM4F' #arg
ports="${sysroot}portable/${compiler}/${mcu}/"
mem_mang='portable/MemMang/' # no arg, just cp all c files
incl='include/'

cp $sysroot*.c $dest
cp $sysroot$mem_mang*.c $dest
cp $sysroot$incl*.h $dest
cp $ports* $dest

opencm3="
#include \"FreeRTOS.h\"
#include \"task.h\"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

extern void vPortSVCHandler( void ) __attribute__ (( naked ));
extern void xPortPendSVHandler( void ) __attribute__ (( naked ));
extern void xPortSysTickHandler( void );

void sv_call_handler(void) {
        vPortSVCHandler();
}

void pend_sv_handler(void) {
        xPortPendSVHandler();
}

void sys_tick_handler(void) {
        xPortSysTickHandler();
}

/* end opncm3.c */"
echo -n "${opencm3}" > opencm3.c
#tee ./opencm3.c <<<$opencm3
