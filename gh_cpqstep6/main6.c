#include "hal.h"

static volatile uint32_t s_ticks;
void SysTick_Handler(void) {s_ticks++;}

int main(void) {
//uint16_t ld1 = PIN('B',0x00); // PB0  green || pa5 (sb119+120)
uint16_t ld2 = PIN('B',0x07); // PB7  blue
uint16_t ld3 = PIN('B',0x0e); // PB14 red
//clock_init();
gpio_output(ld2);       // Set blue LED to output mode
gpio_output(ld3);
  //uart_init(UART_DEBUG, 115200);              // Initialise UART
volatile uint32_t timer_blue = 0, period_blue = 500;  // declareTimers
volatile uint32_t timer_red = 0, period_red = 1000;
for (;;) {
if (timer_expired(&timer_blue, period_blue, s_ticks)) {
static bool on;       // This block is executed
gpio_write(ld2, on);  // Every `period` milliseconds
//gpio_write(ld3, on);
on = !on;             // Toggle LED state
     // printf("LED: %d, tick: %lu\r\n", on, s_ticks);  // Write message
}//timer_blue

if (timer_expired(&timer_red, period_red, s_ticks)) {
static bool on; gpio_write(ld3,on);on=!on;}

// Here we could perform other activities!
}//for loop
return 0;}
