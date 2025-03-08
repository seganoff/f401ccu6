
#pragma once //incl protection instead > oldschool style

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "stm32f767xx.h"//"stm32f429xx.h"

#define FREQ 16000000  // CPU frequency, 16 Mhz
#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}

#define GPIO(bank) ((GPIO_TypeDef *) (GPIOA_BASE + 0x400U * (bank)))
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
  GPIO_TypeDef *gpio = GPIO(PINBANK(pin));  // GPIO bank
  int n = PINNO(pin);                       // Pin number
  RCC->AHB1ENR |= BIT(PINBANK(pin));        // Enable GPIO clock
  gpio->MODER &= ~(3U << (n * 2));          // Clear existing setting
  gpio->MODER |= (mode & 3U) << (n * 2);    // Set new mode
}

static inline void gpio_set_af(uint16_t pin, uint8_t af_num) {
  GPIO_TypeDef *gpio = GPIO(PINBANK(pin));  // GPIO bank
  int n = PINNO(pin);                       // Pin number
  gpio->AFR[n >> 3] &= ~(15UL << ((n & 7) * 4));
  gpio->AFR[n >> 3] |= ((uint32_t) af_num) << ((n & 7) * 4);
}

static inline void gpio_write(uint16_t pin, bool val) {
  GPIO_TypeDef *gpio = GPIO(PINBANK(pin));
  gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

static inline bool timer_expired(volatile uint32_t *t, uint32_t prd,
                                 uint32_t now) {
  if (now + prd < *t) *t = 0;                    // Time wrapped? Reset timer
  if (*t == 0) *t = now + prd;                   // Firt poll? Set expiration
  if (*t > now) return false;                    // Not expired yet, return
  *t = (now - *t) > prd ? now + prd : *t + prd;  // Next expiration time
  return true;                                   // Expired, return true
}
