/* usbcdcdemo.c : USBCDC Example (adventure)
 * Warren Gay
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

//#include "common.h"
#include "usbcdc.h"

//static SemaphoreHandle_t sem_flash = 0;

static void flasher(void *arg __attribute__((unused))) {
while(1)//for (;;) 
{
gpio_toggle(GPIOC,GPIO13);
vTaskDelay(pdMS_TO_TICKS(500));	
}
}

int main(void) {

rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

rcc_periph_clock_enable(RCC_GPIOC);
gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO14);
gpio_clear(GPIOC,GPIO14);

usb_start();

//xTaskCreate(adventure,"game",300,NULL,configMAX_PRIORITIES-1,NULL);
xTaskCreate(flasher,"flash",100,NULL,configMAX_PRIORITIES-1,NULL);
//xTaskCreate(flasher14,"flash14",100,NULL,configMAX_PRIORITIES-1,NULL);

vTaskStartScheduler();
for (;;);
return 0;
}

// End main.c
