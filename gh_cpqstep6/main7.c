#include "hal.h"

static volatile uint32_t s_ticks;
void SysTick_Handler(void) {s_ticks++;}

int main(void) {
uint16_t led = PIN('B', 7);                 // Blue LED
gpio_output(led);       // Set blue LED to output mode
  //uart_init(UART_DEBUG, 115200);              // Initialise UART
volatile uint32_t timer = 0, period = 500;  // Declare timers
for (;;) {
if (timer_expired(&timer, period, s_ticks)) {
static bool on;       // This block is executed
gpio_write(led, on);  // Every `period` milliseconds
on = !on;             // Toggle LED state
     // printf("LED: %d, tick: %lu\r\n", on, s_ticks);  // Write message
}//if
    // Here we could perform other activities!
}//for loop
return 0;}
