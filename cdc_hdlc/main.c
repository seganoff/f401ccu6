/* usbipd list (--help)
 *   bind --busid x-y (admin mode)
 *   attach --wsl --busid x-y (detach)
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>


#include "usbcdc.h"

static int encode_frame(const uint8_t *data, uint8_t data_len/*,uint8_t seqNr*/,uint8_t *frame_out) {
uint8_t byte;//data
int cout = 0;//deque or vector
frame_out[cout]=0x7e;cout++;//boundry octet
while (data_len) {
byte = *data++;
//fcs = _crc_ccitt_update(fcs, data);
if ((byte == 0x7d) || (byte == 0x7e)) { frame_out[cout]=0x7d;cout++;byte^=0x20;}
frame_out[cout]=byte;cout++;
data_len--;
}//while
frame_out[cout]=0x7e;cout++;
return cout;
}

//static SemaphoreHandle_t sem_flash = 0;

static uint32_t crc32(const uint8_t *data,size_t len) {
uint32_t crc=0xFFFFFFFF;	
for(size_t i=0;i<len;i++) {
uint8_t ch=data[i];
for(size_t j=0;j<8;j++) 
{uint32_t b=(ch^crc)&1;crc>>=1;if(b) crc=crc^0xEDB88320;ch>>=1;}
}//outer for loop
return ~crc;}


static void counter(void *arg __attribute__((unused))) {
//int userChar = usb_getc();//user connect
uint8_t data0[] = {'a','b','c','d','e','f'};
uint8_t out[8];
uint32_t frame_length = encode_frame(data0,6,out);

while(1)
{
usb_write(out,8);
//for(int i=0;i<frame_length; i++){
  //xQueueSend(usb_txq,buf,portMAX_DELAY);
  //usb_putc(out[i]);
  //vTaskDelay(pdMS_TO_TICKS(400));
//}
//ch_= (ch_ + 1) % 140;
vTaskDelay(pdMS_TO_TICKS(400));
//usb_puts("new input \n");//userChar = usb_getc();
}
}

static void flasher(void *arg __attribute__((unused))) {
while(1)//for (;;) 
{
//gpio_set(GPIOC,GPIO13);
//vTaskDelay(pdMS_TO_TICKS(2000));
gpio_toggle(GPIOC,GPIO13);
vTaskDelay(pdMS_TO_TICKS(400));
}
}

//int main(void) __attribute__ ((noreturn)); > warning, implicit main return
int main(void){
/*
lib/stm32/f4/rcc.c
const struct rcc_clock_scale rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_END] = {
	{ // 84MHz
		.pllm = 25,
		.plln = 336,
		.pllp = 4,
		.pllq = 7,
		.pllr = 0,
		.pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
		.hpre = RCC_CFGR_HPRE_NODIV,
		.ppre1 = RCC_CFGR_PPRE_DIV2,
		.ppre2 = RCC_CFGR_PPRE_NODIV,
		.voltage_scale = PWR_SCALE1,
		.flash_config = FLASH_ACR_DCEN | FLASH_ACR_ICEN |
				FLASH_ACR_LATENCY_2WS,
		.ahb_frequency  = 84000000,
		.apb1_frequency = 42000000,
		.apb2_frequency = 84000000,
	},
*/
rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

rcc_periph_clock_enable(RCC_GPIOC);
gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

//gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
//gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO14);
//gpio_clear(GPIOC,GPIO14);

//usbcdc.c:413 xTaskCreate(usb_task,"USB",200,udev,configMAX_PRIORITIES-1,NULL);
usb_start();

//xTaskCreate(adventure,"game",300,NULL,configMAX_PRIORITIES-1,NULL);
xTaskCreate(flasher,"flash",  100,NULL,configMAX_PRIORITIES-1,NULL);
xTaskCreate(counter,"counter",100,NULL,configMAX_PRIORITIES-1,NULL);

vTaskStartScheduler();
for (;;);
//return 0;//noreturn
}

// End main.c
