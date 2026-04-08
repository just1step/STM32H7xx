#include <stdint.h>

volatile uint32_t _bootflag __attribute__((section(".dtcmdata")));
