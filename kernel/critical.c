#include "critical.h"

volatile uint32_t g_critical_nesting = 0;
volatile uint32_t g_isr_mask_state = 0;
