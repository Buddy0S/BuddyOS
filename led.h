#pragma once

#define __GPIO1_start__ 0x4804C000
#define SET_LED_OFFSET 0x194
#define RESET_LED_OFFSET 0x190
#define IO_MODE_OFFSET 0x134

#define GLBL_GPIO_CLOCK 0x44E00000

#define SET_USERLED1 __GPIO1_start__ + SET_LED_OFFSET
#define RESET_USERLED1 __GPIO1_start__ + RESET_LED_OFFSET

void initLED(void);
void LEDon(int led);
void LEDoff(int led);
